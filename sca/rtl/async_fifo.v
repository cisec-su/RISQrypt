module async_fifo
   #(
        parameter WIDTH = 8,
        parameter DEPTH = 128
    )
    (
        input  wire             wr_clk,
        input  wire             wr_rst,
        input  wire [WIDTH-1:0] din   ,
        input  wire             wr_en ,
        output wire             full  ,

        input  wire             rd_clk,
        input  wire             rd_rst,
        output reg  [WIDTH-1:0] dout  ,
        input  wire             rd_en ,
        output wire             empty
    );

localparam ADDR_WIDTH = $clog2(DEPTH);

// ====================================================================
// Memory
// ====================================================================
reg [WIDTH-1:0] mem [0:DEPTH-1];

// ====================================================================
// Pointers
// ====================================================================
reg [ADDR_WIDTH:0] wr_ptr_bin;
reg [ADDR_WIDTH:0] wr_ptr_gray;
reg [ADDR_WIDTH:0] rd_ptr_bin;
reg [ADDR_WIDTH:0] rd_ptr_gray;

// ====================================================================
// Synchronized pointers
// ====================================================================
reg [ADDR_WIDTH:0] wr_ptr_gray_rdclk_1, wr_ptr_gray_rdclk_2;
reg [ADDR_WIDTH:0] rd_ptr_gray_wrclk_1, rd_ptr_gray_wrclk_2;

// Converted binary versions
wire [ADDR_WIDTH:0] wr_ptr_bin_rdclk;
wire [ADDR_WIDTH:0] rd_ptr_bin_wrclk;

// ====================================================================
// Gray → Binary conversion function
// ====================================================================
function [ADDR_WIDTH:0] gray2bin;
    input [ADDR_WIDTH:0] g;
    integer i;
    begin
        gray2bin[ADDR_WIDTH] = g[ADDR_WIDTH];
        for (i = ADDR_WIDTH-1; i >= 0; i = i-1)
            gray2bin[i] = gray2bin[i+1] ^ g[i];
    end
endfunction

assign wr_ptr_bin_rdclk = gray2bin(wr_ptr_gray_rdclk_2);
assign rd_ptr_bin_wrclk = gray2bin(rd_ptr_gray_wrclk_2);

// ====================================================================
// WRITE SIDE
// ====================================================================
always @(posedge wr_clk or posedge wr_rst) begin
    if (wr_rst) begin
        wr_ptr_bin  <= 0;
        wr_ptr_gray <= 0;
    end else if (wr_en && !full) begin
        mem[wr_ptr_bin[ADDR_WIDTH-1:0]] <= din;
        wr_ptr_bin  <= wr_ptr_bin + 1;
        wr_ptr_gray <= (wr_ptr_bin + 1) ^ ((wr_ptr_bin + 1) >> 1);
    end
end

// ====================================================================
// READ SIDE
// ====================================================================
always @(posedge rd_clk or posedge rd_rst) begin
    if (rd_rst) begin
        rd_ptr_bin  <= 0;
        rd_ptr_gray <= 0;
        dout        <= 0;
    end else if (rd_en && !empty) begin
        dout        <= mem[rd_ptr_bin[ADDR_WIDTH-1:0]];
        rd_ptr_bin  <= rd_ptr_bin + 1;
        rd_ptr_gray <= (rd_ptr_bin + 1) ^ ((rd_ptr_bin + 1) >> 1);
    end
end

// ====================================================================
// CROSS-DOMAIN SYNCHRONIZATION
// ====================================================================
// Sync write pointer into read domain
always @(posedge rd_clk or posedge rd_rst) begin
    if (rd_rst) begin
        wr_ptr_gray_rdclk_1 <= 0;
        wr_ptr_gray_rdclk_2 <= 0;
    end else begin
        wr_ptr_gray_rdclk_1 <= wr_ptr_gray;
        wr_ptr_gray_rdclk_2 <= wr_ptr_gray_rdclk_1;
    end
end

// Sync read pointer into write domain
always @(posedge wr_clk or posedge wr_rst) begin
    if (wr_rst) begin
        rd_ptr_gray_wrclk_1 <= 0;
        rd_ptr_gray_wrclk_2 <= 0;
    end else begin
        rd_ptr_gray_wrclk_1 <= rd_ptr_gray;
        rd_ptr_gray_wrclk_2 <= rd_ptr_gray_wrclk_1;
    end
end

// ====================================================================
// STATUS FLAGS
// ====================================================================
assign full  = ( (wr_ptr_gray[ADDR_WIDTH]     != rd_ptr_gray_wrclk_2[ADDR_WIDTH]) &&
                 (wr_ptr_gray[ADDR_WIDTH-1]   != rd_ptr_gray_wrclk_2[ADDR_WIDTH-1]) &&
                 (wr_ptr_gray[ADDR_WIDTH-2:0] == rd_ptr_gray_wrclk_2[ADDR_WIDTH-2:0]) );

assign empty = (wr_ptr_gray_rdclk_2 == rd_ptr_gray);

endmodule
