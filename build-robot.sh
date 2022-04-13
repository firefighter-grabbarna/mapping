#!/usr/bin/env bash

set -xe
shopt -s nullglob

mkdir -p build
g++ -flto -O3 -std=c++17 src/{common,robot}/*.cpp -o build/robot
build/robot /dev/tty{ACM,USB}*
