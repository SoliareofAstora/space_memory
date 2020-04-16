from components.environment.space_enviro.spaceLib import space_env
import numpy as np


class env(object):
    def __init__(self, params):
        self.discrete = np.array(params['action_matrix'])
        self.env = space_env(params)
        params["action_space"] = self.discrete.shape[0]

    def reset(self):
        return self.env.reset()

    def active(self):
        return self.env.active()

    def step(self, actions):
        action_list = actions.cpu().tolist()
        actions_real = np.array((self.discrete[action_list, 0], self.discrete[action_list, 1]), dtype=np.float32)
        return self.env.step(actions_real)

# # actions_real = np.array((main_power[(actions / side_space).cpu().tolist()], side_power[(actions % side_space).cpu().tolist()]),  $
# from components.environment.space_enviro.spaceLib import Window
# w1 = Window(10)
# del w1
#
#
# from components.environment.space_enviro.spaceLib import space_env
# p = {}
# a=space_env(p)
# del a