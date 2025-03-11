module x2x_acc_fsm
   #(
        parameter SHARES  = 2 ,
        parameter B       = 32,
        parameter LOGL    = 10,
        parameter PARAM_WIDTH = 0,
        parameter RND_SHARES = 0,
        parameter RND_SHARES_8bit = 0,
        parameter N_SHARES = 0,
        
        parameter BURST_LEN = 2 // MINIMUM 2
    )
    (
        input                   clk                        ,
        input                   rst_n                      ,
        // fsm <-> ctrl        
        input                   ctrl_start                 ,
        input      [       3:0] ctrl_cmd                   ,
        output reg              ctrl_busy                  ,
        output reg              ctrl_done                  ,
        input      [      31:0] ctrl_din_addr  [0:SHARES-1],
        input      [      31:0] ctrl_dout_addr [0:SHARES-1],
        input      [  LOGL-1:0] ctrl_data_len              ,
        input      [      31:0] ctrl_seed_addr             , 
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
        
        output reg [PARAM_WIDTH - 1 : 0]             x2x_fresh_rnd_shares   [RND_SHARES - 1 : 0]    ,
        output reg  [8-1:0]            x2x_fresh_rnd_shares_8bit [RND_SHARES_8bit - 1 : 0] ,
        output reg [PARAM_WIDTH - 1 : 0]             x2x_original_data [2 - 1 : 0] [N_SHARES - 1:0]        ,
        input      [PARAM_WIDTH - 1 : 0]             x2x_converted_data [2 - 1 : 0][N_SHARES - 1:0]
    );


`include "keccak_acc.vh"

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

localparam ST_ABSORB_DMA_WAIT_DONE       = 4'd2;
localparam ST_ABSORB_KECCAK              = 4'd3;
localparam ST_PAD_BUFFER_WR              = 4'd4;
localparam ST_PAD_KECCAK                 = 4'd5;
localparam ST_SQUEEZE_INIT               = 4'd6;
localparam ST_SQUEEZE_KECCAK             = 4'd7;
localparam ST_SQUEEZE_DMA_FEED           = 4'd8;
localparam ST_SQUEEZE_DMA_WAIT_DONE      = 4'd9;
localparam ST_RESET                       = 4'd15;

reg [3:0] fsm_state, fsm_next_state;
reg [LOGN-1:0] ctr_array;
reg ctr_array_rst, ctr_array_inc, ctr_array_inc_b;
/*reg [LOGN-1:0] ctr_a1;
reg ctr_a1_rst, ctr_a1_inc, ctr_a1_inc_b ;*/
reg [LOGN-1:0] ctr_block;
reg ctr_block_rst, ctr_block_inc;
reg [LOGN-1:0] ctr_iter;
reg ctr_iter_rst, ctr_iter_inc;

reg [PARAM_WIDTH-1:0] shares [SHARES - 1 : 0][BURST_LEN - 1:0];
reg write_s0, write_s1;

reg [LOGN-1:0] ctr_b;
reg ctr_b_rst, ctr_b_set, ctr_b_inc;
reg [LOGS-1:0] ctr_s;
reg ctr_s_rst, ctr_s_inc;
reg [LOGL-1:0] ctr_m;
reg ctr_m_rst, ctr_m_inc, ctr_m_sub_num_words, ctr_m_add_num_words;
reg [LOGN-1:0] num_words;
reg set_num_words;

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
    ctrl_busy = (fsm_state != ST_IDLE);//1'b0;
    ctrl_done = 1'b0;
    // mem
    mem_addr = 32'd0;
    mem_re = 1'b0;
    mem_we = 1'b0;
    // internal
    ctr_array_rst = 1'b0;
    ctr_array_inc = 1'b0;
    ctr_array_inc_b = 1'b0;
    ctr_block_rst = 1'b0;
    ctr_block_inc = 1'b0;
    ctr_iter_rst = 1'b0;
    ctr_iter_inc = 1'b0;
    write_s0 = 0;
    write_s1 = 0;
    x2x_valid_data = 0;
    x2x_ready_result = 0;
    
    x2x_original_data[0][0] = 0;
    x2x_original_data[0][1] = 0;
    x2x_original_data[1][0] = 0;
    x2x_original_data[1][1] = 0;
    
    case(fsm_state)
    ST_IDLE:
    begin
       if(ctrl_start)
           fsm_next_state = ST_FETCH_DATA_0_0;     
    end
    ST_FETCH_DATA_0_0:
    begin
       if(mem_i_ready)
       begin
           fsm_next_state = ST_FETCH_DATA_0_1;
           mem_addr = ctrl_din_addr[0] + (ctr_array << 2) + (ctr_block << 2);
           mem_re = 1;      
           ctr_block_inc = 1;
       end
    end
    ST_FETCH_DATA_0_1:
    begin
        write_s0 = 1;
        if(mem_i_ready)
        begin
            if(ctr_block == (BURST_LEN - 1))
            begin
                ctr_block_rst = 1;
                fsm_next_state = ST_FETCH_DATA_0_2;
            end
            else
            begin
                ctr_block_inc = 1;
            end
            mem_addr = ctrl_din_addr[0] + (ctr_array << 2) + (ctr_block << 2);
            mem_re = 1;      
            
        end
    end
    ST_FETCH_DATA_0_2:
    begin
        fsm_next_state = ST_FETCH_DATA_1_0;
        write_s0 = 1;
    end
    ST_FETCH_DATA_1_0:
    begin
       if(mem_i_ready)
       begin
           fsm_next_state = ST_FETCH_DATA_1_1;
           mem_addr = ctrl_din_addr[1] + (ctr_array << 2) + (ctr_block << 2);
           mem_re = 1; 
           ctr_block_inc = 1;   
       end
    end
    ST_FETCH_DATA_1_1:
    begin
        write_s1 = 1;
        if(mem_i_ready)
        begin
            if(ctr_block == (BURST_LEN - 1))
            begin
                ctr_block_rst = 1;
                fsm_next_state = ST_FETCH_DATA_1_2;
            end
            else
            begin
                ctr_block_inc = 1;
            end
            mem_addr = ctrl_din_addr[1] + (ctr_array << 2)  + (ctr_block << 2);
            mem_re = 1;      
        end
    end
    ST_FETCH_DATA_1_2:
    begin
        write_s1 = 1;
        fsm_next_state = ST_MASK_SEND;
    end 
    ST_MASK_SEND:
    begin
        x2x_original_data[0][0] = shares[0][ctr_block];
        x2x_original_data[0][1] = shares[1][ctr_block];
        x2x_valid_data = 1;
        if(x2x_ready_data)
            fsm_next_state = ST_MASK_WAIT;
    end
    ST_MASK_WAIT:
    begin
        x2x_ready_result = 1;
        if(x2x_valid_result)
        begin
            if(ctr_block == (BURST_LEN - 1))
            begin
                fsm_next_state = ST_PUT_DATA_0;
                ctr_block_rst = 1;
            end
            else
            begin
                fsm_next_state = ST_MASK_SEND;
                ctr_block_inc = 1;
            end
            
        end 
    end
    ST_PUT_DATA_0:
    begin
        mem_o_data = shares[0][ctr_block];
        mem_addr = ctrl_dout_addr[0] + (ctr_array << 2)  + (ctr_block << 2);
        mem_we = 1; 
        if(ctr_block < (BURST_LEN - 1))
        begin
            ctr_block_inc = 1; 
        end
        else
        begin
            ctr_block_rst = 1;
            fsm_next_state = ST_PUT_DATA_1;
        end
    end
    ST_PUT_DATA_1:
    begin
        mem_o_data = shares[1][ctr_block];
        mem_addr = ctrl_dout_addr[1] + (ctr_array << 2)  + (ctr_block << 2);
        mem_we = 1;
        if(ctr_array == (ctrl_data_len - BURST_LEN))
        begin
            ctr_block_rst = 1;
            ctr_array_rst = 1; 
            ctr_iter_rst = 1;
            fsm_next_state = ST_IDLE;
        end 
        else if(ctr_block == (BURST_LEN - 1))
        begin
            ctr_block_rst = 1;
            ctr_array_inc_b = 1;
            ctr_iter_inc = 1;
            fsm_next_state = ST_FETCH_DATA_0_0;
        end
        else
        begin
            ctr_block_inc = 1; 
        end
        
    end
    endcase
end

always @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin
        ctr_array <= {LOGN{1'b0}};
    end
    else if (ctr_array_inc) begin
        ctr_array <= ctr_array + 1;
    end
    else if (ctr_array_inc_b) begin
        ctr_array <= ctr_array + BURST_LEN;
    end
    else if (ctr_array_rst) begin
        ctr_array <= {LOGN{1'b0}};
    end
end

always @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin
        ctr_block <= {LOGN{1'b0}};
    end
    else if (ctr_block_inc) begin
        ctr_block <= ctr_block + 1;
    end
    else if (ctr_block_rst) begin
        ctr_block <= {LOGN{1'b0}};
    end
end

always @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin
        ctr_iter <= {LOGN{1'b0}};
    end
    else if (ctr_iter_inc) begin
        ctr_iter <= ctr_iter + 1;
    end
    else if (ctr_iter_rst) begin
        ctr_iter <= {LOGN{1'b0}};
    end
end

for(genvar i = 0; i < SHARES ; i = i + 1) begin
    for(genvar j = 0; j < BURST_LEN ; j = j + 1) begin
        always @(posedge clk) begin
            if (!rst_n)
                shares[i][j] <= 0;
            else if (write_s0 && ctr_block == j)
                shares[0][j] <= mem_i_data;
            else if (write_s1 && ctr_block == j)
                shares[1][j] <= mem_i_data;
            else if ((fsm_state == ST_MASK_WAIT) && x2x_valid_result)
            begin
                shares[0][ctr_block] <= x2x_converted_data[0][0];
                shares[1][ctr_block] <= x2x_converted_data[0][1];
            end    
        end
    end
end

for(genvar j = 0; j < RND_SHARES ; j = j + 1) begin
    always @(posedge clk) begin
        if (!rst_n)
            x2x_fresh_rnd_shares[j] <= 0;   
    end
end

for(genvar j = 0; j < RND_SHARES_8bit ; j = j + 1) begin
    always @(posedge clk) begin
        if (!rst_n)
            x2x_fresh_rnd_shares_8bit[j] <= 0;     
    end
end
endmodule