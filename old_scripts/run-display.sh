#!/usr/bin/env bash

cd "$(dirname "$0")"

set -e

echo "Compiling"
make build/bin/display

echo "Running"
build/bin/display /dev/ttyACM*
