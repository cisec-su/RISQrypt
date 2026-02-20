`timescale 1ns/1ps

module risqrypt_top_tb;

    // Testbench signals
    reg M100_clk_i;
    reg reset_i;

    // Instantiate the DUT (Device Under Test)
    risqrypt_top dut (
        .clk_i(M100_clk_i),
        .reset_i(reset_i),
        .rx_i(),        // Unconnected
        .tx_o()        // Unconnected
//        .led1(), .led2(), .led4() // Ignored outputs
    );

    // Generate the clock signal
    initial begin
        M100_clk_i = 0;
        forever #5 M100_clk_i = ~M100_clk_i; // 10ns clock period (100MHz)
    end

    // Test sequence
    initial begin
        // Apply reset
        reset_i = 0; // Active-high reset
        #50;         // Hold reset for 50ns
        
        reset_i = 1; // Release reset
        #1000;       // Simulate for 1000ns to observe behavior
        
        // $stop;       // End the simulation
    end

endmodule