module memory_2rw_wb_dma
    #(  
        parameter ADDR_WIDTH     = 18, 
        parameter ROM_START      = 0 ,
        parameter NUM_DMA_ACCS   = 3 ,
        parameter FPGA_READMEM   = 1 ,
        parameter NUM_WMASKS     = 4 ,
        parameter DATA_WIDTH     = 32,
        parameter RAM_DEPTH      = 1 << ADDR_WIDTH,
        parameter DATA_MEM_START = 16'hC000
    )
    (
        input             port0_wb_cyc_i  ,
        input             port0_wb_stb_i  ,
        input             port0_wb_we_i   ,
        input      [31:0] port0_wb_adr_i  ,
        input      [31:0] port0_wb_dat_i  ,
        input      [ 3:0] port0_wb_sel_i  ,
        input             port0_wb_rst_i  ,
        input             port0_wb_clk_i  ,
        output            port0_wb_stall_o,
        output            port0_wb_ack_o  ,
        output     [31:0] port0_wb_dat_o  ,
        output            port0_wb_err_o  ,

        input             port1_wb_cyc_i  ,
        input             port1_wb_stb_i  ,
        input             port1_wb_we_i   ,
        input      [31:0] port1_wb_adr_i  ,
        input      [31:0] port1_wb_dat_i  ,
        input      [ 3:0] port1_wb_sel_i  ,
        input             port1_wb_rst_i  ,
        input             port1_wb_clk_i  ,
        output reg        port1_wb_stall_o,
        output reg        port1_wb_ack_o  ,
        output reg [31:0] port1_wb_dat_o  ,
        output reg        port1_wb_err_o  ,

        input             dma_cyc_i   [NUM_DMA_ACCS-1:0],
        input             dma_stb_i   [NUM_DMA_ACCS-1:0],
        input             dma_we_i    [NUM_DMA_ACCS-1:0],
        input      [31:0] dma_adr_i   [NUM_DMA_ACCS-1:0],
        input      [31:0] dma_dat_i   [NUM_DMA_ACCS-1:0],
        input      [ 3:0] dma_sel_i   [NUM_DMA_ACCS-1:0],
        input             dma_rst_i   [NUM_DMA_ACCS-1:0],
        output reg        dma_stall_o [NUM_DMA_ACCS-1:0],
        output reg        dma_ack_o   [NUM_DMA_ACCS-1:0],
        output reg [31:0] dma_dat_o   [NUM_DMA_ACCS-1:0],
        output reg        dma_err_o   [NUM_DMA_ACCS-1:0]
    );

localparam LOG_NUM_DMA_ACCS = $clog2(NUM_DMA_ACCS);


//inst RAM P1
reg          inst_p1_cyc_i;
reg          inst_p1_stb_i;
reg          inst_p1_we_i;
reg [31:0]   inst_p1_adr_i;
reg [31:0]   inst_p1_dat_i;
reg [3:0]    inst_p1_sel_i;
reg          inst_p1_rst_i;
reg          inst_p1_clk_i;
wire         inst_p1_stall_o;
wire         inst_p1_ack_o;
wire  [31:0] inst_p1_dat_o;
wire         inst_p1_err_o;

//data RAM P0
reg          data_p0_cyc_i;
reg          data_p0_stb_i;
reg          data_p0_we_i;
reg [31:0]   data_p0_adr_i;
reg [31:0]   data_p0_dat_i;
reg [3:0]    data_p0_sel_i;
reg          data_p0_rst_i;
reg          data_p0_clk_i;
wire         data_p0_stall_o;
wire         data_p0_ack_o;
wire  [31:0] data_p0_dat_o;
wire         data_p0_err_o;

//data RAM P1
reg          data_dma_cyc_i;
reg          data_dma_stb_i;
reg          data_dma_we_i;
reg [31:0]   data_dma_adr_i;
reg [31:0]   data_dma_dat_i;
reg [3:0]    data_dma_sel_i;
reg          data_dma_rst_i;
wire         data_dma_clk_i;
wire         data_dma_stall_o;
wire         data_dma_ack_o;
wire  [31:0] data_dma_dat_o;
wire         data_dma_err_o;

reg  [    NUM_DMA_ACCS-1:0] dma_stb_q;
reg  [LOG_NUM_DMA_ACCS-1:0] dma_stb_bin;
reg  [LOG_NUM_DMA_ACCS-1:0] dma_stb_q_bin;


memory_2rw_wb #(
        .ADDR_WIDTH  (ADDR_WIDTH  ),
        .DATA_WIDTH  (DATA_WIDTH  ),
        .NUM_WMASKS  (NUM_WMASKS  ),
        .FPGA_READMEM(FPGA_READMEM),
        .RAM_DEPTH   (DATA_MEM_START >> 2),
        .ROM_START   (ROM_START   )
    ) memory_inst (
        .port0_wb_cyc_i  (port0_wb_cyc_i  ),
        .port0_wb_stb_i  (port0_wb_stb_i  ),
        .port0_wb_we_i   (port0_wb_we_i   ),
        .port0_wb_adr_i  (port0_wb_adr_i  ),
        .port0_wb_dat_i  (port0_wb_dat_i  ),
        .port0_wb_sel_i  (port0_wb_sel_i  ),
        .port0_wb_stall_o(port0_wb_stall_o),
        .port0_wb_ack_o  (port0_wb_ack_o  ),
        .port0_wb_dat_o  (port0_wb_dat_o  ),
        .port0_wb_err_o  (port0_wb_err_o  ),
        .port0_wb_rst_i  (port0_wb_rst_i  ),
        .port0_wb_clk_i  (port0_wb_clk_i  ),

        .port1_wb_cyc_i  (inst_p1_cyc_i   ),
        .port1_wb_stb_i  (inst_p1_stb_i   ),
        .port1_wb_we_i   (inst_p1_we_i    ),
        .port1_wb_adr_i  (inst_p1_adr_i   ),
        .port1_wb_dat_i  (inst_p1_dat_i   ),
        .port1_wb_sel_i  (inst_p1_sel_i   ),
        .port1_wb_stall_o(inst_p1_stall_o ),
        .port1_wb_ack_o  (inst_p1_ack_o   ),
        .port1_wb_dat_o  (inst_p1_dat_o   ),
        .port1_wb_err_o  (inst_p1_err_o   ),
        .port1_wb_rst_i  (inst_p1_rst_i   ),
        .port1_wb_clk_i  (inst_p1_clk_i   )
    );
    
memory_2rw_wb #(
        .ADDR_WIDTH  (ADDR_WIDTH),
        .DATA_WIDTH  (DATA_WIDTH),
        .NUM_WMASKS  (NUM_WMASKS),
        .FPGA_READMEM(0         ),
        .RAM_DEPTH   (RAM_DEPTH - (DATA_MEM_START >> 2)),
        .ROM_START   (0         )
    ) memory_data (
        .port0_wb_cyc_i  (data_p0_cyc_i  ),
        .port0_wb_stb_i  (data_p0_stb_i  ),
        .port0_wb_we_i   (data_p0_we_i   ),
        .port0_wb_adr_i  (data_p0_adr_i  ),
        .port0_wb_dat_i  (data_p0_dat_i  ),
        .port0_wb_sel_i  (data_p0_sel_i  ),
        .port0_wb_stall_o(data_p0_stall_o),
        .port0_wb_ack_o  (data_p0_ack_o  ),
        .port0_wb_dat_o  (data_p0_dat_o  ),
        .port0_wb_err_o  (data_p0_err_o  ),
        .port0_wb_rst_i  (data_p0_rst_i  ),
        .port0_wb_clk_i  (data_p0_clk_i  ),

        .port1_wb_cyc_i  (data_dma_cyc_i  ),
        .port1_wb_stb_i  (data_dma_stb_i  ),
        .port1_wb_we_i   (data_dma_we_i   ),
        .port1_wb_adr_i  (data_dma_adr_i  ),
        .port1_wb_dat_i  (data_dma_dat_i  ),
        .port1_wb_sel_i  (data_dma_sel_i  ),
        .port1_wb_stall_o(data_dma_stall_o),
        .port1_wb_ack_o  (data_dma_ack_o  ),
        .port1_wb_dat_o  (data_dma_dat_o  ),
        .port1_wb_err_o  (data_dma_err_o  ),
        .port1_wb_rst_i  (data_dma_rst_i  ),
        .port1_wb_clk_i  (data_dma_clk_i  )
    );
   
// CORE DATA inputs    
always @(*)
begin
    if(port1_wb_adr_i < DATA_MEM_START)
    begin
        inst_p1_cyc_i = port1_wb_cyc_i;
        inst_p1_stb_i = port1_wb_stb_i;
        inst_p1_we_i  = port1_wb_we_i;
        inst_p1_adr_i = port1_wb_adr_i;
        inst_p1_dat_i = port1_wb_dat_i;
        inst_p1_sel_i = port1_wb_sel_i;
        inst_p1_rst_i = port1_wb_rst_i;
        inst_p1_clk_i = port1_wb_clk_i;
        
        data_p0_cyc_i = 0;
        data_p0_stb_i = 0;
        data_p0_we_i  = 0;
        data_p0_adr_i = 0;
        data_p0_dat_i = 0;
        data_p0_sel_i = 0;
        data_p0_rst_i = port1_wb_rst_i;
        data_p0_clk_i = port1_wb_clk_i;
    end
    else
    begin
        inst_p1_cyc_i = 0;
        inst_p1_stb_i = 0;
        inst_p1_we_i  = 0;
        inst_p1_adr_i = 0;
        inst_p1_dat_i = 0;
        inst_p1_sel_i = 0;
        inst_p1_rst_i = port1_wb_rst_i;
        inst_p1_clk_i = port1_wb_clk_i;
        
        data_p0_cyc_i = port1_wb_cyc_i;
        data_p0_stb_i = port1_wb_stb_i;
        data_p0_we_i  = port1_wb_we_i;
        data_p0_adr_i = port1_wb_adr_i - DATA_MEM_START;
        data_p0_dat_i = port1_wb_dat_i;
        data_p0_sel_i = port1_wb_sel_i;
        data_p0_rst_i = port1_wb_rst_i;
        data_p0_clk_i = port1_wb_clk_i;
    end
end

// CORE P1 outputs
always @(*)
begin
    if(inst_p1_ack_o)
    begin
        port1_wb_stall_o = inst_p1_stall_o;
        port1_wb_ack_o   = inst_p1_ack_o;
        port1_wb_dat_o   = inst_p1_dat_o;
        port1_wb_err_o   = inst_p1_err_o;
    end  
    else
    begin
        port1_wb_stall_o = data_p0_stall_o;
        port1_wb_ack_o   = data_p0_ack_o;
        port1_wb_dat_o   = data_p0_dat_o;
        port1_wb_err_o   = data_p0_err_o;
    end    
end


always @(*) begin
    dma_stb_q_bin = 0;
    for (int i = NUM_DMA_ACCS - 1; i >= 0; i = i - 1) begin
        if (dma_stb_q[i]) begin
            dma_stb_q_bin = i;
        end
    end
end


always @(*) begin
    dma_stb_bin = 0;
    for (int i = NUM_DMA_ACCS - 1; i >= 0; i = i - 1) begin
        if (dma_stb_i[i]) begin
            dma_stb_bin = i;
        end
    end
end


assign data_dma_clk_i = port1_wb_clk_i;

// DMA DATA inputs 
always @(*)
begin
    data_dma_cyc_i = dma_cyc_i[dma_stb_bin];
    data_dma_stb_i = dma_stb_i[dma_stb_bin];
    data_dma_we_i  = dma_we_i [dma_stb_bin];
    data_dma_adr_i = dma_adr_i[dma_stb_bin] - DATA_MEM_START;
    data_dma_dat_i = dma_dat_i[dma_stb_bin];
    data_dma_sel_i = dma_sel_i[dma_stb_bin];
    data_dma_rst_i = dma_rst_i[dma_stb_bin];
end

// DMA DATA outputs 
always @(*)
begin
    for (int i = 0; i < NUM_DMA_ACCS; i = i + 1) begin
        dma_stall_o[i] = 0;
        dma_ack_o  [i] = 0;
        dma_dat_o  [i] = 0;
        dma_err_o  [i] = 0;
    end
    if (|dma_stb_q) begin
        dma_stall_o[dma_stb_q_bin] = data_dma_stall_o;
        dma_ack_o  [dma_stb_q_bin] = data_dma_ack_o;
        dma_dat_o  [dma_stb_q_bin] = data_dma_dat_o;
        dma_err_o  [dma_stb_q_bin] = data_dma_err_o;
    end
end


for (genvar i = 0; i < NUM_DMA_ACCS; i = i + 1) begin
    always @(posedge port0_wb_clk_i or posedge port0_wb_rst_i)
    begin
        if(port0_wb_rst_i)
            dma_stb_q[i] <= 0;
        else 
            dma_stb_q[i] <= dma_stb_i[i];
    end
end


endmodule