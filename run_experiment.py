import json
import os
import pathlib
import shutil
import time
import components.rl_algorithm.dqn as dqn
import locks

def run_experiment(path):
    path = pathlib.Path(path)
    params = {}
    files = list(path.glob("parameters/*.json"))
    for file in files:
        with open(str(file), "r") as f:
            params.update(json.load(f))
    if "seed" not in params.keys():
        params["seed"] = int(time.time())

    os.mkdir(path/"weights")
    os.mkdir(path/"frames")
    os.mkdir(path/"logs")
    os.mkdir(path/"results")
    params["frames_path"] = str((path/"frames").absolute())
    params["path"] = str(path)
    if params["rl_name"] == "dqn":
        output = dqn.run(params)

    if output == 0:
        results_path = pathlib.Path("experiments/done")/params["scenario_name"]/params["rl_name"]/params["series_name"]
        if not results_path.exists():
            results_path.mkdir(parents=True)

        locks.lock_done()
        shutil.move(str(path),str(results_path),"-r")
        locks.unlock_done()
    else:
        exit("error while running:",path)
        #todo globaly plik z errorami

