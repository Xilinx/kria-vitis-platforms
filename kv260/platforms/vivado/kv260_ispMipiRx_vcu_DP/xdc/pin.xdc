# (C) Copyright 2020 - 2021 Xilinx, Inc.
# SPDX-License-Identifier: Apache-2.0

#MIPI
set_property DIFF_TERM_ADV TERM_100 [get_ports {mipi_phy_if_clk_p}]
set_property DIFF_TERM_ADV TERM_100 [get_ports {mipi_phy_if_clk_n}]
set_property DIFF_TERM_ADV TERM_100 [get_ports {mipi_phy_if_data_p[*]}]
set_property DIFF_TERM_ADV TERM_100 [get_ports {mipi_phy_if_data_n[*]}]


#GPIO
#ISP AP1302_RST_B HDA02
set_property PACKAGE_PIN J11 [get_ports {ap1302_rst_b}]
set_property IOSTANDARD LVCMOS33 [get_ports {ap1302_rst_b}]
set_property SLEW SLOW [get_ports {ap1302_rst_b}]
set_property DRIVE 4 [get_ports {ap1302_rst_b}]

#ISP AP1302_STANDBY HDA03
set_property PACKAGE_PIN J10 [get_ports {ap1302_standby}]
set_property IOSTANDARD LVCMOS33 [get_ports {ap1302_standby}]
set_property SLEW SLOW [get_ports {ap1302_standby}]
set_property DRIVE 4 [get_ports {ap1302_standby}]

#Fan Speed Enable
set_property PACKAGE_PIN A12 [get_ports {fan_en_b}]
set_property IOSTANDARD LVCMOS33 [get_ports {fan_en_b}]
set_property SLEW SLOW [get_ports {fan_en_b}]
set_property DRIVE 4 [get_ports {fan_en_b}]


#I2C signals --> I2C switch 0--> ISP AP1302 + Sensor AR1335
set_property PACKAGE_PIN G11 [get_ports iic_scl_io]
set_property PACKAGE_PIN F10 [get_ports iic_sda_io]
set_property IOSTANDARD LVCMOS33 [get_ports iic_*]
set_property SLEW SLOW [get_ports iic_*]
set_property DRIVE 4 [get_ports iic_*]


#Digilent PMOD pins
set_property PACKAGE_PIN H12 [get_ports mclk_out_tx]
set_property IOSTANDARD LVCMOS33 [get_ports mclk_out_tx]
set_property SLEW SLOW [get_ports mclk_out_tx]
set_property DRIVE 4 [get_ports mclk_out_tx]

set_property PACKAGE_PIN E10 [get_ports lrclk_tx]
set_property IOSTANDARD LVCMOS33 [get_ports lrclk_tx]
set_property SLEW SLOW [get_ports lrclk_tx]
set_property DRIVE 4 [get_ports lrclk_tx]

set_property PACKAGE_PIN D10 [get_ports sclk_tx]
set_property IOSTANDARD LVCMOS33 [get_ports sclk_tx]
set_property SLEW SLOW [get_ports sclk_tx]
set_property DRIVE 4 [get_ports sclk_tx]

set_property PACKAGE_PIN C11 [get_ports sdata_tx]
set_property IOSTANDARD LVCMOS33 [get_ports sdata_tx]
set_property SLEW SLOW [get_ports sdata_tx]
set_property DRIVE 4 [get_ports sdata_tx]

#
##input side
set_property PACKAGE_PIN B10 [get_ports mclk_out_rx]
set_property IOSTANDARD LVCMOS33 [get_ports mclk_out_rx]
set_property SLEW SLOW [get_ports mclk_out_rx]
set_property DRIVE 4 [get_ports mclk_out_rx]

set_property PACKAGE_PIN E12 [get_ports lrclk_rx]
set_property IOSTANDARD LVCMOS33 [get_ports lrclk_rx]
set_property SLEW SLOW [get_ports lrclk_rx]
set_property DRIVE 4 [get_ports lrclk_rx]

set_property PACKAGE_PIN D11 [get_ports sclk_rx]
set_property IOSTANDARD LVCMOS33 [get_ports sclk_rx]
set_property SLEW SLOW [get_ports sclk_rx]
set_property DRIVE 4 [get_ports sclk_rx]

set_property PACKAGE_PIN B11 [get_ports sdata_rx]
set_property IOSTANDARD LVCMOS33 [get_ports sdata_rx]

set_property BITSTREAM.CONFIG.OVERTEMPSHUTDOWN ENABLE [current_design]
