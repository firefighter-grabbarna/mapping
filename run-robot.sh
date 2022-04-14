#!/usr/bin/env bash

cd "$(dirname "$0")"

set -xe
make build/bin/robot
build/bin/robot /dev/ttyACM*
