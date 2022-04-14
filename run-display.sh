#!/usr/bin/env bash

cd "$(dirname "$0")"

set -xe
make build/bin/display
build/bin/display
