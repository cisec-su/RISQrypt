// ============================================================================
// async_reg
// ----------------------------------------------------------------------------
// A simple asynchronous flag register.
// - Set in one clock domain.
// - Cleared in another clock domain (with safe synchronization).
// The flag remains high until cleared.
//
// Usage example:
//   async_reg u_flag (
//       .set_clk   (wb_clk_i),
//       .set_rst   (wb_rst_i),
//       .set_cond  (done_signal),
//       .clr_clk   (usb_clk),
//       .clr_rst   (wb_rst_i),
//       .clr_cond  (usb_read_pulse),
//       .flag_set  (flag_in_wb_clk),
//       .flag_clr  (flag_in_usb_clk)
//   );
// ============================================================================

module async_reg
    (
        input  wire set_clk,
        input  wire set_rst,
        input  wire set_cond,

        input  wire clr_clk,
        input  wire clr_rst,
        input  wire clr_cond,

        output wire flag_set,
        output wire flag_clr
    );

// ========================================================================
// Internal signals
// ========================================================================
reg flag_reg;              // flag in set domain
reg clr_req;               // clear request in clr domain
reg [1:0] clr_sync_set;    // clr_req synchronized into set_clk
reg [1:0] flag_sync_clr;   // flag_reg synchronized into clr_clk

// ========================================================================
// Set-domain logic
// ========================================================================
always @(posedge set_clk or posedge set_rst) begin
    if (set_rst)
        flag_reg <= 1'b0;
    else if (set_cond)
        flag_reg <= 1'b1;
    else if (clr_sync_set[1])
        flag_reg <= 1'b0;
end

// ========================================================================
// Clear-domain logic
// ========================================================================
always @(posedge clr_clk or posedge clr_rst) begin
    if (clr_rst)
        clr_req <= 1'b0;
    else if (clr_cond)
        clr_req <= 1'b1;
    else if (clr_sync_set[1])
        clr_req <= 1'b0;
end

// ========================================================================
// Synchronize clear request into set domain
// ========================================================================
always @(posedge set_clk or posedge set_rst) begin
    if (set_rst)
        clr_sync_set <= 2'b00;
    else
        clr_sync_set <= {clr_sync_set[0], clr_req};
end

// ========================================================================
// Synchronize flag into clear domain
// ========================================================================
always @(posedge clr_clk or posedge clr_rst) begin
    if (clr_rst)
        flag_sync_clr <= 2'b00;
    else
        flag_sync_clr <= {flag_sync_clr[0], flag_reg};
end

// ========================================================================
// Outputs
// ========================================================================
assign flag_set = flag_reg;
assign flag_clr = flag_sync_clr[1];

endmodule