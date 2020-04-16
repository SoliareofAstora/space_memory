import numpy as np
import template_factory
import copy


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
                Source_series = copy.deepcopy(self.series)
                self.series = []
                for v in range(len(change["values"])):
                    self.series.extend(copy.deepcopy(Source_series))
                    for i in range(v * old_len, (v + 1) * old_len):
                        self.series[i][change["name"]] = change["values"][v]
        return self.series

    def save(self):
        # TODO tworząc template w qqueue też trzeba zakładać locka
        params = self.prepare()
        for param in params:
            template = copy.deepcopy(self.template)
            template.params["series"] = {"changes": self.changes}
            template.params["series"]["name"] = self.name
            for key in param.keys():
                template.change(key, param[key])
            # def save(self, path="experiments/templates/", splitter="/", overwrite_folder_name=None, prefix="v"):
            template.save("experiments/queue/", "_", self.name, "_")


abc = SeriesFactory()
abc.create("/home/SoliareofAstora/space_memory/experiments/templates/stopping_rectangle_linear_dqn/v1", "second")

abc.multiply_parameters("batch_size", [32,64,128,256])
abc.multiply_parameters("memory_size",[500,1000,2000,4000])
abc.multiply_parameters("n",[2,5,10,50,100])
# abc.add_parameters("abb", [4, 5, 6])
# abc.multiply_parameters("stuff", ["a", "b", "c"])
# abc.add_parameters("TMP", [0,1,2,3,4,5,6,7,8])

tmp = abc.prepare()

abc.save()
