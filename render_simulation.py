import os
import pathlib
import time
import json
import numpy as np
import torch
import components.environment.env_wrapper as env_wrapper
import multiprocessing

with open("selected_runs.json",'r') as f:
    selected = json.load(f)


sel1 = np.array([item for sublist in selected for item in sublist])

# e = sel1[0]
# e = sel1[1]
# e = sel1[2]
# e = sel1[3]
# e = sel1[4]
# e = sel1[5]
# e = sel1[6]
# e = sel1[7]
# e = sel1[8]


# for e in sel1[3::3]:
weight = pathlib.Path(e['weights'][-1])
params = {}
files = list(weight.parents[1].glob("parameters/*.json"))
for file in files:
    with open(str(file), "r") as f:
        params.update(json.load(f))
if "seed" not in params.keys():
    params["seed"] = int(time.time())

device = torch.device("cuda" if torch.cuda.is_available() else "cpu")
if not torch.cuda.is_available():
    exit(1)

if str(weight.parents[4].name) == "stopping":
    params['n']=5
else:
    params['n']= 25
params["render"] = False
params["record"] = True
params["debug"] = True
starttime = time.time()
pathname = "/frames/" + str(weight.parents[4].name)+"_"+e['agentType'] +"_"+ str(int(starttime))
os.mkdir(pathname)
params["frames_path"] = pathname
env = env_wrapper.env(params)
state = env.reset()
state = torch.autograd.Variable(torch.Tensor(state).to(device))

policy_net = torch.load(weight, map_location="cpu")
policy_net.eval()
policy_net.to(device)

for t in range(10000):
    with torch.no_grad():
        actions = policy_net(state).max(1)[1]
    new_state, _, _ = env.step(actions)
    state = torch.autograd.Variable(torch.Tensor(new_state).to(device))
del env




