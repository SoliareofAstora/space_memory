import json
import pathlib

# {"scenario_name": "stopping",
# "passThreshold": 2,
# "resetThreshold": 20,
# "resetAngleThreshold": 2,
# "minV": 5,
# "maxV": 10,
# "maxAngleV": 2,
# "action_matrix": [[1, 0], [0, -1], [0, 1], [1, -1], [1, 1]],
# "observation_space": 3,
#
# }

params = {}

params['selector'] = "eps_greedy"
params['eps_start'] = 0.9
params['eps_end'] = 0.1
params['eps_decay'] = 10000

FILE_NAME = "parameters/action_selection/eps_greedy.json"

if pathlib.Path(FILE_NAME).exists():
    print("File exists")
else:
    output = json.dumps(params).replace(", ",", \n")
    with open(FILE_NAME, 'w') as f:
        f.writelines(output)
