# Copyright (C) 2022, Advanced Micro Devices, Inc.
# SPDX-License-Identifier: Apache-2.0

#------------------------------------------------------------------
# Bitstream properties
#------------------------------------------------------------------
set_property BITSTREAM.CONFIG.UNUSEDPIN PULLNONE [current_design]
set_property BITSTREAM.GENERAL.COMPRESS TRUE [current_design]

#------------------------------------------------------------------
# Pin Constraints
#------------------------------------------------------------------

## MDIO constraints
## Port 1
set_property PACKAGE_PIN L1 [get_ports gem1_mdio_mdio_io]
set_property IOSTANDARD LVCMOS18 [get_ports gem1_mdio_mdio_io]
set_property SLEW SLOW [get_ports gem1_mdio_mdio_io]
set_property DRIVE 4 [get_ports gem1_mdio_mdio_io]

set_property PACKAGE_PIN K2 [get_ports gem1_mdio_mdc]
set_property IOSTANDARD LVCMOS18 [get_ports gem1_mdio_mdc]
set_property SLEW SLOW [get_ports gem1_mdio_mdc]
set_property DRIVE 4 [get_ports gem1_mdio_mdc]

set_property PACKAGE_PIN M1 [get_ports {gem1_phy_rst_n[0]}]
set_property IOSTANDARD LVCMOS18 [get_ports {gem1_phy_rst_n[0]}]
set_property SLEW SLOW [get_ports {gem1_phy_rst_n[0]}]
set_property DRIVE 4 [get_ports {gem1_phy_rst_n[0]}]

# Port 2
set_property PACKAGE_PIN V5 [get_ports gem2_mdio_mdio_io]
set_property IOSTANDARD LVCMOS18 [get_ports gem2_mdio_mdio_io]
set_property SLEW SLOW [get_ports gem2_mdio_mdio_io]
set_property DRIVE 4 [get_ports gem2_mdio_mdio_io]

set_property PACKAGE_PIN V6 [get_ports gem2_mdio_mdc]
set_property IOSTANDARD LVCMOS18 [get_ports gem2_mdio_mdc]
set_property SLEW SLOW [get_ports gem2_mdio_mdc]
set_property DRIVE 4 [get_ports gem2_mdio_mdc]

set_property PACKAGE_PIN D4 [get_ports {gem2_phy_rst_n[0]}]
set_property IOSTANDARD LVCMOS18 [get_ports {gem2_phy_rst_n[0]}]
set_property SLEW SLOW [get_ports {gem2_phy_rst_n[0]}]
set_property DRIVE 4 [get_ports {gem2_phy_rst_n[0]}]

## RGMII constraints

# GEM 2 (Port 2)
set_property PACKAGE_PIN G6 [get_ports gem2_rgmii_tx_ctl]
set_property IOSTANDARD LVCMOS18 [get_ports gem2_rgmii_tx_ctl]
set_property SLEW SLOW [get_ports gem2_rgmii_tx_ctl]
set_property DRIVE 4 [get_ports gem2_rgmii_tx_ctl]

set_property PACKAGE_PIN F7 [get_ports gem2_rgmii_txc]
set_property IOSTANDARD LVCMOS18 [get_ports gem2_rgmii_txc]
set_property SLEW SLOW [get_ports gem2_rgmii_txc]
set_property DRIVE 4 [get_ports gem2_rgmii_txc]

set_property PACKAGE_PIN H2 [get_ports {gem2_rgmii_td[0]}]
set_property IOSTANDARD LVCMOS18 [get_ports {gem2_rgmii_td[0]}]
set_property SLEW SLOW [get_ports {gem2_rgmii_td[0]}]
set_property DRIVE 4 [get_ports {gem2_rgmii_td[0]}]

set_property PACKAGE_PIN H1 [get_ports {gem2_rgmii_td[1]}]
set_property IOSTANDARD LVCMOS18 [get_ports {gem2_rgmii_td[1]}]
set_property SLEW SLOW [get_ports {gem2_rgmii_td[1]}]
set_property DRIVE 4 [get_ports {gem2_rgmii_td[1]}]

set_property PACKAGE_PIN G2 [get_ports {gem2_rgmii_td[2]}]
set_property IOSTANDARD LVCMOS18 [get_ports {gem2_rgmii_td[2]}]
set_property SLEW SLOW [get_ports {gem2_rgmii_td[2]}]
set_property DRIVE 4 [get_ports {gem2_rgmii_td[2]}]

set_property PACKAGE_PIN G1 [get_ports {gem2_rgmii_td[3]}]
set_property IOSTANDARD LVCMOS18 [get_ports {gem2_rgmii_td[3]}]
set_property SLEW SLOW [get_ports {gem2_rgmii_td[3]}]
set_property DRIVE 4 [get_ports {gem2_rgmii_td[3]}]

set_property PACKAGE_PIN F1 [get_ports gem2_rgmii_rx_ctl]
set_property IOSTANDARD LVCMOS18 [get_ports gem2_rgmii_rx_ctl]

set_property PACKAGE_PIN A4 [get_ports gem2_rgmii_rxc]
set_property IOSTANDARD LVCMOS18 [get_ports gem2_rgmii_rxc]

set_property PACKAGE_PIN C1 [get_ports {gem2_rgmii_rd[0]}]
set_property IOSTANDARD LVCMOS18 [get_ports {gem2_rgmii_rd[0]}]

set_property PACKAGE_PIN D1 [get_ports {gem2_rgmii_rd[1]}]
set_property IOSTANDARD LVCMOS18 [get_ports {gem2_rgmii_rd[1]}]

set_property PACKAGE_PIN D2 [get_ports {gem2_rgmii_rd[2]}]
set_property IOSTANDARD LVCMOS18 [get_ports {gem2_rgmii_rd[2]}]

set_property PACKAGE_PIN E2 [get_ports {gem2_rgmii_rd[3]}]
set_property IOSTANDARD LVCMOS18 [get_ports {gem2_rgmii_rd[3]}]


# GEM 1
set_property PACKAGE_PIN K6 [get_ports gem1_rgmii_tx_ctl]
set_property IOSTANDARD LVCMOS18 [get_ports gem1_rgmii_tx_ctl]
set_property SLEW SLOW [get_ports gem1_rgmii_tx_ctl]
set_property DRIVE 4 [get_ports gem1_rgmii_tx_ctl]

set_property PACKAGE_PIN K7 [get_ports gem1_rgmii_txc]
set_property IOSTANDARD LVCMOS18 [get_ports gem1_rgmii_txc]
set_property SLEW SLOW [get_ports gem1_rgmii_txc]
set_property DRIVE 4 [get_ports gem1_rgmii_txc]

set_property PACKAGE_PIN T2 [get_ports {gem1_rgmii_td[0]}]
set_property IOSTANDARD LVCMOS18 [get_ports {gem1_rgmii_td[0]}]
set_property SLEW SLOW [get_ports {gem1_rgmii_td[0]}]
set_property DRIVE 4 [get_ports {gem1_rgmii_td[0]}]

set_property PACKAGE_PIN U2 [get_ports {gem1_rgmii_td[1]}]
set_property IOSTANDARD LVCMOS18 [get_ports {gem1_rgmii_td[1]}]
set_property SLEW SLOW [get_ports {gem1_rgmii_td[1]}]
set_property DRIVE 4 [get_ports {gem1_rgmii_td[1]}]

set_property PACKAGE_PIN V4 [get_ports {gem1_rgmii_td[2]}]
set_property IOSTANDARD LVCMOS18 [get_ports {gem1_rgmii_td[2]}]
set_property SLEW SLOW [get_ports {gem1_rgmii_td[2]}]
set_property DRIVE 4 [get_ports {gem1_rgmii_td[2]}]

set_property PACKAGE_PIN V3 [get_ports {gem1_rgmii_td[3]}]
set_property IOSTANDARD LVCMOS18 [get_ports {gem1_rgmii_td[3]}]
set_property SLEW SLOW [get_ports {gem1_rgmii_td[3]}]
set_property DRIVE 4 [get_ports {gem1_rgmii_td[3]}]

set_property PACKAGE_PIN P1 [get_ports gem1_rgmii_rx_ctl]
set_property IOSTANDARD LVCMOS18 [get_ports gem1_rgmii_rx_ctl]

set_property PACKAGE_PIN P2 [get_ports gem1_rgmii_rxc]
set_property IOSTANDARD LVCMOS18 [get_ports gem1_rgmii_rxc]


set_property PACKAGE_PIN R1 [get_ports {gem1_rgmii_rd[0]}]
set_property IOSTANDARD LVCMOS18 [get_ports {gem1_rgmii_rd[0]}]

set_property PACKAGE_PIN T1 [get_ports {gem1_rgmii_rd[1]}]
set_property IOSTANDARD LVCMOS18 [get_ports {gem1_rgmii_rd[1]}]

set_property PACKAGE_PIN N2 [get_ports {gem1_rgmii_rd[2]}]
set_property IOSTANDARD LVCMOS18 [get_ports {gem1_rgmii_rd[2]}]

set_property PACKAGE_PIN N1 [get_ports {gem1_rgmii_rd[3]}]
set_property IOSTANDARD LVCMOS18 [get_ports {gem1_rgmii_rd[3]}]

## Test PMOD constraints

set_property PACKAGE_PIN D9 [get_ports {GPIO_tri_io[0]}]
set_property IOSTANDARD LVCMOS33 [get_ports {GPIO_tri_io[0]}]
set_property SLEW SLOW [get_ports {GPIO_tri_io[0]}]
set_property DRIVE 4 [get_ports {GPIO_tri_io[0]}]

set_property PACKAGE_PIN D13 [get_ports {GPIO_tri_io[1]}]
set_property IOSTANDARD LVCMOS33 [get_ports {GPIO_tri_io[1]}]
set_property SLEW SLOW [get_ports {GPIO_tri_io[1]}]
set_property DRIVE 4 [get_ports {GPIO_tri_io[1]}]

set_property PACKAGE_PIN C13 [get_ports {GPIO_tri_io[2]}]
set_property IOSTANDARD LVCMOS33 [get_ports {GPIO_tri_io[2]}]
set_property SLEW SLOW [get_ports {GPIO_tri_io[2]}]
set_property DRIVE 4 [get_ports {GPIO_tri_io[2]}]

set_property PACKAGE_PIN C10 [get_ports {GPIO_tri_io[3]}]
set_property IOSTANDARD LVCMOS33 [get_ports {GPIO_tri_io[3]}]
set_property SLEW SLOW [get_ports {GPIO_tri_io[3]}]
set_property DRIVE 4 [get_ports {GPIO_tri_io[3]}]

set_property PACKAGE_PIN E9 [get_ports {GPIO_tri_io[4]}]
set_property IOSTANDARD LVCMOS33 [get_ports {GPIO_tri_io[4]}]
set_property SLEW SLOW [get_ports {GPIO_tri_io[4]}]
set_property DRIVE 4 [get_ports {GPIO_tri_io[4]}]

set_property PACKAGE_PIN F13 [get_ports {GPIO_tri_io[5]}]
set_property IOSTANDARD LVCMOS33 [get_ports {GPIO_tri_io[5]}]
set_property SLEW SLOW [get_ports {GPIO_tri_io[5]}]
set_property DRIVE 4 [get_ports {GPIO_tri_io[5]}]

set_property PACKAGE_PIN E12 [get_ports {GPIO_tri_io[6]}]
set_property IOSTANDARD LVCMOS33 [get_ports {GPIO_tri_io[6]}]
set_property SLEW SLOW [get_ports {GPIO_tri_io[6]}]
set_property DRIVE 4 [get_ports {GPIO_tri_io[6]}]

set_property PACKAGE_PIN D10 [get_ports {GPIO_tri_io[7]}]
set_property IOSTANDARD LVCMOS33 [get_ports {GPIO_tri_io[7]}]
set_property SLEW SLOW [get_ports {GPIO_tri_io[7]}]
set_property DRIVE 4 [get_ports {GPIO_tri_io[7]}]

##Fan Speed Enable
set_property PACKAGE_PIN G12 [get_ports {fan_en_b[0]}]
set_property IOSTANDARD LVCMOS33 [get_ports {fan_en_b[0]}]
set_property SLEW SLOW [get_ports {fan_en_b[0]}]
set_property DRIVE 4 [get_ports {fan_en_b[0]}]

## TQ Constraints 
set_property PACKAGE_PIN J4 [get_ports TQ_SCLK]
set_property IOSTANDARD LVCMOS18 [get_ports TQ_SCLK]
set_property SLEW SLOW [get_ports TQ_SCLK]
set_property DRIVE 4 [get_ports TQ_SCLK]

set_property PACKAGE_PIN L6 [get_ports {TQ_CSn[0]}]
set_property IOSTANDARD LVCMOS18 [get_ports {TQ_CSn[0]}]
set_property SLEW SLOW [get_ports {TQ_CSn[0]}]
set_property DRIVE 4 [get_ports {TQ_CSn[0]}]

set_property PACKAGE_PIN K4 [get_ports TQ_SDI]
set_property IOSTANDARD LVCMOS18 [get_ports TQ_SDI]
set_property SLEW SLOW [get_ports TQ_SDI]
set_property DRIVE 4 [get_ports TQ_SDI]

set_property PACKAGE_PIN F4 [get_ports TQ_SDO]
set_property IOSTANDARD LVCMOS18 [get_ports TQ_SDO]

## ADC Constraints
set_property PACKAGE_PIN K1 [get_ports {dc_link_adc_cs_n[0]}]
set_property IOSTANDARD LVCMOS18 [get_ports {dc_link_adc_cs_n[0]}]
set_property SLEW SLOW [get_ports {dc_link_adc_cs_n[0]}]
set_property DRIVE 4 [get_ports {dc_link_adc_cs_n[0]}]

set_property PACKAGE_PIN H8 [get_ports dc_link_adc_sclk]
set_property IOSTANDARD LVCMOS18 [get_ports dc_link_adc_sclk]
set_property SLEW SLOW [get_ports dc_link_adc_sclk]
set_property DRIVE 4 [get_ports dc_link_adc_sclk]

set_property PACKAGE_PIN J7 [get_ports dc_link_data_v]
set_property IOSTANDARD LVCMOS18 [get_ports dc_link_data_v]

set_property PACKAGE_PIN J1 [get_ports dc_link_data_i]
set_property IOSTANDARD LVCMOS18 [get_ports dc_link_data_i]

set_property PACKAGE_PIN U5 [get_ports {motor_adc_cs_n[0]}]
set_property IOSTANDARD LVCMOS18 [get_ports {motor_adc_cs_n[0]}]
set_property SLEW SLOW [get_ports {motor_adc_cs_n[0]}]
set_property DRIVE 4 [get_ports {motor_adc_cs_n[0]}]

set_property PACKAGE_PIN N6 [get_ports motor_adc_sclk]
set_property IOSTANDARD LVCMOS18 [get_ports motor_adc_sclk]
set_property SLEW SLOW [get_ports motor_adc_sclk]
set_property DRIVE 4 [get_ports motor_adc_sclk]

set_property PACKAGE_PIN P6 [get_ports motor_pa_data_v]
set_property IOSTANDARD LVCMOS18 [get_ports motor_pa_data_v]

set_property PACKAGE_PIN N4 [get_ports motor_pa_data_i]
set_property IOSTANDARD LVCMOS18 [get_ports motor_pa_data_i]

set_property PACKAGE_PIN P4 [get_ports motor_pb_data_v]
set_property IOSTANDARD LVCMOS18 [get_ports motor_pb_data_v]

set_property PACKAGE_PIN M7 [get_ports motor_pb_data_i]
set_property IOSTANDARD LVCMOS18 [get_ports motor_pb_data_i]

set_property PACKAGE_PIN N7 [get_ports motor_pc_data_v]
set_property IOSTANDARD LVCMOS18 [get_ports motor_pc_data_v]

set_property PACKAGE_PIN T6 [get_ports motor_pc_data_i]
set_property IOSTANDARD LVCMOS18 [get_ports motor_pc_data_i]

## One wire
set_property PACKAGE_PIN H13 [get_ports {one_wire[0]}]
set_property IOSTANDARD LVCMOS33 [get_ports {one_wire[0]}]
set_property SLEW SLOW [get_ports {one_wire[0]}]
set_property DRIVE 4 [get_ports {one_wire[0]}]

## Gate Drive (check iostandards)
set_property PACKAGE_PIN B9 [get_ports {brake_cntrl[0]}]
set_property IOSTANDARD LVCMOS33 [get_ports {brake_cntrl[0]}]
set_property SLEW SLOW [get_ports {brake_cntrl[0]}]
set_property DRIVE 4 [get_ports {brake_cntrl[0]}]

set_property PACKAGE_PIN G9 [get_ports {gate_drive_en[0]}]
set_property IOSTANDARD LVCMOS33 [get_ports {gate_drive_en[0]}]
set_property SLEW SLOW [get_ports {gate_drive_en[0]}]
set_property DRIVE 4 [get_ports {gate_drive_en[0]}]

set_property PACKAGE_PIN H10 [get_ports gate_drive_phase_a]
set_property IOSTANDARD LVCMOS33 [get_ports gate_drive_phase_a]
set_property SLEW SLOW [get_ports gate_drive_phase_a]
set_property DRIVE 4 [get_ports gate_drive_phase_a]

set_property PACKAGE_PIN H9 [get_ports gate_drive_phase_b]
set_property IOSTANDARD LVCMOS33 [get_ports gate_drive_phase_b]
set_property SLEW SLOW [get_ports gate_drive_phase_b]
set_property DRIVE 4 [get_ports gate_drive_phase_b]

set_property PACKAGE_PIN G10 [get_ports gate_drive_phase_c]
set_property IOSTANDARD LVCMOS33 [get_ports gate_drive_phase_c]
set_property SLEW SLOW [get_ports gate_drive_phase_c]
set_property DRIVE 4 [get_ports gate_drive_phase_c]

## QEI_SE
set_property PACKAGE_PIN F10 [get_ports qei_se_A]
set_property IOSTANDARD LVCMOS33 [get_ports qei_se_A]

set_property PACKAGE_PIN F9 [get_ports qei_se_B]
set_property IOSTANDARD LVCMOS33 [get_ports qei_se_B]

set_property PACKAGE_PIN E10 [get_ports qei_se_I]
set_property IOSTANDARD LVCMOS33 [get_ports qei_se_I]

## Clock inputs from board

# GEM clks
set_property PACKAGE_PIN L2 [get_ports gem1_clk_in]
set_property IOSTANDARD LVCMOS18 [get_ports gem1_clk_in]
set_property PACKAGE_PIN C4 [get_ports gem2_clk_in]
set_property IOSTANDARD LVCMOS18 [get_ports gem2_clk_in]

#------------------------------------------------------------------
# Timing Constraints
#------------------------------------------------------------------


set_property DELAY_VALUE 0 [get_cells -hier -filter {NAME =~ */rgmii_interface/delay_rgmii_tx_clk}]
set_property DELAY_VALUE 0 [get_cells -hier -filter {NAME =~ */rgmii_interface/delay_rgmii_rx*}]
set_property DELAY_VALUE 0 [get_cells -hier -filter {NAME =~ */rgmii_interface/rxdata_bus[*].delay_rgmii_rx*}]

## Refclk frequency
set_property REFCLK_FREQUENCY 333.33333 [get_cells -hier -filter {NAME =~ *delay_rgmii_tx*}]
set_property REFCLK_FREQUENCY 333.33333 [get_cells -hier -filter {NAME =~ *delay_rgmii_rx_ctl}]
set_property REFCLK_FREQUENCY 333.33333 [get_cells -hier -filter {NAME =~ *delay_rgmii_rxd}]

## False paths
set_clock_groups -asynchronous -group [get_clocks gem1_clk_in]
set_clock_groups -asynchronous -group [get_clocks gem2_clk_in]
set_clock_groups -asynchronous -group [get_clocks gem1_rgmii_rxc]
set_clock_groups -asynchronous -group [get_clocks gem2_rgmii_rxc]
set_clock_groups -asynchronous -group [get_clocks clk_pl_0]
set_clock_groups -asynchronous -group [get_clocks clk_pl_1]
set_clock_groups -asynchronous -group [get_clocks kd240_bist_i/ethernet_subsystem/axi_ethernet_0/inst/mac/inst_rgmii_tx_clk]
set_clock_groups -asynchronous -group [get_clocks kd240_bist_i/ethernet_subsystem/axi_ethernet_1/inst/mac/inst_rgmii_tx_clk]
set_clock_groups -asynchronous -group [get_clocks clk_out1_kd240_bist_clk_wiz_0]
set_clock_groups -asynchronous -group [get_clocks clk_out2_kd240_bist_clk_wiz_0]
set_clock_groups -asynchronous -group [get_clocks clk_out1_kd240_bist_clk_wiz_1_0]


