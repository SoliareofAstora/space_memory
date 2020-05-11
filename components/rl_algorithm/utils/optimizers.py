import torch

def get_optimizer(params, network_weights):
    if params["optimizer"] == "RMSprop":
        return torch.optim.RMSprop(network_weights, params["lr"])