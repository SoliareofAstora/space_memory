import json
import os
import pathlib
import shutil
import sys
import time
import components.rl_algorithm.dqn as dqn
import locks

def run_soft_experiment(path):
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
        dqn.run(params)


if __name__ == "__main__":
    run_soft_experiment(sys.argv[1])