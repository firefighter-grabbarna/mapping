#!/usr/bin/env bash

rsync -av --exclude={".*",data,resurser,src/canon,src/motor} . 'pi@[fe80::bef4:5354:913b:fd01%enp1s0]':firefighter-grabbarna
