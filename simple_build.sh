#!/bin/bash

if [ $1 == "clean" ]; then
    cmake --build build/ --target clean
elif [ $1 == "Debug" ] || [ $1 == "Release" ]; then
    cmake -S . -B build/ -DCMAKE_BUILD_TYPE=$1
    cmake --build build/
else
    echo "Provided command '$1' is not found"
fi