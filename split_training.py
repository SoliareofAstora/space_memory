import pathlib
import os
import subprocess
import sys
import random
from zipfile import ZipFile

remotes = [
    # "neptune",
    "walter"
]


def split_training():
    os.chdir(str(pathlib.Path.home())+"/space_memory")
    all_connected = True
    for remote in remotes:
        ssh = subprocess.Popen(["ssh", remote, "uname -a"],
                               shell=False,
                               stdout=subprocess.PIPE,
                               stderr=subprocess.PIPE)
        result = ssh.stdout.readlines()
        if result == []:
            error = ssh.stderr.readlines()
            all_connected = False
            print ("ERROR: %s" % error)
        else:
            print (result)

    if not all_connected:
        print("Error while connecting to remote hosts, aborting.")
        # return 1

    local_queue = list(pathlib.Path("experiments/queue").glob("*"))
    queue_size = len(local_queue)

    if queue_size > 0:
        random.shuffle(local_queue)
        step = int(queue_size/len(remotes))
        indexList = [[i + j * step for i in range(0, step)] for j in range(len(remotes))]

        if queue_size - indexList[-1][-1] > 1:
            indexList[-1].extend([i for i in range(indexList[-1][-1]+1,len(local_queue))])

        for i in range(len(remotes)):
            remote = remotes[i]
            print("Preparing zip for", remote)
            with ZipFile("experiments/archives/"+remote+".zip","w") as zipFile:
                for j in indexList[i]:
                    experiment = local_queue[j]
                    for file in list(experiment.glob("*/*")):
                        zip.write(file)

        for remote in remotes:
            print("Moving archive to", remote)
            os.system("scp experiments/archives/"+remote+".zip "
                      +remote+":~/space_memory/experiments/archives/queue.zip")

        for remote in remotes:
            print("Unpacking archive to", remote)
            os.system("ssh "+remote+" python space_memory/unpack_queue.py")


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