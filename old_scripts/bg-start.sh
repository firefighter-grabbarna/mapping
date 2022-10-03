#!/usr/bin/env bash

cd "$(dirname "$0")"

nohup ./run-robot.sh > /dev/null &
disown -a
