#!/bin/env bash

mkdir -p build && cd build
conan install .. --output-folder=. --build=missing
cmake ..  -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_COMPILER=clang++ --preset conan-debug
#
# -DCMAKE_C_COMPILER=clang 
# -DCMAKE_TOOLCHAIN_FILE=build/conan_toolchain.cmake
# -G "Visual Studio 17 2022" -A x64 
#
cmake --build .
