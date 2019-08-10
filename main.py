import space_enviro.spaceLib as environment

import math
import random
import numpy as np
# import matplotlib
# import matplotlib.pyplot as plt
from collections import namedtuple
from itertools import count

# env = environment.initialize("haba")
# state = env.reset()
# print(state)


import torch
import torch.nn as nn
import torch.optim as optim
import torch.nn.functional as F
import torch.distributions.multinomial as multi

main_space = 2
side_space = 3
main_power = np.linspace(0, 1, main_space)
side_power = np.linspace(-1, 1, side_space)

action_matrix = np.array([[0,0],[1,0],[0,-1],[0,1]])
action_space = 4
# action_space = main_space * side_space

size = 50
class Model(nn.Module):
    def __init__(self):
        super(Model, self).__init__()

        self.fc0 = nn.Linear(5, size, False)
        self.fc1 = nn.Linear(size, size, False)
        self.fc2 = nn.Linear(size, size, False)
        # self.fc3 = nn.Linear(size, size, False)
        # self.fc4 = nn.Linear(size, size)
        self.fcoutput = nn.Linear(size, action_space, False)

        self.bn1 = nn.BatchNorm1d(num_features=size)
        self.bn2 = nn.BatchNorm1d(num_features=size)
        # self.bn3 = nn.BatchNorm1d(num_features=size)
        # self.bn4 = nn.BatchNorm1d(num_features=size)



    def forward(self, x):
        x = torch.relu(self.fc0(x))
        x = self.bn1(torch.relu(self.fc1(x)))
        x = self.bn2(torch.relu(self.fc2(x)))
        # x = self.bn3(torch.relu(self.fc3(x)))
        # x = self.bn4(torch.tanh(self.fc4(x)))
        return self.fcoutput(x)


transition = namedtuple('Transition', ('state', 'actions', 'next_state', 'reward'))


class ReplayMemory(object):
    def __init__(self, capacity):
        self.capacity = capacity
        self.memory = []
        self.position = 0

    def push(self, *args):
        if len(self.memory) < self.capacity:
            self.memory.append(None)
        self.memory[self.position] = transition(*args)
        self.position = (self.position + 1) % self.capacity

    def sample(self, batch_size):
        return random.sample(self.memory, batch_size)

    def erase(self):
        self.memory = []
        self.position = 0

    def __len__(self):
        return len(self.memory)


if not torch.cuda.is_available():
    exit("No cuda available ")

device = torch.device("cuda")

BATCH_SIZE = 50
GAMMA = 0.999
EPS_START = 0.8

EPS_END = 0.01
EPS_DECAY = 10000

policy_net = Model().to(device)
target_net = Model().to(device)

target_net.load_state_dict(policy_net.state_dict())
target_net.eval()

loss_function = F.smooth_l1_loss
optimizer = optim.RMSprop(policy_net.parameters(), 0.01)
memory = ReplayMemory(1000)

steps_done = 0

def get_actions(state):
    with torch.no_grad():
        output = target_net(state)
        q_values = F.softmax(output, dim=0)

    sample = random.random()
    eps_threshold = EPS_END + (EPS_START - EPS_END) * \
                    math.exp(-1. * steps_done / EPS_DECAY)

    if sample > eps_threshold:
        return q_values.max(1)[1]
    else:
        # return q_values.multinomial(1).squeeze().detach()
        return torch.randint(action_space, [q_values.shape[0]]).to(device)


def optimize_model(i):
    if len(memory) < BATCH_SIZE:
        return
    transitions = memory.sample(BATCH_SIZE)
    batch = transition(*zip(*transitions))

    reward_batch = torch.cat(batch.reward)
    state_batch = torch.cat(batch.state)
    actions_batch = torch.cat(batch.actions)
    next_state_batch = torch.cat(batch.next_state)

    action = policy_net(state_batch)
    action_values = action.gather(1, actions_batch.unsqueeze(-1))

    next_state_value = target_net(next_state_batch)

    excepted_state_action_value = (next_state_value.max(1)[0] * GAMMA) + reward_batch

    loss = loss_function(action_values, excepted_state_action_value.unsqueeze(1).detach())
    loss.backward()

    for param in policy_net.parameters():
        param.grad.data.clamp_(-1, 1)
    optimizer.step()
    optimizer.zero_grad()

env = environment.initialize("haba")
num_episodes = 99999

for i_episodes in range(num_episodes):
    state = env.reset()
    state = torch.Tensor(state).to(device)
    for t in count():
        actions = get_actions(state)
        actions_real = np.array((action_matrix[actions.cpu().tolist(),0], action_matrix[actions.cpu().tolist(),1]),dtype=np.float32)
        # actions_real = np.array((main_power[(actions / side_space).cpu().tolist()], side_power[(actions % side_space).cpu().tolist()]),            dtype=np.float32)
        new_state, reward, done = env.step(actions_real)
        new_state[:, 0] = new_state[:, 0] / 3000
        new_state = torch.Tensor(new_state).to(device)
        if not env.active():
            exit(0)
        positive_reward = list(filter(lambda x: x>0, reward))
        print(t, int(np.sum(positive_reward)), len(positive_reward), int(np.sum(reward)))
        memory.push(state, actions, new_state, torch.Tensor(reward).to(device))


        state = new_state
        optimize_model(t)

        if (t+1) % 100 == 0:
            target_net.load_state_dict(policy_net.state_dict())
            # memory.erase()