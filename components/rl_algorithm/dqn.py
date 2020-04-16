import components.model.load_model as load_model
import components.environment.env_wrapper as env_wrapper
from components.rl_algorithm.utils.replay_storage import ReplayList
import pathlib
import json
import time

import math
import random
import numpy as np

from collections import namedtuple
from itertools import count

import torch
import torch.optim as optim
import torch.nn.functional as F

def run(params):
    env = env_wrapper.env(params)

    device = torch.device("cuda" if torch.cuda.is_available() else "cpu")
    if not torch.cuda.is_available():
        return 1

    transition = namedtuple('Transition', ('state', 'actions', 'next_state', 'reward'))

    memory = ReplayList(transition, params["memory_size"])

    BATCH_SIZE = params["batch_size"]
    GAMMA = 0.80
    EPS_START = 0.90

    EPS_END = 0.1
    EPS_DECAY = 10000

    policy_net = load_model.get_model(params).to(device)
    target_net = load_model.get_model(params).to(device)

    target_net.load_state_dict(policy_net.state_dict())
    target_net.eval()

    optimizer = optim.RMSprop(policy_net.parameters(), params["lr"])

    steps_done = 0
    action_space = params["action_space"]

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

        loss = F.smooth_l1_loss(action_values, excepted_state_action_value.unsqueeze(1).detach())
        loss.backward()

        for param in policy_net.parameters():
            param.grad.data.clamp_(-1, 1)
        optimizer.step()
        optimizer.zero_grad()


    class WalkingAverage(object):
        limit = 100
        memory = []
        current = 0

        def __init__(self, limit):
            self.limit = limit

        def append(self, value):
            if self.current >= self.limit:
                self.memory[self.current % self.limit] = value
            else:
                self.memory.append(value)
            self.current += 1

        def get(self):
            return np.average(self.memory)

    average = WalkingAverage(10)
    living_penalty = params["living_penalty"]
    target_update = params["target_update_frequency"]
    limit = params["steps"]
    save_interval = params["model_save_interval"]
    path = pathlib.Path(params["path"])
    state = env.reset()
    state = torch.autograd.Variable(torch.Tensor(state).to(device))
    for t in range(limit):
        actions = get_actions(state)
        new_state, reward, _ = env.step(actions)
        average.append(np.sum(reward) * 100)
        reward -= living_penalty
        if not env.active():
            exit()
        new_state = torch.autograd.Variable(torch.Tensor(new_state).to(device))
        memory.append(state, actions, new_state, torch.autograd.Variable(torch.Tensor(reward).to(device)))
        with open(path/"logs"/"scores.txt", "a") as f:
            f.write(str(t)+","+str(average.get())+'\n')
        state = new_state
        optimize_model(t)
        steps_done += 1

        if (t - 1) % target_update == 0:
            target_net.load_state_dict(policy_net.state_dict())

        if (t - 1) % save_interval == 0:
            torch.save(policy_net, path/"weights"/(str(int(time.time())) + ".pth"))

    del env
    return 0

