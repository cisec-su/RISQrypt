module x2x_acc_lfsr
    #(
        parameter LFSR_WIDTH = 64,
        parameter IV_WIDTH   = 16,
        parameter LFSR_POLY  = 64'h1b, // 2^64 + 2^4 + 2^3 + 2^1 + 1
        parameter DATA_WIDTH = 256
    )
    (
        input                   clk       ,
        input                   rst_n     ,
        input                   load      ,
        input  [LFSR_WIDTH-1:0] key       ,
        input  [  IV_WIDTH-1:0] iv        ,
        output [DATA_WIDTH-1:0] stream_out
    );

localparam NUM_LFSR = ((DATA_WIDTH + LFSR_WIDTH - 1) / LFSR_WIDTH);
localparam LOG_LFSR = $clog2(NUM_LFSR);

reg  [LFSR_WIDTH-1:0] state_in [NUM_LFSR-1:0];
wire [LFSR_WIDTH-1:0] state_out[NUM_LFSR-1:0];
wire [LFSR_WIDTH-1:0] data_in  [NUM_LFSR-1:0];


reg load_q;


always @(posedge clk) begin
    load_q <= load;
end


for (genvar i = 0; i < NUM_LFSR; i++) begin
    localparam S0 = i*LFSR_WIDTH;
    localparam S1 = (i == (NUM_LFSR - 1)) ? DATA_WIDTH - 1 : (i + 1)*LFSR_WIDTH - 1;


    lfsr #(
        .LFSR_WIDTH       (LFSR_WIDTH       ),
        .LFSR_POLY        (LFSR_POLY        ),
        .LFSR_CONFIG      ("FIBONACCI"      ),
        .LFSR_FEED_FORWARD(0                ),
        .REVERSE          (0                ),
        .DATA_WIDTH       (S1 - S0 + 1      ),
        .STYLE            ("AUTO"           )
    ) lfsr_inst (
        .data_in  (data_in   [i][S1-S0:0]),
        .state_in (state_in  [i]         ),
        .data_out (stream_out[S1:S0]     ),
        .state_out(state_out [i]         )
    );


    always @(posedge clk) begin
        if (!rst_n) 
            state_in[i] <= 0;
        else if (load)
            state_in[i] <= key;
        else
            state_in[i] <= state_out[i];
    end


    assign data_in[i] = (load_q) ? {iv, i[LOG_LFSR-1:0]} : 0;

end



endmodule