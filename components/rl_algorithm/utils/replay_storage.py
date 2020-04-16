import random
from collections import deque
import copy


class ReplayList(object):
    def __init__(self, data_type, capacity=5000):
        self.dtype = data_type
        self.capacity = capacity
        self.memory = []
        self.position = 0

    def append(self, *args):
        if len(self.memory) < self.capacity:
            self.memory.append(None)
        self.memory[self.position] = self.dtype(*args)
        self.position = (self.position + 1) % self.capacity

    def extend(self, args):
        for arg in args:
            self.append(*arg)

    def sample(self, batch_size):
        return random.sample(self.memory, batch_size)

    def deepcopy(self):
        return copy.deepcopy(self.memory)

    def erase(self):
        self.memory = []
        self.position = 0

    def __len__(self):
        return len(self.memory)


class ReplayQueue(object):
    def __init__(self, data_type, capacity=10):
        self.dtype = data_type
        self.memory = deque()
        self.capacity = capacity

    def append(self, *args):
        self.memory.append(self.dtype(*args))
        if len(self.memory) > self.capacity:
            return self.memory.popleft()
        return None

    def sample(self, batch_size):
        return random.sample(list(self.memory), batch_size)

    def __iter__(self):
        return iter(self.memory)

    def deepcopy(self):
        return copy.deepcopy(list(self.memory))

    def erase(self):
        self.memory.clear()

    def __len__(self):
        return len(self.memory)



