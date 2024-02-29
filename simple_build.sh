#!/bin/bash

if [ $1 == "clean" ]; then
    cmake --build build/ --target clean
else
    cmake -S . -B build/ -DCMAKE_BUILD_TYPE=$1
    cmake --build build/
fi