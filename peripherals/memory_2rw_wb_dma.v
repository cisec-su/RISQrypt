
module memory_2rw_wb_dma(
input         port0_wb_cyc_i,
input         port0_wb_stb_i,
input         port0_wb_we_i,
input [31:0]  port0_wb_adr_i,
input [31:0]  port0_wb_dat_i,
input [3:0]   port0_wb_sel_i,
output        port0_wb_stall_o,
output        port0_wb_ack_o,
output  [31:0] port0_wb_dat_o,
output        port0_wb_err_o,
input         port0_wb_rst_i,
input         port0_wb_clk_i,

input         port1_wb_cyc_i,
input         port1_wb_stb_i,
input         port1_wb_we_i,
input [31:0]  port1_wb_adr_i,
input [31:0]  port1_wb_dat_i,
input [3:0]   port1_wb_sel_i,
output reg       port1_wb_stall_o,
output reg       port1_wb_ack_o,
output reg [31:0] port1_wb_dat_o,
output reg       port1_wb_err_o,
input         port1_wb_rst_i,
input         port1_wb_clk_i,

input         dma_cyc_i,
input         dma_stb_i,
input         dma_we_i,
input [31:0]  dma_adr_i,
input [31:0]  dma_dat_i,
input [3:0]   dma_sel_i,
output reg       dma_stall_o,
output reg       dma_ack_o,
output reg [31:0] dma_dat_o,
output reg       dma_err_o,
input         dma_rst_i);


parameter FPGA_READMEM = 1 ;
parameter NUM_WMASKS = 4 ;
parameter DATA_WIDTH = 32 ;
parameter ADDR_WIDTH = 18 ;
parameter RAM_DEPTH = 1 << ADDR_WIDTH;
parameter ROM_START = 16'h7400;


reg         wb_cyc_i;
reg         wb_stb_i;
reg         wb_we_i;
reg [31:0]  wb_adr_i;
reg [31:0]  wb_dat_i;
reg [3:0]   wb_sel_i;
reg         wb_rst_i;
reg         wb_clk_i;

wire         wb_stall_o;
wire         wb_ack_o;
wire  [31:0] wb_dat_o;
wire         wb_err_o;

reg stb;

always @ (*)
begin
    if(stb)
    begin
        port1_wb_stall_o = wb_stall_o;
        port1_wb_ack_o = wb_ack_o;
        port1_wb_dat_o = wb_dat_o;
        port1_wb_err_o = wb_err_o;
        dma_stall_o = 1'b1;
        dma_ack_o = 1'b0;
        dma_dat_o = 1'b0;
        dma_err_o = 1'b0;
    end
    else
    begin
        port1_wb_stall_o = wb_stall_o;
        port1_wb_ack_o = wb_ack_o;
        port1_wb_dat_o = wb_dat_o;
        port1_wb_err_o = wb_err_o;
        dma_stall_o = wb_stall_o;
        dma_ack_o = wb_ack_o;
        dma_dat_o = wb_dat_o;
        dma_err_o = wb_err_o;
    end
    
        
    if(port1_wb_stb_i)
    begin
        wb_cyc_i = port1_wb_cyc_i;
        wb_stb_i = port1_wb_stb_i;
        wb_we_i = port1_wb_we_i;
        wb_adr_i = port1_wb_adr_i;
        wb_dat_i = port1_wb_dat_i;
        wb_sel_i = port1_wb_sel_i;  
        wb_rst_i = port1_wb_rst_i;
        wb_clk_i = port1_wb_clk_i; 

    end
    else
    begin
        wb_cyc_i = dma_cyc_i;
        wb_stb_i = dma_stb_i;
        wb_we_i = dma_we_i;
        wb_adr_i = dma_adr_i;
        wb_dat_i = dma_dat_i;
        wb_sel_i = dma_sel_i;  
        wb_rst_i = dma_rst_i;
        wb_clk_i = port1_wb_clk_i; 
        
        
    end
end

always @(posedge port0_wb_clk_i or posedge port0_wb_rst_i)
begin
    if(port0_wb_rst_i)
        stb <= 0;
    else 
        stb <= port1_wb_stb_i;
end








memory_2rw_wb #(
        .ADDR_WIDTH(ADDR_WIDTH),
        .DATA_WIDTH(DATA_WIDTH),
        .NUM_WMASKS(NUM_WMASKS),
        .FPGA_READMEM(FPGA_READMEM),
        .RAM_DEPTH(RAM_DEPTH),
        .ROM_START(ROM_START)
    ) memory (
        .port0_wb_cyc_i(port0_wb_cyc_i),
        .port0_wb_stb_i(port0_wb_stb_i),
        .port0_wb_we_i(port0_wb_we_i),
        .port0_wb_adr_i(port0_wb_adr_i),
        .port0_wb_dat_i(port0_wb_dat_i),
        .port0_wb_sel_i(port0_wb_sel_i),
        .port0_wb_stall_o(port0_wb_stall_o),
        .port0_wb_ack_o(port0_wb_ack_o),
        .port0_wb_dat_o(port0_wb_dat_o),
        .port0_wb_err_o(port0_wb_err_o),
        .port0_wb_rst_i(port0_wb_rst_i),
        .port0_wb_clk_i(port0_wb_clk_i),

        .port1_wb_cyc_i(wb_cyc_i),
        .port1_wb_stb_i(wb_stb_i),
        .port1_wb_we_i(wb_we_i),
        .port1_wb_adr_i(wb_adr_i),
        .port1_wb_dat_i(wb_dat_i),
        .port1_wb_sel_i(wb_sel_i),
        .port1_wb_stall_o(wb_stall_o),
        .port1_wb_ack_o(wb_ack_o),
        .port1_wb_dat_o(wb_dat_o),
        .port1_wb_err_o(wb_err_o),
        .port1_wb_rst_i(wb_rst_i),
        .port1_wb_clk_i(wb_clk_i)
);

endmodule
