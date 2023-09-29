# (C) Copyright 2020 - 2022 Xilinx, Inc.
# SPDX-License-Identifier: Apache-2.0

#----------------------------------------------------------------------------
# Bitstream properties
#----------------------------------------------------------------------------

set_property BITSTREAM.GENERAL.COMPRESS TRUE [current_design]

#----------------------------------------------------------------------------
# Pin Constraints
#----------------------------------------------------------------------------

## MDIO constraints

set_property PACKAGE_PIN F3 [get_ports mdio_io]
set_property IOSTANDARD LVCMOS18 [get_ports mdio_io]
set_property SLEW SLOW [get_ports mdio_io]
set_property DRIVE 4 [get_ports mdio_io]

set_property PACKAGE_PIN G3 [get_ports mdio_mdc]
set_property IOSTANDARD LVCMOS18 [get_ports mdio_mdc]
set_property SLEW SLOW [get_ports mdio_mdc]
set_property DRIVE 4 [get_ports mdio_mdc]

set_property PACKAGE_PIN B1 [get_ports {Phy_reset_n[0]}]
set_property IOSTANDARD LVCMOS18 [get_ports {Phy_reset_n[0]}]
set_property SLEW SLOW [get_ports {Phy_reset_n[0]}]
set_property DRIVE 4 [get_ports {Phy_reset_n[0]}]

set_property PACKAGE_PIN T8 [get_ports mdio2_io]
set_property IOSTANDARD LVCMOS18 [get_ports mdio2_io]
set_property SLEW SLOW [get_ports mdio2_io]
set_property DRIVE 4 [get_ports mdio2_io]
set_property PACKAGE_PIN R8 [get_ports mdio2_mdc]
set_property IOSTANDARD LVCMOS18 [get_ports mdio2_mdc]
set_property SLEW SLOW [get_ports mdio2_mdc]
set_property DRIVE 4 [get_ports mdio2_mdc]
set_property PACKAGE_PIN K1 [get_ports {Phy_reset_n[1]}]
set_property IOSTANDARD LVCMOS18 [get_ports {Phy_reset_n[1]}]
set_property SLEW SLOW [get_ports {Phy_reset_n[1]}]
set_property DRIVE 4 [get_ports {Phy_reset_n[1]}]

## RGMII constraints

# PHY0
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


# PHY1
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



## RS485 AXI uartlite PMOD constraints

set_property PACKAGE_PIN B10 [get_ports UART_0_rxen]
set_property IOSTANDARD LVCMOS33 [get_ports UART_0_rxen]
set_property SLEW SLOW [get_ports UART_0_rxen]
set_property DRIVE 4 [get_ports UART_0_rxen]


set_property PACKAGE_PIN E12 [get_ports UART_0_txd]
set_property IOSTANDARD LVCMOS33 [get_ports UART_0_txd]
set_property SLEW SLOW [get_ports UART_0_txd]
set_property DRIVE 4 [get_ports UART_0_txd]


set_property PACKAGE_PIN D11 [get_ports UART_0_rxd]
set_property IOSTANDARD LVCMOS33 [get_ports UART_0_rxd]
set_property SLEW SLOW [get_ports UART_0_rxd]
set_property DRIVE 4 [get_ports UART_0_rxd]


set_property PACKAGE_PIN B11 [get_ports UART_0_txen]
set_property IOSTANDARD LVCMOS33 [get_ports UART_0_txen]
set_property SLEW SLOW [get_ports UART_0_txen]
set_property DRIVE 4 [get_ports UART_0_txen]

## Test PMOD constraints

set_property PACKAGE_PIN J11 [get_ports ptp_timer]
set_property IOSTANDARD LVCMOS33 [get_ports ptp_timer]
set_property SLEW SLOW [get_ports ptp_timer]
set_property DRIVE 4 [get_ports ptp_timer]

set_property PACKAGE_PIN H11 [get_ports p1_txs]
set_property IOSTANDARD LVCMOS33 [get_ports p1_txs]
set_property SLEW SLOW [get_ports p1_txs]
set_property DRIVE 4 [get_ports p1_txs]

set_property PACKAGE_PIN G10 [get_ports l1_rxs]
set_property IOSTANDARD LVCMOS33 [get_ports l1_rxs]
set_property SLEW SLOW [get_ports l1_rxs]
set_property DRIVE 4 [get_ports l1_rxs]

set_property PACKAGE_PIN F12 [get_ports p2_txb]
set_property IOSTANDARD LVCMOS33 [get_ports p2_txb]
set_property SLEW SLOW [get_ports p2_txb]
set_property DRIVE 4 [get_ports p2_txb]

set_property PACKAGE_PIN F11 [get_ports l2_rxb]
set_property IOSTANDARD LVCMOS33 [get_ports l2_rxb]
set_property SLEW SLOW [get_ports l2_rxb]
set_property DRIVE 4 [get_ports l2_rxb]

set_property PACKAGE_PIN K13 [get_ports p3_txx]
set_property IOSTANDARD LVCMOS33 [get_ports p3_txx]
set_property SLEW SLOW [get_ports p3_txx]
set_property DRIVE 4 [get_ports p3_txx]

set_property PACKAGE_PIN K12 [get_ports l3_rxx]
set_property IOSTANDARD LVCMOS33 [get_ports l3_rxx]
set_property SLEW SLOW [get_ports l3_rxx]
set_property DRIVE 4 [get_ports l3_rxx]

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
set_false_path -from [get_clocks clk_out1_kr260_tsn_rs485pmod_clk_wiz_0_0] -to [get_clocks clk_out2_kr260_tsn_rs485pmod_clk_wiz_0_0] 
set_false_path -from [get_clocks clk_out2_kr260_tsn_rs485pmod_clk_wiz_0_0] -to [get_clocks clk_out1_kr260_tsn_rs485pmod_clk_wiz_0_0] 
set_false_path -from [get_clocks clk_out2_kr260_tsn_rs485pmod_clk_wiz_0_0] -to [get_clocks clk_out4_kr260_tsn_rs485pmod_clk_wiz_0_0] 
set_false_path -from [get_clocks clk_out4_kr260_tsn_rs485pmod_clk_wiz_0_0] -to [get_clocks clk_out2_kr260_tsn_rs485pmod_clk_wiz_0_0] 

set_property DELAY_VALUE 0 [get_cells -hier -filter {NAME =~ */rgmii_interface/delay_rgmii_tx_clk}]
set_property DELAY_VALUE 0 [get_cells -hier -filter {NAME =~ */rgmii_interface/delay_rgmii_rx*}]
set_property DELAY_VALUE 0 [get_cells -hier -filter {NAME =~ */rgmii_interface/rxdata_bus[*].delay_rgmii_rx*}]


## Refclk frequency
set_property REFCLK_FREQUENCY 300.000 [get_cells -hier -filter {NAME =~ *delay_rgmii_tx*}]
set_clock_groups -asynchronous -group [get_clocks CLK_IN_gem]
set_clock_groups -asynchronous -group [get_clocks clk_out1_kr260_tsn_rs485pmod_clk_wiz_0*]
set_clock_groups -asynchronous -group [get_clocks clk_out2_kr260_tsn_rs485pmod_clk_wiz_0*]
set_clock_groups -asynchronous -group [get_clocks clk_out3_kr260_tsn_rs485pmod_clk_wiz_0*]
set_clock_groups -asynchronous -group [get_clocks clk_out4_kr260_tsn_rs485pmod_clk_wiz_0*]
set_clock_groups -asynchronous -group [get_clocks rgmii_rxc]
set_clock_groups -asynchronous -group [get_clocks rgmii2_rxc]
set_clock_groups -asynchronous -group [get_clocks clk_pl_0]
set_clock_groups -asynchronous -group [get_clocks kr260_tsn*_i/my_tsn_ip/inst/tsn_temac_1/inst_rgmii_tx_clk]
set_clock_groups -asynchronous -group [get_clocks kr260_tsn*_i/my_tsn_ip/inst/tsn_temac_2/inst_rgmii_tx_clk]



