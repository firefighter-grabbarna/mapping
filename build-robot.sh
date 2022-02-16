#!/usr/bin/env bash

set -xe

mkdir -p build
g++ -flto -std=c++17 -O3 src/{common,robot}/*.cpp -o build/robot
build/robot
