`timescale 1ns/1ps

module ntt_acc_wb
                        (
                          input         wb_cyc_i,
                          input         wb_stb_i,
                          input         wb_we_i,
                          input [31:0]  wb_adr_i,
                          input [31:0]  wb_dat_i,
                          input [3:0]   wb_sel_i,
                          output        wb_stall_o,
                          output        wb_ack_o,
                          output reg [31:0] wb_dat_o,
                          output        wb_err_o,
                          input         wb_rst_i,
                          input         wb_clk_i);


parameter HP = 5;
parameter FP = (2*HP);
parameter B = 32;
parameter W = 64;
parameter N = 64;
parameter L = 11;

wire clk, rst;
assign clk = wb_clk_i;
assign rst = ~wb_rst_i;

reg stb, we;
reg [3:0] sel;
reg [31:0] adr,dat;

assign wb_err_o = 1'b0;
assign wb_stall_o = 1'b0;
//assign wb_stall_o = 1'b0;
assign wb_ack_o = stb & wb_cyc_i;

reg [2:0] cmd;
reg [1:0] mode;
reg [31:0] i_data;
reg i_valid;

wire busy;
wire o_valid;
wire [31:0] o_data;

small_ntt #(
    .N(N),
    .W(W),
    .B(B),
    .L(L)
) small_ntt_inst (
    .clk(clk),
    .rst(~rst),
    .cmd(cmd),
    .i_data(i_data),
    .i_valid(i_valid),
    .o_data(o_data),
    .o_valid(o_valid),
    .busy(busy),
    .mode(mode)
);

//input registers
always @(posedge clk or negedge rst)
begin
    if(!rst)
    begin
        {stb,we,sel,adr,dat} <= 70'b0;
    end
    else
    begin
        stb <= wb_stb_i;
        we <= wb_we_i;
        sel <= wb_sel_i;
        adr <= wb_adr_i;
        dat <= wb_dat_i;
    end
end



parameter [31:0] ADDR_LOAD_Q =          32'h00040020;
parameter [31:0] ADDR_LOAD_TWIDDLE =    32'h00040024;
parameter [31:0] ADDR_LOAD_POLY =       32'h00040028;
parameter [31:0] ADDR_RST =             32'h0004002C;
parameter [31:0] ADDR_LOAD_CMD =        32'h00040030;
parameter [31:0] ADDR_CHECK_STATUS =    32'h00040034;
parameter [31:0] ADDR_READ_RESULT =     32'h00040038;
parameter [31:0] ADDR_LOAD_MODE =       32'h0004003C;

always @ (posedge clk or negedge rst)
begin
    if(!rst)
    begin
        cmd <= 3'b0;
        mode <= 2'd0;
    end
    else if (wb_cyc_i && stb && we)
    begin
        case(adr)
            ADDR_LOAD_Q:
            begin
                cmd <= 3'd1;
                i_valid <= 1'b1;
                i_data <= dat;
            end
            ADDR_LOAD_TWIDDLE:
            begin
                cmd <= 3'd2;
                i_valid <= 1'b1;
                i_data <= dat;
            end
            ADDR_LOAD_POLY:
            begin
                cmd <= 3'd3;
                i_valid <= 1'b1;
                i_data <= dat;
            end
            ADDR_LOAD_MODE:
                mode <= dat[1:0];   
        endcase
    
    end
    else if(stb && (adr==ADDR_READ_RESULT))
        cmd <= 3'd5;
    else
    begin
        cmd <= 0;
        i_valid <= 1'b0;
        i_data <= 0;
    end
end

always @ (*)
begin
    if(adr == ADDR_CHECK_STATUS)
        wb_dat_o = {31'd0,o_valid};
    else if(adr == ADDR_READ_RESULT)
        wb_dat_o = o_data;
    else 
        wb_dat_o = 0;
end


endmodule
