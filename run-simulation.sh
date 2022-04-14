#!/usr/bin/env bash

cd "$(dirname "$0")"

set -xe
make build/bin/simulation
build/bin/simulation
