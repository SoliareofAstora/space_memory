import pathlib
import os
import subprocess
import sys
import random
from zipfile import ZipFile
import locks


def unpack_queue():
    locks.lock_queue()
    os.chdir(str(pathlib.Path.home()) + "/space_memory")
    with ZipFile("experiments/archives/queue.zip","r") as zipFile:
        zip.extractall()
    locks.unlock_queue()


if __name__ == '__main__':
    unpack_queue()