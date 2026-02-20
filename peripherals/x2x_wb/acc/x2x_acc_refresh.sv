module x2x_acc_refresh
    (
        input                   clk                        ,
        input                   rst_n                      ,
        input [31:0]            A_in,
        input [31:0]            B_in,
        input [31:0]            rnd_ref,
        input                   conv_mode,
        input                   dual_mode,
        input                   data_type,
        input                   valid_data,
        input [31:0]            modulus,
        (* keep = "true" *)
        output reg [31:0]       A_out,
        (* keep = "true" *)
        output reg [31:0]       B_out,
        output reg              valid_result
    );

wire [31:0] xor1_in, xor2_in;
wire [31:0] xor1_out, xor2_out;

assign xor1_in = (conv_mode) ? A_in : 32'd0;
assign xor2_in = (conv_mode) ? B_in : 32'd0;

x2x_acc_xor xor1
(
    .data_in(xor1_in),
    .rnd_ref(rnd_ref),
    .data_out(xor1_out)  
);

x2x_acc_xor xor2
(
    .data_in(xor2_in),
    .rnd_ref(rnd_ref),
    .data_out(xor2_out)  
);

wire [31:0] add_in, sub_in;
wire [31:0] add_out, sub_out; 
wire [31:0] modulus_int;

assign add_in = (!conv_mode) ? A_in : 32'd0;
assign sub_in = (!conv_mode) ? B_in : 32'd0;

assign modulus_int = (dual_mode) ? {modulus[15:0], modulus[15:0]} : modulus;

modadd #(
    .LOGQ(32)
) add (
    .A(add_in),
    .B(rnd_ref),
    .Q(modulus_int),
    .dual(dual_mode),
    .C(add_out)
);

modsub #(
    .LOGQ(32)
) sub (
    .A(sub_in),
    .B(rnd_ref),
    .Q(modulus_int),
    .dual(dual_mode),
    .C(sub_out)
);


always @(posedge clk)
begin
    if(!rst_n)
    begin
        A_out <= 0;
        B_out <= 0;
        valid_result <= 0; 
    end
    else if(valid_data)
    begin
        valid_result <= 1'b1;
        case(conv_mode)
        1://B
        begin
            A_out <= xor1_out;
            B_out <= xor2_out;
        end
        0://A
        begin
            A_out <= add_out;
            B_out <= sub_out;
        end
        endcase
    end
    else begin
        valid_result <= 1'b0;
        A_out <= 32'd0;
        B_out <= 32'd0;
    end
end


endmodule
