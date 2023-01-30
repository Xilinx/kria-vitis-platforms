# Copyright (C) 2022, Advanced Micro Devices, Inc.
# SPDX-License-Identifier: Apache-2.0

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
  set_property CONFIG.C_SIZE {1} $and_last_valid


  # Create instance: and_ready, and set properties
  set and_ready [ create_bd_cell -type ip -vlnv xilinx.com:ip:util_vector_logic and_ready ]
  set_property CONFIG.C_SIZE {1} $and_ready


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
  set_property CONFIG.C_SIZE {1} $and_last_valid


  # Create instance: and_ready, and set properties
  set and_ready [ create_bd_cell -type ip -vlnv xilinx.com:ip:util_vector_logic and_ready ]
  set_property CONFIG.C_SIZE {1} $and_ready


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
  set_property CONFIG.C_SIZE {1} $and_last_valid


  # Create instance: and_ready, and set properties
  set and_ready [ create_bd_cell -type ip -vlnv xilinx.com:ip:util_vector_logic and_ready ]
  set_property CONFIG.C_SIZE {1} $and_ready


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
  set_property CONFIG.C_SIZE {1} $and_last_valid


  # Create instance: and_ready, and set properties
  set and_ready [ create_bd_cell -type ip -vlnv xilinx.com:ip:util_vector_logic and_ready ]
  set_property CONFIG.C_SIZE {1} $and_ready


  # Create port connections
  connect_bd_net -net Op2_1 [get_bd_pins tready] [get_bd_pins and_ready/Op2]
  connect_bd_net -net and_ready_Res [get_bd_pins and_l_v_r] [get_bd_pins and_ready/Res]
  connect_bd_net -net ta_dma_0_m_axis_st_tlast [get_bd_pins tlast] [get_bd_pins and_last_valid/Op1]
  connect_bd_net -net ta_dma_0_m_axis_st_tvalid [get_bd_pins tvalid] [get_bd_pins and_last_valid/Op2]
  connect_bd_net -net util_vector_logic_0_Res [get_bd_pins and_last_valid/Res] [get_bd_pins and_ready/Op1]

  # Restore current instance
  current_bd_instance $oldCurInst
}

# Hierarchical cell: gate_driver
proc create_hier_cell_gate_driver { parentCell nameHier } {

  variable script_folder

  if { $parentCell eq "" || $nameHier eq "" } {
     catch {common::send_gid_msg -ssname BD::TCL -id 2092 -severity "ERROR" "create_hier_cell_gate_driver() - Empty argument(s)!"}
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
  create_bd_pin -dir O gate_drive_phase_a
  create_bd_pin -dir O gate_drive_phase_b
  create_bd_pin -dir O gate_drive_phase_c
  create_bd_pin -dir I phase_lower_a
  create_bd_pin -dir I phase_lower_b
  create_bd_pin -dir I phase_lower_c
  create_bd_pin -dir I phase_upper_a
  create_bd_pin -dir I phase_upper_b
  create_bd_pin -dir I phase_upper_c
  create_bd_pin -dir I -type clk sys_clk
  create_bd_pin -dir I -type rst sys_rstn

  # Create instance: gate_driver_A, and set properties
  set gate_driver_A [ create_bd_cell -type ip -vlnv xilinx.com:user:gate_driver gate_driver_A ]

  # Create instance: gate_driver_B, and set properties
  set gate_driver_B [ create_bd_cell -type ip -vlnv xilinx.com:user:gate_driver gate_driver_B ]

  # Create instance: gate_driver_C, and set properties
  set gate_driver_C [ create_bd_cell -type ip -vlnv xilinx.com:user:gate_driver gate_driver_C ]

  # Create port connections
  connect_bd_net -net gate_driver_A_gate_drive [get_bd_pins gate_drive_phase_a] [get_bd_pins gate_driver_A/gate_drive]
  connect_bd_net -net gate_driver_B_gate_drive [get_bd_pins gate_drive_phase_b] [get_bd_pins gate_driver_B/gate_drive]
  connect_bd_net -net gate_driver_C_gate_drive [get_bd_pins gate_drive_phase_c] [get_bd_pins gate_driver_C/gate_drive]
  connect_bd_net -net phase_lower_0_1 [get_bd_pins phase_lower_a] [get_bd_pins gate_driver_A/phase_lower]
  connect_bd_net -net phase_lower_1_1 [get_bd_pins phase_lower_b] [get_bd_pins gate_driver_B/phase_lower]
  connect_bd_net -net phase_lower_2_1 [get_bd_pins phase_lower_c] [get_bd_pins gate_driver_C/phase_lower]
  connect_bd_net -net phase_upper_0_1 [get_bd_pins phase_upper_a] [get_bd_pins gate_driver_A/phase_upper]
  connect_bd_net -net phase_upper_1_1 [get_bd_pins phase_upper_b] [get_bd_pins gate_driver_B/phase_upper]
  connect_bd_net -net phase_upper_2_1 [get_bd_pins phase_upper_c] [get_bd_pins gate_driver_C/phase_upper]
  connect_bd_net -net zynq_ultra_ps_e_0_pl_clk0 [get_bd_pins sys_clk] [get_bd_pins gate_driver_A/sys_clk] -boundary_type upper
  connect_bd_net -net zynq_ultra_ps_e_0_pl_clk0 [get_bd_pins sys_clk] [get_bd_pins gate_driver_B/sys_clk] -boundary_type upper
  connect_bd_net -net zynq_ultra_ps_e_0_pl_clk0 [get_bd_pins sys_clk] [get_bd_pins gate_driver_C/sys_clk] -boundary_type upper
  connect_bd_net -net zynq_ultra_ps_e_0_pl_resetn0 [get_bd_pins sys_rstn] [get_bd_pins gate_driver_A/sys_rstn] -boundary_type upper
  connect_bd_net -net zynq_ultra_ps_e_0_pl_resetn0 [get_bd_pins sys_rstn] [get_bd_pins gate_driver_B/sys_rstn] -boundary_type upper
  connect_bd_net -net zynq_ultra_ps_e_0_pl_resetn0 [get_bd_pins sys_rstn] [get_bd_pins gate_driver_C/sys_rstn] -boundary_type upper

  # Restore current instance
  current_bd_instance $oldCurInst
}

# Hierarchical cell: broadcast_i
proc create_hier_cell_broadcast_i { parentCell nameHier } {

  variable script_folder

  if { $parentCell eq "" || $nameHier eq "" } {
     catch {common::send_gid_msg -ssname BD::TCL -id 2092 -severity "ERROR" "create_hier_cell_broadcast_i() - Empty argument(s)!"}
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
  create_bd_intf_pin -mode Master -vlnv xilinx.com:interface:axis_rtl:1.0 M00_AXIS0

  create_bd_intf_pin -mode Master -vlnv xilinx.com:interface:axis_rtl:1.0 M00_AXIS1

  create_bd_intf_pin -mode Master -vlnv xilinx.com:interface:axis_rtl:1.0 M00_AXIS2

  create_bd_intf_pin -mode Master -vlnv xilinx.com:interface:axis_rtl:1.0 M01_AXIS

  create_bd_intf_pin -mode Master -vlnv xilinx.com:interface:axis_rtl:1.0 M01_AXIS1

  create_bd_intf_pin -mode Master -vlnv xilinx.com:interface:axis_rtl:1.0 M01_AXIS2

  create_bd_intf_pin -mode Slave -vlnv xilinx.com:interface:axis_rtl:1.0 S_AXIS0

  create_bd_intf_pin -mode Slave -vlnv xilinx.com:interface:axis_rtl:1.0 S_AXIS1

  create_bd_intf_pin -mode Slave -vlnv xilinx.com:interface:axis_rtl:1.0 S_AXIS2


  # Create pins
  create_bd_pin -dir I -type clk aclk
  create_bd_pin -dir I -type rst aresetn

  # Create instance: axis_broadcaster_0, and set properties
  set axis_broadcaster_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:axis_broadcaster axis_broadcaster_0 ]

  # Create instance: axis_broadcaster_1, and set properties
  set axis_broadcaster_1 [ create_bd_cell -type ip -vlnv xilinx.com:ip:axis_broadcaster axis_broadcaster_1 ]

  # Create instance: axis_broadcaster_2, and set properties
  set axis_broadcaster_2 [ create_bd_cell -type ip -vlnv xilinx.com:ip:axis_broadcaster axis_broadcaster_2 ]

  # Create interface connections
  connect_bd_intf_net -intf_net Conn1 [get_bd_intf_pins S_AXIS0] [get_bd_intf_pins axis_broadcaster_0/S_AXIS]
  connect_bd_intf_net -intf_net Conn2 [get_bd_intf_pins S_AXIS1] [get_bd_intf_pins axis_broadcaster_1/S_AXIS]
  connect_bd_intf_net -intf_net Conn3 [get_bd_intf_pins S_AXIS2] [get_bd_intf_pins axis_broadcaster_2/S_AXIS]
  connect_bd_intf_net -intf_net Conn4 [get_bd_intf_pins M00_AXIS0] [get_bd_intf_pins axis_broadcaster_0/M00_AXIS]
  connect_bd_intf_net -intf_net Conn5 [get_bd_intf_pins M00_AXIS1] [get_bd_intf_pins axis_broadcaster_1/M00_AXIS]
  connect_bd_intf_net -intf_net Conn6 [get_bd_intf_pins M00_AXIS2] [get_bd_intf_pins axis_broadcaster_2/M00_AXIS]
  connect_bd_intf_net -intf_net Conn7 [get_bd_intf_pins M01_AXIS] [get_bd_intf_pins axis_broadcaster_0/M01_AXIS]
  connect_bd_intf_net -intf_net Conn8 [get_bd_intf_pins M01_AXIS1] [get_bd_intf_pins axis_broadcaster_1/M01_AXIS]
  connect_bd_intf_net -intf_net Conn9 [get_bd_intf_pins M01_AXIS2] [get_bd_intf_pins axis_broadcaster_2/M01_AXIS]

  # Create port connections
  connect_bd_net -net aclk_1 [get_bd_pins aclk] [get_bd_pins axis_broadcaster_0/aclk] -boundary_type upper
  connect_bd_net -net aclk_1 [get_bd_pins aclk] [get_bd_pins axis_broadcaster_1/aclk] -boundary_type upper
  connect_bd_net -net aclk_1 [get_bd_pins aclk] [get_bd_pins axis_broadcaster_2/aclk] -boundary_type upper
  connect_bd_net -net aresetn_1 [get_bd_pins aresetn] [get_bd_pins axis_broadcaster_0/aresetn] -boundary_type upper
  connect_bd_net -net aresetn_1 [get_bd_pins aresetn] [get_bd_pins axis_broadcaster_1/aresetn] -boundary_type upper
  connect_bd_net -net aresetn_1 [get_bd_pins aresetn] [get_bd_pins axis_broadcaster_2/aresetn] -boundary_type upper

  # Restore current instance
  current_bd_instance $oldCurInst
}

# Hierarchical cell: TSN_subsystem
proc create_hier_cell_TSN_subsystem { parentCell nameHier } {

  variable script_folder

  if { $parentCell eq "" || $nameHier eq "" } {
     catch {common::send_gid_msg -ssname BD::TCL -id 2092 -severity "ERROR" "create_hier_cell_TSN_subsystem() - Empty argument(s)!"}
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
  create_bd_intf_pin -mode Master -vlnv xilinx.com:interface:aximm_rtl:1.0 M00_AXI

  create_bd_intf_pin -mode Slave -vlnv xilinx.com:interface:aximm_rtl:1.0 S00_AXI

  create_bd_intf_pin -mode Master -vlnv xilinx.com:interface:mdio_io:1.0 mdio

  create_bd_intf_pin -mode Master -vlnv xilinx.com:interface:mdio_io:1.0 mdio2

  create_bd_intf_pin -mode Master -vlnv xilinx.com:interface:rgmii_rtl:1.0 rgmii

  create_bd_intf_pin -mode Master -vlnv xilinx.com:interface:rgmii_rtl:1.0 rgmii2


  # Create pins
  create_bd_pin -dir I -type clk CLK_IN_gem
  create_bd_pin -dir O -from 1 -to 0 Phy_reset_n
  create_bd_pin -dir O -type clk clk_out1
  create_bd_pin -dir O -type clk clk_out4
  create_bd_pin -dir I -type rst ext_reset_in
  create_bd_pin -dir O -type intr irq
  create_bd_pin -dir O lis1_rxs_0
  create_bd_pin -dir O lis2_rxb_0
  create_bd_pin -dir O lis3_rxx_0
  create_bd_pin -dir O ptp_timer
  create_bd_pin -dir O pub1_txs_0
  create_bd_pin -dir O pub2_txb_0
  create_bd_pin -dir O pub3_txx_0

  # Create instance: axi_intc_0, and set properties
  set axi_intc_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:axi_intc axi_intc_0 ]
  set_property -dict [list \
    CONFIG.C_CASCADE_MASTER {1} \
    CONFIG.C_EN_CASCADE_MODE {0} \
    CONFIG.C_IRQ_CONNECTION {1} \
  ] $axi_intc_0


  # Create instance: axi_mcdma_0, and set properties
  set axi_mcdma_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:axi_mcdma axi_mcdma_0 ]
  set_property -dict [list \
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
  set_property CONFIG.NUM_SI {4} $axi_smc


  # Create instance: axis_switch_0, and set properties
  set axis_switch_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:axis_switch axis_switch_0 ]
  set_property -dict [list \
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
  set_property -dict [list \
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
  set_property CONFIG.NUM_PORTS {7} $interrupts_concat_0


  # Create instance: interrupts_concat_1, and set properties
  set interrupts_concat_1 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlconcat interrupts_concat_1 ]
  set_property CONFIG.NUM_PORTS {8} $interrupts_concat_1


  # Create instance: my_tsn_ip, and set properties
  set my_tsn_ip [ create_bd_cell -type ip -vlnv xilinx.com:ip:tsn_endpoint_ethernet_mac my_tsn_ip ]
  set_property -dict [list \
    CONFIG.ADDR_TRANSLATION_TYPE {false} \
    CONFIG.ENABLE_EP_TADMA_INTERFACE {true} \
    CONFIG.EN_CB_RSVD_BYTES {true} \
    CONFIG.EN_COMMON_STATS {false} \
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
    CONFIG.PHYSICAL_INTERFACE {RGMII} \
    CONFIG.STORE_AND_FORWARD {1} \
    CONFIG.TSN_EP_Switch {1} \
    CONFIG.USE_HARD_FIFO {0} \
  ] $my_tsn_ip


  # Create instance: proc_sys_reset_0, and set properties
  set proc_sys_reset_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:proc_sys_reset proc_sys_reset_0 ]

  # Create instance: ps8_0_axi_periph, and set properties
  set ps8_0_axi_periph [ create_bd_cell -type ip -vlnv xilinx.com:ip:axi_interconnect ps8_0_axi_periph ]
  set_property -dict [list \
    CONFIG.M01_HAS_REGSLICE {3} \
    CONFIG.NUM_MI {5} \
  ] $ps8_0_axi_periph


  # Create instance: rst_clk_wiz_0_100M, and set properties
  set rst_clk_wiz_0_100M [ create_bd_cell -type ip -vlnv xilinx.com:ip:proc_sys_reset rst_clk_wiz_0_100M ]

  # Create instance: rst_clk_wiz_0_125M, and set properties
  set rst_clk_wiz_0_125M [ create_bd_cell -type ip -vlnv xilinx.com:ip:proc_sys_reset rst_clk_wiz_0_125M ]

  # Create instance: rst_clk_wiz_0_200M, and set properties
  set rst_clk_wiz_0_200M [ create_bd_cell -type ip -vlnv xilinx.com:ip:proc_sys_reset rst_clk_wiz_0_200M ]

  # Create instance: rst_clk_wiz_0_300M, and set properties
  set rst_clk_wiz_0_300M [ create_bd_cell -type ip -vlnv xilinx.com:ip:proc_sys_reset rst_clk_wiz_0_300M ]

  # Create instance: rx_b
  create_hier_cell_rx_b $hier_obj rx_b

  # Create instance: rx_s
  create_hier_cell_rx_s $hier_obj rx_s

  # Create instance: ta_dma_0, and set properties
  set ta_dma_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:ta_dma ta_dma_0 ]

  # Create instance: test_pmod_controller_0, and set properties
  set test_pmod_controller_0 [ create_bd_cell -type ip -vlnv user.org:user:test_pmod_controller test_pmod_controller_0 ]

  # Create instance: tx_b
  create_hier_cell_tx_b $hier_obj tx_b

  # Create instance: tx_s
  create_hier_cell_tx_s $hier_obj tx_s

  # Create instance: xlconcat_2, and set properties
  set xlconcat_2 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlconcat xlconcat_2 ]
  set_property CONFIG.NUM_PORTS {3} $xlconcat_2


  # Create instance: xlconcat_7, and set properties
  set xlconcat_7 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlconcat xlconcat_7 ]
  set_property -dict [list \
    CONFIG.IN0_WIDTH {32} \
    CONFIG.IN1_WIDTH {48} \
  ] $xlconcat_7


  # Create instance: xlconcat_tlast, and set properties
  set xlconcat_tlast [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlconcat xlconcat_tlast ]

  # Create instance: xlconcat_tvld, and set properties
  set xlconcat_tvld [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlconcat xlconcat_tvld ]

  # Create instance: xlconstant_0, and set properties
  set xlconstant_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlconstant xlconstant_0 ]
  set_property -dict [list \
    CONFIG.CONST_VAL {3} \
    CONFIG.CONST_WIDTH {2} \
  ] $xlconstant_0


  # Create instance: xlconstant_1, and set properties
  set xlconstant_1 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlconstant xlconstant_1 ]

  # Create instance: xlconstant_2, and set properties
  set xlconstant_2 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlconstant xlconstant_2 ]
  set_property -dict [list \
    CONFIG.CONST_VAL {0} \
    CONFIG.CONST_WIDTH {2} \
  ] $xlconstant_2


  # Create instance: xlconstant_4, and set properties
  set xlconstant_4 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlconstant xlconstant_4 ]

  # Create instance: xlslice_1, and set properties
  set xlslice_1 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlslice xlslice_1 ]
  set_property -dict [list \
    CONFIG.DIN_FROM {138} \
    CONFIG.DIN_TO {136} \
    CONFIG.DIN_WIDTH {512} \
    CONFIG.DOUT_WIDTH {1} \
  ] $xlslice_1


  # Create instance: xlslice_2, and set properties
  set xlslice_2 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlslice xlslice_2 ]
  set_property -dict [list \
    CONFIG.DIN_FROM {146} \
    CONFIG.DIN_TO {144} \
    CONFIG.DIN_WIDTH {512} \
    CONFIG.DOUT_WIDTH {1} \
  ] $xlslice_2


  # Create instance: xlslice_3, and set properties
  set xlslice_3 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlslice xlslice_3 ]
  set_property -dict [list \
    CONFIG.DIN_FROM {154} \
    CONFIG.DIN_TO {152} \
    CONFIG.DIN_WIDTH {512} \
    CONFIG.DOUT_WIDTH {1} \
  ] $xlslice_3


  # Create instance: xlslice_4, and set properties
  set xlslice_4 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlslice xlslice_4 ]
  set_property -dict [list \
    CONFIG.DIN_FROM {161} \
    CONFIG.DIN_TO {160} \
    CONFIG.DIN_WIDTH {512} \
    CONFIG.DOUT_WIDTH {1} \
  ] $xlslice_4


  # Create instance: xlslice_5, and set properties
  set xlslice_5 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlslice xlslice_5 ]
  set_property -dict [list \
    CONFIG.DIN_FROM {193} \
    CONFIG.DIN_TO {192} \
    CONFIG.DIN_WIDTH {512} \
    CONFIG.DOUT_WIDTH {1} \
  ] $xlslice_5


  # Create instance: xlslice_6, and set properties
  set xlslice_6 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlslice xlslice_6 ]
  set_property -dict [list \
    CONFIG.DIN_FROM {0} \
    CONFIG.DIN_TO {0} \
    CONFIG.DIN_WIDTH {48} \
    CONFIG.DOUT_WIDTH {1} \
  ] $xlslice_6


  # Create instance: xlslice_7, and set properties
  set xlslice_7 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlslice xlslice_7 ]
  set_property -dict [list \
    CONFIG.DIN_FROM {9} \
    CONFIG.DIN_TO {9} \
    CONFIG.DIN_WIDTH {32} \
    CONFIG.DOUT_WIDTH {1} \
  ] $xlslice_7


  # Create instance: xlslice_8, and set properties
  set xlslice_8 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlslice xlslice_8 ]
  set_property -dict [list \
    CONFIG.DIN_FROM {19} \
    CONFIG.DIN_TO {19} \
    CONFIG.DIN_WIDTH {32} \
    CONFIG.DOUT_WIDTH {1} \
  ] $xlslice_8


  # Create instance: xlslice_9, and set properties
  set xlslice_9 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlslice xlslice_9 ]
  set_property -dict [list \
    CONFIG.DIN_FROM {21} \
    CONFIG.DIN_TO {21} \
    CONFIG.DIN_WIDTH {32} \
    CONFIG.DOUT_WIDTH {1} \
  ] $xlslice_9


  # Create instance: xlslice_10, and set properties
  set xlslice_10 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlslice xlslice_10 ]
  set_property -dict [list \
    CONFIG.DIN_FROM {14} \
    CONFIG.DIN_TO {14} \
    CONFIG.DIN_WIDTH {32} \
    CONFIG.DOUT_WIDTH {1} \
  ] $xlslice_10


  # Create instance: xlslice_11, and set properties
  set xlslice_11 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlslice xlslice_11 ]
  set_property -dict [list \
    CONFIG.DIN_FROM {8} \
    CONFIG.DIN_TO {0} \
    CONFIG.DIN_WIDTH {32} \
    CONFIG.DOUT_WIDTH {9} \
  ] $xlslice_11


  # Create instance: xlslice_12, and set properties
  set xlslice_12 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlslice xlslice_12 ]
  set_property -dict [list \
    CONFIG.DIN_FROM {15} \
    CONFIG.DIN_TO {0} \
    CONFIG.DIN_WIDTH {64} \
    CONFIG.DOUT_WIDTH {16} \
  ] $xlslice_12


  # Create instance: xlslice_13, and set properties
  set xlslice_13 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlslice xlslice_13 ]
  set_property -dict [list \
    CONFIG.DIN_FROM {63} \
    CONFIG.DIN_TO {63} \
    CONFIG.DIN_WIDTH {64} \
    CONFIG.DOUT_WIDTH {1} \
  ] $xlslice_13


  # Create instance: xlslice_14, and set properties
  set xlslice_14 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlslice xlslice_14 ]
  set_property -dict [list \
    CONFIG.DIN_FROM {31} \
    CONFIG.DIN_TO {16} \
    CONFIG.DIN_WIDTH {64} \
    CONFIG.DOUT_WIDTH {16} \
  ] $xlslice_14


  # Create instance: xlslice_15, and set properties
  set xlslice_15 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlslice xlslice_15 ]
  set_property -dict [list \
    CONFIG.DIN_FROM {47} \
    CONFIG.DIN_TO {32} \
    CONFIG.DIN_WIDTH {64} \
    CONFIG.DOUT_WIDTH {1} \
  ] $xlslice_15


  # Create instance: xlslice_16, and set properties
  set xlslice_16 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlslice xlslice_16 ]
  set_property -dict [list \
    CONFIG.DIN_FROM {62} \
    CONFIG.DIN_TO {62} \
    CONFIG.DIN_WIDTH {64} \
    CONFIG.DOUT_WIDTH {1} \
  ] $xlslice_16


  # Create instance: xlslice_trdy_0, and set properties
  set xlslice_trdy_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlslice xlslice_trdy_0 ]
  set_property CONFIG.DIN_WIDTH {2} $xlslice_trdy_0


  # Create instance: xlslice_trdy_1, and set properties
  set xlslice_trdy_1 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlslice xlslice_trdy_1 ]
  set_property -dict [list \
    CONFIG.DIN_FROM {1} \
    CONFIG.DIN_TO {1} \
    CONFIG.DIN_WIDTH {2} \
    CONFIG.DOUT_WIDTH {1} \
  ] $xlslice_trdy_1


  # Create interface connections
  connect_bd_intf_net -intf_net PS_0_M_AXI_HPM0_LPD [get_bd_intf_pins S00_AXI] [get_bd_intf_pins ps8_0_axi_periph/S00_AXI]
  connect_bd_intf_net -intf_net axi_mcdma_0_M_AXIS_MM2S [get_bd_intf_pins axi_mcdma_0/M_AXIS_MM2S] [get_bd_intf_pins my_tsn_ip/tx_axis_be]
  connect_bd_intf_net -intf_net axi_mcdma_0_M_AXI_MM2S [get_bd_intf_pins axi_mcdma_0/M_AXI_MM2S] [get_bd_intf_pins axi_smc/S01_AXI]
  connect_bd_intf_net -intf_net axi_mcdma_0_M_AXI_S2MM [get_bd_intf_pins axi_mcdma_0/M_AXI_S2MM] [get_bd_intf_pins axi_smc/S02_AXI]
  connect_bd_intf_net -intf_net axi_mcdma_0_M_AXI_SG [get_bd_intf_pins axi_mcdma_0/M_AXI_SG] [get_bd_intf_pins axi_smc/S00_AXI]
  connect_bd_intf_net -intf_net axi_smc_M00_AXI [get_bd_intf_pins M00_AXI] [get_bd_intf_pins axi_smc/M00_AXI]
  connect_bd_intf_net -intf_net axis_switch_0_M00_AXIS [get_bd_intf_pins axi_mcdma_0/S_AXIS_S2MM] [get_bd_intf_pins axis_switch_0/M00_AXIS]
  connect_bd_intf_net -intf_net my_tsn_ip_rx_axis_be [get_bd_intf_pins axis_switch_0/S01_AXIS] [get_bd_intf_pins my_tsn_ip/rx_axis_be]
  connect_bd_intf_net -intf_net ps8_0_axi_periph_M00_AXI [get_bd_intf_pins axi_mcdma_0/S_AXI_LITE] [get_bd_intf_pins ps8_0_axi_periph/M00_AXI]
  connect_bd_intf_net -intf_net ps8_0_axi_periph_M01_AXI [get_bd_intf_pins ps8_0_axi_periph/M01_AXI] [get_bd_intf_pins ta_dma_0/S_AXI]
  connect_bd_intf_net -intf_net ps8_0_axi_periph_M02_AXI [get_bd_intf_pins my_tsn_ip/s_axi] [get_bd_intf_pins ps8_0_axi_periph/M02_AXI]
  connect_bd_intf_net -intf_net ps8_0_axi_periph_M03_AXI [get_bd_intf_pins axi_intc_0/s_axi] [get_bd_intf_pins ps8_0_axi_periph/M03_AXI]
  connect_bd_intf_net -intf_net ps8_0_axi_periph_M04_AXI [get_bd_intf_pins ps8_0_axi_periph/M04_AXI] [get_bd_intf_pins test_pmod_controller_0/S00_AXI]
  connect_bd_intf_net -intf_net ta_dma_0_M_AXI [get_bd_intf_pins axi_smc/S03_AXI] [get_bd_intf_pins ta_dma_0/M_AXI]
  connect_bd_intf_net -intf_net ta_dma_0_M_AXIS_ST_INTF [get_bd_intf_pins my_tsn_ip/tx_axis_st] [get_bd_intf_pins ta_dma_0/M_AXIS_ST_INTF]
  connect_bd_intf_net -intf_net tsn_endpoint_ethernet_mac_0_mdio_external1 [get_bd_intf_pins mdio] [get_bd_intf_pins my_tsn_ip/mdio_external1]
  connect_bd_intf_net -intf_net tsn_endpoint_ethernet_mac_0_mdio_external2 [get_bd_intf_pins mdio2] [get_bd_intf_pins my_tsn_ip/mdio_external2]
  connect_bd_intf_net -intf_net tsn_endpoint_ethernet_mac_0_rgmii1 [get_bd_intf_pins rgmii] [get_bd_intf_pins my_tsn_ip/rgmii1]
  connect_bd_intf_net -intf_net tsn_endpoint_ethernet_mac_0_rgmii2 [get_bd_intf_pins rgmii2] [get_bd_intf_pins my_tsn_ip/rgmii2]
  connect_bd_intf_net -intf_net tsn_endpoint_ethernet_mac_0_rx_axis_st [get_bd_intf_pins axis_switch_0/S00_AXIS] [get_bd_intf_pins my_tsn_ip/rx_axis_st]

  # Create port connections
  connect_bd_net -net Op2_1 [get_bd_pins my_tsn_ip/tx_axis_st_tready] [get_bd_pins ta_dma_0/m_axis_st_tready] -boundary_type upper
  connect_bd_net -net Op2_1 [get_bd_pins my_tsn_ip/tx_axis_st_tready] [get_bd_pins tx_s/tready] -boundary_type upper
  connect_bd_net -net axi_intc_0_irq [get_bd_pins irq] [get_bd_pins axi_intc_0/irq]
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
  connect_bd_net -net axis_switch_0_s_axis_tready [get_bd_pins axis_switch_0/s_axis_tready] [get_bd_pins xlslice_trdy_0/Din] -boundary_type upper
  connect_bd_net -net axis_switch_0_s_axis_tready [get_bd_pins axis_switch_0/s_axis_tready] [get_bd_pins xlslice_trdy_1/Din] -boundary_type upper
  connect_bd_net -net clk_in1_0_1 [get_bd_pins CLK_IN_gem] [get_bd_pins clk_wiz_0/clk_in1]
  connect_bd_net -net clk_wiz_0_clk_out1 [get_bd_pins clk_out1] [get_bd_pins axi_mcdma_0/s_axi_aclk] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_out1 [get_bd_pins clk_out1] [get_bd_pins axi_smc/aclk] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_out1 [get_bd_pins clk_out1] [get_bd_pins axis_switch_0/aclk] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_out1 [get_bd_pins clk_out1] [get_bd_pins clk_wiz_0/clk_out1] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_out1 [get_bd_pins clk_out1] [get_bd_pins my_tsn_ip/host_rxfifo_aclk] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_out1 [get_bd_pins clk_out1] [get_bd_pins my_tsn_ip/host_txfifo_aclk] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_out1 [get_bd_pins clk_out1] [get_bd_pins ps8_0_axi_periph/M01_ACLK] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_out1 [get_bd_pins clk_out1] [get_bd_pins rst_clk_wiz_0_200M/slowest_sync_clk] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_out1 [get_bd_pins clk_out1] [get_bd_pins ta_dma_0/dma_clk] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_out2 [get_bd_pins clk_wiz_0/clk_out2] [get_bd_pins my_tsn_ip/gtx_clk] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_out2 [get_bd_pins clk_wiz_0/clk_out2] [get_bd_pins my_tsn_ip/gtx_clk90] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_out2 [get_bd_pins clk_wiz_0/clk_out2] [get_bd_pins rst_clk_wiz_0_125M/slowest_sync_clk] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_out2 [get_bd_pins clk_wiz_0/clk_out2] [get_bd_pins ta_dma_0/rtc_clk] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_out3 [get_bd_pins clk_wiz_0/clk_out3] [get_bd_pins my_tsn_ip/refclk] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_out3 [get_bd_pins clk_wiz_0/clk_out3] [get_bd_pins rst_clk_wiz_0_300M/slowest_sync_clk] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_out4 [get_bd_pins clk_out4] [get_bd_pins axi_intc_0/s_axi_aclk] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_out4 [get_bd_pins clk_out4] [get_bd_pins axi_mcdma_0/s_axi_lite_aclk] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_out4 [get_bd_pins clk_out4] [get_bd_pins clk_wiz_0/clk_out4] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_out4 [get_bd_pins clk_out4] [get_bd_pins my_tsn_ip/s_axi_aclk] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_out4 [get_bd_pins clk_out4] [get_bd_pins ps8_0_axi_periph/ACLK] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_out4 [get_bd_pins clk_out4] [get_bd_pins ps8_0_axi_periph/M00_ACLK] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_out4 [get_bd_pins clk_out4] [get_bd_pins ps8_0_axi_periph/M02_ACLK] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_out4 [get_bd_pins clk_out4] [get_bd_pins ps8_0_axi_periph/M03_ACLK] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_out4 [get_bd_pins clk_out4] [get_bd_pins ps8_0_axi_periph/M04_ACLK] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_out4 [get_bd_pins clk_out4] [get_bd_pins ps8_0_axi_periph/S00_ACLK] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_out4 [get_bd_pins clk_out4] [get_bd_pins rst_clk_wiz_0_100M/slowest_sync_clk] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_out4 [get_bd_pins clk_out4] [get_bd_pins test_pmod_controller_0/s00_axi_aclk] -boundary_type upper
  connect_bd_net -net clk_wiz_0_locked [get_bd_pins clk_wiz_0/locked] [get_bd_pins my_tsn_ip/gtx_dcm_locked] -boundary_type upper
  connect_bd_net -net clk_wiz_0_locked [get_bd_pins clk_wiz_0/locked] [get_bd_pins rst_clk_wiz_0_100M/dcm_locked] -boundary_type upper
  connect_bd_net -net clk_wiz_0_locked [get_bd_pins clk_wiz_0/locked] [get_bd_pins rst_clk_wiz_0_125M/dcm_locked] -boundary_type upper
  connect_bd_net -net clk_wiz_0_locked [get_bd_pins clk_wiz_0/locked] [get_bd_pins rst_clk_wiz_0_200M/dcm_locked] -boundary_type upper
  connect_bd_net -net clk_wiz_0_locked [get_bd_pins clk_wiz_0/locked] [get_bd_pins rst_clk_wiz_0_300M/dcm_locked] -boundary_type upper
  connect_bd_net -net interrupts_concat_0_dout [get_bd_pins interrupts_concat_0/dout] [get_bd_pins xlconcat_2/In1]
  connect_bd_net -net interrupts_concat_1_dout [get_bd_pins interrupts_concat_1/dout] [get_bd_pins xlconcat_2/In0]
  connect_bd_net -net my_tsn_ip_ptp_timers_clk8k [get_bd_pins ptp_timer] [get_bd_pins my_tsn_ip/ptp_timers_clk8k] -boundary_type upper
  connect_bd_net -net my_tsn_ip_ptp_timers_clk8k [get_bd_pins ptp_timer] [get_bd_pins proc_sys_reset_0/slowest_sync_clk] -boundary_type upper
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
  connect_bd_net -net rst_clk_wiz_0_100M_1_peripheral_aresetn [get_bd_pins axi_intc_0/s_axi_aresetn] [get_bd_pins axi_mcdma_0/axi_resetn] -boundary_type upper
  connect_bd_net -net rst_clk_wiz_0_100M_1_peripheral_aresetn [get_bd_pins axi_intc_0/s_axi_aresetn] [get_bd_pins my_tsn_ip/glbl_rstn] -boundary_type upper
  connect_bd_net -net rst_clk_wiz_0_100M_1_peripheral_aresetn [get_bd_pins axi_intc_0/s_axi_aresetn] [get_bd_pins my_tsn_ip/s_axi_aresetn] -boundary_type upper
  connect_bd_net -net rst_clk_wiz_0_100M_1_peripheral_aresetn [get_bd_pins axi_intc_0/s_axi_aresetn] [get_bd_pins ps8_0_axi_periph/ARESETN] -boundary_type upper
  connect_bd_net -net rst_clk_wiz_0_100M_1_peripheral_aresetn [get_bd_pins axi_intc_0/s_axi_aresetn] [get_bd_pins ps8_0_axi_periph/M00_ARESETN] -boundary_type upper
  connect_bd_net -net rst_clk_wiz_0_100M_1_peripheral_aresetn [get_bd_pins axi_intc_0/s_axi_aresetn] [get_bd_pins ps8_0_axi_periph/M02_ARESETN] -boundary_type upper
  connect_bd_net -net rst_clk_wiz_0_100M_1_peripheral_aresetn [get_bd_pins axi_intc_0/s_axi_aresetn] [get_bd_pins ps8_0_axi_periph/M03_ARESETN] -boundary_type upper
  connect_bd_net -net rst_clk_wiz_0_100M_1_peripheral_aresetn [get_bd_pins axi_intc_0/s_axi_aresetn] [get_bd_pins ps8_0_axi_periph/M04_ARESETN] -boundary_type upper
  connect_bd_net -net rst_clk_wiz_0_100M_1_peripheral_aresetn [get_bd_pins axi_intc_0/s_axi_aresetn] [get_bd_pins ps8_0_axi_periph/S00_ARESETN] -boundary_type upper
  connect_bd_net -net rst_clk_wiz_0_100M_1_peripheral_aresetn [get_bd_pins axi_intc_0/s_axi_aresetn] [get_bd_pins rst_clk_wiz_0_100M/peripheral_aresetn] -boundary_type upper
  connect_bd_net -net rst_clk_wiz_0_100M_1_peripheral_aresetn [get_bd_pins axi_intc_0/s_axi_aresetn] [get_bd_pins ta_dma_0/global_resetn] -boundary_type upper
  connect_bd_net -net rst_clk_wiz_0_100M_1_peripheral_aresetn [get_bd_pins axi_intc_0/s_axi_aresetn] [get_bd_pins test_pmod_controller_0/s00_axi_aresetn] -boundary_type upper
  connect_bd_net -net rst_clk_wiz_0_100M_peripheral_aresetn [get_bd_pins axi_smc/aresetn] [get_bd_pins axis_switch_0/aresetn] -boundary_type upper
  connect_bd_net -net rst_clk_wiz_0_100M_peripheral_aresetn [get_bd_pins axi_smc/aresetn] [get_bd_pins ps8_0_axi_periph/M01_ARESETN] -boundary_type upper
  connect_bd_net -net rst_clk_wiz_0_100M_peripheral_aresetn [get_bd_pins axi_smc/aresetn] [get_bd_pins rst_clk_wiz_0_200M/peripheral_aresetn] -boundary_type upper
  connect_bd_net -net rx_b_and_l_v_r [get_bd_pins rx_b/and_l_v_r] [get_bd_pins test_pmod_controller_0/in_rx_b]
  connect_bd_net -net rx_s_and_l_v_r [get_bd_pins rx_s/and_l_v_r] [get_bd_pins test_pmod_controller_0/in_rx_s]
  connect_bd_net -net ta_dma_0_introut [get_bd_pins interrupts_concat_0/In0] [get_bd_pins ta_dma_0/introut]
  connect_bd_net -net ta_dma_0_m_axis_st_tlast [get_bd_pins my_tsn_ip/tx_axis_st_tlast] [get_bd_pins ta_dma_0/m_axis_st_tlast] -boundary_type upper
  connect_bd_net -net ta_dma_0_m_axis_st_tlast [get_bd_pins my_tsn_ip/tx_axis_st_tlast] [get_bd_pins tx_s/tlast] -boundary_type upper
  connect_bd_net -net ta_dma_0_m_axis_st_tvalid [get_bd_pins my_tsn_ip/tx_axis_st_tvalid] [get_bd_pins ta_dma_0/m_axis_st_tvalid] -boundary_type upper
  connect_bd_net -net ta_dma_0_m_axis_st_tvalid [get_bd_pins my_tsn_ip/tx_axis_st_tvalid] [get_bd_pins tx_s/tvalid] -boundary_type upper
  connect_bd_net -net test_pmod_controller_0_lis1_rxs [get_bd_pins lis1_rxs_0] [get_bd_pins test_pmod_controller_0/lis1_rxs]
  connect_bd_net -net test_pmod_controller_0_lis2_rxb [get_bd_pins lis2_rxb_0] [get_bd_pins test_pmod_controller_0/lis2_rxb]
  connect_bd_net -net test_pmod_controller_0_lis3_rxx [get_bd_pins lis3_rxx_0] [get_bd_pins test_pmod_controller_0/lis3_rxx]
  connect_bd_net -net test_pmod_controller_0_pub1_txs [get_bd_pins pub1_txs_0] [get_bd_pins test_pmod_controller_0/pub1_txs]
  connect_bd_net -net test_pmod_controller_0_pub2_txb [get_bd_pins pub2_txb_0] [get_bd_pins test_pmod_controller_0/pub2_txb]
  connect_bd_net -net test_pmod_controller_0_pub3_txx [get_bd_pins pub3_txx_0] [get_bd_pins test_pmod_controller_0/pub3_txx]
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
  connect_bd_net -net xlconstant_0_dout [get_bd_pins Phy_reset_n] [get_bd_pins xlconstant_0/dout]
  connect_bd_net -net xlconstant_1_dout [get_bd_pins test_pmod_controller_0/in_rx_x] [get_bd_pins test_pmod_controller_0/in_tx_x] -boundary_type upper
  connect_bd_net -net xlconstant_1_dout [get_bd_pins test_pmod_controller_0/in_rx_x] [get_bd_pins xlconstant_1/dout] -boundary_type upper
  connect_bd_net -net xlconstant_2_dout [get_bd_pins axis_switch_0/s_req_suppress] [get_bd_pins xlconstant_2/dout]
  connect_bd_net -net xlconstant_4_dout [get_bd_pins ta_dma_0/m_axis_be_tready] [get_bd_pins ta_dma_0/m_axis_res_tready] -boundary_type upper
  connect_bd_net -net xlconstant_4_dout [get_bd_pins ta_dma_0/m_axis_be_tready] [get_bd_pins xlconstant_4/dout] -boundary_type upper
  connect_bd_net -net xlslice_0_Dout [get_bd_pins ext_reset_in] [get_bd_pins proc_sys_reset_0/ext_reset_in] -boundary_type upper
  connect_bd_net -net xlslice_0_Dout [get_bd_pins ext_reset_in] [get_bd_pins rst_clk_wiz_0_100M/ext_reset_in] -boundary_type upper
  connect_bd_net -net xlslice_0_Dout [get_bd_pins ext_reset_in] [get_bd_pins rst_clk_wiz_0_125M/ext_reset_in] -boundary_type upper
  connect_bd_net -net xlslice_0_Dout [get_bd_pins ext_reset_in] [get_bd_pins rst_clk_wiz_0_200M/ext_reset_in] -boundary_type upper
  connect_bd_net -net xlslice_0_Dout [get_bd_pins ext_reset_in] [get_bd_pins rst_clk_wiz_0_300M/ext_reset_in] -boundary_type upper
  connect_bd_net -net xlslice_0_Dout1 [get_bd_pins my_tsn_ip/rx_axis_st_tready] [get_bd_pins rx_s/tready] -boundary_type upper
  connect_bd_net -net xlslice_0_Dout1 [get_bd_pins my_tsn_ip/rx_axis_st_tready] [get_bd_pins xlslice_trdy_0/Dout] -boundary_type upper
  connect_bd_net -net xlslice_12_Dout [get_bd_pins ta_dma_0/st_byte_space_in_fifo] [get_bd_pins xlslice_12/Dout]
  connect_bd_net -net xlslice_13_Dout [get_bd_pins ta_dma_0/new_schedule_start] [get_bd_pins xlslice_13/Dout]
  connect_bd_net -net xlslice_14_Dout [get_bd_pins ta_dma_0/re_byte_space_in_fifo] [get_bd_pins xlslice_14/Dout]
  connect_bd_net -net xlslice_15_Dout [get_bd_pins ta_dma_0/be_byte_space_in_fifo] [get_bd_pins xlslice_15/Dout]
  connect_bd_net -net xlslice_16_Dout [get_bd_pins ta_dma_0/cycle_start] [get_bd_pins xlslice_16/Dout]
  connect_bd_net -net xlslice_17_Dout [get_bd_pins my_tsn_ip/rx_axis_be_tready] [get_bd_pins rx_b/tready] -boundary_type upper
  connect_bd_net -net xlslice_17_Dout [get_bd_pins my_tsn_ip/rx_axis_be_tready] [get_bd_pins xlslice_trdy_1/Dout] -boundary_type upper

  # Restore current instance
  current_bd_instance $oldCurInst
}

# Hierarchical cell: ADC
proc create_hier_cell_ADC { parentCell nameHier } {

  variable script_folder

  if { $parentCell eq "" || $nameHier eq "" } {
     catch {common::send_gid_msg -ssname BD::TCL -id 2092 -severity "ERROR" "create_hier_cell_ADC() - Empty argument(s)!"}
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
  create_bd_intf_pin -mode Slave -vlnv xilinx.com:interface:aximm_rtl:1.0 S_AXI

  create_bd_intf_pin -mode Master -vlnv xilinx.com:interface:axis_rtl:1.0 dc_i_AXIS

  create_bd_intf_pin -mode Master -vlnv xilinx.com:interface:axis_rtl:1.0 pa_i_AXIS

  create_bd_intf_pin -mode Master -vlnv xilinx.com:interface:axis_rtl:1.0 pb_i_AXIS

  create_bd_intf_pin -mode Master -vlnv xilinx.com:interface:axis_rtl:1.0 pc_i_AXIS


  # Create pins
  create_bd_pin -dir I -type clk CLK
  create_bd_pin -dir O -from 7 -to 0 CSn
  create_bd_pin -dir I -type rst RESETn
  create_bd_pin -dir I -type clk SCLK
  create_bd_pin -dir I -type rst SCLK_RESETn
  create_bd_pin -dir I -from 7 -to 0 SDATA
  create_bd_pin -dir I UPDATE
  create_bd_pin -dir O -from 0 -to 0 dc_link_oc_err
  create_bd_pin -dir O -from 0 -to 0 dc_link_ov_err
  create_bd_pin -dir O -type intr interrupt
  create_bd_pin -dir O -from 0 -to 0 phase_a_oc_err
  create_bd_pin -dir O -from 0 -to 0 phase_b_oc_err
  create_bd_pin -dir O -from 0 -to 0 phase_c_oc_err

  # Create instance: adc_hub_phase_dc, and set properties
  set adc_hub_phase_dc [ create_bd_cell -type ip -vlnv xilinx.com:user:adc_hub adc_hub_phase_dc ]
  set_property -dict [list \
   CONFIG.L0_TYPE {VOLTAGE} \
   CONFIG.L1_TYPE {CURRENT} \
   CONFIG.L2_TYPE {VOLTAGE} \
   CONFIG.L3_TYPE {CURRENT} \
   CONFIG.L4_TYPE {VOLTAGE} \
   CONFIG.L5_TYPE {CURRENT} \
   CONFIG.L6_TYPE {VOLTAGE} \
   CONFIG.L7_TYPE {CURRENT} \
   CONFIG.L0_CODE {BTC} \
   CONFIG.L1_CODE {BTC} \
   CONFIG.L2_CODE {BTC} \
   CONFIG.L3_CODE {BTC} \
   CONFIG.L4_CODE {BTC} \
   CONFIG.L5_CODE {BTC} \
   CONFIG.L6_CODE {BTC} \
   CONFIG.L7_CODE {BTC} \
   CONFIG.NUM_CHANNELS {8} \
  ] $adc_hub_phase_dc


  # Create instance: adc_if, and set properties
  set adc_if [ create_bd_cell -type ip -vlnv xilinx.com:user:adc7352_if adc_if ]
  set_property CONFIG.NUM_CHANNELS {8} $adc_if

  # Create instance: adc_usb2btc, and set properties
  set adc_usb2btc [ create_bd_cell -type ip -vlnv xilinx.com:user:adc_usb2btc adc_usb2btc ]
  set_property CONFIG.NUM_CHANNELS {8} $adc_usb2btc

  # Create instance: adc_bob2btc, and set properties
  set adc_bob2btc [ create_bd_cell -type ip -vlnv xilinx.com:user:adc_bob2btc:1.0 adc_bob2btc ]
  set_property CONFIG.NUM_CHANNELS {8} $adc_bob2btc

  # Create instance: xlslice_oc_err_0, and set properties
  set xlslice_oc_err_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlslice xlslice_oc_err_0 ]
  set_property -dict [list \
    CONFIG.DIN_FROM {0} \
    CONFIG.DIN_TO {0} \
    CONFIG.DIN_WIDTH {8} \
    CONFIG.DOUT_WIDTH {1} \
  ] $xlslice_oc_err_0


  # Create instance: xlslice_oc_err_1, and set properties
  set xlslice_oc_err_1 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlslice xlslice_oc_err_1 ]
  set_property -dict [list \
    CONFIG.DIN_FROM {1} \
    CONFIG.DIN_TO {1} \
    CONFIG.DIN_WIDTH {8} \
    CONFIG.DOUT_WIDTH {1} \
  ] $xlslice_oc_err_1


  # Create instance: xlslice_oc_err_2, and set properties
  set xlslice_oc_err_2 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlslice xlslice_oc_err_2 ]
  set_property -dict [list \
    CONFIG.DIN_FROM {2} \
    CONFIG.DIN_TO {2} \
    CONFIG.DIN_WIDTH {8} \
  ] $xlslice_oc_err_2


  # Create instance: xlslice_oc_err_3, and set properties
  set xlslice_oc_err_3 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlslice xlslice_oc_err_3 ]
  set_property -dict [list \
    CONFIG.DIN_FROM {3} \
    CONFIG.DIN_TO {3} \
    CONFIG.DIN_WIDTH {8} \
    CONFIG.DOUT_WIDTH {1} \
  ] $xlslice_oc_err_3


  # Create instance: xlslice_oc_err_7, and set properties
  set xlslice_oc_err_7 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlslice xlslice_oc_err_7 ]
  set_property -dict [list \
    CONFIG.DIN_FROM {7} \
    CONFIG.DIN_TO {7} \
    CONFIG.DIN_WIDTH {8} \
    CONFIG.DOUT_WIDTH {1} \
  ] $xlslice_oc_err_7


  # Create interface connections
  connect_bd_intf_net -intf_net Conn1 [get_bd_intf_pins pc_i_AXIS] [get_bd_intf_pins adc_hub_phase_dc/L4_AXIS]
  connect_bd_intf_net -intf_net Conn2 [get_bd_intf_pins dc_i_AXIS] [get_bd_intf_pins adc_hub_phase_dc/L6_AXIS]
  connect_bd_intf_net -intf_net adc_hub_0_L2_AXIS [get_bd_intf_pins pb_i_AXIS] [get_bd_intf_pins adc_hub_phase_dc/L2_AXIS]
  connect_bd_intf_net -intf_net adc_hub_1_L0_AXIS [get_bd_intf_pins pa_i_AXIS] [get_bd_intf_pins adc_hub_phase_dc/L0_AXIS]
  connect_bd_intf_net -intf_net adc_if_L0_ADC [get_bd_intf_pins adc_if/L0_ADC] [get_bd_intf_pins adc_usb2btc/L0_ADC]
  connect_bd_intf_net -intf_net adc_if_L1_ADC [get_bd_intf_pins adc_if/L1_ADC] [get_bd_intf_pins adc_bob2btc/L1_ADC]
  connect_bd_intf_net -intf_net adc_if_L2_ADC [get_bd_intf_pins adc_if/L2_ADC] [get_bd_intf_pins adc_usb2btc/L2_ADC]
  connect_bd_intf_net -intf_net adc_if_L3_ADC [get_bd_intf_pins adc_if/L3_ADC] [get_bd_intf_pins adc_bob2btc/L3_ADC]
  connect_bd_intf_net -intf_net adc_if_L4_ADC [get_bd_intf_pins adc_if/L4_ADC] [get_bd_intf_pins adc_usb2btc/L4_ADC]
  connect_bd_intf_net -intf_net adc_if_L5_ADC [get_bd_intf_pins adc_if/L5_ADC] [get_bd_intf_pins adc_bob2btc/L5_ADC]
  connect_bd_intf_net -intf_net adc_if_L6_ADC [get_bd_intf_pins adc_if/L6_ADC] [get_bd_intf_pins adc_usb2btc/L6_ADC]
  connect_bd_intf_net -intf_net adc_if_L7_ADC [get_bd_intf_pins adc_if/L7_ADC] [get_bd_intf_pins adc_usb2btc/L7_ADC]
  connect_bd_intf_net -intf_net adc_usb2btc_L0_SYS [get_bd_intf_pins adc_hub_phase_dc/L0_ADC] [get_bd_intf_pins adc_usb2btc/L0_SYS]
  connect_bd_intf_net -intf_net adc_bob2btc_L1_SYS [get_bd_intf_pins adc_hub_phase_dc/L1_ADC] [get_bd_intf_pins adc_bob2btc/L1_SYS]
  connect_bd_intf_net -intf_net adc_usb2btc_L2_SYS [get_bd_intf_pins adc_hub_phase_dc/L2_ADC] [get_bd_intf_pins adc_usb2btc/L2_SYS]
  connect_bd_intf_net -intf_net adc_bob2btc_L3_SYS [get_bd_intf_pins adc_hub_phase_dc/L3_ADC] [get_bd_intf_pins adc_bob2btc/L3_SYS]
  connect_bd_intf_net -intf_net adc_usb2btc_L4_SYS [get_bd_intf_pins adc_hub_phase_dc/L4_ADC] [get_bd_intf_pins adc_usb2btc/L4_SYS]
  connect_bd_intf_net -intf_net adc_bob2btc_L5_SYS [get_bd_intf_pins adc_hub_phase_dc/L5_ADC] [get_bd_intf_pins adc_bob2btc/L5_SYS]
  connect_bd_intf_net -intf_net adc_usb2btc_L6_SYS [get_bd_intf_pins adc_hub_phase_dc/L6_ADC] [get_bd_intf_pins adc_usb2btc/L6_SYS]
  connect_bd_intf_net -intf_net adc_usb2btc_L7_SYS [get_bd_intf_pins adc_hub_phase_dc/L7_ADC] [get_bd_intf_pins adc_usb2btc/L7_SYS]
  connect_bd_intf_net -intf_net axi_interconnect_0_M00_AXI [get_bd_intf_pins S_AXI] [get_bd_intf_pins adc_hub_phase_dc/S_AXI]

  # Create port connections
  connect_bd_net -net SDATA_0_1 [get_bd_pins SDATA] [get_bd_pins adc_if/SDATA]
  connect_bd_net -net adc_hub_1_interrupt [get_bd_pins interrupt] [get_bd_pins adc_hub_phase_dc/interrupt]
  connect_bd_net -net adc_hub_1_over_fault [get_bd_pins adc_hub_phase_dc/over_fault] [get_bd_pins xlslice_oc_err_0/Din] -boundary_type upper
  connect_bd_net -net adc_hub_1_over_fault [get_bd_pins adc_hub_phase_dc/over_fault] [get_bd_pins xlslice_oc_err_1/Din] -boundary_type upper
  connect_bd_net -net adc_hub_1_over_fault [get_bd_pins adc_hub_phase_dc/over_fault] [get_bd_pins xlslice_oc_err_2/Din] -boundary_type upper
  connect_bd_net -net adc_hub_1_over_fault [get_bd_pins adc_hub_phase_dc/over_fault] [get_bd_pins xlslice_oc_err_3/Din] -boundary_type upper
  connect_bd_net -net adc_hub_1_over_fault [get_bd_pins adc_hub_phase_dc/over_fault] [get_bd_pins xlslice_oc_err_7/Din] -boundary_type upper
  connect_bd_net -net adc_if_0_CSn [get_bd_pins CSn] [get_bd_pins adc_if/CSn]
  connect_bd_net -net clk_wiz_0_clk_out_20M1 [get_bd_pins SCLK] [get_bd_pins adc_if/SCLK]
  connect_bd_net -net phase_a_i_Dout3 [get_bd_pins phase_b_oc_err] [get_bd_pins xlslice_oc_err_1/Dout]
  connect_bd_net -net phase_b_i_Dout3 [get_bd_pins phase_a_oc_err] [get_bd_pins xlslice_oc_err_0/Dout]
  connect_bd_net -net proc_sys_reset_1_peripheral_aresetn [get_bd_pins SCLK_RESETn] [get_bd_pins adc_if/SCLK_RESETn]
  connect_bd_net -net xlconstant_1_dout [get_bd_pins UPDATE] [get_bd_pins adc_if/UPDATE]
  connect_bd_net -net xlslice_oc_err_2_Dout [get_bd_pins phase_c_oc_err] [get_bd_pins xlslice_oc_err_2/Dout]
  connect_bd_net -net xlslice_oc_err_3_Dout [get_bd_pins dc_link_oc_err] [get_bd_pins xlslice_oc_err_3/Dout]
  connect_bd_net -net xlslice_oc_err_4_Dout [get_bd_pins dc_link_ov_err] [get_bd_pins xlslice_oc_err_7/Dout]
  connect_bd_net -net zynq_ultra_ps_e_0_pl_clk0 [get_bd_pins CLK] [get_bd_pins adc_hub_phase_dc/S_AXI_ACLK] -boundary_type upper
  connect_bd_net -net zynq_ultra_ps_e_0_pl_clk0 [get_bd_pins CLK] [get_bd_pins adc_if/CLK] -boundary_type upper
  connect_bd_net -net zynq_ultra_ps_e_0_pl_clk0 [get_bd_pins CLK] [get_bd_pins adc_usb2btc/CLK] -boundary_type upper
  connect_bd_net -net zynq_ultra_ps_e_0_pl_clk0 [get_bd_pins CLK] [get_bd_pins adc_bob2btc/CLK] -boundary_type upper
  connect_bd_net -net zynq_ultra_ps_e_0_pl_resetn0 [get_bd_pins RESETn] [get_bd_pins adc_hub_phase_dc/S_AXI_ARESETN] -boundary_type upper
  connect_bd_net -net zynq_ultra_ps_e_0_pl_resetn0 [get_bd_pins RESETn] [get_bd_pins adc_if/RESETn] -boundary_type upper
  connect_bd_net -net zynq_ultra_ps_e_0_pl_resetn0 [get_bd_pins RESETn] [get_bd_pins adc_usb2btc/RESETn] -boundary_type upper
  connect_bd_net -net zynq_ultra_ps_e_0_pl_resetn0 [get_bd_pins RESETn] [get_bd_pins adc_bob2btc/RESETn] -boundary_type upper

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
  set QEI_A [ create_bd_port -dir I QEI_A ]
  set QEI_B [ create_bd_port -dir I QEI_B ]
  set QEI_I [ create_bd_port -dir I QEI_I ]
  set dc_link_adc_cs_n [ create_bd_port -dir O -from 0 -to 0 dc_link_adc_cs_n ]
  set dc_link_adc_sclk [ create_bd_port -dir O -type clk dc_link_adc_sclk ]
  set dc_link_data_i [ create_bd_port -dir I -from 0 -to 0 dc_link_data_i ]
  set dc_link_data_v [ create_bd_port -dir I -from 0 -to 0 dc_link_data_v ]
  set fan_en_b [ create_bd_port -dir O -from 0 -to 0 fan_en_b ]
  set gate_drive_en [ create_bd_port -dir O -from 0 -to 0 gate_drive_en ]
  set gate_drive_phase_a [ create_bd_port -dir O gate_drive_phase_a ]
  set gate_drive_phase_b [ create_bd_port -dir O gate_drive_phase_b ]
  set gate_drive_phase_c [ create_bd_port -dir O gate_drive_phase_c ]
  set l1_rxs [ create_bd_port -dir O l1_rxs ]
  set l2_rxb [ create_bd_port -dir O l2_rxb ]
  set l3_rxx [ create_bd_port -dir O l3_rxx ]
  set motor_adc_cs_n [ create_bd_port -dir O -from 0 -to 0 motor_adc_cs_n ]
  set motor_adc_sclk [ create_bd_port -dir O -type clk motor_adc_sclk ]
  set motor_pa_data_i [ create_bd_port -dir I -from 0 -to 0 motor_pa_data_i ]
  set motor_pa_data_v [ create_bd_port -dir I -from 0 -to 0 motor_pa_data_v ]
  set motor_pb_data_i [ create_bd_port -dir I -from 0 -to 0 motor_pb_data_i ]
  set motor_pb_data_v [ create_bd_port -dir I -from 0 -to 0 motor_pb_data_v ]
  set motor_pc_data_i [ create_bd_port -dir I -from 0 -to 0 motor_pc_data_i ]
  set motor_pc_data_v [ create_bd_port -dir I -from 0 -to 0 motor_pc_data_v ]
  set p1_txs [ create_bd_port -dir O p1_txs ]
  set p2_txb [ create_bd_port -dir O p2_txb ]
  set p3_txx [ create_bd_port -dir O p3_txx ]
  set ptp_timer [ create_bd_port -dir O ptp_timer ]

  # Create instance: ADC
  create_hier_cell_ADC [current_bd_instance .] ADC

# Create instance: PS_0, and set properties
  set PS_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:zynq_ultra_ps_e PS_0 ]
  apply_bd_automation -rule xilinx.com:bd_rule:zynq_ultra_ps_e -config {apply_board_preset "1" }  [get_bd_cells PS_0]
  set_property -dict [ list \
   CONFIG.PSU__USE__IRQ0 {1} \
   CONFIG.PSU__USE__IRQ1 {1} \
   CONFIG.PSU__NUM_FABRIC_RESETS {2} \
   CONFIG.PSU__USE__M_AXI_GP0 {1} \
   CONFIG.PSU__USE__M_AXI_GP1 {0} \
   CONFIG.PSU__USE__M_AXI_GP2 {1} \
   CONFIG.PSU__USE__S_AXI_GP2 {1} \
   CONFIG.PSU__MAXIGP0__DATA_WIDTH {32} \
   CONFIG.PSU__MAXIGP2__DATA_WIDTH {32} \
   CONFIG.PSU__SAXIGP2__DATA_WIDTH {128} \
   CONFIG.PSU__FPGA_PL1_ENABLE {1} \
   CONFIG.PSU__GPIO_EMIO__PERIPHERAL__ENABLE {1}  \
   CONFIG.PSU__TTC0__WAVEOUT__ENABLE {1} \
   CONFIG.PSU__TTC0__WAVEOUT__IO {EMIO} \
   CONFIG.PSU_MIO_45_PULLUPDOWN {disable} \
   CONFIG.PSU_MIO_46_PULLUPDOWN {disable} \
   CONFIG.PSU_MIO_47_PULLUPDOWN {disable} \
   CONFIG.PSU_MIO_48_PULLUPDOWN {disable} \
 ] $PS_0


  # Create instance: TSN_subsystem
  create_hier_cell_TSN_subsystem [current_bd_instance .] TSN_subsystem

  # Create instance: axi_interconnect_cntrl, and set properties
  set axi_interconnect_cntrl [ create_bd_cell -type ip -vlnv xilinx.com:ip:axi_interconnect axi_interconnect_cntrl ]
  set_property CONFIG.NUM_MI {6} $axi_interconnect_cntrl


  # Create instance: broadcast_i
  create_hier_cell_broadcast_i [current_bd_instance .] broadcast_i

  # Create instance: clk_wiz_0, and set properties
  set clk_wiz_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:clk_wiz clk_wiz_0 ]
  set_property -dict [list \
    CONFIG.CLKOUT1_REQUESTED_OUT_FREQ {100} \
    CONFIG.CLKOUT2_JITTER {162.169} \
    CONFIG.CLKOUT2_PHASE_ERROR {87.181} \
    CONFIG.CLKOUT2_REQUESTED_OUT_FREQ {20} \
    CONFIG.CLKOUT2_USED {true} \
    CONFIG.CLK_OUT1_PORT {clk_out_100M} \
    CONFIG.CLK_OUT2_PORT {clk_out_20M} \
    CONFIG.MMCM_CLKOUT1_DIVIDE {60} \
    CONFIG.NUM_OUT_CLKS {2} \
    CONFIG.PRIM_SOURCE {Global_buffer} \
    CONFIG.RESET_PORT {resetn} \
    CONFIG.RESET_TYPE {ACTIVE_LOW} \
  ] $clk_wiz_0


  # Create instance: gate_driver
  create_hier_cell_gate_driver [current_bd_instance .] gate_driver

  # Create instance: hls_foc_periodic_top_0, and set properties
  set hls_foc_periodic_top_0 [ create_bd_cell -type ip -vlnv xilinx.com:hls:hls_foc_periodic_top_ap_fixed hls_foc_periodic_top_0 ]

  # Create instance: hls_pwm_gen_0, and set properties
  set hls_pwm_gen_0 [ create_bd_cell -type ip -vlnv xilinx.com:hls:hls_pwm_gen hls_pwm_gen_0 ]

  # Create instance: hls_qei_top_0, and set properties
  set hls_qei_top_0 [ create_bd_cell -type ip -vlnv xilinx.com:hls:hls_qei_top hls_qei_top_0 ]

  # Create instance: hls_svpwm_duty_0, and set properties
  set hls_svpwm_duty_0 [ create_bd_cell -type ip -vlnv xilinx.com:hls:hls_svpwm_duty hls_svpwm_duty_0 ]

  # Create instance: motor_control_0, and set properties
  set motor_control_0 [ create_bd_cell -type ip -vlnv user.org:user:motor_control motor_control_0 ]

  # Create instance: proc_sys_reset_0, and set properties
  set proc_sys_reset_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:proc_sys_reset proc_sys_reset_0 ]

  # Create instance: proc_sys_reset_1, and set properties
  set proc_sys_reset_1 [ create_bd_cell -type ip -vlnv xilinx.com:ip:proc_sys_reset proc_sys_reset_1 ]

  # Create instance: xlconcat_0, and set properties
  set xlconcat_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlconcat xlconcat_0 ]
  set_property CONFIG.NUM_PORTS {8} $xlconcat_0


  # Create instance: xlconcat_int, and set properties
  set xlconcat_int [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlconcat xlconcat_int ]
  set_property CONFIG.NUM_PORTS {3} $xlconcat_int


  # Create instance: xlconstant_1, and set properties
  set xlconstant_1 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlconstant xlconstant_1 ]

  # Create instance: xlslice_CSn, and set properties
  set xlslice_CSn [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlslice xlslice_CSn ]
  set_property CONFIG.DIN_WIDTH {8} $xlslice_CSn


  # Create instance: xlslice_dc_CSn, and set properties
  set xlslice_dc_CSn [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlslice xlslice_dc_CSn ]
  set_property -dict [list \
    CONFIG.DIN_FROM {7} \
    CONFIG.DIN_TO {7} \
    CONFIG.DIN_WIDTH {8} \
  ] $xlslice_dc_CSn


  # Create instance: xlslice_fan, and set properties
  set xlslice_fan [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlslice xlslice_fan ]
  set_property -dict [list \
    CONFIG.DIN_FROM {2} \
    CONFIG.DIN_TO {2} \
    CONFIG.DIN_WIDTH {3} \
  ] $xlslice_fan


  # Create interface connections
  connect_bd_intf_net -intf_net ADC_L0_AXIS [get_bd_intf_pins ADC/pa_i_AXIS] [get_bd_intf_pins broadcast_i/S_AXIS0]
  connect_bd_intf_net -intf_net ADC_dc_i_AXIS [get_bd_intf_pins ADC/dc_i_AXIS] [get_bd_intf_pins hls_svpwm_duty_0/strm_dc_link]
  connect_bd_intf_net -intf_net S_AXIS1_1 [get_bd_intf_pins ADC/pb_i_AXIS] [get_bd_intf_pins broadcast_i/S_AXIS1]
  connect_bd_intf_net -intf_net S_AXIS2_1 [get_bd_intf_pins ADC/pc_i_AXIS] [get_bd_intf_pins broadcast_i/S_AXIS2]
  connect_bd_intf_net -intf_net TSN_subsystem_M00_AXI [get_bd_intf_pins PS_0/S_AXI_HP0_FPD] [get_bd_intf_pins TSN_subsystem/M00_AXI]
  connect_bd_intf_net -intf_net axi_interconnect_0_M00_AXI [get_bd_intf_pins ADC/S_AXI] [get_bd_intf_pins axi_interconnect_cntrl/M00_AXI]
  connect_bd_intf_net -intf_net axi_interconnect_cntrl_M01_AXI [get_bd_intf_pins axi_interconnect_cntrl/M01_AXI] [get_bd_intf_pins hls_qei_top_0/s_axi_qei_args]
  connect_bd_intf_net -intf_net axi_interconnect_cntrl_M02_AXI [get_bd_intf_pins axi_interconnect_cntrl/M02_AXI] [get_bd_intf_pins motor_control_0/s_axi_cntrl]
  connect_bd_intf_net -intf_net axi_interconnect_cntrl_M03_AXI [get_bd_intf_pins axi_interconnect_cntrl/M03_AXI] [get_bd_intf_pins hls_foc_periodic_top_0/s_axi_foc_args]
  connect_bd_intf_net -intf_net axi_interconnect_cntrl_M04_AXI [get_bd_intf_pins axi_interconnect_cntrl/M04_AXI] [get_bd_intf_pins hls_pwm_gen_0/s_axi_pwm_args]
  connect_bd_intf_net -intf_net axi_interconnect_cntrl_M05_AXI [get_bd_intf_pins axi_interconnect_cntrl/M05_AXI] [get_bd_intf_pins hls_svpwm_duty_0/s_axi_pwm_args]
  connect_bd_intf_net -intf_net broadcast_i_M00_AXIS [get_bd_intf_pins broadcast_i/M00_AXIS0] [get_bd_intf_pins motor_control_0/s_axis_phase_a_i]
  connect_bd_intf_net -intf_net broadcast_i_M00_AXIS1 [get_bd_intf_pins broadcast_i/M00_AXIS1] [get_bd_intf_pins motor_control_0/s_axis_phase_b_i]
  connect_bd_intf_net -intf_net broadcast_i_M00_AXIS2 [get_bd_intf_pins broadcast_i/M00_AXIS2] [get_bd_intf_pins motor_control_0/s_axis_phase_c_i]
  connect_bd_intf_net -intf_net broadcast_i_M01_AXIS [get_bd_intf_pins broadcast_i/M01_AXIS] [get_bd_intf_pins hls_foc_periodic_top_0/Ia]
  connect_bd_intf_net -intf_net broadcast_i_M01_AXIS1 [get_bd_intf_pins broadcast_i/M01_AXIS1] [get_bd_intf_pins hls_foc_periodic_top_0/Ib]
  connect_bd_intf_net -intf_net broadcast_i_M01_AXIS2 [get_bd_intf_pins broadcast_i/M01_AXIS2] [get_bd_intf_pins hls_foc_periodic_top_0/Ic]
  connect_bd_intf_net -intf_net hls_foc_periodic_top_0_Va_cmd [get_bd_intf_pins hls_foc_periodic_top_0/Va_cmd] [get_bd_intf_pins hls_svpwm_duty_0/strm_Va_cmd]
  connect_bd_intf_net -intf_net hls_foc_periodic_top_0_Vb_cmd [get_bd_intf_pins hls_foc_periodic_top_0/Vb_cmd] [get_bd_intf_pins hls_svpwm_duty_0/strm_Vb_cmd]
  connect_bd_intf_net -intf_net hls_foc_periodic_top_0_Vc_cmd [get_bd_intf_pins hls_foc_periodic_top_0/Vc_cmd] [get_bd_intf_pins hls_svpwm_duty_0/strm_Vc_cmd]
  connect_bd_intf_net -intf_net hls_qei_top_0_qei_RPM_THETA_m [get_bd_intf_pins hls_foc_periodic_top_0/SPEED_THETA_m] [get_bd_intf_pins hls_qei_top_0/qei_RPM_THETA_m]
  connect_bd_intf_net -intf_net hls_svpwm_duty_0_strm_duty_ratio_a [get_bd_intf_pins hls_pwm_gen_0/strm_duty_ratio_a] [get_bd_intf_pins hls_svpwm_duty_0/strm_duty_ratio_a]
  connect_bd_intf_net -intf_net hls_svpwm_duty_0_strm_duty_ratio_b [get_bd_intf_pins hls_pwm_gen_0/strm_duty_ratio_b] [get_bd_intf_pins hls_svpwm_duty_0/strm_duty_ratio_b]
  connect_bd_intf_net -intf_net hls_svpwm_duty_0_strm_duty_ratio_c [get_bd_intf_pins hls_pwm_gen_0/strm_duty_ratio_c] [get_bd_intf_pins hls_svpwm_duty_0/strm_duty_ratio_c]
  connect_bd_intf_net -intf_net tsn_endpoint_ethernet_mac_0_mdio_external1 [get_bd_intf_ports mdio] [get_bd_intf_pins TSN_subsystem/mdio]
  connect_bd_intf_net -intf_net tsn_endpoint_ethernet_mac_0_mdio_external2 [get_bd_intf_ports mdio2] [get_bd_intf_pins TSN_subsystem/mdio2]
  connect_bd_intf_net -intf_net tsn_endpoint_ethernet_mac_0_rgmii1 [get_bd_intf_ports rgmii] [get_bd_intf_pins TSN_subsystem/rgmii]
  connect_bd_intf_net -intf_net tsn_endpoint_ethernet_mac_0_rgmii2 [get_bd_intf_ports rgmii2] [get_bd_intf_pins TSN_subsystem/rgmii2]
  connect_bd_intf_net -intf_net zynq_ultra_ps_e_0_M_AXI_HPM0_FPD [get_bd_intf_pins PS_0/M_AXI_HPM0_FPD] [get_bd_intf_pins axi_interconnect_cntrl/S00_AXI]
  connect_bd_intf_net -intf_net zynq_ultra_ps_e_0_M_AXI_HPM0_LPD [get_bd_intf_pins PS_0/M_AXI_HPM0_LPD] [get_bd_intf_pins TSN_subsystem/S00_AXI]

  # Create port connections
  connect_bd_net -net ADC_CSn [get_bd_pins ADC/CSn] [get_bd_pins xlslice_CSn/Din] -boundary_type upper
  connect_bd_net -net ADC_CSn [get_bd_pins ADC/CSn] [get_bd_pins xlslice_dc_CSn/Din] -boundary_type upper
  connect_bd_net -net ADC_dc_link_oc_err [get_bd_pins ADC/dc_link_oc_err] [get_bd_pins motor_control_0/dc_link_oc_err]
  connect_bd_net -net ADC_dc_link_ov_err [get_bd_pins ADC/dc_link_ov_err] [get_bd_pins motor_control_0/dc_link_ov_err]
  connect_bd_net -net ADC_phase_a_oc_err [get_bd_pins ADC/phase_a_oc_err] [get_bd_pins motor_control_0/phase_a_oc_err]
  connect_bd_net -net ADC_phase_b_oc_err [get_bd_pins ADC/phase_b_oc_err] [get_bd_pins motor_control_0/phase_b_oc_err]
  connect_bd_net -net ADC_phase_c_oc_err [get_bd_pins ADC/phase_c_oc_err] [get_bd_pins motor_control_0/phase_c_oc_err]
  connect_bd_net -net ARESETN_1 [get_bd_pins axi_interconnect_cntrl/ARESETN] [get_bd_pins proc_sys_reset_0/interconnect_aresetn]
  connect_bd_net -net A_0_1 [get_bd_ports QEI_A] [get_bd_pins hls_qei_top_0/qei_A_dout]
  connect_bd_net -net B_0_1 [get_bd_ports QEI_B] [get_bd_pins hls_qei_top_0/qei_B_dout]
  connect_bd_net -net I_0_1 [get_bd_ports QEI_I] [get_bd_pins hls_qei_top_0/qei_I_dout]
  connect_bd_net -net In0_0_1 [get_bd_ports motor_pa_data_v] [get_bd_pins xlconcat_0/In0]
  connect_bd_net -net In1_0_1 [get_bd_ports motor_pa_data_i] [get_bd_pins xlconcat_0/In1]
  connect_bd_net -net In2_0_1 [get_bd_ports motor_pb_data_v] [get_bd_pins xlconcat_0/In2]
  connect_bd_net -net In3_0_1 [get_bd_ports motor_pb_data_i] [get_bd_pins xlconcat_0/In3]
  connect_bd_net -net In4_0_1 [get_bd_ports motor_pc_data_v] [get_bd_pins xlconcat_0/In4]
  connect_bd_net -net In5_0_1 [get_bd_ports motor_pc_data_i] [get_bd_pins xlconcat_0/In5]
  connect_bd_net -net In6_0_1 [get_bd_ports dc_link_data_v] [get_bd_pins xlconcat_0/In6]
  connect_bd_net -net In7_0_1 [get_bd_ports dc_link_data_1] [get_bd_pins xlconcat_0/In7]
  connect_bd_net -net SDATA_1 [get_bd_pins ADC/SDATA] [get_bd_pins xlconcat_0/dout]
  connect_bd_net -net TSN_subsystem_clk_out1 [get_bd_pins PS_0/saxihp0_fpd_aclk] [get_bd_pins TSN_subsystem/clk_out1]
  connect_bd_net -net TSN_subsystem_clk_out4 [get_bd_pins PS_0/maxihpm0_lpd_aclk] [get_bd_pins TSN_subsystem/clk_out4]
  connect_bd_net -net TSN_subsystem_irq [get_bd_pins TSN_subsystem/irq] [get_bd_pins xlconcat_int/In2]
  connect_bd_net -net TSN_subsystem_lis1_rxs_0 [get_bd_ports l1_rxs] [get_bd_pins TSN_subsystem/lis1_rxs_0]
  connect_bd_net -net TSN_subsystem_lis2_rxb_0 [get_bd_ports l2_rxb] [get_bd_pins TSN_subsystem/lis2_rxb_0]
  connect_bd_net -net TSN_subsystem_lis3_rxx_0 [get_bd_ports l3_rxx] [get_bd_pins TSN_subsystem/lis3_rxx_0]
  connect_bd_net -net TSN_subsystem_ptp_timer [get_bd_ports ptp_timer] [get_bd_pins TSN_subsystem/ptp_timer]
  connect_bd_net -net TSN_subsystem_pub1_txs_0 [get_bd_ports p1_txs] [get_bd_pins TSN_subsystem/pub1_txs_0]
  connect_bd_net -net TSN_subsystem_pub2_txb_0 [get_bd_ports p2_txb] [get_bd_pins TSN_subsystem/pub2_txb_0]
  connect_bd_net -net TSN_subsystem_pub3_txx_0 [get_bd_ports p3_txx] [get_bd_pins TSN_subsystem/pub3_txx_0]
  connect_bd_net -net adc_hub_1_interrupt [get_bd_pins ADC/interrupt] [get_bd_pins xlconcat_int/In0]
  connect_bd_net -net clk_in1_0_1 [get_bd_ports CLK_IN_gem] [get_bd_pins TSN_subsystem/CLK_IN_gem]
  connect_bd_net -net clk_wiz_0_clk_out_20M1 [get_bd_ports dc_link_adc_sclk] [get_bd_ports motor_adc_sclk] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_out_20M1 [get_bd_ports dc_link_adc_sclk] [get_bd_pins ADC/SCLK] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_out_20M1 [get_bd_ports dc_link_adc_sclk] [get_bd_pins clk_wiz_0/clk_out_20M] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_out_20M1 [get_bd_ports dc_link_adc_sclk] [get_bd_pins proc_sys_reset_1/slowest_sync_clk] -boundary_type upper
  connect_bd_net -net clk_wiz_0_locked [get_bd_pins clk_wiz_0/locked] [get_bd_pins proc_sys_reset_0/dcm_locked] -boundary_type upper
  connect_bd_net -net clk_wiz_0_locked [get_bd_pins clk_wiz_0/locked] [get_bd_pins proc_sys_reset_1/dcm_locked] -boundary_type upper
  connect_bd_net -net gate_driver_A_gate_drive [get_bd_ports gate_drive_phase_a] [get_bd_pins gate_driver/gate_drive_phase_a]
  connect_bd_net -net gate_driver_B_gate_drive [get_bd_ports gate_drive_phase_b] [get_bd_pins gate_driver/gate_drive_phase_b]
  connect_bd_net -net gate_driver_C_gate_drive [get_bd_ports gate_drive_phase_c] [get_bd_pins gate_driver/gate_drive_phase_c]
  connect_bd_net -net hls_pwm_gen_0_strm_pwm_h_c_din [get_bd_pins gate_driver/phase_upper_c] [get_bd_pins hls_pwm_gen_0/strm_pwm_h_c_din]
  connect_bd_net -net hls_pwm_gen_0_strm_pwm_l_a_din [get_bd_pins gate_driver/phase_lower_a] [get_bd_pins hls_pwm_gen_0/strm_pwm_l_a_din]
  connect_bd_net -net hls_pwm_gen_0_strm_pwm_l_b_din [get_bd_pins gate_driver/phase_lower_b] [get_bd_pins hls_pwm_gen_0/strm_pwm_l_b_din]
  connect_bd_net -net hls_pwm_gen_0_strm_pwm_l_c_din [get_bd_pins gate_driver/phase_lower_c] [get_bd_pins hls_pwm_gen_0/strm_pwm_l_c_din]
  connect_bd_net -net motor_control_0_gate_drive_en [get_bd_ports gate_drive_en] [get_bd_pins motor_control_0/gate_drive_en]
  connect_bd_net -net motor_control_0_interrupt [get_bd_pins motor_control_0/interrupt] [get_bd_pins xlconcat_int/In1]
  connect_bd_net -net phase_upper_a_1 [get_bd_pins gate_driver/phase_upper_a] [get_bd_pins hls_pwm_gen_0/strm_pwm_h_a_din]
  connect_bd_net -net phase_upper_b_1 [get_bd_pins gate_driver/phase_upper_b] [get_bd_pins hls_pwm_gen_0/strm_pwm_h_b_din]
  connect_bd_net -net proc_sys_reset_1_peripheral_aresetn [get_bd_pins ADC/SCLK_RESETn] [get_bd_pins proc_sys_reset_1/peripheral_aresetn]
  connect_bd_net -net xlconcat_irq [get_bd_pins PS_0/pl_ps_irq0] [get_bd_pins xlconcat_int/dout]
  connect_bd_net -net xlconstant_0_dout [get_bd_ports Phy_reset_n] [get_bd_pins TSN_subsystem/Phy_reset_n]
  connect_bd_net -net xlconstant_1_dout [get_bd_pins ADC/UPDATE] [get_bd_pins hls_foc_periodic_top_0/ap_start] -boundary_type upper
  connect_bd_net -net xlconstant_1_dout [get_bd_pins ADC/UPDATE] [get_bd_pins hls_pwm_gen_0/ap_start] -boundary_type upper
  connect_bd_net -net xlconstant_1_dout [get_bd_pins ADC/UPDATE] [get_bd_pins hls_qei_top_0/ap_start] -boundary_type upper
  connect_bd_net -net xlconstant_1_dout [get_bd_pins ADC/UPDATE] [get_bd_pins hls_svpwm_duty_0/ap_start] -boundary_type upper
  connect_bd_net -net xlconstant_1_dout [get_bd_pins ADC/UPDATE] [get_bd_pins xlconstant_1/dout] -boundary_type upper
  connect_bd_net -net xlslice_0_Dout1 [get_bd_ports fan_en_b] [get_bd_pins xlslice_fan/Dout]
  connect_bd_net -net xlslice_CSn_Dout [get_bd_ports motor_adc_cs_n] [get_bd_pins xlslice_CSn/Dout]
  connect_bd_net -net xlslice_dc_CSn_Dout [get_bd_ports dc_link_adc_cs_n] [get_bd_pins xlslice_dc_CSn/Dout]
  connect_bd_net -net zynq_ultra_ps_e_0_emio_ttc0_wave_o [get_bd_pins PS_0/emio_ttc0_wave_o] [get_bd_pins xlslice_fan/Din]
  connect_bd_net -net zynq_ultra_ps_e_0_pl_clk0 [get_bd_pins ADC/CLK] [get_bd_pins PS_0/maxihpm0_fpd_aclk] -boundary_type upper
  connect_bd_net -net zynq_ultra_ps_e_0_pl_clk0 [get_bd_pins ADC/CLK] [get_bd_pins axi_interconnect_cntrl/ACLK] -boundary_type upper
  connect_bd_net -net zynq_ultra_ps_e_0_pl_clk0 [get_bd_pins ADC/CLK] [get_bd_pins axi_interconnect_cntrl/M00_ACLK] -boundary_type upper
  connect_bd_net -net zynq_ultra_ps_e_0_pl_clk0 [get_bd_pins ADC/CLK] [get_bd_pins axi_interconnect_cntrl/M01_ACLK] -boundary_type upper
  connect_bd_net -net zynq_ultra_ps_e_0_pl_clk0 [get_bd_pins ADC/CLK] [get_bd_pins axi_interconnect_cntrl/M02_ACLK] -boundary_type upper
  connect_bd_net -net zynq_ultra_ps_e_0_pl_clk0 [get_bd_pins ADC/CLK] [get_bd_pins axi_interconnect_cntrl/M03_ACLK] -boundary_type upper
  connect_bd_net -net zynq_ultra_ps_e_0_pl_clk0 [get_bd_pins ADC/CLK] [get_bd_pins axi_interconnect_cntrl/M04_ACLK] -boundary_type upper
  connect_bd_net -net zynq_ultra_ps_e_0_pl_clk0 [get_bd_pins ADC/CLK] [get_bd_pins axi_interconnect_cntrl/M05_ACLK] -boundary_type upper
  connect_bd_net -net zynq_ultra_ps_e_0_pl_clk0 [get_bd_pins ADC/CLK] [get_bd_pins axi_interconnect_cntrl/S00_ACLK] -boundary_type upper
  connect_bd_net -net zynq_ultra_ps_e_0_pl_clk0 [get_bd_pins ADC/CLK] [get_bd_pins broadcast_i/aclk] -boundary_type upper
  connect_bd_net -net zynq_ultra_ps_e_0_pl_clk0 [get_bd_pins ADC/CLK] [get_bd_pins clk_wiz_0/clk_out_100M] -boundary_type upper
  connect_bd_net -net zynq_ultra_ps_e_0_pl_clk0 [get_bd_pins ADC/CLK] [get_bd_pins gate_driver/sys_clk] -boundary_type upper
  connect_bd_net -net zynq_ultra_ps_e_0_pl_clk0 [get_bd_pins ADC/CLK] [get_bd_pins hls_foc_periodic_top_0/ap_clk] -boundary_type upper
  connect_bd_net -net zynq_ultra_ps_e_0_pl_clk0 [get_bd_pins ADC/CLK] [get_bd_pins hls_pwm_gen_0/ap_clk] -boundary_type upper
  connect_bd_net -net zynq_ultra_ps_e_0_pl_clk0 [get_bd_pins ADC/CLK] [get_bd_pins hls_qei_top_0/ap_clk] -boundary_type upper
  connect_bd_net -net zynq_ultra_ps_e_0_pl_clk0 [get_bd_pins ADC/CLK] [get_bd_pins hls_svpwm_duty_0/ap_clk] -boundary_type upper
  connect_bd_net -net zynq_ultra_ps_e_0_pl_clk0 [get_bd_pins ADC/CLK] [get_bd_pins motor_control_0/s_axi_cntrl_clk] -boundary_type upper
  connect_bd_net -net zynq_ultra_ps_e_0_pl_clk0 [get_bd_pins ADC/CLK] [get_bd_pins proc_sys_reset_0/slowest_sync_clk] -boundary_type upper
  connect_bd_net -net zynq_ultra_ps_e_0_pl_clk1 [get_bd_pins PS_0/pl_clk0] [get_bd_pins clk_wiz_0/clk_in1]
  connect_bd_net -net zynq_ultra_ps_e_0_pl_resetn0 [get_bd_pins ADC/RESETn] [get_bd_pins axi_interconnect_cntrl/M00_ARESETN] -boundary_type upper
  connect_bd_net -net zynq_ultra_ps_e_0_pl_resetn0 [get_bd_pins ADC/RESETn] [get_bd_pins axi_interconnect_cntrl/M01_ARESETN] -boundary_type upper
  connect_bd_net -net zynq_ultra_ps_e_0_pl_resetn0 [get_bd_pins ADC/RESETn] [get_bd_pins axi_interconnect_cntrl/M02_ARESETN] -boundary_type upper
  connect_bd_net -net zynq_ultra_ps_e_0_pl_resetn0 [get_bd_pins ADC/RESETn] [get_bd_pins axi_interconnect_cntrl/M03_ARESETN] -boundary_type upper
  connect_bd_net -net zynq_ultra_ps_e_0_pl_resetn0 [get_bd_pins ADC/RESETn] [get_bd_pins axi_interconnect_cntrl/M04_ARESETN] -boundary_type upper
  connect_bd_net -net zynq_ultra_ps_e_0_pl_resetn0 [get_bd_pins ADC/RESETn] [get_bd_pins axi_interconnect_cntrl/M05_ARESETN] -boundary_type upper
  connect_bd_net -net zynq_ultra_ps_e_0_pl_resetn0 [get_bd_pins ADC/RESETn] [get_bd_pins axi_interconnect_cntrl/S00_ARESETN] -boundary_type upper
  connect_bd_net -net zynq_ultra_ps_e_0_pl_resetn0 [get_bd_pins ADC/RESETn] [get_bd_pins broadcast_i/aresetn] -boundary_type upper
  connect_bd_net -net zynq_ultra_ps_e_0_pl_resetn0 [get_bd_pins ADC/RESETn] [get_bd_pins gate_driver/sys_rstn] -boundary_type upper
  connect_bd_net -net zynq_ultra_ps_e_0_pl_resetn0 [get_bd_pins ADC/RESETn] [get_bd_pins hls_foc_periodic_top_0/ap_rst_n] -boundary_type upper
  connect_bd_net -net zynq_ultra_ps_e_0_pl_resetn0 [get_bd_pins ADC/RESETn] [get_bd_pins hls_pwm_gen_0/ap_rst_n] -boundary_type upper
  connect_bd_net -net zynq_ultra_ps_e_0_pl_resetn0 [get_bd_pins ADC/RESETn] [get_bd_pins hls_qei_top_0/ap_rst_n] -boundary_type upper
  connect_bd_net -net zynq_ultra_ps_e_0_pl_resetn0 [get_bd_pins ADC/RESETn] [get_bd_pins hls_svpwm_duty_0/ap_rst_n] -boundary_type upper
  connect_bd_net -net zynq_ultra_ps_e_0_pl_resetn0 [get_bd_pins ADC/RESETn] [get_bd_pins motor_control_0/s_axi_cntrl_resetn] -boundary_type upper
  connect_bd_net -net zynq_ultra_ps_e_0_pl_resetn0 [get_bd_pins ADC/RESETn] [get_bd_pins proc_sys_reset_0/peripheral_aresetn] -boundary_type upper
  connect_bd_net -net zynq_ultra_ps_e_0_pl_resetn1 [get_bd_pins PS_0/pl_resetn0] [get_bd_pins TSN_subsystem/ext_reset_in] -boundary_type upper
  connect_bd_net -net zynq_ultra_ps_e_0_pl_resetn1 [get_bd_pins PS_0/pl_resetn0] [get_bd_pins clk_wiz_0/resetn] -boundary_type upper
  connect_bd_net -net zynq_ultra_ps_e_0_pl_resetn1 [get_bd_pins PS_0/pl_resetn0] [get_bd_pins proc_sys_reset_0/ext_reset_in] -boundary_type upper
  connect_bd_net -net zynq_ultra_ps_e_0_pl_resetn1 [get_bd_pins PS_0/pl_resetn0] [get_bd_pins proc_sys_reset_1/ext_reset_in] -boundary_type upper

  
  # Create address segments
  assign_bd_address -offset 0xA0010000 -range 0x00010000 -target_address_space [get_bd_addr_spaces PS_0/Data] [get_bd_addr_segs ADC/adc_hub_phase_dc/S_AXI/Reg] -force
  assign_bd_address -offset 0x80020000 -range 0x00001000 -target_address_space [get_bd_addr_spaces PS_0/Data] [get_bd_addr_segs TSN_subsystem/axi_intc_0/S_AXI/Reg] -force
  assign_bd_address -offset 0x80000000 -range 0x00001000 -target_address_space [get_bd_addr_spaces PS_0/Data] [get_bd_addr_segs TSN_subsystem/axi_mcdma_0/S_AXI_LITE/Reg] -force
  assign_bd_address -offset 0xA0000000 -range 0x00010000 -target_address_space [get_bd_addr_spaces PS_0/Data] [get_bd_addr_segs hls_foc_periodic_top_0/s_axi_foc_args/Reg] -force
  assign_bd_address -offset 0xA0040000 -range 0x00010000 -target_address_space [get_bd_addr_spaces PS_0/Data] [get_bd_addr_segs hls_pwm_gen_0/s_axi_pwm_args/Reg] -force
  assign_bd_address -offset 0xA0030000 -range 0x00010000 -target_address_space [get_bd_addr_spaces PS_0/Data] [get_bd_addr_segs hls_qei_top_0/s_axi_qei_args/Reg] -force
  assign_bd_address -offset 0xA0050000 -range 0x00010000 -target_address_space [get_bd_addr_spaces PS_0/Data] [get_bd_addr_segs hls_svpwm_duty_0/s_axi_pwm_args/Reg] -force
  assign_bd_address -offset 0xA0020000 -range 0x00001000 -target_address_space [get_bd_addr_spaces PS_0/Data] [get_bd_addr_segs motor_control_0/s_axi_cntrl/reg0] -force
  assign_bd_address -offset 0x80040000 -range 0x00040000 -target_address_space [get_bd_addr_spaces PS_0/Data] [get_bd_addr_segs TSN_subsystem/my_tsn_ip/s_axi/Reg] -force
  assign_bd_address -offset 0x90000000 -range 0x00800000 -target_address_space [get_bd_addr_spaces PS_0/Data] [get_bd_addr_segs TSN_subsystem/ta_dma_0/S_AXI/Reg] -force
  assign_bd_address -offset 0x80030000 -range 0x00010000 -target_address_space [get_bd_addr_spaces PS_0/Data] [get_bd_addr_segs TSN_subsystem/test_pmod_controller_0/S00_AXI/S00_AXI_reg] -force
  assign_bd_address -offset 0x00000000 -range 0x80000000 -target_address_space [get_bd_addr_spaces TSN_subsystem/axi_mcdma_0/Data_MM2S] [get_bd_addr_segs PS_0/SAXIGP2/HP0_DDR_LOW] -force
  assign_bd_address -offset 0x00000000 -range 0x80000000 -target_address_space [get_bd_addr_spaces TSN_subsystem/axi_mcdma_0/Data_S2MM] [get_bd_addr_segs PS_0/SAXIGP2/HP0_DDR_LOW] -force
  assign_bd_address -offset 0x00000000 -range 0x80000000 -target_address_space [get_bd_addr_spaces TSN_subsystem/axi_mcdma_0/Data_SG] [get_bd_addr_segs PS_0/SAXIGP2/HP0_DDR_LOW] -force
  assign_bd_address -offset 0x00000000 -range 0x80000000 -target_address_space [get_bd_addr_spaces TSN_subsystem/ta_dma_0/Data] [get_bd_addr_segs PS_0/SAXIGP2/HP0_DDR_LOW] -force


  # Restore current instance
  current_bd_instance $oldCurInst

  # Create PFM attributes
  set_property PFM_NAME {xilinx:kd240_motor_ctrl_qei:kd240_motor_ctrl_qei:1.0} [get_files [current_bd_design].bd]
  set_property PFM.AXI_PORT {M_AXI_HPM1_FPD {memport "M_AXI_GP" sptag "" memory "" is_range "false"} S_AXI_HPC0_FPD {memport "S_AXI_HP" sptag "HPC0" memory "PS_0 HPC0_DDR_LOW" is_range "false"} S_AXI_HPC1_FPD {memport "S_AXI_HP" sptag "HPC1" memory "PS_0 HPC1_DDR_LOW" is_range "false"} S_AXI_HP1_FPD {memport "S_AXI_HP" sptag "HP1" memory "PS_0 HP1_DDR_LOW" is_range "false"} S_AXI_HP2_FPD {memport "S_AXI_HP" sptag "HP2" memory "PS_0 HP2_DDR_LOW" is_range "false"} S_AXI_HP3_FPD {memport "S_AXI_HP" sptag "HP3" memory "PS_0 HP3_DDR_LOW" is_range "false"} S_AXI_LPD {memport "MIG" sptag "LPD" memory "PS_0 LPD_DDR_LOW" is_range "false"}} [get_bd_cells /PS_0]
  set_property PFM.CLOCK {clk_out_100M {id "4" is_default "true" proc_sys_reset "/proc_sys_reset_0" status "fixed" freq_hz "99999000"} clk_out_20M {id "5" is_default "false" proc_sys_reset "/proc_sys_reset_1" status "fixed" freq_hz "19999800"}} [get_bd_cells /clk_wiz_0]
  set_property PFM.IRQ {pl_ps_irq1 {id 0 range 7}} [get_bd_cells /PS_0]


  validate_bd_design
  save_bd_design
}
# End of create_root_design()


##################################################################
# MAIN FLOW
##################################################################

create_root_design ""


