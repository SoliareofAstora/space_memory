import os
import pathlib
local = str(pathlib.Path.home())
os.chdir(local + "/space_memory/experiments")

folders = ["archives","done","in_progress","queue","templates"]
for folder in folders:
    os.system("rm -r "+folder)
    os.system("mkdir "+folder)
