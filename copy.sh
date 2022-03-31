#!/usr/bin/env bash

rsync -r . 'pi@[fe80::bef4:5354:913b:fd01%enp1s0]':firefighter-grabbarna
