# (C) Copyright 2020 - 2021 Xilinx, Inc.
# SPDX-License-Identifier: Apache-2.0

#MIPI ISP
set_property DIFF_TERM_ADV TERM_100 [get_ports {mipi_phy_if_isp_clk_p}]
set_property DIFF_TERM_ADV TERM_100 [get_ports {mipi_phy_if_isp_clk_n}] 
set_property DIFF_TERM_ADV TERM_100 [get_ports {mipi_phy_if_isp_data_p[*]}]
set_property DIFF_TERM_ADV TERM_100 [get_ports {mipi_phy_if_isp_data_n[*]}]

#MIPI IAS
set_property DIFF_TERM_ADV TERM_100 [get_ports {mipi_phy_if_ias_clk_p}]
set_property DIFF_TERM_ADV TERM_100 [get_ports {mipi_phy_if_ias_clk_n}] 
set_property DIFF_TERM_ADV TERM_100 [get_ports {mipi_phy_if_ias_data_p[*]}]
set_property DIFF_TERM_ADV TERM_100 [get_ports {mipi_phy_if_ias_data_n[*]}]

#MIPI Raspi
set_property DIFF_TERM_ADV TERM_100 [get_ports {mipi_phy_if_raspi_clk_p}]
set_property DIFF_TERM_ADV TERM_100 [get_ports {mipi_phy_if_raspi_clk_n}] 
set_property DIFF_TERM_ADV TERM_100 [get_ports {mipi_phy_if_raspi_data_p[*]}]
set_property DIFF_TERM_ADV TERM_100 [get_ports {mipi_phy_if_raspi_data_n[*]}]


#GPIO
#ISP AP1302_RST_B HDA02
set_property PACKAGE_PIN J11 [get_ports {GPIO_tri_o[0]}]
#ISP AP1302_STANDBY HDA03
set_property PACKAGE_PIN J10 [get_ports {GPIO_tri_o[1]}]
#IAS_XSHUTDOWN_B HDA04
set_property PACKAGE_PIN K13 [get_ports {GPIO_tri_o[2]}]
#Raspi Enable HDA09
set_property PACKAGE_PIN F11 [get_ports {GPIO_tri_o[3]}]
set_property IOSTANDARD LVCMOS33 [get_ports {GPIO_tri_o[*]}]
set_property SLEW SLOW [get_ports {GPIO_tri_o[*]}]
set_property DRIVE 4 [get_ports {GPIO_tri_o[*]}]

#Fan Speed Enable
set_property PACKAGE_PIN A12 [get_ports {fan_en_b}]
set_property IOSTANDARD LVCMOS33 [get_ports {fan_en_b}]
set_property SLEW SLOW [get_ports {fan_en_b}]
set_property DRIVE 4 [get_ports {fan_en_b}]


#I2C signals --> I2C switch 0--> ISP AP1302 + Sensor AR1335
#I2C signals --> I2C switch 1--> Sensor AR1335
#I2C signals --> I2C switch 2--> Raspi Camera
set_property PACKAGE_PIN G11 [get_ports iic_scl_io]
set_property PACKAGE_PIN F10 [get_ports iic_sda_io]
set_property IOSTANDARD LVCMOS33 [get_ports iic_*]
set_property SLEW SLOW [get_ports iic_*]
set_property DRIVE 4 [get_ports iic_*]


#Digilent PMOD pins  
# output - HDA11, HDA12, HDA13, HDA14 
set_property PACKAGE_PIN H12 [get_ports PMOD_OUT_tri_o[0]]
set_property PACKAGE_PIN E10 [get_ports PMOD_OUT_tri_o[1]]
set_property PACKAGE_PIN D10 [get_ports PMOD_OUT_tri_o[2]]
set_property PACKAGE_PIN C11 [get_ports PMOD_OUT_tri_o[3]]
set_property IOSTANDARD LVCMOS33 [get_ports PMOD_OUT_tri_o[*]]
set_property SLEW SLOW [get_ports PMOD_OUT_tri_o[*]]
set_property DRIVE 4 [get_ports PMOD_OUT_tri_o[*]]

#input - HDA15, HDA16, HDA17, HDA18 
set_property PACKAGE_PIN B10 [get_ports PMOD_IN_tri_i[0]]
set_property PACKAGE_PIN E12 [get_ports PMOD_IN_tri_i[1]]
set_property PACKAGE_PIN D11 [get_ports PMOD_IN_tri_i[2]]
set_property PACKAGE_PIN B11 [get_ports PMOD_IN_tri_i[3]]
set_property IOSTANDARD LVCMOS33 [get_ports PMOD_IN_tri_i[*]]
set_property SLEW SLOW [get_ports PMOD_IN_tri_i[*]]
set_property DRIVE 4 [get_ports PMOD_IN_tri_i[*]]

set_property BITSTREAM.CONFIG.OVERTEMPSHUTDOWN ENABLE [current_design]
