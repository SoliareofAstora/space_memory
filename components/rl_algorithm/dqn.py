import pathlib
import time
from collections import namedtuple

import numpy as np
import torch

import components.environment.env_wrapper as env_wrapper
import components.model.load_model as load_model
import components.rl_algorithm.utils.action_selection as action_selection
from components.logging_tools import WalkingAverage
from components.rl_algorithm.utils.replay_storage import ReplayList
from components.rl_algorithm.utils.optimizers import get_optimizer
from components.rl_algorithm.utils.loss import get_loss

def run(params):
    env = env_wrapper.env(params)

    device = torch.device("cuda" if torch.cuda.is_available() else "cpu")
    if not torch.cuda.is_available():
        return 1

    transition = namedtuple('Transition', ('state', 'actions', 'next_state', 'reward'))

    memory = ReplayList(transition, params["memory_size"])

    BATCH_SIZE = params["batch_size"]
    GAMMA = params["gamma"]
    get_actions = action_selection.load_action_selection(params)

    policy_net = load_model.get_model(params).to(device)
    target_net = load_model.get_model(params).to(device)

    target_net.load_state_dict(policy_net.state_dict())
    target_net.eval()

    optimizer = get_optimizer(params, policy_net.parameters())
    lossFun = get_loss(params)

    def optimize_model():
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

        loss = lossFun(action_values, excepted_state_action_value.unsqueeze(1).detach())
        loss.backward()

        for param in policy_net.parameters():
            param.grad.data.clamp_(-1, 1)
        optimizer.step()
        optimizer.zero_grad()

    log_frequency = params["log_frequency"]
    average = WalkingAverage(log_frequency)
    living_penalty = params["living_penalty"]
    target_update = params["target_update_frequency"]
    limit = params["steps"]
    save_interval = params["model_save_interval"]
    path = pathlib.Path(params["path"])
    state = env.reset()
    state = torch.autograd.Variable(torch.Tensor(state).to(device))
    for t in range(limit):
        actions = get_actions(policy_net, state).to(device)
        new_state, reward, _ = env.step(actions)
        average.append(reward)
        reward -= living_penalty
        if not env.active():
            return 0
        new_state = torch.autograd.Variable(torch.Tensor(new_state).to(device))
        memory.append(state, actions, new_state, torch.autograd.Variable(torch.Tensor(reward).to(device)))
        state = new_state
        optimize_model()

        if (t + 1) % log_frequency == 0:
            with open(path/"results"/"scores.txt", "a") as f:
                f.write(str(t)+","+str(average.get())+'\n')

        if (t + 1) % target_update == 0:
            target_net.load_state_dict(policy_net.state_dict())

        if (t + 1) % save_interval == 0:
            torch.save(policy_net, path/"weights"/(str(int(time.time())) + ".pth"))

    del env
    return 0

