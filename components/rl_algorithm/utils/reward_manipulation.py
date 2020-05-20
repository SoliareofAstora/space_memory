def identity(reward):
    return reward


def no_negative_reward(reward):
    reward[reward<0] = 0
    return reward


def load_reward_manipulation(params):
    if "reward_manipulation" not in params.keys():
        return identity

    if params["reward_manipulation"] == "identity":
        return identity

    if params["reward_manipulation"] == "no_negative_reward":
        return no_negative_reward
