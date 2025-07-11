#!/bin/bash
# Yusuf Sur
# Created on: 11/07/2025

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

echo "Pick your choice:"
echo "0: Only program FPGA"
echo "1: Generate bitstream and program FPGA (must if first run)"
read -p "Your choice: " choice

if [ "$choice" == "0" ]; then
    vivado -mode batch -source "$SCRIPT_DIR/program.tcl"

elif [ "$choice" == "1" ]; then
    vivado -mode batch -source "./Hornet-FHE-vivado.tcl"
    vivado -mode batch -source "$SCRIPT_DIR/program.tcl"

else
    echo "Wrong choice. Abort"
    exit 1
fi

rm -f "$SCRIPT_DIR"/*.jou "$SCRIPT_DIR"/*.log
