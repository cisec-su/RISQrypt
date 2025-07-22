module x2x_acc_rng
   #(
        parameter B       = 32,
        parameter LOGL    = 10,
        parameter PARAM_WIDTH = 0,
        parameter RND_SHARES = 0,
        parameter RND_SHARES_8bit = 0
    )
    (
        input                   clk                        ,
        input                   rst_n                      ,
        input      [31:0] modulus,
        input      [      63:0] ctrl_seed             , 
        input                   ctrl_load_seed             ,
        input ctrl_conv_mode, // 0 -> A2B, 1 -> B2A
        input ctrl_data_type, // 0 -> power-of-two, 1 -> prime
        input [4:0] log_modulus,  ///////////
        input ctrl_rej_samp,////////// 
        output reg [PARAM_WIDTH - 1 : 0]             x2x_fresh_rnd_shares   [RND_SHARES - 1 : 0]    ,
        output reg  [8-1:0]            x2x_fresh_rnd_shares_8bit [RND_SHARES_8bit - 1 : 0],
        output rnd_ready 
    );   
wire [255:0] stream_out1; 
wire [255:0] stream_out2;  

reg [RND_SHARES - 1 : 0] data_ready;
reg [PARAM_WIDTH - 1 : 0] x2x_fresh_rnd_shares1   [RND_SHARES - 1 : 0];
reg [PARAM_WIDTH - 1 : 0] x2x_fresh_rnd_shares2   [RND_SHARES - 1 : 0];

assign rnd_ready = (data_ready == 11'h7ff); // TODO : will be parametric

Trivium RNG1 (
    .clk(clk),
    .rst(rst_n),
    .load(ctrl_load_seed),
    .key(80'd0),
    .iv({16'd0, ctrl_seed}),
    //.iv({48'd0, 32'd1}),
    .stream_out(stream_out1)
);

Trivium RNG2 (
    .clk(clk),
    .rst(rst_n),
    .load(ctrl_load_seed),
    .key({16'd0, ctrl_seed}),
    .iv(80'd0),
    //.iv({48'd0, 32'd1}),
    .stream_out(stream_out2)
);


for(genvar j = 0; j < RND_SHARES ; j = j + 1) begin
    always @(*) begin
            //x2x_fresh_rnd_shares[j] = (stream_out[(PARAM_WIDTH*(j+1)-1):(PARAM_WIDTH*j)] & 13'h03FF);   
            if((stream_out1[(PARAM_WIDTH*(j+1)-1):(PARAM_WIDTH*j)] & 13'h0fff) > 13'h0d00)
                x2x_fresh_rnd_shares[j] = (stream_out1[(PARAM_WIDTH*(j+1)-1):(PARAM_WIDTH*j)] & 13'h0fff) - 13'h0d01;
            else
                x2x_fresh_rnd_shares[j] = (stream_out1[(PARAM_WIDTH*(j+1)-1):(PARAM_WIDTH*j)] & 13'h0fff);
    end
end
/*
for(genvar j = 0; j < RND_SHARES ; j = j + 1) begin
    always @(*) begin
            x2x_fresh_rnd_shares1[j] = (stream_out1[(PARAM_WIDTH*(j+1)-1):(PARAM_WIDTH*j)] & 13'h03FF);   
    end
end

for(genvar j = 0; j < RND_SHARES ; j = j + 1) begin
    always @(*) begin
            x2x_fresh_rnd_shares2[j] = (stream_out2[(PARAM_WIDTH*(j+1)-1):(PARAM_WIDTH*j)] & 13'h03FF);   
    end
end

for(genvar j = 0; j < RND_SHARES ; j = j + 1) begin
    always @(*) begin
            
            if(x2x_fresh_rnd_shares1[j] < modulus)
            begin
                x2x_fresh_rnd_shares[j] = x2x_fresh_rnd_shares1[j];
                data_ready[j] = 1;
            end
            else if (x2x_fresh_rnd_shares2[j] < modulus)
            begin
                x2x_fresh_rnd_shares[j] = x2x_fresh_rnd_shares2[j];
                data_ready[j] = 1;
            end
            else
            begin
                x2x_fresh_rnd_shares[j] = 0;
                data_ready[j] = 0;
            end
    end
end
*/
for(genvar j = 0; j < RND_SHARES_8bit ; j = j + 1) begin
    always @(*) begin
            x2x_fresh_rnd_shares_8bit[j] = stream_out1[(8*(j+1)-1):(8*j)];     
    end
end











endmodule