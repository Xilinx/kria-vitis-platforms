# (C) Copyright 2020 - 2022 Xilinx, Inc.
# SPDX-License-Identifier: Apache-2.0

# Create instance: zynq_ultra_ps_e instance, and set properties
set ::PS_INST PS_0
set PS_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:zynq_ultra_ps_e PS_0 ]
apply_bd_automation -rule xilinx.com:bd_rule:zynq_ultra_ps_e -config {apply_board_preset "1" }  [get_bd_cells PS_0]

set_property -dict [list \
  CONFIG.PSU__USE__M_AXI_GP0 {0} \
  CONFIG.PSU__USE__M_AXI_GP1 {0} \
  CONFIG.PSU__USE__M_AXI_GP2 {1} \
  CONFIG.PSU__USE__S_AXI_GP2 {1} \
  CONFIG.PSU__USE__IRQ0 {1} \
  CONFIG.PSU__USE__IRQ1 {1} \
  CONFIG.PSU__TTC0__WAVEOUT__ENABLE {1} \
  CONFIG.PSU__TTC0__WAVEOUT__IO {EMIO} \
] $PS_0
##################################################################
# DESIGN PROCs
##################################################################


# Hierarchical cell: tx_s
proc create_hier_cell_tx_s { parentCell nameHier } {

  variable script_folder

  if { $parentCell eq "" || $nameHier eq "" } {
     catch {common::send_gid_msg -ssname BD::TCL -id 2092 -severity "ERROR" "create_hier_cell_tx_s() - Empty argument(s)!"}
     return
  }

  # Get object for parentCell
  set parentObj [get_bd_cells $parentCell]
  if { $parentObj == "" } {
     catch {common::send_gid_msg -ssname BD::TCL -id 2090 -severity "ERROR" "Unable to find parent cell <$parentCell>!"}
     return
  }

  # Make sure parentObj is hier blk
  set parentType [get_property TYPE $parentObj]
  if { $parentType ne "hier" } {
     catch {common::send_gid_msg -ssname BD::TCL -id 2091 -severity "ERROR" "Parent <$parentObj> has TYPE = <$parentType>. Expected to be <hier>."}
     return
  }

  # Save current instance; Restore later
  set oldCurInst [current_bd_instance .]

  # Set parent object as current
  current_bd_instance $parentObj

  # Create cell and set as current instance
  set hier_obj [create_bd_cell -type hier $nameHier]
  current_bd_instance $hier_obj

  # Create interface pins

  # Create pins
  create_bd_pin -dir O -from 0 -to 0 and_l_v_r
  create_bd_pin -dir I -from 0 -to 0 tlast
  create_bd_pin -dir I -from 0 -to 0 tready
  create_bd_pin -dir I -from 0 -to 0 tvalid

  # Create instance: and_last_valid, and set properties
  set and_last_valid [ create_bd_cell -type ip -vlnv xilinx.com:ip:util_vector_logic and_last_valid ]
  set_property -dict [ list \
   CONFIG.C_SIZE {1} \
 ] $and_last_valid

  # Create instance: and_ready, and set properties
  set and_ready [ create_bd_cell -type ip -vlnv xilinx.com:ip:util_vector_logic and_ready ]
  set_property -dict [ list \
   CONFIG.C_SIZE {1} \
 ] $and_ready

  # Create port connections
  connect_bd_net -net Op2_1 [get_bd_pins tready] [get_bd_pins and_ready/Op2]
  connect_bd_net -net and_ready_Res [get_bd_pins and_l_v_r] [get_bd_pins and_ready/Res]
  connect_bd_net -net ta_dma_0_m_axis_st_tlast [get_bd_pins tlast] [get_bd_pins and_last_valid/Op1]
  connect_bd_net -net ta_dma_0_m_axis_st_tvalid [get_bd_pins tvalid] [get_bd_pins and_last_valid/Op2]
  connect_bd_net -net util_vector_logic_0_Res [get_bd_pins and_last_valid/Res] [get_bd_pins and_ready/Op1]

  # Restore current instance
  current_bd_instance $oldCurInst
}

# Hierarchical cell: tx_b
proc create_hier_cell_tx_b { parentCell nameHier } {

  variable script_folder

  if { $parentCell eq "" || $nameHier eq "" } {
     catch {common::send_gid_msg -ssname BD::TCL -id 2092 -severity "ERROR" "create_hier_cell_tx_b() - Empty argument(s)!"}
     return
  }

  # Get object for parentCell
  set parentObj [get_bd_cells $parentCell]
  if { $parentObj == "" } {
     catch {common::send_gid_msg -ssname BD::TCL -id 2090 -severity "ERROR" "Unable to find parent cell <$parentCell>!"}
     return
  }

  # Make sure parentObj is hier blk
  set parentType [get_property TYPE $parentObj]
  if { $parentType ne "hier" } {
     catch {common::send_gid_msg -ssname BD::TCL -id 2091 -severity "ERROR" "Parent <$parentObj> has TYPE = <$parentType>. Expected to be <hier>."}
     return
  }

  # Save current instance; Restore later
  set oldCurInst [current_bd_instance .]

  # Set parent object as current
  current_bd_instance $parentObj

  # Create cell and set as current instance
  set hier_obj [create_bd_cell -type hier $nameHier]
  current_bd_instance $hier_obj

  # Create interface pins

  # Create pins
  create_bd_pin -dir O -from 0 -to 0 and_l_v_r
  create_bd_pin -dir I -from 0 -to 0 tlast
  create_bd_pin -dir I -from 0 -to 0 tready
  create_bd_pin -dir I -from 0 -to 0 tvalid

  # Create instance: and_last_valid, and set properties
  set and_last_valid [ create_bd_cell -type ip -vlnv xilinx.com:ip:util_vector_logic and_last_valid ]
  set_property -dict [ list \
   CONFIG.C_SIZE {1} \
 ] $and_last_valid

  # Create instance: and_ready, and set properties
  set and_ready [ create_bd_cell -type ip -vlnv xilinx.com:ip:util_vector_logic and_ready ]
  set_property -dict [ list \
   CONFIG.C_SIZE {1} \
 ] $and_ready

  # Create port connections
  connect_bd_net -net Op2_1 [get_bd_pins tready] [get_bd_pins and_ready/Op2]
  connect_bd_net -net and_ready_Res [get_bd_pins and_l_v_r] [get_bd_pins and_ready/Res]
  connect_bd_net -net ta_dma_0_m_axis_st_tlast [get_bd_pins tlast] [get_bd_pins and_last_valid/Op1]
  connect_bd_net -net ta_dma_0_m_axis_st_tvalid [get_bd_pins tvalid] [get_bd_pins and_last_valid/Op2]
  connect_bd_net -net util_vector_logic_0_Res [get_bd_pins and_last_valid/Res] [get_bd_pins and_ready/Op1]

  # Restore current instance
  current_bd_instance $oldCurInst
}

# Hierarchical cell: rx_s
proc create_hier_cell_rx_s { parentCell nameHier } {

  variable script_folder

  if { $parentCell eq "" || $nameHier eq "" } {
     catch {common::send_gid_msg -ssname BD::TCL -id 2092 -severity "ERROR" "create_hier_cell_rx_s() - Empty argument(s)!"}
     return
  }

  # Get object for parentCell
  set parentObj [get_bd_cells $parentCell]
  if { $parentObj == "" } {
     catch {common::send_gid_msg -ssname BD::TCL -id 2090 -severity "ERROR" "Unable to find parent cell <$parentCell>!"}
     return
  }

  # Make sure parentObj is hier blk
  set parentType [get_property TYPE $parentObj]
  if { $parentType ne "hier" } {
     catch {common::send_gid_msg -ssname BD::TCL -id 2091 -severity "ERROR" "Parent <$parentObj> has TYPE = <$parentType>. Expected to be <hier>."}
     return
  }

  # Save current instance; Restore later
  set oldCurInst [current_bd_instance .]

  # Set parent object as current
  current_bd_instance $parentObj

  # Create cell and set as current instance
  set hier_obj [create_bd_cell -type hier $nameHier]
  current_bd_instance $hier_obj

  # Create interface pins

  # Create pins
  create_bd_pin -dir O -from 0 -to 0 and_l_v_r
  create_bd_pin -dir I -from 0 -to 0 tlast
  create_bd_pin -dir I -from 0 -to 0 tready
  create_bd_pin -dir I -from 0 -to 0 tvalid

  # Create instance: and_last_valid, and set properties
  set and_last_valid [ create_bd_cell -type ip -vlnv xilinx.com:ip:util_vector_logic and_last_valid ]
  set_property -dict [ list \
   CONFIG.C_SIZE {1} \
 ] $and_last_valid

  # Create instance: and_ready, and set properties
  set and_ready [ create_bd_cell -type ip -vlnv xilinx.com:ip:util_vector_logic and_ready ]
  set_property -dict [ list \
   CONFIG.C_SIZE {1} \
 ] $and_ready

  # Create port connections
  connect_bd_net -net Op2_1 [get_bd_pins tready] [get_bd_pins and_ready/Op2]
  connect_bd_net -net and_ready_Res [get_bd_pins and_l_v_r] [get_bd_pins and_ready/Res]
  connect_bd_net -net ta_dma_0_m_axis_st_tlast [get_bd_pins tlast] [get_bd_pins and_last_valid/Op1]
  connect_bd_net -net ta_dma_0_m_axis_st_tvalid [get_bd_pins tvalid] [get_bd_pins and_last_valid/Op2]
  connect_bd_net -net util_vector_logic_0_Res [get_bd_pins and_last_valid/Res] [get_bd_pins and_ready/Op1]

  # Restore current instance
  current_bd_instance $oldCurInst
}

# Hierarchical cell: rx_b
proc create_hier_cell_rx_b { parentCell nameHier } {

  variable script_folder

  if { $parentCell eq "" || $nameHier eq "" } {
     catch {common::send_gid_msg -ssname BD::TCL -id 2092 -severity "ERROR" "create_hier_cell_rx_b() - Empty argument(s)!"}
     return
  }

  # Get object for parentCell
  set parentObj [get_bd_cells $parentCell]
  if { $parentObj == "" } {
     catch {common::send_gid_msg -ssname BD::TCL -id 2090 -severity "ERROR" "Unable to find parent cell <$parentCell>!"}
     return
  }

  # Make sure parentObj is hier blk
  set parentType [get_property TYPE $parentObj]
  if { $parentType ne "hier" } {
     catch {common::send_gid_msg -ssname BD::TCL -id 2091 -severity "ERROR" "Parent <$parentObj> has TYPE = <$parentType>. Expected to be <hier>."}
     return
  }

  # Save current instance; Restore later
  set oldCurInst [current_bd_instance .]

  # Set parent object as current
  current_bd_instance $parentObj

  # Create cell and set as current instance
  set hier_obj [create_bd_cell -type hier $nameHier]
  current_bd_instance $hier_obj

  # Create interface pins

  # Create pins
  create_bd_pin -dir O -from 0 -to 0 and_l_v_r
  create_bd_pin -dir I -from 0 -to 0 tlast
  create_bd_pin -dir I -from 0 -to 0 tready
  create_bd_pin -dir I -from 0 -to 0 tvalid

  # Create instance: and_last_valid, and set properties
  set and_last_valid [ create_bd_cell -type ip -vlnv xilinx.com:ip:util_vector_logic and_last_valid ]
  set_property -dict [ list \
   CONFIG.C_SIZE {1} \
 ] $and_last_valid

  # Create instance: and_ready, and set properties
  set and_ready [ create_bd_cell -type ip -vlnv xilinx.com:ip:util_vector_logic and_ready ]
  set_property -dict [ list \
   CONFIG.C_SIZE {1} \
 ] $and_ready

  # Create port connections
  connect_bd_net -net Op2_1 [get_bd_pins tready] [get_bd_pins and_ready/Op2]
  connect_bd_net -net and_ready_Res [get_bd_pins and_l_v_r] [get_bd_pins and_ready/Res]
  connect_bd_net -net ta_dma_0_m_axis_st_tlast [get_bd_pins tlast] [get_bd_pins and_last_valid/Op1]
  connect_bd_net -net ta_dma_0_m_axis_st_tvalid [get_bd_pins tvalid] [get_bd_pins and_last_valid/Op2]
  connect_bd_net -net util_vector_logic_0_Res [get_bd_pins and_last_valid/Res] [get_bd_pins and_ready/Op1]

  # Restore current instance
  current_bd_instance $oldCurInst
}


# Procedure to create entire design; Provide argument to make
# procedure reusable. If parentCell is "", will use root.
proc create_root_design { parentCell } {

  variable script_folder
  variable design_name

  if { $parentCell eq "" } {
     set parentCell [get_bd_cells /]
  }

  # Get object for parentCell
  set parentObj [get_bd_cells $parentCell]
  if { $parentObj == "" } {
     catch {common::send_gid_msg -ssname BD::TCL -id 2090 -severity "ERROR" "Unable to find parent cell <$parentCell>!"}
     return
  }

  # Make sure parentObj is hier blk
  set parentType [get_property TYPE $parentObj]
  if { $parentType ne "hier" } {
     catch {common::send_gid_msg -ssname BD::TCL -id 2091 -severity "ERROR" "Parent <$parentObj> has TYPE = <$parentType>. Expected to be <hier>."}
     return
  }

  # Save current instance; Restore later
  set oldCurInst [current_bd_instance .]

  # Set parent object as current
  current_bd_instance $parentObj


  # Create interface ports
  set mdio [ create_bd_intf_port -mode Master -vlnv xilinx.com:interface:mdio_io:1.0 mdio ]

  set mdio2 [ create_bd_intf_port -mode Master -vlnv xilinx.com:interface:mdio_io:1.0 mdio2 ]

  set rgmii [ create_bd_intf_port -mode Master -vlnv xilinx.com:interface:rgmii_rtl:1.0 rgmii ]

  set rgmii2 [ create_bd_intf_port -mode Master -vlnv xilinx.com:interface:rgmii_rtl:1.0 rgmii2 ]


  # Create ports
  set CLK_IN_gem [ create_bd_port -dir I -type clk -freq_hz 25000000 CLK_IN_gem ]
  set Phy_reset_n [ create_bd_port -dir O -from 1 -to 0 Phy_reset_n ]
  set UART_0_rxd [ create_bd_port -dir I UART_0_rxd ]
  set UART_0_rxen [ create_bd_port -dir O UART_0_rxen ]
  set UART_0_txd [ create_bd_port -dir O UART_0_txd ]
  set UART_0_txen [ create_bd_port -dir O UART_0_txen ]
  set fan_en_b [ create_bd_port -dir O -from 0 -to 0 fan_en_b ]
  set l1_rxs [ create_bd_port -dir O l1_rxs ]
  set l2_rxb [ create_bd_port -dir O l2_rxb ]
  set l3_rxx [ create_bd_port -dir O l3_rxx ]
  set p1_txs [ create_bd_port -dir O p1_txs ]
  set p2_txb [ create_bd_port -dir O p2_txb ]
  set p3_txx [ create_bd_port -dir O p3_txx ]
  set ptp_timer [ create_bd_port -dir O ptp_timer ]

  # Create instance: axi_intc_0, and set properties
  set axi_intc_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:axi_intc axi_intc_0 ]
  set_property -dict [ list \
   CONFIG.C_CASCADE_MASTER {1} \
   CONFIG.C_EN_CASCADE_MODE {0} \
   CONFIG.C_IRQ_CONNECTION {1} \
 ] $axi_intc_0

  # Create instance: axi_mcdma_0, and set properties
  set axi_mcdma_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:axi_mcdma axi_mcdma_0 ]
  set_property -dict [ list \
   CONFIG.c_addr_width {32} \
   CONFIG.c_group1_mm2s {0000000000000011} \
   CONFIG.c_group1_s2mm {0000000000001111} \
   CONFIG.c_include_mm2s {1} \
   CONFIG.c_include_mm2s_dre {1} \
   CONFIG.c_include_s2mm_dre {1} \
   CONFIG.c_mm2s_scheduler {0} \
   CONFIG.c_num_mm2s_channels {2} \
   CONFIG.c_num_s2mm_channels {4} \
 ] $axi_mcdma_0

  # Create instance: axi_smc, and set properties
  set axi_smc [ create_bd_cell -type ip -vlnv xilinx.com:ip:smartconnect axi_smc ]
  set_property -dict [ list \
   CONFIG.NUM_SI {4} \
 ] $axi_smc

  # Create instance: axi_uartlite_0, and set properties
  set axi_uartlite_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:axi_uartlite axi_uartlite_0 ]

  # Create instance: axis_switch_0, and set properties
  set axis_switch_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:axis_switch axis_switch_0 ]
  set_property -dict [ list \
   CONFIG.ARB_ALGORITHM {1} \
   CONFIG.ARB_ON_TLAST {1} \
   CONFIG.HAS_TKEEP {1} \
   CONFIG.HAS_TLAST {1} \
   CONFIG.M00_AXIS_HIGHTDEST {0x00000006} \
   CONFIG.NUM_SI {2} \
   CONFIG.ROUTING_MODE {0} \
   CONFIG.TDATA_NUM_BYTES {4} \
   CONFIG.TDEST_WIDTH {3} \
   CONFIG.TUSER_WIDTH {32} \
 ] $axis_switch_0

  # Create instance: clk_wiz_0, and set properties
  set clk_wiz_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:clk_wiz clk_wiz_0 ]
  set_property -dict [ list \
   CONFIG.CLKIN1_JITTER_PS {400.0} \
   CONFIG.CLKOUT1_JITTER {141.764} \
   CONFIG.CLKOUT1_PHASE_ERROR {222.521} \
   CONFIG.CLKOUT1_REQUESTED_OUT_FREQ {200.000} \
   CONFIG.CLKOUT2_JITTER {148.988} \
   CONFIG.CLKOUT2_PHASE_ERROR {222.521} \
   CONFIG.CLKOUT2_REQUESTED_OUT_FREQ {125.000} \
   CONFIG.CLKOUT2_USED {true} \
   CONFIG.CLKOUT3_JITTER {135.994} \
   CONFIG.CLKOUT3_PHASE_ERROR {222.521} \
   CONFIG.CLKOUT3_REQUESTED_OUT_FREQ {300.000} \
   CONFIG.CLKOUT3_USED {true} \
   CONFIG.CLKOUT4_JITTER {152.629} \
   CONFIG.CLKOUT4_PHASE_ERROR {222.521} \
   CONFIG.CLKOUT4_REQUESTED_OUT_FREQ {100.000} \
   CONFIG.CLKOUT4_USED {true} \
   CONFIG.MMCM_CLKFBOUT_MULT_F {60.000} \
   CONFIG.MMCM_CLKIN1_PERIOD {40.000} \
   CONFIG.MMCM_CLKIN2_PERIOD {10.0} \
   CONFIG.MMCM_CLKOUT0_DIVIDE_F {7.500} \
   CONFIG.MMCM_CLKOUT1_DIVIDE {12} \
   CONFIG.MMCM_CLKOUT2_DIVIDE {5} \
   CONFIG.MMCM_CLKOUT3_DIVIDE {15} \
   CONFIG.MMCM_DIVCLK_DIVIDE {1} \
   CONFIG.NUM_OUT_CLKS {4} \
   CONFIG.PRIM_IN_FREQ {25} \
   CONFIG.PRIM_SOURCE {Single_ended_clock_capable_pin} \
   CONFIG.USE_BOARD_FLOW {true} \
   CONFIG.USE_RESET {false} \
 ] $clk_wiz_0

  # Create instance: interrupts_concat_0, and set properties
  set interrupts_concat_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlconcat interrupts_concat_0 ]
  set_property -dict [ list \
   CONFIG.NUM_PORTS {7} \
 ] $interrupts_concat_0

  # Create instance: interrupts_concat_1, and set properties
  set interrupts_concat_1 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlconcat interrupts_concat_1 ]
  set_property -dict [ list \
   CONFIG.NUM_PORTS {8} \
 ] $interrupts_concat_1

  # Create instance: my_tsn_ip, and set properties
  set my_tsn_ip [ create_bd_cell -type ip -vlnv xilinx.com:ip:tsn_endpoint_ethernet_mac my_tsn_ip ]
  set_property -dict [ list \
   CONFIG.ADDR_TRANSLATION_TYPE {false} \
   CONFIG.ENABLE_EP_TADMA_INTERFACE {true} \
   CONFIG.EN_CB_RSVD_BYTES {true} \
   CONFIG.EN_COMMON_STATS {false} \
   CONFIG.EN_FRAME_FILTERS {false} \
   CONFIG.EN_FRAME_PREEMPTION_SUPPORT {true} \
   CONFIG.EN_FRAME_PREEMPTION_SUPPORT_MAC_2 {true} \
   CONFIG.EN_FRER {true} \
   CONFIG.EN_HW_ADDR_LEARNING {true} \
   CONFIG.EN_MDIO_IO_BUF {true} \
   CONFIG.EN_PERVLAN_AGING_TIME {1} \
   CONFIG.EN_PER_PRIQ_CNFGS {false} \
   CONFIG.EN_PSFP {true} \
   CONFIG.EN_RX_CUSTOM_TIMESTAMP {true} \
   CONFIG.EN_TX_CUSTOM_TIMESTAMP {true} \
   CONFIG.NUM_FRAME_FILTERS {0} \
   CONFIG.NUM_PRIORITIES {2} \
   CONFIG.PHYSICAL_INTERFACE {RGMII} \
   CONFIG.SCHEDULED_QUEUE {0} \
   CONFIG.STORE_AND_FORWARD {1} \
   CONFIG.TSN_EP_Switch {1} \
   CONFIG.USE_HARD_FIFO {0} \
 ] $my_tsn_ip

  # Create instance: proc_sys_reset_0, and set properties
  set proc_sys_reset_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:proc_sys_reset proc_sys_reset_0 ]

  # Create instance: ps8_0_axi_periph, and set properties
  set ps8_0_axi_periph [ create_bd_cell -type ip -vlnv xilinx.com:ip:axi_interconnect ps8_0_axi_periph ]
  set_property -dict [ list \
   CONFIG.M01_HAS_REGSLICE {3} \
   CONFIG.NUM_MI {6} \
 ] $ps8_0_axi_periph

  # Create instance: test_pmod_controller_0, and set properties
  set test_pmod_controller_0 [ create_bd_cell -type ip -vlnv user.org:user:test_pmod_controller test_pmod_controller_0 ]

  # Create instance: rst_clk_wiz_0_100M, and set properties
  set rst_clk_wiz_0_100M [ create_bd_cell -type ip -vlnv xilinx.com:ip:proc_sys_reset rst_clk_wiz_0_100M ]

  # Create instance: rst_clk_wiz_0_125M, and set properties
  set rst_clk_wiz_0_125M [ create_bd_cell -type ip -vlnv xilinx.com:ip:proc_sys_reset rst_clk_wiz_0_125M ]

  # Create instance: rst_clk_wiz_0_200M, and set properties
  set rst_clk_wiz_0_200M [ create_bd_cell -type ip -vlnv xilinx.com:ip:proc_sys_reset rst_clk_wiz_0_200M ]

  # Create instance: rst_clk_wiz_0_300M, and set properties
  set rst_clk_wiz_0_300M [ create_bd_cell -type ip -vlnv xilinx.com:ip:proc_sys_reset rst_clk_wiz_0_300M ]

  # Create instance: rx_b
  create_hier_cell_rx_b [current_bd_instance .] rx_b

  # Create instance: rx_s
  create_hier_cell_rx_s [current_bd_instance .] rx_s

  # Create instance: ta_dma_0, and set properties
  set ta_dma_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:ta_dma ta_dma_0 ]

  # Create instance: tx_b
  create_hier_cell_tx_b [current_bd_instance .] tx_b

  # Create instance: tx_s
  create_hier_cell_tx_s [current_bd_instance .] tx_s

  # Create instance: xlconcat_2, and set properties
  set xlconcat_2 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlconcat xlconcat_2 ]
  set_property -dict [ list \
   CONFIG.NUM_PORTS {3} \
 ] $xlconcat_2

  # Create instance: xlconcat_7, and set properties
  set xlconcat_7 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlconcat xlconcat_7 ]
  set_property -dict [ list \
   CONFIG.IN0_WIDTH {32} \
   CONFIG.IN1_WIDTH {48} \
 ] $xlconcat_7

  # Create instance: xlconcat_tlast, and set properties
  set xlconcat_tlast [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlconcat xlconcat_tlast ]

  # Create instance: xlconcat_tvld, and set properties
  set xlconcat_tvld [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlconcat xlconcat_tvld ]

  # Create instance: xlconstant_0, and set properties
  set xlconstant_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlconstant xlconstant_0 ]
  set_property -dict [ list \
   CONFIG.CONST_VAL {3} \
   CONFIG.CONST_WIDTH {2} \
 ] $xlconstant_0

  # Create instance: xlconstant_1, and set properties
  set xlconstant_1 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlconstant xlconstant_1 ]

  # Create instance: xlconstant_2, and set properties
  set xlconstant_2 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlconstant xlconstant_2 ]
  set_property -dict [ list \
   CONFIG.CONST_VAL {0} \
   CONFIG.CONST_WIDTH {2} \
 ] $xlconstant_2

  # Create instance: xlconstant_4, and set properties
  set xlconstant_4 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlconstant xlconstant_4 ]

  # Create instance: xlslice_1, and set properties
  set xlslice_1 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlslice xlslice_1 ]
  set_property -dict [ list \
   CONFIG.DIN_FROM {138} \
   CONFIG.DIN_TO {136} \
   CONFIG.DIN_WIDTH {512} \
   CONFIG.DOUT_WIDTH {1} \
 ] $xlslice_1

  # Create instance: xlslice_2, and set properties
  set xlslice_2 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlslice xlslice_2 ]
  set_property -dict [ list \
   CONFIG.DIN_FROM {146} \
   CONFIG.DIN_TO {144} \
   CONFIG.DIN_WIDTH {512} \
   CONFIG.DOUT_WIDTH {1} \
 ] $xlslice_2

  # Create instance: xlslice_3, and set properties
  set xlslice_3 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlslice xlslice_3 ]
  set_property -dict [ list \
   CONFIG.DIN_FROM {154} \
   CONFIG.DIN_TO {152} \
   CONFIG.DIN_WIDTH {512} \
   CONFIG.DOUT_WIDTH {1} \
 ] $xlslice_3

  # Create instance: xlslice_4, and set properties
  set xlslice_4 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlslice xlslice_4 ]
  set_property -dict [ list \
   CONFIG.DIN_FROM {161} \
   CONFIG.DIN_TO {160} \
   CONFIG.DIN_WIDTH {512} \
   CONFIG.DOUT_WIDTH {1} \
 ] $xlslice_4

  # Create instance: xlslice_5, and set properties
  set xlslice_5 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlslice xlslice_5 ]
  set_property -dict [ list \
   CONFIG.DIN_FROM {193} \
   CONFIG.DIN_TO {192} \
   CONFIG.DIN_WIDTH {512} \
   CONFIG.DOUT_WIDTH {1} \
 ] $xlslice_5

  # Create instance: xlslice_6, and set properties
  set xlslice_6 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlslice xlslice_6 ]
  set_property -dict [ list \
   CONFIG.DIN_FROM {0} \
   CONFIG.DIN_TO {0} \
   CONFIG.DIN_WIDTH {48} \
   CONFIG.DOUT_WIDTH {1} \
 ] $xlslice_6

  # Create instance: xlslice_7, and set properties
  set xlslice_7 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlslice xlslice_7 ]
  set_property -dict [ list \
   CONFIG.DIN_FROM {9} \
   CONFIG.DIN_TO {9} \
   CONFIG.DIN_WIDTH {32} \
   CONFIG.DOUT_WIDTH {1} \
 ] $xlslice_7

  # Create instance: xlslice_8, and set properties
  set xlslice_8 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlslice xlslice_8 ]
  set_property -dict [ list \
   CONFIG.DIN_FROM {19} \
   CONFIG.DIN_TO {19} \
   CONFIG.DIN_WIDTH {32} \
   CONFIG.DOUT_WIDTH {1} \
 ] $xlslice_8

  # Create instance: xlslice_9, and set properties
  set xlslice_9 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlslice xlslice_9 ]
  set_property -dict [ list \
   CONFIG.DIN_FROM {21} \
   CONFIG.DIN_TO {21} \
   CONFIG.DIN_WIDTH {32} \
   CONFIG.DOUT_WIDTH {1} \
 ] $xlslice_9

  # Create instance: xlslice_10, and set properties
  set xlslice_10 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlslice xlslice_10 ]
  set_property -dict [ list \
   CONFIG.DIN_FROM {14} \
   CONFIG.DIN_TO {14} \
   CONFIG.DIN_WIDTH {32} \
   CONFIG.DOUT_WIDTH {1} \
 ] $xlslice_10

  # Create instance: xlslice_11, and set properties
  set xlslice_11 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlslice xlslice_11 ]
  set_property -dict [ list \
   CONFIG.DIN_FROM {8} \
   CONFIG.DIN_TO {0} \
   CONFIG.DIN_WIDTH {32} \
   CONFIG.DOUT_WIDTH {9} \
 ] $xlslice_11

  # Create instance: xlslice_12, and set properties
  set xlslice_12 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlslice xlslice_12 ]
  set_property -dict [ list \
   CONFIG.DIN_FROM {15} \
   CONFIG.DIN_TO {0} \
   CONFIG.DIN_WIDTH {64} \
   CONFIG.DOUT_WIDTH {16} \
 ] $xlslice_12

  # Create instance: xlslice_13, and set properties
  set xlslice_13 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlslice xlslice_13 ]
  set_property -dict [ list \
   CONFIG.DIN_FROM {63} \
   CONFIG.DIN_TO {63} \
   CONFIG.DIN_WIDTH {64} \
   CONFIG.DOUT_WIDTH {1} \
 ] $xlslice_13

  # Create instance: xlslice_14, and set properties
  set xlslice_14 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlslice xlslice_14 ]
  set_property -dict [ list \
   CONFIG.DIN_FROM {31} \
   CONFIG.DIN_TO {16} \
   CONFIG.DIN_WIDTH {64} \
   CONFIG.DOUT_WIDTH {16} \
 ] $xlslice_14

  # Create instance: xlslice_15, and set properties
  set xlslice_15 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlslice xlslice_15 ]
  set_property -dict [ list \
   CONFIG.DIN_FROM {47} \
   CONFIG.DIN_TO {32} \
   CONFIG.DIN_WIDTH {64} \
   CONFIG.DOUT_WIDTH {1} \
 ] $xlslice_15

  # Create instance: xlslice_16, and set properties
  set xlslice_16 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlslice xlslice_16 ]
  set_property -dict [ list \
   CONFIG.DIN_FROM {62} \
   CONFIG.DIN_TO {62} \
   CONFIG.DIN_WIDTH {64} \
   CONFIG.DOUT_WIDTH {1} \
 ] $xlslice_16

  # Create instance: xlslice_trdy_0, and set properties
  set xlslice_trdy_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlslice xlslice_trdy_0 ]
  set_property -dict [ list \
   CONFIG.DIN_WIDTH {2} \
 ] $xlslice_trdy_0

  # Create instance: xlslice_trdy_1, and set properties
  set xlslice_trdy_1 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlslice xlslice_trdy_1 ]
  set_property -dict [ list \
   CONFIG.DIN_FROM {1} \
   CONFIG.DIN_TO {1} \
   CONFIG.DIN_WIDTH {2} \
   CONFIG.DOUT_WIDTH {1} \
 ] $xlslice_trdy_1

  # Create instance: xlslice_ttc_0, and set properties
  set xlslice_ttc_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlslice xlslice_ttc_0 ]
  set_property -dict [ list \
   CONFIG.DIN_FROM {2} \
   CONFIG.DIN_TO {2} \
   CONFIG.DIN_WIDTH {3} \
   CONFIG.DOUT_WIDTH {1} \
 ] $xlslice_ttc_0

  # Create interface connections
  connect_bd_intf_net -intf_net PS_0_M_AXI_HPM0_LPD [get_bd_intf_pins PS_0/M_AXI_HPM0_LPD] [get_bd_intf_pins ps8_0_axi_periph/S00_AXI]
  connect_bd_intf_net -intf_net axi_mcdma_0_M_AXIS_MM2S [get_bd_intf_pins axi_mcdma_0/M_AXIS_MM2S] [get_bd_intf_pins my_tsn_ip/tx_axis_be]
  connect_bd_intf_net -intf_net axi_mcdma_0_M_AXI_MM2S [get_bd_intf_pins axi_mcdma_0/M_AXI_MM2S] [get_bd_intf_pins axi_smc/S01_AXI]
  connect_bd_intf_net -intf_net axi_mcdma_0_M_AXI_S2MM [get_bd_intf_pins axi_mcdma_0/M_AXI_S2MM] [get_bd_intf_pins axi_smc/S02_AXI]
  connect_bd_intf_net -intf_net axi_mcdma_0_M_AXI_SG [get_bd_intf_pins axi_mcdma_0/M_AXI_SG] [get_bd_intf_pins axi_smc/S00_AXI]
  connect_bd_intf_net -intf_net axi_smc_M00_AXI [get_bd_intf_pins PS_0/S_AXI_HP0_FPD] [get_bd_intf_pins axi_smc/M00_AXI]
  connect_bd_intf_net -intf_net axis_switch_0_M00_AXIS [get_bd_intf_pins axi_mcdma_0/S_AXIS_S2MM] [get_bd_intf_pins axis_switch_0/M00_AXIS]
  connect_bd_intf_net -intf_net my_tsn_ip_rx_axis_be [get_bd_intf_pins axis_switch_0/S01_AXIS] [get_bd_intf_pins my_tsn_ip/rx_axis_be]
  connect_bd_intf_net -intf_net ps8_0_axi_periph_M00_AXI [get_bd_intf_pins axi_mcdma_0/S_AXI_LITE] [get_bd_intf_pins ps8_0_axi_periph/M00_AXI]
  connect_bd_intf_net -intf_net ps8_0_axi_periph_M01_AXI [get_bd_intf_pins ps8_0_axi_periph/M01_AXI] [get_bd_intf_pins ta_dma_0/S_AXI]
  connect_bd_intf_net -intf_net ps8_0_axi_periph_M02_AXI [get_bd_intf_pins my_tsn_ip/s_axi] [get_bd_intf_pins ps8_0_axi_periph/M02_AXI]
  connect_bd_intf_net -intf_net ps8_0_axi_periph_M03_AXI [get_bd_intf_pins axi_intc_0/s_axi] [get_bd_intf_pins ps8_0_axi_periph/M03_AXI]
  connect_bd_intf_net -intf_net ps8_0_axi_periph_M04_AXI [get_bd_intf_pins axi_uartlite_0/S_AXI] [get_bd_intf_pins ps8_0_axi_periph/M04_AXI]
  connect_bd_intf_net -intf_net ps8_0_axi_periph_M05_AXI [get_bd_intf_pins ps8_0_axi_periph/M05_AXI] [get_bd_intf_pins test_pmod_controller_0/S00_AXI]
  connect_bd_intf_net -intf_net ta_dma_0_M_AXI [get_bd_intf_pins axi_smc/S03_AXI] [get_bd_intf_pins ta_dma_0/M_AXI]
  connect_bd_intf_net -intf_net ta_dma_0_M_AXIS_ST_INTF [get_bd_intf_pins my_tsn_ip/tx_axis_st] [get_bd_intf_pins ta_dma_0/M_AXIS_ST_INTF]
  connect_bd_intf_net -intf_net tsn_endpoint_ethernet_mac_0_mdio_external1 [get_bd_intf_ports mdio] [get_bd_intf_pins my_tsn_ip/mdio_external1]
  connect_bd_intf_net -intf_net tsn_endpoint_ethernet_mac_0_mdio_external2 [get_bd_intf_ports mdio2] [get_bd_intf_pins my_tsn_ip/mdio_external2]
  connect_bd_intf_net -intf_net tsn_endpoint_ethernet_mac_0_rgmii1 [get_bd_intf_ports rgmii] [get_bd_intf_pins my_tsn_ip/rgmii1]
  connect_bd_intf_net -intf_net tsn_endpoint_ethernet_mac_0_rgmii2 [get_bd_intf_ports rgmii2] [get_bd_intf_pins my_tsn_ip/rgmii2]
  connect_bd_intf_net -intf_net tsn_endpoint_ethernet_mac_0_rx_axis_st [get_bd_intf_pins axis_switch_0/S00_AXIS] [get_bd_intf_pins my_tsn_ip/rx_axis_st]

  # Create port connections
  connect_bd_net -net Op2_1 [get_bd_pins my_tsn_ip/tx_axis_st_tready] [get_bd_pins ta_dma_0/m_axis_st_tready] -boundary_type upper
  connect_bd_net -net Op2_1 [get_bd_pins my_tsn_ip/tx_axis_st_tready] [get_bd_pins tx_s/tready] -boundary_type upper
  connect_bd_net -net PS_0_emio_ttc0_wave_o [get_bd_pins PS_0/emio_ttc0_wave_o] [get_bd_pins xlslice_ttc_0/Din]
  connect_bd_net -net axi_intc_0_irq [get_bd_pins PS_0/pl_ps_irq0] [get_bd_pins axi_intc_0/irq]
  connect_bd_net -net axi_mcdma_0_m_axis_mm2s_tlast [get_bd_pins axi_mcdma_0/m_axis_mm2s_tlast] [get_bd_pins my_tsn_ip/tx_axis_be_tlast] -boundary_type upper
  connect_bd_net -net axi_mcdma_0_m_axis_mm2s_tlast [get_bd_pins axi_mcdma_0/m_axis_mm2s_tlast] [get_bd_pins tx_b/tlast] -boundary_type upper
  connect_bd_net -net axi_mcdma_0_m_axis_mm2s_tvalid [get_bd_pins axi_mcdma_0/m_axis_mm2s_tvalid] [get_bd_pins my_tsn_ip/tx_axis_be_tvalid] -boundary_type upper
  connect_bd_net -net axi_mcdma_0_m_axis_mm2s_tvalid [get_bd_pins axi_mcdma_0/m_axis_mm2s_tvalid] [get_bd_pins tx_b/tvalid] -boundary_type upper
  connect_bd_net -net axi_mcdma_0_mm2s_ch1_introut [get_bd_pins axi_mcdma_0/mm2s_ch1_introut] [get_bd_pins interrupts_concat_0/In1]
  connect_bd_net -net axi_mcdma_0_mm2s_ch2_introut [get_bd_pins axi_mcdma_0/mm2s_ch2_introut] [get_bd_pins interrupts_concat_0/In2]
  connect_bd_net -net axi_mcdma_0_s2mm_ch1_introut [get_bd_pins axi_mcdma_0/s2mm_ch1_introut] [get_bd_pins interrupts_concat_0/In3]
  connect_bd_net -net axi_mcdma_0_s2mm_ch2_introut [get_bd_pins axi_mcdma_0/s2mm_ch2_introut] [get_bd_pins interrupts_concat_0/In4]
  connect_bd_net -net axi_mcdma_0_s2mm_ch3_introut [get_bd_pins axi_mcdma_0/s2mm_ch3_introut] [get_bd_pins interrupts_concat_0/In5]
  connect_bd_net -net axi_mcdma_0_s2mm_ch4_introut [get_bd_pins axi_mcdma_0/s2mm_ch4_introut] [get_bd_pins interrupts_concat_0/In6]
  connect_bd_net -net axi_uartlite_0_interrupt [get_bd_pins axi_uartlite_0/interrupt] [get_bd_pins xlconcat_2/In2]
  connect_bd_net -net axi_uartlite_0_tx [get_bd_ports UART_0_txd] [get_bd_pins axi_uartlite_0/tx]
  connect_bd_net -net axi_uartlite_0_txen [get_bd_ports UART_0_rxen] [get_bd_ports UART_0_txen] -boundary_type upper
  connect_bd_net -net axi_uartlite_0_txen [get_bd_ports UART_0_rxen] [get_bd_pins axi_uartlite_0/txen] -boundary_type upper
  connect_bd_net -net axis_switch_0_s_axis_tready [get_bd_pins axis_switch_0/s_axis_tready] [get_bd_pins xlslice_trdy_0/Din] -boundary_type upper
  connect_bd_net -net axis_switch_0_s_axis_tready [get_bd_pins axis_switch_0/s_axis_tready] [get_bd_pins xlslice_trdy_1/Din] -boundary_type upper
  connect_bd_net -net clk_in1_0_1 [get_bd_ports CLK_IN_gem] [get_bd_pins clk_wiz_0/clk_in1]
  connect_bd_net -net clk_wiz_0_clk_out1 [get_bd_pins PS_0/saxihp0_fpd_aclk] [get_bd_pins axi_mcdma_0/s_axi_aclk] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_out1 [get_bd_pins PS_0/saxihp0_fpd_aclk] [get_bd_pins axi_smc/aclk] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_out1 [get_bd_pins PS_0/saxihp0_fpd_aclk] [get_bd_pins axis_switch_0/aclk] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_out1 [get_bd_pins PS_0/saxihp0_fpd_aclk] [get_bd_pins clk_wiz_0/clk_out1] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_out1 [get_bd_pins PS_0/saxihp0_fpd_aclk] [get_bd_pins my_tsn_ip/host_rxfifo_aclk] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_out1 [get_bd_pins PS_0/saxihp0_fpd_aclk] [get_bd_pins my_tsn_ip/host_txfifo_aclk] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_out1 [get_bd_pins PS_0/saxihp0_fpd_aclk] [get_bd_pins ps8_0_axi_periph/M01_ACLK] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_out1 [get_bd_pins PS_0/saxihp0_fpd_aclk] [get_bd_pins rst_clk_wiz_0_200M/slowest_sync_clk] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_out1 [get_bd_pins PS_0/saxihp0_fpd_aclk] [get_bd_pins ta_dma_0/dma_clk] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_out2 [get_bd_pins clk_wiz_0/clk_out2] [get_bd_pins my_tsn_ip/gtx_clk] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_out2 [get_bd_pins clk_wiz_0/clk_out2] [get_bd_pins my_tsn_ip/gtx_clk90] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_out2 [get_bd_pins clk_wiz_0/clk_out2] [get_bd_pins rst_clk_wiz_0_125M/slowest_sync_clk] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_out2 [get_bd_pins clk_wiz_0/clk_out2] [get_bd_pins ta_dma_0/rtc_clk] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_out3 [get_bd_pins clk_wiz_0/clk_out3] [get_bd_pins my_tsn_ip/refclk] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_out3 [get_bd_pins clk_wiz_0/clk_out3] [get_bd_pins rst_clk_wiz_0_300M/slowest_sync_clk] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_out4 [get_bd_pins PS_0/maxihpm0_lpd_aclk] [get_bd_pins axi_intc_0/s_axi_aclk] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_out4 [get_bd_pins PS_0/maxihpm0_lpd_aclk] [get_bd_pins axi_mcdma_0/s_axi_lite_aclk] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_out4 [get_bd_pins PS_0/maxihpm0_lpd_aclk] [get_bd_pins axi_uartlite_0/s_axi_aclk] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_out4 [get_bd_pins PS_0/maxihpm0_lpd_aclk] [get_bd_pins clk_wiz_0/clk_out4] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_out4 [get_bd_pins PS_0/maxihpm0_lpd_aclk] [get_bd_pins my_tsn_ip/s_axi_aclk] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_out4 [get_bd_pins PS_0/maxihpm0_lpd_aclk] [get_bd_pins ps8_0_axi_periph/ACLK] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_out4 [get_bd_pins PS_0/maxihpm0_lpd_aclk] [get_bd_pins ps8_0_axi_periph/M00_ACLK] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_out4 [get_bd_pins PS_0/maxihpm0_lpd_aclk] [get_bd_pins ps8_0_axi_periph/M02_ACLK] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_out4 [get_bd_pins PS_0/maxihpm0_lpd_aclk] [get_bd_pins ps8_0_axi_periph/M03_ACLK] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_out4 [get_bd_pins PS_0/maxihpm0_lpd_aclk] [get_bd_pins ps8_0_axi_periph/M04_ACLK] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_out4 [get_bd_pins PS_0/maxihpm0_lpd_aclk] [get_bd_pins ps8_0_axi_periph/M05_ACLK] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_out4 [get_bd_pins PS_0/maxihpm0_lpd_aclk] [get_bd_pins ps8_0_axi_periph/S00_ACLK] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_out4 [get_bd_pins PS_0/maxihpm0_lpd_aclk] [get_bd_pins test_pmod_controller_0/s00_axi_aclk] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_out4 [get_bd_pins PS_0/maxihpm0_lpd_aclk] [get_bd_pins rst_clk_wiz_0_100M/slowest_sync_clk] -boundary_type upper
  connect_bd_net -net clk_wiz_0_locked [get_bd_pins clk_wiz_0/locked] [get_bd_pins my_tsn_ip/gtx_dcm_locked] -boundary_type upper
  connect_bd_net -net clk_wiz_0_locked [get_bd_pins clk_wiz_0/locked] [get_bd_pins rst_clk_wiz_0_100M/dcm_locked] -boundary_type upper
  connect_bd_net -net clk_wiz_0_locked [get_bd_pins clk_wiz_0/locked] [get_bd_pins rst_clk_wiz_0_125M/dcm_locked] -boundary_type upper
  connect_bd_net -net clk_wiz_0_locked [get_bd_pins clk_wiz_0/locked] [get_bd_pins rst_clk_wiz_0_200M/dcm_locked] -boundary_type upper
  connect_bd_net -net clk_wiz_0_locked [get_bd_pins clk_wiz_0/locked] [get_bd_pins rst_clk_wiz_0_300M/dcm_locked] -boundary_type upper
  connect_bd_net -net interrupts_concat_0_dout [get_bd_pins interrupts_concat_0/dout] [get_bd_pins xlconcat_2/In1]
  connect_bd_net -net interrupts_concat_1_dout [get_bd_pins interrupts_concat_1/dout] [get_bd_pins xlconcat_2/In0]
  connect_bd_net -net my_tsn_ip_ptp_timers_clk8k [get_bd_ports ptp_timer] [get_bd_pins my_tsn_ip/ptp_timers_clk8k] -boundary_type upper
  connect_bd_net -net my_tsn_ip_ptp_timers_clk8k [get_bd_ports ptp_timer] [get_bd_pins proc_sys_reset_0/slowest_sync_clk] -boundary_type upper
  connect_bd_net -net my_tsn_ip_rx_axis_be_tlast [get_bd_pins my_tsn_ip/rx_axis_be_tlast] [get_bd_pins rx_b/tlast] -boundary_type upper
  connect_bd_net -net my_tsn_ip_rx_axis_be_tlast [get_bd_pins my_tsn_ip/rx_axis_be_tlast] [get_bd_pins xlconcat_tlast/In1] -boundary_type upper
  connect_bd_net -net my_tsn_ip_rx_axis_be_tvalid [get_bd_pins my_tsn_ip/rx_axis_be_tvalid] [get_bd_pins rx_b/tvalid] -boundary_type upper
  connect_bd_net -net my_tsn_ip_rx_axis_be_tvalid [get_bd_pins my_tsn_ip/rx_axis_be_tvalid] [get_bd_pins xlconcat_tvld/In1] -boundary_type upper
  connect_bd_net -net my_tsn_ip_rx_axis_st_tlast [get_bd_pins my_tsn_ip/rx_axis_st_tlast] [get_bd_pins rx_s/tlast] -boundary_type upper
  connect_bd_net -net my_tsn_ip_rx_axis_st_tlast [get_bd_pins my_tsn_ip/rx_axis_st_tlast] [get_bd_pins xlconcat_tlast/In0] -boundary_type upper
  connect_bd_net -net my_tsn_ip_rx_axis_st_tvalid [get_bd_pins my_tsn_ip/rx_axis_st_tvalid] [get_bd_pins rx_s/tvalid] -boundary_type upper
  connect_bd_net -net my_tsn_ip_rx_axis_st_tvalid [get_bd_pins my_tsn_ip/rx_axis_st_tvalid] [get_bd_pins xlconcat_tvld/In0] -boundary_type upper
  connect_bd_net -net my_tsn_ip_tx_axis_be_tready [get_bd_pins axi_mcdma_0/m_axis_mm2s_tready] [get_bd_pins my_tsn_ip/tx_axis_be_tready] -boundary_type upper
  connect_bd_net -net my_tsn_ip_tx_axis_be_tready [get_bd_pins axi_mcdma_0/m_axis_mm2s_tready] [get_bd_pins tx_b/tready] -boundary_type upper
  connect_bd_net -net test_pmod_controller_0_listner_0 [get_bd_ports l1_rxs] [get_bd_pins test_pmod_controller_0/lis1_rxs]
  connect_bd_net -net test_pmod_controller_0_listner_1 [get_bd_ports l2_rxb] [get_bd_pins test_pmod_controller_0/lis2_rxb]
  connect_bd_net -net test_pmod_controller_0_listner_2 [get_bd_ports l3_rxx] [get_bd_pins test_pmod_controller_0/lis3_rxx]
  connect_bd_net -net test_pmod_controller_0_publisher_0 [get_bd_ports p1_txs] [get_bd_pins test_pmod_controller_0/pub1_txs]
  connect_bd_net -net test_pmod_controller_0_publisher_1 [get_bd_ports p2_txb] [get_bd_pins test_pmod_controller_0/pub2_txb]
  connect_bd_net -net test_pmod_controller_0_publisher_2 [get_bd_ports p3_txx] [get_bd_pins test_pmod_controller_0/pub3_txx]
  connect_bd_net -net rst_clk_wiz_0_100M_1_peripheral_aresetn [get_bd_pins axi_intc_0/s_axi_aresetn] [get_bd_pins axi_mcdma_0/axi_resetn] -boundary_type upper
  connect_bd_net -net rst_clk_wiz_0_100M_1_peripheral_aresetn [get_bd_pins axi_intc_0/s_axi_aresetn] [get_bd_pins axi_uartlite_0/s_axi_aresetn] -boundary_type upper
  connect_bd_net -net rst_clk_wiz_0_100M_1_peripheral_aresetn [get_bd_pins axi_intc_0/s_axi_aresetn] [get_bd_pins my_tsn_ip/glbl_rstn] -boundary_type upper
  connect_bd_net -net rst_clk_wiz_0_100M_1_peripheral_aresetn [get_bd_pins axi_intc_0/s_axi_aresetn] [get_bd_pins my_tsn_ip/s_axi_aresetn] -boundary_type upper
  connect_bd_net -net rst_clk_wiz_0_100M_1_peripheral_aresetn [get_bd_pins axi_intc_0/s_axi_aresetn] [get_bd_pins ps8_0_axi_periph/ARESETN] -boundary_type upper
  connect_bd_net -net rst_clk_wiz_0_100M_1_peripheral_aresetn [get_bd_pins axi_intc_0/s_axi_aresetn] [get_bd_pins ps8_0_axi_periph/M00_ARESETN] -boundary_type upper
  connect_bd_net -net rst_clk_wiz_0_100M_1_peripheral_aresetn [get_bd_pins axi_intc_0/s_axi_aresetn] [get_bd_pins ps8_0_axi_periph/M02_ARESETN] -boundary_type upper
  connect_bd_net -net rst_clk_wiz_0_100M_1_peripheral_aresetn [get_bd_pins axi_intc_0/s_axi_aresetn] [get_bd_pins ps8_0_axi_periph/M03_ARESETN] -boundary_type upper
  connect_bd_net -net rst_clk_wiz_0_100M_1_peripheral_aresetn [get_bd_pins axi_intc_0/s_axi_aresetn] [get_bd_pins ps8_0_axi_periph/M04_ARESETN] -boundary_type upper
  connect_bd_net -net rst_clk_wiz_0_100M_1_peripheral_aresetn [get_bd_pins axi_intc_0/s_axi_aresetn] [get_bd_pins ps8_0_axi_periph/M05_ARESETN] -boundary_type upper
  connect_bd_net -net rst_clk_wiz_0_100M_1_peripheral_aresetn [get_bd_pins axi_intc_0/s_axi_aresetn] [get_bd_pins ps8_0_axi_periph/S00_ARESETN] -boundary_type upper
  connect_bd_net -net rst_clk_wiz_0_100M_1_peripheral_aresetn [get_bd_pins axi_intc_0/s_axi_aresetn] [get_bd_pins test_pmod_controller_0/s00_axi_aresetn] -boundary_type upper
  connect_bd_net -net rst_clk_wiz_0_100M_1_peripheral_aresetn [get_bd_pins axi_intc_0/s_axi_aresetn] [get_bd_pins rst_clk_wiz_0_100M/peripheral_aresetn] -boundary_type upper
  connect_bd_net -net rst_clk_wiz_0_100M_1_peripheral_aresetn [get_bd_pins axi_intc_0/s_axi_aresetn] [get_bd_pins ta_dma_0/global_resetn] -boundary_type upper
  connect_bd_net -net rst_clk_wiz_0_100M_peripheral_aresetn [get_bd_pins axi_smc/aresetn] [get_bd_pins axis_switch_0/aresetn] -boundary_type upper
  connect_bd_net -net rst_clk_wiz_0_100M_peripheral_aresetn [get_bd_pins axi_smc/aresetn] [get_bd_pins ps8_0_axi_periph/M01_ARESETN] -boundary_type upper
  connect_bd_net -net rst_clk_wiz_0_100M_peripheral_aresetn [get_bd_pins axi_smc/aresetn] [get_bd_pins rst_clk_wiz_0_200M/peripheral_aresetn] -boundary_type upper
  connect_bd_net -net rx_0_1 [get_bd_ports UART_0_rxd] [get_bd_pins axi_uartlite_0/rx]
  connect_bd_net -net rx_b_and_l_v_r [get_bd_pins test_pmod_controller_0/in_rx_b] [get_bd_pins rx_b/and_l_v_r]
  connect_bd_net -net rx_s_and_l_v_r [get_bd_pins test_pmod_controller_0/in_rx_s] [get_bd_pins rx_s/and_l_v_r]
  connect_bd_net -net ta_dma_0_introut [get_bd_pins interrupts_concat_0/In0] [get_bd_pins ta_dma_0/introut]
  connect_bd_net -net ta_dma_0_m_axis_st_tlast [get_bd_pins my_tsn_ip/tx_axis_st_tlast] [get_bd_pins ta_dma_0/m_axis_st_tlast] -boundary_type upper
  connect_bd_net -net ta_dma_0_m_axis_st_tlast [get_bd_pins my_tsn_ip/tx_axis_st_tlast] [get_bd_pins tx_s/tlast] -boundary_type upper
  connect_bd_net -net ta_dma_0_m_axis_st_tvalid [get_bd_pins my_tsn_ip/tx_axis_st_tvalid] [get_bd_pins ta_dma_0/m_axis_st_tvalid] -boundary_type upper
  connect_bd_net -net ta_dma_0_m_axis_st_tvalid [get_bd_pins my_tsn_ip/tx_axis_st_tvalid] [get_bd_pins tx_s/tvalid] -boundary_type upper
  connect_bd_net -net tready_Res [get_bd_pins test_pmod_controller_0/in_tx_s] [get_bd_pins tx_s/and_l_v_r]
  connect_bd_net -net tsn_endpoint_ethernet_mac_0_ep_tadma_if_ep_tadma_if_out [get_bd_pins my_tsn_ip/ep_tadma_if_out] [get_bd_pins xlslice_12/Din] -boundary_type upper
  connect_bd_net -net tsn_endpoint_ethernet_mac_0_ep_tadma_if_ep_tadma_if_out [get_bd_pins my_tsn_ip/ep_tadma_if_out] [get_bd_pins xlslice_13/Din] -boundary_type upper
  connect_bd_net -net tsn_endpoint_ethernet_mac_0_ep_tadma_if_ep_tadma_if_out [get_bd_pins my_tsn_ip/ep_tadma_if_out] [get_bd_pins xlslice_14/Din] -boundary_type upper
  connect_bd_net -net tsn_endpoint_ethernet_mac_0_ep_tadma_if_ep_tadma_if_out [get_bd_pins my_tsn_ip/ep_tadma_if_out] [get_bd_pins xlslice_15/Din] -boundary_type upper
  connect_bd_net -net tsn_endpoint_ethernet_mac_0_ep_tadma_if_ep_tadma_if_out [get_bd_pins my_tsn_ip/ep_tadma_if_out] [get_bd_pins xlslice_16/Din] -boundary_type upper
  connect_bd_net -net tsn_endpoint_ethernet_mac_0_interrupt_ptp_rx_1 [get_bd_pins interrupts_concat_1/In0] [get_bd_pins my_tsn_ip/interrupt_ptp_rx_1]
  connect_bd_net -net tsn_endpoint_ethernet_mac_0_interrupt_ptp_rx_2 [get_bd_pins interrupts_concat_1/In1] [get_bd_pins my_tsn_ip/interrupt_ptp_rx_2]
  connect_bd_net -net tsn_endpoint_ethernet_mac_0_interrupt_ptp_timer [get_bd_pins interrupts_concat_1/In6] [get_bd_pins my_tsn_ip/interrupt_ptp_timer]
  connect_bd_net -net tsn_endpoint_ethernet_mac_0_interrupt_ptp_tx_1 [get_bd_pins interrupts_concat_1/In2] [get_bd_pins my_tsn_ip/interrupt_ptp_tx_1]
  connect_bd_net -net tsn_endpoint_ethernet_mac_0_interrupt_ptp_tx_2 [get_bd_pins interrupts_concat_1/In3] [get_bd_pins my_tsn_ip/interrupt_ptp_tx_2]
  connect_bd_net -net tsn_endpoint_ethernet_mac_0_mac_irq_1 [get_bd_pins interrupts_concat_1/In4] [get_bd_pins my_tsn_ip/mac_irq_1]
  connect_bd_net -net tsn_endpoint_ethernet_mac_0_mac_irq_2 [get_bd_pins interrupts_concat_1/In5] [get_bd_pins my_tsn_ip/mac_irq_2]
  connect_bd_net -net tsn_endpoint_ethernet_mac_0_ptp_timers_syntonised_nanosec_field [get_bd_pins my_tsn_ip/ptp_timers_syntonised_nanosec_field] [get_bd_pins xlconcat_7/In0]
  connect_bd_net -net tsn_endpoint_ethernet_mac_0_ptp_timers_syntonised_sec_field [get_bd_pins my_tsn_ip/ptp_timers_syntonised_sec_field] [get_bd_pins xlconcat_7/In1]
  connect_bd_net -net tsn_endpoint_ethernet_mac_0_rtc_nanosec_field [get_bd_pins my_tsn_ip/ptp_timers_ns_field] [get_bd_pins xlslice_10/Din] -boundary_type upper
  connect_bd_net -net tsn_endpoint_ethernet_mac_0_rtc_nanosec_field [get_bd_pins my_tsn_ip/ptp_timers_ns_field] [get_bd_pins xlslice_11/Din] -boundary_type upper
  connect_bd_net -net tsn_endpoint_ethernet_mac_0_rtc_nanosec_field [get_bd_pins my_tsn_ip/ptp_timers_ns_field] [get_bd_pins xlslice_7/Din] -boundary_type upper
  connect_bd_net -net tsn_endpoint_ethernet_mac_0_rtc_nanosec_field [get_bd_pins my_tsn_ip/ptp_timers_ns_field] [get_bd_pins xlslice_8/Din] -boundary_type upper
  connect_bd_net -net tsn_endpoint_ethernet_mac_0_rtc_nanosec_field [get_bd_pins my_tsn_ip/ptp_timers_ns_field] [get_bd_pins xlslice_9/Din] -boundary_type upper
  connect_bd_net -net tsn_endpoint_ethernet_mac_0_rtc_sec_field [get_bd_pins my_tsn_ip/ptp_timers_s_field] [get_bd_pins xlslice_6/Din]
  connect_bd_net -net tsn_endpoint_ethernet_mac_0_tsn_debug_hooks [get_bd_pins my_tsn_ip/misc_out_tsn_debug_hooks] [get_bd_pins xlslice_1/Din] -boundary_type upper
  connect_bd_net -net tsn_endpoint_ethernet_mac_0_tsn_debug_hooks [get_bd_pins my_tsn_ip/misc_out_tsn_debug_hooks] [get_bd_pins xlslice_2/Din] -boundary_type upper
  connect_bd_net -net tsn_endpoint_ethernet_mac_0_tsn_debug_hooks [get_bd_pins my_tsn_ip/misc_out_tsn_debug_hooks] [get_bd_pins xlslice_3/Din] -boundary_type upper
  connect_bd_net -net tsn_endpoint_ethernet_mac_0_tsn_debug_hooks [get_bd_pins my_tsn_ip/misc_out_tsn_debug_hooks] [get_bd_pins xlslice_4/Din] -boundary_type upper
  connect_bd_net -net tsn_endpoint_ethernet_mac_0_tsn_debug_hooks [get_bd_pins my_tsn_ip/misc_out_tsn_debug_hooks] [get_bd_pins xlslice_5/Din] -boundary_type upper
  connect_bd_net -net tsn_endpoint_ethernet_mac_0_tsn_ep_scheduler_irq [get_bd_pins interrupts_concat_1/In7] [get_bd_pins my_tsn_ip/tsn_ep_scheduler_irq]
  connect_bd_net -net tx_b_and_l_v_r [get_bd_pins test_pmod_controller_0/in_tx_b] [get_bd_pins tx_b/and_l_v_r]
  connect_bd_net -net xlconcat_0_dout [get_bd_pins axis_switch_0/s_axis_tlast] [get_bd_pins xlconcat_tlast/dout]
  connect_bd_net -net xlconcat_1_dout [get_bd_pins axis_switch_0/s_axis_tvalid] [get_bd_pins xlconcat_tvld/dout]
  connect_bd_net -net xlconcat_2_dout [get_bd_pins axi_intc_0/intr] [get_bd_pins xlconcat_2/dout]
  connect_bd_net -net xlconcat_7_dout [get_bd_pins ta_dma_0/current_time] [get_bd_pins xlconcat_7/dout]
  connect_bd_net -net xlconstant_0_dout [get_bd_ports Phy_reset_n] [get_bd_pins xlconstant_0/dout]
  connect_bd_net -net xlconstant_1_dout [get_bd_pins test_pmod_controller_0/in_rx_x] [get_bd_pins test_pmod_controller_0/in_tx_x] -boundary_type upper
  connect_bd_net -net xlconstant_1_dout [get_bd_pins test_pmod_controller_0/in_rx_x] [get_bd_pins xlconstant_1/dout] -boundary_type upper
  connect_bd_net -net xlconstant_2_dout [get_bd_pins axis_switch_0/s_req_suppress] [get_bd_pins xlconstant_2/dout]
  connect_bd_net -net xlconstant_4_dout [get_bd_pins ta_dma_0/m_axis_be_tready] [get_bd_pins ta_dma_0/m_axis_res_tready] -boundary_type upper
  connect_bd_net -net xlconstant_4_dout [get_bd_pins ta_dma_0/m_axis_be_tready] [get_bd_pins xlconstant_4/dout] -boundary_type upper
  connect_bd_net -net xlslice_0_Dout [get_bd_pins PS_0/pl_resetn0] [get_bd_pins proc_sys_reset_0/ext_reset_in] -boundary_type upper
  connect_bd_net -net xlslice_0_Dout [get_bd_pins PS_0/pl_resetn0] [get_bd_pins rst_clk_wiz_0_100M/ext_reset_in] -boundary_type upper
  connect_bd_net -net xlslice_0_Dout [get_bd_pins PS_0/pl_resetn0] [get_bd_pins rst_clk_wiz_0_125M/ext_reset_in] -boundary_type upper
  connect_bd_net -net xlslice_0_Dout [get_bd_pins PS_0/pl_resetn0] [get_bd_pins rst_clk_wiz_0_200M/ext_reset_in] -boundary_type upper
  connect_bd_net -net xlslice_0_Dout [get_bd_pins PS_0/pl_resetn0] [get_bd_pins rst_clk_wiz_0_300M/ext_reset_in] -boundary_type upper
  connect_bd_net -net xlslice_0_Dout1 [get_bd_pins my_tsn_ip/rx_axis_st_tready] [get_bd_pins rx_s/tready] -boundary_type upper
  connect_bd_net -net xlslice_0_Dout1 [get_bd_pins my_tsn_ip/rx_axis_st_tready] [get_bd_pins xlslice_trdy_0/Dout] -boundary_type upper
  connect_bd_net -net xlslice_12_Dout [get_bd_pins ta_dma_0/st_byte_space_in_fifo] [get_bd_pins xlslice_12/Dout]
  connect_bd_net -net xlslice_13_Dout [get_bd_pins ta_dma_0/new_schedule_start] [get_bd_pins xlslice_13/Dout]
  connect_bd_net -net xlslice_14_Dout [get_bd_pins ta_dma_0/re_byte_space_in_fifo] [get_bd_pins xlslice_14/Dout]
  connect_bd_net -net xlslice_15_Dout [get_bd_pins ta_dma_0/be_byte_space_in_fifo] [get_bd_pins xlslice_15/Dout]
  connect_bd_net -net xlslice_16_Dout [get_bd_pins ta_dma_0/cycle_start] [get_bd_pins xlslice_16/Dout]
  connect_bd_net -net xlslice_17_Dout [get_bd_pins my_tsn_ip/rx_axis_be_tready] [get_bd_pins rx_b/tready] -boundary_type upper
  connect_bd_net -net xlslice_17_Dout [get_bd_pins my_tsn_ip/rx_axis_be_tready] [get_bd_pins xlslice_trdy_1/Dout] -boundary_type upper
  connect_bd_net -net xlslice_ttc_0_Dout [get_bd_ports fan_en_b] [get_bd_pins xlslice_ttc_0/Dout]

  # Create address segments
  assign_bd_address -offset 0x80020000 -range 0x00001000 -target_address_space [get_bd_addr_spaces PS_0/Data] [get_bd_addr_segs axi_intc_0/S_AXI/Reg] -force
  assign_bd_address -offset 0x80000000 -range 0x00001000 -target_address_space [get_bd_addr_spaces PS_0/Data] [get_bd_addr_segs axi_mcdma_0/S_AXI_LITE/Reg] -force
  assign_bd_address -offset 0x80010000 -range 0x00010000 -target_address_space [get_bd_addr_spaces PS_0/Data] [get_bd_addr_segs axi_uartlite_0/S_AXI/Reg] -force
  assign_bd_address -offset 0x90000000 -range 0x00800000 -target_address_space [get_bd_addr_spaces PS_0/Data] [get_bd_addr_segs ta_dma_0/S_AXI/Reg] -force
  assign_bd_address -offset 0x80040000 -range 0x00040000 -target_address_space [get_bd_addr_spaces PS_0/Data] [get_bd_addr_segs my_tsn_ip/s_axi/Reg] -force
  assign_bd_address -offset 0x80030000 -range 0x00010000 -target_address_space [get_bd_addr_spaces PS_0/Data] [get_bd_addr_segs test_pmod_controller_0/S00_AXI/S00_AXI_reg] -force
  
  assign_bd_address -offset 0x00000000 -range 0x80000000 -target_address_space [get_bd_addr_spaces axi_mcdma_0/Data_SG] [get_bd_addr_segs PS_0/SAXIGP2/HP0_DDR_LOW] -force
  assign_bd_address -offset 0x00000000 -range 0x80000000 -target_address_space [get_bd_addr_spaces axi_mcdma_0/Data_MM2S] [get_bd_addr_segs PS_0/SAXIGP2/HP0_DDR_LOW] -force
  assign_bd_address -offset 0x00000000 -range 0x80000000 -target_address_space [get_bd_addr_spaces axi_mcdma_0/Data_S2MM] [get_bd_addr_segs PS_0/SAXIGP2/HP0_DDR_LOW] -force
  assign_bd_address -offset 0xC0000000 -range 0x20000000 -target_address_space [get_bd_addr_spaces axi_mcdma_0/Data_SG] [get_bd_addr_segs PS_0/SAXIGP2/HP0_QSPI] -force
  assign_bd_address -offset 0xC0000000 -range 0x20000000 -target_address_space [get_bd_addr_spaces axi_mcdma_0/Data_MM2S] [get_bd_addr_segs PS_0/SAXIGP2/HP0_QSPI] -force
  assign_bd_address -offset 0xC0000000 -range 0x20000000 -target_address_space [get_bd_addr_spaces axi_mcdma_0/Data_S2MM] [get_bd_addr_segs PS_0/SAXIGP2/HP0_QSPI] -force
  assign_bd_address -offset 0x00000000 -range 0x80000000 -target_address_space [get_bd_addr_spaces ta_dma_0/Data] [get_bd_addr_segs PS_0/SAXIGP2/HP0_DDR_LOW] -force
  assign_bd_address -offset 0xC0000000 -range 0x20000000 -target_address_space [get_bd_addr_spaces ta_dma_0/Data] [get_bd_addr_segs PS_0/SAXIGP2/HP0_QSPI] -force
 
  # Restore current instance
  current_bd_instance $oldCurInst

  # Create PFM attributes
  set_property PFM_NAME {xilinx:kr260_tsn_rs485pmod:kr260_tsn_rs485pmod:1.0} [get_files [current_bd_design].bd]
  set_property PFM.AXI_PORT {M_AXI_HPM0_FPD {memport "M_AXI_GP" sptag "" memory "" is_range "false"} S_AXI_HPC0_FPD {memport "S_AXI_HP" sptag "HPC0" memory "PS_0 HPC0_DDR_LOW" is_range "false"} S_AXI_HPC1_FPD {memport "S_AXI_HP" sptag "HPC1" memory "PS_0 HPC1_DDR_LOW" is_range "false"} S_AXI_HP1_FPD {memport "S_AXI_HP" sptag "HP1" memory "PS_0 HP1_DDR_LOW" is_range "false"} S_AXI_HP2_FPD {memport "S_AXI_HP" sptag "HP2" memory "PS_0 HP2_DDR_LOW" is_range "false"} S_AXI_HP3_FPD {memport "S_AXI_HP" sptag "HP3" memory "PS_0 HP3_DDR_LOW" is_range "false"} S_AXI_LPD {memport "MIG" sptag "LPD" memory "PS_0 LPD_DDR_LOW" is_range "false"}} [get_bd_cells /PS_0]
  set_property PFM.IRQ {pl_ps_irq1 {id 0 range 7}} [get_bd_cells /PS_0]
  set_property PFM.CLOCK {clk_out1 {id "0" is_default "true" proc_sys_reset "/rst_clk_wiz_0_200M" status "fixed" freq_hz "200000000"} clk_out2 {id "1" is_default "false" proc_sys_reset "/rst_clk_wiz_0_125M" status "fixed" freq_hz "125000000"} clk_out3 {id "2" is_default "false" proc_sys_reset "/rst_clk_wiz_0_300M" status "fixed" freq_hz "300000000"} clk_out4 {id "3" is_default "false" proc_sys_reset "/rst_clk_wiz_0_100M" status "fixed" freq_hz "100000000"}} [get_bd_cells /clk_wiz_0]


  validate_bd_design
  save_bd_design
}
# End of create_root_design()


##################################################################
# MAIN FLOW
##################################################################

create_root_design ""
