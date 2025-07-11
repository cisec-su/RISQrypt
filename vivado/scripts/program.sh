#!/bin/bash
# Yusuf Sur
# Created on: 11/07/2025
# -g generate bitstream and program FPGA
# -p program FPGA only

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

case "$1" in
    -g)
        vivado -mode batch -source "$SCRIPT_DIR/build.tcl"
        vivado -mode batch -source "$SCRIPT_DIR/program.tcl"
        ;;
    -p)
        vivado -mode batch -source "$SCRIPT_DIR/program.tcl"
        ;;
    *)
        echo "Usage: $0 -g   (generate bitstream + program)"
        echo "       $0 -p   (program FPGA only)"
        exit 1
        ;;
esac

rm -f "$SCRIPT_DIR"/*.jou "$SCRIPT_DIR"/*.log
