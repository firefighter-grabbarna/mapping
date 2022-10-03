#!/usr/bin/env bash

cd "$(dirname "$0")"

shopt -s nullglob
set -e

echo "Compiling"
make build/bin/robot

echo "Running"
build/bin/robot /dev/ttyACM*
