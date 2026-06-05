module x2x_acc_tb
   #(
        parameter BASE_ADDR       = 32'h40001100,
        parameter SHARES          = 2           , // do not change
        parameter MEM_SIZE        = 2048        
    )();

`include "x2x_acc.vh"
    
localparam CLOCK_PERIOD = 10;



// Clock and reset signals
reg clk;
reg rst_n;

// Wishbone signals
reg wb_cyc_i;
reg wb_stb_i;
reg wb_we_i;
reg [31:0] wb_adr_i;
reg [31:0] wb_dat_i;
reg [3:0] wb_sel_i;
wire wb_stall_o;
wire wb_ack_o;
wire [31:0] wb_dat_o;
wire wb_err_o;
reg wb_rst_i;

// DMA signals
wire        dma_cyc_i  [SHARES-1:0];
wire        dma_stb_i  [SHARES-1:0];
wire        dma_we_i   [SHARES-1:0];
wire [31:0] dma_adr_i  [SHARES-1:0];
wire [31:0] dma_dat_i  [SHARES-1:0];
wire [3 :0] dma_sel_i  [SHARES-1:0];
reg         dma_stall_o[SHARES-1:0];
reg         dma_ack_o  [SHARES-1:0];
reg  [31:0] dma_dat_o  [SHARES-1:0];
reg         dma_err_o  [SHARES-1:0];
wire        dma_rst_i  [SHARES-1:0];

// tb signals
reg [31:0] read_data;
reg [31:0] write_data;
reg [31:0] mem [0:MEM_SIZE-1]; // 4KB memory
reg [31:0] temp;
reg [31:0] temp1;
reg flag;


// Clock generation
initial begin
    clk = 0;
    forever #(CLOCK_PERIOD / 2) clk = ~clk; // 100MHz clock
end

// Reset generation
initial begin
    rst_n = 0;
    #20 rst_n = 1;
end



// Instantiate the x2x_acc_top module
x2x_acc_top #(
    .BASE_ADDR(BASE_ADDR)
) uut (
    .wb_cyc_i(wb_cyc_i),
    .wb_stb_i(wb_stb_i),
    .wb_we_i(wb_we_i),
    .wb_adr_i(wb_adr_i),
    .wb_dat_i(wb_dat_i),
    .wb_sel_i(wb_sel_i),
    .wb_stall_o(wb_stall_o),
    .wb_ack_o(wb_ack_o),
    .wb_dat_o(wb_dat_o),
    .wb_err_o(wb_err_o),
    .wb_rst_i(!rst_n),
    .wb_clk_i(clk),
    .dma_cyc_i(dma_cyc_i),
    .dma_stb_i(dma_stb_i),
    .dma_we_i(dma_we_i),
    .dma_adr_i(dma_adr_i),
    .dma_dat_i(dma_dat_i),
    .dma_sel_i(dma_sel_i),
    .dma_stall_o(dma_stall_o),
    .dma_ack_o(dma_ack_o),
    .dma_dat_o(dma_dat_o),
    .dma_err_o(dma_err_o),
    .dma_rst_i(dma_rst_i)
);

always @(posedge clk) begin
    if (dma_stb_i[0] && !dma_we_i[0]  && !dma_we_i[1]) begin
        dma_dat_o[0] <= mem[dma_adr_i[0]];
        dma_dat_o[1] <= 0;
    end
    else if (dma_stb_i[1] && !dma_we_i[0]  && !dma_we_i[1]) begin
        dma_dat_o[0] <= 0;
        dma_dat_o[1] <= mem[dma_adr_i[1]];
    end
    else begin
        dma_dat_o[0] <= 0;
        dma_dat_o[1] <= 0;
    end
end

always @(posedge clk) begin
    if (!rst_n) begin
        for (int i = 0; i < MEM_SIZE; i = i + 1) begin
            mem[i] <= 0;
        end
    end
    else if (dma_we_i[0]) begin
        mem[dma_adr_i[0]] <= dma_dat_i[0];
    end
    else if (dma_we_i[1]) begin
        mem[dma_adr_i[1]] <= dma_dat_i[1];
    end
end    

task wb_write(input [31:0] address, input [31:0] data);
begin
    @(posedge clk);
    wb_cyc_i = 1;
    wb_stb_i = 1;
    wb_we_i = 1;
    wb_adr_i = address;
    wb_dat_i = data;
    @(posedge clk);
    wb_cyc_i = 0;
    wb_stb_i = 0;
    wb_we_i = 0;
end
endtask


task wb_read(input [31:0] address, output [31:0] read_data);
begin
    @(posedge clk);
    wb_cyc_i = 1;
    wb_stb_i = 1;
    wb_we_i = 0;
    wb_adr_i = address;
    @(posedge clk);
    read_data = wb_dat_o;
    wb_cyc_i = 0;
    wb_stb_i = 0;
end
endtask

// Testbench logic
initial begin
    // Initialize Wishbone signals
    wb_cyc_i = 0;
    wb_stb_i = 0;
    wb_we_i = 0;
    wb_adr_i = 0;
    wb_dat_i = 0;
    wb_sel_i = 4'b1111;
    wb_rst_i = 0;

    // Wait for reset deassertion
    @(posedge rst_n);
    @(posedge clk);

    $display("Loading seed");
    wb_write(uut.x2x_acc_ctrl_inst.BASE_ADDR + uut.x2x_acc_ctrl_inst.SEED_ADDR_LOW, 32'hdeadbeef);
    wb_write(uut.x2x_acc_ctrl_inst.BASE_ADDR + uut.x2x_acc_ctrl_inst.SEED_ADDR_HIGH, 32'hdeadbeef);
    for (int i = 0; i < 50; i = i + 1) begin
        @(posedge clk);
    end


    $display("Test Case 1: B2A_2k");

    for (int i = 0; i < 64; i = i + 4) begin
        mem[i ] = $urandom() & 32'h00ffffff;
        mem[i + 64] = $urandom() & 32'h00ffffff;
    end


    // Perform write operations using the function
    wb_write(uut.x2x_acc_ctrl_inst.BASE_ADDR + uut.x2x_acc_ctrl_inst.DIN_PTR_ADDR_START, 0);
    wb_write(uut.x2x_acc_ctrl_inst.BASE_ADDR + uut.x2x_acc_ctrl_inst.DIN_PTR_ADDR_START + 4, 64);
    wb_write(uut.x2x_acc_ctrl_inst.BASE_ADDR + uut.x2x_acc_ctrl_inst.DOUT_PTR_ADDR_START, 128);
    wb_write(uut.x2x_acc_ctrl_inst.BASE_ADDR + uut.x2x_acc_ctrl_inst.DOUT_PTR_ADDR_START + 4, 192);
    wb_write(uut.x2x_acc_ctrl_inst.BASE_ADDR + uut.x2x_acc_ctrl_inst.DATA_LEN_ADDR, 32'd16);
    wb_write(uut.x2x_acc_ctrl_inst.BASE_ADDR + uut.x2x_acc_ctrl_inst.MODULUS_ADDR, 32'h00ffffff);



    $display("Starting Op");

    // Read control register using the function
    wb_read(uut.x2x_acc_ctrl_inst.BASE_ADDR + uut.x2x_acc_ctrl_inst.CTRL_ADDR, read_data);
    $display("Control Register: %h", read_data);

    write_data = (`X2X_DATA_TYPE_POW2 << uut.x2x_acc_ctrl_inst.CTRL_DATA_TYPE_BIT) | (`X2X_CONV_MODE_B2A << uut.x2x_acc_ctrl_inst.CTRL_CONV_MODE_BIT) | (`X2X_CMD_REFX2X << uut.x2x_acc_ctrl_inst.CTRL_OPCODE_LSB) | (1'b1 << uut.x2x_acc_ctrl_inst.CTRL_START_BIT);
    wb_write(uut.x2x_acc_ctrl_inst.BASE_ADDR + uut.x2x_acc_ctrl_inst.CTRL_ADDR, write_data);

    do begin
        wb_read(uut.x2x_acc_ctrl_inst.BASE_ADDR + uut.x2x_acc_ctrl_inst.CTRL_ADDR, read_data);
    end while ((read_data & (1 << uut.x2x_acc_ctrl_inst.CTRL_DONE_BIT)) == 0);
    $display("Op done: Control Register: %h", read_data);

    flag = 1;
    for (int i = 0; i < 64; i = i + 4) begin
        temp = (mem[i + 128] + mem[i + 192]) & 32'h00ffffff;
        $display("mem[%0d] ^ mem[%0d] = %h", i + 128, i + 192, temp);
        if (temp != (mem[i] ^ mem[i + 64])) begin
            $display("Test failed at word %d. Computed %h, Expected %h ", i, temp, (mem[i] ^ mem[i + 64]));
            flag = 0;
        end
    end
    if (flag) begin
        $display("Test Case 1 passed");
    end
    else begin
        $display("Test Case 1 failed");
        $stop;
    end
    $stop;


//-----------------------------------------------------------------------------------------------//



    $display("Test Case 2: B2A_2k large input");

    for (int i = 0; i < 128; i = i + 4) begin
        mem[i ] = $urandom() & 32'h00ffffff;
        mem[i + 128] = $urandom() & 32'h00ffffff;
    end


    // Perform write operations using the function
    wb_write(uut.x2x_acc_ctrl_inst.BASE_ADDR + uut.x2x_acc_ctrl_inst.DIN_PTR_ADDR_START, 0);
    wb_write(uut.x2x_acc_ctrl_inst.BASE_ADDR + uut.x2x_acc_ctrl_inst.DIN_PTR_ADDR_START + 4, 128);
    wb_write(uut.x2x_acc_ctrl_inst.BASE_ADDR + uut.x2x_acc_ctrl_inst.DOUT_PTR_ADDR_START, 256);
    wb_write(uut.x2x_acc_ctrl_inst.BASE_ADDR + uut.x2x_acc_ctrl_inst.DOUT_PTR_ADDR_START + 4, 384);
    wb_write(uut.x2x_acc_ctrl_inst.BASE_ADDR + uut.x2x_acc_ctrl_inst.DATA_LEN_ADDR, 32'd32);
    wb_write(uut.x2x_acc_ctrl_inst.BASE_ADDR + uut.x2x_acc_ctrl_inst.MODULUS_ADDR, 32'h00ffffff);



    $display("Starting Op");

    // Read control register using the function
    wb_read(uut.x2x_acc_ctrl_inst.BASE_ADDR + uut.x2x_acc_ctrl_inst.CTRL_ADDR, read_data);
    $display("Control Register: %h", read_data);

    write_data = (`X2X_DATA_TYPE_POW2 << uut.x2x_acc_ctrl_inst.CTRL_DATA_TYPE_BIT) | (`X2X_CONV_MODE_B2A << uut.x2x_acc_ctrl_inst.CTRL_CONV_MODE_BIT) | (`X2X_CMD_REFX2X << uut.x2x_acc_ctrl_inst.CTRL_OPCODE_LSB) | (1'b1 << uut.x2x_acc_ctrl_inst.CTRL_START_BIT);
    wb_write(uut.x2x_acc_ctrl_inst.BASE_ADDR + uut.x2x_acc_ctrl_inst.CTRL_ADDR, write_data);

    do begin
        wb_read(uut.x2x_acc_ctrl_inst.BASE_ADDR + uut.x2x_acc_ctrl_inst.CTRL_ADDR, read_data);
    end while ((read_data & (1 << uut.x2x_acc_ctrl_inst.CTRL_DONE_BIT)) == 0);
    $display("Op done: Control Register: %h", read_data);

    flag = 1;
    for (int i = 0; i < 128; i = i + 4) begin
        temp = (mem[i + 256] + mem[i + 384]) & 32'h00ffffff;
        $display("mem[%0d] ^ mem[%0d] = %h", i + 256, i + 384, temp);
        if (temp != (mem[i] ^ mem[i + 128])) begin
            $display("Test failed at word %d. Computed %h, Expected %h ", i, temp, (mem[i] ^ mem[i + 128]));
            flag = 0;
        end
    end
    if (flag) begin
        $display("Test Case 2 passed");
    end
    else begin
        $display("Test Case 2 failed");
        $stop;
    end



//-----------------------------------------------------------------------------------------------//



    $display("Test Case 3: Share Mode B large input");

    for (int i = 0; i < 128; i = i + 4) begin
        mem[i ] = $urandom() & 32'h00ffffff;
        mem[i + 128] = 0;//$urandom() & 32'h00ffffff;
    end


    // Perform write operations using the function
    wb_write(uut.x2x_acc_ctrl_inst.BASE_ADDR + uut.x2x_acc_ctrl_inst.DIN_PTR_ADDR_START, 0);
    wb_write(uut.x2x_acc_ctrl_inst.BASE_ADDR + uut.x2x_acc_ctrl_inst.DIN_PTR_ADDR_START + 4, 0);
    wb_write(uut.x2x_acc_ctrl_inst.BASE_ADDR + uut.x2x_acc_ctrl_inst.DOUT_PTR_ADDR_START, 256);
    wb_write(uut.x2x_acc_ctrl_inst.BASE_ADDR + uut.x2x_acc_ctrl_inst.DOUT_PTR_ADDR_START + 4, 384);
    wb_write(uut.x2x_acc_ctrl_inst.BASE_ADDR + uut.x2x_acc_ctrl_inst.DATA_LEN_ADDR, 32'd32);
    wb_write(uut.x2x_acc_ctrl_inst.BASE_ADDR + uut.x2x_acc_ctrl_inst.MODULUS_ADDR, 32'h00ffffff);



    $display("Starting Op");

    // Read control register using the function
    wb_read(uut.x2x_acc_ctrl_inst.BASE_ADDR + uut.x2x_acc_ctrl_inst.CTRL_ADDR, read_data);
    $display("Control Register: %h", read_data);

    write_data = (`X2X_DATA_TYPE_POW2 << uut.x2x_acc_ctrl_inst.CTRL_DATA_TYPE_BIT) | (`X2X_CONV_MODE_B2A << uut.x2x_acc_ctrl_inst.CTRL_CONV_MODE_BIT) | (`X2X_CMD_REFX2X << uut.x2x_acc_ctrl_inst.CTRL_OPCODE_LSB) | (1'b1 << uut.x2x_acc_ctrl_inst.CTRL_START_BIT) | (1'b1 << uut.x2x_acc_ctrl_inst.CTRL_SHARE_MODE_BIT);
    wb_write(uut.x2x_acc_ctrl_inst.BASE_ADDR + uut.x2x_acc_ctrl_inst.CTRL_ADDR, write_data);

    do begin
        wb_read(uut.x2x_acc_ctrl_inst.BASE_ADDR + uut.x2x_acc_ctrl_inst.CTRL_ADDR, read_data);
    end while ((read_data & (1 << uut.x2x_acc_ctrl_inst.CTRL_DONE_BIT)) == 0);
    $display("Op done: Control Register: %h", read_data);

    flag = 1;
    for (int i = 0; i < 128; i = i + 4) begin
        temp = (mem[i + 256] + mem[i + 384]) & 32'h00ffffff;
        $display("mem[%0d] ^ mem[%0d] = %h", i + 256, i + 384, temp);
        if (temp != (mem[i])) begin
            $display("Test failed at word %d. Computed %h, Expected %h ", i, temp, (mem[i]));
            flag = 0;
        end
    end
    if (flag) begin
        $display("Test Case 3 passed");
    end
    else begin
        $display("Test Case 3 failed");
        $stop;
    end



//-----------------------------------------------------------------------------------------------//


    $display("Test Case 4: B Ref");

    for (int i = 0; i < 64; i = i + 4) begin
        mem[i     ] = $urandom() & 32'h00ffffff;
        mem[i + 64] = $urandom() & 32'h00ffffff;
    end


    // Perform write operations using the function
    wb_write(uut.x2x_acc_ctrl_inst.BASE_ADDR + uut.x2x_acc_ctrl_inst.DIN_PTR_ADDR_START, 0);
    wb_write(uut.x2x_acc_ctrl_inst.BASE_ADDR + uut.x2x_acc_ctrl_inst.DIN_PTR_ADDR_START + 4, 64);
    wb_write(uut.x2x_acc_ctrl_inst.BASE_ADDR + uut.x2x_acc_ctrl_inst.DOUT_PTR_ADDR_START, 128);
    wb_write(uut.x2x_acc_ctrl_inst.BASE_ADDR + uut.x2x_acc_ctrl_inst.DOUT_PTR_ADDR_START + 4, 192);
    wb_write(uut.x2x_acc_ctrl_inst.BASE_ADDR + uut.x2x_acc_ctrl_inst.DATA_LEN_ADDR, 32'd16);
    wb_write(uut.x2x_acc_ctrl_inst.BASE_ADDR + uut.x2x_acc_ctrl_inst.MODULUS_ADDR, 32'h00ffffff);



    $display("Starting Op");

    // Read control register using the function
    wb_read(uut.x2x_acc_ctrl_inst.BASE_ADDR + uut.x2x_acc_ctrl_inst.CTRL_ADDR, read_data);
    $display("Control Register: %h", read_data);

    write_data = (`X2X_DATA_TYPE_POW2 << uut.x2x_acc_ctrl_inst.CTRL_DATA_TYPE_BIT) | (1'b1 << uut.x2x_acc_ctrl_inst.CTRL_CONV_MODE_BIT) | (`X2X_CMD_REF << uut.x2x_acc_ctrl_inst.CTRL_OPCODE_LSB) | (1'b1 << uut.x2x_acc_ctrl_inst.CTRL_START_BIT);
    wb_write(uut.x2x_acc_ctrl_inst.BASE_ADDR + uut.x2x_acc_ctrl_inst.CTRL_ADDR, write_data);

    do begin
        wb_read(uut.x2x_acc_ctrl_inst.BASE_ADDR + uut.x2x_acc_ctrl_inst.CTRL_ADDR, read_data);
    end while ((read_data & (1 << uut.x2x_acc_ctrl_inst.CTRL_DONE_BIT)) == 0);
    $display("Op done: Control Register: %h", read_data);

    flag = 1;
    for (int i = 0; i < 64; i = i + 4) begin
        temp = (mem[i + 128] ^ mem[i + 192]);
        $display("mem[%0d] ^ mem[%0d] = %h", i + 128, i + 192, temp);
        if (temp != (mem[i] ^ mem[i + 64])) begin
            $display("Test failed at word %d. Computed %h, Expected %h ", i, temp, (mem[i] ^ mem[i + 64]));
            flag = 0;
        end
    end
    if (flag) begin
        $display("Test Case 4 passed");
    end
    else begin
        $display("Test Case 4 failed");
        $stop;
    end

//-----------------------------------------------------------------------------------------------//

    $display("Test Case 5: PRNG");


    // Perform write operations using the function
    wb_write(uut.x2x_acc_ctrl_inst.BASE_ADDR + uut.x2x_acc_ctrl_inst.DIN_PTR_ADDR_START, 0);
    wb_write(uut.x2x_acc_ctrl_inst.BASE_ADDR + uut.x2x_acc_ctrl_inst.DIN_PTR_ADDR_START + 4, 0);
    wb_write(uut.x2x_acc_ctrl_inst.BASE_ADDR + uut.x2x_acc_ctrl_inst.DOUT_PTR_ADDR_START, 128);
    wb_write(uut.x2x_acc_ctrl_inst.BASE_ADDR + uut.x2x_acc_ctrl_inst.DOUT_PTR_ADDR_START + 4, 192);
    wb_write(uut.x2x_acc_ctrl_inst.BASE_ADDR + uut.x2x_acc_ctrl_inst.DATA_LEN_ADDR, 32'd16);
    wb_write(uut.x2x_acc_ctrl_inst.BASE_ADDR + uut.x2x_acc_ctrl_inst.MODULUS_ADDR, 32'h00ffffff);

    for (int i = 0; i < uut.x2x_acc_fsm_inst.BURST_LEN; i = i + 1) begin
        uut.x2x_acc_fsm_inst.shares[SHARES-1][i] = 0;
    end

    $display("Starting Op");

    // Read control register using the function
    wb_read(uut.x2x_acc_ctrl_inst.BASE_ADDR + uut.x2x_acc_ctrl_inst.CTRL_ADDR, read_data);
    $display("Control Register: %h", read_data);

    write_data = (`X2X_DATA_TYPE_POW2 << uut.x2x_acc_ctrl_inst.CTRL_DATA_TYPE_BIT) | (`X2X_CMD_PRNG << uut.x2x_acc_ctrl_inst.CTRL_OPCODE_LSB) | (1'b1 << uut.x2x_acc_ctrl_inst.CTRL_START_BIT);
    wb_write(uut.x2x_acc_ctrl_inst.BASE_ADDR + uut.x2x_acc_ctrl_inst.CTRL_ADDR, write_data);

    do begin
        wb_read(uut.x2x_acc_ctrl_inst.BASE_ADDR + uut.x2x_acc_ctrl_inst.CTRL_ADDR, read_data);
    end while ((read_data & (1 << uut.x2x_acc_ctrl_inst.CTRL_DONE_BIT)) == 0);
    $display("Op done: Control Register: %h", read_data);

    flag = 1;
    for (int i = 0; i < 64; i = i + 4) begin
        temp = mem[i + 192];
        $display("mem[%0d] = %h", i + 192, temp);
        if (temp == 0) begin
            $display("Test failed at word %d. Computed %h, Expected non-zero ", i, temp);
            flag = 0;
        end
    end
    if (flag) begin
        $display("Test Case 5 passed");
    end
    else begin
        $display("Test Case 5 failed");
        $stop;
    end

//-----------------------------------------------------------------------------------------------//


    $stop;
    $display("Test Case 6: B2A_q dual");

    for (int i = 0; i < 128; i = i + 4) begin
        temp  = $urandom() % 32'd3329;
        temp1 = $urandom() % 32'd3329;
        mem[i][15: 0] = $urandom() & 16'hffff;
        mem[i][31:16] = $urandom() & 16'hffff;
        mem[i + 128][15: 0] = temp [15: 0] ^ mem[i][15: 0];
        mem[i + 128][31:16] = temp1[31:16] ^ mem[i][31:16];
    end


    // Perform write operations using the function
    wb_write(uut.x2x_acc_ctrl_inst.BASE_ADDR + uut.x2x_acc_ctrl_inst.DIN_PTR_ADDR_START, 0);
    wb_write(uut.x2x_acc_ctrl_inst.BASE_ADDR + uut.x2x_acc_ctrl_inst.DIN_PTR_ADDR_START + 4, 128);
    wb_write(uut.x2x_acc_ctrl_inst.BASE_ADDR + uut.x2x_acc_ctrl_inst.DOUT_PTR_ADDR_START, 256);
    wb_write(uut.x2x_acc_ctrl_inst.BASE_ADDR + uut.x2x_acc_ctrl_inst.DOUT_PTR_ADDR_START + 4, 384);
    wb_write(uut.x2x_acc_ctrl_inst.BASE_ADDR + uut.x2x_acc_ctrl_inst.DATA_LEN_ADDR, 32'd32);
    wb_write(uut.x2x_acc_ctrl_inst.BASE_ADDR + uut.x2x_acc_ctrl_inst.MODULUS_ADDR, 32'd3329);



    $display("Starting Op");

    // Read control register using the function
    wb_read(uut.x2x_acc_ctrl_inst.BASE_ADDR + uut.x2x_acc_ctrl_inst.CTRL_ADDR, read_data);
    $display("Control Register: %h", read_data);

    write_data = (`X2X_DATA_TYPE_PRIME << uut.x2x_acc_ctrl_inst.CTRL_DATA_TYPE_BIT) | (`X2X_CONV_MODE_B2A << uut.x2x_acc_ctrl_inst.CTRL_CONV_MODE_BIT) | (`X2X_CMD_X2X << uut.x2x_acc_ctrl_inst.CTRL_OPCODE_LSB) | (1'b1 << uut.x2x_acc_ctrl_inst.CTRL_DUAL_MODE_BIT) | (5'd12 << uut.x2x_acc_ctrl_inst.CTRL_MOD_SIZE_LSB) | (1'b1 << uut.x2x_acc_ctrl_inst.CTRL_START_BIT);
    wb_write(uut.x2x_acc_ctrl_inst.BASE_ADDR + uut.x2x_acc_ctrl_inst.CTRL_ADDR, write_data);

    do begin
        wb_read(uut.x2x_acc_ctrl_inst.BASE_ADDR + uut.x2x_acc_ctrl_inst.CTRL_ADDR, read_data);
    end while ((read_data & (1 << uut.x2x_acc_ctrl_inst.CTRL_DONE_BIT)) == 0);
    $display("Op done: Control Register: %h", read_data);

    flag = 1;
    for (int i = 0; i < 128; i = i + 4) begin
        temp  = (mem[i + 256][15:0] + mem[i + 384][15:0]) % 32'd3329;
        temp1 = (mem[i      ][15:0] ^ mem[i + 128][15:0]);
        $display("mem[%0d] ^ mem[%0d] = %h", i + 256, i + 384, temp);
        if (temp != temp1) begin
            $display("Test failed at word %d - LOW. Computed %h, Expected %h ", i, temp, temp1);
            flag = 0;
        end
        temp  = (mem[i + 256][31:16] + mem[i + 384][31:16]) % 32'd3329;
        temp1 = (mem[i      ][31:16] ^ mem[i + 128][31:16]);
        $display("mem[%0d] ^ mem[%0d] = %h", i + 256, i + 384, temp);
        if (temp != temp1) begin
            $display("Test failed at word %d - HIGH. Computed %h, Expected %h ", i, temp, temp1);
            flag = 0;
        end
    end
    if (flag) begin
        $display("Test Case 6 passed");
    end
    else begin
        $display("Test Case 6 failed");
        $stop;
    end



//-----------------------------------------------------------------------------------------------//


    $display("Test Case 7: B2A_q dual one-bit");

    for (int i = 0; i < 32; i = i + 4) begin
        mem[i     ] = $urandom();
        mem[i + 32] = $urandom();
    end


    // Perform write operations using the function
    wb_write(uut.x2x_acc_ctrl_inst.BASE_ADDR + uut.x2x_acc_ctrl_inst.DIN_PTR_ADDR_START, 0);
    wb_write(uut.x2x_acc_ctrl_inst.BASE_ADDR + uut.x2x_acc_ctrl_inst.DIN_PTR_ADDR_START + 4, 32);
    wb_write(uut.x2x_acc_ctrl_inst.BASE_ADDR + uut.x2x_acc_ctrl_inst.DOUT_PTR_ADDR_START, 256);
    wb_write(uut.x2x_acc_ctrl_inst.BASE_ADDR + uut.x2x_acc_ctrl_inst.DOUT_PTR_ADDR_START + 4, 768);
    wb_write(uut.x2x_acc_ctrl_inst.BASE_ADDR + uut.x2x_acc_ctrl_inst.DATA_LEN_ADDR, 16'd256);
    wb_write(uut.x2x_acc_ctrl_inst.BASE_ADDR + uut.x2x_acc_ctrl_inst.MODULUS_ADDR, 32'd3329);



    $display("Starting Op");

    // Read control register using the function
    wb_read(uut.x2x_acc_ctrl_inst.BASE_ADDR + uut.x2x_acc_ctrl_inst.CTRL_ADDR, read_data);
    $display("Control Register: %h", read_data);

    write_data = (`X2X_DATA_TYPE_PRIME << uut.x2x_acc_ctrl_inst.CTRL_DATA_TYPE_BIT) | (`X2X_CONV_MODE_B2A << uut.x2x_acc_ctrl_inst.CTRL_CONV_MODE_BIT) | (`X2X_CMD_REFX2X << uut.x2x_acc_ctrl_inst.CTRL_OPCODE_LSB) | (1'b1 << uut.x2x_acc_ctrl_inst.CTRL_DUAL_MODE_BIT) | (5'd12 << uut.x2x_acc_ctrl_inst.CTRL_MOD_SIZE_LSB) | (1'b1 << uut.x2x_acc_ctrl_inst.CTRL_ONE_BIT_MODE_BIT) | (1'b0 << uut.x2x_acc_ctrl_inst.CTRL_LOG_STRIDE_LSB) |(1'b1 << uut.x2x_acc_ctrl_inst.CTRL_START_BIT);
    wb_write(uut.x2x_acc_ctrl_inst.BASE_ADDR + uut.x2x_acc_ctrl_inst.CTRL_ADDR, write_data);

    do begin
        wb_read(uut.x2x_acc_ctrl_inst.BASE_ADDR + uut.x2x_acc_ctrl_inst.CTRL_ADDR, read_data);
    end while ((read_data & (1 << uut.x2x_acc_ctrl_inst.CTRL_DONE_BIT)) == 0);
    $display("Op done: Control Register: %h", read_data);

    flag = 1;
    for (int i = 0; i < 512; i = i + 4) begin
        temp  = (mem[i + 256][15:0] + mem[i + 768][15:0]) % 32'd3329;
        temp1 = (mem[(i >> 6) << 2][(((i & 8'h3f) >> 2) << 1)] ^ mem[32 + ((i >> 6) << 2)][(((i & 8'h3f) >> 2) << 1)]);
        $display("mem[%0d] ^ mem[%0d] = %h", i + 256, i + 768, temp);
        if (temp != temp1) begin
            $display("Test failed at word %d - LOW. Computed %h, Expected %h ", i, temp, temp1);
            flag = 0;
        end
        temp  = (mem[i + 256][31:16] + mem[i + 768][31:16]) % 32'd3329;
        temp1 = (mem[(i >> 6) << 2][(((i & 8'h3f) >> 2) << 1) + 1] ^ mem[32 + ((i >> 6) << 2)][(((i & 8'h3f) >> 2) << 1) + 1]);
        $display("mem[%0d] ^ mem[%0d] = %h", i + 256, i + 768, temp);
        if (temp != temp1) begin
            $display("Test failed at word %d - HIGH. Computed %h, Expected %h ", i, temp, temp1);
            flag = 0;
        end
    end
    if (flag) begin
        $display("Test Case 7 passed");
    end
    else begin
        $display("Test Case 7 failed");
        $stop;
    end



//-----------------------------------------------------------------------------------------------//

    // Finish simulation
    $display("Simulation finished");
    $finish;
end

endmodule


    