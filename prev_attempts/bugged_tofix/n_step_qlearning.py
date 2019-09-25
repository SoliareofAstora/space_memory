import space_enviro.spaceLib as environment

import math
import random
import numpy as np
import namedlist
from collections import  deque
from itertools import count

# env = environment.initialize("haba")
# state = env.reset()
# print(state)

import torch
import torch.nn as nn
import torch.optim as optim
import torch.nn.functional as F

main_space = 2
side_space = 3
main_power = np.linspace(0, 1, main_space)
side_power = np.linspace(-1, 1, side_space)

action_matrix = np.array([[0,0],[1,0],[0,-1],[0,1]])
action_space = 4
# action_space = main_space * side_space

size = 150
class Model(nn.Module):
    def __init__(self):
        super(Model, self).__init__()

        self.fc0 = nn.Linear(5, size, False)
        self.fc1 = nn.Linear(size, size, False)
        self.fc2 = nn.Linear(size, size, False)
        self.fc3 = nn.Linear(size, size, False)
        self.fc4 = nn.Linear(size, size)
        self.fcoutput = nn.Linear(size, action_space, False)

        self.bn1 = nn.BatchNorm1d(num_features=size)
        self.bn2 = nn.BatchNorm1d(num_features=size)
        self.bn3 = nn.BatchNorm1d(num_features=size)
        self.bn4 = nn.BatchNorm1d(num_features=size)

    def forward(self, x):
        x = torch.relu(self.fc0(x))
        x = self.bn1(torch.relu(self.fc1(x)))
        x = self.bn2(torch.relu(self.fc2(x)))
        x = self.bn3(torch.relu(self.fc3(x)))
        x = self.bn4(torch.tanh(self.fc4(x)))
        return self.fcoutput(x)


Step = namedlist.namedlist('Step', ('state', 'action', 'reward', 'done'))
if not torch.cuda.is_available():
    exit("No cuda available ")

device = torch.device("cuda")

GAMMA = 0.9
N_STEP = 100
MEMORY_SIZE = 100000
N_UPDATES = 10

EPS_START = 0.9
EPS_END = 0.01
EPS_DECAY = 1000

env = environment.initialize(250)
num_episodes = 99999

policy_net = Model().to(device)
target_net = Model().to(device)

target_net.load_state_dict(policy_net.state_dict())
target_net.eval()

loss_function = nn.MSELoss()
optimizer = optim.Adam(policy_net.parameters(), 0.001)

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


memory = ReplayQueue(MEMORY_SIZE)

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

    # for param in policy_net.parameters():
    #     param.grad.data.clamp_(-1, 1)

    if (t + 1) % N_UPDATES == 0:
        optimizer.step()
        optimizer.zero_grad()
        target_net.load_state_dict(policy_net.state_dict())


for i_episodes in range(num_episodes):
    state = env.reset()
    state = torch.Tensor(state).to(device)
    for t in count():
        actions = get_actions(state)
        actions_real = np.array((action_matrix[actions.cpu().tolist(),0], action_matrix[actions.cpu().tolist(),1]),dtype=np.float32)
        new_state, reward, done = env.step(actions_real)
        reward[reward<-1]=-1
        new_state[:, 0] = new_state[:, 0] / 2000

        # reward -= 0.1 * new_state[:, 4] ** 2
        memory.push(state, actions, torch.Tensor(reward).to(device), done)

        new_state = torch.Tensor(new_state).to(device)
        if not env.active():
            exit(0)
        positive_reward = list(filter(lambda x: x > 0, reward))
        print(t, int(np.sum(positive_reward)), len(positive_reward), int(np.sum(reward)))
        steps_done+=1
        prev_rewards = reward
        state = new_state
        optimize_model(t)