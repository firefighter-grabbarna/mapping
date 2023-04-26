#!/usr/bin/env bash

cd "$(dirname "$0")"

DEVICE="enp1s0"
#DEVICE=enx00e04c78bf20
#ADDR="fe80::bef4:5354:913b:fd01%$DEVICE" #rpi 2
#ADDR="fe80::3545:8d5:24f3:fe87%$DEVICE" #rpi 4 (old)
ADDR_ETH="fe80::abf6:20e3:c24d:2463%$DEVICE" #rpi 4 (new)
ADDR_WIFI="10.255.197.188" #rpi 4 (new)

function copy {
    rsync -rcpv --exclude={.git,breezyslam,build,data,resurser,src/canon,src/motor} \
        . "pi@[$ADDR_WIFI]":firefighter-grabbarna
}
function connect {
    ssh -Xt "pi@$ADDR_WIFI" $@
}

case $1 in
date)
    connect "sudo date -s $(date -Ins)"
    ;;
network)
    ssh -Xt "pi@$ADDR_ETH" "curl 'http://wifi.liu.se/cgi-bin/login' -X POST --data-raw 'accept_aup=accept_aup'"
    ;;
ifconfig)
    ssh -Xt "pi@$ADDR_ETH" "sudo ifconfig | grep -n1 'wlan0' | tail -n 1 | awk '{ print \$3 }'"
    ;;
compile)
    ./x.sh build-js
    cargo build --release --target armv7-unknown-linux-gnueabihf --bin robot || exit
    scp target/armv7-unknown-linux-gnueabihf/release/robot "pi@[$ADDR_WIFI]":firefighter-grabbarna
    connect "RUST_BACKTRACE=1 firefighter-grabbarna/robot"
    ;;
run)
    connect "RUST_BACKTRACE=1 firefighter-grabbarna/robot"
    ;;

# copy)
#     copy
#     ;;
eth-ssh)
    ssh -Xt "pi@$ADDR_ETH" 
    ;;
ssh)
    connect "cd firefighter-grabbarna; bash"
    
    ;;
# run)
#     copy
#     connect "firefighter-grabbarna/run-robot.sh"
#     ;;
# start)
#     copy
#     connect "firefighter-grabbarna/bg-start.sh"
#     ;;
# stop)
#     connect "firefighter-grabbarna/bg-stop.sh"
#     ;;
*)
    echo "Usage:"
    echo "$0 date   Sets the date to the current date"
    echo "$0 ssh    Opens a shell"
    ;;
esac
