module x2x_acc_rng
   #(
        parameter PARAM_WIDTH           = 0,
        parameter BOX_WIDTH             = 0,
        parameter RND_SHARES_2SHARE     = 0,
        parameter RND_SHARES_2SHARE_BOX = 0
    )
    (
        input                         clk           ,
        input                         rst_n         ,
        input                 [31:0]  modulus       ,
        input                 [63:0]  ctrl_seed     , 
        input                         ctrl_load_seed,
        input                         ctrl_conv_mode,// 0 -> A2B, 1 -> B2A
        input                         ctrl_data_type,// 0 -> power-of-two, 1 -> prime
        input                         ctrl_dual_mode,
        input                 [ 4:0]  log_modulus   ,
        input                         ctrl_rej_samp ,
        input                         ctrl_nonzero,
        input                         ctrl_prng_off ,
        output reg [PARAM_WIDTH-1:0]  x2x_fresh_rnd_shares      [RND_SHARES_2SHARE    -1:0],
        output reg [  BOX_WIDTH-1:0]  x2x_fresh_rnd_shares_8bit [RND_SHARES_2SHARE_BOX-1:0],
        output                        rnd_x2x_ready,
        output reg [31:0]             rnd_ref,
        output reg                    rnd_ref_ready
        
    );


wire [255:0] stream_out1; 
wire [255:0] stream_out2;
wire [159:0] stream_out3; 
wire [159:0] stream_out4; 
wire [159:0] stream_out5; 
wire [191:0] stream_out6; 
wire [191:0] stream_out7; 
wire [191:0] stream_out8;  

reg [PARAM_WIDTH-1:0] modulus_mask_c;
reg [PARAM_WIDTH-1:0] modulus_int_c;

reg [PARAM_WIDTH-1:0] modulus_mask;
reg [PARAM_WIDTH-1:0] modulus_int;

reg  [RND_SHARES_2SHARE-1:0] data_ready;


assign modulus_mask_c = (ctrl_data_type == 0) ? (modulus)                   :
                      (ctrl_rej_samp      ) ? (1 << (log_modulus - 1)) - 1:
                                              (1 << log_modulus)       - 1;

assign modulus_int_c = (ctrl_data_type) ?    {1'b0, modulus}    : 
                                           {1'b0, modulus} + 1;


always @(posedge clk) begin
   modulus_mask <= modulus_mask_c;
   modulus_int  <= modulus_int_c;
end

assign rnd_x2x_ready = (data_ready == 0); // TODO : will be parametric*/


Trivium #(
    .output_bits(261)
) RNG1 (
    .clk(clk),
    .rst(rst_n & ~ctrl_prng_off),
    .load(ctrl_load_seed),
    .key({16'd0, ctrl_seed}),
    .iv(80'd1),
    .stream_out({stream_out1, stream_out5[132:128]})
);

Trivium #(
    .output_bits(261)
) RNG2 (
    .clk(clk),
    .rst(rst_n & ~ctrl_prng_off),
    .load(ctrl_load_seed),
    .key({16'd0, ctrl_seed}),
    .iv(80'd2),
    .stream_out({stream_out2, stream_out5[137:133]})
);

Trivium #(
    .output_bits(261)
) RNG3 (
    .clk(clk),
    .rst(rst_n & ~ctrl_prng_off),
    .load(ctrl_load_seed),
    .key({16'd0, ctrl_seed}),
    .iv(80'd3),
    .stream_out({stream_out3, stream_out4[159:64], stream_out5[142:138]})
);

Trivium #(
    .output_bits(261)
) RNG4 (
    .clk(clk),
    .rst(rst_n & ~ctrl_prng_off),
    .load(ctrl_load_seed),
    .key({16'd0, ctrl_seed}),
    .iv(80'd4),
    .stream_out({stream_out6, stream_out4[63:0], stream_out5[147:143]})
);

Trivium #(
    .output_bits(262)
) RNG5 (
    .clk(clk),
    .rst(rst_n & ~ctrl_prng_off),
    .load(ctrl_load_seed),
    .key({16'd0, ctrl_seed}),
    .iv(80'd5),
    .stream_out({stream_out7, stream_out5[127:64], stream_out5[153:148]})
);

Trivium #(
    .output_bits(262)
) RNG6 (
    .clk(clk),
    .rst(rst_n & ~ctrl_prng_off),
    .load(ctrl_load_seed),
    .key({16'd0, ctrl_seed}),
    .iv(80'd6),
    .stream_out({stream_out8, stream_out5[63:0], stream_out5[159:154]})
);

for(genvar j = 0; j < RND_SHARES_2SHARE; j = j + 1) begin
    if(j < (RND_SHARES_2SHARE/2))
    begin               
        always @(*) begin
            if((stream_out3[(PARAM_WIDTH*(j+1)-1):(PARAM_WIDTH*j)] & modulus_mask) < modulus_int)
            begin
                x2x_fresh_rnd_shares[j] = (stream_out3[(PARAM_WIDTH*(j+1)-1):(PARAM_WIDTH*j)] & modulus_mask);
                data_ready[j] = 0; 
            end
            else if((stream_out4[(PARAM_WIDTH*(j+1)-1):(PARAM_WIDTH*j)] & modulus_mask) < modulus_int)
            begin
                x2x_fresh_rnd_shares[j] = (stream_out4[(PARAM_WIDTH*(j+1)-1):(PARAM_WIDTH*j)] & modulus_mask);
                data_ready[j] = 0;
            end
            else if((stream_out5[(PARAM_WIDTH*(j+1)-1):(PARAM_WIDTH*j)] & modulus_mask) < modulus_int)
            begin
                x2x_fresh_rnd_shares[j] = (stream_out5[(PARAM_WIDTH*(j+1)-1):(PARAM_WIDTH*j)] & modulus_mask);
                data_ready[j] = 0;
            end
            else
            begin
                x2x_fresh_rnd_shares[j] = 0;
                data_ready[j] = 1;
            end
        end
    end
    else
    begin
        always @(*) begin
            if((stream_out6[(PARAM_WIDTH*((j-RND_SHARES_2SHARE/2)+1)-1):(PARAM_WIDTH*(j-RND_SHARES_2SHARE/2))] & modulus_mask) < modulus_int)
            begin
                x2x_fresh_rnd_shares[j] = (stream_out6[(PARAM_WIDTH*((j-RND_SHARES_2SHARE/2)+1)-1):(PARAM_WIDTH*(j-RND_SHARES_2SHARE/2))] & modulus_mask);
                data_ready[j] = 0;
            end
            else if((stream_out7[(PARAM_WIDTH*((j-RND_SHARES_2SHARE/2)+1)-1):(PARAM_WIDTH*(j-RND_SHARES_2SHARE/2))] & modulus_mask) < modulus_int)
            begin
                x2x_fresh_rnd_shares[j] = (stream_out7[(PARAM_WIDTH*((j-RND_SHARES_2SHARE/2)+1)-1):(PARAM_WIDTH*(j-RND_SHARES_2SHARE/2))] & modulus_mask);
                data_ready[j] = 0;
            end
            else if((stream_out8[(PARAM_WIDTH*((j-RND_SHARES_2SHARE/2)+1)-1):(PARAM_WIDTH*(j-RND_SHARES_2SHARE/2))] & modulus_mask) < modulus_int)
            begin
                x2x_fresh_rnd_shares[j] = (stream_out8[(PARAM_WIDTH*((j-RND_SHARES_2SHARE/2)+1)-1):(PARAM_WIDTH*(j-RND_SHARES_2SHARE/2))] & modulus_mask);
                data_ready[j] = 0;
            end
            else
            begin
                x2x_fresh_rnd_shares[j] = 0;
                data_ready[j] = 1;
            end
        end
    end
end


for(genvar j = 0; j < RND_SHARES_2SHARE_BOX ; j = j + 1) begin
    if(j<(RND_SHARES_2SHARE_BOX/2)) begin
        always @(*) begin
            x2x_fresh_rnd_shares_8bit[j] = stream_out1[(BOX_WIDTH*(j+1)-1):(BOX_WIDTH*j)]; 
        end
    end
    else begin
        always @(*) begin
            x2x_fresh_rnd_shares_8bit[j] = stream_out2[(BOX_WIDTH*((j-RND_SHARES_2SHARE_BOX/2)+1)-1):(BOX_WIDTH*(j-RND_SHARES_2SHARE_BOX/2))];     
        end
    end
end

always @(*)
begin
    if(!ctrl_data_type && ctrl_dual_mode)
    begin
        rnd_ref = {(stream_out6[191:176] & modulus_mask[15:0]),(stream_out6[175:160] & modulus_mask[15:0])};
        rnd_ref_ready = 1;
    end
    else
    begin
        if(((stream_out6[191:160] & modulus_mask) < modulus_int) && (!ctrl_nonzero || ((stream_out6[191:160] & modulus_mask) != 0 )))
        begin
            rnd_ref = (stream_out6[191:160] & modulus_mask);
            rnd_ref_ready = 1; 
        end
        else if(((stream_out7[191:160] & modulus_mask) < modulus_int) && (!ctrl_nonzero || ((stream_out7[191:160] & modulus_mask) != 0 )))
        begin
            rnd_ref = (stream_out7[191:160] & modulus_mask);
            rnd_ref_ready = 1;
        end
        else if(((stream_out8[191:160] & modulus_mask) < modulus_int) && (!ctrl_nonzero || ((stream_out8[191:160] & modulus_mask) != 0 )))
        begin
            rnd_ref = (stream_out8[191:160] & modulus_mask);
            rnd_ref_ready = 1;
        end
        else
        begin
            rnd_ref = 0;
            rnd_ref_ready = 0;
        end
    end
end

endmodule