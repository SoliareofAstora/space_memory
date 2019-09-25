def get_env(name, n):
    if name == 'space_enviro':
        import space_enviro.spaceLib
        return space_enviro.spaceLib.initialize(n)
    else:
        import rl_utils.gym_wrapper
        return rl_utils.gym_wrapper.parallel_envs(name, n)