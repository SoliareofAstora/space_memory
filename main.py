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

size = 500
class Model(nn.Module):
    def __init__(self):
        super(Model, self).__init__()

        self.fc0 = nn.Linear(5, size)
        self.fc1 = nn.Linear(size, size)
        self.fc2 = nn.Linear(size, size)
        # self.fc3 = nn.Linear(size, size)
        # self.fc4 = nn.Linear(size, size)

        self.bn1 = nn.BatchNorm1d(num_features=size)
        self.bn2 = nn.BatchNorm1d(num_features=size)
        # self.bn3 = nn.BatchNorm1d(num_features=size)
        # self.bn4 = nn.BatchNorm1d(num_features=size)

        self.fcmain = nn.Linear(size, main_space)
        self.fcside = nn.Linear(size, side_space)

    def forward(self, x):
        x = torch.tanh(self.fc0(x))
        x = self.bn1(torch.relu(self.fc1(x)))
        x = self.bn2(torch.relu(self.fc2(x)))
        # x = self.bn3(torch.relu(self.fc3(x)))
        # x = self.bn4(torch.tanh(self.fc4(x)))
        return self.fcmain(x), self.fcside(x)


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
    exit("No cuda")

device = torch.device("cuda")

BATCH_SIZE = 50
GAMMA = 0.8
EPS_START = 0.999
EPS_END = 0.15
EPS_DECAY = 1000

policy_net = Model().to(device)
target_net = Model().to(device)

target_net.load_state_dict(policy_net.state_dict())
target_net.eval()

optimizer = optim.RMSprop(policy_net.parameters(), 0.001)
memory = ReplayMemory(1000)

steps_done = 0

def get_actions(state):
    with torch.no_grad():
        output = policy_net(state)
        q_values_0 = F.softmax(output[0], dim=1)
        q_values_1 = F.softmax(output[1], dim=1)
    sample = random.random()
    eps_threshold = EPS_END + (EPS_START - EPS_END) * \
                    math.exp(-1. * steps_done / EPS_DECAY)

    if sample > eps_threshold:
        return torch.cat([q_values_0.max(1)[1].unsqueeze(1), q_values_1.max(1)[1].unsqueeze(1)],1)
    else:
        # return torch.cat([q_values_0.multinomial(1), q_values_1.multinomial(1)],1)
        return torch.cat([torch.randint(main_space, (q_values_0.shape[0],1)).to(device), torch.randint(side_space, (q_values_1.shape[0],1)).to(device)],1)


def optimize_model():
    if len(memory) < BATCH_SIZE:
        return
    transitions = memory.sample(BATCH_SIZE)
    batch = transition(*zip(*transitions))

    reward_batch = torch.cat(batch.reward)
    state_batch = torch.cat(batch.state)
    actions_batch = torch.cat(batch.actions)
    next_state_batch = torch.cat(batch.next_state)

    main_engine, side_engine= policy_net(state_batch)
    main_action_values = main_engine.gather(1, actions_batch[:, 0].unsqueeze(-1))
    side_action_values = side_engine.gather(1, actions_batch[:, 1].unsqueeze(-1))

    next_state_main_values, next_state_side_values = target_net(next_state_batch)

    excepted_state_action_value = ((next_state_main_values.max(1)[0] + next_state_side_values.max(1)[0]) * GAMMA) + reward_batch

    loss = F.smooth_l1_loss(main_action_values + side_action_values,
                            excepted_state_action_value.unsqueeze(1).detach())

    optimizer.zero_grad()
    loss.backward()
    for param in policy_net.parameters():
        param.grad.data.clamp_(-1, 1)
    optimizer.step()


env = environment.initialize("haba")
num_episodes = 99999


for i_episodes in range(num_episodes):
    state = env.reset()
    state = torch.Tensor(state).to(device)

    for t in count():
        actions = get_actions(state)
        #TODO check whats faster. actions[].cpu or action.cpu()[]
        actions_real = np.array((main_power[actions[:,0].cpu().tolist()], side_power[actions[:,1].cpu().tolist()]),dtype=np.float32)
        new_state, reward = env.step(actions_real)
        # reward -= 0.01*new_state[:,4]**2
        new_state = torch.Tensor(new_state).to(device)
        if env.done():
            exit(0)
        positive_reward = len(list(filter(lambda x: x>0, reward)))
        print(t, int(np.sum(reward)), positive_reward)
        memory.push(state, actions, new_state, torch.Tensor(reward).to(device))

        # if positive_reward < 5:
        #     # state = env.reset()
        #     # state = torch.Tensor(state).to(device)
        #     exit("This universe is hopeless. Lets create another one :)")


        state = new_state
        # for x in range(5):
        optimize_model()

        if t % 100 == 0:
            target_net.load_state_dict(policy_net.state_dict())
            memory.erase()
