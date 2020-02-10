#!/usr/bin/bash

apt-get -y install cmake

cd ~/src

git clone https://github.com/jpcunningh/tinyb.git
cd tinyb; mkdir build; cd build
cmake ..
make

cd ~/Lookout/transmitterWorker; mkdir build; cd build
cmake ..
make


