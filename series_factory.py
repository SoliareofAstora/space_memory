import numpy as np
import template_factory
import copy
import locks


def linear(start, stop, n):
    return list(np.linspace(start, stop, n))


def discrete(start, stop, step=1):
    return list(np.arange(start, stop, step))


def exponential(exp_start, exp_stop, base, step=1):
    return list(np.power(base, discrete(exp_start, exp_stop, step)))


class SeriesFactory(object):
    def __init__(self):
        self.changes = []
        self.series = []
        self.size = 1
        self.template = None
        self.name = None

    def create(self, template_name, name, training_schedule=None):
        self.template = template_factory.TemplateFactory()
        self.template.load(template_name)
        self.name = name

    def add_parameters(self, name, values):
        if self.size == len(values):
            self.changes.append({"name": name, "type": "add", "values": values})
        else:
            print("Adding parameters size missmatch. Excepted:", self.size, "got:", len(values))

    def multiply_parameters(self, name, values):
        self.size *= len(values)
        self.changes.append({"name": name, "type": "multi", "values": values})

    def prepare(self):
        self.series = []
        for value in self.changes[0]["values"]:
            self.series.append({self.changes[0]["name"]: value})
        for change in self.changes[1:]:
            if change['type'] == "add":
                for i in range(len(change['values'])):
                    self.series[i][change["name"]] = change['values'][i]
            else:
                old_len = len(self.series)
                source_series = copy.deepcopy(self.series)
                self.series = []
                for v in range(len(change["values"])):
                    self.series.extend(copy.deepcopy(source_series))
                    for i in range(v * old_len, (v + 1) * old_len):
                        self.series[i][change["name"]] = change["values"][v]
        return self.series

    def save(self):
        locks.lock_queue()
        params = self.prepare()
        for param in params:
            template = copy.deepcopy(self.template)
            template.params["series"] = {"changes": self.changes}
            template.params["series"]["series_name"] = self.name
            for key in param.keys():
                template.change(key, param[key])
            template.save("experiments/queue/", "_", self.name, "_")
        locks.unlock_queue()

    def print(self):
        self.template.print()

    def __len__(self):
        return len(self.prepare())


abc = SeriesFactory()
abc.create("/home/SoliareofAstora/space_memory/experiments/templates/checkpoint_single_fat_linear_dqn/v0", "first")
abc.multiply_parameters("memory_size", [100000])
abc.multiply_parameters("model_save_interval", [100000])
abc.multiply_parameters("batch_size", [1024])
abc.multiply_parameters("living_penalty", [-1, 0, 0.1, 1])
abc.multiply_parameters("eps_decay", [10000, 100000])
abc.multiply_parameters("gamma", [0.99])
abc.multiply_parameters("target_update_frequency", [1000, 10000])
abc.multiply_parameters("reward_manipulation", ["identity", "no_negative_reward"])
abc.print()
print(len(abc))

abc.save()

# abc.create("experiments/templates/stopping_rectangle_linear_dqn/v0", "first")
# abc.multiply_parameters("memory_size", [3000])
# abc.multiply_parameters("model_save_interval", [10000])
# abc.multiply_parameters("batch_size", [32, 128, 512])
# abc.multiply_parameters("depth", [5, 10])
# abc.multiply_parameters("width", [25, 100, 200])
# abc.multiply_parameters("living_penalty", [0.01, 0.1, 0.25, 0.5, 1.0, 0.0])
# abc.multiply_parameters("target_update_frequency", [100, 1000])
# abc.multiply_parameters("eps_end", [0.1, 0.01])
# abc.multiply_parameters("eps_decay", [1000, 10000])


# abc.multiply_parameters("batch_size", [32, 128, 512])
# abc.multiply_parameters("depth", [5, 10])
# abc.multiply_parameters("width", [200])
# abc.multiply_parameters("living_penalty", [0.01, 0.1, 0.25,0.5, 1.0, 0.0])
# abc.multiply_parameters("target_update_frequency", [100, 1000])
# abc.multiply_parameters("eps_end", [0.1, 0.01])
# abc.multiply_parameters("eps_decay", [1000, 10000])

# abc.multiply_parameters("batch_size", [32, 128, 512])
# abc.multiply_parameters("depth", [5, 10, 50])
# abc.multiply_parameters("width", [25, 100])
# abc.multiply_parameters("living_penalty", [0.01, 0.1, 0.25])
# abc.multiply_parameters("target_update_frequency", [100, 1000])
# abc.multiply_parameters("eps_end", [0.1, 0.01])
# abc.multiply_parameters("eps_decay", [1000, 10000])
