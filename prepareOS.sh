sudo apt-get install libsfml-dev
sudo apt-get install libboost-all-dev

wget https://repo.anaconda.com/miniconda/Miniconda3-latest-Linux-x86_64.sh
chmod +x Miniconda3-latest-Linux-x86_64.sh
./Miniconda3-latest-Linux-x86_64.sh

source .bashrc
conda env create -f ./space_memory/conda_enviro.yaml

