#!/usr/bin/env bash

cd "$(dirname "$0")"

shopt -s nullglob
set -e

echo "Compiling"
make build/bin/robot build/bin/stop

# Stop the motors when ctrl-c is pressed
trap "echo; echo Stopping; build/bin/stop /dev/ttyACM*" EXIT

echo "Running"
build/bin/robot /dev/tty{USB,ACM}*
