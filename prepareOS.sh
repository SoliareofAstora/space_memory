sudo apt-get -y install libsfml-dev libboost-python-dev libboost-numpy-dev

wget https://repo.anaconda.com/miniconda/Miniconda3-latest-Linux-x86_64.sh
chmod +x Miniconda3-latest-Linux-x86_64.sh
./Miniconda3-latest-Linux-x86_64.sh

source ~/.bashrc
conda env create -f ./space_memory/conda_enviro.yaml

