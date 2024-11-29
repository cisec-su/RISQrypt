`ifndef _DEFINES_
`define _DEFINES_

parameter ROM_START     =  32'h0000_0000;
parameter ROM_END       =  32'h0000_3FFF;
parameter RAM_START     =  32'h0000_4000;
parameter RAM_END       =  32'h0000_FFFF;
parameter MTIME_START   =  32'h0001_0000;
parameter MTIME_END     =  32'h0001_000F;
parameter DEBUG_I_START =  32'h0001_0010;
parameter DEBUG_I_END   =  32'h0001_0010;

`endif