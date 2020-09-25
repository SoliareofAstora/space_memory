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
cmap = matplotlib.cm.jet


def softmax(x):
    return np.exp(x) / np.sum(np.exp(x), axis=1)[:, None]

for i in range(len(selected)):
    selected[i][0]["agentType"] = "worst"
    selected[i][1]["agentType"] = "fast"
    selected[i][2]["agentType"] = "safe"

input = selected[0]

qall = []
stateall = []
policies = []

for r in input:
    weights = r['weights'][-1]
    path = pathlib.Path(weights).parents[1]
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

    policy_net = torch.load(weights, map_location="cpu")
    policy_net.eval()
    policy_net.to(device)
    policies.append(policy_net)

    log_frequency = params["log_frequency"]
    average = WalkingAverage(log_frequency)
    target_update = params["target_update_frequency"]
    limit = params["steps"]
    save_interval = params["model_save_interval"]

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


        new_state = torch.autograd.Variable(torch.Tensor(new_state).to(device))
        state = new_state
    del env

    qtable = np.concatenate(qlist)
    statetable = np.concatenate(statelist)

    qall.append(qtable)
    stateall.append(statetable)


stateall

qtabs = []
for i in range(len(qall)):
    qtabs.append(softmax(qall[i]))

oaxes = ['Szybkość pojazd','Kierunek prędkości pojazdu','Prędkość kątowa']
onames = ['najgorszy','szybki','bezpieczny']
oactions = ['Naprzód','Lewo','Prawo','Naprzód\nLewo','Naprzód\nPrawo']
tactions = ['Naprzód','Lewo','Prawo','Naprzód + Lewo','Naprzód + Prawo']

axes = ['Ship_velocity','Ship_velocity_direction','Angular_velocity']
names = ['Dangerous','Fast','Safe']
actions = ['Forward','Left','Right','Forward+Left','Forward+Right']
statepairs = [[2,1],[0,1],[0,2]]
save = True
dpi = 150

for i in range(len(policies)):
    print(i)
    for pair in statepairs:
        print(pair)
        plt.hexbin(stateall[i][:, pair[0]], stateall[i][:, pair[1]], cmap=cmap,gridsize=100)
        plt.xlabel(oaxes[pair[0]])
        plt.ylabel(oaxes[pair[1]])
        plt.title("Agent " + onames[i] + " - liczba zaobserwowanych stanów.")
        cb = plt.colorbar()
        cb.set_label("Liczba stanów")
        if save:
            plt.savefig(
                "/home/SoliareofAstora/space_memory/paper/images/plots/" + names[i]+"distribution" + axes[pair[0]] +
                axes[pair[1]] + ".png",  bbox_inches='tight',dpi=dpi)
            plt.close()
        else:
            plt.show()

        plt.hexbin(stateall[i][:, pair[0]], stateall[i][:, pair[1]], cmap=cmap, C=qtabs[i].argmax(1),gridsize=200)
        plt.xlabel(oaxes[pair[0]])
        plt.ylabel(oaxes[pair[1]])
        plt.title("Agent " + onames[i] + " - wybrana akcja")
        cb = plt.colorbar(ticks = [0,1,2,3,4])
        cb.ax.set_yticklabels(oactions)
        cb.set_label("wybrana akcja")
        if save:
            plt.savefig("/home/SoliareofAstora/space_memory/paper/images/plots/" + names[i] + "selected" + axes[pair[0]] + axes[pair[1]] + ".png",dpi=dpi, bbox_inches='tight')
            plt.close()
        else:
            plt.show()

        plt.hexbin(stateall[i][:, pair[0]], stateall[i][:, pair[1]], cmap=cmap, C=qall[i].max(1))
        plt.xlabel(oaxes[pair[0]])
        plt.ylabel(oaxes[pair[1]])
        plt.title("Agent " + onames[i] + " - estymacja wartości stanu")
        cb = plt.colorbar()
        cb.set_label("Wartość stanu")
        if save:
            plt.savefig("/home/SoliareofAstora/space_memory/paper/images/plots/" + names[i]+ "statev" + axes[pair[0]] + axes[pair[1]] + ".png",dpi=dpi, bbox_inches='tight')
            plt.close()
        else:
            plt.show()

        for j in range(5):
            plt.hexbin(stateall[i][:, pair[0]], stateall[i][:, pair[1]], cmap=cmap, C=qtabs[i][:, j],vmin=0.15,vmax=0.28)
            plt.xlabel(oaxes[pair[0]])
            plt.ylabel(oaxes[pair[1]])
            plt.title("Agent " + onames[i]+" - relatywna wartość akcji "+tactions[j])
            cb = plt.colorbar()
            cb.set_label("Relatywna wartość akcji")
            if save:
                plt.savefig("/home/SoliareofAstora/space_memory/paper/images/plots/"+names[i]+actions[j]+"probabil"+axes[pair[0]]+axes[pair[1]]+".png",dpi=dpi, bbox_inches='tight')
                plt.close()
            else:
                plt.show()

            plt.hexbin(stateall[i][:, pair[0]], stateall[i][:, pair[1]], cmap=cmap, C=qall[i][:, j])
            plt.xlabel(oaxes[pair[0]])
            plt.ylabel(oaxes[pair[1]])
            plt.title("Agent " + onames[i] + " - wartość akcji " + tactions[j])
            cb = plt.colorbar()
            cb.set_label("Action q_value")
            if save:
                plt.savefig(
                    "/home/SoliareofAstora/space_memory/paper/images/plots/" + names[i] + actions[j] + "qval" + axes[
                        pair[0]] + axes[pair[1]] + ".png", dpi=dpi, bbox_inches='tight')
                plt.close()
            else:
                plt.show()