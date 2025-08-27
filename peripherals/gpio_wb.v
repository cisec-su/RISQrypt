module wb_gpio #
    (
        parameter BASE_ADDR = 32'h40000000,
        parameter width = 8
    )
    (
        // inout wire [width-1:0]  gpio,
		output     [width-1:0]  gpio,
        input                   wb_clk_i,         // clock
        input                   wb_rst_i,         // reset 
        input                   wb_cyc_i,         // cycle
        input                   wb_stb_i,         // strobe
        input       [31:0]      wb_adr_i,         // address adr_i[1]
        input                   wb_we_i,          // write enable
        input       [ 3:0]      wb_sel_i,
        input       [31:0]      wb_dat_i,         // data output
        output reg  [31:0]      wb_dat_o,         // data input
        output reg              wb_ack_o,         // normal bus termination
        output                  wb_stall_o,       // stall
        output                  wb_err_o          // error
    );

localparam INPUT_ADDR      = 8'h00; // Offset for input register
localparam OUTPUT_ADDR     = 8'h04; // Offset for output_addr register
localparam DIRECTION_ADDR  = 8'h08; // Offset for direction_addr register


wire [width-1:0 ] input_reg;
reg  [width-1:0 ] output_reg;
reg  [width-1:0 ] direction_reg;
wire valid;


for (genvar i = 0; i < width; i = i + 1) begin
    // assign gpio[i]      = direction_reg[i] ? output_reg[i] : 1'bz;
    // assign input_reg[i] = direction_reg[i] ? 1'b0 : gpio[i];
    assign gpio[i]      = output_reg[i];
    assign input_reg[i] = 1'b0;
end


assign valid = wb_cyc_i && wb_stb_i;
assign wb_stall_o = 1'b0;
assign wb_err_o   = 1'b0;

always @(posedge wb_clk_i or posedge wb_rst_i) begin
    if (wb_rst_i) begin
        direction_reg <= 0;
        output_reg <= 0;
    end
    else if (valid && wb_we_i) begin
        case (wb_adr_i - BASE_ADDR)
            OUTPUT_ADDR    : output_reg    <= wb_dat_i [width-1:0];
            DIRECTION_ADDR : direction_reg <= wb_dat_i [width-1:0];
            default : ;
        endcase
    end
end


always @(posedge wb_clk_i or posedge wb_rst_i) begin
    if (wb_rst_i) begin
        wb_dat_o <= 0;
    end
    else if (valid && (!wb_we_i)) begin
        case (wb_adr_i - BASE_ADDR)
            INPUT_ADDR      : wb_dat_o <= input_reg;
            OUTPUT_ADDR     : wb_dat_o <= output_reg;
            DIRECTION_ADDR  : wb_dat_o <= direction_reg;
            default         : wb_dat_o <= 0;
        endcase
    end
end


always @(posedge wb_clk_i or posedge wb_rst_i) begin
    if (wb_rst_i)
        wb_ack_o <= 1'b0;
    else
        wb_ack_o <= valid & ~ wb_stall_o;
end

endmodule
