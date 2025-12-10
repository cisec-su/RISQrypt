`include "x2x_acc.vh"


module x2x_acc_op_core
   #(
    parameter HALFCYCLE = 1,              // ceil(LOG2(3329))
    parameter N_STAGES = 5,
    
//    parameter TWO_COMPLEMENT_MINUS_q = 4294963967,//4863, //62207, //(2^16 - q)
//    parameter KYBER_q = 3329,
    
    parameter PARAM_WIDTH = 32,  // # bits / data word
    parameter BOX_WIDTH = 16,
    parameter N_SHARES = 2, // protection order + 1 (# primary arithmetic shares)
    parameter RND_SHARES,
    parameter RND_SHARES_BOX,
    parameter RND_SHARES_2SHARE,
    parameter RND_SHARES_2SHARE_BOX
   
)
(
    // GENERAL
    input logic clk,
    input logic rst_n,
    
    // CONTROL
    input logic valid_data,     // valid A/B shares ready for input
    output logic ready_data,    // ready to accept new A/B shares and start A2B/B2A
    
    input logic ready_result,   // ready to accept new B/A shares and transfer
    output reg valid_result,  // valid B/A shares ready for output
    
    input logic valid_rng,     // valid A/B shares ready for input
    
    input logic conv_mode,    // 0 -> A2B, 1 -> B2A
    input logic data_type,     // 0 -> power-of-two, 1 -> prime
    input logic dual_mode,          // 0 -> single input, 1 -> dual input
    
    input logic [PARAM_WIDTH - 1 : 0]   modulus,
    input logic [PARAM_WIDTH - 1 : 0]   modulus_complement,
    input logic [PARAM_WIDTH - 1 : 0]   modulus_half,													 
    input logic [4 : 0]   log_modulus,
    //output logic dual_mode_out,     // 0 -> single output, 1 -> dual output
    
    // RND 1 + 2
    input logic [PARAM_WIDTH - 1 : 0]   x2x_fresh_rnd_shares        [RND_SHARES - 1 : 0],
    input logic [BOX_WIDTH - 1 : 0]     x2x_fresh_rnd_shares_8bit   [RND_SHARES_BOX - 1 : 0], // FOR 2 SECADD, either mod q or power-of-two in parallel
    
    // DATA 1 + 2
    input logic  [PARAM_WIDTH - 1 : 0] original_data    [2 - 1 : 0][N_SHARES - 1:0],
    output reg [PARAM_WIDTH - 1 : 0] converted_data   [2 - 1 : 0][N_SHARES - 1:0],
    
    input logic [1:0] opcode,
    input logic [31:0] rnd_ref
);

reg [PARAM_WIDTH - 1 : 0] x2x_original_data_raw    [2 - 1 : 0][N_SHARES - 1:0];
wire [PARAM_WIDTH - 1 : 0] x2x_original_data [1:0][1:0];
wire [PARAM_WIDTH - 1 : 0] x2x_converted_data   [2 - 1 : 0][N_SHARES - 1:0];
wire [PARAM_WIDTH - 1 : 0] x2x_converted_data_raw   [2 - 1 : 0][N_SHARES - 1:0];

wire x2x_valid_result;
reg x2x_valid_data;

reg [PARAM_WIDTH-1:0] modulus_mask_c;
reg [PARAM_WIDTH-1:0] modulus_int_c;

reg [PARAM_WIDTH-1:0] modulus_mask;
reg [PARAM_WIDTH-1:0] modulus_int;

reg  [RND_SHARES_2SHARE-1:0] data_ready;


reg [31:0] rnd_ref_int;
reg [PARAM_WIDTH - 1 : 0] original_data_int    [2 - 1 : 0][N_SHARES - 1:0];
reg [PARAM_WIDTH - 1 : 0]   x2x_fresh_rnd_shares_int        [RND_SHARES - 1 : 0];
reg valid_data_int;
reg valid_rng_int;


if (HALFCYCLE) begin
    always @(posedge clk) begin
        original_data_int <= original_data;
        x2x_fresh_rnd_shares_int <= x2x_fresh_rnd_shares;
        valid_data_int <= valid_data;
        valid_rng_int <= valid_rng;
        rnd_ref_int <= rnd_ref;
    end
end
else begin
    always @(*) begin
        original_data_int = original_data;
        x2x_fresh_rnd_shares_int = x2x_fresh_rnd_shares;
        valid_data_int = valid_data;
        valid_rng_int = valid_rng;
        rnd_ref_int = rnd_ref;
    end
end


assign modulus_mask_c = (data_type == 0) ?          (modulus)    :
                                           (1 << log_modulus) - 1;

assign modulus_int_c = (data_type) ? {1'b0, modulus}    : 
                                     {1'b0, modulus} + 1;


always @(posedge clk) begin
   modulus_mask <= modulus_mask_c;
   modulus_int  <= modulus_int_c;
end


X2X_32b_2SHARE_HALFCYCLE_STREAM #(
    .HALFCYCLE          (HALFCYCLE      ),
    .PARAM_WIDTH        (PARAM_WIDTH    ),
    .N_SHARES           (2              ),
    .RND_SHARES         (RND_SHARES_2SHARE),
    .RND_SHARES_BOX    (RND_SHARES_2SHARE_BOX)
) x2x_inst (
    .clk                    (clk           ),
    .rst_n                  (rst_n         ),
    // x2x <- ctrl
    .conversion_mode        (conv_mode),
    .data_type_mode         (data_type),
    .dual_mode              (dual_mode),
    // x2x <-> fsm
    .valid_data             (x2x_valid_data),
    .ready_data             (ready_data),
    .ready_result           (ready_result),
    .valid_result           (x2x_valid_result),
    
    .modulus(modulus_int),
    .modulus_twoc(modulus_complement),
        
    .fresh_rnd_shares       (x2x_fresh_rnd_shares_int),//TBC
    .fresh_rnd_shares_8bit  (x2x_fresh_rnd_shares_8bit),//TBC
    .original_data          (x2x_original_data),
    .converted_data         (x2x_converted_data_raw)
);


assign x2x_converted_data[0][0] = (x2x_converted_data_raw[0][0] & modulus_mask);
assign x2x_converted_data[0][1] = (x2x_converted_data_raw[0][1] & modulus_mask);
assign x2x_converted_data[1][0] = (x2x_converted_data_raw[1][0] & modulus_mask);
assign x2x_converted_data[1][1] = (x2x_converted_data_raw[1][1] & modulus_mask);

wire randgen_valid_result;
wire [31:0] randgen_data_out;


assign x2x_original_data[0][0] = ((!conv_mode & data_type) & (x2x_original_data_raw[0][0] > modulus_half)) ?
                                     (x2x_original_data_raw[0][0] + modulus_complement) :
                                     x2x_original_data_raw[0][0];
assign x2x_original_data[0][1] = ((!conv_mode & data_type) & (x2x_original_data_raw[0][1] > modulus_half)) ?
                                     (x2x_original_data_raw[0][1] + modulus_complement) :
                                     x2x_original_data_raw[0][1];
assign x2x_original_data[1][0] = x2x_original_data_raw[1][0];
assign x2x_original_data[1][1] = x2x_original_data_raw[1][1];

x2x_acc_randgen randgen(
    .clk(clk),
    .rst_n(rst_n),
    .valid_data(valid_data_int),
    .valid_result(randgen_valid_result),
    .DATA_IN(rnd_ref_int),
    .DATA_OUT(randgen_data_out)
);

wire refresh_valid_result;
reg  refresh_valid_data;
reg [31:0] refresh_data_in1, refresh_data_in2, refresh_rnd_ref;
wire [31:0] refresh_data_out1, refresh_data_out2;

x2x_acc_refresh refresh(
    .clk(clk),
    .rst_n(rst_n),
    .A_in(refresh_data_in1),
    .B_in(refresh_data_in2),
    .rnd_ref(refresh_rnd_ref),
    .conv_mode        (conv_mode),
    .dual_mode       (dual_mode),
    .data_type       (data_type),
    .modulus        (modulus_int),
    
    .valid_data(refresh_valid_data),
	.valid_rng(valid_rng_int),					  
    .valid_result(refresh_valid_result),
    .x2x_dis(!opcode[0]),
    .A_out(refresh_data_out1),
    .B_out(refresh_data_out2)
);


always @(*)
begin
    x2x_original_data_raw[0][0] = 0;
    x2x_original_data_raw[0][1] = 0;
    x2x_original_data_raw[1][0] = 0;
    x2x_original_data_raw[1][1] = 0;
    
    converted_data[0][0] = 0;
    converted_data[0][1] = 0;
    converted_data[1][0] = 0;
    converted_data[1][1] = 0;
    
    valid_result = 0;
    
    refresh_data_in1 = 0;
    refresh_data_in2 = 0;
    refresh_rnd_ref = 0;
    refresh_valid_data = 0;
    
    x2x_valid_data = 0;
    
    if(opcode == `X2X_CMD_PRNG)
    begin
        valid_result = randgen_valid_result;
        if(dual_mode && !data_type)
        begin
            converted_data[0][1][15:0] = randgen_data_out[15:0];
            converted_data[1][1][15:0] = randgen_data_out[31:16];
        end
        else
            converted_data[0][1] = randgen_data_out;
    end
    
    else if(opcode == `X2X_CMD_X2X)
    begin
        x2x_original_data_raw[0][0] = original_data_int[0][0];
        x2x_original_data_raw[0][1] = original_data_int[0][1];
        x2x_original_data_raw[1][0] = original_data_int[1][0];
        x2x_original_data_raw[1][1] = original_data_int[1][1];
        
        converted_data[0][0] = x2x_converted_data[0][0];
        converted_data[0][1] = x2x_converted_data[0][1];
        converted_data[1][0] = x2x_converted_data[1][0];
        converted_data[1][1] = x2x_converted_data[1][1];
        
        x2x_valid_data = valid_data_int;
        valid_result = x2x_valid_result;
    end
    
    else if(opcode == `X2X_CMD_REF)
    begin
        if(dual_mode && !data_type)
        begin
            refresh_data_in1 = {original_data_int[1][0][15:0],original_data_int[0][0][15:0]};
            refresh_data_in2 = {original_data_int[1][1][15:0],original_data_int[0][1][15:0]};
            converted_data[0][0] = refresh_data_out1[15:0];
            converted_data[0][1] = refresh_data_out2[15:0];
            converted_data[1][0] = refresh_data_out1[31:16];
            converted_data[1][1] = refresh_data_out2[31:16];
        end
        else
        begin
            refresh_data_in1 = original_data_int[0][0];
            refresh_data_in2 = original_data_int[0][1];
            converted_data[0][0] = refresh_data_out1;
            converted_data[0][1] = refresh_data_out2;
        end
        
        refresh_rnd_ref = rnd_ref_int;
        refresh_valid_data = valid_data_int;
        valid_result = refresh_valid_result;
    end
    
    else if(opcode == `X2X_CMD_REFX2X)
    begin
        if(dual_mode && !data_type)
        begin
            refresh_data_in1 = {original_data_int[1][0][15:0],original_data_int[0][0][15:0]};
            refresh_data_in2 = {original_data_int[1][1][15:0],original_data_int[0][1][15:0]};
            
            x2x_original_data_raw[0][0] = refresh_data_out1[15:0];
            x2x_original_data_raw[0][1] = refresh_data_out2[15:0];
            x2x_original_data_raw[1][0] = refresh_data_out1[31:16];
            x2x_original_data_raw[1][1] = refresh_data_out2[31:16];
        end
        else
        begin
            refresh_data_in1 = original_data_int[0][0];
            refresh_data_in2 = original_data_int[0][1];
            
			/*if(!conv_mode & data_type) // A2B Unsigned
            begin
                if(refresh_data_out1 > modulus_half) 
                    x2x_original_data[0][0] = refresh_data_out1 + modulus_complement;
                else
                    x2x_original_data[0][0] = refresh_data_out1;
                    
                if(refresh_data_out2 > modulus_half) 
                    x2x_original_data[0][1] = refresh_data_out2 + modulus_complement;
                else 
                    x2x_original_data[0][1] = refresh_data_out2;
            end
            else
            begin*/
                x2x_original_data_raw[0][0] = refresh_data_out1;
                x2x_original_data_raw[0][1] = refresh_data_out2;
            //end  
        end
        
        converted_data[0][0] = x2x_converted_data[0][0];
        converted_data[0][1] = x2x_converted_data[0][1];
        converted_data[1][0] = x2x_converted_data[1][0];
        converted_data[1][1] = x2x_converted_data[1][1];
        
        refresh_rnd_ref = rnd_ref_int;
        refresh_valid_data = valid_data_int;
        
        x2x_valid_data = refresh_valid_result & (valid_data_int || valid_rng_int);
        valid_result = x2x_valid_result;
        
        
    end
    
end


endmodule