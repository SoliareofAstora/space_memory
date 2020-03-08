import torch
import torch.nn as nn
import torch.nn.functional as F
import torch.optim as optim
import gym
import namedlist
from rl_utils import replay_storage, action_selection
from itertools import count
import math


env = gym.make('CartPole-v1')
action_space = env.action_space.n
state_shape = env.observation_space.shape[0]
hidden_size = 200


class Model(nn.Module):
    def __init__(self):
        super(Model, self).__init__()

        self.fc0 = nn.Linear(state_shape, hidden_size, False)
        self.fc1 = nn.Linear(hidden_size, hidden_size, False)
        self.fc2 = nn.Linear(hidden_size, hidden_size, False)
        self.fc3 = nn.Linear(hidden_size, hidden_size, False)
        self.fc4 = nn.Linear(hidden_size, hidden_size, False)

        self.fcoutput = nn.Linear(hidden_size, action_space, False)

    def forward(self, x):
        x = torch.tanh(self.fc0(x))
        x = torch.tanh(self.fc1(x))
        x = torch.relu(self.fc2(x))
        x = torch.relu(self.fc3(x))
        x = torch.relu(self.fc4(x))

        return self.fcoutput(x)


if not torch.cuda.is_available():
    exit("No cuda available ")
device = torch.device("cuda")

transition = namedlist.namedlist("transition",('state','action','reward','next_state'))
memory = replay_storage.ReplayList(transition,capacity=5000)

select_action = action_selection.EpsGreedy(action_space)

policy_net = Model().to(device)
target_net = Model().to(device)

target_net.load_state_dict(policy_net.state_dict())
target_net.eval()

loss_function = F.smooth_l1_loss
optimizer = optim.Adam(policy_net.parameters())

N_EPISODES = 5000
BATCH_SIZE = 500
GAMMA = 0.8


def optimize_model():
    if len(memory) < BATCH_SIZE:
        return

    transitions = memory.sample(BATCH_SIZE)

    batch = transition(*zip(*transitions))

    state_batch = torch.cat(batch.state)
    action_batch = torch.cat(batch.action)
    reward_batch = torch.cat(batch.reward)
    next_state_batch = torch.cat(batch.next_state)

    action_values = policy_net(state_batch)
    action_values = action_values.gather(1, action_batch.unsqueeze(-1))

    next_state_values = target_net(next_state_batch)

    excepted_state_action_value = reward_batch + GAMMA * next_state_values.max(1)[0]

    loss = loss_function(action_values,excepted_state_action_value.unsqueeze(1).detach())
    loss.backward()

    optimizer.step()
    optimizer.zero_grad()


for i_episode in range(N_EPISODES):
    state = env.reset()
    state = torch.Tensor(state).to(device).unsqueeze(0)
    episode_memory = replay_storage.ReplayList(transition)
    for t in count():
        env.render()
        actions = select_action(policy_net,state)
        new_state, reward, done, _ = env.step(actions.cpu().numpy()[0])
        new_state = torch.Tensor(new_state).to(device).unsqueeze(0)
        if done:
            break

        episode_memory.append(state, torch.Tensor([actions]).to(device).long(), reward, new_state)
        state = new_state
        optimize_model()
    print(i_episode, t)

    for i in range(t,0,-1):
        transit = episode_memory.memory[i-1]
        transit.reward = torch.Tensor([1 - math.exp(-1*(t-i)/10)]).to(device)
        memory.append(*transit)

    if (i_episode + 1) % 10 == 0:
        target_net.load_state_dict(policy_net.state_dict())