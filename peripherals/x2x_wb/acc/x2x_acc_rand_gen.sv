module x2x_acc_randgen
    (
        input  clk,
        input  rst_n,
        input  valid_data ,
        output reg valid_result,
        input  [31:0] DATA_IN  ,
        output reg [31:0] DATA_OUT
    );

always @(posedge clk/* or negedge rst_n*/) begin
    if (!rst_n) 
    begin
        valid_result <= 0;
        DATA_OUT <= 0;
    end 
    else if (valid_data) 
    begin
        valid_result <= 1'b1;
        DATA_OUT <= DATA_IN;
    end
    else begin
        valid_result <= 1'b0;
        DATA_OUT <= 32'd0;
    end
end
endmodule