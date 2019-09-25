import gym
import math
import numpy as np
import namedlist
from itertools import count

import torch
import torch.nn as nn
import torch.optim as optim
import torch.nn.functional as F

from rl_utils import action_selection, replay_storage


action_space = 2
# action_space = main_space * side_space

size = 150
class Model(nn.Module):
    def __init__(self):
        super(Model, self).__init__()

        self.fc0 = nn.Linear(4, size, False)
        self.fc1 = nn.Linear(size, size, False)
        # self.fc2 = nn.Linear(size, size, False)
        # self.fc3 = nn.Linear(size, size, False)
        # self.fc4 = nn.Linear(size, size, False)

        self.fcoutput = nn.Linear(size, action_space, False)

    def forward(self, x):
        x = torch.relu(self.fc0(x))
        x = torch.relu(self.fc1(x))
        # x = torch.relu(self.fc2(x))
        # x = torch.relu(self.fc3(x))
        # x = torch.relu(self.fc4(x))

        return self.fcoutput(x)

Step = namedlist.namedlist('Step', ('state', 'action', 'reward', 'done'))

if not torch.cuda.is_available():
    exit("No cuda available ")


device = torch.device("cuda")

SIMULATION_SIZE = 1
GAMMA = 0.9
MEMORY_SIZE = 1000
BATCH_SIZE = 100

EPS_START = 0.8
EPS_END = 0.01
EPS_DECAY = 1000
get_actions = action_selection.EpsGreedy(action_space, EPS_START, EPS_END, EPS_DECAY)


env = gym.make('CartPole-v0')

num_episodes = 99999

policy_net = Model().to(device)

# loss_function = nn.MSELoss()
loss_function = F.smooth_l1_loss
optimizer = optim.RMSprop(policy_net.parameters())
# optimizer = optim.Adam(policy_net.parameters(), 0.01)


run_memory = replay_storage.ReplayQueue(Step,500)
memory = replay_storage.ReplayList(Step,1000)

steps_done = 0

def compute_nstep(step):
    step[-1].reward = policy_net(step[-1].state.unsqueeze(0)).max(1)[0].detach()
    step[-1].reward[step[-1].done] = -5.
    for i in range(len(step)-2,-1,-1):
        step[i].reward += GAMMA * step[i+1].reward
    return step

priority = []


def optimize_model(t):
    if len(memory) < BATCH_SIZE:
        return
    steps = memory.sample(BATCH_SIZE)

    batch = Step(*zip(*steps))
    state_batch = torch.cat(batch.state).view([-1,4])
    reward_batch = torch.cat(batch.reward)
    action_batch = torch.cat(batch.action)

    action = policy_net(state_batch)
    action_values = action.gather(1, action_batch.unsqueeze(-1))
    loss = loss_function(action_values.squeeze(), reward_batch)

    loss = torch.sum(loss)
    optimizer.zero_grad()

    loss.backward()
    for param in policy_net.parameters():
        param.grad.data.clamp_(-1, 1)
    optimizer.step()

lifetime = np.zeros(SIMULATION_SIZE)
t=0
j = 0

state = env.reset()
state = torch.Tensor(state).to(device)


for t in count():
    env.render()

    actions = get_actions(policy_net, state.unsqueeze(0))
    new_state, reward, done, _ = env.step(actions.cpu().numpy()[0])
    lifetime += 1
    run_memory.append(state, actions.to(device), torch.Tensor([reward]).to(device), done)
    if done or lifetime > 450:
        j+=1
        memory.extend(compute_nstep(list(run_memory)))
        run_memory.erase()

        for i in range(10):
            optimize_model(t)

        print(j, int(np.average(lifetime)))
        lifetime = 0
        new_state = env.reset()

    for i in range(4):
        optimize_model(t)
    new_state = torch.Tensor(new_state).to(device)
    state = new_state
    steps_done +=1

    if j>500:
        env.close()
        break

