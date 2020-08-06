import pathlib
import time
import os


def lock_path(path):
    lock = pathlib.Path(path/"lock")
    while lock.exists():
        time.sleep(1)
    os.mkdir(lock)


def unlock_path(path):
    os.rmdir(pathlib.Path(path/"lock"))


def lock_queue_path():
    return pathlib.Path("experiments/queue/lock")


def lock_queue():
    lock = lock_queue_path()
    while lock.exists():
        time.sleep(1)
    os.mkdir(lock)


def unlock_queue():
    os.rmdir(lock_queue_path())


def lock_done_path():
    return pathlib.Path("experiments/done/lock")


def lock_done():
    lock = lock_done_path()
    while lock.exists():
        time.sleep(1)
    os.mkdir(lock)


def unlock_done():
    os.rmdir(lock_done_path())
