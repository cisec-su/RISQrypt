module x2x_acc_wb
    (
        // wb<->ctrl
        output     [31:0]  addr        ,
        output     [31:0]  wdata       ,
        output             we          ,
        output             re          ,
        input      [31:0]  rdata       ,
        // wb<->top
        output             clk         ,
        output             rst_n       ,
        // wishbone connections
        input              wb_cyc_i,
        input              wb_stb_i,
        input              wb_we_i,
        input      [31:0]  wb_adr_i,
        input      [31:0]  wb_dat_i,
        input      [ 3:0]  wb_sel_i,
        output reg         wb_stall_o,
        output reg         wb_ack_o,
        output     [31:0]  wb_dat_o,
        output reg         wb_err_o,
        input              wb_rst_i,
        input              wb_clk_i
    );



// Internal signals
wire re_int;
wire we_int;
wire valid_access;

assign clk = wb_clk_i;
assign rst_n = ~wb_rst_i;

assign valid_access = wb_sel_i == 4'b1111;
assign re_int = wb_cyc_i && wb_stb_i && !wb_we_i && valid_access;
assign we_int = wb_cyc_i && wb_stb_i &&  wb_we_i && valid_access;


assign addr  = wb_adr_i;
assign we    = wb_cyc_i && wb_stb_i &&  wb_we_i;
assign re    = wb_cyc_i && wb_stb_i && !wb_we_i;
assign wdata = wb_dat_i;

assign wb_dat_o = rdata;

// Wishbone protocol handling
always @(posedge wb_clk_i/* or negedge rst_n*/) begin
    if (!rst_n) begin
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


endmodule