`timescale 1ns / 1ps

module small_ntt_tb();

`include "small_ntt_cmd.vh"

parameter HP = 5;
parameter FP = (2*HP);
parameter B = 32;
parameter W = 64;
parameter N = 16;
parameter L = 11;

localparam LOGWB = $clog2(W/B);
localparam WBMASK = (1 << LOGWB) - 1;

reg clk;
reg rst;

reg [W-1:0] q [0:0];
reg [W-1:0] psi      [0:N-2];
reg [W-1:0] poly_in  [0:N-1];
reg [W-1:0] poly_out [0:N-1];

reg  [2  :0] cmd;
reg  [B-1:0] i_data;
reg          i_valid;
wire [B-1:0] o_data;
wire         o_valid;
wire         busy;

integer i, j;

always #HP clk = ~clk;


/*

module small_ntt
   #(   
        parameter N    = 16, // fixed for now
        parameter W    = 64, // word size, fixed for now
        parameter B    = 32, // bandwidth, must divide W
        parameter L    = 9   // BU latency
    )
    (
        input              clk,
        input              rst,
        input      [2  :0] cmd,
        input      [B-1:0] i_data,
        input              i_valid,
        output reg [B-1:0] o_data,
        output reg         o_valid,
        output reg         busy
    );
*/

small_ntt #(
    .N(N),
    .W(W),
    .B(B),
    .L(L)
) small_ntt_inst (
    .clk(clk),
    .rst(rst),
    .cmd(cmd),
    .i_data(i_data),
    .i_valid(i_valid),
    .o_data(o_data),
    .o_valid(o_valid),
    .busy(busy)
);

initial begin

    $readmemh("../../../../../test/q.txt" , q);
    $readmemh("../../../../../test/psi.txt" , psi);
    $readmemh("../../../../../test/poly_in.txt" , poly_in);
    $readmemh("../../../../../test/poly_out.txt" , poly_out);

    $display("Simulation started.");

    clk = 1'b0;
    rst = 1'b0;
    #FP;
    rst = 1'b1;
    #FP;
    rst = 1'b0;
    #(HP);
    #(1);

    cmd = `CMD_LOAD_Q;
    for (i = 0; i < (W/B); i = i + 1) begin
        i_data = q[0] >> (i*B);
        i_valid = 1'b1;
        #FP;
    end
    i_valid = 1'b0;

    cmd = `CMD_LOAD_TWIDDLE;
    for (i = 0; i < N; i = i + 1) begin
        for (j = 0; j < (W/B); j = j + 1) begin
            i_data = (i == (N-1)) ? 0 : psi[i] >> (j*B);
            i_valid = 1'b1;
            #FP;
        end
    end
    i_valid = 1'b0;

    cmd = `CMD_LOAD_POLY;
    for (i = 0; i < N; i = i + 1) begin
        for (j = 0; j < (W/B); j = j + 1) begin
            i_data = poly_in[i] >> (j*B);
            i_valid = 1'b1;
            #FP;
        end
    end
    i_valid = 1'b0;
    i = 0;
    j = 0;

    while(1) begin
        #FP;
        if (o_valid) begin
            $display("Output: %h", o_data);
            if (o_data != poly_out[i >> LOGWB][(i & WBMASK) * B +: B]) begin
                $display("Output mismatch at index %d: %h != %h", i, o_data, poly_out[i >> LOGWB][(i & WBMASK) * B +: B]);
                j = 1;
            end
            i = i + 1;
        end
        else if(!busy) begin
            $display("Simulation finished.");
            if (j == 0)
                $display("(SUCCESS)");
            else
                $display("(FAIL)");
            $finish;
        end
    end

end

endmodule