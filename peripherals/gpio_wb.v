module gpio_wb #
    (
        parameter BASE_ADDR = 32'h40000000,
        parameter WIDTH     = 8
    )
    (
        // inout wire [WIDTH-1:0]  gpio,
        input                   wb_clk_i  ,         
        input                   wb_rst_i  ,          
        input                   wb_cyc_i  ,         
        input                   wb_stb_i  ,         
        input           [31:0]  wb_adr_i  ,         
        input                   wb_we_i   ,          
        input           [ 3:0]  wb_sel_i  ,
        input           [31:0]  wb_dat_i  ,         
        output reg      [31:0]  wb_dat_o  ,         
        output reg              wb_ack_o  ,         
        output                  wb_stall_o, 
        output                  wb_err_o  ,
        
        output     [WIDTH-1:0]  gpio
    );

localparam INPUT_ADDR      = 8'h00;
localparam OUTPUT_ADDR     = 8'h04;
localparam DIRECTION_ADDR  = 8'h08;


wire [WIDTH-1:0 ] input_mx;
reg  [WIDTH-1:0 ] output_q;
reg  [WIDTH-1:0 ] direction_q;
wire valid;


for (genvar i = 0; i < WIDTH; i = i + 1) begin
    // assign gpio[i]      = direction_q[i] ? output_q[i] : 1'bz;
    // assign input_mx[i] = direction_q[i] ? 1'b0 : gpio[i];
    assign gpio[i]     = output_q[i];
    assign input_mx[i] = 1'b0;
end


assign valid = wb_cyc_i && wb_stb_i;
assign wb_stall_o = 1'b0;
assign wb_err_o   = 1'b0;

always @(posedge wb_clk_i or posedge wb_rst_i) begin
    if (wb_rst_i) begin
        direction_q <= 0;
        output_q <= 0;
    end
    else if (valid && wb_we_i) begin
        case (wb_adr_i - BASE_ADDR)
            OUTPUT_ADDR    : output_q    <= wb_dat_i [WIDTH-1:0];
            DIRECTION_ADDR : direction_q <= wb_dat_i [WIDTH-1:0];
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
            INPUT_ADDR      : wb_dat_o <= input_mx;
            OUTPUT_ADDR     : wb_dat_o <= output_q;
            DIRECTION_ADDR  : wb_dat_o <= direction_q;
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
