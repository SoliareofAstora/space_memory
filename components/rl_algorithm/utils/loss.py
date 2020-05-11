import torch.nn.functional as F

def get_loss(params):
    if params["loss_name"] == "smooth_l1_loss":
        return F.smooth_l1_loss