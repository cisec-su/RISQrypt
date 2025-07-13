module x2x_acc_top
   #(
        parameter BASE_ADDR       = 32'h1004_0050,
        parameter SHARES          = 2           ,
        parameter SLICES_PARALLEL = 16          ,
        parameter LESS_RAND       = 1           ,
        parameter LOGL            = 10
    )
    (
        // wishbone
        input              wb_cyc_i,
        input              wb_stb_i,
        input              wb_we_i,
        input      [31:0]  wb_adr_i,
        input      [31:0]  wb_dat_i,
        input      [ 3:0]  wb_sel_i,
        output             wb_stall_o,
        output             wb_ack_o,
        output     [31:0]  wb_dat_o,
        output             wb_err_o,
        input              wb_rst_i,
        input              wb_clk_i,
        // dma
        output             dma_cyc_i,
        output             dma_stb_i,
        output             dma_we_i,
        output     [31:0]  dma_adr_i,
        output     [31:0]  dma_dat_i,
        output     [ 3:0]  dma_sel_i,
        input              dma_stall_o,
        input              dma_ack_o,
        input      [31:0]  dma_dat_o,
        input              dma_err_o,
        output             dma_rst_i
    );

localparam LOGS = $rtoi($ceil($clog2(SHARES)));
localparam B    = 32;
localparam N    = 1600 / B;
localparam LOGN = $rtoi($ceil($clog2(N + 1)));

localparam N_SHARES = 2;
localparam N_STAGES = 4;
localparam LOG2_OF_q = 12;
localparam PARAM_WIDTH = LOG2_OF_q + 1;
localparam RND_SHARES = 2 * (N_SHARES - 1) + 2 * N_SHARES + 4 * (N_SHARES * (N_SHARES - 1) / 2);
localparam RND_SHARES_8bit = 2 * N_STAGES * 3 *(N_SHARES * (N_SHARES - 1) / 2);
   
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
wire [LOGL-1:0] data_len;
wire [63:0] seed;
wire [31:0] modulus;
wire load_seed;
wire busy, done;
wire share_mode;
//wire arith_mode;
wire one_bit_mode;
wire [4:0] log_modulus;
wire rej_samp;
wire seed_ip;

// fsm <-> dma
wire [31:0] mem_addr;
wire [31:0] mem_i_data;
wire [31:0] mem_o_data;
wire mem_re, mem_we;
wire mem_i_valid, mem_i_ready, mem_o_ready;

// fsm <-> x2x
wire x2x_valid_data;
wire x2x_ready_data;
wire x2x_ready_result;
wire x2x_valid_result;
wire [PARAM_WIDTH - 1 : 0]   x2x_fresh_rnd_shares        [RND_SHARES - 1 : 0];
wire [8 - 1 : 0]             x2x_fresh_rnd_shares_8bit   [RND_SHARES_8bit - 1 : 0];
wire [PARAM_WIDTH - 1 : 0] x2x_original_data    [2 - 1 : 0][N_SHARES - 1:0];
wire [PARAM_WIDTH - 1 : 0] x2x_converted_data   [2 - 1 : 0][N_SHARES - 1:0];



MaskConversion_HALFCYCLE_STREAM #(
    .HALFCYCLE          (1              ),
    .LOG2_OF_q          (12             ),
    .q                  (3329           ),
    .PARAM_WIDTH        (PARAM_WIDTH    ),
    .N_SHARES           (2              ),
    .N_STAGES           (4              ),
    .RND_SHARES         (RND_SHARES),
    .RND_SHARES_8bit    (2 * N_STAGES * 3 *(N_SHARES * (N_SHARES - 1) / 2))
) x2x_inst (
    .clk                    (clk           ),
    .rst_n                  (rst_n         ),
    // x2x <- ctrl
    .conversion_mode        (x2x_conv_mode),
    .data_type_mode         (x2x_data_type),
    .dual_mode              (x2x_dual_mode & !x2x_data_type),
    // x2x <-> fsm
    .valid_data             (x2x_valid_data),
    .ready_data             (x2x_ready_data),
    .ready_result           (x2x_ready_result),
    .valid_result           (x2x_valid_result),
   
    .fresh_rnd_shares       (x2x_fresh_rnd_shares),//TBC
    .fresh_rnd_shares_8bit  (x2x_fresh_rnd_shares_8bit),//TBC
    .original_data          (x2x_original_data),
    .converted_data         (x2x_converted_data)
);



x2x_acc_fsm #(
    .SHARES       (SHARES     ),
    .LOGL         (LOGL       ),
    .B            (B          ),
    .PARAM_WIDTH  (PARAM_WIDTH),
    .RND_SHARES   (RND_SHARES ),
    .RND_SHARES_8bit   (RND_SHARES_8bit ),
    .N_SHARES     (N_SHARES   )
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
    .ctrl_load_seed  (load_seed        ),
    .ctrl_share_mode(share_mode),
    .ctrl_arith_mode(arith_mode),
    .ctrl_conv_mode(x2x_conv_mode),
    .ctrl_dual_mode(x2x_dual_mode),
    .ctrl_data_type(x2x_data_type),
    .ctrl_one_bit_mode(one_bit_mode),///////////
    .log_modulus(log_modulus),  ///////////
    .ctrl_rej_samp(rej_samp),////////// 
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
   
    .x2x_fresh_rnd_shares       (x2x_fresh_rnd_shares),
    .x2x_fresh_rnd_shares_8bit  (x2x_fresh_rnd_shares_8bit),
    .x2x_original_data          (x2x_original_data),
    .x2x_converted_data         (x2x_converted_data)
);




x2x_acc_dma x2x_acc_dma_inst 
(
    .clk         (clk        ),
    .rst_n       (rst_n      ),
    // dma <-> fsm
    .addr        (mem_addr   ),
    .re          (mem_re     ),
    .we          (mem_we     ),
    .i_data      (mem_i_data     ),
    .i_valid     (mem_i_valid),
    .i_ready     (mem_i_ready),
    .o_ready     (mem_o_ready),
    .o_data      (mem_o_data     ),
    // dma connections
    .dma_cyc_i   (dma_cyc_i  ),
    .dma_stb_i   (dma_stb_i  ),
    .dma_we_i    (dma_we_i   ),
    .dma_adr_i   (dma_adr_i  ),
    .dma_dat_i   (dma_dat_i  ),
    .dma_sel_i   (dma_sel_i  ),
    .dma_stall_o (dma_stall_o),
    .dma_ack_o   (dma_ack_o  ),
    .dma_dat_o   (dma_dat_o  ),
    .dma_err_o   (dma_err_o  ),
    .dma_rst_i   (dma_rst_i  )
);


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
    .BASE_ADDR (BASE_ADDR),
    .SHARES    (SHARES   ),
    .LOGL      (LOGL     )
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
    .rej_samp(rej_samp),//////////   
        
    .din_addr  (din_addr  ),
    .dout_addr (dout_addr ),
    .data_len  (data_len  ),
    .seed (seed ),
    .modulus (modulus),
    .load_seed (load_seed ),
    .busy      (busy      ),
    .seed_ip   (seed_ip),//////
    .done      (done      )
);



       
       


endmodule