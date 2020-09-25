import remote_config
import os
import sys

def check_status():
    for remote in remote_config.remotes:
        sys.stdout.write(remote+" remaining experiments: ")
        sys.stdout.flush()
        os.system("ssh " + remote + " ls space_memory/experiments/queue | wc -l")

    for remote in remote_config.remotes:
        print(remote)
        os.system("ssh " + remote + " nvidia-smi")
        print()


if __name__ == "__main__":
    check_status()