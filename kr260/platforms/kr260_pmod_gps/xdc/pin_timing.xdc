
# --(C) Copyright 2020 - 2021 Xilinx, Inc. 
# --Copyright (C) 2022 - 2023, Advanced Micro Devices, Inc 
# --SPDX-License-Identifier: Apache-2.0

#----------------------------------------------------------------------------
# Bitstream properties
#----------------------------------------------------------------------------

set_property BITSTREAM.GENERAL.COMPRESS TRUE [current_design]

#----------------------------------------------------------------------------
# Pin Constraints
#----------------------------------------------------------------------------

set_property PACKAGE_PIN E12 [get_ports UART_0_txd]
set_property IOSTANDARD LVCMOS33 [get_ports UART_0_txd]
set_property SLEW SLOW [get_ports UART_0_txd]
set_property DRIVE 4 [get_ports UART_0_txd]

set_property PACKAGE_PIN D11 [get_ports UART_0_rxd]
set_property IOSTANDARD LVCMOS33 [get_ports UART_0_rxd]
set_property SLEW SLOW [get_ports UART_0_rxd]
set_property DRIVE 4 [get_ports UART_0_rxd]

set_property PACKAGE_PIN B11 [get_ports PPS_in]
set_property IOSTANDARD LVCMOS33 [get_ports PPS_in]
set_property SLEW SLOW [get_ports PPS_in]
set_property DRIVE 4 [get_ports PPS_in] 

#Fan Speed Enable
set_property PACKAGE_PIN A12 [get_ports {fan_en_b}]
set_property IOSTANDARD LVCMOS33 [get_ports {fan_en_b}]
set_property SLEW SLOW [get_ports {fan_en_b}]
set_property DRIVE 4 [get_ports {fan_en_b}]
