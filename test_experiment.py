import json
import os
import pathlib
import shutil
import time
import components.rl_algorithm.dqn as dqn
import locks


def test_experiment(path):
    path = pathlib.Path(path)
    params = {}
    files = list(path.glob("parameters/*.json"))
    for file in files:
        with open(str(file), "r") as f:
            params.update(json.load(f))
    if "seed" not in params.keys():
        params["seed"] = int(time.time())

    try:
        os.mkdir(path/"weights")
        os.mkdir(path/"frames")
        os.mkdir(path/"logs")
        os.mkdir(path/"results")
    except FileExistsError:
        pass
    params["frames_path"] = str((path/"frames").absolute())
    params["path"] = str(path)
    params["render"] = True
    params["debug"] = True
    if params["rl_name"] == "dqn":
        output = dqn.run(params)

path = "/home/SoliareofAstora/space_memory/experiments/templates/checkpoint_single_rectangle_linear_dqn/v1"
test_experiment(path)