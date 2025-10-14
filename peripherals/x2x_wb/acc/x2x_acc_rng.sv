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
        input                 [ 4:0]  log_modulus   ,
        input                         ctrl_rej_samp ,
        output reg [PARAM_WIDTH-1:0]  x2x_fresh_rnd_shares      [RND_SHARES_2SHARE    -1:0],
        output reg [  BOX_WIDTH-1:0]  x2x_fresh_rnd_shares_8bit [RND_SHARES_2SHARE_BOX-1:0],
        output                        rnd_ready 
    );


wire [255:0] stream_out1; 
wire [255:0] stream_out2;
wire [159:0] stream_out3; 
wire [159:0] stream_out4; 
wire [159:0] stream_out5; 
wire [159:0] stream_out6; 
wire [159:0] stream_out7; 
wire [159:0] stream_out8;  

wire [PARAM_WIDTH-1:0] modulus_mask;
wire [PARAM_WIDTH  :0] modulus_int;

reg  [RND_SHARES_2SHARE-1:0] data_ready;


assign modulus_mask = (ctrl_data_type == 0) ? modulus                     :
                      (ctrl_rej_samp      ) ? (1 << (log_modulus - 1)) - 1:
                                              (1 << log_modulus)       - 1;

assign modulus_int = (ctrl_data_type) ?    {1'b0, modulus} : 
                                        {1'b0, modulus} + 1;

assign rnd_ready = (data_ready == 0); // TODO : will be parametric*/


Trivium #(
    .output_bits(256)
) RNG1 (
    .clk(clk),
    .rst(rst_n),
    .load(ctrl_load_seed),
    .key({16'd0, ctrl_seed}),
    .iv(80'd1),
    .stream_out(stream_out1)
);

Trivium #(
    .output_bits(256)
) RNG2 (
    .clk(clk),
    .rst(rst_n),
    .load(ctrl_load_seed),
    .key({16'd0, ctrl_seed}),
    .iv(80'd2),
    .stream_out(stream_out2)
);

Trivium #(
    .output_bits(256)
) RNG3 (
    .clk(clk),
    .rst(rst_n),
    .load(ctrl_load_seed),
    .key({16'd0, ctrl_seed}),
    .iv(80'd3),
    .stream_out(stream_out3)
);

Trivium #(
    .output_bits(160)
) RNG4 (
    .clk(clk),
    .rst(rst_n),
    .load(ctrl_load_seed),
    .key({16'd0, ctrl_seed}),
    .iv(80'd4),
    .stream_out(stream_out4)
);

Trivium #(
    .output_bits(160)
) RNG5 (
    .clk(clk),
    .rst(rst_n),
    .load(ctrl_load_seed),
    .key({16'd0, ctrl_seed}),
    .iv(80'd5),
    .stream_out(stream_out5)
);

Trivium #(
    .output_bits(160)
) RNG6 (
    .clk(clk),
    .rst(rst_n),
    .load(ctrl_load_seed),
    .key({16'd0, ctrl_seed}),
    .iv(80'd6),
    .stream_out(stream_out6)
);

Trivium #(
    .output_bits(160)
) RNG7 (
    .clk(clk),
    .rst(rst_n),
    .load(ctrl_load_seed),
    .key({16'd0, ctrl_seed}),
    .iv(80'd7),
    .stream_out(stream_out7)
);

Trivium #(
    .output_bits(160)
) RNG8 (
    .clk(clk),
    .rst(rst_n),
    .load(ctrl_load_seed),
    .key({16'd0, ctrl_seed}),
    .iv(80'd8),
    .stream_out(stream_out8)
);


for(genvar j = 0; j < RND_SHARES_2SHARE; j = j + 1) begin
    always @(*) begin
        if(j < (RND_SHARES_2SHARE/2))
        begin               
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
        else
        begin               
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
    always @(*) begin
        if(j<(RND_SHARES_2SHARE_BOX/2))
            x2x_fresh_rnd_shares_8bit[j] = stream_out1[(BOX_WIDTH*(j+1)-1):(BOX_WIDTH*j)]; 
        else
            x2x_fresh_rnd_shares_8bit[j] = stream_out2[(BOX_WIDTH*((j-RND_SHARES_2SHARE_BOX/2)+1)-1):(BOX_WIDTH*(j-RND_SHARES_2SHARE_BOX/2))];     
    end
end


endmodule