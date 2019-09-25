from rl_utils import environment

import math
import random
import numpy as np
import namedlist
from collections import deque
from itertools import count


import torch
import torch.nn as nn
import torch.optim as optim
import torch.nn.functional as F

action_space = 2
# action_space = main_space * side_space

size = 100
class Model(nn.Module):
    def __init__(self):
        super(Model, self).__init__()

        self.fc0 = nn.Linear(4, size, False)
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

Step = namedlist.namedlist('Step', ('state', 'action', 'reward', 'done'))

if not torch.cuda.is_available():
    exit("No cuda available ")


device = torch.device("cuda")

SIMULATION_SIZE = 1
GAMMA = 0.5
N_STEP = 10
MEMORY_SIZE = 100000
TARGET_UPDATE = 10

EPS_START = 0.9
EPS_END = 0.1
EPS_DECAY = 100

env = environment.get_env('CartPole-v1', SIMULATION_SIZE)
num_episodes = 99999

policy_net = Model().to(device)
target_net = Model().to(device)

target_net.load_state_dict(policy_net.state_dict())
target_net.eval()

loss_function = nn.MSELoss()
# loss_function = F.smooth_l1_loss
optimizer = optim.RMSprop(policy_net.parameters())

steps_done = 0

class ReplayQueue(object):
    def __init__(self, capacity):
        self.memory = deque()
        self.capacity = capacity

    def push(self, *args):
        self.memory.append(Step(*args))
        if len(self.memory) > self.capacity:
            self.memory.popleft()

    def sample(self):
        tmp_memory = list(self.memory)
        point = random.randint(N_STEP,len(self.memory))
        return tmp_memory[point - N_STEP: point]

    def erase(self):
        self.memory.clear()

    def __len__(self):
        return len(self.memory)

memory = ReplayQueue(10000)



def get_actions(state):
    with torch.no_grad():
        output = policy_net(state)
        q_values = F.softmax(output, dim=0)

    sample = random.random()
    eps_threshold = EPS_END + (EPS_START - EPS_END) * \
                    math.exp(-1. * steps_done / EPS_DECAY)

    if sample > eps_threshold:
        return q_values.max(1)[1]
    else:
        # return q_values.multinomial(1).squeeze()
        return torch.randint(action_space, [q_values.shape[0]]).to(device)


# Step = namedtuple('Step', ('state', 'action', 'reward', 'done'))
def optimize_model(t):
    if len(memory) < N_STEP:
        return
    steps = memory.sample()

    steps[-1].reward = target_net(steps[-1].state).max(1)[0].detach()
    steps[-1].reward[steps[-1].done] = 0

    for i in range(len(steps)-2,-1,-1):
        steps[i].reward += GAMMA * steps[i+1].reward
        steps[i].reward[steps[i].done] = 0.

    batch = Step(*zip(*steps))
    state_batch = torch.cat(batch.state)
    reward_batch = torch.cat(batch.reward)
    action_batch = torch.cat(batch.action)

    action = policy_net(state_batch)
    action_values = action.gather(1, action_batch.unsqueeze(-1))

    loss = loss_function(action_values.squeeze(), reward_batch)
    loss.backward()

    for param in policy_net.parameters():
        param.grad.data.clamp_(-1, 1)

    optimizer.zero_grad()
    optimizer.step()

    if (t + 1) % TARGET_UPDATE == 0:
        target_net.load_state_dict(policy_net.state_dict())

lifetime = np.zeros(SIMULATION_SIZE)
t=0

for i_episodes in range(num_episodes):
    state = env.reset()
    state = torch.Tensor(state).to(device)
    for t in count():
        env.render()
        actions = get_actions(state)
        new_state, reward, done = env.step(actions.cpu().numpy())

        memory.push(state, actions, torch.Tensor(reward).to(device), done)
        new_state = torch.Tensor(new_state).to(device)

        lifetime += 1
        lifetime[done] = 0

        print(t, int(np.average(lifetime)))
        if t>2000:
            env.close()
            exit(0)
        prev_rewards = reward
        state = new_state
        optimize_model(t)
