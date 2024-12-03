`timescale 1ns/1ps

module ntt_acc_wb
                    (
input         wb_cyc_i,
input         wb_stb_i,
input         wb_we_i,
input [31:0]  wb_adr_i,
input [31:0]  wb_dat_i,
input [3:0]   wb_sel_i,
output        wb_stall_o,
output        wb_ack_o,
output reg [31:0] wb_dat_o,
output        wb_err_o,
input         wb_rst_i,
input         wb_clk_i,


output reg        DMA_cyc_i,
output reg         DMA_stb_i,
output reg         DMA_we_i,
output reg [31:0]  DMA_adr_i,
output reg [31:0]  DMA_dat_i,
output reg [3:0]   DMA_sel_i,
input          DMA_stall_o,
input          DMA_ack_o,
input  [31:0]  DMA_dat_o,
input          DMA_err_o,
output reg         DMA_rst_i);


parameter HP = 5;
parameter FP = (2*HP);
parameter B = 32;
parameter W = 64;
parameter N = 64;
parameter L = 11;

parameter [31:0] ADDR_LOAD_Q =          32'h00040020;
parameter [31:0] ADDR_LOAD_TWIDDLE =    32'h00040024;
parameter [31:0] ADDR_LOAD_POLY =       32'h00040028;
parameter [31:0] ADDR_RST =             32'h0004002C;
parameter [31:0] ADDR_LOAD_CMD =        32'h00040030;
parameter [31:0] ADDR_CHECK_STATUS =    32'h00040034;
parameter [31:0] ADDR_READ_RESULT =     32'h00040038;
parameter [31:0] ADDR_LOAD_MODE =       32'h0004003C;

wire clk, rst;
assign clk = wb_clk_i;
assign rst = ~wb_rst_i;

reg stb, we;
reg [3:0] sel;
reg [31:0] adr,dat;

assign wb_err_o = 1'b0;
assign wb_stall_o = 1'b0;
assign wb_ack_o = stb & wb_cyc_i;

reg [2:0] cmd;
reg [1:0] mode;
reg [31:0] i_data;
reg i_valid;
reg status;

wire busy;
wire o_valid;
wire [31:0] o_data;

small_ntt #(
    .N(N),
    .W(W),
    .B(B),
    .L(L)
) small_ntt_inst (
    .clk(clk),
    .rst(~rst),
    .cmd(cmd),
    .i_data(i_data),
    .i_valid(i_valid),
    .o_data(o_data),
    .o_valid(o_valid),
    .busy(busy),
    .mode(mode)
);

//WB input registers
always @(posedge clk or negedge rst)
begin
    if(!rst)
    begin
        {stb,we,sel,adr,dat} <= 70'b0;
    end
    else
    begin
        stb <= wb_stb_i;
        we <= wb_we_i;
        sel <= wb_sel_i;
        adr <= wb_adr_i;
        dat <= wb_dat_i;
    end
end

reg [31:0] q_addr;
reg [31:0] psi_addr;
reg [31:0] poly_addr;
reg [2:0] addr_flag;

reg [31:0] cstate, nstate, pstate, cnt, cnt2;

reg cntup,cntres;

always @(posedge clk or negedge rst)
begin
    if(!rst)
    begin
        q_addr <= 0;
        psi_addr <= 0;
        poly_addr <= 0;
        addr_flag <= 0;
        cstate <= 0;
        pstate <= 0;
        cnt <= 0;
        cnt2 <= 0;
        status <= 0;
    end
    else
    begin
        cstate <= nstate;
        pstate <= cstate;
        
        if(cntup)
            cnt <= cnt + 1;
        else if(cntres)
            cnt <= 0;
            
        if(wb_cyc_i && stb && we)
        begin
            status <= 0;
            case(adr)
                ADDR_LOAD_Q: 
                begin
                    q_addr <= dat;
                    addr_flag[0] <= 1'b1;
                end
                ADDR_LOAD_TWIDDLE: 
                begin
                    psi_addr <= dat;
                    addr_flag[1] <= 1'b1;
                end
                ADDR_LOAD_POLY: 
                begin
                    poly_addr <= dat;
                    addr_flag[2] <= 1'b1;
                end
            endcase
        end
        
        
        if((cstate == ST_Q) && (cnt == 1))
            addr_flag[0] <= 1'b0;
        if((cstate == ST_PSI) && (cnt == (2*N-1)))
            addr_flag[1] <= 1'b0;
        if((cstate == ST_POLY) && (cnt == (2*N-1)))
            addr_flag[2] <= 1'b0;
        if((cstate == ST_READY) && !o_valid)
            status <= 1;
    end
end

parameter [31:0] ST_IDLE = 0;
parameter [31:0] ST_Q = 1;
parameter [31:0] ST_PSI = 2;
parameter [31:0] ST_POLY = 3;
parameter [31:0] ST_RUN = 4;
parameter [31:0] ST_READY = 5;

always @(*)
begin
    cntup = 0;
    cntres = 0;
    nstate = cstate;
    
    case(cstate)
        ST_IDLE:
        begin
            if(addr_flag[0])
                nstate = ST_Q;
            else if (addr_flag[1])    
                nstate = ST_PSI;
            else if (addr_flag[2])    
                nstate = ST_POLY;   
            else 
                nstate = ST_IDLE;
        end
        ST_Q:
        begin
            if(cnt < 1)
            begin
                nstate = ST_Q;
                cntup = ~DMA_stall_o;
            end 
            else
            begin
                nstate = ST_IDLE;
                cntres = 1;
            end 
        end
        ST_PSI:
        begin
            if(cnt < (2*N)-1)
            begin
                nstate = ST_PSI;
                cntup = ~DMA_stall_o;
            end 
            else
            begin
                nstate = ST_IDLE;
                cntres = 1;
            end 
        end
        ST_POLY:
        begin
            if(cnt < (2*N)-1)
            begin
                nstate = ST_POLY;
                cntup = ~DMA_stall_o;
            end 
            else
            begin
                nstate = ST_RUN;
                cntres = 1;
            end 
        end
        ST_RUN:
        begin
            if(!o_valid)
                nstate = ST_RUN;
            else
                nstate = ST_READY;
        end
        ST_READY:
        begin
            if(cnt < (2*N))
            begin
                nstate = ST_READY;
                cntup = ~DMA_stall_o;
            end 
            else
            begin
                nstate = ST_IDLE;
                cntres = 1;
            end
        end
    endcase
end

always @ (*)
begin
    case(cstate)
    ST_IDLE:
    begin
        DMA_cyc_i=0;
        DMA_stb_i=0;
        DMA_we_i=0;
        DMA_adr_i=0;
        DMA_dat_i=0;
        DMA_sel_i=0;
        DMA_rst_i=0;
    end
    ST_Q:
    begin
        DMA_cyc_i=1;
        DMA_stb_i=1;
        DMA_we_i=0;
        DMA_adr_i= q_addr + (cnt<<2);
        DMA_dat_i=0;
        DMA_sel_i=0;
        DMA_rst_i=0;
    end
    ST_PSI:
    begin
        DMA_cyc_i=1;
        DMA_stb_i=1;
        DMA_we_i=0;
        DMA_adr_i= psi_addr + (cnt<<2);
        DMA_dat_i=0;
        DMA_sel_i=0;
        DMA_rst_i=0;
    end
    ST_POLY:
    begin
        DMA_cyc_i=1;
        DMA_stb_i=1;
        DMA_we_i=0;
        DMA_adr_i= poly_addr + (cnt<<2);
        DMA_dat_i=0;
        DMA_sel_i=0;
        DMA_rst_i=0;
    end
    ST_RUN:
    begin
        DMA_cyc_i=0;
        DMA_stb_i=0;
        DMA_we_i=0;
        DMA_adr_i= 0;
        DMA_dat_i=0;
        DMA_sel_i=0;
        DMA_rst_i=0;
    end
    ST_READY:
    begin
        DMA_cyc_i=1;
        DMA_stb_i=1;
        DMA_we_i=1;
        DMA_adr_i= poly_addr + (cnt<<2);
        DMA_dat_i= o_data;
        DMA_sel_i=4'hf;
        DMA_rst_i=0;
    end
    endcase
    
end
reg DMA_cyc_i_prev;
reg DMA_stb_i_prev;
reg DMA_we_i_prev;
always @ (posedge clk or negedge rst)
begin
    if(!rst)
    begin
        DMA_cyc_i_prev <= 0;
        DMA_stb_i_prev <= 0;
        DMA_we_i_prev <= 0;
    end
    else
    begin
        DMA_cyc_i_prev <= DMA_cyc_i;
        DMA_stb_i_prev <= DMA_stb_i;
        DMA_we_i_prev <= DMA_we_i;
    end
end

always @ (posedge clk or negedge rst)
begin
    if(!rst)
    begin
        cmd <= 3'b0;
        mode <= 2'd0;
        
    end
    else if (DMA_cyc_i_prev && DMA_stb_i_prev && !DMA_we_i_prev)
    begin
        case(pstate)
            ST_Q:
            begin
                cmd <= 3'd1;
                i_valid <= 1'b1;
                i_data <= DMA_dat_o;
               
            end
            ST_PSI:
            begin
                cmd <= 3'd2;
                i_valid <= 1'b1;
                i_data <= DMA_dat_o;
                
            end
            ST_POLY:
            begin
                cmd <= 3'd3;
                i_valid <= 1'b1;
                i_data <= DMA_dat_o;
                
            end
        endcase
    
    end
    
    else if((cstate == ST_RUN) && o_valid)
    begin
        cmd <= 3'd5;
        
    end    
    else if((cstate == ST_READY) && !o_valid)
    begin
        cmd <= 3'd0;
        
    end
    //else
    //begin
        //cmd <= 0;
        //i_valid <= 1'b0;
        //i_data <= 0;
    //end
end

always @ (*)
begin
    if(adr == ADDR_CHECK_STATUS)
        wb_dat_o = {31'd0,status};
    else if(adr == ADDR_READ_RESULT)
        wb_dat_o = o_data;
    else 
        wb_dat_o = 0;
end


endmodule
