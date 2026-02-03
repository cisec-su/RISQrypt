module x2x_acc_dma
    (
        input              clk        ,
        input              rst_n      ,
        input      [31:0]  addr       ,
        input              re         ,
        input              we         ,
        output reg [31:0]  i_data     ,
        output reg         i_valid    ,
        output             i_ready    ,
        output             o_ready    ,
        input      [31:0]  o_data     ,
        // dma connections
        output reg         dma_cyc_i  ,
        output reg         dma_stb_i  ,
        output reg         dma_we_i   ,
        output reg [31:0]  dma_adr_i  ,
        output reg [31:0]  dma_dat_i  ,
        output reg [ 3:0]  dma_sel_i  ,
        input              dma_stall_o,
        input              dma_ack_o  ,
        input      [31:0]  dma_dat_o  ,
        input              dma_err_o  ,
        output reg         dma_rst_i
    );


assign i_ready = 1;
assign o_ready = 1;


(* dont_touch *) reg        re_q    ;
(* dont_touch *) reg        we_q    ;
(* dont_touch *) reg [31:0] addr_q  ;
(* dont_touch *) reg [31:0] o_data_q;



always @ (*)
begin
    dma_cyc_i = 0;
    dma_stb_i = 0;
    dma_we_i = 0;
    dma_adr_i = 0;
    dma_dat_i = 0;
    dma_sel_i = 0; 
    
    dma_rst_i = ~rst_n;

    if(re)
    begin
        dma_cyc_i = 1;
        dma_stb_i = 1;
        dma_we_i = 0;
        dma_adr_i = addr;
        dma_dat_i = 0;
        dma_sel_i = 4'hf; 
    end
    else if(we_q)
    begin
        dma_cyc_i = 1;
        dma_stb_i = 1;
        dma_we_i = 1;
        dma_adr_i = addr_q;
        dma_dat_i = o_data_q;
        dma_sel_i = 4'hf; 
    end
end


always @(posedge clk) begin
    if (!rst_n) begin
        we_q     <= 1'b0;
        addr_q   <= 32'd0;
        o_data_q <= 32'd0;
    end
    else begin
        we_q     <= we;
        addr_q   <= addr;
        o_data_q <= (we) ? o_data : 32'd0;
    end
end


always @ (posedge clk)
begin
    if(!rst_n) begin
        i_valid <= 0;
        re_q    <= 0;
        i_data  <= 32'd0;
    end
    else begin
        re_q    <= re;
        i_valid <= re_q;  
        if (re_q) begin
            i_data <= dma_dat_o;
        end
        else begin
            i_data <= 32'd0;
        end   
    end
end

  

endmodule