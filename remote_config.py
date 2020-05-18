import subprocess

remotes = ["walter",
           # "neptune",
           "neptune_local"
           ]

def all_connected():
    all = True
    for remote in remotes:
        ssh = subprocess.Popen(["ssh", remote, "uname -a"],
                               shell=False,
                               stdout=subprocess.PIPE,
                               stderr=subprocess.PIPE)
        result = ssh.stdout.readlines()
        if result == []:
            error = ssh.stderr.readlines()
            all = False
            print("ERROR: %s" % error)
        else:
            print(result)

    return all

def get_remotes():
    return remotes
