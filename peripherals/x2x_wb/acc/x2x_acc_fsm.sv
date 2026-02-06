`include "x2x_acc.vh"


module x2x_acc_fsm
   #(
        parameter SHARES  = 2 ,
        parameter LOGL    = 10,
        parameter PARAM_WIDTH = 0,
        parameter RND_SHARES_8bit = 0,
        parameter N_SHARES = 0,
        
        parameter BURST_LEN = 16, // MINIMUM 2
        
        parameter RND_SHARES_2SHARE = 0,
        parameter RND_SHARES_2SHARE_BOX = 0,
        parameter BOX_WIDTH = 0,
        parameter PRNG_INIT_CC    = 30
    )
    (
        input                   clk                        ,
        input                   rst_n                      ,
        // fsm <-> ctrl        
        input                   ctrl_start                 ,
        input      [       3:0] ctrl_cmd                   ,
        output reg              ctrl_busy                  ,
        output reg ctrl_seed_ip ,//////
        output reg              ctrl_done                  ,
        input      [      31:0] ctrl_din_addr  [0:SHARES-1],
        input      [      31:0] ctrl_dout_addr [0:SHARES-1],
        input      [      31:0] ctrl_data_len              ,
        input      [      63:0] ctrl_seed             , 
        input      [31:0] modulus,
		input [31:0] modulus_complement,
        input [31:0] modulus_half,								
        input                   ctrl_load_seed             ,
        input ctrl_share_mode,
        input ctrl_dual_mode,
        input ctrl_conv_mode, // 0 -> A2B, 1 -> B2A
        input ctrl_data_type, // 0 -> power-of-two, 1 -> prime
        input ctrl_arith_mode, // 0 -> unsigned 1 -> signed
        input ctrl_one_bit_mode,///////////
        input [4:0] log_modulus,  ///////////
        input [2:0] log_stride,  ///////////
        input ctrl_rej_samp,//////////
        input ctrl_prng_off, 
        input [1:0] opcode,
        // fsm <-> dma        
        output reg [      31:0] mem_addr       [SHARES-1:0],
        output reg              mem_re         [SHARES-1:0],
        output reg              mem_we         [SHARES-1:0],
        input      [      31:0] mem_i_data     [SHARES-1:0],
        input                   mem_i_valid    [SHARES-1:0],
        input                   mem_i_ready    [SHARES-1:0],
        output reg [      31:0] mem_o_data     [SHARES-1:0],
        input                   mem_o_ready    [SHARES-1:0],
        // fsm <-> x2x        
        output reg              x2x_valid_data             ,
        input                   x2x_ready_data             ,
        output reg              x2x_ready_result           , 
        input                   x2x_valid_result           , 
        
        output reg              x2x_valid_rng              ,
        
        output [PARAM_WIDTH - 1 : 0]             x2x_fresh_rnd_shares   [RND_SHARES_2SHARE - 1 : 0]    ,
        output [BOX_WIDTH-1:0]            x2x_fresh_rnd_shares_8bit [RND_SHARES_2SHARE_BOX - 1 : 0] ,
        output reg [PARAM_WIDTH - 1 : 0]             x2x_original_data [2 - 1 : 0] [N_SHARES - 1:0]        ,
        input      [PARAM_WIDTH - 1 : 0]             x2x_converted_data [2 - 1 : 0][N_SHARES - 1:0],
        output [31:0] rnd_ref
    );


localparam ST_IDLE         = 4'd0;
localparam ST_FETCH_DATA_0 = 4'd1;
localparam ST_FETCH_DATA_1 = 4'd2;
localparam ST_MASK_SEND    = 4'd3;
localparam ST_MASK_WAIT    = 4'd4;
localparam ST_PUT_DATA_0   = 4'd5;
localparam ST_PUT_DATA_1   = 4'd6;
localparam ST_DONE         = 4'd7;
localparam ST_DELAY        = 4'd8;
localparam ST_PRNG         = 4'd14;
localparam ST_RESET        = 4'd15;

localparam WORD_PAD        = 32-PARAM_WIDTH;
localparam HALF_PAD        = 32-PARAM_WIDTH;

localparam LOGB = $clog2(BURST_LEN);
localparam LOGP = $clog2(PRNG_INIT_CC);


reg [3:0] fsm_state, fsm_next_state;
wire [LOGL-1:0] ctr_array;

reg [LOGB:0] ctr_block_r_mem;
reg ctr_block_r_mem_rst, ctr_block_r_mem_inc;

reg [LOGB:0] ctr_block_r_reg;
reg [LOGB:0] ctr_block_r_reg_q;
reg ctr_block_r_reg_rst, ctr_block_r_reg_inc;

reg [LOGB:0] ctr_block_w_mem;
reg [LOGB:0] ctr_block_w_mem_q;
reg ctr_block_w_mem_rst, ctr_block_w_mem_inc;

reg [LOGB:0] ctr_block_w_reg;
reg [LOGB:0] ctr_block_w_reg_q;
reg ctr_block_w_reg_rst, ctr_block_w_reg_inc;


reg [LOGL-1:0] ctr_iter;
reg ctr_iter_rst, ctr_iter_inc;

reg [31:0] ctr_onebit_addr_off_0, ctr_onebit_addr_off_1;
reg ctr_onebit_addr_off_0_rst, ctr_onebit_addr_off_0_inc;
reg ctr_onebit_addr_off_1_rst, ctr_onebit_addr_off_1_inc;

wire [31:0] onebit_inc_amount;


reg [LOGP-1:0] ctr_prng;

reg [31:0] bit_calc;
reg [2:0] reg_index;
reg [5:0] bit_index;
reg [31:0] addr_offset;
reg onebit_addr_routine;

reg [31:0] shares [SHARES - 1 : 0][BURST_LEN - 1:0];
reg [31:0] shares_one_bit [SHARES - 1 : 0][1:0];
// reg write_s[0], write_s[1];
reg write_s[SHARES-1:0];

wire dualprime;

reg wdone, wdone_set, wdone_clr;


reg rnd_ready;
wire rnd_ref_ready;
wire rnd_x2x_ready;

wire rng_non_zero;

reg reg_wen;


assign onebit_inc_amount = ctrl_dual_mode ? (ctrl_data_len >> (log_stride + 1)) : (ctrl_data_len >> log_stride);
assign dualprime = ctrl_dual_mode & ctrl_data_type;

assign ctr_array = ctr_iter << LOGB;


always @(*)
begin
    case(opcode)
    `X2X_CMD_PRNG  : rnd_ready = rnd_ref_ready;
    `X2X_CMD_X2X   : rnd_ready = rnd_x2x_ready;
    `X2X_CMD_REF   : rnd_ready = rnd_ref_ready;
    `X2X_CMD_REFX2X: rnd_ready = rnd_ref_ready & rnd_x2x_ready;
    endcase
end

assign rng_non_zero = (opcode == `X2X_CMD_PRNG) ? ctrl_share_mode : 1'b0;

x2x_acc_rng x2x_acc_rng_inst (
    .clk(clk),
    .rst_n(rst_n),
    .modulus(modulus),
    .ctrl_seed(ctrl_seed),
    .ctrl_load_seed(ctrl_load_seed),
    .ctrl_conv_mode(ctrl_conv_mode), 
    .ctrl_data_type(ctrl_data_type), 
    .ctrl_dual_mode(ctrl_dual_mode),
    .log_modulus(log_modulus),  
    .ctrl_rej_samp(ctrl_rej_samp),
    .ctrl_nonzero(rng_non_zero),
    .ctrl_prng_off(ctrl_prng_off),
    .x2x_fresh_rnd_shares(x2x_fresh_rnd_shares),
    .x2x_fresh_rnd_shares_8bit(x2x_fresh_rnd_shares_8bit),
    .rnd_x2x_ready(rnd_x2x_ready),
    .rnd_ref(rnd_ref),
    .rnd_ref_ready(rnd_ref_ready)
);



always @(posedge clk) begin
    if (!rst_n) begin
        fsm_state <= ST_IDLE;
    end else begin
        fsm_state <= fsm_next_state;
    end
end

always @(*) begin
    fsm_next_state = fsm_state;
    // ctrl
    ctrl_busy    = 1'b1; // (fsm_state != ST_IDLE)&&(fsm_state != ST_PRNG)&&(fsm_state != ST_RESET);
    ctrl_seed_ip = 1'b0;
    ctrl_done = 1'b0;
    // mem
    for (int i = 0; i < SHARES; i = i + 1) begin
        mem_addr[i] = 32'd0;
        mem_re[i] = 1'b0;
        mem_we[i] = 1'b0;
        mem_o_data[i] = 32'd0;
    end
    // mem_addr = 32'd0;
    // mem_re = 1'b0;
    // mem_we = 1'b0;
    // mem_o_data = reg_s0r;//0;
    // internal
    ctr_block_r_mem_rst = 1'b0;
    ctr_block_r_mem_inc = 1'b0;
    ctr_block_r_reg_rst = 1'b0;
    ctr_block_r_reg_inc = 1'b0;
    ctr_block_w_mem_rst = 1'b0;
    ctr_block_w_mem_inc = 1'b0;
    ctr_block_w_reg_rst = 1'b0;
    ctr_block_w_reg_inc = 1'b0;
    ctr_iter_rst = 1'b0;
    ctr_iter_inc = 1'b0;
    ctr_onebit_addr_off_0_rst = 1'b0;
    ctr_onebit_addr_off_0_inc = 1'b0;
    ctr_onebit_addr_off_1_rst = 1'b0;
    ctr_onebit_addr_off_1_inc = 1'b0;
    for (int i = 0; i < SHARES; i = i + 1) begin
        write_s[i] = 0;
    end
    x2x_valid_data = 0;
    x2x_ready_result = 0;
    x2x_valid_rng = 0;    
    
    x2x_original_data[0][0] = 0;
    x2x_original_data[0][1] = 0;
    x2x_original_data[1][0] = 0;
    x2x_original_data[1][1] = 0;
    
    reg_index = 0;
    bit_index = 0;
    bit_calc = 0;
    addr_offset = ctr_array + ctr_block_r_mem;
   
    wdone_set = 0;
    wdone_clr = 0;
    
    onebit_addr_routine = 0;

    reg_wen = 0;

    case(fsm_state)
    
    ST_IDLE:
    begin
        ctrl_busy = 1'b0;
        if(ctrl_start)
        begin
            if(opcode != `X2X_CMD_PRNG)
                    fsm_next_state = ST_FETCH_DATA_0;
            else
                    fsm_next_state = ST_MASK_SEND;
        end
        else if(ctrl_load_seed)
            fsm_next_state = ST_PRNG;     
    end
    ST_PRNG:
    begin
        ctrl_busy = 1'b0;
        ctrl_seed_ip = 1'b1;
        // mem_o_data[0] = shares[0][ctr_block_r_mem];
        if(ctr_prng == PRNG_INIT_CC)
            fsm_next_state = ST_IDLE;
    end   
	ST_FETCH_DATA_0:
    begin
        //////////////////////// READ FROM MEM //////////////////////////
        if(mem_i_ready[0])
        begin
            if(ctrl_one_bit_mode)
            begin
                if(ctrl_dual_mode)
                    mem_addr[0] = ctrl_din_addr[0] + (ctr_iter << 2) + (ctr_block_r_mem << 2);
                else
                    mem_addr[0] = ctrl_din_addr[0] + (ctr_iter << 1) + (ctr_block_r_mem << 2);

                if(ctr_block_r_mem <= 1) begin
                    mem_re[0] = 1;      
                    ctr_block_r_mem_inc = 1;
                end
            
            end
            else
            begin
                mem_addr[0] = ctrl_din_addr[0] + ((ctr_array + ctr_block_r_mem) << 2);
				if(ctr_block_r_mem <= (BURST_LEN - 1))
				begin
                    mem_re[0] = 1;      
                    ctr_block_r_mem_inc = 1;
				end
            end
       end
       //////////////////////// WRITE TO REG //////////////////////////
	   if(mem_i_valid[0])
	   begin
			ctr_block_w_mem_inc = 1;
			write_s[0] = 1;
            if(ctrl_one_bit_mode)
            begin
                if(ctr_block_w_mem == 1)
                begin
                    fsm_next_state = ST_FETCH_DATA_1;
                    ctr_block_w_mem_rst = 1;
                    ctr_block_r_mem_rst = 1;
                end
            end
            else
            begin
                if(ctr_block_w_mem == (BURST_LEN - 1))
                begin
                    fsm_next_state = ST_FETCH_DATA_1;
                    ctr_block_w_mem_rst = 1;
                    ctr_block_r_mem_rst = 1;
                end
            end
	   end
    end
	ST_FETCH_DATA_1:
    begin
        if(!ctrl_share_mode)
        begin
            //////////////////////// READ FROM MEM //////////////////////////
            if(mem_i_ready[1])
            begin
                if(ctrl_one_bit_mode)
                begin
                    if(ctrl_dual_mode)
                        mem_addr[1] = ctrl_din_addr[1] + (ctr_iter << 2) + (ctr_block_r_mem << 2);
                    else
                        mem_addr[1] = ctrl_din_addr[1] + (ctr_iter << 1) + (ctr_block_r_mem << 2);

                    if(ctr_block_r_mem <= 1) begin
                        mem_re[1] = 1;      
                        ctr_block_r_mem_inc = 1;
                    end                
                end
                else
                begin
                    mem_addr[1] = ctrl_din_addr[1] + ((ctr_array + ctr_block_r_mem) << 2);

                    if(ctr_block_r_mem <= (BURST_LEN - 1)) begin
                        mem_re[1] = 1;      
                        ctr_block_r_mem_inc = 1;
                    end
                end
            end
            //////////////////////// WRITE TO REG //////////////////////////
            if(mem_i_valid[1])
            begin
                ctr_block_w_mem_inc = 1;
                write_s[1] = 1;
                if(ctrl_one_bit_mode)
                begin
                    if(ctr_block_w_mem == 1)
                    begin
                        fsm_next_state = ST_MASK_SEND;
                        ctr_block_w_mem_rst = 1;
                        ctr_block_r_mem_rst = 1;
                    end
                end
                else
                begin
                    if(ctr_block_w_mem == (BURST_LEN - 1))
                    begin
                        fsm_next_state = ST_MASK_SEND;
                        ctr_block_w_mem_rst = 1;
                        ctr_block_r_mem_rst = 1;
                    end
                end
            end
        end
        else
        begin
            //////////////////////// WRITE TO REG //////////////////////////
            write_s[1] = 1;
            ctr_block_w_mem_inc = 1;
            if(ctr_block_w_mem == (BURST_LEN - 1))
            begin
                fsm_next_state = ST_MASK_SEND;
                ctr_block_w_mem_rst = 1;
                ctr_block_r_mem_rst = 1;
            end
        end
    end
    ST_MASK_SEND:
    begin
        reg_wen = 1;
        if(opcode != `X2X_CMD_PRNG)
        begin
            if(ctrl_one_bit_mode)
            begin
                if(ctrl_dual_mode)
                    bit_calc = ((((ctr_array << 1)+ ctr_block_r_reg) & 6'h3f) << log_stride);
                else
                    bit_calc = (((ctr_array + ctr_block_r_reg) & 6'h3f) << log_stride);
                
                bit_index = (bit_calc[5:0] + bit_calc[31:6]) & 6'h3f;
                x2x_original_data[0][0] = {{(PARAM_WIDTH - 1){1'b0}}, shares_one_bit[0][bit_index[5]][bit_index[4:0]]};
                x2x_original_data[0][1] = {{(PARAM_WIDTH - 1){1'b0}}, shares_one_bit[1][bit_index[5]][bit_index[4:0]]};
            end
            else
            begin
                if(dualprime)
                begin
                    if(ctr_block_r_reg[0])
                    begin
                        x2x_original_data[0][0] = shares[0][(ctr_block_r_reg >> 1)][31:16];
                        x2x_original_data[0][1] = shares[1][(ctr_block_r_reg >> 1)][31:16];
                    end
                    else
                    begin
                        x2x_original_data[0][0] = shares[0][(ctr_block_r_reg >> 1)][15:0];
                        x2x_original_data[0][1] = shares[1][(ctr_block_r_reg >> 1)][15:0];
                    end
                end
                else
                begin
                    if(!ctrl_conv_mode & !ctrl_arith_mode  & ctrl_data_type) // A2B Unsigned
                    begin
                        
                        x2x_original_data[0][0] = shares[0][ctr_block_r_reg][PARAM_WIDTH-1:0];
                        x2x_original_data[0][1] = shares[1][ctr_block_r_reg][PARAM_WIDTH-1:0];
                    end
                    else if (ctrl_dual_mode && !ctrl_data_type)
                    begin
                        x2x_original_data[0][0] = shares[0][ctr_block_r_reg][16-1:0];
                        x2x_original_data[0][1] = shares[1][ctr_block_r_reg][16-1:0];
                    end
                    else
                    begin
                        x2x_original_data[0][0] = shares[0][ctr_block_r_reg][PARAM_WIDTH-1:0];
                        x2x_original_data[0][1] = shares[1][ctr_block_r_reg][PARAM_WIDTH-1:0];
                    end
                end
                if(ctrl_dual_mode && !ctrl_data_type)
                begin
                    x2x_original_data[1][0] = shares[0][ctr_block_r_reg][31:16];
                    x2x_original_data[1][1] = shares[1][ctr_block_r_reg][31:16];
                end
            end
        end 
        x2x_valid_data = rnd_ready;
        x2x_ready_result = 1;
        if(x2x_ready_data & rnd_ready)
            ctr_block_r_reg_inc = 1;
        
        if(ctrl_one_bit_mode)
        begin
            if((ctrl_dual_mode && (ctr_block_r_reg == ((BURST_LEN << 1) - 1)) || 
               !ctrl_dual_mode && (ctr_block_r_reg == ( BURST_LEN       - 1))) && rnd_ready)
            begin
                fsm_next_state = ST_MASK_WAIT;
                ctr_block_r_reg_rst = 1;
            end
        end
        else
        begin
            if((dualprime && (ctr_block_r_reg == ((BURST_LEN << 1) - 1)) ||
               !dualprime && (ctr_block_r_reg == ( BURST_LEN       - 1))) && rnd_ready)
            begin
                fsm_next_state = ST_MASK_WAIT;
                ctr_block_r_reg_rst = 1;
            end
        end

        if(x2x_valid_result) begin
            ctr_block_w_reg_inc = 1;
            /////////////////////// OUTPUT FOR SHARE 0 ////////////////////////
            // for dual prime, we send output for every two cycle
            if (ctr_block_w_reg && (!dualprime || (dualprime && !ctr_block_w_reg[0]))) begin // can't produce output at ctr_block_w_reg == 0
                if (opcode == `X2X_CMD_PRNG) begin
                    mem_o_data[1] = shares[1][ctr_block_r_mem];
                    mem_we[1] = 1;
                    mem_addr[1] = ctrl_dout_addr[1] + (ctr_array << 2)  + (ctr_block_r_mem << 2);
                    if(mem_o_ready[1])
                    begin
                        ctr_block_r_mem_inc = 1;
                    end
                end
                else begin
                    mem_o_data[0] = shares[0][ctr_block_r_mem];
                    mem_we[0] = 1;
                    if (ctrl_one_bit_mode) // only address calculation changes in case of one-bit mode and otherwise
                    begin
                        onebit_addr_routine = 1;
                    end
                    else begin
                        mem_addr[0] = ctrl_dout_addr[0] + (ctr_array << 2)  + (ctr_block_r_mem << 2);
                    end
                    if(mem_o_ready[0])
                    begin
                        ctr_block_r_mem_inc = 1;
                    end
                end
            end
        end

    end
    ST_MASK_WAIT:
    begin
        reg_wen = 1;

        x2x_ready_result = 1;
        x2x_valid_rng = rnd_ready;
        if(x2x_valid_result)
        begin
            if(ctrl_one_bit_mode)
            begin
                if((ctrl_dual_mode && (ctr_block_w_reg == ((BURST_LEN << 1) - 1)) || 
                  (!ctrl_dual_mode && (ctr_block_w_reg == ( BURST_LEN       - 1)))))
                begin
                    fsm_next_state = ST_PUT_DATA_0;
                    ctr_block_w_reg_rst = 1;
                end 
                else
                begin
                    ctr_block_w_reg_inc = 1;
                end                
            end
            else
            begin
                if((dualprime && (ctr_block_w_reg == ((BURST_LEN << 1) - 1)) ||
                  (!dualprime && (ctr_block_w_reg == ( BURST_LEN       - 1)))))
                begin
                    if(opcode != `X2X_CMD_PRNG)
                    begin
                        fsm_next_state = ST_PUT_DATA_0;
                    end
                    else 
                    begin
                        fsm_next_state = ST_PUT_DATA_1;
                    end
                    // ctr_block_r_mem_rst = 1;
                    ctr_block_w_reg_rst = 1;
                end
                else 
                begin
                    ctr_block_w_reg_inc = 1;
                end
            end

            ///////////////////// OUTPUT FOR SHARE 0 ////////////////////////
            if (ctr_block_w_reg && (!dualprime || (dualprime && !ctr_block_w_reg[0]))) begin // can't produce output at ctr_block_w_reg == 0
                if (opcode == `X2X_CMD_PRNG) begin
                    mem_o_data[1] = shares[1][ctr_block_r_mem];
                    mem_we[1] = 1;
                    mem_addr[1] = ctrl_dout_addr[1] + (ctr_array << 2)  + (ctr_block_r_mem << 2);
                    if(mem_o_ready[1])
                    begin
                        // below comment applies here as well.
                        ctr_block_r_mem_inc = 1;
                    end
                end
                else begin
                    mem_o_data[0] = shares[0][ctr_block_r_mem];
                    mem_we[0] = 1;
                    if(ctrl_one_bit_mode) // only address calculation changes in case of one-bit mode and otherwise
                    begin
                        onebit_addr_routine = 1;
                    end
                    else begin
                        mem_addr[0] = ctrl_dout_addr[0] + (ctr_array << 2)  + (ctr_block_r_mem << 2);
                    end
                    if(mem_o_ready[0])
                    begin
                        // we ignore the rst case as it would be handled above.
                        // this loop can't iterate more than the above loop, so state transition as well as ctr_block_r_mem_rst will be done as some more data to write for share 0 will be left.
                        // in other words, ST_PUT_DATA_0 will always be entered.
                        ctr_block_r_mem_inc = 1;
                    end                    
                end
            end
        end


    end
    ST_PUT_DATA_0:
    begin
        // reg0_rsel = 1;
        mem_o_data[0] = shares[0][ctr_block_r_mem];
        if(ctrl_one_bit_mode)
        begin
            onebit_addr_routine = 1;
            if(mem_o_ready[0])
            begin
                if(ctr_block_r_mem == (BURST_LEN - 1))
                begin
                    fsm_next_state = ST_PUT_DATA_1;
                    ctr_block_r_mem_rst = 1;
                end
                else
                begin
                    ctr_block_r_mem_inc = 1;
                end
            end
        end
        else
        begin
            mem_addr[0] = ctrl_dout_addr[0] + (ctr_array << 2)  + (ctr_block_r_mem << 2);
            if(mem_o_ready[0])
            begin
                if(ctr_block_r_mem == (BURST_LEN - 1))
                begin
                    fsm_next_state = ST_PUT_DATA_1;
                    ctr_block_r_mem_rst = 1;
                end
                else
                begin
                    ctr_block_r_mem_inc = 1;
                end
            end
        end
        
        mem_we[0] =  1;

    end
    ST_PUT_DATA_1:
    begin
        mem_o_data[1] = shares[1][ctr_block_r_mem];
        if(ctrl_one_bit_mode)
        begin
		    if(ctrl_dual_mode)
            begin
                if((addr_offset[4:0] & ((1 << (5 - log_stride)) - 1)) == ((1 << (5 - log_stride)) - 1))
                begin
                    if((addr_offset[4:0] >> (5 - log_stride)) == ((1 << log_stride)-1))
                       ctr_onebit_addr_off_1_rst = 1; 
                    else
                       ctr_onebit_addr_off_1_inc = 1;
                end
                mem_addr[1] = ctrl_dout_addr[1] + (((addr_offset[31:5] << (5 - log_stride)) + (addr_offset[4:0] & ((1 << (5 - log_stride)) - 1)) + ctr_onebit_addr_off_1) << 2);
            end
            else 
            begin
                if((addr_offset[5:0] & ((1 << (6 - log_stride)) - 1)) == ((1 << (6 - log_stride)) - 1))
                begin
                    if((addr_offset[5:0] >> (6 - log_stride)) == ((1 << log_stride)-1))
                       ctr_onebit_addr_off_1_rst = 1; 
                    else
                       ctr_onebit_addr_off_1_inc = 1;
                end
                mem_addr[1] = ctrl_dout_addr[1] + (((addr_offset[31:6] << (6 - log_stride)) + (addr_offset[5:0] & ((1 << (6 - log_stride)) - 1)) + ctr_onebit_addr_off_1) << 2);
            end
			if(mem_o_ready[1])
			begin
				if(ctr_block_r_mem == (BURST_LEN - 1))
				begin
					ctr_block_r_mem_rst = 1;
					ctr_iter_inc = 1;
					if(ctrl_dual_mode)
					begin
						if (ctr_array == ((ctrl_data_len >> 1) - BURST_LEN))
						begin
							fsm_next_state = ST_DONE;
						end
						else
						begin
							if(!ctr_iter[0])
                                fsm_next_state = ST_MASK_SEND;
                            else
                                fsm_next_state = ST_DELAY;
						end
					end
					else
					begin
						
                        if (ctr_array == (ctrl_data_len - BURST_LEN))
                        begin
                            fsm_next_state = ST_DONE;
                        end
                        else
                        begin
                            if(ctr_iter[1:0] != 2'h3)
                                fsm_next_state = ST_MASK_SEND;
                            else
                                fsm_next_state = ST_DELAY;
                        end
                    
					end	
				end
				else
				begin
					ctr_block_r_mem_inc = 1; 
				end
			end
			
		
        end
        else
        begin
            mem_addr[1] = ctrl_dout_addr[1] + (ctr_array << 2)  + (ctr_block_r_mem << 2);
            if(mem_o_ready[1])
            begin
                if(ctr_block_r_mem == (BURST_LEN - 1))
                begin
                    ctr_block_r_mem_rst = 1;
                    ctr_iter_inc = 1;
                    if(ctr_array == (ctrl_data_len - BURST_LEN))
                    begin
                        fsm_next_state = ST_DONE;
                    end
                    else
                    begin
                        if(opcode != `X2X_CMD_PRNG)
                            fsm_next_state = ST_DELAY;
                        else
                            fsm_next_state = ST_MASK_SEND;
                    end
                end
                else
                begin
                    ctr_block_r_mem_inc = 1; 
                end
            end
        end
        mem_we[1] = 1;
    end
    ST_DELAY: // delay state is needed to prevent collision of DMA1 write and DMA0 read
    begin
        fsm_next_state = ST_FETCH_DATA_0;
    end
    ST_DONE:
    begin
        ctr_block_r_mem_rst = 1;
        ctr_block_r_reg_rst = 1;
        ctr_block_w_mem_rst = 1;
        ctr_block_w_reg_rst = 1;
        ctr_iter_rst = 1;
        ctr_onebit_addr_off_0_rst = 1; 
        ctr_onebit_addr_off_1_rst = 1; 
        fsm_next_state = ST_IDLE;
        ctrl_done = 1;  
    end
    endcase


    if (onebit_addr_routine) begin
        if(ctrl_dual_mode)
        begin
            if((addr_offset[4:0] & ((1 << (5 - log_stride)) - 1)) == ((1 << (5 - log_stride)) - 1))
            begin
                if((addr_offset[4:0] >> (5 - log_stride)) == ((1 << log_stride)-1))
                    ctr_onebit_addr_off_0_rst = 1; 
                else
                    ctr_onebit_addr_off_0_inc = 1;
            end
            mem_addr[0] = ctrl_dout_addr[0] + (((addr_offset[31:5] << (5 - log_stride)) + (addr_offset[4:0] & ((1 << (5 - log_stride)) - 1)) + ctr_onebit_addr_off_0) << 2);
        end
        else 
        begin
            if((addr_offset[5:0] & ((1 << (6 - log_stride)) - 1)) == ((1 << (6 - log_stride)) - 1))
            begin
                if((addr_offset[5:0] >> (6 - log_stride)) == ((1 << log_stride)-1))
                    ctr_onebit_addr_off_0_rst = 1; 
                else
                    ctr_onebit_addr_off_0_inc = 1;
            end
            mem_addr[0] = ctrl_dout_addr[0] + (((addr_offset[31:6] << (6 - log_stride)) + (addr_offset[5:0] & ((1 << (6 - log_stride)) - 1)) + ctr_onebit_addr_off_0) << 2);
        end
    end



end



always @(posedge clk) begin
    if (!rst_n) begin
        ctr_block_r_mem <= {(LOGB+1){1'b0}};
    end
    else if (ctr_block_r_mem_rst) begin
        ctr_block_r_mem <= {(LOGB+1){1'b0}};
    end
    else if (ctr_block_r_mem_inc) begin
        ctr_block_r_mem <= ctr_block_r_mem + 1;
    end
end


always @(posedge clk) begin
    if (!rst_n) begin
        ctr_block_r_reg <= {(LOGB+1){1'b0}};
    end
    else if (ctr_block_r_reg_rst) begin
        ctr_block_r_reg <= {(LOGB+1){1'b0}};
    end
    else if (ctr_block_r_reg_inc) begin
        ctr_block_r_reg <= ctr_block_r_reg + 1;
    end
end


always @(posedge clk) begin
    ctr_block_r_reg_q <= ctr_block_r_reg;
end



always @(posedge clk) begin
    if (!rst_n) begin
        ctr_block_w_mem <= {(LOGB+1){1'b0}};
    end
    else if (ctr_block_w_mem_rst) begin
        ctr_block_w_mem <= {(LOGB+1){1'b0}};
    end
    else if (ctr_block_w_mem_inc) begin
        ctr_block_w_mem <= ctr_block_w_mem + 1;
    end 
end


always @(posedge clk) begin
    ctr_block_w_mem_q <= ctr_block_w_mem;
end

always @(posedge clk) begin
    if (!rst_n) begin
        ctr_block_w_reg <= {LOGB{1'b0}};
    end
    else if (ctr_block_w_reg_rst) begin
        ctr_block_w_reg <= {(LOGB+1){1'b0}};
    end
    else if (ctr_block_w_reg_inc) begin
        ctr_block_w_reg <= ctr_block_w_reg + 1;
    end
    
end


always @(posedge clk) begin
    ctr_block_w_reg_q <= ctr_block_w_reg;
end


always @(posedge clk) begin
    if (!rst_n) begin
        ctr_iter <= {LOGL{1'b0}};
    end
    else if (ctr_iter_rst) begin
        ctr_iter <= {LOGL{1'b0}};
    end
    else if (ctr_iter_inc) begin
        ctr_iter <= ctr_iter + 1;
    end
end

always @(posedge clk) begin
    if (!rst_n) begin
        ctr_onebit_addr_off_0 <= 32'd0;
    end
    else if (ctr_onebit_addr_off_0_rst) begin
        ctr_onebit_addr_off_0 <= 32'd0;
    end
    else if (ctr_onebit_addr_off_0_inc) begin
        ctr_onebit_addr_off_0 <= ctr_onebit_addr_off_0 + onebit_inc_amount;
    end
end

always @(posedge clk) begin
    if (!rst_n) begin
        ctr_onebit_addr_off_1 <= 32'd0;
    end
    else if (ctr_onebit_addr_off_1_rst) begin
        ctr_onebit_addr_off_1 <= 32'd0;
    end
    else if (ctr_onebit_addr_off_1_inc) begin
        ctr_onebit_addr_off_1 <= ctr_onebit_addr_off_1 + onebit_inc_amount;
    end
end



always @(posedge clk) begin
    if (!rst_n) begin
        ctr_prng <= 0;
    end
    else if (fsm_state == ST_PRNG) begin
        ctr_prng <= ctr_prng + 1;
    end
    else begin
        ctr_prng <= 0;
    end
end


for (genvar i = 0; i < SHARES; i = i + 1) begin
    always @(posedge clk) begin
        if (!rst_n) 
        begin
            // for (int i = 0; i < SHARES; i = i + 1)
            // begin
                shares_one_bit[i][0] <= 0;
                shares_one_bit[i][1] <= 0;
                for (int j = 0; j < BURST_LEN; j = j + 1)
                    shares[i][j] <= 0;
            // end
        end 
        else 
        begin
            if (write_s[i])
            begin
                if (ctrl_one_bit_mode)
                    shares_one_bit[i][ctr_block_w_mem] <= mem_i_data[i];
                else
                    shares[i][ctr_block_w_mem] <= (i == 1 && ctrl_share_mode) ? 0 : mem_i_data[i];
            end
            else if (x2x_valid_result && reg_wen)
            begin
                if(ctrl_one_bit_mode)
                begin
                    if(ctrl_dual_mode)
                    begin
                        if(ctr_block_w_reg[0])
                        begin
                            shares[i][(ctr_block_w_reg >> 1)][31:16] <= {{(16 - PARAM_WIDTH){1'b0}}, x2x_converted_data[0][i]};
                        end
                        else
                        begin
                            shares[i][(ctr_block_w_reg >> 1)][15:0] <= {{(16 - PARAM_WIDTH){1'b0}}, x2x_converted_data[0][i]};
                        end
                    end
                    else
                    begin
                        shares[i][ctr_block_w_reg] <= {{(32 - PARAM_WIDTH){1'b0}}, x2x_converted_data[0][i]};
                    end
                end
                else if(dualprime)
                begin
                    if(ctr_block_w_reg[0])
                    begin
                        shares[i][(ctr_block_w_reg >> 1)][31:16] <= {{(16 - PARAM_WIDTH){1'b0}}, x2x_converted_data[0][i]};
                    end
                    else
                    begin
                        shares[i][(ctr_block_w_reg >> 1)][15:0] <= {{(16 - PARAM_WIDTH){1'b0}}, x2x_converted_data[0][i]};
                    end
                end
                else if (!ctrl_dual_mode  && !ctrl_data_type  && !ctrl_conv_mode) 
                begin
                    shares[i][ctr_block_w_reg] <= x2x_converted_data[0][i];
                end
                else if (!ctrl_dual_mode) 
                begin
                    shares[i][ctr_block_w_reg] <= x2x_converted_data[0][i];
                end 
                else 
                begin
                    shares[i][ctr_block_w_reg] <= {x2x_converted_data[1][i][15:0], x2x_converted_data[0][i][15:0]};
                end
            end
        end
    end
end

endmodule