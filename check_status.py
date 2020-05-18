import remote_config
import os


def check_status():
    for remote in remote_config.remotes:
        print(remote)
        os.system("ssh " + remote + " ls space_memory/experiments/queue | wc -l")
        print()


if __name__ == "__main__":
    check_status()