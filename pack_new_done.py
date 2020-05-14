import pathlib
import os
import sys
from zipfile import ZipFile
import subprocess
import locks
import json


def pack_new_done():
    local = str(pathlib.Path.home())
    os.chdir(local + "/space_memory")
    local_path = local + "/space_memory/experiments/done/"
    with open(local+"/space_memory/tmp/currentfilelist.json","r") as f:
        files_already_there = json.load(f)

    locks.lock_done()
    local_files = subprocess.check_output(["find", local_path]).decode()
    locks.unlock_done()

    #FIXME codesmell
    local_files = local_files.replace(str(locks.lock_done_path()), "").replace(local_path, "").split("\n")
    local_files = list((filter(lambda x: x != "", local_files)))
    files = set(local_files) - set(files_already_there)

    print(len(files))
    if len(files) > 0:
        with ZipFile(local + "/space_memory/experiments/archives/done.zip","w") as zipFile:
            for file in files:
                zipFile.write("experiments/done/"+file)


if __name__ == '__main__':
    pack_new_done()



