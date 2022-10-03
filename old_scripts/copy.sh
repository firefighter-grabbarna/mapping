#!/usr/bin/env bash

cd "$(dirname "$0")"

rsync -rcpv --exclude={.git,breezyslam,data,resurser,src/canon,src/motor} . 'pi@[fe80::bef4:5354:913b:fd01%enp1s0]':firefighter-grabbarna
