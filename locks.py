import pathlib
import time
import os


def lock_queue_path():
    return pathlib.Path("experiments/queue/lock")


def lock_queue():
    lock = lock_queue_path()
    while lock.exists():
        time.sleep(1)
    os.mkdir(lock)


def unlock_queue():
    os.rmdir(lock_queue_path())
