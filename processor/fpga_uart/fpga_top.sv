module fpga_top
    (
        input                       clk_i  ,
        input                       reset_i,
        input                       rx_i   ,
        output                      tx_o   ,
        output                      led    ,
        output     [GPIO_WIDTH-1:0] gpio

`ifdef CW305
       ,input                       usb_clk    ,
        inout                 [7:0] usb_data   ,
        input  [USB_ADDR_WIDTH-1:0] usb_addr   ,
        input                       usb_rdn    ,
        input                       usb_wrn    ,
        input                       usb_cen    ,
        input                       usb_trigger,
        output                      tio_clkout ,
        output                      tio_trigger
`endif
    );


///////////////////////////////////////////////////////
//////////////// 0: Crypto disabled......./////////////
//////////////// 1: Crypto enabled w/out masking...////
//////////////// 2: Crypto enabled  with masking...////
parameter MODE = 2;////////////////////////////////////
///////////////////////////////////////////////////////

`ifdef CW305
parameter SYS_CLK_FREQ     = 40000000;
parameter USB_ADDR_WIDTH   = 21      ;
parameter CW305_FIFO_BSIZE = 128     ;
parameter PRNG_OFF_EN      = 1       ;
`else
parameter SYS_CLK_FREQ   = 61000000;
parameter PRNG_OFF_EN    = 0       ;
`endif
parameter UART_BAUD      = 9600    ;
parameter GPIO_WIDTH     = 8       ;

parameter ROM_START      = 32'h0000_0000;
parameter ROM_END        = 32'h0000_081F;

parameter RAM_INST_START = 32'h0000_0820;
parameter RAM_INST_END   = 32'h0000_FFFF;

parameter RAM_DATA_START = 32'h0001_0000;
parameter RAM_DATA_END   = 32'h0002_FFFF;

parameter MTIME_START    = 32'h2000_8000;
parameter MTIME_END      = 32'h2000_800F;

parameter UART_START     = 32'h1000_8010;
parameter UART_END       = 32'h1000_8013;

parameter RESET_START    = 32'h1000_8014;
parameter RESET_END      = 32'h1000_8014;

parameter TIMER_START    = 32'h1000_8018;
parameter TIMER_END      = 32'h1000_801F;

parameter GPIO_START     = 32'h1000_8020;
parameter GPIO_END       = 32'h1000_802F;

parameter NTT_START      = 32'h1004_0000;
parameter NTT_END        = 32'h1004_001F;

parameter KECCAK_START   = 32'h1004_0020;
parameter KECCAK_END     = 32'h1004_005F;

parameter X2X_START      = 32'h1004_0060;
parameter X2X_END        = 32'h1004_009F;

parameter CW305_START    = 32'h1004_1000;
parameter CW305_END      = 32'h1004_1003;


localparam NUM_DMA_ACCS  = (MODE == 2)? 3 : (MODE == 1)? 2 : 0;
localparam NUM_DMA_ACCS_ = (NUM_DMA_ACCS == 0) ? 1 : NUM_DMA_ACCS; // to avoid zero-width arrays
`ifdef CW305
localparam CW305_SLAVE   = 1;
`else
localparam CW305_SLAVE   = 0;
`endif
localparam NUM_SLAVES    = 6 + NUM_DMA_ACCS + CW305_SLAVE;


wire clk;
wire loader_reset;
wire reset;
wire irq_ack_o;
wire bootloader_en;
wire mtip;
wire rx_irq_o;
wire [7:0] rx_byte;

`ifdef CW305
wire clk_i_bufg, clk_i_buf;
`endif


//Wishbone master interface signals for core
wire        data_wb_cyc_o  ;
wire        data_wb_stb_o  ;
wire        data_wb_we_o   ;
wire [31:0] data_wb_adr_o  ;
wire [31:0] data_wb_dat_o  ;
wire [ 3:0] data_wb_sel_o  ;
reg         data_wb_stall_i;
reg         data_wb_ack_i  ;
reg  [31:0] data_wb_dat_i  ;
reg         data_wb_err_i  ;
wire        data_wb_rst_i  ;
wire        data_wb_clk_i  ;

wire        inst_wb_cyc_o  ;
wire        inst_wb_stb_o  ;
wire        inst_wb_we_o   ;
wire [31:0] inst_wb_adr_o  ;
wire [31:0] inst_wb_dat_o  ;
wire [ 3:0] inst_wb_sel_o  ;
wire        inst_wb_stall_i;
wire        inst_wb_ack_i  ;
wire [31:0] inst_wb_dat_i  ;
wire        inst_wb_err_i  ;
wire        inst_wb_rst_i  ;
wire        inst_wb_clk_i  ;

//Wishbone slave signals for peripherals
wire        wb_cyc_i   [NUM_SLAVES-1:0];
wire        wb_stb_i   [NUM_SLAVES-1:0];
wire        wb_we_i    [NUM_SLAVES-1:0];
wire [31:0] wb_adr_i   [NUM_SLAVES-1:0];
wire [31:0] wb_dat_i   [NUM_SLAVES-1:0];
wire [ 3:0] wb_sel_i   [NUM_SLAVES-1:0];
wire        wb_stall_o [NUM_SLAVES-1:0];
wire        wb_ack_o   [NUM_SLAVES-1:0];
wire [31:0] wb_dat_o   [NUM_SLAVES-1:0];
wire        wb_err_o   [NUM_SLAVES-1:0];
wire        wb_rst_i   [NUM_SLAVES-1:0];
wire        wb_clk_i   [NUM_SLAVES-1:0];
reg         stb_q      [NUM_SLAVES-1:0];

wire [31:0] slave_adr_begin [NUM_SLAVES-1:0];
wire [31:0] slave_adr_end   [NUM_SLAVES-1:0];

wire        dma_cyc_i   [NUM_DMA_ACCS_-1:0];
wire        dma_stb_i   [NUM_DMA_ACCS_-1:0];
wire        dma_we_i    [NUM_DMA_ACCS_-1:0];
wire [31:0] dma_adr_i   [NUM_DMA_ACCS_-1:0];
wire [31:0] dma_dat_i   [NUM_DMA_ACCS_-1:0];
wire [ 3:0] dma_sel_i   [NUM_DMA_ACCS_-1:0];
wire        dma_stall_o [NUM_DMA_ACCS_-1:0];
wire        dma_ack_o   [NUM_DMA_ACCS_-1:0];
wire [31:0] dma_dat_o   [NUM_DMA_ACCS_-1:0];
wire        dma_err_o   [NUM_DMA_ACCS_-1:0];
wire        dma_rst_i   [NUM_DMA_ACCS_-1:0];


assign slave_adr_begin[0] = ROM_START   ;
assign slave_adr_end  [0] = RAM_DATA_END;

assign slave_adr_begin[1] = MTIME_START ;
assign slave_adr_end  [1] = MTIME_END   ;

assign slave_adr_begin[2] = UART_START  ;
assign slave_adr_end  [2] = UART_END    ;

assign slave_adr_begin[3] = RESET_START ;
assign slave_adr_end  [3] = RESET_END   ;

assign slave_adr_begin[4] = TIMER_START ;
assign slave_adr_end  [4] = TIMER_END   ;

assign slave_adr_begin[5] = GPIO_START  ;
assign slave_adr_end  [5] = GPIO_END    ;

if (MODE == 1 || MODE == 2) begin
assign slave_adr_begin[6] = NTT_START   ;
assign slave_adr_end  [6] = NTT_END     ;

assign slave_adr_begin[7] = KECCAK_START;
assign slave_adr_end  [7] = KECCAK_END  ;
end

if (MODE == 2) begin
assign slave_adr_begin[8] = X2X_START   ;
assign slave_adr_end  [8] = X2X_END     ;
end

`ifdef CW305
assign slave_adr_begin[NUM_SLAVES-1] = CW305_START ;
assign slave_adr_end  [NUM_SLAVES-1] = CW305_END   ;
`endif


assign inst_wb_rst_i   = ~reset;
assign inst_wb_clk_i   = clk   ;


for (genvar i = 0; i < NUM_SLAVES; i = i + 1)
begin
    assign wb_cyc_i[i] = data_wb_cyc_o;
    assign wb_stb_i[i] = data_wb_stb_o & ((slave_adr_begin[i] <= wb_adr_i[i]) && (wb_adr_i[i] <= slave_adr_end[i]));
    assign wb_we_i [i] = data_wb_we_o ;
    assign wb_adr_i[i] = data_wb_adr_o;
    assign wb_dat_i[i] = data_wb_dat_o;
    assign wb_sel_i[i] = data_wb_sel_o;
    if(i == 3)
        assign wb_rst_i[i] = ~reset_i;
    else
        assign wb_rst_i[i] = ~reset;
    assign wb_clk_i[i] = clk;
end


//Register strobe signals
for (genvar i = 0; i < NUM_SLAVES; i = i + 1)
begin
    always @(posedge wb_clk_i[0] or posedge wb_rst_i[0])
    begin
        if(wb_rst_i[0])
            stb_q[i] <= 0;
        else
            stb_q[i] <= wb_stb_i[i];
    end
end


always @(*)
begin
    for (int i = 0; i < NUM_SLAVES; i = i + 1)
    begin
        data_wb_dat_i   = 32'd0;
        data_wb_stall_i = 1'b0 ;
        data_wb_err_i   = 1'b0 ;
        data_wb_ack_i   = 1'b0 ;
    end

    for (int i = 0; i < NUM_SLAVES; i = i + 1)
    begin
        if(stb_q[i])
        begin
            data_wb_dat_i   = wb_dat_o  [i];
            data_wb_stall_i = wb_stall_o[i];
            data_wb_err_i   = wb_err_o  [i];
            data_wb_ack_i   = wb_ack_o  [i];
        end
    end
end

assign data_wb_clk_i   = clk;
assign data_wb_rst_i   = ~reset;


if (MODE == 0) begin
    assign dma_stb_i[0] = 1'b0;
end


assign reset = loader_reset & reset_i;
assign led   = bootloader_en;



`ifdef CW305
IBUFG clkibuf (
    .I(clk_i     ),
    .O(clk_i_bufg)
);
BUFG clkbuf(
    .I(clk_i_bufg),
    .O(clk       )
);
`else
clk_wiz_0 clkwiz0
(
   .clk_out1(clk  ),
   .reset   (1'b0 ),
   .clk_in1 (clk_i)
);
`endif


core_wb #(
    .reset_vector(ROM_START)
) core0 (
    .reset_i(reset),
    .clk_i  (clk  ),
    
    .data_wb_cyc_o  (data_wb_cyc_o  ),
    .data_wb_stb_o  (data_wb_stb_o  ),
    .data_wb_we_o   (data_wb_we_o   ),
    .data_wb_adr_o  (data_wb_adr_o  ),
    .data_wb_dat_o  (data_wb_dat_o  ),
    .data_wb_sel_o  (data_wb_sel_o  ),
    .data_wb_stall_i(data_wb_stall_i),
    .data_wb_ack_i  (data_wb_ack_i  ),
    .data_wb_dat_i  (data_wb_dat_i  ),
    .data_wb_err_i  (data_wb_err_i  ),
    .data_wb_rst_i  (data_wb_rst_i  ),
    .data_wb_clk_i  (data_wb_clk_i  ),
    
    .inst_wb_cyc_o  (inst_wb_cyc_o  ),
    .inst_wb_stb_o  (inst_wb_stb_o  ),
    .inst_wb_we_o   (inst_wb_we_o   ),
    .inst_wb_adr_o  (inst_wb_adr_o  ),
    .inst_wb_dat_o  (inst_wb_dat_o  ),
    .inst_wb_sel_o  (inst_wb_sel_o  ),
    .inst_wb_stall_i(inst_wb_stall_i),
    .inst_wb_ack_i  (inst_wb_ack_i  ),
    .inst_wb_dat_i  (inst_wb_dat_i  ),
    .inst_wb_err_i  (inst_wb_err_i  ),
    .inst_wb_rst_i  (inst_wb_rst_i  ),
    .inst_wb_clk_i  (inst_wb_clk_i  ),
    
    .meip_i    (1'b0            ),
    .mtip_i    (mtip            ),
    .msip_i    (1'b0            ),
    .fast_irq_i({15'b0,rx_irq_o}),
    .irq_ack_o (irq_ack_o       )
);


memory_2rw_wb_dma #(
    .ROM_START     (ROM_START     ),
    .ROM_END       (ROM_END       ),
    .RAM_INST_START(RAM_INST_START),
    .RAM_INST_END  (RAM_INST_END  ),
    .RAM_DATA_START(RAM_DATA_START),
    .RAM_DATA_END  (RAM_DATA_END  ),
    .NUM_DMA_ACCS  (NUM_DMA_ACCS_ )
) memory (
    .bootloader_en   (bootloader_en),

    .port0_wb_cyc_i  (inst_wb_cyc_o  ),
    .port0_wb_stb_i  (inst_wb_stb_o  ),
    .port0_wb_we_i   (inst_wb_we_o   ),
    .port0_wb_adr_i  (inst_wb_adr_o  ),
    .port0_wb_dat_i  (inst_wb_dat_o  ),
    .port0_wb_sel_i  (inst_wb_sel_o  ),
    .port0_wb_stall_o(inst_wb_stall_i),
    .port0_wb_ack_o  (inst_wb_ack_i  ),
    .port0_wb_dat_o  (inst_wb_dat_i  ),
    .port0_wb_err_o  (inst_wb_err_i  ),
    .port0_wb_rst_i  (inst_wb_rst_i  ),
    .port0_wb_clk_i  (inst_wb_clk_i  ),

    .port1_wb_cyc_i  (wb_cyc_i  [0]),
    .port1_wb_stb_i  (wb_stb_i  [0]),
    .port1_wb_we_i   (wb_we_i   [0]),
    .port1_wb_adr_i  (wb_adr_i  [0]),
    .port1_wb_dat_i  (wb_dat_i  [0]),
    .port1_wb_sel_i  (wb_sel_i  [0]),
    .port1_wb_stall_o(wb_stall_o[0]),
    .port1_wb_ack_o  (wb_ack_o  [0]),
    .port1_wb_dat_o  (wb_dat_o  [0]),
    .port1_wb_err_o  (wb_err_o  [0]),
    .port1_wb_rst_i  (wb_rst_i  [0]),
    .port1_wb_clk_i  (wb_clk_i  [0]),
    
    .dma_cyc_i  (dma_cyc_i  ),
    .dma_stb_i  (dma_stb_i  ),
    .dma_we_i   (dma_we_i   ),
    .dma_adr_i  (dma_adr_i  ),
    .dma_dat_i  (dma_dat_i  ),
    .dma_sel_i  (dma_sel_i  ),
    .dma_stall_o(dma_stall_o),
    .dma_ack_o  (dma_ack_o  ),
    .dma_dat_o  (dma_dat_o  ),
    .dma_err_o  (dma_err_o  ),
    .dma_rst_i  (dma_rst_i  )
);


mtime_registers_wb #(
    .BASE_ADDR(MTIME_START)
) mtime_regs ( 
    .wb_cyc_i  (wb_cyc_i  [1]),
    .wb_stb_i  (wb_stb_i  [1]),
    .wb_we_i   (wb_we_i   [1]),
    .wb_adr_i  (wb_adr_i  [1]),
    .wb_dat_i  (wb_dat_i  [1]),
    .wb_sel_i  (wb_sel_i  [1]),
    .wb_stall_o(wb_stall_o[1]),
    .wb_ack_o  (wb_ack_o  [1]),
    .wb_dat_o  (wb_dat_o  [1]),
    .wb_err_o  (wb_err_o  [1]),
    .wb_rst_i  (wb_rst_i  [1]),
    .wb_clk_i  (wb_clk_i  [1]),

    .mtip_o    (mtip         )
);


uart_wb #(
    .SYS_CLK_FREQ(SYS_CLK_FREQ),
    .BAUD        (UART_BAUD   )
) uart0 ( 
    .wb_cyc_i  (wb_cyc_i  [2]),
    .wb_stb_i  (wb_stb_i  [2]),
    .wb_we_i   (wb_we_i   [2]),
    .wb_adr_i  (wb_adr_i  [2]),
    .wb_dat_i  (wb_dat_i  [2]),
    .wb_sel_i  (wb_sel_i  [2]),
    .wb_stall_o(wb_stall_o[2]),
    .wb_ack_o  (wb_ack_o  [2]),
    .wb_dat_o  (wb_dat_o  [2]),
    .wb_err_o  (wb_err_o  [2]),
    .wb_rst_i  (wb_rst_i  [2]),
    .wb_clk_i  (wb_clk_i  [2]),

    .rx_i      (rx_i    ),
    .tx_o      (tx_o    ),
    .rx_byte_o (rx_byte ),
    .rx_irq_o  (rx_irq_o)
);


loader_wb #(
    .SYS_CLK_FREQ(SYS_CLK_FREQ)
) loader0 (
    .wb_cyc_i  (wb_cyc_i  [3]),
    .wb_stb_i  (wb_stb_i  [3]),
    .wb_we_i   (wb_we_i   [3]),
    .wb_adr_i  (wb_adr_i  [3]),
    .wb_dat_i  (wb_dat_i  [3]),
    .wb_sel_i  (wb_sel_i  [3]),
    .wb_stall_o(wb_stall_o[3]),
    .wb_ack_o  (wb_ack_o  [3]),
    .wb_dat_o  (wb_dat_o  [3]),
    .wb_err_o  (wb_err_o  [3]),
    .wb_rst_i  (wb_rst_i  [3]),
    .wb_clk_i  (wb_clk_i  [3]),

    .bootloader_en(bootloader_en),
    .uart_rx_irq  (rx_irq_o     ),
    .uart_rx_byte (rx_byte      ),
    .reset_o      (loader_reset )
);


timer_wb #(
    .BASE_ADDR(TIMER_START)
) timer0 (
    .wb_cyc_i  (wb_cyc_i  [4]),
    .wb_stb_i  (wb_stb_i  [4]),
    .wb_we_i   (wb_we_i   [4]),
    .wb_adr_i  (wb_adr_i  [4]),
    .wb_dat_i  (wb_dat_i  [4]),
    .wb_sel_i  (wb_sel_i  [4]),
    .wb_stall_o(wb_stall_o[4]),
    .wb_ack_o  (wb_ack_o  [4]),
    .wb_dat_o  (wb_dat_o  [4]),
    .wb_err_o  (wb_err_o  [4]),
    .wb_rst_i  (wb_rst_i  [4]),
    .wb_clk_i  (wb_clk_i  [4])
);


gpio_wb #(
    .BASE_ADDR(GPIO_START),
    .WIDTH    (GPIO_WIDTH)
) gpio_inst (
    .wb_cyc_i  (wb_cyc_i  [5]),
    .wb_stb_i  (wb_stb_i  [5]),
    .wb_we_i   (wb_we_i   [5]),
    .wb_adr_i  (wb_adr_i  [5]),
    .wb_dat_i  (wb_dat_i  [5]),
    .wb_sel_i  (wb_sel_i  [5]),
    .wb_stall_o(wb_stall_o[5]),
    .wb_ack_o  (wb_ack_o  [5]),
    .wb_dat_o  (wb_dat_o  [5]),
    .wb_err_o  (wb_err_o  [5]),
    .wb_rst_i  (wb_rst_i  [5]),
    .wb_clk_i  (wb_clk_i  [5]),

    .gpio      (gpio)
);


if (MODE == 1 || MODE == 2) begin

ntt_lite_acc_top #(
    .BASE_ADDR(NTT_START)
) ntt_lite_acc_top_inst (
    .wb_cyc_i  (wb_cyc_i  [6]),
    .wb_stb_i  (wb_stb_i  [6]),
    .wb_we_i   (wb_we_i   [6]),
    .wb_adr_i  (wb_adr_i  [6]),
    .wb_dat_i  (wb_dat_i  [6]),
    .wb_sel_i  (wb_sel_i  [6]),
    .wb_stall_o(wb_stall_o[6]),
    .wb_ack_o  (wb_ack_o  [6]),
    .wb_dat_o  (wb_dat_o  [6]),
    .wb_err_o  (wb_err_o  [6]),
    .wb_rst_i  (wb_rst_i  [6]),
    .wb_clk_i  (wb_clk_i  [6]),
    
    .dma_cyc_i  (dma_cyc_i  [0]),
    .dma_stb_i  (dma_stb_i  [0]),
    .dma_we_i   (dma_we_i   [0]),
    .dma_adr_i  (dma_adr_i  [0]),
    .dma_dat_i  (dma_dat_i  [0]),
    .dma_sel_i  (dma_sel_i  [0]),
    .dma_stall_o(dma_stall_o[0]),
    .dma_ack_o  (dma_ack_o  [0]),
    .dma_dat_o  (dma_dat_o  [0]),
    .dma_err_o  (dma_err_o  [0]),
    .dma_rst_i  (dma_rst_i  [0])
);


keccak_acc_top #(
    .BASE_ADDR(KECCAK_START),
    .SHARES   (MODE        )
) keccak_acc_top_inst (
    .wb_cyc_i  (wb_cyc_i  [7]),
    .wb_stb_i  (wb_stb_i  [7]),
    .wb_we_i   (wb_we_i   [7]),
    .wb_adr_i  (wb_adr_i  [7]),
    .wb_dat_i  (wb_dat_i  [7]),
    .wb_sel_i  (wb_sel_i  [7]),
    .wb_stall_o(wb_stall_o[7]),
    .wb_ack_o  (wb_ack_o  [7]),
    .wb_dat_o  (wb_dat_o  [7]),
    .wb_err_o  (wb_err_o  [7]),
    .wb_rst_i  (wb_rst_i  [7]),
    .wb_clk_i  (wb_clk_i  [7]),
    
    .dma_cyc_i  (dma_cyc_i  [1]),
    .dma_stb_i  (dma_stb_i  [1]),
    .dma_we_i   (dma_we_i   [1]),
    .dma_adr_i  (dma_adr_i  [1]),
    .dma_dat_i  (dma_dat_i  [1]),
    .dma_sel_i  (dma_sel_i  [1]),
    .dma_stall_o(dma_stall_o[1]),
    .dma_ack_o  (dma_ack_o  [1]),
    .dma_dat_o  (dma_dat_o  [1]),
    .dma_err_o  (dma_err_o  [1]),
    .dma_rst_i  (dma_rst_i  [1])
);

end


if (MODE == 2) begin

x2x_acc_top #(
    .BASE_ADDR  (X2X_START  ),
    .PRNG_OFF_EN(PRNG_OFF_EN)
) x2x_acc_top_inst (
    .wb_cyc_i  (wb_cyc_i  [8]),
    .wb_stb_i  (wb_stb_i  [8]),
    .wb_we_i   (wb_we_i   [8]),
    .wb_adr_i  (wb_adr_i  [8]),
    .wb_dat_i  (wb_dat_i  [8]),
    .wb_sel_i  (wb_sel_i  [8]),
    .wb_stall_o(wb_stall_o[8]),
    .wb_ack_o  (wb_ack_o  [8]),
    .wb_dat_o  (wb_dat_o  [8]),
    .wb_err_o  (wb_err_o  [8]),
    .wb_rst_i  (wb_rst_i  [8]),
    .wb_clk_i  (wb_clk_i  [8]),
    
    .dma_cyc_i  (dma_cyc_i  [2]),
    .dma_stb_i  (dma_stb_i  [2]),
    .dma_we_i   (dma_we_i   [2]),
    .dma_adr_i  (dma_adr_i  [2]),
    .dma_dat_i  (dma_dat_i  [2]),
    .dma_sel_i  (dma_sel_i  [2]),
    .dma_stall_o(dma_stall_o[2]),
    .dma_ack_o  (dma_ack_o  [2]),
    .dma_dat_o  (dma_dat_o  [2]),
    .dma_err_o  (dma_err_o  [2]),
    .dma_rst_i  (dma_rst_i  [2])
);

end


`ifdef CW305

cw305_wb #(
    .BASE_ADDR     (CW305_START     ),
    .USB_ADDR_WIDTH(USB_ADDR_WIDTH  ),
    .FIFO_BSIZE    (CW305_FIFO_BSIZE)
) cw305_inst (
    .wb_cyc_i  (wb_cyc_i  [NUM_SLAVES-1]),
    .wb_stb_i  (wb_stb_i  [NUM_SLAVES-1]),
    .wb_we_i   (wb_we_i   [NUM_SLAVES-1]),
    .wb_adr_i  (wb_adr_i  [NUM_SLAVES-1]),
    .wb_dat_i  (wb_dat_i  [NUM_SLAVES-1]),
    .wb_sel_i  (wb_sel_i  [NUM_SLAVES-1]),
    .wb_stall_o(wb_stall_o[NUM_SLAVES-1]),
    .wb_ack_o  (wb_ack_o  [NUM_SLAVES-1]),
    .wb_dat_o  (wb_dat_o  [NUM_SLAVES-1]),
    .wb_err_o  (wb_err_o  [NUM_SLAVES-1]),
    .wb_rst_i  (wb_rst_i  [NUM_SLAVES-1]),
    .wb_clk_i  (wb_clk_i  [NUM_SLAVES-1]),

    .usb_clk    (usb_clk      ),
    .usb_data   (usb_data     ),
    .usb_addr   (usb_addr     ),
    .usb_rdn    (usb_rdn      ),
    .usb_wrn    (usb_wrn      ),
    .usb_cen    (usb_cen      ),
    .usb_trigger(usb_trigger  ),
    .tio_clkout (tio_clkout   ),
    .tio_trigger(tio_trigger  )
);

`endif

endmodule