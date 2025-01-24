`ifndef _DEFINES_
`define _DEFINES_

parameter ROM_START     =  32'h0000_0000;
parameter ROM_END       =  32'h0000_5FFF;
parameter RAM_START     =  32'h0000_6000;
parameter RAM_END       =  32'h0003_FFFF;
parameter MTIME_START   =  32'h0004_0000;
parameter MTIME_END     =  32'h0004_000F;
parameter DEBUG_I_START =  32'h0004_0010;
parameter DEBUG_I_END   =  32'h0004_0010;
parameter NTT_I_START =  32'h0004_0020;
parameter NTT_I_END   =  32'h0004_0040;

`endif