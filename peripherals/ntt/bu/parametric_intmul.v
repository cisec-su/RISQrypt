
`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 05/21/2024 10:40:22 AM
// Design Name: 
// Module Name: intmul32_64_top
// Project Name: 
// Target Devices: 
// Tool Versions: 
// Description: 
// 
// Dependencies: 
// 
// Revision:
// Revision 0.01 - File Created
// Additional Comments:
// 
//////////////////////////////////////////////////////////////////////////////////


module parametric_intmul(input clk,rst,
              input [63:0] A,
              input [63:0] B,
              output reg [127:0] D);


(* use_dsp = "yes" *) reg [42:0] p0_0,p0_1,p0_2,p1_0,p1_1,p1_2,p2_0,p2_1,p2_2,p3_0,p3_1,p3_2;


always @(posedge clk or posedge rst) begin
    if(rst) begin
        p0_0 <= 0;
        p0_1 <= 0;
        p0_2 <= 0;
        p1_0 <= 0;
        p1_1 <= 0;
        p1_2 <= 0;
        p2_0 <= 0;
        p2_1 <= 0;
        p2_2 <= 0;
        p3_0 <= 0;
        p3_1 <= 0;
        p3_2 <= 0;


    end
    else begin
        p0_0 <= A[16:0] * B[25:0];
        p0_1 <= A[16:0] * B[51:26];
        p0_2 <= A[16:0] * B[63:52];
        p1_0 <= A[33:17] * B[25:0];
        p1_1 <= A[33:17] * B[51:26];
        p1_2 <= A[33:17] * B[63:52];
        p2_0 <= A[50:34] * B[25:0];
        p2_1 <= A[50:34] * B[51:26];
        p2_2 <= A[50:34] * B[63:52];
        p3_0 <= A[63:51] * B[25:0];
        p3_1 <= A[63:51] * B[51:26];
        p3_2 <= A[63:51] * B[63:52];

    end
end

wire [127:0] p;



assign p = {{13'b0,p2_2,p1_1,p0_0}} + {{30'b0,p1_2,p0_1,26'b0}} + {{47'b0,p0_2,52'b0}} + {{p3_2,p2_1,p1_0,17'b0}} + {{12'b0,p3_1,p2_0,34'b0}} + {{38'b0,p3_0,51'b0}}; 


always @(*) begin
    D = p;
end
          
endmodule


