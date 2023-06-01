# Copyright (C) 2022, Advanced Micro Devices, Inc.
# SPDX-License-Identifier: Apache-2.0

#------------------------------------------------------------------
# Bitstream properties
#------------------------------------------------------------------
set_property BITSTREAM.GENERAL.COMPRESS TRUE [current_design]
set_property BITSTREAM.CONFIG.UNUSEDPIN PULLNONE [current_design]

#----------------------------------------------------------------------------
# Pin Constraints
#----------------------------------------------------------------------------

## MDIO constraints

#set_property PACKAGE_PIN L1 [get_ports mdio_io]
#set_property IOSTANDARD LVCMOS18 [get_ports mdio_io]
#set_property SLEW SLOW [get_ports mdio_io]
#set_property DRIVE 4 [get_ports mdio_io]
#
#set_property PACKAGE_PIN K2 [get_ports mdio_mdc]
#set_property IOSTANDARD LVCMOS18 [get_ports mdio_mdc]
#set_property SLEW SLOW [get_ports mdio_mdc]
#set_property DRIVE 4 [get_ports mdio_mdc]
#
#set_property PACKAGE_PIN M1 [get_ports {Phy_reset_n[0]}]
#set_property IOSTANDARD LVCMOS18 [get_ports {Phy_reset_n[0]}]
#set_property SLEW SLOW [get_ports {Phy_reset_n[0]}]
#set_property DRIVE 4 [get_ports {Phy_reset_n[0]}]
#
#set_property PACKAGE_PIN V5 [get_ports mdio2_io]
#set_property IOSTANDARD LVCMOS18 [get_ports mdio2_io]
#set_property SLEW SLOW [get_ports mdio2_io]
#set_property DRIVE 4 [get_ports mdio2_io]
#
#set_property PACKAGE_PIN V6 [get_ports mdio2_mdc]
#set_property IOSTANDARD LVCMOS18 [get_ports mdio2_mdc]
#set_property SLEW SLOW [get_ports mdio2_mdc]
#set_property DRIVE 4 [get_ports mdio2_mdc]
#
#set_property PACKAGE_PIN D4 [get_ports {Phy_reset_n[1]}]
#set_property IOSTANDARD LVCMOS18 [get_ports {Phy_reset_n[1]}]
#set_property SLEW SLOW [get_ports {Phy_reset_n[1]}]
#set_property DRIVE 4 [get_ports {Phy_reset_n[1]}]
#
### RGMII constraints
#
## PHY0
#set_property PACKAGE_PIN K6 [get_ports rgmii_tx_ctl]
#set_property IOSTANDARD LVCMOS18 [get_ports rgmii_tx_ctl]
#set_property SLEW SLOW [get_ports rgmii_tx_ctl]
#set_property DRIVE 4 [get_ports rgmii_tx_ctl]
#
#set_property PACKAGE_PIN K7 [get_ports rgmii_txc]
#set_property IOSTANDARD LVCMOS18 [get_ports rgmii_txc]
#set_property SLEW SLOW [get_ports rgmii_txc]
#set_property DRIVE 4 [get_ports rgmii_txc]
#
#set_property PACKAGE_PIN T2 [get_ports {rgmii_td[0]}]
#set_property IOSTANDARD LVCMOS18 [get_ports {rgmii_td[0]}]
#set_property SLEW SLOW [get_ports {rgmii_td[0]}]
#set_property DRIVE 4 [get_ports {rgmii_td[0]}]
#
#set_property PACKAGE_PIN U2 [get_ports {rgmii_td[1]}]
#set_property IOSTANDARD LVCMOS18 [get_ports {rgmii_td[1]}]
#set_property SLEW SLOW [get_ports {rgmii_td[1]}]
#set_property DRIVE 4 [get_ports {rgmii_td[1]}]
#
#set_property PACKAGE_PIN V4 [get_ports {rgmii_td[2]}]
#set_property IOSTANDARD LVCMOS18 [get_ports {rgmii_td[2]}]
#set_property SLEW SLOW [get_ports {rgmii_td[2]}]
#set_property DRIVE 4 [get_ports {rgmii_td[2]}]
#
#set_property PACKAGE_PIN V3 [get_ports {rgmii_td[3]}]
#set_property IOSTANDARD LVCMOS18 [get_ports {rgmii_td[3]}]
#set_property SLEW SLOW [get_ports {rgmii_td[3]}]
#set_property DRIVE 4 [get_ports {rgmii_td[3]}]
#
#set_property PACKAGE_PIN P1 [get_ports rgmii_rx_ctl]
#set_property IOSTANDARD LVCMOS18 [get_ports rgmii_rx_ctl]
#set_property SLEW SLOW [get_ports {rgmii_rx_ctl}]
#set_property DRIVE 4 [get_ports {rgmii_rx_ctl}]
#
#set_property PACKAGE_PIN P2 [get_ports rgmii_rxc]
#set_property IOSTANDARD LVCMOS18 [get_ports rgmii_rxc]
#set_property SLEW SLOW [get_ports {rgmii_rxc}]
#set_property DRIVE 4 [get_ports {rgmii_rxc}]
#
#set_property PACKAGE_PIN R1 [get_ports {rgmii_rd[0]}]
#set_property IOSTANDARD LVCMOS18 [get_ports {rgmii_rd[0]}]
#set_property SLEW SLOW [get_ports {rgmii_rd[0]}]
#set_property DRIVE 4 [get_ports {rgmii_rd[0]}]
#
#set_property PACKAGE_PIN T1 [get_ports {rgmii_rd[1]}]
#set_property IOSTANDARD LVCMOS18 [get_ports {rgmii_rd[1]}]
#set_property SLEW SLOW [get_ports {rgmii_rd[1]}]
#set_property DRIVE 4 [get_ports {rgmii_rd[1]}]
#
#set_property PACKAGE_PIN N2 [get_ports {rgmii_rd[2]}]
#set_property IOSTANDARD LVCMOS18 [get_ports {rgmii_rd[2]}]
#set_property SLEW SLOW [get_ports {rgmii_rd[2]}]
#set_property DRIVE 4 [get_ports {rgmii_rd[2]}]
#
#set_property PACKAGE_PIN N1 [get_ports {rgmii_rd[3]}]
#set_property IOSTANDARD LVCMOS18 [get_ports {rgmii_rd[3]}]
#set_property SLEW SLOW [get_ports {rgmii_rd[3]}]
#set_property DRIVE 4 [get_ports {rgmii_rd[3]}]
#
#
## PHY1
#set_property PACKAGE_PIN G6 [get_ports rgmii2_tx_ctl]
#set_property IOSTANDARD LVCMOS18 [get_ports rgmii2_tx_ctl]
#set_property SLEW SLOW [get_ports rgmii2_tx_ctl]
#set_property DRIVE 4 [get_ports rgmii2_tx_ctl]
#
#set_property PACKAGE_PIN F7 [get_ports rgmii2_txc]
#set_property IOSTANDARD LVCMOS18 [get_ports rgmii2_txc]
#set_property SLEW SLOW [get_ports rgmii2_txc]
#set_property DRIVE 4 [get_ports rgmii2_txc]
#
#set_property PACKAGE_PIN H2 [get_ports {rgmii2_td[0]}]
#set_property IOSTANDARD LVCMOS18 [get_ports {rgmii2_td[0]}]
#set_property SLEW SLOW [get_ports {rgmii2_td[0]}]
#set_property DRIVE 4 [get_ports {rgmii2_td[0]}]
#
#set_property PACKAGE_PIN H1 [get_ports {rgmii2_td[1]}]
#set_property IOSTANDARD LVCMOS18 [get_ports {rgmii2_td[1]}]
#set_property SLEW SLOW [get_ports {rgmii2_td[1]}]
#set_property DRIVE 4 [get_ports {rgmii2_td[1]}]
#
#set_property PACKAGE_PIN G2 [get_ports {rgmii2_td[2]}]
#set_property IOSTANDARD LVCMOS18 [get_ports {rgmii2_td[2]}]
#set_property SLEW SLOW [get_ports {rgmii2_td[2]}]
#set_property DRIVE 4 [get_ports {rgmii2_td[2]}]
#
#set_property PACKAGE_PIN G1 [get_ports {rgmii2_td[3]}]
#set_property IOSTANDARD LVCMOS18 [get_ports {rgmii2_td[3]}]
#set_property SLEW SLOW [get_ports {rgmii2_td[3]}]
#set_property DRIVE 4 [get_ports {rgmii2_td[3]}]
#
#set_property PACKAGE_PIN F1 [get_ports rgmii2_rx_ctl]
#set_property IOSTANDARD LVCMOS18 [get_ports rgmii2_rx_ctl]
#set_property SLEW SLOW [get_ports {rgmii2_rx_ctl}]
#set_property DRIVE 4 [get_ports {rgmii2_rx_ctl}]
#
#set_property PACKAGE_PIN A4 [get_ports rgmii2_rxc]
#set_property IOSTANDARD LVCMOS18 [get_ports rgmii2_rxc]
#set_property SLEW SLOW [get_ports {rgmii2_rxc}]
#set_property DRIVE 4 [get_ports {rgmii2_rxc}]
#
#
#set_property PACKAGE_PIN C1 [get_ports {rgmii2_rd[0]}]
#set_property IOSTANDARD LVCMOS18 [get_ports {rgmii2_rd[0]}]
#set_property SLEW SLOW [get_ports {rgmii2_rd[0]}]
#set_property DRIVE 4 [get_ports {rgmii2_rd[0]}]
#
#set_property PACKAGE_PIN D1 [get_ports {rgmii2_rd[1]}]
#set_property IOSTANDARD LVCMOS18 [get_ports {rgmii2_rd[1]}]
#set_property SLEW SLOW [get_ports {rgmii2_rd[1]}]
#set_property DRIVE 4 [get_ports {rgmii2_rd[1]}]
#
#set_property PACKAGE_PIN D2 [get_ports {rgmii2_rd[2]}]
#set_property IOSTANDARD LVCMOS18 [get_ports {rgmii2_rd[2]}]
#set_property SLEW SLOW [get_ports {rgmii2_rd[2]}]
#set_property DRIVE 4 [get_ports {rgmii2_rd[2]}]
#
#set_property PACKAGE_PIN E2 [get_ports {rgmii2_rd[3]}]
#set_property IOSTANDARD LVCMOS18 [get_ports {rgmii2_rd[3]}]
#set_property SLEW SLOW [get_ports {rgmii2_rd[3]}]
#set_property DRIVE 4 [get_ports {rgmii2_rd[3]}]
#
#
### Test PMOD constraints
#
#set_property PACKAGE_PIN E9 [get_ports ptp_timer]
#set_property IOSTANDARD LVCMOS33 [get_ports ptp_timer]
#set_property SLEW SLOW [get_ports ptp_timer]
#set_property DRIVE 4 [get_ports ptp_timer]
#
#set_property PACKAGE_PIN D9 [get_ports p1_txs]
#set_property IOSTANDARD LVCMOS33 [get_ports p1_txs]
#set_property SLEW SLOW [get_ports p1_txs]
#set_property DRIVE 4 [get_ports p1_txs]
#
#set_property PACKAGE_PIN D13 [get_ports l1_rxs]
#set_property IOSTANDARD LVCMOS33 [get_ports l1_rxs]
#set_property SLEW SLOW [get_ports l1_rxs]
#set_property DRIVE 4 [get_ports l1_rxs]
#
#set_property PACKAGE_PIN C13 [get_ports p2_txb]
#set_property IOSTANDARD LVCMOS33 [get_ports p2_txb]
#set_property SLEW SLOW [get_ports p2_txb]
#set_property DRIVE 4 [get_ports p2_txb]
#
#set_property PACKAGE_PIN C10 [get_ports l2_rxb]
#set_property IOSTANDARD LVCMOS33 [get_ports l2_rxb]
#set_property SLEW SLOW [get_ports l2_rxb]
#set_property DRIVE 4 [get_ports l2_rxb]

# ADC Sample test signals
set_property PACKAGE_PIN E12 [get_ports pwm_sync_phase_a]
set_property IOSTANDARD LVCMOS33 [get_ports pwm_sync_phase_a]
set_property SLEW SLOW [get_ports pwm_sync_phase_a]
set_property DRIVE 4 [get_ports pwm_sync_phase_a]

set_property PACKAGE_PIN D10 [get_ports foc_periodic_rate]
set_property IOSTANDARD LVCMOS33 [get_ports foc_periodic_rate]
set_property SLEW SLOW [get_ports foc_periodic_rate]
set_property DRIVE 4 [get_ports foc_periodic_rate]

set_property PACKAGE_PIN F13 [get_ports motor_adc_sample_cmd]
set_property IOSTANDARD LVCMOS33 [get_ports motor_adc_sample_cmd]
set_property SLEW SLOW [get_ports motor_adc_sample_cmd]
set_property DRIVE 4 [get_ports motor_adc_sample_cmd]

#Fan Speed Enable
set_property PACKAGE_PIN G12 [get_ports {fan_en_b}]
set_property IOSTANDARD LVCMOS33 [get_ports {fan_en_b}]
set_property SLEW SLOW [get_ports {fan_en_b}]
set_property DRIVE 4 [get_ports {fan_en_b}]

## Gate Drive

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

## QEI (Encoder)
set_property PACKAGE_PIN F10 [get_ports QEI_A]
set_property IOSTANDARD LVCMOS33 [get_ports QEI_A]

set_property PACKAGE_PIN F9 [get_ports QEI_B]
set_property IOSTANDARD LVCMOS33 [get_ports QEI_B]

set_property PACKAGE_PIN E10 [get_ports QEI_I]
set_property IOSTANDARD LVCMOS33 [get_ports QEI_I]

## ADC Constraints
# Chip Selects
set_property PACKAGE_PIN K1 [get_ports {dc_link_adc_cs_n[0]}]
set_property IOSTANDARD LVCMOS18 [get_ports {dc_link_adc_cs_n[0]}]
set_property SLEW SLOW [get_ports {dc_link_adc_cs_n[0]}]
set_property DRIVE 4 [get_ports {dc_link_adc_cs_n[0]}]

set_property PACKAGE_PIN U5 [get_ports {motor_adc_cs_n[0]}]
set_property IOSTANDARD LVCMOS18 [get_ports {motor_adc_cs_n[0]}]
set_property SLEW SLOW [get_ports {motor_adc_cs_n[0]}]
set_property DRIVE 4 [get_ports {motor_adc_cs_n[0]}]

# ADC output clocks
set_property PACKAGE_PIN H8 [get_ports dc_link_adc_sclk]
set_property IOSTANDARD LVCMOS18 [get_ports dc_link_adc_sclk]
set_property SLEW SLOW [get_ports dc_link_adc_sclk]
set_property DRIVE 4 [get_ports dc_link_adc_sclk]

set_property PACKAGE_PIN N6 [get_ports motor_adc_sclk]
set_property IOSTANDARD LVCMOS18 [get_ports motor_adc_sclk]
set_property SLEW SLOW [get_ports motor_adc_sclk]
set_property DRIVE 4 [get_ports motor_adc_sclk]

# Currents
set_property PACKAGE_PIN N4 [get_ports motor_pa_data_i]
set_property IOSTANDARD LVCMOS18 [get_ports motor_pa_data_i]

set_property PACKAGE_PIN M7 [get_ports motor_pb_data_i]
set_property IOSTANDARD LVCMOS18 [get_ports motor_pb_data_i]

set_property PACKAGE_PIN T6 [get_ports motor_pc_data_i]
set_property IOSTANDARD LVCMOS18 [get_ports motor_pc_data_i]

set_property PACKAGE_PIN J1 [get_ports dc_link_data_i]
set_property IOSTANDARD LVCMOS18 [get_ports dc_link_data_i]

# Volatges
set_property PACKAGE_PIN P6 [get_ports motor_pa_data_v]
set_property IOSTANDARD LVCMOS18 [get_ports motor_pa_data_v]

set_property PACKAGE_PIN P4 [get_ports motor_pb_data_v]
set_property IOSTANDARD LVCMOS18 [get_ports motor_pb_data_v]

set_property PACKAGE_PIN N7 [get_ports motor_pc_data_v]
set_property IOSTANDARD LVCMOS18 [get_ports motor_pc_data_v]

set_property PACKAGE_PIN J7 [get_ports dc_link_data_v]
set_property IOSTANDARD LVCMOS18 [get_ports dc_link_data_v]


## Clock inputs from board

# 25Mhz clock
set_property PACKAGE_PIN L2 [get_ports CLK_IN_gem]
set_property IOSTANDARD LVCMOS18 [get_ports CLK_IN_gem]

#------------------------------------------------------------------
# Timing Constraints
#------------------------------------------------------------------

## False paths
set_false_path -from [get_clocks clk_out1_*_clk_wiz_0_0] -to [get_clocks clk_out2_*_clk_wiz_0_0]
set_false_path -from [get_clocks clk_out2_*_clk_wiz_0_0] -to [get_clocks clk_out1_*_clk_wiz_0_0]
set_false_path -from [get_clocks clk_out2_*_clk_wiz_0_0] -to [get_clocks clk_out4_*_clk_wiz_0_0]
set_false_path -from [get_clocks clk_out4_*_clk_wiz_0_0] -to [get_clocks clk_out2_*_clk_wiz_0_0]

set_property DELAY_VALUE 0 [get_cells -hier -filter {NAME =~ */rgmii_interface/delay_rgmii_tx_clk}]
set_property DELAY_VALUE 0 [get_cells -hier -filter {NAME =~ */rgmii_interface/delay_rgmii_rx*}]
set_property DELAY_VALUE 0 [get_cells -hier -filter {NAME =~ */rgmii_interface/rxdata_bus[*].delay_rgmii_rx*}]


## Refclk frequency
set_property REFCLK_FREQUENCY 300.000 [get_cells -hier -filter {NAME =~ *delay_rgmii_tx*}]
set_clock_groups -asynchronous -group [get_clocks CLK_IN_gem]
set_clock_groups -asynchronous -group [get_clocks clk_out1_*_clk_wiz_0*]
set_clock_groups -asynchronous -group [get_clocks clk_out2_*_clk_wiz_0*]
set_clock_groups -asynchronous -group [get_clocks clk_out3_*_clk_wiz_0*]
set_clock_groups -asynchronous -group [get_clocks clk_out4_*_clk_wiz_0*]
set_clock_groups -asynchronous -group [get_clocks rgmii_rxc]
set_clock_groups -asynchronous -group [get_clocks rgmii2_rxc]
set_clock_groups -asynchronous -group [get_clocks clk_pl_0]
set_clock_groups -asynchronous -group [get_clocks kd240_*/TSN_subsystem/my_tsn_ip/inst/tsn_temac_1/inst_rgmii_tx_clk]
set_clock_groups -asynchronous -group [get_clocks kd240_*/TSN_subsystem/my_tsn_ip/inst/tsn_temac_2/inst_rgmii_tx_clk]
set_clock_groups -asynchronous -group [get_clocks clk_out_48M_*]
set_clock_groups -asynchronous -group [get_clocks clk_out_100M_*]