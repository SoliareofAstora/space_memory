import space_enviro.spaceLib as environment
import numpy as np


env = environment.initialize("habababa")
while env.check():
    actions = np.array([1.,1.],dtype=np.float32)
    env.update(actions)
    a = env.get_observations()
    b = env.get_stats()
    c = env.get_reward()
print(b)
print(a.shape)
print (c)
