import pathlib
import os
from zipfile import ZipFile
import locks


def unpack_queue():
    os.chdir(str(pathlib.Path.home()) + "/space_memory")
    locks.lock_queue()
    with ZipFile("experiments/archives/queue.zip", "r") as zipFile:
        zipFile.extractall()
    locks.unlock_queue()


if __name__ == '__main__':
    unpack_queue()
