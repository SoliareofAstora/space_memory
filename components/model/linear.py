import torch
import torch.nn as nn
import torch.nn.functional as F
from components.model.activations import get_activation_function


# create algotithm for multiple separated layers combinations. Its gonna be lists in config
def get_linear_block(input_size, output_size, activation=None, normalization=None, dropout=None):
    layers = [nn.Linear(input_size, output_size)]
    if activation is not None:
        layers.append(get_activation_function(activation))
    if normalization is not None:
        layers.append(nn.BatchNorm1d(output_size))
    if dropout is not None:
        layers.append(nn.Dropout(dropout))

    return nn.Sequential(*layers)


def create_linear_model(params):
    depth = params["depth"]
    width = params["width"]
    input_size = params["observation_space"]
    dropout_rate = params["dropout_rate"]
    normalization = params["normalization"]
    output_size = params["action_space"]
    activation = params["activation"]

    layers = []
    layers.append(get_linear_block(input_size, width, activation))

    for i in range(depth):
        layers.append(get_linear_block(width, width, activation, normalization, dropout_rate))
    # add output activation
    layers.append(get_linear_block(width, output_size))

    return nn.Sequential(*layers)
