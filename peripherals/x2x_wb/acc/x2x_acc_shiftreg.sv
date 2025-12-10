module x2x_acc_shiftreg
    #(
        parameter SHIFT = 0 , 
        parameter DATA  = 32
   )
   (
        input             clk     ,
        input             rst_n     ,
        input  [DATA-1:0] data_in ,
        output [DATA-1:0] data_out
    );

reg [DATA-1:0] shift_array [SHIFT-1:0];

generate
    for(genvar shft = 0; shft < SHIFT; shft = shft + 1) begin: DELAY_BLOCK
        always @(posedge clk) begin
            if (!rst_n)
                shift_array[shft] <= 0;
            else
                shift_array[shft] <= (shft == 0) ? data_in : shift_array[shft - 1];
        end
    end
endgenerate

assign data_out = shift_array[SHIFT - 1];

endmodule