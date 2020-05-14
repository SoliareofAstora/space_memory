import pathlib
import os
import subprocess
import sys
import random
from zipfile import ZipFile
import remote_config


def split_training():
    os.chdir(str(pathlib.Path.home()) + "/space_memory")
    if not remote_config.all_connected():
        print("Error while connecting to remote hosts, aborting.")
        return 1

    local_queue = list(pathlib.Path("experiments/queue").glob("*"))
    queue_size = len(local_queue)

    if queue_size == 0:
        print("Queue is empty")
        return

    random.shuffle(local_queue)
    step = int(queue_size / len(remote_config.remotes))
    indexList = [[i + j * step for i in range(0, step)] for j in range(len(remote_config.remotes))]

    if queue_size - indexList[-1][-1] > 1:
        indexList[-1].extend([i for i in range(indexList[-1][-1] + 1, len(local_queue))])

    for i in range(len(remote_config.remotes)):
        remote = remote_config.remotes[i]
        print("Preparing zip for", remote)
        with ZipFile("experiments/archives/" + remote + ".zip", "w") as zipFile:
            for j in indexList[i]:
                experiment = local_queue[j]
                for file in list(experiment.glob("*/*")):
                    zipFile.write(file)

    for remote in remote_config.remotes:
        print("Moving archive to", remote)
        os.system("scp experiments/archives/" + remote + ".zip "
                  + remote + ":~/space_memory/experiments/archives/queue.zip")

    for remote in remote_config.remotes:
        print("Unpacking archive to", remote)
        os.system("ssh " + remote + " python3 space_memory/unpack_queue.py")

    #todo clear queue
if __name__ == "__main__":
    split_training()

# import subprocess
# import os
# import pathlib
# import tqdm
#
# tmp = subprocess.check_output(["ssh","neptune","ls","space_memory/experiments/queue/"])
# tmp = tmp.decode().split("\n")
#
# for t in tqdm.tqdm(tmp[:15]):
#     p = pathlib.Path(t)
#     p=(p.home()/p)
#     if not p.exists():
#         p.mkdir(parents=True)
#     os.system("scp -r neptune:~/space_memory/experiments/queue/"+t+" ~/space_memory/experiments/queue/"+t)
#     os.system('ssh neptune "rm -r ~/space_memory/experiments/queue/'+t+'"')
#
# zip -r package.zip . -i done/*.{json,txt}
