#!/usr/bin/env bash

cd "$(dirname "$0")"

DEVICE="enp1s0"
#DEVICE=enx00e04c78bf20
#ADDR="fe80::bef4:5354:913b:fd01%$DEVICE" #rpi 2
ADDR="fe80::3545:8d5:24f3:fe87%$DEVICE" #rpi 4

function copy {
    rsync -rcpv --exclude={.git,breezyslam,build,data,resurser,src/canon,src/motor} \
        . "pi@[$ADDR]":firefighter-grabbarna
}
function connect {
    ssh -Xt "pi@$ADDR" $@
}

case $1 in
date)
    connect "sudo date -s $(date -Ins)"
    ;;
copy)
    copy
    ;;
ssh)
    connect "cd firefighter-grabbarna; bash"
    ;;
run)
    copy
    connect "firefighter-grabbarna/run-robot.sh"
    ;;
start)
    copy
    connect "firefighter-grabbarna/bg-start.sh"
    ;;
stop)
    connect "firefighter-grabbarna/bg-stop.sh"
    ;;
*)
    echo "Usage:"
    echo "$0 date   Sets the date to the current date"
    echo "$0 copy   Copies the source files"
    echo "$0 ssh    Opens a shell"
    echo "$0 run    Copies the source files, compiles them and starts the program"
    ;;
esac
