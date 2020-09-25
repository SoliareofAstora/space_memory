import numpy as np
import matplotlib.pyplot as plt
import json
import pathlib
import multiprocessing

def fun(path):
    params = {}
    files = list(pathlib.Path(path).glob("parameters/*.json"))
    for file in files:
        with open(str(file), "r") as f:
            params.update(json.load(f))
    return params


def funfun(index):
    path = paths[index]
    experiments = list(map(lambda x: {"source":str(x.parent)},path))

    for e in experiments:
        e["parameters"] = fun(e["source"])
        try:
            with open(e["source"]+"/results/scores.txt",'r') as f:
                res = f.readlines()
            res = list(map(lambda x: x.split(",")[1],res))
            e["results"] = np.array(res,dtype=np.float)
        except FileNotFoundError:
            continue
    return experiments

path = pathlib.Path("/home/SoliareofAstora/space_memory/experiments/done/stopping/dqn/first")
# path = pathlib.Path("/home/SoliareofAstora/space_memory/experiments/done/checkpoint_single/dqn/1m_long")
# path = pathlib.Path("/home/SoliareofAstora/space_memory/experiments/done/checkpoint_single_v2/dqn/1m_long")

paths = np.array(list(path.glob("**/test.json")))

res = []
for p in paths:
    with open(p, "r") as f:
        res.extend(json.load(f))



experiments = list(filter(lambda x: len(x)==3,experiments))

# np.unique(list(map(lambda x: x["parameters"]["steps"], experiments)))
experiments = list(filter(lambda x: x['parameters']["depth"]!=50,experiments))

toverify = []
for key in experiments[0]["parameters"].keys():
    try:
        unique = np.unique(list(map(lambda x: x["parameters"][key], experiments)))
    except TypeError:
        continue
    if len(unique)>1:
        toverify.append({"key":key,"values":unique})

toverify = list(filter(lambda x:x['key']!="action_matrix", toverify))
toverify = list(filter(lambda x:x['key']!="changes", toverify))


for t in toverify:
    print(t['key'],t['values'])

def running_mean(x, N):
    cumsum = np.cumsum(np.insert(x, 0, 0))
    return (cumsum[N:] - cumsum[:-N]) / float(N)

colors = ['b','r','c','m','g','y','k']
color_id = 0
legend = False

for ver in toverify:

    plt.title(ver['key'])
    color_id = 0
    for value in ver['values']:
        legend = True
        selectedExperiments = list(filter(lambda x: x['parameters'][ver['key']]==value,experiments))

        arr = np.array(list(map(lambda x: x['results'],selectedExperiments)))
        arr = np.average(arr,axis=0)
        # plt.plot(arr, color=colors[color_id], label=value)
        plt.plot(running_mean(arr,500),color=colors[color_id], label=value, linewidth=0.5)

        # plt.plot(running_mean(selectedExperiments[0]["results"], 50), color=colors[color_id], label=value)
        # for e in selectedExperiments[1:]:
        #     plt.plot(running_mean(e["results"], 50),color=colors[color_id])

        color_id += 1
        legend = True
    plt.legend()
    # plt.savefig("plot_results/" + ver['key'] + ".png", dpi=300)
    plt.show()

for e in experiments:
    plt.plot(running_mean(e["results"],50),linewidth=0.05)
plt.title("All "+str(len(experiments))+"experiments: Stopping DQN 3 actions")
#plt.savefig("plot_results/AllRuns"+str(len(experiments))+".png", dpi=400)
plt.show()

# plt.cla()
# plt.clf()
# plt.close()import numpy as np
