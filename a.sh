#!/bin/zsh
rm -R build
mkdir build 
cd build 
cmake -DCMAKE_BUILD_TYPE=Debug ..
cmake  --build .
./game
