module x2x_acc_rng
    (
        input              clk           ,
        input              rst_n         ,
        input      [31:0]  modulus       ,
        input      [63:0]  ctrl_seed     , 
        input              ctrl_load_seed,
        input              ctrl_conv_mode,// 0 -> A2B, 1 -> B2A
        input              ctrl_data_type,// 0 -> power-of-two, 1 -> prime
        input              ctrl_dual_mode,
        input      [ 4:0]  log_modulus   ,
        input              ctrl_rej_samp ,
        input              ctrl_nonzero,
        input              ctrl_prng_off ,
        output reg [31:0]  x2x_fresh_rnd_shares      [ 9:0],
        output     [15:0]  x2x_fresh_rnd_shares_8bit [27:0],
        output reg         rnd_x2x_ready,
        output reg [31:0]  rnd_ref,
        output reg         rnd_ref_ready
        
    );


wire [223:0] stream_out1; 
wire [223:0] stream_out2;
wire [239:0] stream_out3; 
wire [239:0] stream_out4; 

reg [32:0] modulus_mask_c;
reg [32:0] modulus_int_c;

reg [32:0] modulus_mask;
reg [32:0] modulus_int;


assign modulus_mask_c = (ctrl_data_type == 0) ? (modulus)                   :
                        (ctrl_rej_samp      ) ? (1 << (log_modulus - 1)) - 1:
                                                (1 << log_modulus)       - 1;

assign modulus_int_c = (ctrl_data_type) ?    {1'b0, modulus}    : 
                                             {1'b0, modulus} + 1;


always @(posedge clk) begin
   modulus_mask <= modulus_mask_c;
   modulus_int  <= modulus_int_c;
end


x2x_acc_lfsr #(
    .DATA_WIDTH(224)
) RNG1 (
    .clk(clk),
    .rst_n(rst_n & ~ctrl_prng_off),
    .load(ctrl_load_seed),
    .key({ctrl_seed}),
    .iv(64'd1),
    .stream_out(stream_out1)
);

x2x_acc_lfsr #(
    .DATA_WIDTH(224)
) RNG2 (
    .clk(clk),
    .rst_n(rst_n & ~ctrl_prng_off),
    .load(ctrl_load_seed),
    .key({ctrl_seed}),
    .iv(64'd2),
    .stream_out(stream_out2)
);

x2x_acc_lfsr #(
    .DATA_WIDTH(240)
) RNG3 (
    .clk(clk),
    .rst_n(rst_n & ~ctrl_prng_off),
    .load(ctrl_load_seed),
    .key({ctrl_seed}),
    .iv(64'd3),
    .stream_out(stream_out3)
);

x2x_acc_lfsr #(
    .DATA_WIDTH(240)
) RNG4 (
    .clk(clk),
    .rst_n(rst_n & ~ctrl_prng_off),
    .load(ctrl_load_seed),
    .key({ctrl_seed}),
    .iv(64'd4),
    .stream_out(stream_out4)
);


for(genvar j = 0; j < 10; j = j + 1) begin
    if (j == 9) begin
        always @(*) begin
            x2x_fresh_rnd_shares[j] = stream_out4[223:192];
        end
    end
    else if (j < 4) begin
        always @(*) begin
            x2x_fresh_rnd_shares[j] = stream_out3[(32*(j+1)-1):(32*j)];
        end
    end
    else if (j > 5) begin
        always @(*) begin
            x2x_fresh_rnd_shares[j] = stream_out3[(32*((j-2)+1)-1):(32*(j-2))];
        end
    end
end


for (genvar j = 0; j < 28; j = j + 1) begin
    if(j < 14) begin
        assign x2x_fresh_rnd_shares_8bit[j] = stream_out1[(16*(j+1)-1):(16*j)]; 
    end
    else begin
        assign x2x_fresh_rnd_shares_8bit[j] = stream_out2[(16*((j-14)+1)-1):(16*(j-14))];     
    end
end


always @(*)
begin
    if(!ctrl_data_type && ctrl_dual_mode)
    begin
        rnd_ref = {(stream_out4[31:16] & modulus_mask[15:0]), (stream_out4[15:0] & modulus_mask[15:0])};
        rnd_ref_ready = 1;
    end
    else
    begin
        if(((stream_out4[31:0] & modulus_mask) < modulus_int) && (!ctrl_nonzero || ((stream_out4[31:0] & modulus_mask) != 0 )))
        begin
            rnd_ref = (stream_out4[31:0] & modulus_mask);
            rnd_ref_ready = 1; 
        end
        else if(((stream_out4[63:32] & modulus_mask) < modulus_int) && (!ctrl_nonzero || ((stream_out4[63:32] & modulus_mask) != 0 )))
        begin
            rnd_ref = (stream_out4[63:32] & modulus_mask);
            rnd_ref_ready = 1;
        end
        else if(((stream_out4[95:64] & modulus_mask) < modulus_int) && (!ctrl_nonzero || ((stream_out4[95:64] & modulus_mask) != 0 )))
        begin
            rnd_ref = (stream_out4[95:64] & modulus_mask);
            rnd_ref_ready = 1;
        end
        else
        begin
            rnd_ref = 0;
            rnd_ref_ready = 0;
        end
    end
end


always @(*) begin
    x2x_fresh_rnd_shares[4] = stream_out4[127:96] & modulus_mask;
    if ((stream_out4[127:96] & modulus_mask) < modulus_int)
    begin
        x2x_fresh_rnd_shares[4] = (stream_out4[127:96] & modulus_mask);
        rnd_x2x_ready = 1;
    end
    else if ((stream_out4[159:128] & modulus_mask) < modulus_int)
    begin
        x2x_fresh_rnd_shares[4] = (stream_out4[159:128] & modulus_mask);
        rnd_x2x_ready = 1;
    end
    else if ((stream_out4[191:160] & modulus_mask) < modulus_int)
    begin
        x2x_fresh_rnd_shares[4] = (stream_out4[191:160] & modulus_mask);
        rnd_x2x_ready = 1;
    end
    else if (ctrl_conv_mode)
    begin
        rnd_x2x_ready = 0;
    end
    else
    begin
        rnd_x2x_ready = 1;
    end    
end


always @(*) begin
    // x2x_fresh_rnd_shares[5] = {23'b0, stream_out4[227:224], stream_out3[228:224]};
    x2x_fresh_rnd_shares[5] = {stream_out4[239:224], stream_out3[239:224]};    
end


endmodule