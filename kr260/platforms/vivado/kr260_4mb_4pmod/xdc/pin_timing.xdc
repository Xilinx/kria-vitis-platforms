# (C) Copyright 2020 - 2022 Xilinx, Inc.
# Copyright (C) 2022 - 2023, Advanced Micro Devices, Inc
# SPDX-License-Identifier: Apache-2.0

#----------------------------------------------------------------------------
# Bitstream properties
#----------------------------------------------------------------------------

set_property BITSTREAM.GENERAL.COMPRESS TRUE [current_design]

#----------------------------------------------------------------------------
# Pin Constraints
#----------------------------------------------------------------------------

#
# Setup AXI_IC2 to RasberryPI header.
#
set_property IOSTANDARD LVCMOS33 [get_ports IIC_0_scl_io]
set_property IOSTANDARD LVCMOS33 [get_ports IIC_0_sda_io]
set_property PACKAGE_PIN AE14 [get_ports IIC_0_scl_io]
set_property PACKAGE_PIN AE15 [get_ports IIC_0_sda_io]
set_property PULLUP true [get_ports IIC_0_scl_io]
set_property PULLUP true [get_ports IIC_0_sda_io]
#set_property PULLUP true [get_ports {pmod_1_tri_io[4]}]
#set_property PULLUP true [get_ports {pmod_1_tri_io[6]}]

## MDIO constraints

set_property PACKAGE_PIN F3 [get_ports mdio_mdio_io]
set_property IOSTANDARD LVCMOS18 [get_ports mdio_mdio_io]
set_property SLEW SLOW [get_ports mdio_mdio_io]
set_property DRIVE 4 [get_ports mdio_mdio_io]

set_property PACKAGE_PIN G3 [get_ports mdio_mdc]
set_property IOSTANDARD LVCMOS18 [get_ports mdio_mdc]
set_property SLEW SLOW [get_ports mdio_mdc]
set_property DRIVE 4 [get_ports mdio_mdc]

set_property PACKAGE_PIN B1 [get_ports {Phy_reset_n[0]}]
set_property IOSTANDARD LVCMOS18 [get_ports {Phy_reset_n[0]}]
set_property SLEW SLOW [get_ports {Phy_reset_n[0]}]
set_property DRIVE 4 [get_ports {Phy_reset_n[0]}]

set_property PACKAGE_PIN T8 [get_ports mdio2_mdio_io]
set_property IOSTANDARD LVCMOS18 [get_ports mdio2_mdio_io]
set_property SLEW SLOW [get_ports mdio2_mdio_io]
set_property DRIVE 4 [get_ports mdio2_mdio_io]
set_property PACKAGE_PIN R8 [get_ports mdio2_mdc]
set_property IOSTANDARD LVCMOS18 [get_ports mdio2_mdc]
set_property SLEW SLOW [get_ports mdio2_mdc]
set_property DRIVE 4 [get_ports mdio2_mdc]
set_property PACKAGE_PIN K1 [get_ports {Phy_reset_n[1]}]
set_property IOSTANDARD LVCMOS18 [get_ports {Phy_reset_n[1]}]
set_property SLEW SLOW [get_ports {Phy_reset_n[1]}]
set_property DRIVE 4 [get_ports {Phy_reset_n[1]}]

### RGMII constraints

## PHY0
set_property PACKAGE_PIN F1 [get_ports rgmii_tx_ctl]
set_property IOSTANDARD LVCMOS18 [get_ports rgmii_tx_ctl]
set_property SLEW SLOW [get_ports rgmii_tx_ctl]
set_property DRIVE 4 [get_ports rgmii_tx_ctl]

set_property PACKAGE_PIN A2 [get_ports rgmii_txc]
set_property IOSTANDARD LVCMOS18 [get_ports rgmii_txc]
set_property SLEW SLOW [get_ports rgmii_txc]
set_property DRIVE 4 [get_ports rgmii_txc]

set_property PACKAGE_PIN E1 [get_ports {rgmii_td[0]}]
set_property IOSTANDARD LVCMOS18 [get_ports {rgmii_td[0]}]
set_property SLEW SLOW [get_ports {rgmii_td[0]}]
set_property DRIVE 4 [get_ports {rgmii_td[0]}]

set_property PACKAGE_PIN D1 [get_ports {rgmii_td[1]}]
set_property IOSTANDARD LVCMOS18 [get_ports {rgmii_td[1]}]
set_property SLEW SLOW [get_ports {rgmii_td[1]}]
set_property DRIVE 4 [get_ports {rgmii_td[1]}]

set_property PACKAGE_PIN F2 [get_ports {rgmii_td[2]}]
set_property IOSTANDARD LVCMOS18 [get_ports {rgmii_td[2]}]
set_property SLEW SLOW [get_ports {rgmii_td[2]}]
set_property DRIVE 4 [get_ports {rgmii_td[2]}]

set_property PACKAGE_PIN E2 [get_ports {rgmii_td[3]}]
set_property IOSTANDARD LVCMOS18 [get_ports {rgmii_td[3]}]
set_property SLEW SLOW [get_ports {rgmii_td[3]}]
set_property DRIVE 4 [get_ports {rgmii_td[3]}]

set_property PACKAGE_PIN A4 [get_ports rgmii_rx_ctl]
set_property IOSTANDARD LVCMOS18 [get_ports rgmii_rx_ctl]
set_property SLEW SLOW [get_ports {rgmii_rx_ctl}]
set_property DRIVE 4 [get_ports {rgmii_rx_ctl}]

set_property PACKAGE_PIN D4 [get_ports rgmii_rxc]
set_property IOSTANDARD LVCMOS18 [get_ports rgmii_rxc]
set_property SLEW SLOW [get_ports {rgmii_rxc}]
set_property DRIVE 4 [get_ports {rgmii_rxc}]

set_property PACKAGE_PIN A1 [get_ports {rgmii_rd[0]}]
set_property IOSTANDARD LVCMOS18 [get_ports {rgmii_rd[0]}]
set_property SLEW SLOW [get_ports {rgmii_rd[0]}]
set_property DRIVE 4 [get_ports {rgmii_rd[0]}]

set_property PACKAGE_PIN B3 [get_ports {rgmii_rd[1]}]
set_property IOSTANDARD LVCMOS18 [get_ports {rgmii_rd[1]}]
set_property SLEW SLOW [get_ports {rgmii_rd[1]}]
set_property DRIVE 4 [get_ports {rgmii_rd[1]}]

set_property PACKAGE_PIN A3 [get_ports {rgmii_rd[2]}]
set_property IOSTANDARD LVCMOS18 [get_ports {rgmii_rd[2]}]
set_property SLEW SLOW [get_ports {rgmii_rd[2]}]
set_property DRIVE 4 [get_ports {rgmii_rd[2]}]

set_property PACKAGE_PIN B4 [get_ports {rgmii_rd[3]}]
set_property IOSTANDARD LVCMOS18 [get_ports {rgmii_rd[3]}]
set_property SLEW SLOW [get_ports {rgmii_rd[3]}]
set_property DRIVE 4 [get_ports {rgmii_rd[3]}]


## PHY1
set_property PACKAGE_PIN Y8 [get_ports rgmii2_tx_ctl]
set_property IOSTANDARD LVCMOS18 [get_ports rgmii2_tx_ctl]
set_property SLEW SLOW [get_ports rgmii2_tx_ctl]
set_property DRIVE 4 [get_ports rgmii2_tx_ctl]

set_property PACKAGE_PIN J1 [get_ports rgmii2_txc]
set_property IOSTANDARD LVCMOS18 [get_ports rgmii2_txc]
set_property SLEW SLOW [get_ports rgmii2_txc]
set_property DRIVE 4 [get_ports rgmii2_txc]

set_property PACKAGE_PIN U9 [get_ports {rgmii2_td[0]}]
set_property IOSTANDARD LVCMOS18 [get_ports {rgmii2_td[0]}]
set_property SLEW SLOW [get_ports {rgmii2_td[0]}]
set_property DRIVE 4 [get_ports {rgmii2_td[0]}]

set_property PACKAGE_PIN V9 [get_ports {rgmii2_td[1]}]
set_property IOSTANDARD LVCMOS18 [get_ports {rgmii2_td[1]}]
set_property SLEW SLOW [get_ports {rgmii2_td[1]}]
set_property DRIVE 4 [get_ports {rgmii2_td[1]}]

set_property PACKAGE_PIN U8 [get_ports {rgmii2_td[2]}]
set_property IOSTANDARD LVCMOS18 [get_ports {rgmii2_td[2]}]
set_property SLEW SLOW [get_ports {rgmii2_td[2]}]
set_property DRIVE 4 [get_ports {rgmii2_td[2]}]

set_property PACKAGE_PIN V8 [get_ports {rgmii2_td[3]}]
set_property IOSTANDARD LVCMOS18 [get_ports {rgmii2_td[3]}]
set_property SLEW SLOW [get_ports {rgmii2_td[3]}]
set_property DRIVE 4 [get_ports {rgmii2_td[3]}]

set_property PACKAGE_PIN H3 [get_ports rgmii2_rx_ctl]
set_property IOSTANDARD LVCMOS18 [get_ports rgmii2_rx_ctl]
set_property SLEW SLOW [get_ports {rgmii2_rx_ctl}]
set_property DRIVE 4 [get_ports {rgmii2_rx_ctl}]

set_property PACKAGE_PIN K4 [get_ports rgmii2_rxc]
set_property IOSTANDARD LVCMOS18 [get_ports rgmii2_rxc]
set_property SLEW SLOW [get_ports {rgmii2_rxc}]
set_property DRIVE 4 [get_ports {rgmii2_rxc}]


set_property PACKAGE_PIN H1 [get_ports {rgmii2_rd[0]}]
set_property IOSTANDARD LVCMOS18 [get_ports {rgmii2_rd[0]}]
set_property SLEW SLOW [get_ports {rgmii2_rd[0]}]
set_property DRIVE 4 [get_ports {rgmii2_rd[0]}]

set_property PACKAGE_PIN K2 [get_ports {rgmii2_rd[1]}]
set_property IOSTANDARD LVCMOS18 [get_ports {rgmii2_rd[1]}]
set_property SLEW SLOW [get_ports {rgmii2_rd[1]}]
set_property DRIVE 4 [get_ports {rgmii2_rd[1]}]

set_property PACKAGE_PIN J2 [get_ports {rgmii2_rd[2]}]
set_property IOSTANDARD LVCMOS18 [get_ports {rgmii2_rd[2]}]
set_property SLEW SLOW [get_ports {rgmii2_rd[2]}]
set_property DRIVE 4 [get_ports {rgmii2_rd[2]}]

set_property PACKAGE_PIN H4 [get_ports {rgmii2_rd[3]}]
set_property IOSTANDARD LVCMOS18 [get_ports {rgmii2_rd[3]}]
set_property SLEW SLOW [get_ports {rgmii2_rd[3]}]
set_property DRIVE 4 [get_ports {rgmii2_rd[3]}]



## RS485 AXI uartlite PMOD1 constraints
## (now replaced by io_switch)
#set_property PACKAGE_PIN B10 [get_ports UART_0_rxen]
#set_property IOSTANDARD LVCMOS33 [get_ports UART_0_rxen]
#set_property SLEW SLOW [get_ports UART_0_rxen]
#set_property DRIVE 4 [get_ports UART_0_rxen]
#
#
#set_property PACKAGE_PIN E12 [get_ports UART_0_txd]
#set_property IOSTANDARD LVCMOS33 [get_ports UART_0_txd]
#set_property SLEW SLOW [get_ports UART_0_txd]
#set_property DRIVE 4 [get_ports UART_0_txd]
#
#
#set_property PACKAGE_PIN D11 [get_ports UART_0_rxd]
#set_property IOSTANDARD LVCMOS33 [get_ports UART_0_rxd]
#set_property SLEW SLOW [get_ports UART_0_rxd]
#set_property DRIVE 4 [get_ports UART_0_rxd]
#
#
#set_property PACKAGE_PIN B11 [get_ports UART_0_txen]
#set_property IOSTANDARD LVCMOS33 [get_ports UART_0_txen]
#set_property SLEW SLOW [get_ports UART_0_txen]
#set_property DRIVE 4 [get_ports UART_0_txen]


#Fan Speed Enable
set_property PACKAGE_PIN A12 [get_ports {fan_en_b}]
set_property IOSTANDARD LVCMOS33 [get_ports {fan_en_b}]
set_property SLEW SLOW [get_ports {fan_en_b}]
set_property DRIVE 4 [get_ports {fan_en_b}]


## Clock inputs from board

# 25Mhz clock
set_property PACKAGE_PIN C3 [get_ports CLK_IN_gem]
set_property IOSTANDARD LVCMOS18 [get_ports CLK_IN_gem]

#------------------------------------------------------------------
# Timing Constraints
#------------------------------------------------------------------

## False paths
set_false_path -from [get_clocks clk_out1_kr260_4mb_4pmod_clk_wiz_0_0] -to [get_clocks clk_out2_kr260_4mb_4pmod_clk_wiz_0_0] 
set_false_path -from [get_clocks clk_out2_kr260_4mb_4pmod_clk_wiz_0_0] -to [get_clocks clk_out1_kr260_4mb_4pmod_clk_wiz_0_0] 
set_false_path -from [get_clocks clk_out2_kr260_4mb_4pmod_clk_wiz_0_0] -to [get_clocks clk_out4_kr260_4mb_4pmod_clk_wiz_0_0] 
set_false_path -from [get_clocks clk_out4_kr260_4mb_4pmod_clk_wiz_0_0] -to [get_clocks clk_out2_kr260_4mb_4pmod_clk_wiz_0_0] 
set_false_path -from [get_clocks clk_out2_kr260_4mb_4pmod_clk_wiz_0_0] -to [get_clocks clk_out5_kr260_4mb_4pmod_clk_wiz_0_0] 
set_false_path -from [get_clocks clk_out5_kr260_4mb_4pmod_clk_wiz_0_0] -to [get_clocks clk_out2_kr260_4mb_4pmod_clk_wiz_0_0] 

set_property DELAY_VALUE 0 [get_cells -hier -filter {NAME =~ */rgmii_interface/delay_rgmii_tx_clk}]
set_property DELAY_VALUE 0 [get_cells -hier -filter {NAME =~ */rgmii_interface/delay_rgmii_rx*}]
set_property DELAY_VALUE 0 [get_cells -hier -filter {NAME =~ */rgmii_interface/rxdata_bus[*].delay_rgmii_rx*}]

#set_property DELAY_VALUE 1100 [get_cells -hier -filter {NAME =~ */rgmii_interface/delay_rgmii_tx_clk}]
#set_property DELAY_VALUE 900 [get_cells -hier -filter {NAME =~ */rgmii_interface/delay_rgmii_rx*}]
#set_property DELAY_VALUE 900 [get_cells -hier -filter {NAME =~ */rgmii_interface/rxdata_bus[*].delay_rgmii_rx*}]


## Refclk frequency
set_property REFCLK_FREQUENCY 333.333 [get_cells -hier -filter {NAME =~ *delay_rgmii_tx*}]
set_property REFCLK_FREQUENCY 333.333 [get_cells -hier -filter {NAME =~ *delay_rgmii_rx*}]
set_clock_groups -asynchronous -group [get_clocks CLK_IN_gem]
set_clock_groups -asynchronous -group [get_clocks clk_out1_kr260_4mb_4pmod_clk_wiz_0_0*]
set_clock_groups -asynchronous -group [get_clocks clk_out2_kr260_4mb_4pmod_clk_wiz_0_0*]
set_clock_groups -asynchronous -group [get_clocks clk_out3_kr260_4mb_4pmod_clk_wiz_0_0*]
set_clock_groups -asynchronous -group [get_clocks clk_out4_kr260_4mb_4pmod_clk_wiz_0_0*]
set_clock_groups -asynchronous -group [get_clocks clk_out5_kr260_4mb_4pmod_clk_wiz_0_0*]
set_clock_groups -asynchronous -group [get_clocks rgmii_rxc]
set_clock_groups -asynchronous -group [get_clocks rgmii2_rxc]
set_clock_groups -asynchronous -group [get_clocks clk_pl_0]
#
set_clock_groups -asynchronous -group [get_clocks kr260_4mb_4pmod_i/mb_subsystem_*/axi_ethernet_0/inst/mac/inst_rgmii_tx_clk]


#for subsystem 0
set_property IODELAY_GROUP tri_mode_ethernet_mac_iodelay_grp0 [get_cells -hierarchical -filter { PRIMITIVE_TYPE == I/O.DELAY.IDELAYCTRL && NAME =~  "*mb_subsystem_0*" } ]
set_property IODELAY_GROUP tri_mode_ethernet_mac_iodelay_grp0 [get_cells -hierarchical -filter { PRIMITIVE_TYPE == I/O.DELAY.IDELAYE3 && NAME =~  "*mb_subsystem_0*" } ]
set_property IODELAY_GROUP tri_mode_ethernet_mac_iodelay_grp0 [get_cells -hierarchical -filter { PRIMITIVE_TYPE == I/O.DELAY.ODELAYE3 && NAME =~  "*mb_subsystem_0*" } ]
#for subsystem 1
set_property IODELAY_GROUP tri_mode_ethernet_mac_iodelay_grp1 [get_cells -hierarchical -filter { PRIMITIVE_TYPE == I/O.DELAY.IDELAYCTRL && NAME =~  "*mb_subsystem_1*" } ]
set_property IODELAY_GROUP tri_mode_ethernet_mac_iodelay_grp1 [get_cells -hierarchical -filter { PRIMITIVE_TYPE == I/O.DELAY.IDELAYE3 && NAME =~  "*mb_subsystem_1*" } ]
set_property IODELAY_GROUP tri_mode_ethernet_mac_iodelay_grp1 [get_cells -hierarchical -filter { PRIMITIVE_TYPE == I/O.DELAY.ODELAYE3 && NAME =~  "*mb_subsystem_1*" } ]


#pmod locations
#set_property PACKAGE_PIN AF12 [get_ports pmod3_tx]; # pmod3_pin2 <--> som240_2_d45 <--> AF12
#set_property PACKAGE_PIN AG10 [get_ports pmod3_rx]; # pmod3_pin3 <--> som240_2_d46 <--> AG10
#set_property IOSTANDARD LVCMOS33 [get_ports pmod3_tx]; # pmod3_pin2 <--> som240_2_d45 <--> AF12
#set_property IOSTANDARD LVCMOS33 [get_ports pmod3_rx]; # pmod3_pin3 <--> som240_2_d46 <--> AG10
#
#set_property PACKAGE_PIN AD12 [get_ports pmod4_tx]; # pmod4_pin2 <--> som240_2_c50 <--> AD12
#set_property PACKAGE_PIN AE10 [get_ports pmod4_rx]; # pmod4_pin3 <--> som240_2_c51 <--> AE10
#set_property IOSTANDARD LVCMOS33 [get_ports pmod4_tx]; # pmod4_pin2 <--> som240_2_c50 <--> AD12
#set_property IOSTANDARD LVCMOS33 [get_ports pmod4_rx]; # pmod4_pin3 <--> som240_2_c51 <--> AE10



set_property IOSTANDARD LVCMOS33 [get_ports pmod_1_tri_io[0]]; # pmod1_pin1 <--> som240_1_a17 <--> H12
set_property IOSTANDARD LVCMOS33 [get_ports pmod_1_tri_io[1]]; # pmod1_pin2 <--> som240_1_d20 <--> E10
set_property IOSTANDARD LVCMOS33 [get_ports pmod_1_tri_io[2]]; # pmod1_pin3 <--> som240_1_d21 <--> D10
set_property IOSTANDARD LVCMOS33 [get_ports pmod_1_tri_io[3]]; # pmod1_pin4 <--> som240_1_d22 <--> C11
set_property IOSTANDARD LVCMOS33 [get_ports pmod_1_tri_io[4]]; # pmod1_pin5 <--> som240_1_b20 <--> B10
set_property IOSTANDARD LVCMOS33 [get_ports pmod_1_tri_io[5]]; # pmod1_pin6 <--> som240_1_b21 <--> E12
set_property IOSTANDARD LVCMOS33 [get_ports pmod_1_tri_io[6]]; # pmod1_pin7 <--> som240_1_b22 <--> D11
set_property IOSTANDARD LVCMOS33 [get_ports pmod_1_tri_io[7]]; # pmod1_pin8 <--> som240_1_c22 <--> B11

set_property IOSTANDARD LVCMOS33 [get_ports pmod_2_tri_io[0]]; # pmod2_pin1 <--> som240_1_d18 <--> J11
set_property IOSTANDARD LVCMOS33 [get_ports pmod_2_tri_io[1]]; # pmod2_pin2 <--> som240_1_b16 <--> J10
set_property IOSTANDARD LVCMOS33 [get_ports pmod_2_tri_io[2]]; # pmod2_pin3 <--> som240_1_b17 <--> K13
set_property IOSTANDARD LVCMOS33 [get_ports pmod_2_tri_io[3]]; # pmod2_pin4 <--> som240_1_b18 <--> K12
set_property IOSTANDARD LVCMOS33 [get_ports pmod_2_tri_io[4]]; # pmod2_pin5 <--> som240_1_c18 <--> H11
set_property IOSTANDARD LVCMOS33 [get_ports pmod_2_tri_io[5]]; # pmod2_pin6 <--> som240_1_c19 <--> G10
set_property IOSTANDARD LVCMOS33 [get_ports pmod_2_tri_io[6]]; # pmod2_pin7 <--> som240_1_c20 <--> F12
set_property IOSTANDARD LVCMOS33 [get_ports pmod_2_tri_io[7]]; # pmod2_pin8 <--> som240_1_a15 <--> F11


set_property IOSTANDARD LVCMOS33 [get_ports pmod_3_tri_io[0]]; # pmod3_pin1 <--> som240_2_d44 <--> AE12
set_property IOSTANDARD LVCMOS33 [get_ports pmod_3_tri_io[1]]; # pmod3_pin2 <--> som240_2_d45 <--> AF12
set_property IOSTANDARD LVCMOS33 [get_ports pmod_3_tri_io[2]]; # pmod3_pin3 <--> som240_2_d46 <--> AG10
set_property IOSTANDARD LVCMOS33 [get_ports pmod_3_tri_io[3]]; # pmod3_pin4 <--> som240_2_d48 <--> AH10
set_property IOSTANDARD LVCMOS33 [get_ports pmod_3_tri_io[4]]; # pmod3_pin5 <--> som240_2_d49 <--> AF11
set_property IOSTANDARD LVCMOS33 [get_ports pmod_3_tri_io[5]]; # pmod3_pin6 <--> som240_2_d50 <--> AG11
set_property IOSTANDARD LVCMOS33 [get_ports pmod_3_tri_io[6]]; # pmod3_pin7 <--> som240_2_c46 <--> AH12
set_property IOSTANDARD LVCMOS33 [get_ports pmod_3_tri_io[7]]; # pmod3_pin8 <--> som240_2_c47 <--> AH11

set_property IOSTANDARD LVCMOS33 [get_ports pmod_4_tri_io[0]]; # pmod4_pin1 <--> som240_2_c48 <--> AC12
set_property IOSTANDARD LVCMOS33 [get_ports pmod_4_tri_io[1]]; # pmod4_pin2 <--> som240_2_c50 <--> AD12
set_property IOSTANDARD LVCMOS33 [get_ports pmod_4_tri_io[2]]; # pmod4_pin3 <--> som240_2_c51 <--> AE10
set_property IOSTANDARD LVCMOS33 [get_ports pmod_4_tri_io[3]]; # pmod4_pin4 <--> som240_2_c52 <--> AF10
set_property IOSTANDARD LVCMOS33 [get_ports pmod_4_tri_io[4]]; # pmod4_pin5 <--> som240_2_b44 <--> AD11
set_property IOSTANDARD LVCMOS33 [get_ports pmod_4_tri_io[5]]; # pmod4_pin6 <--> som240_2_b45 <--> AD10
set_property IOSTANDARD LVCMOS33 [get_ports pmod_4_tri_io[6]]; # pmod4_pin7 <--> som240_2_b46 <--> AA11
set_property IOSTANDARD LVCMOS33 [get_ports pmod_4_tri_io[7]]; # pmod4_pin8 <--> som240_2_b48 <--> AA10

set_property PACKAGE_PIN H12 [get_ports pmod_1_tri_io[0]]; # pmod1_pin1 <--> som240_1_a17 <--> H12
set_property PACKAGE_PIN E10 [get_ports pmod_1_tri_io[1]]; # pmod1_pin2 <--> som240_1_d20 <--> E10
set_property PACKAGE_PIN D10 [get_ports pmod_1_tri_io[2]]; # pmod1_pin3 <--> som240_1_d21 <--> D10
set_property PACKAGE_PIN C11 [get_ports pmod_1_tri_io[3]]; # pmod1_pin4 <--> som240_1_d22 <--> C11
set_property PACKAGE_PIN B10 [get_ports pmod_1_tri_io[4]]; # pmod1_pin5 <--> som240_1_b20 <--> B10
set_property PACKAGE_PIN E12 [get_ports pmod_1_tri_io[5]]; # pmod1_pin6 <--> som240_1_b21 <--> E12
set_property PACKAGE_PIN D11 [get_ports pmod_1_tri_io[6]]; # pmod1_pin7 <--> som240_1_b22 <--> D11
set_property PACKAGE_PIN B11 [get_ports pmod_1_tri_io[7]]; # pmod1_pin8 <--> som240_1_c22 <--> B11

set_property PACKAGE_PIN J11 [get_ports pmod_2_tri_io[0]]; # pmod2_pin1 <--> som240_1_d18 <--> J11
set_property PACKAGE_PIN J10 [get_ports pmod_2_tri_io[1]]; # pmod2_pin2 <--> som240_1_b16 <--> J10
set_property PACKAGE_PIN K13 [get_ports pmod_2_tri_io[2]]; # pmod2_pin3 <--> som240_1_b17 <--> K13
set_property PACKAGE_PIN K12 [get_ports pmod_2_tri_io[3]]; # pmod2_pin4 <--> som240_1_b18 <--> K12
set_property PACKAGE_PIN H11 [get_ports pmod_2_tri_io[4]]; # pmod2_pin5 <--> som240_1_c18 <--> H11
set_property PACKAGE_PIN G10 [get_ports pmod_2_tri_io[5]]; # pmod2_pin6 <--> som240_1_c19 <--> G10
set_property PACKAGE_PIN F12 [get_ports pmod_2_tri_io[6]]; # pmod2_pin7 <--> som240_1_c20 <--> F12
set_property PACKAGE_PIN F11 [get_ports pmod_2_tri_io[7]]; # pmod2_pin8 <--> som240_1_a15 <--> F11

set_property PACKAGE_PIN AE12 [get_ports pmod_3_tri_io[0]]; # pmod3_pin1 <--> som240_2_d44 <--> AE12
set_property PACKAGE_PIN AF12 [get_ports pmod_3_tri_io[1]]; # pmod3_pin2 <--> som240_2_d45 <--> AF12
set_property PACKAGE_PIN AG10 [get_ports pmod_3_tri_io[2]]; # pmod3_pin3 <--> som240_2_d46 <--> AG10
set_property PACKAGE_PIN AH10 [get_ports pmod_3_tri_io[3]]; # pmod3_pin4 <--> som240_2_d48 <--> AH10
set_property PACKAGE_PIN AF11 [get_ports pmod_3_tri_io[4]]; # pmod3_pin5 <--> som240_2_d49 <--> AF11
set_property PACKAGE_PIN AG11 [get_ports pmod_3_tri_io[5]]; # pmod3_pin6 <--> som240_2_d50 <--> AG11
set_property PACKAGE_PIN AH12 [get_ports pmod_3_tri_io[6]]; # pmod3_pin7 <--> som240_2_c46 <--> AH12
set_property PACKAGE_PIN AH11 [get_ports pmod_3_tri_io[7]]; # pmod3_pin8 <--> som240_2_c47 <--> AH11

set_property PACKAGE_PIN AC12 [get_ports pmod_4_tri_io[0]]; # pmod4_pin1 <--> som240_2_c48 <--> AC12
set_property PACKAGE_PIN AD12 [get_ports pmod_4_tri_io[1]]; # pmod4_pin2 <--> som240_2_c50 <--> AD12
set_property PACKAGE_PIN AE10 [get_ports pmod_4_tri_io[2]]; # pmod4_pin3 <--> som240_2_c51 <--> AE10
set_property PACKAGE_PIN AF10 [get_ports pmod_4_tri_io[3]]; # pmod4_pin4 <--> som240_2_c52 <--> AF10
set_property PACKAGE_PIN AD11 [get_ports pmod_4_tri_io[4]]; # pmod4_pin5 <--> som240_2_b44 <--> AD11
set_property PACKAGE_PIN AD10 [get_ports pmod_4_tri_io[5]]; # pmod4_pin6 <--> som240_2_b45 <--> AD10
set_property PACKAGE_PIN AA11 [get_ports pmod_4_tri_io[6]]; # pmod4_pin7 <--> som240_2_b46 <--> AA11
set_property PACKAGE_PIN AA10 [get_ports pmod_4_tri_io[7]]; # pmod4_pin8 <--> som240_2_b48 <--> AA10

#this was for testing the i2c signals without io_switch on J21 connector
#set_property PULLUP true [get_ports pmod_4_tri_io[0]]; #I2C SDA
#
#set_property PACKAGE_PIN W14 [get_ports iic_j21_0_scl_io]; # W14 - B54 - J21.8
#set_property IOSTANDARD LVCMOS33 [get_ports iic_j21_0_scl_io];
#
#set_property PACKAGE_PIN W13 [get_ports iic_j21_0_sda_io]; # W13 - B56 - J21.10
#set_property PULLUP true [get_ports iic_j21_0_sda_io]; #I2C SDA
#set_property IOSTANDARD LVCMOS33 [get_ports iic_j21_0_sda_io];
