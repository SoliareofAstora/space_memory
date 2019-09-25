import gym
import psutil
import ray
import math
import itertools


class enviro():
    def __init__(self, name):
        self.env = gym.make(name)

    def reset(self):
        return self.env.reset()

    def step(self, action):
        state, reward, done, _ = self.env.step(action)
        if done:
            state = self.env.reset()
        return state, reward, done

    def close(self):
        self.env.close()


@ray.remote
class sequential_envs():
    def __init__(self, name, n):
        self.n = n
        self.envs = [enviro(name) for i in range(n)]

    def reset(self):
        return [e.reset() for e in self.envs]

    def step(self, actions):
        return list(zip(*[self.envs[i].step(actions[i]) for i in range(self.n)]))

    def render(self):
        self.envs[0].env.render()

    def close(self):
        for e in self.envs:
            e.close()


class parallel_envs():
    def __init__(self, name, n):
        self.num_cpus = psutil.cpu_count()*2
        ray.init(object_store_memory=500 * 1024 * 1024, num_cpus=self.num_cpus)

        self.n = n
        step = math.ceil(n / self.num_cpus)
        self.indexses = [[i * step, (i + 1) * step] for i in range((self.n + step - 1) // step)]
        self.indexses[-1][1] = self.n
        self.envs = [sequential_envs.remote(name, i[1] - i[0]) for i in self.indexses]

    def reset(self):
        return list(itertools.chain.from_iterable(ray.get([e.reset.remote() for e in self.envs])))

    def step(self, actions):
        return [list(itertools.chain.from_iterable(z)) for z in list(zip(*
            ray.get(
            [self.envs[i].step.remote(actions[self.indexses[i][0]:self.indexses[i][1]]) for i in range(self.num_cpus)])))]

    def render(self):
        self.envs[0].render.remote()

    def close(self):
        for e in self.envs:
            e.close.remote()
        ray.shutdown()


# import numpy as np
#
# name = 'CartPole-v1'
# n = 128
# a = parallel_envs(name, n)
#
# b = a.reset()
#
# action = np.array([0] * n)
# for i in range(10000):
#     state, reward, done = a.step(action)
#
# a.close()
