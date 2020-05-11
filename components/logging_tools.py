import numpy as np


class WalkingAverage(object):
    limit = 100
    memory = []
    current = 0

    def __init__(self, limit):
        self.limit = limit

    def append(self, value):
        if self.current >= self.limit:
            self.memory[self.current % self.limit] = np.average(value)
        else:
            self.memory.append(np.average(value))
        self.current += 1

    def get(self):
        return np.average(self.memory)
