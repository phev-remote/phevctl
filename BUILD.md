# Build Instructions

## Requirements

Unix based OS

- Tested on Debian and variants including Ubuntu, WSL & Raspberry Pi.
- Also tested on MacOS (Catalina).

### Debian based and Mac OS

Use the steps below, or have a look at [this repository](https://github.com/steady286/phevbuild).

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
