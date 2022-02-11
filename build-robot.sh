#!/usr/bin/env bash

set -xe

mkdir -p build
g++ -std=c++17 -O2 src/{common,robot}/*.cpp -o build/robot
build/robot
