# Build Instructions

## Requirements

Linux based OS - Tested on Debian and variants including WSL & Raspberry Pi.

### Debian based OS

```
sudo apt-get install build-essential cmake git
git clone https://github.com/papawattu/msg-core
cd msg-core
mkdir build
cd build
cmake ..
make
sudo make install

cd ../../
git clone https://github.com/phev-remote/phevcore.git
cd phevcore
mkdir build
cd build
cmake ..
make
sudo make install
cd ../../

git clone https://github.com/DaveGamble/cJSON.git
cd cJSON
mkdir build
cd build
cmake ..
make
sudo make install
cd ../../

git clone https://github.com/phev-remote/phevctl.git
cd phevctl
mkdir build
cd build
cmake ..
make

./phevctl --help
```
