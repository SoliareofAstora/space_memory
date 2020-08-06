import os
import locks
import pathlib
import time
import json
import numpy as np
import torch
import components.environment.env_wrapper as env_wrapper


def run_test_queue():
    test_path = pathlib.Path("experiments/done")
    testqueue = list(map(lambda x: x.parent,list(test_path.glob("**/parameters"))))
    for test in testqueue:
        locks.lock_path(test_path)
        if (test/"results"/"test.json").exists():
            locks.unlock_path(test_path)
            continue
        f = open((test/"results"/"test.json"),'a')
        f.close()
        locks.unlock_path(test_path)

        params = {}
        files = list(test.glob("parameters/*.json"))
        for file in files:
            with open(str(file), "r") as f:
                params.update(json.load(f))
        if "seed" not in params.keys():
            params["seed"] = int(time.time())

        device = torch.device("cuda" if torch.cuda.is_available() else "cpu")
        if not torch.cuda.is_available():
            os.remove((test/"results"/"test.json"))
            f = open((test_path/"NOCUDA"), 'a')
            f.close()
            exit(1)
        result = []
        for weights in list(test.glob("weights/*.pth")):
            policy_net = torch.load(weights, map_location="cpu")
            policy_net.eval()
            policy_net.to(device)

            summary_passed = 0
            summary_failed = 0

            env = env_wrapper.env(params)
            state = env.reset()
            state = torch.autograd.Variable(torch.Tensor(state).to(device))

            for t in range(10000):
                with torch.no_grad():
                    actions = policy_net(state).max(1)[1]
                new_state, reward, terminate = env.step(actions)

                summary_passed += (np.sum(reward[terminate] > 0))
                summary_failed += (np.sum(reward[terminate] < 0))
                state = torch.autograd.Variable(torch.Tensor(new_state).to(device))
            result.append({"weights":str(weights), "passed":int(summary_passed), "failed":int(summary_failed)})
            del env
        with open((test/"results"/"test.json"),'w') as f:
            json.dump(result, f)


if __name__ == "__main__":
    run_test_queue()
