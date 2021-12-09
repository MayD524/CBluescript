#!/bin/bash

current_dir=$(pwd)
echo "Cleaning up old files"
rm ./cbs
echo "Compiling..."
g++ -std=c++17 -g -static -o3 -o cbs *.cpp
#g++ -std=c++17 -static -o cbs *.cpp


