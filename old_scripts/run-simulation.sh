#!/usr/bin/env bash

cd "$(dirname "$0")"

set -e

echo "Compiling"
make build/bin/simulation

echo "Running"
build/bin/simulation
