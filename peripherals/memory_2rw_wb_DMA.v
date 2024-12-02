
module memory_2rw_wb_DMA(
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

input         DMA_cyc_i,
input         DMA_stb_i,
input         DMA_we_i,
input [31:0]  DMA_adr_i,
input [31:0]  DMA_dat_i,
input [3:0]   DMA_sel_i,
output reg       DMA_stall_o,
output reg       DMA_ack_o,
output reg [31:0] DMA_dat_o,
output reg       DMA_err_o,
input         DMA_rst_i);


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

always @ (*)
begin
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
        
        port1_wb_stall_o = wb_stall_o;
        port1_wb_ack_o = wb_ack_o;
        port1_wb_dat_o = wb_dat_o;
        port1_wb_err_o = wb_err_o;
        
        DMA_stall_o = 1'b1;
        DMA_ack_o = 1'b0;
        DMA_dat_o = 1'b0;
        DMA_err_o = 1'b0;
    end
    else
    begin
        wb_cyc_i = DMA_cyc_i;
        wb_stb_i = DMA_stb_i;
        wb_we_i = DMA_we_i;
        wb_adr_i = DMA_adr_i;
        wb_dat_i = DMA_dat_i;
        wb_sel_i = DMA_sel_i;  
        wb_rst_i = DMA_rst_i;
        wb_clk_i = port1_wb_clk_i; 
        
        port1_wb_stall_o = 0;
        port1_wb_ack_o = 0;
        port1_wb_dat_o = 0;
        port1_wb_err_o = 0;
        
        DMA_stall_o = wb_stall_o;
        DMA_ack_o = wb_ack_o;
        DMA_dat_o = wb_dat_o;
        DMA_err_o = wb_err_o;
    end
end









memory_2rw_wb memory(
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
