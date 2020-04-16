import json
import pathlib

params = {}

params['passThreshold'] = 2
params['resetThreshold'] = 20
params['resetAngleThreshold'] = 2
params['minV'] = 5
params['maxV'] = 10
params['maxAngleV'] = 2


FILE_NAME = "environment/default_env_params/universal.json"

with open(FILE_NAME, 'r') as f:
    abc = json.load(f)

if pathlib.Path(FILE_NAME).exists():
    print("File exists")
else:
    output = json.dumps(abc).replace(", ",", \n")
    with open(FILE_NAME, 'w') as f:
        f.writelines(output)
        json.dump(params, f)
