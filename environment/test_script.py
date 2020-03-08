import space_enviro.spaceLib as environment
import numpy as np
params = {}
params["scenario_name"] = 'stopping'
params['n'] = 9
params['seed']=100
params['passThreshold'] = 0.4
params['resetThreshold'] = 20
params['resetAngleThreshold'] = 3
params['minV'] = 5
params['maxV'] = 10
params['maxAngleV'] = 2
params['render']=True

env = environment.initialize(params)

#połowa statków ma włączony boczny silnik a połowa główny
while env.active():
    states = env.render_step(np.array([1,0]*9, dtype=np.float32))
    print(states)
del env