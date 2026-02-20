module x2x_acc_lfsr_tb;



// Parameters
localparam DATA_WIDTH = 240;


localparam CLOCK_PERIOD = 10;

// Signals
reg clk;
reg rst_n;
reg load;

wire [DATA_WIDTH-1:0] data_out;


// DUT instantiation
x2x_acc_lfsr #(
    .DATA_WIDTH(DATA_WIDTH)
) dut (
    .clk(clk),
    .rst_n(rst_n),
    .load(load),
    .key (64'h100000001),
    .iv({DATA_WIDTH{1'b0}} ^ 2),
    .stream_out(data_out)
);

// Clock generation
initial begin
    clk = 1;
    forever #(CLOCK_PERIOD/2) clk = ~clk;
end


// Reset generation
initial begin
    rst_n = 1;
    #(CLOCK_PERIOD*4) rst_n = 0;

    load = 0;

    #(CLOCK_PERIOD*4) rst_n = 1;
    
    #0.1
    
    #(CLOCK_PERIOD*16);

    load = 1;

    #(CLOCK_PERIOD);
    
    load = 0;

end





endmodule;