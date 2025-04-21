module x2x_acc_ctrl 
   #(
        parameter SHARES    = 2           ,
        parameter LOGL      = 10          ,
        parameter BASE_ADDR = 32'h1004_0050
    )
    (    
        input                    clk                   ,
        input                    rst_n                 ,    // Active-low reset

        input      [     31:0]   addr                  ,     // Address input
        input      [     31:0]   wdata                 ,     // Write data
        input                    we                    ,     // Write enable
        input                    re                    ,     // Read enable
        output reg [      31:0]  rdata                 ,     // Read data      
        // ctrl reg fields                    
        output reg               start                 ,     // (Write/Self-Clear)
        output reg               conv_mode             ,     // (Write)
        output reg               data_type             ,     // (Read/Write)
        output reg               dual_mode             ,     // (Read/Write)
        output reg               mask_mode             ,     // (Write)
        
        // data address registers
        output reg [      31:0]  din_addr  [0:SHARES-1],     // (Write)
        output reg [      31:0]  dout_addr [0:SHARES-1],     // (Write)
        output reg [  LOGL-1:0]  data_len              ,     // (Write)
        output reg [      63:0]  seed                  ,     // (Write)
        output reg               start_RNG             ,     // (Write/Self-Clear)

        input                    busy                  ,     // Busy status input
        input                    done                        // Done status input
    );


// Register relative addresses
localparam CTRL_ADDR           = 12'h0000;   // Offset for ctrl register
localparam DATA_LEN_ADDR       = 12'h0004;   // Offset for data_len register
localparam SEED_ADDR_LOW       = 12'h0008;   // Offset for seed low bits register
localparam SEED_ADDR_HIGH      = 12'h000C;   // Offset for seed low bits register
localparam MODULUS_ADDR        = 12'h0010;   // Offset to load modulus
localparam DIN_PTR_ADDR_START  = 12'h0014;   // Offset for first share din_addr register
localparam DIN_PTR_ADDR_END    = DIN_PTR_ADDR_START + ((SHARES - 1) << 2); // Offset for last share din_addr register
localparam DOUT_PTR_ADDR_START = 12'h0034;   // Offset for first share dout_addr register
localparam DOUT_PTR_ADDR_END   = DOUT_PTR_ADDR_START + ((SHARES - 1) << 2);      // Offset for last share dout_addr register

// Bit-fields for ctrl register
localparam CTRL_START_BIT     = 0;
localparam CTRL_RESET_BIT     = 1;
localparam CTRL_CONV_MODE_BIT = 2;
localparam CTRL_DATA_TYPE_BIT = 3;   //readback
localparam CTRL_DUAL_MODE_BIT = 4;   //readback
localparam CTRL_MASK_DATA_BIT = 5; 
localparam CTRL_WORD_SIZE_BIT = 6; // 0-->32-bit // 1--> 2*16 //readback
localparam CTRL_BUSY_BIT     = 30;
localparam CTRL_DONE_BIT     = 31;

wire [31:0] addr_offset;
reg done_q;

reg [31:0] modulus;


assign addr_offset = addr - BASE_ADDR;


// DONE flag (Clear on Read)
always @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin
        done_q <= 1'b0;
    end else if (done) begin
        done_q <= 1'b1;
    end else if (re && (addr_offset == CTRL_ADDR) && done_q) begin
        done_q <= 1'b0;
    end
end

// START (Self-Clear)
always @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin
        start <= 1'd0;
    end
    else if (we && (addr_offset == CTRL_ADDR)) begin
        start <= wdata[CTRL_START_BIT];
    end
    else begin
        start <= 1'd0;
    end
end

// CONV_MODE
always @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin
        conv_mode <= 1'd0;
    end
    else if (we && (addr_offset == CTRL_ADDR)) begin
        conv_mode <= wdata[CTRL_CONV_MODE_BIT];
    end
    
end


// DATA_TYPE
always @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin
        data_type <= 1'd0;
    end
    else if (we && (addr_offset == CTRL_ADDR)) begin
        data_type <= wdata[CTRL_DATA_TYPE_BIT];
    end
    
end


// DUAL_MODE
always @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin
        dual_mode <= 1'd0;
    end
    else if (we && (addr_offset == CTRL_ADDR)) begin
        dual_mode <= wdata[CTRL_DUAL_MODE_BIT];
    end 
end


always @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin
        mask_mode <= 1'd0;
    end
    else if (we && (addr_offset == CTRL_ADDR)) begin
        mask_mode <= wdata[CTRL_MASK_DATA_BIT];
    end
end


generate
    for (genvar i = 0; i < SHARES; i = i + 1) begin : SHARE_GEN
        // (Write)
        always @(posedge clk or negedge rst_n) begin
            if (!rst_n) begin
                din_addr[i] <= 32'd0;
                dout_addr[i] <= 32'd0;
            end else if (we) begin
                if (addr_offset == (DIN_PTR_ADDR_START + (i << 2))) begin
                    din_addr[i] <= wdata;
                end else if (addr_offset == (DOUT_PTR_ADDR_START + (i << 2))) begin
                    dout_addr[i] <= wdata;
                end
            end
        end
    end
endgenerate


always @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin
        data_len <= 32'd0;
    end else if (we) begin
        if (addr_offset == DATA_LEN_ADDR) begin
            data_len <= wdata;
        end
    end
end

always @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin
        seed <= 64'd0;
    end else if (we) begin
        if (addr_offset == SEED_ADDR_LOW) begin
            seed[31:0] <= wdata;
        end
        else if (addr_offset == SEED_ADDR_HIGH) begin
            seed[63:32] <= wdata;
        end
    end
end

// START RNG (Self-Clear)
always @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin
        start_RNG <= 1'd0;
    end
    else if (we && (addr_offset == SEED_ADDR_H)) begin
        start_RNG <= 1;
    end
    else begin
        start_RNG <= 1'd0;
    end
end


always @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin
        modulus <= 32'd0;
    end else if (we) begin
        if (addr_offset == MODULUS_ADDR) begin
            modulus <= wdata;
        end
    end
end

// (Read)
always @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin
        rdata <= 32'd0;
    end else if (re) begin
        if (addr_offset <= DIN_PTR_ADDR_END && addr_offset >= DIN_PTR_ADDR_START) begin
            rdata <= 32'd0;
        end else if (addr_offset <= DOUT_PTR_ADDR_END && addr_offset >= DOUT_PTR_ADDR_START) begin
            rdata <= 32'd0;
        end else begin
            case (addr_offset)
                CTRL_ADDR: begin
                    rdata[CTRL_DATA_TYPE_BIT]             <= data_type;
                    rdata[CTRL_DUAL_MODE_BIT]             <= dual_mode;
                    rdata[CTRL_BUSY_BIT     ]             <= busy;
                    rdata[CTRL_DONE_BIT     ]             <= done_q | done;

                end
                DATA_LEN_ADDR:    rdata <= data_len;
                MODULUS_ADDR :    rdata <= modulus;
                default:          rdata <= 32'd0;
            endcase
        end
    end
end
endmodule
