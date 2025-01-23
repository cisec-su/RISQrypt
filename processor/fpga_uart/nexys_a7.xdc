#Constraints for NEXYS-A7 FPGA Board

#100MHz input clock
set_property -dict { PACKAGE_PIN E3    IOSTANDARD LVCMOS33 } [get_ports { M100_clk_i }]; #IO_L12P_T1_MRCC_35 Sch=clk100mhz
#create_clock -add -name clk_in1 -period 10.00 -waveform {0 5} [get_ports {M100_clk_i}];
#create_generated_clock -name clk_div_1 -source [get_ports M100_clk_i] -divide_by 2 [get_nets { clk_i }]

#Reset input from switch
set_property -dict { PACKAGE_PIN J15   IOSTANDARD LVCMOS33 } [get_ports { reset_i }]; #IO_L24N_T3_RS0_15 Sch=sw[0]


#Uart RX
set_property -dict { PACKAGE_PIN C4    IOSTANDARD LVCMOS33 } [get_ports { rx_i }]; #IO_L7P_T1_AD6P_35 Sch=uart_txd_in
set_property -dict { PACKAGE_PIN D4    IOSTANDARD LVCMOS33 } [get_ports { tx_o }]; #IO_L11N_T1_SRCC_35 Sch=uart_rxd_out
#set_property -dict { PACKAGE_PIN D3    IOSTANDARD LVCMOS33 } [get_ports { uart_rts }]; #IO_L12N_T1_MRCC_35 Sch=uart_cts
#set_property -dict { PACKAGE_PIN E5    IOSTANDARD LVCMOS33 } [get_ports { uart_cts }]; #IO_L5N_T0_AD13N_35 Sch=uart_rts


#Debug leds
set_property -dict { PACKAGE_PIN K15   IOSTANDARD LVCMOS33 } [get_ports { led1 }]; #IO_L24P_T3_RS1_15 Sch=led[1]
set_property -dict { PACKAGE_PIN J13   IOSTANDARD LVCMOS33 } [get_ports { led2 }]; #IO_L17N_T2_A25_15 Sch=led[2]
set_property -dict { PACKAGE_PIN R18   IOSTANDARD LVCMOS33 } [get_ports { led4 }]; #IO_L7P_T1_D09_14 Sch=led[4]