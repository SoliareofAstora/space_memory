import random
import math
import torch


class EpsGreedy(object):
    def __init__(self, action_space, e_start=0.8, e_end=0.05, e_decay=1000):
        self.action_space = action_space
        self.start = e_start
        self.end = e_end
        self.decay = e_decay
        self.counter = 0
        self.current_threshold = e_start

    def __call__(self, ai, state):
        with torch.no_grad():
            q_values = ai(state).max(1)[1]
        sample = random.random()
        self.counter += 1
        self.current_threshold = self.end + (self.start - self.end) * math.exp(-1. * self.counter / self.decay)

        if sample > self.current_threshold:
            return q_values
        else:
            return torch.randint(self.action_space, [q_values.shape[0]])