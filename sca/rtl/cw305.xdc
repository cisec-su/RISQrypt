
######## HARDWARE ON BOARD

#UART
set_property PACKAGE_PIN B12 [get_ports tx_o]
set_property PACKAGE_PIN A13 [get_ports rx_i]


#LEDs, GPIOs, and Trigger
set_property DRIVE 8 [get_ports led]
set_property PACKAGE_PIN T2 [get_ports led]

set_property DRIVE 8 [get_ports gpio[0]]
set_property PACKAGE_PIN T3 [get_ports gpio[0]]

set_property DRIVE 8 [get_ports gpio[1]]
set_property PACKAGE_PIN T4 [get_ports gpio[1]]

set_property PACKAGE_PIN A15 [get_ports gpio[2]]
set_property PACKAGE_PIN B15 [get_ports gpio[3]]
set_property PACKAGE_PIN C12 [get_ports gpio[4]]
set_property PACKAGE_PIN C11 [get_ports gpio[5]]
set_property PACKAGE_PIN B14 [get_ports gpio[6]]
set_property PACKAGE_PIN C14 [get_ports gpio[7]]

# T14 = tio_trigger
set_property PACKAGE_PIN T14 [get_ports tio_trigger]


#UART
set_property PACKAGE_PIN B12 [get_ports tx_o]
set_property PACKAGE_PIN A13 [get_ports rx_i]


#Switch's
# set_property PACKAGE_PIN J16 [get_ports j16_sel]
# set_property PACKAGE_PIN K16 [get_ports k16_sel]
# set_property PACKAGE_PIN L14 [get_ports l14_sel]
# set_property PACKAGE_PIN K15 [get_ports k15_sel]
set_property PACKAGE_PIN R1 [get_ports reset_i]

#PLL Connections
set_property PACKAGE_PIN N13 [get_ports clk_i]
#set_property PACKAGE_PIN E12 [get_ports pll_clk2]

######## 20-Pin Connector

set_property PACKAGE_PIN T14 [get_ports tio_trigger]

set_property PACKAGE_PIN M16 [get_ports tio_clkout]

# set_property PACKAGE_PIN N14 [get_ports tio_clkin]

####### USB Connector

set_property PACKAGE_PIN F5 [get_ports usb_clk]

set_property IOSTANDARD LVCMOS33 [get_ports *]
set_property PACKAGE_PIN A7 [get_ports {usb_data[0]}]
set_property PACKAGE_PIN B6 [get_ports {usb_data[1]}]
set_property PACKAGE_PIN D3 [get_ports {usb_data[2]}]
set_property PACKAGE_PIN E3 [get_ports {usb_data[3]}]
set_property PACKAGE_PIN F3 [get_ports {usb_data[4]}]
set_property PACKAGE_PIN B5 [get_ports {usb_data[5]}]
set_property PACKAGE_PIN K1 [get_ports {usb_data[6]}]
set_property PACKAGE_PIN K2 [get_ports {usb_data[7]}]

set_property PACKAGE_PIN F4 [get_ports {usb_addr[0]}]
set_property PACKAGE_PIN G5 [get_ports {usb_addr[1]}]
set_property PACKAGE_PIN J1 [get_ports {usb_addr[2]}]
set_property PACKAGE_PIN H1 [get_ports {usb_addr[3]}]
set_property PACKAGE_PIN H2 [get_ports {usb_addr[4]}]
set_property PACKAGE_PIN G1 [get_ports {usb_addr[5]}]
set_property PACKAGE_PIN G2 [get_ports {usb_addr[6]}]
set_property PACKAGE_PIN F2 [get_ports {usb_addr[7]}]
set_property PACKAGE_PIN E1 [get_ports {usb_addr[8]}]
set_property PACKAGE_PIN E2 [get_ports {usb_addr[9]}]
set_property PACKAGE_PIN D1 [get_ports {usb_addr[10]}]
set_property PACKAGE_PIN C1 [get_ports {usb_addr[11]}]
set_property PACKAGE_PIN K3 [get_ports {usb_addr[12]}]
set_property PACKAGE_PIN L2 [get_ports {usb_addr[13]}]
set_property PACKAGE_PIN J3 [get_ports {usb_addr[14]}]
set_property PACKAGE_PIN B2 [get_ports {usb_addr[15]}]
set_property PACKAGE_PIN C7 [get_ports {usb_addr[16]}]
set_property PACKAGE_PIN C6 [get_ports {usb_addr[17]}]
set_property PACKAGE_PIN D6 [get_ports {usb_addr[18]}]
set_property PACKAGE_PIN C4 [get_ports {usb_addr[19]}]
set_property PACKAGE_PIN D5 [get_ports {usb_addr[20]}]

set_property PACKAGE_PIN A4 [get_ports usb_rdn]
set_property PACKAGE_PIN C2 [get_ports usb_wrn]
set_property PACKAGE_PIN A3 [get_ports usb_cen]

set_property PACKAGE_PIN A5 [get_ports usb_trigger]


create_clock -period 10.000  -name usb_clk -waveform {0.000 5.000}  [get_nets usb_clk]
create_clock -period 50.000  -name clk_i   -waveform {0.000 25.000} [get_nets clk_i  ]
# create_clock -period 10.000 -name tio_clkin -waveform {0.000 5.000} [get_nets tio_clkin]
# create_clock -period 100.000 -name M100_clk_i -waveform {0.000 50.000} [get_nets M100_clk_i]

# create_generated_clock -name clk_out1_clk_wiz_0 \
#                        -source [get_pins clkwiz0/inst/clk_in1] \
#                                [get_pins clkwiz0/inst/clk_out1]

set_clock_groups -asynchronous \
                 -group [get_clocks usb_clk] \
                 -group [get_clocks clk_i  ]

# both input clocks have same properties so there is no point in doing timing analysis for both:
# set_case_analysis 1 [get_pins U_clocks/CCLK_MUX/S]

# # No spec for these, seems sensible:
set_input_delay -clock usb_clk -add_delay 2.000 [get_ports usb_addr]
set_input_delay -clock usb_clk -add_delay 2.000 [get_ports usb_data]
set_input_delay -clock usb_clk -add_delay 2.000 [get_ports usb_trigger]
set_input_delay -clock usb_clk -add_delay 2.000 [get_ports usb_cen]
set_input_delay -clock usb_clk -add_delay 2.000 [get_ports usb_rdn]
set_input_delay -clock usb_clk -add_delay 2.000 [get_ports usb_wrn]

# set_input_delay -clock usb_clk -add_delay 0.000 [get_ports j16_sel]
# set_input_delay -clock usb_clk -add_delay 0.000 [get_ports k16_sel]
# set_input_delay -clock [get_clocks usb_clk] -add_delay 0.500 [get_ports pushbutton]

# set_output_delay -clock usb_clk 0.000 [get_ports led1]
# set_output_delay -clock usb_clk 0.000 [get_ports led2]
# set_output_delay -clock usb_clk 0.000 [get_ports led3]
set_output_delay -clock usb_clk 0.000 [get_ports usb_data]
set_output_delay -clock usb_clk 0.000 [get_ports tio_trigger]
set_output_delay -clock usb_clk 0.000 [get_ports tio_clkout]
# set_false_path -to [get_ports led1]
# set_false_path -to [get_ports led2]
# set_false_path -to [get_ports led3]
set_false_path -to [get_ports usb_data]
set_false_path -to [get_ports tio_trigger]
set_false_path -to [get_ports tio_clkout]

set_property CFGBVS VCCO [current_design]
set_property CONFIG_VOLTAGE 3.3 [current_design]

set_property BITSTREAM.CONFIG.USR_ACCESS TIMESTAMP [current_design]
set_property BITSTREAM.GENERAL.COMPRESS TRUE [current_design]

