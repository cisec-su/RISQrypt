module x2x_acc_fsm
   #(
        parameter SHARES  = 2 ,
        parameter B       = 32,
        parameter LOGL    = 10,
        parameter PARAM_WIDTH = 0,
        parameter RND_SHARES = 0,
        parameter RND_SHARES_8bit = 0,
        parameter N_SHARES = 0,
        
        parameter BURST_LEN = 4 // MINIMUM 2
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
        input      [      63:0] ctrl_seed             , 
        input                   ctrl_start_RNG             ,
        input ctrl_mask_mode,
        input ctrl_dual_mode,
        input ctrl_data_type,
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

wire [255:0] stream_out;

Trivium RNG (
    .clk(clk),
    .rst(rst_n),
    .load(ctrl_start_RNG),
    .key(80'd0),
    .iv({16'd0, ctrl_seed}),
    //.iv({48'd0, 32'd1}),
    .stream_out(stream_out)
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


localparam ST_INIT                        = 4'd14;
localparam ST_RESET                       = 4'd15;

localparam WORD_PAD                       = 32-PARAM_WIDTH;
localparam HALF_PAD                       = 32-PARAM_WIDTH;

reg [3:0] fsm_state, fsm_next_state;
reg [LOGL-1:0] ctr_array;
reg ctr_array_rst, ctr_array_inc, ctr_array_inc_b;

reg [LOGL-1:0] ctr_block_r;
reg ctr_block_r_rst, ctr_block_r_inc;

reg [LOGL-1:0] ctr_block_w;
reg ctr_block_w_rst, ctr_block_w_inc;

reg [LOGL-1:0] ctr_iter;
reg ctr_iter_rst, ctr_iter_inc;

reg [4:0] ctr_init;

reg [31:0] shares [SHARES - 1 : 0][BURST_LEN - 1:0];
reg write_s0, write_s1;

reg dualprime_msh, dualprime_comp;
wire dualprime;
assign dualprime = ctrl_dual_mode & ctrl_data_type;

always @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin
        fsm_state <= ST_IDLE;//will be init
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
    mem_o_data = 0;
    // internal
    ctr_array_rst = 1'b0;
    ctr_array_inc = 1'b0;
    ctr_array_inc_b = 1'b0;
    ctr_block_r_rst = 1'b0;
    ctr_block_r_inc = 1'b0;
    ctr_block_w_rst = 1'b0;
    ctr_block_w_inc = 1'b0;
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
    
    dualprime_comp = 0;
    
    case(fsm_state)
    ST_INIT:
    begin
        if(ctr_init == 30)
            fsm_next_state = ST_IDLE;
    end
    ST_IDLE:
    begin
       ctrl_done = 1;
       if(ctrl_start)
           fsm_next_state = ST_FETCH_DATA_0_0;     
    end
    ST_FETCH_DATA_0_0:
    begin
       if(mem_i_ready)
       begin
           fsm_next_state = ST_FETCH_DATA_0_1;
           mem_addr = ctrl_din_addr[0] + (ctr_array << 2) + (ctr_block_w << 2);
           mem_re = 1;      
           ctr_block_w_inc = 1;
       end
    end
    ST_FETCH_DATA_0_1:
    begin
        write_s0 = mem_i_valid;
        if(mem_i_ready)
        begin
            if(ctr_block_w == (BURST_LEN - 1))
            begin
                fsm_next_state = ST_FETCH_DATA_0_2;
            end
            ctr_block_w_inc = 1;
            mem_addr = ctrl_din_addr[0] + (ctr_array << 2) + (ctr_block_w << 2);
            mem_re = 1;      
            
        end
    end
    ST_FETCH_DATA_0_2:
    begin
        fsm_next_state = ST_FETCH_DATA_1_0;
        write_s0 = mem_i_valid;
        ctr_block_w_rst = 1;
    end
    ST_FETCH_DATA_1_0:
    begin
       if(ctrl_mask_mode)
       begin
            fsm_next_state = ST_FETCH_DATA_1_1;
            ctr_block_w_inc = 1;
       end
       else
       begin
           if(mem_i_ready)
           begin
               fsm_next_state = ST_FETCH_DATA_1_1;
               mem_addr = ctrl_din_addr[1] + (ctr_array << 2) + (ctr_block_w << 2);
               mem_re = !ctrl_mask_mode; 
               ctr_block_w_inc = 1;   
           end
       end
    end
    ST_FETCH_DATA_1_1:
    begin
        if(ctrl_mask_mode)
        begin
            write_s1 = 1;
            if(ctr_block_w == (BURST_LEN - 1))
            begin
                fsm_next_state = ST_FETCH_DATA_1_2;
            end
            ctr_block_w_inc = 1;
        end
        else
        begin
            write_s1 = mem_i_valid;
            mem_re = !ctrl_mask_mode;  
            if(mem_i_ready)
            begin
                mem_addr = ctrl_din_addr[1] + (ctr_array << 2)  + (ctr_block_w << 2);
                
                if(ctr_block_w == (BURST_LEN - 1))
                begin
                    fsm_next_state = ST_FETCH_DATA_1_2;
                end
                ctr_block_w_inc = 1;
                    
            end
            else
            begin
                mem_addr = ctrl_din_addr[1] + (ctr_array << 2)  + ((ctr_block_w - 1) << 2);
            end
        end
    end
    ST_FETCH_DATA_1_2:
    begin
        if(ctrl_mask_mode)
            write_s1 = 1;
        else
            write_s1 = mem_i_valid;
        
        fsm_next_state = ST_MASK_SEND;
        ctr_block_w_rst = 1;
    end 
    ST_MASK_SEND:
    begin
        if(dualprime && dualprime_msh)
        begin
            x2x_original_data[0][0] = shares[0][ctr_block_r][16+PARAM_WIDTH-1:16];
            x2x_original_data[0][1] = shares[1][ctr_block_r][16+PARAM_WIDTH-1:16];
        end
        else
        begin
            x2x_original_data[0][0] = shares[0][ctr_block_r][PARAM_WIDTH-1:0];
            x2x_original_data[0][1] = shares[1][ctr_block_r][PARAM_WIDTH-1:0];
        end
        if(ctrl_dual_mode && !ctrl_data_type)
        begin
            x2x_original_data[1][0] = shares[0][ctr_block_r][16+PARAM_WIDTH-1:16];
            x2x_original_data[1][1] = shares[1][ctr_block_r][16+PARAM_WIDTH-1:16];
        end
        x2x_valid_data = 1;
        x2x_ready_result = 1;
        if(x2x_ready_data)
            ctr_block_r_inc = 1;
        if(ctr_block_r == (BURST_LEN - 1))
        begin
            fsm_next_state = ST_MASK_WAIT;
            ctr_block_r_rst = 1;
        end
        if(x2x_valid_result)
            ctr_block_w_inc = 1;
        
    end
    ST_MASK_WAIT:
    begin
        x2x_ready_result = 1;
        if(x2x_valid_result)
        begin
            if(ctr_block_w == (BURST_LEN - 1))
            begin
                if(dualprime)
                begin
                    if(!dualprime_msh)
                        fsm_next_state = ST_MASK_SEND;
                    else
                        fsm_next_state = ST_PUT_DATA_0;
                    
                    dualprime_comp = 1;  
                end
                else
                begin
                    fsm_next_state = ST_PUT_DATA_0;
                end
                ctr_block_w_rst = 1;
            end
            else
            begin
                //fsm_next_state = ST_MASK_SEND;
                ctr_block_w_inc = 1;
            end
            
        end 
    end
    ST_PUT_DATA_0:
    begin
        mem_o_data = shares[0][ctr_block_r];
        mem_addr = ctrl_dout_addr[0] + (ctr_array << 2)  + (ctr_block_r << 2);
        mem_we = 1; 
        if(mem_o_ready)
        begin
            if(ctr_block_r < (BURST_LEN - 1))
            begin
                ctr_block_r_inc = 1; 
            end
            else
            begin
                ctr_block_r_rst = 1;
                fsm_next_state = ST_PUT_DATA_1;
            end
        end
    end
    ST_PUT_DATA_1:
    begin
        mem_o_data = shares[1][ctr_block_r];
        mem_addr = ctrl_dout_addr[1] + (ctr_array << 2)  + (ctr_block_r << 2);
        mem_we = 1;
        if(mem_o_ready)
        begin
            
            if(ctr_block_r == (BURST_LEN - 1))
            begin
                ctr_block_r_rst = 1;
                ctr_array_inc_b = 1;
                ctr_iter_inc = 1;
                if(ctr_array == (ctrl_data_len - BURST_LEN))
                begin
                    fsm_next_state = ST_DONE;
                end
                else
                    fsm_next_state = ST_FETCH_DATA_0_0;
            end
            else
            begin
                ctr_block_r_inc = 1; 
            end
        end
    end
    ST_DONE:
    begin

                ctr_block_r_rst = 1;
                ctr_array_rst = 1; 
                ctr_iter_rst = 1;
                fsm_next_state = ST_IDLE;
                ctrl_done = 1;
          
    end
    endcase
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
        ctr_array <= ctr_array + BURST_LEN;
    end
    
end

always @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin
        ctr_block_r <= {LOGN{1'b0}};
    end
    else if (ctr_block_r_rst) begin
        ctr_block_r <= {LOGN{1'b0}};
    end
    else if (ctr_block_r_inc) begin
        ctr_block_r <= ctr_block_r + 1;
    end
    
end

always @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin
        ctr_block_w <= {LOGN{1'b0}};
    end
    else if (ctr_block_w_rst) begin
        ctr_block_w <= {LOGN{1'b0}};
    end
    else if (ctr_block_w_inc) begin
        ctr_block_w <= ctr_block_w + 1;
    end
    
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
        ctr_init <= 0;
    end
    else if (ctrl_start_RNG) begin
        ctr_init <= ctr_init + 1;
    end
    else if (ctr_iter != 0) begin
        ctr_init <= ctr_init + 1;
    end
end

always @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin
        dualprime_msh <= 0;
    end
    else if (dualprime_comp) begin
        dualprime_msh <= ~dualprime_msh;
    end
end

/*
for(genvar i = 0; i < SHARES ; i = i + 1) begin
    for(genvar j = 0; j < BURST_LEN ; j = j + 1) begin
        always @(posedge clk) begin
            if (!rst_n)
                shares[i][j] <= 0;
            else if (write_s0 && (ctr_block_w - 1) == j)
                shares[0][j] <= mem_i_data;
            else if (write_s1 && (ctr_block_w - 1)  == j)
            begin
                if(!ctrl_mask_mode)
                    shares[1][j] <= mem_i_data;
                else
                    shares[1][j] <= 0;
            end
            else if (x2x_valid_result)
            begin
                if(!ctrl_dual_mode)
                begin
                    shares[0][ctr_block_w] <= {{(32-PARAM_WIDTH){1'b0}},x2x_converted_data[0][0]};
                    shares[1][ctr_block_w] <= {{(32-PARAM_WIDTH){1'b0}},x2x_converted_data[0][1]};
                end
                else
                begin
                    shares[0][ctr_block_w] <= {{(16-PARAM_WIDTH){1'b0}},x2x_converted_data[1][0],{(16-PARAM_WIDTH){1'b0}},x2x_converted_data[0][0]};
                    shares[1][ctr_block_w] <= {{(16-PARAM_WIDTH){1'b0}},x2x_converted_data[1][1],{(16-PARAM_WIDTH){1'b0}},x2x_converted_data[0][1]};
                end
            end    
        end
    end
end
*/

always @(posedge clk) begin
    if (!rst_n) 
    begin
        for (int i = 0; i < SHARES; i = i + 1)
            for (int j = 0; j < BURST_LEN; j = j + 1)
                shares[i][j] <= 0;
    end 
    else 
    begin
        if (write_s0 && (ctr_block_w - 1) < BURST_LEN)
            shares[0][ctr_block_w - 1] <= mem_i_data;

        else if (write_s1 && (ctr_block_w - 1) < BURST_LEN) 
        begin
            if (!ctrl_mask_mode)
                shares[1][ctr_block_w - 1] <= mem_i_data;
            else
                shares[1][ctr_block_w - 1] <= 0;
        end

        else if (x2x_valid_result && ctr_block_w < BURST_LEN) 
        begin
            if (!ctrl_dual_mode) begin
                shares[0][ctr_block_w] <= {{(32 - PARAM_WIDTH){1'b0}}, x2x_converted_data[0][0]};
                shares[1][ctr_block_w] <= {{(32 - PARAM_WIDTH){1'b0}}, x2x_converted_data[0][1]};
            end else begin
                if(dualprime)
                begin
                    if(!dualprime_msh)
                    begin
                        shares[0][ctr_block_w][15:0] <= {{(16 - PARAM_WIDTH){1'b0}}, x2x_converted_data[0][0]};
                        shares[1][ctr_block_w][15:0] <= {{(16 - PARAM_WIDTH){1'b0}}, x2x_converted_data[0][1]};
                    end
                    else
                    begin
                        shares[0][ctr_block_w][31:16] <= {{(16 - PARAM_WIDTH){1'b0}}, x2x_converted_data[0][0]};
                        shares[1][ctr_block_w][31:16] <= {{(16 - PARAM_WIDTH){1'b0}}, x2x_converted_data[0][1]};
                    end
                end
                else
                begin
                    shares[0][ctr_block_w] <= {{(16 - PARAM_WIDTH){1'b0}}, x2x_converted_data[1][0], {(16 - PARAM_WIDTH){1'b0}}, x2x_converted_data[0][0]};
                    shares[1][ctr_block_w] <= {{(16 - PARAM_WIDTH){1'b0}}, x2x_converted_data[1][1], {(16 - PARAM_WIDTH){1'b0}}, x2x_converted_data[0][1]};
                end
            end
        end
    end
end

for(genvar j = 0; j < RND_SHARES ; j = j + 1) begin
    always @(*) begin
            x2x_fresh_rnd_shares[j] = stream_out[(PARAM_WIDTH*(j+1)-1):(PARAM_WIDTH*j)];   
    end
end

for(genvar j = 0; j < RND_SHARES_8bit ; j = j + 1) begin
    always @(*) begin
        if (!rst_n)
            x2x_fresh_rnd_shares_8bit[j] <= stream_out[(8*(j+1)-1):(8*j)];     
    end
end
endmodule