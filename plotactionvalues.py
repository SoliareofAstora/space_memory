import pathlib
import time
from collections import namedtuple
import json
import numpy as np
import torch

import components.environment.env_wrapper as env_wrapper
import components.model.load_model as load_model
import components.rl_algorithm.utils.action_selection as action_selection
from components.logging_tools import WalkingAverage
from components.rl_algorithm.utils.replay_storage import ReplayList

path = "/home/SoliareofAstora/space_memory/experiments/done/stopping/dqn/first/first__1_1589613456"
path = pathlib.Path(path)
params = {}
files = list(path.glob("parameters/*.json"))
for file in files:
    with open(str(file), "r") as f:
        params.update(json.load(f))
if "seed" not in params.keys():
    params["seed"] = int(time.time())

device = torch.device("cuda" if torch.cuda.is_available() else "cpu")
if not torch.cuda.is_available():
    exit(1)

transition = namedtuple('Transition', ('state', 'actions', 'next_state', 'reward'))
memory = ReplayList(transition, params["memory_size"])

# for weights in list(path.glob("weights/*.pth")):
with pathlib.Path("/home/SoliareofAstora/space_memory/experiments/done/stopping/dqn/first/first__1_1589613456/weights/1589613590.pth") as weights:
    policy_net = torch.load(weights, map_location="cpu")
    policy_net.eval()
    policy_net.to(device)

    summary_passed = 0
    summary_failed = 0
    log_frequency = params["log_frequency"]
    average = WalkingAverage(log_frequency)
    target_update = params["target_update_frequency"]
    limit = params["steps"]
    save_interval = params["model_save_interval"]
    # path = pathlib.Path(params["path"])

    env = env_wrapper.env(params)
    state = env.reset()
    state = torch.autograd.Variable(torch.Tensor(state).to(device))

    qlist = []
    statelist = []
    for t in range(10000):
        with torch.no_grad():
            q_values = policy_net(state)
            actions = q_values.max(1)[1]
        qlist.append(q_values.cpu().numpy())
        statelist.append(state.cpu().numpy())
        new_state, reward, terminate = env.step(actions)
        average.append(reward)

        summary_passed += (np.sum(reward[terminate] > 0))
        summary_failed += (np.sum(reward[terminate] < 0))

        new_state = torch.autograd.Variable(torch.Tensor(new_state).to(device))
        memory.append(state, actions, new_state, torch.autograd.Variable(torch.Tensor(reward).to(device)))
        state = new_state
    print(weights)
    print("passed", summary_passed)
    print("failed", summary_failed)
    del env

qtable = np.concatenate(qlist)
statetable = np.concatenate(statelist)

qtable.shape
statetable.shape

import matplotlib
import matplotlib.pyplot as plt
cmap = matplotlib.cm.viridis

plt.scatter(statetable[:,2], statetable[:,1],s=0.2, cmap = cmap, c = qtable[:,0])
plt.colorbar()
plt.show()

plt.scatter(statetable[:,2], statetable[:,1],s=0.2, cmap = cmap, c = qtable[:,3])
plt.colorbar()
plt.show()

plt.scatter(statetable[:,2], statetable[:,1],s=0.2, cmap = cmap, c = qtable[:,4])
plt.colorbar()
plt.show()

plt.scatter(statetable[:,2], statetable[:,1],s=0.2, cmap = cmap, c = qtable[:,1])
plt.colorbar()
plt.show()

plt.scatter(statetable[:,2], statetable[:,1],s=0.2, cmap = cmap, c = qtable[:,2])
plt.colorbar()
plt.show()

plt.scatter(statetable[:,0], statetable[:,1],s=0.2, cmap = cmap, c = qtable[:,0])
plt.colorbar()
plt.show()
plt.scatter(statetable[:,0], statetable[:,1],s=0.2, cmap = cmap, c = qtable[:,1])
plt.colorbar()
plt.show()
plt.scatter(statetable[:,0], statetable[:,1],s=0.2, cmap = cmap, c = qtable[:,2])
plt.colorbar()
plt.show()


def softmax(x):
    return np.exp(x) / np.sum(np.exp(x),axis=1)[:,None]

qtab = softmax(qtable)

plt.scatter(statetable[:,2], statetable[:,1],s=0.2, cmap = cmap, c = qtab[:,0])
plt.colorbar()
plt.show()


plt.scatter(statetable[:,2], statetable[:,1],s=0.2, cmap = cmap, c = qtab[:,1])
plt.colorbar()
plt.show()

plt.scatter(statetable[:,2], statetable[:,1],s=0.2, cmap = cmap, c = qtab[:,2])
plt.colorbar()
plt.show()

plt.scatter(statetable[:,2], statetable[:,1],s=0.2, cmap = cmap, c = qtab[:,3])
plt.colorbar()
plt.show()

plt.scatter(statetable[:,2], statetable[:,1],s=0.2, cmap = cmap, c = qtab[:,4])
plt.colorbar()
plt.show()

plt.scatter(statetable[:,0], statetable[:,1],s=0.2, cmap = cmap, c = qtab[:,0])
plt.colorbar()
plt.show()
plt.scatter(statetable[:,0], statetable[:,1],s=0.2, cmap = cmap, c = qtab[:,1])
plt.colorbar()
plt.show()
plt.scatter(statetable[:,0], statetable[:,1],s=0.2, cmap = cmap, c = qtab[:,2])
plt.colorbar()
plt.show()
