module x2x_su_tb;



// Parameters
localparam HALFCYCLE = 1;
localparam PARAM_WIDTH = 32;
localparam BOX_WIDTH = 16;
localparam N_SHARES = 2;
localparam N_SHARES_2SHARE = 2;
localparam N_STAGES = 5;

localparam B2A_RND_SHARES_2SHARE = N_SHARES_2SHARE - 1;
localparam EXPAND_SHARES_2SHARE = N_SHARES_2SHARE;
localparam TRIANGLE_SHARES_2SHARE = 2 * (N_SHARES_2SHARE * (N_SHARES_2SHARE - 1) / 2);
localparam BOX_SHARES_2SHARE = (N_STAGES - 1) * 3 * (N_SHARES_2SHARE * (N_SHARES_2SHARE - 1) / 2) + 2 * (N_SHARES_2SHARE * (N_SHARES_2SHARE - 1) / 2);

localparam RND_SHARES_2SHARE = 2 * B2A_RND_SHARES_2SHARE + 2 * EXPAND_SHARES_2SHARE + 2 * TRIANGLE_SHARES_2SHARE;
localparam RND_SHARES_2SHARE_BOX = 2 * BOX_SHARES_2SHARE;

localparam RND_SHARES = RND_SHARES_2SHARE;
localparam RND_SHARES_BOX = RND_SHARES_2SHARE_BOX;

localparam CLOCK_PERIOD = 10;

// Signals
reg clk;
reg rst_n;
reg valid_data;
wire ready_data;
reg ready_result;
reg valid_result;
reg conversion_mode;
reg data_type_mode;
reg dual_mode;
reg [PARAM_WIDTH     : 0] modulus;
reg [PARAM_WIDTH - 1 : 0] modulus_twoc;
reg [PARAM_WIDTH - 1 : 0] fresh_rnd_shares [RND_SHARES - 1 : 0];
reg [BOX_WIDTH - 1 : 0] fresh_rnd_shares_8bit [RND_SHARES_BOX - 1 : 0];
reg [PARAM_WIDTH - 1 : 0] original_data [2 - 1 : 0][N_SHARES - 1:0];
reg [PARAM_WIDTH - 1 : 0] converted_data [2 - 1 : 0][N_SHARES - 1:0];


integer i, j;


localparam LAT = 9;


// DUT instantiation
X2X_32b_2SHARE_HALFCYCLE_STREAM #(
    .HALFCYCLE(HALFCYCLE),
    .PARAM_WIDTH(PARAM_WIDTH),
    .BOX_WIDTH(BOX_WIDTH),
    .N_SHARES(N_SHARES),
    .RND_SHARES(RND_SHARES),
    .RND_SHARES_BOX(RND_SHARES_BOX)
) dut (
    .clk(clk),
    .rst_n(rst_n),
    .valid_data(valid_data),
    .ready_data(ready_data),
    .ready_result(ready_result),
    .valid_result(valid_result),
    .conversion_mode(conversion_mode),
    .data_type_mode(data_type_mode),
    .dual_mode(dual_mode),
    .modulus(modulus),
    .modulus_twoc(modulus_twoc),
    .fresh_rnd_shares(fresh_rnd_shares),
    .fresh_rnd_shares_8bit(fresh_rnd_shares_8bit),
    .original_data(original_data),
    .converted_data(converted_data)
);

// Clock generation
initial begin
    clk = 1;
    forever #(CLOCK_PERIOD/2) clk = ~clk;
end

reg [31:0] data_unmasked;
reg signed [32:0] sum_signed;     // one extra bit


// Reset generation
initial begin
    rst_n = 1;
    #(CLOCK_PERIOD*4) rst_n = 0;

    valid_data = 1'b0;
    conversion_mode = 1'b1; // 0 -> A2B, 1 -> B2A
    dual_mode = 1'b0;
    data_type_mode = 1'b0; // 0 -> power-of-two, 1 -> prime
    modulus = 33'h010000000; // dilithium's prime
    modulus_twoc = (32'hffffffff ^ modulus) + 1; // (modulus + 1
    ready_result = 1'b1;

    #(CLOCK_PERIOD*4) rst_n = 1;
    #1;
    #(CLOCK_PERIOD/2);

    for (j = 0; j < 1; j++) begin
        #CLOCK_PERIOD;

        for (i = 0; i < RND_SHARES; i = i + 1) begin
            if (i != 4)
                fresh_rnd_shares[i] = $urandom();
            else //if (i != 5)
                fresh_rnd_shares[i] = $urandom() % modulus;
        end


        for (i = 0; i < RND_SHARES_BOX; i = i + 1) begin
            fresh_rnd_shares_8bit[i] = $urandom_range(0, 2**BOX_WIDTH - 1);
        end 


        valid_data = 1'b1;
        original_data[0][0] = $urandom() % modulus;//8380100;//32'hffffffff;
        original_data[0][1] = $urandom() % modulus;//10 + modulus_twoc;//32'hffffffff;
        $display("A Share 1 Before: %0x, %d", original_data[0][1], original_data[0][1]);
        //if (original_data[0][1] > (modulus >> 1))
//           original_data[0][1] = original_data[0][1] + modulus_twoc;
        //if (original_data[0][0] > (modulus >> 1))
        //    original_data[0][0] = original_data[0][0] - modulus;
        sum_signed = $signed({original_data[0][0]}) + $signed(original_data[0][1]);

        if (sum_signed < 0)
            data_unmasked = sum_signed + modulus;
        else if (sum_signed >= modulus)
            data_unmasked = sum_signed - modulus;
        else 
            data_unmasked = sum_signed;

        //data_unmasked = ($signed({1'b0,original_data[0][0]}) + $signed(original_data[0][1])) % {1'b0,modulus};
        $display("A Share 0: %0x, %d", original_data[0][0], original_data[0][0]);
        $display("A Share 1: %0x, %d", original_data[0][1], original_data[0][1]);
        $display("Unmasked: %0d, hex: %0x", data_unmasked, data_unmasked);
        //$stop;
        #(CLOCK_PERIOD);
        original_data[0][0] = 0;//$urandom() % modulus;//8380100;//32'hffffffff;
        original_data[0][1] = 0;//$urandom() % modulus;//10 + modulus_twoc;//32'hffffffff;

        valid_data = 1'b0;
        fresh_rnd_shares[4] = 0;
        for (i = 0; i < RND_SHARES; i = i + 1) begin
            if (i != 4)
                fresh_rnd_shares[i] = 0;
            else //if (i != 5)
                fresh_rnd_shares[i] = 0;
        end


        for (i = 0; i < RND_SHARES_BOX; i = i + 1) begin
            fresh_rnd_shares_8bit[i] = 0;
        end 

        while (!valid_result) begin
            #CLOCK_PERIOD;
        end

        $display("Converted Data Shares:");
        $display("Share 0: %0x", converted_data[0][0]);
        $display("Share 1: %0x", converted_data[0][1]);
        $display("Unmasked: %0d, hex: %0x", (converted_data[0][0] ^ converted_data[0][1]), (converted_data[0][0] ^ converted_data[0][1]));
        if (((converted_data[0][0] ^ converted_data[0][1])) != data_unmasked) begin
            $display("ERROR");
            $stop;
        end
    end
    $stop;

end





endmodule;