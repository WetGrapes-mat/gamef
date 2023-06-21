#!/bin/zsh
rm -R build
mkdir build 
cd build 
# CXX=/usr/local/Cellar/gcc/13.1.0/bin/g++-13  cmake -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON ..
# cmake -DCMAKE_TOOLCHAIN_FILE=/usr/local/Cellar/mingw-w64/11.0.0/mingw/Toolchain-mingw.cmake -DCMAKE_PREFIX_PATH=/Users/matvejdubajlo/deps ..
# make

#  cp /usr/local/Cellar/mingw-w64/11.0.0/toolchain-x86_64/x86_64-w64-mingw32/lib/libstdc++-6.dll .
#  cp /usr/local/Cellar/mingw-w64/11.0.0/toolchain-x86_64/x86_64-w64-mingw32/lib/libgcc_s_seh-1.dll .
# cp /Users/matvejdubajlo/deps/bin/SDL3.dll .
# cp /usr/local/Cellar/mingw-w64/11.0.0/toolchain-x86_64/x86_64-w64-mingw32/bin/libwinpthread-1.dll .

cmake -DCMAKE_BUILD_TYPE=Debug ..
cmake  --build .
./game

