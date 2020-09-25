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

        with open(e["source"] + "/results/test.json", 'r') as f:
            tmp = json.load(f)
            tmp.sort(key=lambda x: x['weights'])
            e["weights"] = list(map(lambda x: x['weights'], tmp))
            e["passed"] = list(map(lambda x: x['passed'], tmp))
            e["failed"] = list(map(lambda x: x['failed'], tmp))


    return experiments

# resfirst = res
# ressecond = res
# resthird = res

save = False
# dpi = 300
dpi = 100
first = False
second = False



sources = [pathlib.Path("/home/SoliareofAstora/space_memory/experiments/done/stopping/dqn/first"),
          pathlib.Path("/home/SoliareofAstora/space_memory/experiments/done/checkpoint_single/dqn/1m_long"),
          pathlib.Path("/home/SoliareofAstora/space_memory/experiments/done/checkpoint_single_v2/dqn/1m_long")]
namechanger = {'stopping':'Pierwszy',
               'checkpoint_single':'Drugi',
               'checkpoint_single_v2':'Trzeci'}

namechanger2= {'stopping':'pierwszym',
               'checkpoint_single':'drugim',
               'checkpoint_single_v2':'trzecim'}

selected = []

for v in range(len(sources)):
    path = sources[v]
    paths = np.array(list(path.glob("**/parameters")))

    # if len(paths) > 8:
    #     step = int(len(paths) / 8)
    #     indexList = [[i + j * step for i in range(0, step)] for j in range(8)]
    #
    #     if len(paths) - indexList[-1][-1] > 1:
    #         indexList[-1].extend([i for i in range(indexList[-1][-1] + 1, len(paths))])
    #
    #     with multiprocessing.Pool(8) as p:
    #         result = p.map(funfun,indexList)
    #     experiments = []
    #     for res in result:
    #         experiments += res
    #
    # else:
    #     experiments = funfun([i for i in range(len(paths))])
    #
    experiments = funfun([i for i in range(len(paths))])


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

    if first:
        for ver in toverify:
            plt.title(namechanger[experiments[0]['parameters']['scenario_name']]+" scenariusz. Zmieniany parametr: "+ver['key'])
            color_id = 0
            plt.xlabel("krok / "+str(experiments[0]['parameters']['log_frequency']))
            plt.ylabel("nagroda")
            for value in ver['values']:
                legend = True
                selectedExperiments = list(filter(lambda x: x['parameters'][ver['key']]==value,experiments))

                arr = np.array(list(map(lambda x: x['results'],selectedExperiments)))
                arr = np.average(arr,axis=0)
                # plt.plot(arr, color=colors[color_id], label=value)
                plt.plot(running_mean(arr,200),color=colors[color_id], label=value, linewidth=0.5)
                # plt.plot(running_mean(selectedExperiments[0]["results"], 50), color=colors[color_id], label=value)
                # for e in selectedExperiments[1:]:
                #     plt.plot(running_mean(e["results"], 50),color=colors[color_id])

                color_id += 1
                legend = True
            plt.legend()

            if save:
                plt.savefig(
                    "/home/SoliareofAstora/space_memory/paper/images/trainings/" +path.parents[1].name+ver['key']+
     ".png", bbox_inches='tight',dpi=dpi)
                plt.close()
            else:
                plt.show()

    #
    # for ver in toverify:
    #     plt.title(ver['key'])
    #     color_id = 0
    #     for value in ver['values']:
    #         legend = True
    #         selectedExperiments = list(filter(lambda x: x['parameters'][ver['key']]==value,experiments))
    #
    #         arrp = np.array(list(map(lambda x: x['passed'],selectedExperiments)))
    #         arr = np.average(arrp,axis=0)
    #         # plt.plot(arr, color=colors[color_id], label=value)
    #         plt.plot(arr,color=colors[color_id], label=value, linewidth=0.5)
    #         arrn = np.array(list(map(lambda x: x['failed'],selectedExperiments)))
    #         arr = np.average(arrn,axis=0)*-1
    #         # plt.plot(arr, color=colors[color_id], label=value)
    #         plt.plot(arr,color=colors[color_id], linewidth=0.5)
    #         # plt.plot(running_mean(selectedExperiments[0]["results"], 50), color=colors[color_id], label=value)
    #         # for e in selectedExperiments[1:]:
    #         #     plt.plot(running_mean(e["results"], 50),color=colors[color_id])
    #
    #         color_id += 1
    #         legend = True
    #     plt.legend()
    #     # plt.savefig("plot_results/" + ver['key'] + ".png", dpi=300)
    #     plt.show()
    #
    #
    # for e in experiments:
    #     plt.plot(e['passed'],linewidth=0.05)
    #     plt.plot(np.array(e['failed'])*-1,linewidth=0.05)
    # plt.show()
    #
    res = [0,1,2]
    worst = 99999
    best = 0
    safest = 99999999
    efficient = 0


    for e in experiments:
        if e['passed'][-1]/(e['failed'][-1]+1) < worst:
            worst = e['passed'][-1]/(e['failed'][-1]+1)
            res[0] = e
        if e['passed'][-1] > best:
            best = e['passed'][-1]
            res[1] = e
        if e['failed'][-1] < safest:
            safest = e['failed'][-1]
            efficient = e['passed'][-1]
            res[2] = e
        if e['failed'][-1] == safest:
            if e['passed'][-1] > efficient:
                efficient = e['passed'][-1]
                res[2] = e
    selected.append(res)

    if second:
        if v != 2:
            fig = plt.figure(num=None, figsize=(12, 8), dpi=dpi, facecolor='w', edgecolor='k')
            for e in experiments:
                plt.plot(running_mean(e["results"],200),linewidth=0.015)
            plt.plot(running_mean(res[2]["results"],200), label="Agent bezpieczny  Ocalone: "+str(int(res[2]['passed'][-1]))+' Zniszczone: '+str(int(res[2]['failed'][-1])), c='g')
            plt.plot(running_mean(res[1]["results"],200),label="Agent szybki Ocalone: "+str(int(res[1]['passed'][-1]))+' Zniszczone: '+str(int(res[1]['failed'][-1])),c='blue')
            plt.plot(running_mean(res[0]["results"],200),label="Agent najgorszy  Ocalone: "+str(int(res[0]['passed'][-1]))+' Zniszczone: '+str(int(res[0]['failed'][-1])),c='r')
            plt.legend(loc=2)
            plt.xlabel("krok / 200")
            plt.ylabel("nagroda")
            if v == 0:
                plt.xlim(0,4800)
            else:
                plt.xlim(0,9800)
            plt.title("Nagrody otrzymywane przez 3 agentów oraz liczba zniszczonych i ocalonych pojazdów w "+namechanger2[experiments[0]['parameters']['scenario_name']]+" scenariuszu.")
            if save:
                fig.savefig(
                    "/home/SoliareofAstora/space_memory/paper/images/trainings/" + path.parents[1].name + "summary.png", bbox_inches='tight')
                plt.close(fig)
            else:
                fig.show()
        else:
            fig = plt.figure(num=None, figsize=(12, 8), dpi=dpi, facecolor='w', edgecolor='k')
            for e in experiments:
                plt.plot(running_mean(e["results"], 200), linewidth=0.015)
            res_safe = running_mean(res[2]["results"], 200)

            plt.plot(running_mean(res[2]["results"],200)[0:1], label="Agent bezpieczny  Ocalone: "+str(int(res[2]['passed'][-1]))+' Zniszczone: '+str(int(res[2]['failed'][-1])), c='g')
            plt.plot(running_mean(res[1]["results"],200)[0:1],label="Agent szybki Ocalone: "+str(int(res[1]['passed'][-1]))+' Zniszczone: '+str(int(res[1]['failed'][-1])),c='blue')
            qwer = 100
            step = int(9800/qwer)
            for w in range(100):
                if w%2==0:
                    plt.plot(np.arange(step*w,(w+1)*step),res_safe[step*w:(w+1)*step],  c='blue')
                else:
                    plt.plot(np.arange(step*w,(w+1)*step),res_safe[step*w:(w+1)*step],  c='g')
            plt.plot(running_mean(res[0]["results"], 200), label="Agent najgorszy  Ocalone: " + str(int(res[0]['passed'][-1])) + ' Zniszczone: ' + str(int(res[0]['failed'][-1])), c='r')
            plt.legend(loc=2)
            plt.xlabel("krok / 200")
            plt.ylabel("nagroda")
            if v == 0:
                plt.xlim(0, 4800)
            else:
                plt.xlim(0, 9800)
            plt.title(
                "Nagrody otrzymywane przez 3 agentów oraz liczba zniszczonych i ocalonych pojazdów w " + namechanger2[
                    experiments[0]['parameters']['scenario_name']] + " scenariuszu.")
            if save:
                fig.savefig(
                    "/home/SoliareofAstora/space_memory/paper/images/trainings/" + path.parents[1].name + "summary.png",
                    bbox_inches='tight')
                plt.close(fig)
            else:
                fig.show()

