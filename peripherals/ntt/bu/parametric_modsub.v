
`include "defines.v"

module parametric_modsub(input [63:0] A,B,
              input [63:0] q,
              output[63:0] C);

wire [64:0] R;
wire [64:0] Rq;

assign R = A - B;
assign Rq= R +  {1'b0,q[63:17],16'b0,q[0]};

assign C = (R[64] == 0) ? R[63:0] : Rq[63:0];

endmodule


