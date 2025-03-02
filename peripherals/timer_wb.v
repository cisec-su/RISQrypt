module timer_wb
   #(
        parameter BASE_ADDR = 32'h40001000
    )
    (
        input             wb_cyc_i,
        input             wb_stb_i,
        input             wb_we_i,
        input      [31:0] wb_adr_i,
        input      [31:0] wb_dat_i,
        input      [ 3:0] wb_sel_i,
        output reg        wb_stall_o,
        output reg        wb_ack_o,
        output reg [31:0] wb_dat_o,
        output reg        wb_err_o,
        input             wb_rst_i,
        input             wb_clk_i
    );

localparam CTRL_ADDR      = 8'h00;
localparam DATA_ADDR      = 8'h04;

localparam TIMER_START_BIT = 0;
localparam TIMER_RESET_BIT = 1;


wire clk, rst;


reg start, fw_rst;
reg [31:0] timer;
wire we_int;

assign clk = wb_clk_i;
assign rst = wb_rst_i;

assign valid_access = wb_sel_i == 4'b1111;
assign re_int = wb_cyc_i && wb_stb_i && !wb_we_i && valid_access;
assign we_int = wb_cyc_i && wb_stb_i &&  wb_we_i && valid_access;

//input registers
always @(posedge clk or posedge rst) begin
  if (rst) begin
      wb_ack_o   <= 1'b0;
      wb_err_o   <= 1'b0;
      wb_stall_o <= 1'b0;
  end else begin
      wb_stall_o <= 1'b0;

      if (re_int) begin
          wb_ack_o <= 1'b1;
          wb_err_o   <= 1'b0;
      end
      else if (we_int) begin
          wb_ack_o <= 1'b1;
          wb_err_o   <= 1'b0;
      end
      else if (!valid_access && (wb_cyc_i && wb_stb_i)) begin
          wb_ack_o <= 1'b1;
          wb_err_o <= 1'b1;
      end
      else begin
          wb_ack_o <= 1'b0;
          wb_err_o   <= 1'b0;
      end
  end
end


always @(posedge clk) begin
    if (rst) begin
        start <= 1'b0;
    end else if (we_int && ((wb_adr_i - BASE_ADDR) == CTRL_ADDR)) begin
        start <= wb_dat_i[TIMER_START_BIT];
    end
    else if (fw_rst) begin
      start <= 1'b0;
    end
end


always @(posedge clk) begin
    if (rst) begin
        fw_rst <= 1'b0;
    end else if (we_int && ((wb_adr_i - BASE_ADDR) == CTRL_ADDR)) begin
        fw_rst <= wb_dat_i[TIMER_RESET_BIT];
    end
    else if (fw_rst) begin
      fw_rst <= 1'b0;
    end
end


always @(posedge clk) begin
    if (rst) begin
        timer <= 32'h0;
    end else if (start) begin
        timer <= timer + 32'd1;
    end else if (fw_rst) begin
        timer <= 32'h0;
    end
end


// (Read)
always @(posedge clk) begin
    if (rst) begin
        wb_dat_o <= 32'd0;
    end else begin
        case (wb_adr_i - BASE_ADDR)
            CTRL_ADDR: begin
                wb_dat_o[TIMER_START_BIT] <= start;
                wb_dat_o[TIMER_RESET_BIT] <= fw_rst;
            end
            DATA_ADDR   : wb_dat_o <= timer;
            default     : wb_dat_o <= 32'd0;
        endcase
    end
end


endmodule