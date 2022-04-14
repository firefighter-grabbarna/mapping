#!/usr/bin/env bash

rsync -av --exclude={".*",data,resurser,src/canon,src/motor} . 'pi@[fe80::bef4:5354:913b:fd01%enx00e04c78bf20]':firefighter-grabbarna
