
    `include "defines.v"
    
    module parametric_modmul(input        clk,rst,
              input [63:0] q,
              input [63:0] A,B,
              output[63:0] C,
              output[63:0] C_32
             );

    // q registers
    reg [63:0] qred,qint;

    `ifdef USE_DFF_MODMUL
    always @(posedge clk or posedge rst) begin
        if(rst)
            {qred,qint} <= 0;
        else
            {qred,qint} <= {qint,q};
    end
    `else
    always @(posedge clk or posedge rst) begin
        if(rst)
            qred <= 0;
        else
            qred <= q;
    end
    `endif

    // connections
    wire [127:0] D;
    reg  [127:0] D2;

    // integer mult
    parametric_intmul im(clk,rst,A,B,D);

    // connection
    `ifdef USE_DFF_MODMUL
    always @(posedge clk or posedge rst) begin
        if(rst)
            D2 <= 0;
        else
            D2 <= D;
    end
    `else
    always @(*) begin
        D2 = D;
    end
    `endif

    // modular reduction
    parametric_modred mr(clk,rst,qred,D2,C);

    // final 32-bit
    shiftreg #(.SHIFT(`MODRED_CC),.DATA(64)) sre00(clk,rst,D2[63:0],C_32);

    endmodule


