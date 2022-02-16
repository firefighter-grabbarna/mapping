#!/usr/bin/env bash

set -xe

# Kräver: libglfw3 libglfw3-dev

mkdir -p build
g++ -flto -std=c++17 -O3 -Wall -Wextra src/{common,common/window,display}/*.cpp -lglfw -lGL -o build/display
build/display
