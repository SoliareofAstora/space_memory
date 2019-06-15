import numpy as np
import space_enviro.spaceLib as environment
print("Loaded")


env = environment.Environment("haba")
print('Initialized')

arr = np.array([1,2,3,4], np.float32)
tmp = env.step(arr)
print("Step output ", tmp)
exit(0)
import numpy as np

import math
import random
import numpy as np
# import matplotlib
# import matplotlib.pyplot as plt
from collections import namedtuple
from itertools import count

import torch
import torch.nn as nn
import torch.optim as optim
import torch.nn.functional as F
import torch.distributions.multinomial as multi

side_space = 7
main_space = 6
side_power = np.linspace(-1, 1, side_space)
main_power = np.linspace(-0.25, 1, main_space)


class Model(nn.Module):
    def __init__(self):
        super(Model, self).__init__()
        self.conv1 = nn.Conv1d(8, 16, kernel_size=5, stride=2, padding=2)
        self.bn1 = nn.BatchNorm1d(16)
        self.conv2 = nn.Conv1d(16, 32, kernel_size=5, stride=2, padding=2)
        self.bn2 = nn.BatchNorm1d(32)
        self.conv3 = nn.Conv1d(32, 32, kernel_size=5, stride=2, padding=2)
        self.bn3 = nn.BatchNorm1d(32)

        self.fc00 = nn.Linear(32 * 16, 128)
        self.fc01 = nn.Linear(128, 64)
        self.fc1 = nn.Linear(20, 64)
        self.fc20 = nn.Linear(128, 128)
        self.fc21 = nn.Linear(128, 128)
        self.fc22 = nn.Linear(128, 32)

        self.fcside = nn.Linear(32, side_space)
        self.fcmain = nn.Linear(32, main_space)

    def forward(self, view, stats):
        x = F.relu6(self.bn1(self.conv1(view)))
        x = F.relu6(self.bn2(self.conv2(x)))
        x = F.relu6(self.bn3(self.conv3(x)))

        x = torch.tanh(self.fc00(x.view(x.size(0), -1)))
        x = torch.tanh(self.fc01(x))
        y = torch.tanh(self.fc1(stats))
        x = torch.cat((x, y), dim=1)

        x = torch.tanh(self.fc20(x))
        x = torch.tanh(self.fc21(x))
        x = torch.tanh(self.fc22(x))
        return self.fcside(x), self.fcmain(x)


transition = namedtuple('Transition', ('state', 'stats', 'actions', 'next_state', 'next_stats', 'reward'))


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

    def __len__(self):
        return len(self.memory)


device = torch.device("cuda" if torch.cuda.is_available() else "cpu")
BATCH_SIZE = 1024
GAMMA = 0.9
EPS_START = 0.9
EPS_END = 0.15
EPS_DECAY = 1500

policy_net = Model().to(device)
target_net = Model().to(device)

target_net.load_state_dict(policy_net.state_dict())
target_net.eval()

optimizer = optim.RMSprop(policy_net.parameters())
memory = ReplayMemory(1000000)

steps_done = 0



def get_actions(state):
    with torch.no_grad():
        output = policy_net(state[0], state[1])
        q_values_0 = F.softmax(output[0], dim=1)
        q_values_1 = F.softmax(output[1], dim=1)
    sample = random.random()
    eps_threshold = EPS_END + (EPS_START - EPS_END) * \
                    math.exp(-1. * steps_done / EPS_DECAY)

    if sample > eps_threshold:
        return torch.tensor([q_values_0.max(1)[1], q_values_1.max(1)[1]])
    else:
        return torch.tensor([q_values_0.multinomial(1)[0], q_values_1.multinomial(1)[0]])


def get_state(environment):
    return (
        torch.tensor(environment.get_observations(), device=device),
        torch.tensor(environment.get_stats(), device=device)
    )


def optimize_model():
    if len(memory) < BATCH_SIZE:
        return
    transitions = memory.sample(BATCH_SIZE)
    batch = transition(*zip(*transitions))

    state_batch = torch.cat(batch.state)
    stats_batch = torch.cat(batch.stats)
    actions_batch = torch.cat(batch.actions)
    reward_batch = torch.cat(batch.reward)
    next_state_batch = torch.cat(batch.next_state)
    next_stats_batch = torch.cat(batch.next_stats)

    # print(actions_batch.view(BATCH_SIZE,-1)[:,1])

    side_engine, main_engine = policy_net(state_batch, stats_batch)
    # print(side_engine)
    side_action_values = side_engine.gather(1, actions_batch.view(BATCH_SIZE, -1)[:, 0].unsqueeze(-1))
    main_action_values = main_engine.gather(1, actions_batch.view(BATCH_SIZE, -1)[:, 1].unsqueeze(-1))

    next_state_sidevalues, next_state_mainvalues = target_net(next_state_batch, next_stats_batch)

    excepted_state_action_value = ((next_state_sidevalues.max(1)[0] + next_state_mainvalues.max(1)[0]) * GAMMA) + reward_batch

    loss = F.smooth_l1_loss(side_action_values + main_action_values,
                            excepted_state_action_value.unsqueeze(1).detach())

    optimizer.zero_grad()
    loss.backward()
    for param in policy_net.parameters():
        param.grad.data.clamp_(-1, 1)
    optimizer.step()


env = environment.initialize("haba")
episodes_duration = []
num_episodes = 99999

for i_episodes in range(num_episodes):
    rewards = []
    new_state = get_state(env)
    episode_memory = []
    state = [None, None]
    state[0] = torch.cat([new_state[0],new_state[0],new_state[0],new_state[0]], dim=1)
    state[1] = torch.cat([new_state[1],new_state[1],new_state[1],new_state[1]], dim=1)

    for t in count():
        actions = get_actions(state)
        done = env.update(np.array([side_power[actions[0]], main_power[actions[1]]],dtype=np.float32))
        new_state = get_state(env)
        next_state = [torch.cat([new_state[0],state[0][0][:-2].unsqueeze(0)], dim=1),torch.cat([new_state[1],state[1][0][0:-5].unsqueeze(0)], dim=1)]

        if done == 2:
            print("Exit")
            exit(0)
        if done == 1:
            done = True
            print("t:"+str(t)+" e:"+str(i_episodes))

        if not done:
            episode_memory.append(transition(state[0], state[1], actions.to(device), next_state[0], next_state[1], 0))

        state = next_state
        optimize_model()

        # ('state', 'stats', 'actions', 'next_state', 'next_stats', 'reward'))

        if done:
            episodes_duration.append(t + 1)
            for i, tr in enumerate(episode_memory):
                reward_scale = max(0., min(5 * math.exp(-1. * (t - 10 - i) / 50), 4) - 2)
                memory.push(tr.state, tr.stats, tr.actions, tr.next_state, tr.next_stats,
                            torch.tensor([1. - reward_scale], device=device))

            target_net.load_state_dict(policy_net.state_dict())

            break
