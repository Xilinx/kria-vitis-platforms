# (C) Copyright 2020 - 2022 Xilinx, Inc.
# SPDX-License-Identifier: Apache-2.0

#MIPI ISP
# In Vivado 2026.1 the MIPI CSI-2 RX IP no longer accepts LOC params; use XDC.
set_property PACKAGE_PIN G1 [get_ports {mipi_phy_if_isp_clk_p}]
set_property PACKAGE_PIN F1 [get_ports {mipi_phy_if_isp_clk_n}]
set_property PACKAGE_PIN E1 [get_ports {mipi_phy_if_isp_data_p[0]}]
set_property PACKAGE_PIN F2 [get_ports {mipi_phy_if_isp_data_p[1]}]
set_property PACKAGE_PIN G3 [get_ports {mipi_phy_if_isp_data_p[2]}]
set_property PACKAGE_PIN E4 [get_ports {mipi_phy_if_isp_data_p[3]}]
set_property PACKAGE_PIN D1 [get_ports {mipi_phy_if_isp_data_n[0]}]
set_property PACKAGE_PIN E2 [get_ports {mipi_phy_if_isp_data_n[1]}]
set_property PACKAGE_PIN F3 [get_ports {mipi_phy_if_isp_data_n[2]}]
set_property PACKAGE_PIN E3 [get_ports {mipi_phy_if_isp_data_n[3]}]
set_property DIFF_TERM_ADV TERM_100 [get_ports {mipi_phy_if_isp_clk_p}]
set_property DIFF_TERM_ADV TERM_100 [get_ports {mipi_phy_if_isp_clk_n}]
set_property DIFF_TERM_ADV TERM_100 [get_ports {mipi_phy_if_isp_data_p[*]}]
set_property DIFF_TERM_ADV TERM_100 [get_ports {mipi_phy_if_isp_data_n[*]}]

#MIPI Raspi (som240_1_connector_mipi_csi_raspi)
set_property PACKAGE_PIN D7 [get_ports {mipi_phy_if_raspi_clk_p}]
set_property PACKAGE_PIN D6 [get_ports {mipi_phy_if_raspi_clk_n}]
set_property PACKAGE_PIN E5 [get_ports {mipi_phy_if_raspi_data_p[0]}]
set_property PACKAGE_PIN G6 [get_ports {mipi_phy_if_raspi_data_p[1]}]
set_property PACKAGE_PIN D5 [get_ports {mipi_phy_if_raspi_data_n[0]}]
set_property PACKAGE_PIN F6 [get_ports {mipi_phy_if_raspi_data_n[1]}]
set_property DIFF_TERM_ADV TERM_100 [get_ports {mipi_phy_if_raspi_clk_p}]
set_property DIFF_TERM_ADV TERM_100 [get_ports {mipi_phy_if_raspi_clk_n}]
set_property DIFF_TERM_ADV TERM_100 [get_ports {mipi_phy_if_raspi_data_p[*]}]
set_property DIFF_TERM_ADV TERM_100 [get_ports {mipi_phy_if_raspi_data_n[*]}]

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

#Raspi Enable HDA09
set_property PACKAGE_PIN F11 [get_ports {imx_standby}]
set_property IOSTANDARD LVCMOS33 [get_ports {imx_standby}]
set_property SLEW SLOW [get_ports {imx_standby}]
set_property DRIVE 4 [get_ports {imx_standby}]


#I2C signals --> I2C switch 0--> ISP AP1302 + Sensor AR1335
#I2C signals --> I2C switch 1--> Sensor AR1335
#I2C signals --> I2C switch 2--> Raspi Camera
set_property PACKAGE_PIN G11 [get_ports iic_scl_io]
set_property PACKAGE_PIN F10 [get_ports iic_sda_io]
set_property IOSTANDARD LVCMOS33 [get_ports iic_*]


set_property BITSTREAM.CONFIG.OVERTEMPSHUTDOWN ENABLE [current_design]
