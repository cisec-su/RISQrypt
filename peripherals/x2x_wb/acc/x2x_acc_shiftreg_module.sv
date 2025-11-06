module x2x_acc_shiftreg_module
    #(
        parameter N_STAGES = 5, 
        parameter DATA  = 32,
        parameter HALFCYCLE = 1
   )
   (
        input             clk     ,
        input             rst_n     ,
        input conv_mode,    // 0 -> A2B, 1 -> B2A
        input data_type,
        input  [DATA-1:0] data_in ,
        output reg [DATA-1:0] data_out
    );

generate
    if(HALFCYCLE)
    begin
        reg [31:0] sr1_data_in, sr2_data_in; 
        wire [31:0] sr1_data_out, sr2_data_out; 
        
        x2x_acc_shiftreg #(
            .SHIFT ((2 - HALFCYCLE)* (N_STAGES + 1)),
            .DATA (32)
        ) sr1 (
            .clk(clk),
            .rst_n(rst_n),
            .data_in(sr1_data_in),
            .data_out(sr1_data_out)
        );
        
        x2x_acc_shiftreg #(
            .SHIFT ((2 - HALFCYCLE)* (N_STAGES + 1)),
            .DATA (32)
        ) sr2 (
            .clk(clk),
            .rst_n(rst_n),
            .data_in(sr2_data_in),
            .data_out(sr2_data_out)
        );
        
        always @(*)
        begin
            sr1_data_in = 0;
            sr2_data_in = 0;
            data_out = 0;
            case(data_type)
            1'b0:// pow2
            begin
                sr1_data_in = data_in;
                data_out = sr1_data_out;
            end
           
            1'b1://  prime
            begin
                sr1_data_in = data_in;
                sr2_data_in = sr1_data_out;
                data_out = sr2_data_out;
            end
            endcase
        end
        
    
    end
    else
    begin
        reg [31:0] sr1_data_in, sr2_data_in, sr_one_data_in; 
        wire [31:0] sr1_data_out, sr2_data_out, sr_one_data_out; 
        
        x2x_acc_shiftreg #(
            .SHIFT ((2 - HALFCYCLE)* (N_STAGES + 1)),
            .DATA (32)
        ) sr1 (
            .clk(clk),
            .rst_n(rst_n),
            .data_in(sr1_data_in),
            .data_out(sr1_data_out)
        );
        
        x2x_acc_shiftreg #(
            .SHIFT ((2 - HALFCYCLE)* (N_STAGES + 1)),
            .DATA (32)
        ) sr2 (
            .clk(clk),
            .rst_n(rst_n),
            .data_in(sr2_data_in),
            .data_out(sr2_data_out)
        );
        
        x2x_acc_shiftreg #(
            .SHIFT (1),
            .DATA (32)
        ) sr_one (
            .clk(clk),
            .rst_n(rst_n),
            .data_in(sr_one_data_in),
            .data_out(sr_one_data_out)
        );
        
        always @(*)
        begin
            sr1_data_in = 0;
            sr2_data_in = 0;
            sr_one_data_in = 0;
            data_out = 0;
            case({conv_mode,data_type})
            2'b00:// A2B pow2
            begin
                sr1_data_in = data_in;
                data_out = sr1_data_out;
            end
            2'b01:// A2B prime
            begin
                sr1_data_in = data_in;
                sr2_data_in = sr1_data_out;
                data_out = sr2_data_out;
            end
            2'b10:// B2A pow2
            begin
                sr1_data_in = data_in;
                sr_one_data_in = sr1_data_out;
                data_out = sr_one_data_out;
            end
            2'b11:// B2A prime
            begin
                sr1_data_in = data_in;
                sr2_data_in = sr1_data_out;
                sr_one_data_in = sr2_data_out;
                data_out = sr_one_data_out;
            end
            endcase
        end
        
    end
endgenerate
endmodule
