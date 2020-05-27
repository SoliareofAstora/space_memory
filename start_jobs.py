import os
import remote_config


def spawn_job():
    for remote in remote_config.remotes:
        os.system("ssh " + remote + " ./space_memory/runq.sh")


if __name__ == "__main__":
    spawn_job()
