#!/usr/bin/env bash

set -xe

# Kräver: libglfw3 libglfw3-dev

mkdir -p build
g++ -O2 -Wall -Wextra src/{common,common/window,simulation}/*.cpp -lglfw -lGL -o build/simulation
build/simulation
