///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////     //   //    /////   //   //     ///////////////////////////////////////////////
////////////////////////////////////////      // //         //   // //      ///////////////////////////////////////////////
////////////////////////////////////////        //      /////      //       ///////////////////////////////////////////////
////////////////////////////////////////      // //    //        // //      ///////////////////////////////////////////////
////////////////////////////////////////     //   //   ///////  //   //     ///////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                 //    //    //       //    //  //      //  //  ////// /////  //  //  ///// //  // //   //             //
//                / //   //    //      / //   //////      ////    //  // //  // //  //  //    //  // //// //             //
//               //////  //    //     //////  //  //      // ///  //  // ////   //  //    //  //  // // ////             //
//              //    // ///// ///// /     // //  //      //   // ////// // /// ////// /////  ////// //  ///             //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

`include "x2x_acc.vh"

module x2x_acc_op_core
   #(
    parameter HALFCYCLE   = 1      ,
    parameter PARAM_WIDTH = 32     ,
    parameter BOX_WIDTH   = 16     ,
    parameter N_SHARES    = 2      ,
    parameter RND_SHARES           ,
    parameter RND_SHARES_BOX       ,
    parameter RND_SHARES_2SHARE    ,
    parameter RND_SHARES_2SHARE_BOX
   
)
(
    input logic                     clk                                                 ,
    input logic                     rst_n                                               ,

    input  logic                    valid_data                                          ,    // valid A/B shares ready for input
    output logic                    ready_data                                          ,    // ready to accept new A/B shares and start A2B/B2A
    
    input  logic                    ready_result                                        ,  // ready to accept new B/A shares and transfer
    output   reg                    valid_result                                        ,  // valid B/A shares ready for output
    
    input logic                     valid_rng                                           ,      // valid A/B shares ready for input
    
    input logic                     conv_mode                                           ,     // 0 -> A2B, 1 -> B2A
    input logic                     data_type                                           ,     // 0 -> power-of-two, 1 -> prime
    input logic                     dual_mode                                           ,     // 0 -> single input, 1 -> dual input
    
    input logic [PARAM_WIDTH-1:0]   modulus                                             ,
    input logic [PARAM_WIDTH-1:0]   modulus_complement                                  ,
    input logic [PARAM_WIDTH-1:0]   modulus_half                                        ,													 
    input logic [            4:0]   log_modulus                                         ,
    
    input logic [PARAM_WIDTH-1:0]   x2x_fresh_rnd_shares            [RND_SHARES    -1:0],
    input logic [BOX_WIDTH  -1:0]   x2x_fresh_rnd_shares_8bit       [RND_SHARES_BOX-1:0],
    
    // DATA 1 + 2
    input logic [PARAM_WIDTH-1:0]   original_data              [1:0][N_SHARES      -1:0],
    output reg  [PARAM_WIDTH-1:0]   converted_data             [1:0][N_SHARES      -1:0],
    
    input logic [            1:0]   opcode                                              ,
    input logic [           31:0]   rnd_ref
);

reg  [PARAM_WIDTH-1:0] x2x_original_data_raw  [1:0][N_SHARES-1:0];
wire [PARAM_WIDTH-1:0] x2x_original_data      [1:0][N_SHARES-1:0];
wire [PARAM_WIDTH-1:0] x2x_converted_data     [1:0][N_SHARES-1:0];
wire [PARAM_WIDTH-1:0] x2x_converted_data_raw [1:0][N_SHARES-1:0];
reg  [PARAM_WIDTH-1:0] converted_data_int     [1:0][N_SHARES-1:0];

reg  valid_result_int;
wire x2x_valid_result;
reg  x2x_valid_data;

reg [PARAM_WIDTH-1:0] modulus_mask_c;
reg [PARAM_WIDTH-1:0] modulus_int_c ;
reg [PARAM_WIDTH-1:0] modulus_mask  ;
reg [PARAM_WIDTH-1:0] modulus_int   ;

reg [31:0] rnd_ref_int;

reg [PARAM_WIDTH-1:0]   original_data_int             [1:0][N_SHARES      -1:0];
reg [PARAM_WIDTH-1:0]   x2x_fresh_rnd_shares_q           [RND_SHARES    -1:0];
reg [PARAM_WIDTH-1:0]   x2x_fresh_rnd_shares_d                                 ;
reg [PARAM_WIDTH-1:0]   x2x_fresh_rnd_shares_d0                                ;
reg [PARAM_WIDTH-1:0]   x2x_fresh_rnd_shares_c             [RND_SHARES    -1:0];
reg [BOX_WIDTH  -1:0]   x2x_fresh_rnd_shares_8bit_q      [RND_SHARES_BOX-1:0];

reg valid_data_int;
reg valid_rng_int;

wire refresh_valid_result;
reg  refresh_valid_data;
reg  [31:0] refresh_data_in1 , refresh_data_in2, refresh_rnd_ref;
wire [31:0] refresh_data_out1, refresh_data_out2;

reg  randgen_valid_data;
wire randgen_valid_result;
wire [31:0] randgen_data_out;



always @(posedge clk) begin
    if (rst_n == 1'b0) begin
        valid_result <= 1'b0;
    end
    else begin
        valid_result <= valid_result_int;
    end
end


always @(posedge clk) begin
    converted_data[0][0] <= (valid_result_int) ? converted_data_int[0][0] : 0;
    converted_data[0][1] <= (valid_result_int) ? converted_data_int[0][1] : 0;
    converted_data[1][0] <= (valid_result_int) ? converted_data_int[1][0] : 0;
    converted_data[1][1] <= (valid_result_int) ? converted_data_int[1][1] : 0;
end


always @(posedge clk) begin
    original_data_int             <= original_data;
    x2x_fresh_rnd_shares_d        <= x2x_fresh_rnd_shares_q[4];
    x2x_fresh_rnd_shares_q        <= x2x_fresh_rnd_shares;
    x2x_fresh_rnd_shares_8bit_q   <= x2x_fresh_rnd_shares_8bit;
    valid_data_int                <= valid_data;
    valid_rng_int                 <= valid_rng;
    rnd_ref_int                   <= rnd_ref;
end


assign modulus_mask_c = (data_type == 0) ?          (modulus)    :
                                           (1 << log_modulus) - 1;

assign modulus_int_c = (data_type) ? {1'b0, modulus}    : 
                                     {1'b0, modulus} + 1;


always @(posedge clk) begin
   modulus_mask <= modulus_mask_c;
   modulus_int  <= modulus_int_c;
end


assign x2x_converted_data[0][0] = (x2x_converted_data_raw[0][0] & modulus_mask);
assign x2x_converted_data[0][1] = (x2x_converted_data_raw[0][1] & modulus_mask);
assign x2x_converted_data[1][0] = (x2x_converted_data_raw[1][0] & modulus_mask);
assign x2x_converted_data[1][1] = (x2x_converted_data_raw[1][1] & modulus_mask);


assign x2x_original_data[0][0] = x2x_original_data_raw[0][0];
assign x2x_original_data[0][1] = ((!conv_mode) & data_type) ? (x2x_original_data_raw[0][1] + modulus_complement) : x2x_original_data_raw[0][1];
assign x2x_original_data[1][0] = x2x_original_data_raw[1][0];
assign x2x_original_data[1][1] = x2x_original_data_raw[1][1];


x2x_acc_randgen randgen(
    .clk          (clk                  ),
    .rst_n        (rst_n                ),
    .valid_data   (randgen_valid_data   ),
    .valid_result (randgen_valid_result ),
    .DATA_IN      (rnd_ref_int          ),
    .DATA_OUT     (randgen_data_out     )
);


x2x_acc_refresh refresh(
    .clk         (clk                 ),
    .rst_n       (rst_n               ),
    .A_in        (refresh_data_in1    ),
    .B_in        (refresh_data_in2    ),
    .rnd_ref     (refresh_rnd_ref     ),
    .conv_mode   (conv_mode           ),
    .dual_mode   (dual_mode           ),
    .data_type   (data_type           ),
    .modulus     (modulus_int         ),    
    .valid_data  (refresh_valid_data  ),
    .valid_result(refresh_valid_result),
    .A_out       (refresh_data_out1   ),
    .B_out       (refresh_data_out2   )
);


(* dont_touch *)
X2X_32b_2SHARE_HALFCYCLE_STREAM #(
    .HALFCYCLE          (HALFCYCLE     ),
    .PARAM_WIDTH        (PARAM_WIDTH   ),
    .N_SHARES           (N_SHARES      ),
    .RND_SHARES         (RND_SHARES    ),
    .RND_SHARES_BOX     (RND_SHARES_BOX)
) x2x_inst (
    .clk                    (clk                        ),
    .rst_n                  (rst_n                      ),
    .conversion_mode        (conv_mode                  ),
    .data_type_mode         (data_type                  ),
    .dual_mode              (dual_mode                  ),
    .valid_data             (x2x_valid_data             ),
    .ready_data             (ready_data                 ),
    .ready_result           (ready_result               ),
    .valid_result           (x2x_valid_result           ),
    .modulus                (modulus_int                ),
    .modulus_twoc           (modulus_complement         ),
    .fresh_rnd_shares       (x2x_fresh_rnd_shares_c     ),
    .fresh_rnd_shares_8bit  (x2x_fresh_rnd_shares_8bit_q),
    .original_data          (x2x_original_data          ),
    .converted_data         (x2x_converted_data_raw     )
);


always @(*)
begin
    x2x_original_data_raw[0][0] = 0;
    x2x_original_data_raw[0][1] = 0;
    x2x_original_data_raw[1][0] = 0;
    x2x_original_data_raw[1][1] = 0;
    
    converted_data_int[0][0] = 0;
    converted_data_int[0][1] = 0;
    converted_data_int[1][0] = 0;
    converted_data_int[1][1] = 0;

    randgen_valid_data = 0;

    valid_result_int = 0;
    
    refresh_data_in1 = 0;
    refresh_data_in2 = 0;
    refresh_rnd_ref = 0;
    refresh_valid_data = 0;
    
    x2x_valid_data = 0;


    for (int i = 0; i < 10; i = i + 1) begin
        x2x_fresh_rnd_shares_c[i] = x2x_fresh_rnd_shares_q[i];
    end

    // x2x_fresh_rnd_shares_c[0] = x2x_fresh_rnd_shares_q[0];
    // x2x_fresh_rnd_shares_c[1] = x2x_fresh_rnd_shares_q[1];
    // x2x_fresh_rnd_shares_c[2] = x2x_fresh_rnd_shares_q[2];
    // x2x_fresh_rnd_shares_c[3] = x2x_fresh_rnd_shares_q[3];
    // x2x_fresh_rnd_shares_c[4] = x2x_fresh_rnd_shares_q[4];//x2x_fresh_rnd_shares_d0;//x2x_fresh_rnd_shares[4];
    // x2x_fresh_rnd_shares_c[5] = x2x_fresh_rnd_shares_q[5];
    // x2x_fresh_rnd_shares_c[6] = x2x_fresh_rnd_shares_q[6];
    // x2x_fresh_rnd_shares_c[7] = x2x_fresh_rnd_shares_q[7];
    // x2x_fresh_rnd_shares_c[8] = x2x_fresh_rnd_shares_q[8];
    // x2x_fresh_rnd_shares_c[9] = x2x_fresh_rnd_shares_q[9];

    
    if(opcode == `X2X_CMD_PRNG)
    begin
        randgen_valid_data = valid_data_int;
        valid_result_int = randgen_valid_result;
        if(dual_mode && !data_type)
        begin
            converted_data_int[0][1][15:0] = randgen_data_out[15:0];
            converted_data_int[1][1][15:0] = randgen_data_out[31:16];
        end
        else
            converted_data_int[0][1] = randgen_data_out;
    end
    
    else if(opcode == `X2X_CMD_X2X)
    begin

        x2x_original_data_raw[0][0] = original_data_int[0][0];
        x2x_original_data_raw[0][1] = original_data_int[0][1];
        x2x_original_data_raw[1][0] = original_data_int[1][0];
        x2x_original_data_raw[1][1] = original_data_int[1][1];
        
        converted_data_int[0][0] = x2x_converted_data[0][0];
        converted_data_int[0][1] = x2x_converted_data[0][1];
        converted_data_int[1][0] = x2x_converted_data[1][0];
        converted_data_int[1][1] = x2x_converted_data[1][1];
        
        x2x_valid_data = valid_data_int;
        valid_result_int = x2x_valid_result;

        x2x_fresh_rnd_shares_c[4] = x2x_fresh_rnd_shares_q[4];

    end
    
    else if(opcode == `X2X_CMD_REF)
    begin
        if(dual_mode && !data_type)
        begin
            refresh_data_in1 = {original_data_int[1][0][15:0],original_data_int[0][0][15:0]};
            refresh_data_in2 = {original_data_int[1][1][15:0],original_data_int[0][1][15:0]};
            converted_data_int[0][0] = refresh_data_out1[15:0];
            converted_data_int[0][1] = refresh_data_out2[15:0];
            converted_data_int[1][0] = refresh_data_out1[31:16];
            converted_data_int[1][1] = refresh_data_out2[31:16];
        end
        else
        begin
            refresh_data_in1 = original_data_int[0][0];
            refresh_data_in2 = original_data_int[0][1];
            converted_data_int[0][0] = refresh_data_out1;
            converted_data_int[0][1] = refresh_data_out2;
        end
        
        refresh_rnd_ref = rnd_ref_int;
        refresh_valid_data = valid_data_int;
        valid_result_int = refresh_valid_result;
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
            
            x2x_original_data_raw[0][0] = refresh_data_out1;
            x2x_original_data_raw[0][1] = refresh_data_out2;
        end
        
        converted_data_int[0][0] = x2x_converted_data[0][0];
        converted_data_int[0][1] = x2x_converted_data[0][1];
        converted_data_int[1][0] = x2x_converted_data[1][0];
        converted_data_int[1][1] = x2x_converted_data[1][1];
        
        refresh_rnd_ref = rnd_ref_int;
        refresh_valid_data = valid_data_int;
        
        x2x_valid_data = refresh_valid_result;
        valid_result_int = x2x_valid_result;

        x2x_fresh_rnd_shares_c[4] = x2x_fresh_rnd_shares_d;
    end
    
end


endmodule