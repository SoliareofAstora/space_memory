import pathlib
import time
from collections import namedtuple
import json
import numpy as np
import torch
import gc
import components.environment.env_wrapper as env_wrapper
import components.model.load_model as load_model
import components.rl_algorithm.utils.action_selection as action_selection
from components.logging_tools import WalkingAverage
from components.rl_algorithm.utils.replay_storage import ReplayList
import matplotlib
import matplotlib.pyplot as plt

# 0 dangerous
# 1 Fast
# 2 safe

with open("results.json",'r') as f:
    res = json.load(f)

for results in res:
    toverify = []
    for key in results[0]["parameters"].keys():
        try:
            unique = np.unique(list(map(lambda x: x["parameters"][key], results)))
        except TypeError:
            continue
        if len(unique)>1:
            toverify.append({"key":key,"values":unique})

    toverify = list(filter(lambda x:x['key']!="action_matrix", toverify))
    toverify = list(filter(lambda x:x['key']!="changes", toverify))

    print(results[0]["parameters"]['scenario_name'])
    names = ['danger', 'fast', 'safe']
    header = "Agent type"
    for t in toverify:
        header += " & " + t['key']
    print(header + '\\\\')
    for i in range(3):

        for t in toverify:
            names[i] += ' & '+str(results[i]["parameters"][t['key']])
        print(names[i]+'\\\\')