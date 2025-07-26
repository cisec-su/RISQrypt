#!/bin/bash


SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

case "$1" in
    -g)
        vivado -mode batch -source "$SCRIPT_DIR/build.tcl"
        ;;
    -p)
        vivado -mode batch -source "$SCRIPT_DIR/program.tcl"
        ;;
    *)
        echo "Usage: $0 -g   (generate bitstream)"
        echo "       $0 -p   (program FPGA only)"
        exit 1
        ;;
esac

rm -f "$SCRIPT_DIR"/*.jou "$SCRIPT_DIR"/*.log
