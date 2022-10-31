#!/bin/sh

set -e

cd "$(dirname "$0")"

script="./$(basename "$0")"

case $1 in
    build-js)
        esbuild --bundle display/main.js --loader:.js=jsx --jsx-factory=h --outfile=display/static/index.js
        ;;
    run)
        $script build-js
        cargo run --bin simulation
        ;;
    watch)
        cargo watch -s "$script run"
        ;;
    fmt)
        cargo fmt
        ;;
    *)
        echo "Usage:"
        echo "$0 run"
        echo "$0 watch"
        echo "$0 fmt"
        ;;
esac
