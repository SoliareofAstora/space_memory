import pathlib
import os
import json
from zipfile import ZipFile
import subprocess
import remote_config
import time


def synchronize_done():
    if not remote_config.all_connected():
        print("Error while connecting to remote hosts, aborting.")
        return 1

    local = str(pathlib.Path.home())
    os.chdir(local + "/space_memory")

    local_path = local + "/space_memory/experiments/done/"
    local_files = subprocess.check_output(["find", local_path]).decode()
    local_files = local_files.replace(local_path, "").split("\n")
    local_files = list((filter(lambda x: x != "", local_files)))
    try:
        os.mkdir(local + "/space_memory/tmp",)
    except Exception:
        pass
    with open(local + "/space_memory/tmp/currentfilelist.json", "w") as f:
        json.dump(local_files, f)

    for remote in remote_config.remotes:
        print("processing", remote)
        remote_home = subprocess.check_output(["ssh", remote, "echo", "$HOME"]).decode()[:-1]
        try:
            os.system('ssh '+remote+" mkdir "+remote_home + "/space_memory/tmp")
        except Exception:
            pass
        os.system("scp " + local + "/space_memory/tmp/currentfilelist.json " +
                  remote + ":" + remote_home + "/space_memory/tmp/currentfilelist.json")
        tmp = subprocess.check_output(
            ["ssh", remote, "python3", remote_home + "/space_memory/pack_new_done.py"])
        os.system('ssh '+remote+" rm -r "+remote_home + "/space_memory/tmp")
        n_new_files = int(tmp.decode().split("\n")[0])
        print("found",n_new_files,"new files".format())
        if n_new_files > 0:
            fname = remote + str(int(time.time()))
            os.system("scp " + remote + ":" + remote_home + "/space_memory/experiments/archives/done.zip " +
                      local + "/space_memory/experiments/archives/" + fname + ".zip")
            with ZipFile(local + "/space_memory/experiments/archives/" + fname + ".zip", "r") as zipFile:
                zipFile.extractall()
    os.system("rm -r "+local + "/space_memory/tmp")


if __name__ == '__main__':
    synchronize_done()

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
