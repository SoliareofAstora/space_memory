import os
import remote_config


def spawn_job():
    for remote in remote_config.remotes:
        command = "ssh " + remote + " bash space_memory/runq.sh"
        print(command)
        os.system(command)


if __name__ == "__main__":
    spawn_job()
