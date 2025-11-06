module x2x_acc_xor
    
    (
        input  [31:0]            data_in,
        input  [31:0]            rnd_ref,
        output [31:0]       data_out
    );

assign data_out = data_in ^ rnd_ref;


endmodule