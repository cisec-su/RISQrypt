module x2x_acc_top
   #(
        parameter BASE_ADDR       = 32'h1004_0050,
        parameter SHARES          = 2           ,
        parameter LOGL            = 10          ,
        parameter PRNG_OFF_EN     = 0           ,
        parameter HALFCYCLE       = 1
    )
    (
        // wishbone
        input              wb_cyc_i  ,
        input              wb_stb_i  ,
        input              wb_we_i   ,
        input      [31:0]  wb_adr_i  ,
        input      [31:0]  wb_dat_i  ,
        input      [ 3:0]  wb_sel_i  ,
        output             wb_stall_o,
        output             wb_ack_o  ,
        output     [31:0]  wb_dat_o  ,
        output             wb_err_o  ,
        input              wb_rst_i  ,
        input              wb_clk_i  ,
        // dma
        output             dma_cyc_i   [SHARES-1:0],
        output             dma_stb_i   [SHARES-1:0],
        output             dma_we_i    [SHARES-1:0],
        output     [31:0]  dma_adr_i   [SHARES-1:0],
        output     [31:0]  dma_dat_i   [SHARES-1:0],
        output     [ 3:0]  dma_sel_i   [SHARES-1:0],
        input              dma_stall_o [SHARES-1:0],
        input              dma_ack_o   [SHARES-1:0],
        input      [31:0]  dma_dat_o   [SHARES-1:0],
        input              dma_err_o   [SHARES-1:0],
        output             dma_rst_i   [SHARES-1:0]
    );

localparam PARAM_WIDTH = 32;
localparam BOX_WIDTH = 16;
localparam NB_SEEDS = 12;
localparam N_STAGES = 5;

localparam N_SHARES_2SHARE = 2;

localparam B2A_RND_SHARES_2SHARE = N_SHARES_2SHARE - 1;
localparam EXPAND_SHARES_2SHARE = N_SHARES_2SHARE;
localparam TRIANGLE_SHARES_2SHARE = 2 * (N_SHARES_2SHARE * (N_SHARES_2SHARE - 1) / 2);
localparam BOX_SHARES_2SHARE = (N_STAGES - 1) * 3 * (N_SHARES_2SHARE * (N_SHARES_2SHARE - 1) / 2) + 2 * (N_SHARES_2SHARE * (N_SHARES_2SHARE - 1) / 2);

localparam RND_SHARES_2SHARE = 2 * B2A_RND_SHARES_2SHARE + 2 * EXPAND_SHARES_2SHARE + 2 * TRIANGLE_SHARES_2SHARE;
localparam RND_SHARES_2SHARE_BOX = 2 * BOX_SHARES_2SHARE;
    
      
// wb <-> top
wire clk;
wire rst_n;
// wb <-> ctrl
wire [31:0] ctrl_addr, ctrl_wdata;
wire ctrl_we, ctrl_re;
wire [31:0] ctrl_rdata;
// ctrl <-> x2x
wire x2x_conv_mode;
wire x2x_data_type;
wire x2x_dual_mode;
// fsm <-> ctrl

wire start;
wire [3:0] cmd;
wire [31:0] din_addr [0:SHARES-1];
wire [31:0] dout_addr [0:SHARES-1];
wire [31:0] data_len;
wire [63:0] seed;
wire [31:0] modulus;
wire [31:0] modulus_twoc;
wire load_seed;
wire busy, done;
wire share_mode;
//wire arith_mode;
wire one_bit_mode;
wire [4:0] log_modulus;
wire [2:0] log_stride;
wire rej_samp;
wire seed_ip;
wire ctrl_prng_off;
wire fsm_prng_off;
wire [1:0] opcode;

// fsm <-> dma
wire [31:0] mem_addr    [SHARES-1:0];
wire [31:0] mem_i_data  [SHARES-1:0];
wire [31:0] mem_o_data  [SHARES-1:0];
wire        mem_re      [SHARES-1:0];
wire        mem_we      [SHARES-1:0];
wire        mem_i_valid [SHARES-1:0];
wire        mem_i_ready [SHARES-1:0];
wire        mem_o_ready [SHARES-1:0];

// fsm <-> x2x
wire x2x_valid_data;
wire x2x_ready_data;
wire x2x_ready_result;
wire x2x_valid_result;
wire x2x_valid_rng;
wire [PARAM_WIDTH - 1 : 0] x2x_fresh_rnd_shares        [RND_SHARES_2SHARE - 1 : 0];
wire [BOX_WIDTH - 1 : 0]   x2x_fresh_rnd_shares_8bit   [RND_SHARES_2SHARE_BOX - 1 : 0];

wire [PARAM_WIDTH - 1 : 0] x2x_original_data    [2 - 1 : 0][N_SHARES_2SHARE - 1:0];
wire [PARAM_WIDTH - 1 : 0] x2x_converted_data   [2 - 1 : 0][N_SHARES_2SHARE - 1:0];

wire [31:0] rnd_ref;

wire [31:0] modulus_complement;
assign modulus_complement = (32'hFFFFFFFF ^ modulus) + 1;

wire [PARAM_WIDTH-1:0] modulus_half;
assign modulus_half = modulus >> 1;
assign fsm_prng_off = (PRNG_OFF_EN) ? ctrl_prng_off : 1'b0;



x2x_acc_op_core #(
    .HALFCYCLE          (HALFCYCLE      ),
    .PARAM_WIDTH        (PARAM_WIDTH    ),
    .N_SHARES           (2              ),
    .RND_SHARES         (RND_SHARES_2SHARE),
    .RND_SHARES_BOX    (RND_SHARES_2SHARE_BOX),
    .BOX_WIDTH (BOX_WIDTH),
    .RND_SHARES_2SHARE(RND_SHARES_2SHARE),
    .RND_SHARES_2SHARE_BOX(RND_SHARES_2SHARE_BOX)
) x2x_acc_op_core (
    .clk                    (clk           ),
    .rst_n                  (rst_n         ),
    // x2x <- ctrl
    .conv_mode        (x2x_conv_mode),
    .data_type         (x2x_data_type),
    .dual_mode              (x2x_dual_mode & !x2x_data_type),
    // x2x <-> fsm
    .valid_data          (x2x_valid_data),
    .ready_data          (x2x_ready_data),
    .ready_result           (x2x_ready_result),
    .valid_result           (x2x_valid_result),
    .valid_rng              (x2x_valid_rng),
    
    .modulus(modulus),
    .modulus_complement(modulus_complement),
    .modulus_half(modulus_half),
    .log_modulus(log_modulus),
        
    .x2x_fresh_rnd_shares       (x2x_fresh_rnd_shares),//TBC
    .x2x_fresh_rnd_shares_8bit  (x2x_fresh_rnd_shares_8bit),//TBC
    .original_data          (x2x_original_data),
    .converted_data         (x2x_converted_data),
    .opcode(opcode),
    
    .rnd_ref(rnd_ref)
);

        
        
        
        
(* dont_touch *) 
x2x_acc_fsm #(
    .SHARES       (SHARES     ),
    .LOGL         (LOGL       ),
    .PARAM_WIDTH  (PARAM_WIDTH),
    .N_SHARES     (N_SHARES_2SHARE   ),
    .RND_SHARES_2SHARE         (RND_SHARES_2SHARE),
    .RND_SHARES_2SHARE_BOX    (RND_SHARES_2SHARE_BOX),
    .BOX_WIDTH (BOX_WIDTH)
) x2x_acc_fsm_inst (
    .clk             (clk              ),
    .rst_n           (rst_n            ),
    // fsm <-> ctrl
    .ctrl_start      (start            ),
    .ctrl_cmd        (cmd              ),
    .ctrl_busy       (busy             ),
    .ctrl_seed_ip    (seed_ip             ),////////
    .ctrl_done       (done             ),
    .ctrl_din_addr   (din_addr         ),
    .ctrl_dout_addr  (dout_addr        ),
    .ctrl_data_len   (data_len         ),
    .ctrl_seed  (seed        ),
    .modulus (modulus),
	.modulus_complement(modulus_complement),
    .modulus_half(modulus_half),										
    .ctrl_load_seed  (load_seed        ),
    .ctrl_share_mode(share_mode),
    .ctrl_arith_mode(arith_mode),
    .ctrl_conv_mode(x2x_conv_mode),
    .ctrl_dual_mode(x2x_dual_mode),
    .ctrl_data_type(x2x_data_type),
    .ctrl_one_bit_mode(one_bit_mode),///////////
    .log_modulus(log_modulus),  ///////////
    .log_stride(log_stride),  ///////////
    .ctrl_rej_samp(rej_samp),////////// 
    .ctrl_prng_off(fsm_prng_off),
    .opcode(opcode),
    // fsm <-> dma
    .mem_addr        (mem_addr         ),
    .mem_re          (mem_re           ),
    .mem_we          (mem_we           ),
    .mem_i_valid     (mem_i_valid      ),
    .mem_i_ready     (mem_i_ready      ),
    .mem_i_data      (mem_i_data       ),
    .mem_o_ready     (mem_o_ready      ),
    .mem_o_data      (mem_o_data       ),
    // fsm <-> x2x   
    .x2x_valid_data             (x2x_valid_data),
    .x2x_ready_data             (x2x_ready_data),
    .x2x_ready_result           (x2x_ready_result),
    .x2x_valid_result           (x2x_valid_result),
	.x2x_valid_rng           (x2x_valid_rng),										 
   
    .x2x_fresh_rnd_shares       (x2x_fresh_rnd_shares),
    .x2x_fresh_rnd_shares_8bit  (x2x_fresh_rnd_shares_8bit),
    .x2x_original_data          (x2x_original_data),
    .x2x_converted_data         (x2x_converted_data),
    .rnd_ref(rnd_ref)

);


for (genvar i = 0; i < SHARES; i = i + 1) begin
    x2x_acc_dma x2x_acc_dma_inst 
    (
        .clk         (clk        ),
        .rst_n       (rst_n      ),
        // dma <-> fsm
        .addr        (mem_addr   [i]),
        .re          (mem_re     [i]),
        .we          (mem_we     [i]),
        .i_data      (mem_i_data [i]),
        .i_valid     (mem_i_valid[i]),
        .i_ready     (mem_i_ready[i]),
        .o_ready     (mem_o_ready[i]),
        .o_data      (mem_o_data [i]),
        // dma connections
        .dma_cyc_i   (dma_cyc_i  [i]),
        .dma_stb_i   (dma_stb_i  [i]),
        .dma_we_i    (dma_we_i   [i]),
        .dma_adr_i   (dma_adr_i  [i]),
        .dma_dat_i   (dma_dat_i  [i]),
        .dma_sel_i   (dma_sel_i  [i]),
        .dma_stall_o (dma_stall_o[i]),
        .dma_ack_o   (dma_ack_o  [i]),
        .dma_dat_o   (dma_dat_o  [i]),
        .dma_err_o   (dma_err_o  [i]),
        .dma_rst_i   (dma_rst_i  [i])
    );
end


x2x_acc_wb x2x_acc_wb_inst (
    // wb <-> ctrl
    .addr       (ctrl_addr  ),
    .wdata      (ctrl_wdata ),
    .we         (ctrl_we    ),
    .re         (ctrl_re    ),
    .rdata      (ctrl_rdata ),
    // wb <-> top
    .clk        (clk        ),
    .rst_n      (rst_n      ),
    // wishbone connections
    .wb_cyc_i   (wb_cyc_i   ),
    .wb_stb_i   (wb_stb_i   ),
    .wb_we_i    (wb_we_i    ),
    .wb_adr_i   (wb_adr_i   ),
    .wb_dat_i   (wb_dat_i   ),
    .wb_sel_i   (wb_sel_i   ),
    .wb_stall_o (wb_stall_o ),
    .wb_ack_o   (wb_ack_o   ),
    .wb_dat_o   (wb_dat_o   ),
    .wb_err_o   (wb_err_o   ),
    .wb_rst_i   (wb_rst_i   ),
    .wb_clk_i   (wb_clk_i   )
);


x2x_acc_ctrl #(
    .BASE_ADDR  (BASE_ADDR  ),
    .SHARES     (SHARES     ),
    .PRNG_OFF_EN(PRNG_OFF_EN)
) x2x_acc_ctrl_inst (
    .clk       (clk       ),
    .rst_n     (rst_n     ),
    // ctrl <-> wb
    .addr      (ctrl_addr ),
    .wdata     (ctrl_wdata),
    .we        (ctrl_we   ),
    .re        (ctrl_re   ),
    .rdata     (ctrl_rdata),
    // ctrl -> x2x
    .conv_mode (x2x_conv_mode),
    .data_type (x2x_data_type),
    .dual_mode (x2x_dual_mode),
    // ctrl <-> fsm
    .start     (start),
    .share_mode     (share_mode),//////mask
    .arith_mode    (arith_mode),
    .one_bit_mode(one_bit_mode),///////////
    .log_modulus(log_modulus),  ///////////
    .log_stride(log_stride),  ///////////
    .rej_samp(rej_samp),//////////   
    .opcode(opcode),
        
    .din_addr  (din_addr  ),
    .dout_addr (dout_addr ),
    .data_len  (data_len  ),
    .seed (seed ),
    .modulus (modulus),
    .load_seed (load_seed ),
    .busy      (busy      ),
    .seed_ip   (seed_ip),//////
    .prng_off  (ctrl_prng_off),
    .done      (done      )
);



       
       


endmodule