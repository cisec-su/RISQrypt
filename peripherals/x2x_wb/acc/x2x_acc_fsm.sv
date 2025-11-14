`include "x2x_acc.vh"


module x2x_acc_fsm
   #(
        parameter SHARES  = 2 ,
        parameter B       = 32,
        parameter LOGL    = 10,
        parameter PARAM_WIDTH = 0,
        parameter RND_SHARES_8bit = 0,
        parameter N_SHARES = 0,
        
        parameter BURST_LEN = 16, // MINIMUM 2
        
        parameter RND_SHARES_2SHARE = 0,
        parameter RND_SHARES_2SHARE_BOX = 0,
        parameter BOX_WIDTH = 0
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
        output reg [      31:0] mem_addr                   ,
        output reg              mem_re                     ,
        output reg              mem_we                     ,
        input      [      31:0] mem_i_data                 ,
        input                   mem_i_valid                ,
        input                   mem_i_ready                ,
        output reg [      31:0] mem_o_data                 ,
        input                   mem_o_ready                ,
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





localparam LOGS = $rtoi($ceil($clog2(SHARES)));
localparam N    = 1600 / B;
localparam LOGN = $rtoi($ceil($clog2(N + 1)));
localparam RATE_SHIFT = $rtoi($ceil($clog2(64 / B)));


localparam ST_IDLE                       = 4'd0;
localparam ST_FETCH_DATA_0_0             = 4'd1;
localparam ST_FETCH_DATA_0_1             = 4'd2;
localparam ST_FETCH_DATA_0_2             = 4'd3;
localparam ST_FETCH_DATA_1_0             = 4'd4;
localparam ST_FETCH_DATA_1_1             = 4'd5;
localparam ST_FETCH_DATA_1_2             = 4'd6;
localparam ST_MASK_SEND                  = 4'd7;
localparam ST_MASK_WAIT                  = 4'd8;
localparam ST_PUT_DATA_0                 = 4'd9;
localparam ST_PUT_DATA_1                 = 4'd10;
localparam ST_DONE                       = 4'd11;

localparam ST_TRIVIUM                     = 4'd14;
localparam ST_RESET                       = 4'd15;

localparam TRIVIUM_INIT_CC                = 12'd30;

localparam WORD_PAD                       = 32-PARAM_WIDTH;
localparam HALF_PAD                       = 32-PARAM_WIDTH;

reg [3:0] fsm_state, fsm_next_state;
reg [LOGL-1:0] ctr_array;
reg ctr_array_rst, ctr_array_inc, ctr_array_inc_b;

reg [LOGL-1:0] ctr_block_r_mem;
reg [LOGL-1:0] ctr_block_r_mem_q;
reg ctr_block_r_mem_rst, ctr_block_r_mem_inc;

reg [LOGL-1:0] ctr_block_r_reg;
reg [LOGL-1:0] ctr_block_r_reg_q;
reg ctr_block_r_reg_rst, ctr_block_r_reg_inc;

reg [LOGL-1:0] ctr_block_w_mem;
reg [LOGL-1:0] ctr_block_w_mem_q;
reg ctr_block_w_mem_rst, ctr_block_w_mem_inc;

reg [LOGL-1:0] ctr_block_w_reg;
reg [LOGL-1:0] ctr_block_w_reg_q;
reg ctr_block_w_reg_rst, ctr_block_w_reg_inc;


reg [LOGL-1:0] ctr_iter;
reg ctr_iter_rst, ctr_iter_inc;

reg [31:0] ctr_onebit_addr_off_0, ctr_onebit_addr_off_1;
reg ctr_onebit_addr_off_0_rst, ctr_onebit_addr_off_0_inc;
reg ctr_onebit_addr_off_1_rst, ctr_onebit_addr_off_1_inc;

wire [31:0] onebit_inc_amount;
assign onebit_inc_amount = ctrl_dual_mode ? (ctrl_data_len >> (log_stride + 1)) : (ctrl_data_len >> log_stride);


reg [11:0] ctr_trivium;

reg [31:0] bit_calc;
reg [2:0] reg_index;
reg [5:0] bit_index;
reg [31:0] addr_offset;

reg [31:0] shares [SHARES - 1 : 0][BURST_LEN - 1:0];
reg [31:0] shares_one_bit [SHARES - 1 : 0][1:0];
reg write_s0, write_s1;

wire dualprime;
assign dualprime = ctrl_dual_mode & ctrl_data_type;

wire dualpow2;
assign dualpow2 = ctrl_dual_mode & !ctrl_data_type;


wire [LOGL-1:0] input_data_len;
assign input_data_len = ctrl_data_len;

wire [LOGL-1:0] output_data_len;
assign output_data_len = ctrl_one_bit_mode ? (1 << ctrl_data_len) : ctrl_data_len;


reg reg0_rsel;
reg reg1_rsel;
reg [31:0] reg_s0r;
reg [31:0] reg_s1r;

reg [31:0] reg_s0w;
reg [31:0] reg_s1w;
reg write_s0_q;
reg write_s1_q;


reg rnd_ready;
wire rnd_ref_ready;
wire rnd_x2x_ready;
always @(*)
begin
    case(opcode)
    `X2X_CMD_PRNG  : rnd_ready = rnd_ref_ready;
    `X2X_CMD_X2X   : rnd_ready = rnd_x2x_ready;
    `X2X_CMD_REF   : rnd_ready = rnd_ref_ready;
    `X2X_CMD_REFX2X: rnd_ready = rnd_ref_ready & rnd_x2x_ready;
    endcase
end

x2x_acc_rng #(
        .PARAM_WIDTH(PARAM_WIDTH),
        .BOX_WIDTH(BOX_WIDTH),
        .RND_SHARES_2SHARE(RND_SHARES_2SHARE),
        .RND_SHARES_2SHARE_BOX(RND_SHARES_2SHARE_BOX)
) x2x_acc_rng (
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
        .ctrl_nonzero(ctrl_share_mode),
        .ctrl_prng_off(ctrl_prng_off),
        .x2x_fresh_rnd_shares(x2x_fresh_rnd_shares),
        .x2x_fresh_rnd_shares_8bit(x2x_fresh_rnd_shares_8bit),
        .rnd_x2x_ready(rnd_x2x_ready),
        .rnd_ref(rnd_ref),
        .rnd_ref_ready(rnd_ref_ready)
    );



always @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin
        fsm_state <= ST_IDLE;
    end else begin
        fsm_state <= fsm_next_state;
    end
end

always @(*) begin
    fsm_next_state = fsm_state;
    // ctrl
    ctrl_busy = (fsm_state != ST_IDLE)&&(fsm_state != ST_TRIVIUM)&&(fsm_state != ST_RESET);
    ctrl_seed_ip = (fsm_state == ST_TRIVIUM);
    ctrl_done = 1'b0;
    // mem
    mem_addr = 32'd0;
    mem_re = 1'b0;
    mem_we = 1'b0;
    mem_o_data = reg_s0r;//0;
    // internal
    ctr_array_rst = 1'b0;
    ctr_array_inc = 1'b0;
    ctr_array_inc_b = 1'b0;
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
    write_s0 = 0;
    write_s1 = 0;
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
    addr_offset = 0;
    
    reg0_rsel = 0;
    reg1_rsel = 0;
    
    case(fsm_state)
    
    ST_IDLE:
    begin
       if(ctrl_start)
       begin
           if(opcode != 2'b00)
                fsm_next_state = ST_FETCH_DATA_0_0;
           else
                fsm_next_state = ST_MASK_SEND;
       end
       else if(ctrl_load_seed)
           fsm_next_state = ST_TRIVIUM;     
    end
    ST_TRIVIUM:
    begin
         mem_o_data = reg_s1r;
        if(ctr_trivium == TRIVIUM_INIT_CC)
            fsm_next_state = ST_IDLE;
    end   
	ST_FETCH_DATA_0_0:
    begin
       if(mem_i_ready)
       begin
            if(ctrl_one_bit_mode)
            begin
                if(ctrl_dual_mode)
                    mem_addr = ctrl_din_addr[0] + (ctr_iter << 2) + (ctr_block_r_mem << 2);
                else
                    mem_addr = ctrl_din_addr[0] + (ctr_iter << 1) + (ctr_block_r_mem << 2);
				
				mem_re = 1;      
				ctr_block_r_mem_inc = 1;
				if(ctr_block_r_mem == 1)
				begin
					fsm_next_state = ST_FETCH_DATA_0_1;
					ctr_block_r_mem_rst = 1;
				end
			
            end
            else
            begin
                mem_addr = ctrl_din_addr[0] + ((ctr_array + ctr_block_r_mem) << 2);
				mem_re = 1;      
				ctr_block_r_mem_inc = 1;
				if(ctr_block_r_mem == (BURST_LEN - 1))
				begin
					fsm_next_state = ST_FETCH_DATA_0_1;
					ctr_block_r_mem_rst = 1;
				end
            end
       end
	   if(mem_i_valid)
	   begin
			ctr_block_w_mem_inc = 1;
			write_s0 = 1;
	   end
    end
	ST_FETCH_DATA_0_1:
    begin
		if(mem_i_valid)
		begin
		    ctr_block_w_mem_inc = 1;
			write_s0 = 1;
		    if(ctrl_one_bit_mode)
		    begin
		        if(ctr_block_w_mem == 1)
                begin
                    fsm_next_state = ST_FETCH_DATA_1_0;
                    ctr_block_w_mem_rst = 1;
                end
		    end
			else
			begin
                if(ctr_block_w_mem == (BURST_LEN - 1))
                begin
                    fsm_next_state = ST_FETCH_DATA_1_0;
                    ctr_block_w_mem_rst = 1;
                end
			end
		end
	end
	ST_FETCH_DATA_1_0:
    begin
       if(!ctrl_share_mode)
       begin
           if(mem_i_ready)
           begin
                if(ctrl_one_bit_mode)
                begin
                    if(ctrl_dual_mode)
                        mem_addr = ctrl_din_addr[1] + (ctr_iter << 2) + (ctr_block_r_mem << 2);
                    else
                        mem_addr = ctrl_din_addr[1] + (ctr_iter << 1) + (ctr_block_r_mem << 2);
                    
                    mem_re = 1;      
                    ctr_block_r_mem_inc = 1;
                    if(ctr_block_r_mem == 1)
                    begin
                        fsm_next_state = ST_FETCH_DATA_1_1;
                        ctr_block_r_mem_rst = 1;
                    end
                
                end
                else
                begin
                    mem_addr = ctrl_din_addr[1] + ((ctr_array + ctr_block_r_mem) << 2);
                    mem_re = 1;      
                    ctr_block_r_mem_inc = 1;
                    if(ctr_block_r_mem == (BURST_LEN - 1))
                    begin
                        fsm_next_state = ST_FETCH_DATA_1_1;
                        ctr_block_r_mem_rst = 1;
                    end
                end
           end
           if(mem_i_valid)
           begin
                ctr_block_w_mem_inc = 1;
                write_s1 = 1;
           end
	   end
	   else
	   begin
	       write_s1 = 1;
	       ctr_block_w_mem_inc = 1;
	       if(ctr_block_w_mem == (BURST_LEN - 1))
            begin
                fsm_next_state = ST_MASK_SEND;
                ctr_block_w_mem_rst = 1;
            end
	   end
	      
	    if(((opcode == `X2X_CMD_X2X)||(opcode == `X2X_CMD_REFX2X))  && !ctrl_one_bit_mode  && ((((ctr_block_r_reg + 1) < ctr_block_w_mem) && ctrl_data_type) || (((ctr_block_r_reg + 7) < ctr_block_w_mem) && !ctrl_data_type)  ) )
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
            x2x_valid_data = rnd_ready;
            x2x_ready_result = 1;
            if(x2x_ready_data & rnd_ready)
                ctr_block_r_reg_inc = 1;
        end
        
        if(x2x_valid_result)
            ctr_block_w_reg_inc = 1; 
        
    end
	ST_FETCH_DATA_1_1:
    begin
		if(mem_i_valid)
		begin
			ctr_block_w_mem_inc = 1;
			write_s1 = 1;
			if(ctrl_one_bit_mode)
			begin
			    if(ctr_block_w_mem == 1)
                begin
                    fsm_next_state = ST_MASK_SEND;
                    ctr_block_w_mem_rst = 1;
                end
			end
			else
			begin
                if(ctr_block_w_mem == (BURST_LEN - 1))
                begin
                    fsm_next_state = ST_MASK_SEND;
                    ctr_block_w_mem_rst = 1;
                end
			end
		end
		
		
		if(((opcode == `X2X_CMD_X2X)||(opcode == `X2X_CMD_REFX2X)) && !ctrl_one_bit_mode  && ((((ctr_block_r_reg + 1) < ctr_block_w_mem) && ctrl_data_type) || (((ctr_block_r_reg + 7) < ctr_block_w_mem) && !ctrl_data_type)  ) )
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
            x2x_valid_data = rnd_ready;
            x2x_ready_result = 1;
            if(x2x_ready_data & rnd_ready)
                ctr_block_r_reg_inc = 1;
        end
        
        if(x2x_valid_result)
            ctr_block_w_reg_inc = 1; 
		
	end
    ST_MASK_SEND:
    begin
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
            if(ctrl_dual_mode)
            begin
                if((ctr_block_r_reg == ((BURST_LEN << 1)- 1))& rnd_ready)
                begin
                    fsm_next_state = ST_MASK_WAIT;
                    ctr_block_r_reg_rst = 1;
                end
            end
            else
            begin
                if((ctr_block_r_reg == (BURST_LEN - 1))& rnd_ready)
                begin
                    fsm_next_state = ST_MASK_WAIT;
                    ctr_block_r_reg_rst = 1;
                end
            end
        end
        else
        begin
            if(dualprime)
            begin
                if((ctr_block_r_reg == ((BURST_LEN << 1) - 1)) & rnd_ready)
                begin
                    fsm_next_state = ST_MASK_WAIT;
                    ctr_block_r_reg_rst = 1;
                end
            end
            else if((ctr_block_r_reg == (BURST_LEN - 1))& rnd_ready)
            begin
                fsm_next_state = ST_MASK_WAIT;
                ctr_block_r_reg_rst = 1;
            end
        end
        
        if(x2x_valid_result)
            ctr_block_w_reg_inc = 1;
        
    end
    ST_MASK_WAIT:
    begin
        x2x_ready_result = 1;
        x2x_valid_rng = rnd_ready;
        if(x2x_valid_result)
        begin
            if(ctrl_one_bit_mode)
            begin
                if(ctrl_dual_mode)
                begin
                    if(ctr_block_w_reg == ((BURST_LEN << 1) - 1))
                    begin
                        fsm_next_state = ST_PUT_DATA_0;
                        ctr_block_w_reg_rst = 1;
                        reg0_rsel = 1;
                        ctr_block_r_mem_inc = 1;
                    end 
                    else
                    begin
                        ctr_block_w_reg_inc = 1;
                    end
                end
                else
                begin
                    if(ctr_block_w_reg == (BURST_LEN - 1))
                    begin
                        fsm_next_state = ST_PUT_DATA_0;
                        ctr_block_w_reg_rst = 1;
                        reg0_rsel = 1;
                        ctr_block_r_mem_inc = 1;
                    end 
                    else
                    begin
                        ctr_block_w_reg_inc = 1;
                    end
                end
                
            end
            else
            begin
                if(dualprime)
                begin
                    if(ctr_block_w_reg == ((BURST_LEN << 1) - 1))
                    begin
                        if(opcode != `X2X_CMD_PRNG)
                        begin
                            fsm_next_state = ST_PUT_DATA_0;
                            reg0_rsel = 1;
                        end
                        else 
                        begin
                            fsm_next_state = ST_PUT_DATA_1;
                            reg1_rsel = 1;
                        end
                        ctr_block_r_mem_inc = 1;
                        ctr_block_w_reg_rst = 1;
                    end
                    else 
                    begin
                        ctr_block_w_reg_inc = 1;
                    end
                end
                else 
                begin    
                    if(ctr_block_w_reg == (BURST_LEN - 1))
                    begin
                        if(opcode != `X2X_CMD_PRNG)
                            begin
                                fsm_next_state = ST_PUT_DATA_0;
                                reg0_rsel = 1;
                            end
                            else 
                            begin
                                fsm_next_state = ST_PUT_DATA_1;
                                reg1_rsel = 1;
                            end
                        ctr_block_r_mem_inc = 1;
                        ctr_block_w_reg_rst = 1;
                    end
                    else
                    begin
                        ctr_block_w_reg_inc = 1;
                    end
                end
            end
        end 
    end
    ST_PUT_DATA_0:
    begin
        reg0_rsel = 1;
        mem_o_data = reg_s0r;
        if(ctrl_one_bit_mode)
        begin
            addr_offset = (ctr_array + ctr_block_r_mem_q);
            if(ctrl_dual_mode)
            begin
                if((addr_offset[4:0] & ((1 << (5 - log_stride)) - 1)) == ((1 << (5 - log_stride)) - 1))
                begin
                    if((addr_offset[4:0] >> (5 - log_stride)) == ((1 << log_stride)-1))
                       ctr_onebit_addr_off_0_rst = 1; 
                    else
                       ctr_onebit_addr_off_0_inc = 1;
                end
                mem_addr = ctrl_dout_addr[0] + (((addr_offset[31:5] << (5 - log_stride)) + (addr_offset[4:0] & ((1 << (5 - log_stride)) - 1)) + ctr_onebit_addr_off_0) << 2);
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
                mem_addr = ctrl_dout_addr[0] + (((addr_offset[31:6] << (6 - log_stride)) + (addr_offset[5:0] & ((1 << (6 - log_stride)) - 1)) + ctr_onebit_addr_off_0) << 2);
            end
            if(mem_o_ready)
            begin
                if(ctr_block_r_mem_q == (BURST_LEN - 1))
                begin
                    reg1_rsel = 1;
                    reg0_rsel = 0;
                    fsm_next_state = ST_PUT_DATA_1;
                    ctr_block_r_mem_inc = 1;
                end
                else if(ctr_block_r_mem_q == (BURST_LEN - 2))
                begin
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
            mem_addr = ctrl_dout_addr[0] + (ctr_array << 2)  + (ctr_block_r_mem_q << 2);
            if(mem_o_ready)
            begin
                if(ctr_block_r_mem_q == (BURST_LEN - 1))
                begin
                    reg1_rsel = 1;
                    reg0_rsel = 0;
                    fsm_next_state = ST_PUT_DATA_1;
                    ctr_block_r_mem_inc = 1;
                end
                else if(ctr_block_r_mem_q == (BURST_LEN - 2))
                begin
                    ctr_block_r_mem_rst = 1;
                end
                else
                begin
                    ctr_block_r_mem_inc = 1;
                end
            end
        end
        
        mem_we = 1;
    end
    ST_PUT_DATA_1:
    begin
        reg1_rsel = 1;
        mem_o_data = reg_s1r;
        if(ctrl_one_bit_mode)
        begin
			addr_offset = (ctr_array + ctr_block_r_mem_q);
		    if(ctrl_dual_mode)
            begin
                if((addr_offset[4:0] & ((1 << (5 - log_stride)) - 1)) == ((1 << (5 - log_stride)) - 1))
                begin
                    if((addr_offset[4:0] >> (5 - log_stride)) == ((1 << log_stride)-1))
                       ctr_onebit_addr_off_1_rst = 1; 
                    else
                       ctr_onebit_addr_off_1_inc = 1;
                end
                mem_addr = ctrl_dout_addr[1] + (((addr_offset[31:5] << (5 - log_stride)) + (addr_offset[4:0] & ((1 << (5 - log_stride)) - 1)) + ctr_onebit_addr_off_1) << 2);
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
                mem_addr = ctrl_dout_addr[1] + (((addr_offset[31:6] << (6 - log_stride)) + (addr_offset[5:0] & ((1 << (6 - log_stride)) - 1)) + ctr_onebit_addr_off_1) << 2);
            end
			if(mem_o_ready)
			begin
				if(ctr_block_r_mem_q == (BURST_LEN - 1))
				begin
					ctr_block_r_mem_rst = 1;
					ctr_array_inc_b = 1;
					ctr_iter_inc = 1;
					reg1_rsel = 0;
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
                                fsm_next_state = ST_FETCH_DATA_0_0;
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
                                fsm_next_state = ST_FETCH_DATA_0_0;
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
            mem_addr = ctrl_dout_addr[1] + (ctr_array << 2)  + (ctr_block_r_mem_q << 2);
            if(mem_o_ready)
            begin
                if(ctr_block_r_mem_q == (BURST_LEN - 1))
                begin
                    ctr_block_r_mem_rst = 1;
                    ctr_array_inc_b = 1;
                    ctr_iter_inc = 1;
                    reg1_rsel = 0;
                    if(ctr_array == (ctrl_data_len - BURST_LEN))
                    begin
                        fsm_next_state = ST_DONE;
                    end
                    else
                    begin
                        if(opcode != 2'b00)
                            fsm_next_state = ST_FETCH_DATA_0_0;
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
        mem_we = 1;
    end
    ST_DONE:
    begin
                ctr_block_r_mem_rst = 1;
                ctr_block_r_reg_rst = 1;
                ctr_block_w_mem_rst = 1;
                ctr_block_w_reg_rst = 1;
                ctr_array_rst = 1; 
                ctr_iter_rst = 1;
                ctr_onebit_addr_off_0_rst = 1; 
                ctr_onebit_addr_off_1_rst = 1; 
                fsm_next_state = ST_IDLE;
                ctrl_done = 1;
          
    end
    endcase
end


always @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin
        reg_s0r <= 0;
        reg_s1r <= 0;
    end
    else begin
        case(reg0_rsel)
            0: reg_s0r <= 0;
            1: reg_s0r <= shares[0][ctr_block_r_mem];
        endcase
        case(reg1_rsel)
            0: reg_s1r <= 0;
            1: reg_s1r <= shares[1][ctr_block_r_mem];
        endcase    
    end
end


always @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin
        reg_s0w <= 0;
        reg_s1w <= 0;
    end
    else begin
        case(write_s0)
            0: reg_s0w <= 0;
            1: reg_s0w <= mem_i_data;
        endcase
        case(write_s1)
            0: reg_s1w <= 0;
            1: reg_s1w <= mem_i_data;
        endcase    
    end
end


always @(posedge clk) begin
    write_s0_q <= write_s0;
    write_s1_q <= write_s1;
end


always @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin
        ctr_array <= {LOGN{1'b0}};
    end
    else if (ctr_array_rst) begin
        ctr_array <= {LOGN{1'b0}};
    end
    else if (ctr_array_inc) begin
        ctr_array <= ctr_array + 1;
    end
    else if (ctr_array_inc_b) begin
        if(ctrl_one_bit_mode & !ctrl_dual_mode)
            ctr_array <= ctr_array + BURST_LEN;
        else
            ctr_array <= ctr_array + BURST_LEN;
    end
    
end



always @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin
        ctr_block_r_mem <= {LOGN{1'b0}};
    end
    else if (ctr_block_r_mem_rst) begin
        ctr_block_r_mem <= {LOGN{1'b0}};
    end
    else if (ctr_block_r_mem_inc) begin
        ctr_block_r_mem <= ctr_block_r_mem + 1;
    end
end


always @(posedge clk) begin
    ctr_block_r_mem_q <= ctr_block_r_mem;
end

always @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin
        ctr_block_r_reg <= {LOGN{1'b0}};
    end
    else if (ctr_block_r_reg_rst) begin
        ctr_block_r_reg <= {LOGN{1'b0}};
    end
    else if (ctr_block_r_reg_inc) begin
        ctr_block_r_reg <= ctr_block_r_reg + 1;
    end
end


always @(posedge clk) begin
    ctr_block_r_reg_q <= ctr_block_r_reg;
end



always @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin
        ctr_block_w_mem <= {LOGN{1'b0}};
    end
    else if (ctr_block_w_mem_rst) begin
        ctr_block_w_mem <= {LOGN{1'b0}};
    end
    else if (ctr_block_w_mem_inc) begin
        ctr_block_w_mem <= ctr_block_w_mem + 1;
    end
    
end


always @(posedge clk) begin
    ctr_block_w_mem_q <= ctr_block_w_mem;
end

always @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin
        ctr_block_w_reg <= {LOGN{1'b0}};
    end
    else if (ctr_block_w_reg_rst) begin
        ctr_block_w_reg <= {LOGN{1'b0}};
    end
    else if (ctr_block_w_reg_inc) begin
        ctr_block_w_reg <= ctr_block_w_reg + 1;
    end
    
end


always @(posedge clk) begin
    ctr_block_w_reg_q <= ctr_block_w_reg;
end


always @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin
        ctr_iter <= {LOGN{1'b0}};
    end
    else if (ctr_iter_rst) begin
        ctr_iter <= {LOGN{1'b0}};
    end
    else if (ctr_iter_inc) begin
        ctr_iter <= ctr_iter + 1;
    end
end

always @(posedge clk or negedge rst_n) begin
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

always @(posedge clk or negedge rst_n) begin
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



always @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin
        ctr_trivium <= 0;
    end
    else if (fsm_state == ST_TRIVIUM) begin
        ctr_trivium <= ctr_trivium + 1;
    end
    else begin
        ctr_trivium <= 0;
    end
end

always @(posedge clk) begin
    if (!rst_n) 
    begin
        for (int i = 0; i < SHARES; i = i + 1)
        begin
            shares_one_bit[i][0] <= 0;
			shares_one_bit[i][1] <= 0;
            for (int j = 0; j < BURST_LEN; j = j + 1)
                shares[i][j] <= 0;
        end
    end 
    else 
    begin
        if (write_s0_q)
        begin
            if (ctrl_one_bit_mode)
                shares_one_bit[0][ctr_block_w_mem_q] <= reg_s0w;
            else
                shares[0][ctr_block_w_mem_q] <= reg_s0w;
        end
        else if (write_s1_q) 
        begin
            if (ctrl_one_bit_mode)
                shares_one_bit[1][ctr_block_w_mem_q] <= reg_s1w;
            else    
            begin
                if (!ctrl_share_mode)
                    shares[1][ctr_block_w_mem_q] <= reg_s1w;
                else
                    shares[1][ctr_block_w_mem_q] <= 0;
            end
        end

        else if (x2x_valid_result)
        begin
            if(ctrl_one_bit_mode)
            begin
                if(ctrl_dual_mode)
                begin
                    if(ctr_block_w_reg[0])
                    begin
                        shares[0][(ctr_block_w_reg >> 1)][31:16] <= {{(16 - PARAM_WIDTH){1'b0}}, x2x_converted_data[0][0]};
                        shares[1][(ctr_block_w_reg >> 1)][31:16] <= {{(16 - PARAM_WIDTH){1'b0}}, x2x_converted_data[0][1]};
                    end
                    else
                    begin
                        shares[0][(ctr_block_w_reg >> 1)][15:0] <= {{(16 - PARAM_WIDTH){1'b0}}, x2x_converted_data[0][0]};
                        shares[1][(ctr_block_w_reg >> 1)][15:0] <= {{(16 - PARAM_WIDTH){1'b0}}, x2x_converted_data[0][1]};
                    end
                end
                else
                begin
                    shares[0][ctr_block_w_reg] <= {{(32 - PARAM_WIDTH){1'b0}}, x2x_converted_data[0][0]};
                    shares[1][ctr_block_w_reg] <= {{(32 - PARAM_WIDTH){1'b0}}, x2x_converted_data[0][1]};
                end
            end
            else if(dualprime)
            begin
                if(ctr_block_w_reg[0])
                begin
                    shares[0][(ctr_block_w_reg >> 1)][31:16] <= {{(16 - PARAM_WIDTH){1'b0}}, x2x_converted_data[0][0]};
                    shares[1][(ctr_block_w_reg >> 1)][31:16] <= {{(16 - PARAM_WIDTH){1'b0}}, x2x_converted_data[0][1]};
                end
                else
                begin
                    shares[0][(ctr_block_w_reg >> 1)][15:0] <= {{(16 - PARAM_WIDTH){1'b0}}, x2x_converted_data[0][0]};
                    shares[1][(ctr_block_w_reg >> 1)][15:0] <= {{(16 - PARAM_WIDTH){1'b0}}, x2x_converted_data[0][1]};
                end
            end
            else if (!ctrl_dual_mode  && !ctrl_data_type  && !ctrl_conv_mode) 
            begin
                shares[0][ctr_block_w_reg] <= x2x_converted_data[0][0]; 
                shares[1][ctr_block_w_reg] <= x2x_converted_data[0][1];
            end
            else if (!ctrl_dual_mode) 
            begin
                shares[0][ctr_block_w_reg] <= x2x_converted_data[0][0]; 
                shares[1][ctr_block_w_reg] <= x2x_converted_data[0][1]; 
            end 
            else 
            begin
                shares[0][ctr_block_w_reg] <= {x2x_converted_data[1][0][15:0], x2x_converted_data[0][0][15:0]};
                shares[1][ctr_block_w_reg] <= {x2x_converted_data[1][1][15:0], x2x_converted_data[0][1][15:0]};
            end
        end
    end
end

endmodule