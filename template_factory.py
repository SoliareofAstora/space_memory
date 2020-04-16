import json
import os
import pathlib


class TemplateFactory(object):
    def __init__(self):
        self.params = None

    def name(self):
        if self.params is None:
            print("Trying to access name of empty template. Load or create template first.")
        if "name" in self.params["general"].keys():
            return self.params["general"]["name"]
        return self.params["environment"]['env_source'] + "_" + self.params["model"]['model_source'] + "_" + \
               self.params["rl_algorithm"]['rl_source']

    def rename(self, new_name):
        self.params["general"]["name"] = new_name

    def create(self, env, model, rl_algorithm, custom_name=None, training="default", ships="default_ships"):
        self.params = {"general": {}}
        try:
            with open("parameters/environment/scenarios/" + env + ".json", "r") as f:
                self.params["environment"] = json.load(f)
        except IOError:
            print("Missing environment: " + env)
        try:
            with open("parameters/environment/ship_statistics/" + ships + ".json", "r") as f:
                self.params["environment"].update(json.load(f))
        except IOError:
            print("Missing ships: " + ships)
        with open("parameters/environment/universal.json", "r") as f:
            self.params["environment"].update(json.load(f))
        self.params["environment"]['env_source'] = env
        self.params["environment"]['ships_source'] = ships

        try:
            with open("parameters/model/" + model + ".json", "r") as f:
                self.params["model"] = json.load(f)
        except IOError:
            print("Missing model: " + model)
        self.params["model"]['model_source'] = model

        try:
            with open("parameters/rl_algorithm/" + rl_algorithm + ".json", "r") as f:
                self.params["rl_algorithm"] = json.load(f)
        except IOError:
            print("Missing rl_algorithm: " + rl_algorithm)
        self.params["rl_algorithm"]['rl_source'] = rl_algorithm

        try:
            with open("parameters/training/" + training + ".json", "r") as f:
                self.params["training"] = json.load(f)
        except IOError:
            print("Missing training: " + env)
        self.params["training"]['training_source'] = training

        if custom_name is not None:
            self.params["general"]["name"] = custom_name
        else:
            self.params["general"]["name"] = self.name()

    def load(self, path):
        self.params = {}
        files = list(pathlib.Path(path).glob("parameters/*.json"))
        for file in files:
            with open(str(file), "r") as f:
                self.params[file.stem] = json.load(f)
        return self.params

    def change(self, name, value, default="other"):
        for file in self.params.keys():
            if name in self.params[file].keys():
                self.params[file][name] = value
                return
        if default not in self.params.keys():
            self.params[default] = {}
        print("Parameter",name,"not found")
        self.params[default][name] = value

    def save(self, path="experiments/templates/", splitter="/", overwrite_folder_name=None, prefix="v"):
        if overwrite_folder_name is None:
            folder_name = self.name()
        else:
            folder_name = overwrite_folder_name
        if self.params is None:
            print("Trying to save empty template. Load or create template first.")
            return
        version = 0
        complete_string = json.dumps(self.params, sort_keys=True)
        template_name = folder_name + splitter + prefix + str(version)

        if splitter == "/":
            if not os.path.exists(path + folder_name):
                os.mkdir(path + folder_name)

        while os.path.exists(path + template_name):
            other_params = TemplateFactory().load(path + template_name)
            if json.dumps(other_params, sort_keys=True) == complete_string:
                print("This template already exists", template_name)
                return
            version += 1
            template_name = folder_name + splitter + prefix + str(version)
        os.mkdir(path + template_name)
        os.mkdir(path + template_name + "/parameters")
        for key in self.params.keys():
            string = json.dumps(self.params[key]).replace(', "', ', \n"')
            with open(path + template_name + "/parameters/" + key + ".json", "w")as f:
                f.writelines(string)

# tmp = TemplateFactory()
# tmp.create("stopping",'rectangle_linear','dqn')
# tmp.save()
