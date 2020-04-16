import torch.nn as nn

def get_activation_function(name):
    if name is None or name == "identity":
        return nn.Identity()
    if name == "relu":
        return nn.ReLU()
    if name == "relu6":
        return nn.ReLU6()
    if name == "lrelu":
        return nn.LeakyReLU()
    if name == "prelu":
        return nn.PReLU()
    if name == "selu":
        return nn.SELU()
    if name == "celu":
        return nn.CELU()
    if name == "softmax":
        return nn.Softmax(dim=0)
