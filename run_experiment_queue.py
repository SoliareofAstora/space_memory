import pathlib
import shutil
import os
from run_experiment import run_experiment
import time
import locks

def run_queue():
    queue = pathlib.Path("experiments/queue")
    lock = queue/"lock"
    while len(list(queue.glob("*"))) > 0:
        locks.lock_queue()

        runs = set(queue.glob("*")) - {lock}
        if len(runs) == 0:
            os.rmdir(lock)
            return

        run = list(runs)[0]
        run_folder_name = run.name + "_" + str(int(time.time()))
        current_run = shutil.move(str(run), "experiments/in_progress/"+run_folder_name, "-r")
        locks.unlock_queue()

        run_experiment(current_run)


if __name__ == "__main__":
    run_queue()
