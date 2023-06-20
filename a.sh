#!/bin/zsh
# rm -R build
# mkdir build 
cd build 
# CXX=/usr/local/Cellar/gcc/13.1.0/bin/g++-13  cmake -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON ..
# CXX=/usr/local/Cellar/mingw-w64/11.0.0/bin/x86_64-w64-mingw32-g++ cmake  ..

cmake -DCMAKE_BUILD_TYPE=Debug ..
cmake  --build .
./game
