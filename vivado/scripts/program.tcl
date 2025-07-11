# Vivado TCL Script for automatic FPGA bitstream upload.
# Yusuf Sur

set impl_dir "../hornet_fhe.runs/impl_1"
set bitstream_file "$impl_dir/fpga_top.bit"

open_hw_manager
connect_hw_server
open_hw_target

current_hw_device [lindex [get_hw_devices] 0]

refresh_hw_device [current_hw_device]
set_property PROGRAM.FILE $bitstream_file [current_hw_device]
program_hw_devices [current_hw_device]

close_hw_target
disconnect_hw_server
close_hw_manager
