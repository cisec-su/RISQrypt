#!/bin/bash
#Yusuf Sur

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

vivado -mode batch -source "program.tcl"

#clean folder
rm -f "$SCRIPT_DIR"/*.jou "$SCRIPT_DIR"/*.log
