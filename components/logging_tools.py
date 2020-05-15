import numpy as np


class WalkingAverage(object):
    def __init__(self, limit=100):
        self.limit = limit
        self.memory = []
        self.current = 0

    def append(self, value):
        if self.current >= self.limit:
            self.memory[self.current % self.limit] = np.mean(value)
        else:
            self.memory.append(np.mean(value))
        self.current += 1

    def get(self):
        return np.mean(self.memory)
