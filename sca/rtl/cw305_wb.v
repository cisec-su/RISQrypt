module cw305_wb
   #(
        parameter BASE_ADDR      = 32'h00000000,
        parameter FIFO_BSIZE     = 128,
        parameter USB_ADDR_WIDTH = 21 ,
        parameter BYTE_CNT_SIZE  = 7  ,
        parameter REG_RDDLY_LEN  = 3
    )
    (
        input                   wb_clk_i  ,         
        input                   wb_rst_i  ,          
        input                   wb_cyc_i  ,         
        input                   wb_stb_i  ,         
        input           [31:0]  wb_adr_i  ,         
        input                   wb_we_i   ,          
        input           [ 3:0]  wb_sel_i  ,
        input           [31:0]  wb_dat_i  ,         
        output          [31:0]  wb_dat_o  ,         
        output reg              wb_ack_o  ,         
        output                  wb_stall_o, 
        output                  wb_err_o  ,
        
        input                       usb_clk    ,
        inout                 [7:0] usb_data   ,
        input  [USB_ADDR_WIDTH-1:0] usb_addr   ,
        input                       usb_rdn    ,
        input                       usb_wrn    ,
        input                       usb_cen    ,
        input                       usb_trigger,
        output                      tio_clkout ,
        output                      tio_trigger
    );

localparam TX_SEL      = 0;
localparam RX_SEL      = 1;
localparam CTRL_SEL    = 2;
localparam TRIGGER_SEL = 3;

localparam CTRL_RX_ST_BIT   = 0;
localparam CTRL_TX_ST_BIT   = 1;

localparam CTRL_TRIGGER_BIT = 0;

// ctrl signals
reg rx_st;
reg tx_st;
reg trigger_q;


wire valid;
wire valid_r;
wire valid_w;

// usb signals
wire isout;
wire [7:0] usb_dout;
wire [BYTE_CNT_SIZE-1:0] reg_bytecnt;
wire [7:0] reg_datao;
wire [7:0] reg_datai;
wire reg_read;
reg  reg_read_q;
wire reg_read_pulse;
wire reg_write;
wire usb_clk_buf, usb_clk_bufg;

// fifo-o signals
wire fifo_o_full;
wire fifo_o_empty;
wire [7:0] fifo_o_din;
wire [7:0] fifo_o_dout;
wire fifo_o_rd_en;
wire fifo_o_wr_en;


// fifo-i signals
wire fifo_i_full;
wire fifo_i_empty;
wire [7:0] fifo_i_din;
wire [7:0] fifo_i_dout;
wire fifo_i_rd_en;
wire fifo_i_wr_en;



//////////////////////////// wishbone ///////////////////////////////////


assign valid   = wb_cyc_i & wb_stb_i;
assign valid_r = valid & ~wb_we_i;
assign valid_w = valid &  wb_we_i;

assign wb_err_o   = 1'b0;
assign wb_stall_o = 1'b0;


always @(posedge wb_clk_i or posedge wb_rst_i) begin
    if (wb_rst_i)
        wb_ack_o <= 1'b0;
    else
        wb_ack_o <= valid & ~wb_stall_o;
end


always @(posedge wb_clk_i) begin
    rx_st <= ~fifo_i_empty;
end


always @(posedge wb_clk_i) begin
    tx_st <= fifo_o_full;
end


assign wb_dat_o[TX_SEL*8 +: 8] = 8'd0;

assign wb_dat_o[RX_SEL*8 +: 8] = fifo_i_dout;

assign wb_dat_o[CTRL_SEL*8 + CTRL_RX_ST_BIT] = rx_st;
assign wb_dat_o[CTRL_SEL*8 + CTRL_TX_ST_BIT] = tx_st;
assign wb_dat_o[CTRL_SEL*8 + 7 : CTRL_SEL*8 + CTRL_TX_ST_BIT + 1] = 6'd0;

assign wb_dat_o[TRIGGER_SEL*8 + CTRL_TRIGGER_BIT] = trigger_q;
assign wb_dat_o[TRIGGER_SEL*8 + 7 : TRIGGER_SEL*8 + CTRL_TRIGGER_BIT + 1] = 7'd0;

/////////////////////////////////////////////////////////////////////////




//////////////////////////////// fifo ///////////////////////////////////

assign fifo_i_din   = reg_datao;
assign fifo_i_wr_en = reg_write;
assign fifo_i_rd_en = valid_r & wb_sel_i[RX_SEL];

assign fifo_o_din   = wb_dat_i[TX_SEL*8 +: 8];
assign fifo_o_wr_en = valid_w & wb_sel_i[TX_SEL];
assign fifo_o_rd_en = reg_read_pulse;

/////////////////////////////////////////////////////////////////////////




//////////////////////////////// usb ////////////////////////////////////

assign usb_data  = isout ? usb_dout : 8'bZ;
assign reg_datai = fifo_o_dout;

assign reg_read_pulse = reg_read & ~reg_read_q;

always @(posedge usb_clk_buf or posedge wb_rst_i) begin
    if (wb_rst_i)
        reg_read_q  <= 1'b0;
    else
        reg_read_q  <= reg_read;
end

/////////////////////////////////////////////////////////////////////////




//////////////////////////////// trigger ////////////////////////////////

assign tio_trigger = trigger_q;

always @(posedge wb_clk_i or posedge wb_rst_i) begin
    if (wb_rst_i)
        trigger_q <= 1'b0;
    else if (valid_w & wb_sel_i[TRIGGER_SEL])
        trigger_q <= wb_dat_i[TRIGGER_SEL*8 + CTRL_TRIGGER_BIT];
end

/////////////////////////////////////////////////////////////////////////




///////////////////////////// modules ///////////////////////////////////


IBUFG clkibuf (
    .O(usb_clk_bufg),
    .I(usb_clk) 
);
BUFG clkbuf(
    .O(usb_clk_buf),
    .I(usb_clk_bufg)
);


ODDR CWOUT_ODDR (
    .Q(tio_clkout),   // 1-bit DDR output
    .C(wb_clk_i  ),   // 1-bit clock input
    .CE(1'b1),        // 1-bit clock enable input
    .D1(1'b1),        // 1-bit data input (positive edge)
    .D2(1'b0),        // 1-bit data input (negative edge)
    .R (1'b0),        // 1-bit reset
    .S (1'b0)         // 1-bit set
);


cw305_usb_reg_fe #(
    .pBYTECNT_SIZE           (BYTE_CNT_SIZE ),
    .pREG_RDDLY_LEN          (REG_RDDLY_LEN ),
    .pADDR_WIDTH             (USB_ADDR_WIDTH)
) U_usb_reg_fe (
    .rst                     (wb_rst_i   ),
    .usb_clk                 (usb_clk_buf), 
    .usb_din                 (usb_data   ), 
    .usb_dout                (usb_dout   ), 
    .usb_rdn                 (usb_rdn    ), 
    .usb_wrn                 (usb_wrn    ),
    .usb_cen                 (usb_cen    ),
    .usb_alen                (1'b0       ),
    .usb_addr                (usb_addr   ),
    .usb_isout               (isout      ), 
    .reg_bytecnt             (reg_bytecnt), 
    .reg_datao               (reg_datao  ), 
    .reg_datai               (reg_datai  ),
    .reg_read                (reg_read   ), 
    .reg_write               (reg_write  )
);


async_fifo #(
    .WIDTH(8),
    .DEPTH(FIFO_BSIZE)
) fifo_i (
    .wr_clk (usb_clk_buf ),
    .wr_rst (wb_rst_i    ),
    .din    (fifo_i_din  ),
    .wr_en  (fifo_i_wr_en),
    .full   (fifo_i_full ),

    .rd_clk (wb_clk_i    ),
    .rd_rst (wb_rst_i    ),
    .dout   (fifo_i_dout ),
    .rd_en  (fifo_i_rd_en),
    .empty  (fifo_i_empty)
);


async_fifo #(
    .WIDTH(8),
    .DEPTH(FIFO_BSIZE)
) fifo_o (
    .wr_clk (wb_clk_i    ),
    .wr_rst (wb_rst_i    ),
    .din    (fifo_o_din  ),
    .wr_en  (fifo_o_wr_en),
    .full   (fifo_o_full ),

    .rd_clk (usb_clk_buf ),
    .rd_rst (wb_rst_i    ),
    .dout   (fifo_o_dout ),
    .rd_en  (fifo_o_rd_en),
    .empty  (fifo_o_empty)
);


endmodule
