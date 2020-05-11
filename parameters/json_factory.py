import json
import pathlib

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
