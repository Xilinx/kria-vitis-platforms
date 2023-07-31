# Copyright (C) 2022, Advanced Micro Devices, Inc.
# SPDX-License-Identifier: Apache-2.0

##################################################################
# DESIGN PROCs
##################################################################


# Hierarchical cell: sync_vector
proc create_hier_cell_sync_vector { parentCell nameHier } {

  variable script_folder

  if { $parentCell eq "" || $nameHier eq "" } {
     catch {common::send_gid_msg -ssname BD::TCL -id 2092 -severity "ERROR" "create_hier_cell_sync_vector() - Empty argument(s)!"}
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
  create_bd_intf_pin -mode Slave -vlnv User:interface:data_valid_rtl:1.0 DATA_IN0

  create_bd_intf_pin -mode Slave -vlnv User:interface:data_valid_rtl:1.0 DATA_IN1

  create_bd_intf_pin -mode Slave -vlnv User:interface:data_valid_rtl:1.0 DATA_IN2

  create_bd_intf_pin -mode Slave -vlnv User:interface:data_valid_rtl:1.0 DATA_IN3

  create_bd_intf_pin -mode Slave -vlnv User:interface:data_valid_rtl:1.0 DATA_IN4

  create_bd_intf_pin -mode Slave -vlnv User:interface:data_valid_rtl:1.0 DATA_IN5

  create_bd_intf_pin -mode Slave -vlnv User:interface:data_valid_rtl:1.0 DATA_IN6

  create_bd_intf_pin -mode Slave -vlnv User:interface:data_valid_rtl:1.0 DATA_IN7

  create_bd_intf_pin -mode Master -vlnv User:interface:data_valid_rtl:1.0 DATA_OUT0

  create_bd_intf_pin -mode Master -vlnv User:interface:data_valid_rtl:1.0 DATA_OUT1

  create_bd_intf_pin -mode Master -vlnv User:interface:data_valid_rtl:1.0 DATA_OUT2

  create_bd_intf_pin -mode Master -vlnv User:interface:data_valid_rtl:1.0 DATA_OUT3

  create_bd_intf_pin -mode Master -vlnv User:interface:data_valid_rtl:1.0 DATA_OUT4

  create_bd_intf_pin -mode Master -vlnv User:interface:data_valid_rtl:1.0 DATA_OUT5

  create_bd_intf_pin -mode Master -vlnv User:interface:data_valid_rtl:1.0 DATA_OUT6

  create_bd_intf_pin -mode Master -vlnv User:interface:data_valid_rtl:1.0 DATA_OUT7


  # Create pins
  create_bd_pin -dir I -type clk CLK
  create_bd_pin -dir I -type clk SCLK

  # Create instance: synchronizer_vector_0, and set properties
  set synchronizer_vector_0 [ create_bd_cell -type ip -vlnv user.org:user:synchronizer_vector_vld synchronizer_vector_0 ]
  set_property CONFIG.DATA_WIDTH {12} $synchronizer_vector_0


  # Create instance: synchronizer_vector_1, and set properties
  set synchronizer_vector_1 [ create_bd_cell -type ip -vlnv user.org:user:synchronizer_vector_vld synchronizer_vector_1 ]
  set_property CONFIG.DATA_WIDTH {12} $synchronizer_vector_1


  # Create instance: synchronizer_vector_2, and set properties
  set synchronizer_vector_2 [ create_bd_cell -type ip -vlnv user.org:user:synchronizer_vector_vld synchronizer_vector_2 ]
  set_property CONFIG.DATA_WIDTH {12} $synchronizer_vector_2


  # Create instance: synchronizer_vector_3, and set properties
  set synchronizer_vector_3 [ create_bd_cell -type ip -vlnv user.org:user:synchronizer_vector_vld synchronizer_vector_3 ]
  set_property CONFIG.DATA_WIDTH {12} $synchronizer_vector_3


  # Create instance: synchronizer_vector_4, and set properties
  set synchronizer_vector_4 [ create_bd_cell -type ip -vlnv user.org:user:synchronizer_vector_vld synchronizer_vector_4 ]
  set_property CONFIG.DATA_WIDTH {12} $synchronizer_vector_4


  # Create instance: synchronizer_vector_5, and set properties
  set synchronizer_vector_5 [ create_bd_cell -type ip -vlnv user.org:user:synchronizer_vector_vld synchronizer_vector_5 ]
  set_property CONFIG.DATA_WIDTH {12} $synchronizer_vector_5


  # Create instance: synchronizer_vector_6, and set properties
  set synchronizer_vector_6 [ create_bd_cell -type ip -vlnv user.org:user:synchronizer_vector_vld synchronizer_vector_6 ]
  set_property CONFIG.DATA_WIDTH {12} $synchronizer_vector_6


  # Create instance: synchronizer_vector_7, and set properties
  set synchronizer_vector_7 [ create_bd_cell -type ip -vlnv user.org:user:synchronizer_vector_vld synchronizer_vector_7 ]
  set_property CONFIG.DATA_WIDTH {12} $synchronizer_vector_7


  # Create interface connections
  connect_bd_intf_net -intf_net adc_if_L0_ADC [get_bd_intf_pins DATA_IN0] [get_bd_intf_pins synchronizer_vector_0/DATA_IN]
  connect_bd_intf_net -intf_net adc_if_L1_ADC [get_bd_intf_pins DATA_IN1] [get_bd_intf_pins synchronizer_vector_1/DATA_IN]
  connect_bd_intf_net -intf_net adc_if_L2_ADC [get_bd_intf_pins DATA_IN2] [get_bd_intf_pins synchronizer_vector_2/DATA_IN]
  connect_bd_intf_net -intf_net adc_if_L3_ADC [get_bd_intf_pins DATA_IN3] [get_bd_intf_pins synchronizer_vector_3/DATA_IN]
  connect_bd_intf_net -intf_net adc_if_L4_ADC [get_bd_intf_pins DATA_IN4] [get_bd_intf_pins synchronizer_vector_4/DATA_IN]
  connect_bd_intf_net -intf_net adc_if_L5_ADC [get_bd_intf_pins DATA_IN5] [get_bd_intf_pins synchronizer_vector_5/DATA_IN]
  connect_bd_intf_net -intf_net adc_if_L6_ADC [get_bd_intf_pins DATA_IN6] [get_bd_intf_pins synchronizer_vector_6/DATA_IN]
  connect_bd_intf_net -intf_net adc_if_L7_ADC [get_bd_intf_pins DATA_IN7] [get_bd_intf_pins synchronizer_vector_7/DATA_IN]
  connect_bd_intf_net -intf_net synchronizer_vector_0_DATA_OUT [get_bd_intf_pins DATA_OUT0] [get_bd_intf_pins synchronizer_vector_0/DATA_OUT]
  connect_bd_intf_net -intf_net synchronizer_vector_1_DATA_OUT [get_bd_intf_pins DATA_OUT1] [get_bd_intf_pins synchronizer_vector_1/DATA_OUT]
  connect_bd_intf_net -intf_net synchronizer_vector_2_DATA_OUT [get_bd_intf_pins DATA_OUT2] [get_bd_intf_pins synchronizer_vector_2/DATA_OUT]
  connect_bd_intf_net -intf_net synchronizer_vector_3_DATA_OUT [get_bd_intf_pins DATA_OUT3] [get_bd_intf_pins synchronizer_vector_3/DATA_OUT]
  connect_bd_intf_net -intf_net synchronizer_vector_4_DATA_OUT [get_bd_intf_pins DATA_OUT4] [get_bd_intf_pins synchronizer_vector_4/DATA_OUT]
  connect_bd_intf_net -intf_net synchronizer_vector_5_DATA_OUT [get_bd_intf_pins DATA_OUT5] [get_bd_intf_pins synchronizer_vector_5/DATA_OUT]
  connect_bd_intf_net -intf_net synchronizer_vector_6_DATA_OUT [get_bd_intf_pins DATA_OUT6] [get_bd_intf_pins synchronizer_vector_6/DATA_OUT]
  connect_bd_intf_net -intf_net synchronizer_vector_7_DATA_OUT [get_bd_intf_pins DATA_OUT7] [get_bd_intf_pins synchronizer_vector_7/DATA_OUT]

  # Create port connections
  connect_bd_net -net SCLK_1 [get_bd_pins SCLK] [get_bd_pins synchronizer_vector_0/data_in_clk] -boundary_type upper
  connect_bd_net -net SCLK_1 [get_bd_pins SCLK] [get_bd_pins synchronizer_vector_1/data_in_clk] -boundary_type upper
  connect_bd_net -net SCLK_1 [get_bd_pins SCLK] [get_bd_pins synchronizer_vector_2/data_in_clk] -boundary_type upper
  connect_bd_net -net SCLK_1 [get_bd_pins SCLK] [get_bd_pins synchronizer_vector_3/data_in_clk] -boundary_type upper
  connect_bd_net -net SCLK_1 [get_bd_pins SCLK] [get_bd_pins synchronizer_vector_4/data_in_clk] -boundary_type upper
  connect_bd_net -net SCLK_1 [get_bd_pins SCLK] [get_bd_pins synchronizer_vector_5/data_in_clk] -boundary_type upper
  connect_bd_net -net SCLK_1 [get_bd_pins SCLK] [get_bd_pins synchronizer_vector_6/data_in_clk] -boundary_type upper
  connect_bd_net -net SCLK_1 [get_bd_pins SCLK] [get_bd_pins synchronizer_vector_7/data_in_clk] -boundary_type upper
  connect_bd_net -net zynq_ultra_ps_e_0_pl_clk0 [get_bd_pins CLK] [get_bd_pins synchronizer_vector_0/data_out_clk] -boundary_type upper
  connect_bd_net -net zynq_ultra_ps_e_0_pl_clk0 [get_bd_pins CLK] [get_bd_pins synchronizer_vector_1/data_out_clk] -boundary_type upper
  connect_bd_net -net zynq_ultra_ps_e_0_pl_clk0 [get_bd_pins CLK] [get_bd_pins synchronizer_vector_2/data_out_clk] -boundary_type upper
  connect_bd_net -net zynq_ultra_ps_e_0_pl_clk0 [get_bd_pins CLK] [get_bd_pins synchronizer_vector_3/data_out_clk] -boundary_type upper
  connect_bd_net -net zynq_ultra_ps_e_0_pl_clk0 [get_bd_pins CLK] [get_bd_pins synchronizer_vector_4/data_out_clk] -boundary_type upper
  connect_bd_net -net zynq_ultra_ps_e_0_pl_clk0 [get_bd_pins CLK] [get_bd_pins synchronizer_vector_5/data_out_clk] -boundary_type upper
  connect_bd_net -net zynq_ultra_ps_e_0_pl_clk0 [get_bd_pins CLK] [get_bd_pins synchronizer_vector_6/data_out_clk] -boundary_type upper
  connect_bd_net -net zynq_ultra_ps_e_0_pl_clk0 [get_bd_pins CLK] [get_bd_pins synchronizer_vector_7/data_out_clk] -boundary_type upper

  # Restore current instance
  current_bd_instance $oldCurInst
}

# Hierarchical cell: sync
proc create_hier_cell_sync { parentCell nameHier } {

  variable script_folder

  if { $parentCell eq "" || $nameHier eq "" } {
     catch {common::send_gid_msg -ssname BD::TCL -id 2092 -severity "ERROR" "create_hier_cell_sync() - Empty argument(s)!"}
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
  create_bd_pin -dir I -type clk CLK
  create_bd_pin -dir I -type clk SCLK
  create_bd_pin -dir I -from 0 -to 0 -type rst SCLK_RESETn
  create_bd_pin -dir I -from 0 -to 0 csn_dc_in
  create_bd_pin -dir O -from 0 -to 0 csn_dc_out
  create_bd_pin -dir I -from 0 -to 0 csn_motor_in
  create_bd_pin -dir O -from 0 -to 0 csn_motor_out
  create_bd_pin -dir O -from 0 -to 0 sclk_resetn_out
  create_bd_pin -dir I -from 0 -to 0 update_dc_in
  create_bd_pin -dir O -from 0 -to 0 update_dc_out
  create_bd_pin -dir I -from 0 -to 0 update_motor_in
  create_bd_pin -dir O -from 0 -to 0 update_motor_out

  # Create instance: dc_csn_sync, and set properties
  set dc_csn_sync [ create_bd_cell -type ip -vlnv user.org:user:synchronizer_simple dc_csn_sync ]

  # Create instance: dc_update_sync, and set properties
  set dc_update_sync [ create_bd_cell -type ip -vlnv user.org:user:synchronizer_simple dc_update_sync ]

  # Create instance: motor_csn_sync, and set properties
  set motor_csn_sync [ create_bd_cell -type ip -vlnv user.org:user:synchronizer_simple motor_csn_sync ]

  # Create instance: motor_update_sync, and set properties
  set motor_update_sync [ create_bd_cell -type ip -vlnv user.org:user:synchronizer_simple motor_update_sync ]

  # Create instance: sclk_resetn_sync, and set properties
  set sclk_resetn_sync [ create_bd_cell -type ip -vlnv user.org:user:synchronizer_simple sclk_resetn_sync ]

  # Create port connections
  connect_bd_net -net SCLK_1 [get_bd_pins SCLK] [get_bd_pins dc_update_sync/new_clk] -boundary_type upper
  connect_bd_net -net SCLK_1 [get_bd_pins SCLK] [get_bd_pins motor_update_sync/new_clk] -boundary_type upper
  connect_bd_net -net adc_sample_ctrl_dc_sample_adc [get_bd_pins update_dc_in] [get_bd_pins dc_update_sync/data_in]
  connect_bd_net -net adc_sample_ctrl_motor_sample_adc [get_bd_pins update_motor_in] [get_bd_pins motor_update_sync/data_in]
  connect_bd_net -net dc_csn_sync_data_out [get_bd_pins dc_csn_sync/data_out] [get_bd_pins csn_dc_out]
  connect_bd_net -net dc_update_out [get_bd_pins dc_update_sync/data_out] [get_bd_pins update_dc_out]
  connect_bd_net -net motor_csn_sync_data_out [get_bd_pins motor_csn_sync/data_out] [get_bd_pins csn_motor_out]
  connect_bd_net -net proc_sys_reset_1_peripheral_aresetn [get_bd_pins SCLK_RESETn] [get_bd_pins sclk_resetn_sync/data_in]
  connect_bd_net -net sclk_resetn_sync_data_out [get_bd_pins sclk_resetn_sync/data_out] [get_bd_pins sclk_resetn_out]
  connect_bd_net -net smotor_update_out [get_bd_pins motor_update_sync/data_out] [get_bd_pins update_motor_out]
  connect_bd_net -net xlslice_CSn_Dout [get_bd_pins csn_motor_in] [get_bd_pins motor_csn_sync/data_in]
  connect_bd_net -net xlslice_dc_CSn_Dout [get_bd_pins csn_dc_in] [get_bd_pins dc_csn_sync/data_in]
  connect_bd_net -net zynq_ultra_ps_e_0_pl_clk0 [get_bd_pins CLK] [get_bd_pins dc_csn_sync/new_clk] -boundary_type upper
  connect_bd_net -net zynq_ultra_ps_e_0_pl_clk0 [get_bd_pins CLK] [get_bd_pins motor_csn_sync/new_clk] -boundary_type upper
  connect_bd_net -net zynq_ultra_ps_e_0_pl_clk0 [get_bd_pins CLK] [get_bd_pins sclk_resetn_sync/new_clk] -boundary_type upper

  # Restore current instance
  current_bd_instance $oldCurInst
}

# Hierarchical cell: slices
proc create_hier_cell_slices { parentCell nameHier } {

  variable script_folder

  if { $parentCell eq "" || $nameHier eq "" } {
     catch {common::send_gid_msg -ssname BD::TCL -id 2092 -severity "ERROR" "create_hier_cell_slices() - Empty argument(s)!"}
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
  create_bd_pin -dir I -from 7 -to 0 CSn
  create_bd_pin -dir O -from 0 -to 0 dc_link_adc_cs_n
  create_bd_pin -dir O -from 0 -to 0 dc_link_oc_err
  create_bd_pin -dir O -from 0 -to 0 dc_link_ov_err
  create_bd_pin -dir O -from 0 -to 0 dc_link_uc_err
  create_bd_pin -dir O -from 0 -to 0 dc_link_uv_err
  create_bd_pin -dir O -from 0 -to 0 motor_adc_cs_n
  create_bd_pin -dir I -from 7 -to 0 over_fault
  create_bd_pin -dir O -from 0 -to 0 phase_a_oc_err
  create_bd_pin -dir O -from 0 -to 0 phase_a_uc_err
  create_bd_pin -dir O -from 0 -to 0 phase_b_oc_err
  create_bd_pin -dir O -from 0 -to 0 phase_b_uc_err
  create_bd_pin -dir O -from 0 -to 0 phase_c_oc_err
  create_bd_pin -dir O -from 0 -to 0 phase_c_uc_err
  create_bd_pin -dir I -from 7 -to 0 under_fault

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


  # Create instance: xlslice_oc_err_1, and set properties
  set xlslice_oc_err_1 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlslice xlslice_oc_err_1 ]
  set_property -dict [list \
    CONFIG.DIN_FROM {1} \
    CONFIG.DIN_TO {1} \
    CONFIG.DIN_WIDTH {8} \
    CONFIG.DOUT_WIDTH {1} \
  ] $xlslice_oc_err_1


  # Create instance: xlslice_oc_err_3, and set properties
  set xlslice_oc_err_3 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlslice xlslice_oc_err_3 ]
  set_property -dict [list \
    CONFIG.DIN_FROM {3} \
    CONFIG.DIN_TO {3} \
    CONFIG.DIN_WIDTH {8} \
    CONFIG.DOUT_WIDTH {1} \
  ] $xlslice_oc_err_3


  # Create instance: xlslice_oc_err_5, and set properties
  set xlslice_oc_err_5 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlslice xlslice_oc_err_5 ]
  set_property -dict [list \
    CONFIG.DIN_FROM {5} \
    CONFIG.DIN_TO {5} \
    CONFIG.DIN_WIDTH {8} \
  ] $xlslice_oc_err_5


  # Create instance: xlslice_oc_err_7, and set properties
  set xlslice_oc_err_7 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlslice xlslice_oc_err_7 ]
  set_property -dict [list \
    CONFIG.DIN_FROM {7} \
    CONFIG.DIN_TO {7} \
    CONFIG.DIN_WIDTH {8} \
    CONFIG.DOUT_WIDTH {1} \
  ] $xlslice_oc_err_7


  # Create instance: xlslice_ov_err_6, and set properties
  set xlslice_ov_err_6 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlslice xlslice_ov_err_6 ]
  set_property -dict [list \
    CONFIG.DIN_FROM {6} \
    CONFIG.DIN_TO {6} \
    CONFIG.DIN_WIDTH {8} \
    CONFIG.DOUT_WIDTH {1} \
  ] $xlslice_ov_err_6


  # Create instance: xlslice_uc_err_1, and set properties
  set xlslice_uc_err_1 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlslice xlslice_uc_err_1 ]
  set_property -dict [list \
    CONFIG.DIN_FROM {1} \
    CONFIG.DIN_TO {1} \
    CONFIG.DIN_WIDTH {8} \
    CONFIG.DOUT_WIDTH {1} \
  ] $xlslice_uc_err_1


  # Create instance: xlslice_uc_err_3, and set properties
  set xlslice_uc_err_3 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlslice xlslice_uc_err_3 ]
  set_property -dict [list \
    CONFIG.DIN_FROM {3} \
    CONFIG.DIN_TO {3} \
    CONFIG.DIN_WIDTH {8} \
    CONFIG.DOUT_WIDTH {1} \
  ] $xlslice_uc_err_3


  # Create instance: xlslice_uc_err_5, and set properties
  set xlslice_uc_err_5 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlslice xlslice_uc_err_5 ]
  set_property -dict [list \
    CONFIG.DIN_FROM {5} \
    CONFIG.DIN_TO {5} \
    CONFIG.DIN_WIDTH {8} \
  ] $xlslice_uc_err_5


  # Create instance: xlslice_uc_err_7, and set properties
  set xlslice_uc_err_7 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlslice xlslice_uc_err_7 ]
  set_property -dict [list \
    CONFIG.DIN_FROM {7} \
    CONFIG.DIN_TO {7} \
    CONFIG.DIN_WIDTH {8} \
    CONFIG.DOUT_WIDTH {1} \
  ] $xlslice_uc_err_7


  # Create instance: xlslice_uv_err_6, and set properties
  set xlslice_uv_err_6 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlslice xlslice_uv_err_6 ]
  set_property -dict [list \
    CONFIG.DIN_FROM {6} \
    CONFIG.DIN_TO {6} \
    CONFIG.DIN_WIDTH {8} \
    CONFIG.DOUT_WIDTH {1} \
  ] $xlslice_uv_err_6


  # Create port connections
  connect_bd_net -net adc_hub_1_over_fault [get_bd_pins over_fault] [get_bd_pins xlslice_oc_err_1/Din] -boundary_type upper
  connect_bd_net -net adc_hub_1_over_fault [get_bd_pins over_fault] [get_bd_pins xlslice_oc_err_3/Din] -boundary_type upper
  connect_bd_net -net adc_hub_1_over_fault [get_bd_pins over_fault] [get_bd_pins xlslice_oc_err_5/Din] -boundary_type upper
  connect_bd_net -net adc_hub_1_over_fault [get_bd_pins over_fault] [get_bd_pins xlslice_oc_err_7/Din] -boundary_type upper
  connect_bd_net -net adc_hub_1_over_fault [get_bd_pins over_fault] [get_bd_pins xlslice_ov_err_6/Din] -boundary_type upper
  connect_bd_net -net adc_hub_phase_dc_under_fault [get_bd_pins under_fault] [get_bd_pins xlslice_uc_err_1/Din] -boundary_type upper
  connect_bd_net -net adc_hub_phase_dc_under_fault [get_bd_pins under_fault] [get_bd_pins xlslice_uc_err_3/Din] -boundary_type upper
  connect_bd_net -net adc_hub_phase_dc_under_fault [get_bd_pins under_fault] [get_bd_pins xlslice_uc_err_5/Din] -boundary_type upper
  connect_bd_net -net adc_hub_phase_dc_under_fault [get_bd_pins under_fault] [get_bd_pins xlslice_uc_err_7/Din] -boundary_type upper
  connect_bd_net -net adc_hub_phase_dc_under_fault [get_bd_pins under_fault] [get_bd_pins xlslice_uv_err_6/Din] -boundary_type upper
  connect_bd_net -net adc_if_0_CSn [get_bd_pins CSn] [get_bd_pins xlslice_CSn/Din] -boundary_type upper
  connect_bd_net -net adc_if_0_CSn [get_bd_pins CSn] [get_bd_pins xlslice_dc_CSn/Din] -boundary_type upper
  connect_bd_net -net xlslice_CSn_Dout [get_bd_pins xlslice_CSn/Dout] [get_bd_pins motor_adc_cs_n]
  connect_bd_net -net xlslice_dc_CSn_Dout [get_bd_pins xlslice_dc_CSn/Dout] [get_bd_pins dc_link_adc_cs_n]
  connect_bd_net -net xlslice_oc_err_1_Dout [get_bd_pins xlslice_oc_err_1/Dout] [get_bd_pins phase_a_oc_err]
  connect_bd_net -net xlslice_oc_err_3_Dout [get_bd_pins xlslice_oc_err_3/Dout] [get_bd_pins phase_b_oc_err]
  connect_bd_net -net xlslice_oc_err_5_Dout [get_bd_pins xlslice_oc_err_5/Dout] [get_bd_pins phase_c_oc_err]
  connect_bd_net -net xlslice_oc_err_7_Dout [get_bd_pins xlslice_oc_err_7/Dout] [get_bd_pins dc_link_oc_err]
  connect_bd_net -net xlslice_ov_err_6_Dout [get_bd_pins xlslice_ov_err_6/Dout] [get_bd_pins dc_link_ov_err]
  connect_bd_net -net xlslice_uc_err_1_Dout [get_bd_pins xlslice_uc_err_1/Dout] [get_bd_pins phase_a_uc_err]
  connect_bd_net -net xlslice_uc_err_3_Dout [get_bd_pins xlslice_uc_err_3/Dout] [get_bd_pins phase_b_uc_err]
  connect_bd_net -net xlslice_uc_err_5_Dout [get_bd_pins xlslice_uc_err_5/Dout] [get_bd_pins phase_c_uc_err]
  connect_bd_net -net xlslice_uc_err_7_Dout [get_bd_pins xlslice_uc_err_7/Dout] [get_bd_pins dc_link_uc_err]
  connect_bd_net -net xlslice_uv_err_6_Dout [get_bd_pins xlslice_uv_err_6/Dout] [get_bd_pins dc_link_uv_err]

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

  create_bd_intf_pin -mode Master -vlnv xilinx.com:interface:axis_rtl:1.0 dc_v_AXIS

  create_bd_intf_pin -mode Master -vlnv xilinx.com:interface:axis_rtl:1.0 pa_i_AXIS

  create_bd_intf_pin -mode Master -vlnv xilinx.com:interface:axis_rtl:1.0 pb_i_AXIS

  create_bd_intf_pin -mode Master -vlnv xilinx.com:interface:axis_rtl:1.0 pc_i_AXIS


  # Create pins
  create_bd_pin -dir I -type clk CLK
  create_bd_pin -dir I -type rst RESETn
  create_bd_pin -dir I -type clk SCLK
  create_bd_pin -dir I -type rst SCLK_RESETn
  create_bd_pin -dir I -from 7 -to 0 SDATA
  create_bd_pin -dir I motor_pwm
  create_bd_pin -dir O -from 0 -to 0 dc_link_oc_err
  create_bd_pin -dir O -from 0 -to 0 dc_link_ov_err
  create_bd_pin -dir O -from 0 -to 0 dc_link_uc_err
  create_bd_pin -dir O -from 0 -to 0 dc_link_uv_err
  create_bd_pin -dir O -from 0 -to 0 motor_adc_sample_cmd
  create_bd_pin -dir O -from 0 -to 0 motor_adc_cs_n
  create_bd_pin -dir O -from 0 -to 0 dc_link_adc_cs_n
  create_bd_pin -dir O -type intr interrupt
  create_bd_pin -dir O -from 0 -to 0 phase_a_oc_err
  create_bd_pin -dir O -from 0 -to 0 phase_a_uc_err
  create_bd_pin -dir O -from 0 -to 0 phase_b_oc_err
  create_bd_pin -dir O -from 0 -to 0 phase_b_uc_err
  create_bd_pin -dir O -from 0 -to 0 phase_c_oc_err
  create_bd_pin -dir O -from 0 -to 0 phase_c_uc_err

  # Create instance: adc_hub_phase_dc, and set properties
  set adc_hub_phase_dc [ create_bd_cell -type ip -vlnv xilinx.com:user:adc_hub adc_hub_phase_dc ]
  set_property -dict [list \
    CONFIG.L0_CODE {BTC} \
    CONFIG.L0_TYPE {VOLTAGE} \
    CONFIG.L1_CODE {BTC} \
    CONFIG.L1_TYPE {CURRENT} \
    CONFIG.L2_CODE {BTC} \
    CONFIG.L2_TYPE {VOLTAGE} \
    CONFIG.L3_CODE {BTC} \
    CONFIG.L3_TYPE {CURRENT} \
    CONFIG.L4_CODE {BTC} \
    CONFIG.L4_TYPE {VOLTAGE} \
    CONFIG.L5_CODE {BTC} \
    CONFIG.L5_TYPE {CURRENT} \
    CONFIG.L6_CODE {BTC} \
    CONFIG.L6_TYPE {VOLTAGE} \
    CONFIG.L7_CODE {BTC} \
    CONFIG.L7_TYPE {CURRENT} \
    CONFIG.NUM_CHANNELS {8} \
  ] $adc_hub_phase_dc


  # Create instance: adc_if, and set properties
  set adc_if [ create_bd_cell -type ip -vlnv xilinx.com:user:adc7352_if adc_if ]
  set_property CONFIG.NUM_CHANNELS {8} $adc_if


  # Create instance: adc_usb2btc, and set properties
  set adc_usb2btc [ create_bd_cell -type ip -vlnv xilinx.com:user:adc_usb2btc adc_usb2btc ]
  set_property CONFIG.NUM_CHANNELS {8} $adc_usb2btc


  # Create instance: adc_bob2btc, and set properties
  set adc_bob2btc [ create_bd_cell -type ip -vlnv xilinx.com:user:adc_bob2btc adc_bob2btc ]
  set_property CONFIG.NUM_CHANNELS {8} $adc_bob2btc


  # Create instance: adc_sample_ctrl_dc, and set properties
  set adc_sample_ctrl_dc [ create_bd_cell -type ip -vlnv user.org:user:adc_sample_ctrl adc_sample_ctrl_dc ]
  set_property -dict [list \
    CONFIG.CLK_PERIOD_NS {9.999001} \
    CONFIG.SCLK_PERIOD_NS {20.833541} \
  ] $adc_sample_ctrl_dc


  # Create instance: adc_sample_ctrl_motor, and set properties
  set adc_sample_ctrl_motor [ create_bd_cell -type ip -vlnv user.org:user:adc_sample_ctrl adc_sample_ctrl_motor ]
  set_property -dict [list \
    CONFIG.CLK_PERIOD_NS {9.999001} \
    CONFIG.SCLK_PERIOD_NS {20.833541} \
  ] $adc_sample_ctrl_motor


  # Create instance: slices
  create_hier_cell_slices $hier_obj slices

  # Create instance: sync
  create_hier_cell_sync $hier_obj sync

  # Create instance: sync_vector
  create_hier_cell_sync_vector $hier_obj sync_vector

  # Create instance: xlconcat_update, and set properties
  set xlconcat_update [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlconcat xlconcat_update ]
  set_property CONFIG.NUM_PORTS {8} $xlconcat_update


  # Create instance: xlconstant_gnd, and set properties
  set xlconstant_gnd [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlconstant xlconstant_gnd ]

  # Create interface connections
  connect_bd_intf_net -intf_net Conn1 [get_bd_intf_pins pc_i_AXIS] [get_bd_intf_pins adc_hub_phase_dc/L5_AXIS]
  connect_bd_intf_net -intf_net Conn2 [get_bd_intf_pins dc_v_AXIS] [get_bd_intf_pins adc_hub_phase_dc/L6_AXIS]
  connect_bd_intf_net -intf_net adc_bob2btc_L1_SYS [get_bd_intf_pins adc_hub_phase_dc/L1_ADC] [get_bd_intf_pins adc_bob2btc/L1_SYS]
  connect_bd_intf_net -intf_net adc_bob2btc_L3_SYS [get_bd_intf_pins adc_hub_phase_dc/L3_ADC] [get_bd_intf_pins adc_bob2btc/L3_SYS]
  connect_bd_intf_net -intf_net adc_bob2btc_L5_SYS [get_bd_intf_pins adc_hub_phase_dc/L5_ADC] [get_bd_intf_pins adc_bob2btc/L5_SYS]
  connect_bd_intf_net -intf_net adc_hub_0_L2_AXIS [get_bd_intf_pins pb_i_AXIS] [get_bd_intf_pins adc_hub_phase_dc/L3_AXIS]
  connect_bd_intf_net -intf_net adc_hub_1_L0_AXIS [get_bd_intf_pins pa_i_AXIS] [get_bd_intf_pins adc_hub_phase_dc/L1_AXIS]
  connect_bd_intf_net -intf_net adc_if_L0_ADC [get_bd_intf_pins adc_if/L0_ADC] [get_bd_intf_pins sync_vector/DATA_IN0]
  connect_bd_intf_net -intf_net adc_if_L1_ADC [get_bd_intf_pins adc_if/L1_ADC] [get_bd_intf_pins sync_vector/DATA_IN1]
  connect_bd_intf_net -intf_net adc_if_L2_ADC [get_bd_intf_pins adc_if/L2_ADC] [get_bd_intf_pins sync_vector/DATA_IN2]
  connect_bd_intf_net -intf_net adc_if_L3_ADC [get_bd_intf_pins adc_if/L3_ADC] [get_bd_intf_pins sync_vector/DATA_IN3]
  connect_bd_intf_net -intf_net adc_if_L4_ADC [get_bd_intf_pins adc_if/L4_ADC] [get_bd_intf_pins sync_vector/DATA_IN4]
  connect_bd_intf_net -intf_net adc_if_L5_ADC [get_bd_intf_pins adc_if/L5_ADC] [get_bd_intf_pins sync_vector/DATA_IN5]
  connect_bd_intf_net -intf_net adc_if_L6_ADC [get_bd_intf_pins adc_if/L6_ADC] [get_bd_intf_pins sync_vector/DATA_IN6]
  connect_bd_intf_net -intf_net adc_if_L7_ADC [get_bd_intf_pins adc_if/L7_ADC] [get_bd_intf_pins sync_vector/DATA_IN7]
  connect_bd_intf_net -intf_net adc_usb2btc_L0_SYS [get_bd_intf_pins adc_hub_phase_dc/L0_ADC] [get_bd_intf_pins adc_usb2btc/L0_SYS]
  connect_bd_intf_net -intf_net adc_usb2btc_L2_SYS [get_bd_intf_pins adc_hub_phase_dc/L2_ADC] [get_bd_intf_pins adc_usb2btc/L2_SYS]
  connect_bd_intf_net -intf_net adc_usb2btc_L4_SYS [get_bd_intf_pins adc_hub_phase_dc/L4_ADC] [get_bd_intf_pins adc_usb2btc/L4_SYS]
  connect_bd_intf_net -intf_net adc_usb2btc_L6_SYS [get_bd_intf_pins adc_hub_phase_dc/L6_ADC] [get_bd_intf_pins adc_usb2btc/L6_SYS]
  connect_bd_intf_net -intf_net adc_usb2btc_L7_SYS [get_bd_intf_pins adc_hub_phase_dc/L7_ADC] [get_bd_intf_pins adc_usb2btc/L7_SYS]
  connect_bd_intf_net -intf_net axi_interconnect_0_M00_AXI [get_bd_intf_pins S_AXI] [get_bd_intf_pins adc_hub_phase_dc/S_AXI]
  connect_bd_intf_net -intf_net synchronizer_vector_0_DATA_OUT [get_bd_intf_pins adc_usb2btc/L0_ADC] [get_bd_intf_pins sync_vector/DATA_OUT0]
  connect_bd_intf_net -intf_net synchronizer_vector_1_DATA_OUT [get_bd_intf_pins adc_bob2btc/L1_ADC] [get_bd_intf_pins sync_vector/DATA_OUT1]
  connect_bd_intf_net -intf_net synchronizer_vector_2_DATA_OUT [get_bd_intf_pins adc_usb2btc/L2_ADC] [get_bd_intf_pins sync_vector/DATA_OUT2]
  connect_bd_intf_net -intf_net synchronizer_vector_3_DATA_OUT [get_bd_intf_pins adc_bob2btc/L3_ADC] [get_bd_intf_pins sync_vector/DATA_OUT3]
  connect_bd_intf_net -intf_net synchronizer_vector_4_DATA_OUT [get_bd_intf_pins adc_usb2btc/L4_ADC] [get_bd_intf_pins sync_vector/DATA_OUT4]
  connect_bd_intf_net -intf_net synchronizer_vector_5_DATA_OUT [get_bd_intf_pins adc_bob2btc/L5_ADC] [get_bd_intf_pins sync_vector/DATA_OUT5]
  connect_bd_intf_net -intf_net synchronizer_vector_6_DATA_OUT [get_bd_intf_pins adc_usb2btc/L6_ADC] [get_bd_intf_pins sync_vector/DATA_OUT6]
  connect_bd_intf_net -intf_net synchronizer_vector_7_DATA_OUT [get_bd_intf_pins adc_usb2btc/L7_ADC] [get_bd_intf_pins sync_vector/DATA_OUT7]

  # Create port connections
  connect_bd_net -net SCLK_1 [get_bd_pins SCLK] [get_bd_pins adc_if/SCLK] -boundary_type upper
  connect_bd_net -net SCLK_1 [get_bd_pins SCLK] [get_bd_pins sync/SCLK] -boundary_type upper
  connect_bd_net -net SCLK_1 [get_bd_pins SCLK] [get_bd_pins sync_vector/SCLK] -boundary_type upper
  connect_bd_net -net SDATA_0_1 [get_bd_pins SDATA] [get_bd_pins adc_if/SDATA]
  connect_bd_net -net adc_hub_1_interrupt [get_bd_pins adc_hub_phase_dc/interrupt] [get_bd_pins interrupt]
  connect_bd_net -net adc_hub_1_over_fault [get_bd_pins adc_hub_phase_dc/over_fault] [get_bd_pins slices/over_fault]
  connect_bd_net -net adc_hub_phase_dc_under_fault [get_bd_pins adc_hub_phase_dc/under_fault] [get_bd_pins slices/under_fault]
  connect_bd_net -net adc_if_0_CSn [get_bd_pins adc_if/CSn] [get_bd_pins slices/CSn]
  connect_bd_net -net adc_sample_ctrl_dc_sample_adc [get_bd_pins adc_sample_ctrl_dc/sample_adc] [get_bd_pins sync/update_dc_in]
  connect_bd_net -net adc_sample_ctrl_motor_sample_adc [get_bd_pins adc_sample_ctrl_motor/sample_adc] [get_bd_pins sync/update_motor_in]
  connect_bd_net -net dc_csn_sync_data_out [get_bd_pins sync/csn_dc_out] [get_bd_pins adc_sample_ctrl_dc/csn]
  connect_bd_net -net dc_update_out [get_bd_pins sync/update_dc_out] [get_bd_pins xlconcat_update/In6] -boundary_type upper
  connect_bd_net -net dc_update_out [get_bd_pins sync/update_dc_out] [get_bd_pins xlconcat_update/In7] -boundary_type upper
  connect_bd_net -net motor_csn_sync_data_out [get_bd_pins sync/csn_motor_out] [get_bd_pins adc_sample_ctrl_motor/csn]
  connect_bd_net -net motor_update_out [get_bd_pins sync/update_motor_out] [get_bd_pins motor_adc_sample_cmd] -boundary_type upper
  connect_bd_net -net motor_update_out [get_bd_pins sync/update_motor_out] [get_bd_pins xlconcat_update/In0] -boundary_type upper
  connect_bd_net -net motor_update_out [get_bd_pins sync/update_motor_out] [get_bd_pins xlconcat_update/In1] -boundary_type upper
  connect_bd_net -net motor_update_out [get_bd_pins sync/update_motor_out] [get_bd_pins xlconcat_update/In2] -boundary_type upper
  connect_bd_net -net motor_update_out [get_bd_pins sync/update_motor_out] [get_bd_pins xlconcat_update/In3] -boundary_type upper
  connect_bd_net -net motor_update_out [get_bd_pins sync/update_motor_out] [get_bd_pins xlconcat_update/In4] -boundary_type upper
  connect_bd_net -net motor_update_out [get_bd_pins sync/update_motor_out] [get_bd_pins xlconcat_update/In5] -boundary_type upper
  connect_bd_net -net phase_a_motor_v_pwm_sync [get_bd_pins motor_pwm] [get_bd_pins adc_sample_ctrl_motor/pwm_sync]
  connect_bd_net -net proc_sys_reset_1_peripheral_aresetn [get_bd_pins SCLK_RESETn] [get_bd_pins adc_if/RESETn] -boundary_type upper
  connect_bd_net -net proc_sys_reset_1_peripheral_aresetn [get_bd_pins SCLK_RESETn] [get_bd_pins sync/SCLK_RESETn] -boundary_type upper
  connect_bd_net -net sclk_resetn_sync_data_out [get_bd_pins sync/sclk_resetn_out] [get_bd_pins adc_sample_ctrl_dc/sresetn] -boundary_type upper
  connect_bd_net -net sclk_resetn_sync_data_out [get_bd_pins sync/sclk_resetn_out] [get_bd_pins adc_sample_ctrl_motor/sresetn] -boundary_type upper
  connect_bd_net -net xlconcat_update_dout [get_bd_pins xlconcat_update/dout] [get_bd_pins adc_if/UPDATE]
  connect_bd_net -net xlconstant_gnd_dout [get_bd_pins xlconstant_gnd/dout] [get_bd_pins adc_sample_ctrl_dc/pwm_sync]
  connect_bd_net -net xlslice_CSn_Dout [get_bd_pins slices/motor_adc_cs_n] [get_bd_pins motor_adc_cs_n] -boundary_type upper
  connect_bd_net -net xlslice_CSn_Dout [get_bd_pins slices/motor_adc_cs_n] [get_bd_pins sync/csn_motor_in] -boundary_type upper
  connect_bd_net -net xlslice_dc_CSn_Dout [get_bd_pins slices/dc_link_adc_cs_n] [get_bd_pins dc_link_adc_cs_n] -boundary_type upper
  connect_bd_net -net xlslice_dc_CSn_Dout [get_bd_pins slices/dc_link_adc_cs_n] [get_bd_pins sync/csn_dc_in] -boundary_type upper
  connect_bd_net -net xlslice_oc_err_1_Dout [get_bd_pins slices/phase_a_oc_err] [get_bd_pins phase_a_oc_err]
  connect_bd_net -net xlslice_oc_err_3_Dout [get_bd_pins slices/phase_b_oc_err] [get_bd_pins phase_b_oc_err]
  connect_bd_net -net xlslice_oc_err_5_Dout [get_bd_pins slices/phase_c_oc_err] [get_bd_pins phase_c_oc_err]
  connect_bd_net -net xlslice_oc_err_7_Dout [get_bd_pins slices/dc_link_oc_err] [get_bd_pins dc_link_oc_err]
  connect_bd_net -net xlslice_ov_err_6_Dout [get_bd_pins slices/dc_link_ov_err] [get_bd_pins dc_link_ov_err]
  connect_bd_net -net xlslice_uc_err_1_Dout [get_bd_pins slices/phase_a_uc_err] [get_bd_pins phase_a_uc_err]
  connect_bd_net -net xlslice_uc_err_3_Dout [get_bd_pins slices/phase_b_uc_err] [get_bd_pins phase_b_uc_err]
  connect_bd_net -net xlslice_uc_err_5_Dout [get_bd_pins slices/phase_c_uc_err] [get_bd_pins phase_c_uc_err]
  connect_bd_net -net xlslice_uc_err_7_Dout [get_bd_pins slices/dc_link_uc_err] [get_bd_pins dc_link_uc_err]
  connect_bd_net -net xlslice_uv_err_6_Dout [get_bd_pins slices/dc_link_uv_err] [get_bd_pins dc_link_uv_err]
  connect_bd_net -net zynq_ultra_ps_e_0_pl_clk0 [get_bd_pins CLK] [get_bd_pins adc_hub_phase_dc/S_AXI_ACLK] -boundary_type upper
  connect_bd_net -net zynq_ultra_ps_e_0_pl_clk0 [get_bd_pins CLK] [get_bd_pins adc_sample_ctrl_dc/clk] -boundary_type upper
  connect_bd_net -net zynq_ultra_ps_e_0_pl_clk0 [get_bd_pins CLK] [get_bd_pins adc_sample_ctrl_motor/clk] -boundary_type upper
  connect_bd_net -net zynq_ultra_ps_e_0_pl_clk0 [get_bd_pins CLK] [get_bd_pins adc_usb2btc/CLK] -boundary_type upper
  connect_bd_net -net zynq_ultra_ps_e_0_pl_clk0 [get_bd_pins CLK] [get_bd_pins adc_bob2btc/CLK] -boundary_type upper
  connect_bd_net -net zynq_ultra_ps_e_0_pl_clk0 [get_bd_pins CLK] [get_bd_pins sync/CLK] -boundary_type upper
  connect_bd_net -net zynq_ultra_ps_e_0_pl_clk0 [get_bd_pins CLK] [get_bd_pins sync_vector/CLK] -boundary_type upper
  connect_bd_net -net zynq_ultra_ps_e_0_pl_resetn0 [get_bd_pins RESETn] [get_bd_pins adc_hub_phase_dc/S_AXI_ARESETN] -boundary_type upper
  connect_bd_net -net zynq_ultra_ps_e_0_pl_resetn0 [get_bd_pins RESETn] [get_bd_pins adc_sample_ctrl_dc/resetn] -boundary_type upper
  connect_bd_net -net zynq_ultra_ps_e_0_pl_resetn0 [get_bd_pins RESETn] [get_bd_pins adc_sample_ctrl_motor/resetn] -boundary_type upper
  connect_bd_net -net zynq_ultra_ps_e_0_pl_resetn0 [get_bd_pins RESETn] [get_bd_pins adc_usb2btc/RESETn] -boundary_type upper
  connect_bd_net -net zynq_ultra_ps_e_0_pl_resetn0 [get_bd_pins RESETn] [get_bd_pins adc_bob2btc/RESETn] -boundary_type upper

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
  create_bd_pin -dir I phase_lower
  create_bd_pin -dir I phase_lower1
  create_bd_pin -dir I phase_lower2
  create_bd_pin -dir I phase_upper
  create_bd_pin -dir I phase_upper1
  create_bd_pin -dir I phase_upper2
  create_bd_pin -dir I -type clk sys_clk
  create_bd_pin -dir I -type rst sys_rstn

  # Create instance: gate_driver_A, and set properties
  set gate_driver_A [ create_bd_cell -type ip -vlnv xilinx.com:user:gate_driver gate_driver_A ]

  # Create instance: gate_driver_B, and set properties
  set gate_driver_B [ create_bd_cell -type ip -vlnv xilinx.com:user:gate_driver gate_driver_B ]

  # Create instance: gate_driver_C, and set properties
  set gate_driver_C [ create_bd_cell -type ip -vlnv xilinx.com:user:gate_driver gate_driver_C ]

  # Create port connections
  connect_bd_net -net gate_driver_A_gate_drive [get_bd_pins gate_driver_A/gate_drive] [get_bd_pins gate_drive_phase_a]
  connect_bd_net -net gate_driver_B_gate_drive [get_bd_pins gate_driver_B/gate_drive] [get_bd_pins gate_drive_phase_b]
  connect_bd_net -net gate_driver_C_gate_drive [get_bd_pins gate_driver_C/gate_drive] [get_bd_pins gate_drive_phase_c]
  connect_bd_net -net phase_lower1_1 [get_bd_pins phase_lower1] [get_bd_pins gate_driver_B/phase_lower]
  connect_bd_net -net phase_lower2_1 [get_bd_pins phase_lower2] [get_bd_pins gate_driver_C/phase_lower]
  connect_bd_net -net phase_lower_1 [get_bd_pins phase_lower] [get_bd_pins gate_driver_A/phase_lower]
  connect_bd_net -net phase_upper1_1 [get_bd_pins phase_upper1] [get_bd_pins gate_driver_B/phase_upper]
  connect_bd_net -net phase_upper2_1 [get_bd_pins phase_upper2] [get_bd_pins gate_driver_C/phase_upper]
  connect_bd_net -net phase_upper_1 [get_bd_pins phase_upper] [get_bd_pins gate_driver_A/phase_upper]
  connect_bd_net -net zynq_ultra_ps_e_0_pl_clk0 [get_bd_pins sys_clk] [get_bd_pins gate_driver_A/sys_clk] -boundary_type upper
  connect_bd_net -net zynq_ultra_ps_e_0_pl_clk0 [get_bd_pins sys_clk] [get_bd_pins gate_driver_B/sys_clk] -boundary_type upper
  connect_bd_net -net zynq_ultra_ps_e_0_pl_clk0 [get_bd_pins sys_clk] [get_bd_pins gate_driver_C/sys_clk] -boundary_type upper
  connect_bd_net -net zynq_ultra_ps_e_0_pl_resetn0 [get_bd_pins sys_rstn] [get_bd_pins gate_driver_A/sys_rstn] -boundary_type upper
  connect_bd_net -net zynq_ultra_ps_e_0_pl_resetn0 [get_bd_pins sys_rstn] [get_bd_pins gate_driver_B/sys_rstn] -boundary_type upper
  connect_bd_net -net zynq_ultra_ps_e_0_pl_resetn0 [get_bd_pins sys_rstn] [get_bd_pins gate_driver_C/sys_rstn] -boundary_type upper

  # Restore current instance
  current_bd_instance $oldCurInst
}

# Hierarchical cell: ethernet_subsystem
proc create_hier_cell_ethernet_subsystem { parentCell nameHier } {

  variable script_folder

  if { $parentCell eq "" || $nameHier eq "" } {
     catch {common::send_gid_msg -ssname BD::TCL -id 2092 -severity "ERROR" "create_hier_cell_ethernet_subsystem() - Empty argument(s)!"}
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

  create_bd_intf_pin -mode Master -vlnv xilinx.com:interface:mdio_rtl:1.0 gem1_mdio

  create_bd_intf_pin -mode Master -vlnv xilinx.com:interface:rgmii_rtl:1.0 gem1_rgmii

  create_bd_intf_pin -mode Master -vlnv xilinx.com:interface:mdio_rtl:1.0 gem2_mdio

  create_bd_intf_pin -mode Master -vlnv xilinx.com:interface:rgmii_rtl:1.0 gem2_rgmii


  # Create pins
  create_bd_pin -dir O -from 7 -to 0 dout
  create_bd_pin -dir I -type rst ext_reset_in
  create_bd_pin -dir I -type clk gem1_clk_in
  create_bd_pin -dir O -from 0 -to 0 -type rst gem1_phy_rst_n
  create_bd_pin -dir I -type clk gem2_clk_in
  create_bd_pin -dir O -from 0 -to 0 -type rst gem2_phy_rst_n
  create_bd_pin -dir I -type clk m_axi_mm2s_aclk

  # Create instance: axi_dma_gem2, and set properties
  set axi_dma_gem2 [ create_bd_cell -type ip -vlnv xilinx.com:ip:axi_dma axi_dma_gem2 ]

  # Create instance: axi_dma_gem1, and set properties
  set axi_dma_gem1 [ create_bd_cell -type ip -vlnv xilinx.com:ip:axi_dma axi_dma_gem1 ]

  # Create instance: axi_ethernet_gem2, and set properties
  set axi_ethernet_gem2 [ create_bd_cell -type ip -vlnv xilinx.com:ip:axi_ethernet axi_ethernet_gem2 ]
  set_property -dict [list \
    CONFIG.PHY_TYPE {RGMII} \
    CONFIG.SupportLevel {1} \
  ] $axi_ethernet_gem2


  # Create instance: axi_ethernet_gem1, and set properties
  set axi_ethernet_gem1 [ create_bd_cell -type ip -vlnv xilinx.com:ip:axi_ethernet axi_ethernet_gem1 ]
  set_property -dict [list \
    CONFIG.PHY_TYPE {RGMII} \
    CONFIG.SupportLevel {0} \
  ] $axi_ethernet_gem1


  # Create instance: axi_interconnect_0, and set properties
  set axi_interconnect_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:axi_interconnect axi_interconnect_0 ]
  set_property CONFIG.NUM_MI {4} $axi_interconnect_0


  # Create instance: axi_smc, and set properties
  set axi_smc [ create_bd_cell -type ip -vlnv xilinx.com:ip:smartconnect axi_smc ]
  set_property CONFIG.NUM_SI {6} $axi_smc


  # Create instance: clk_wiz, and set properties
  set clk_wiz [ create_bd_cell -type ip -vlnv xilinx.com:ip:clk_wiz clk_wiz ]
  set_property -dict [list \
    CONFIG.CLKOUT1_REQUESTED_OUT_FREQ {125} \
    CONFIG.CLKOUT2_REQUESTED_OUT_FREQ {333.33333} \
    CONFIG.CLKOUT2_USED {true} \
    CONFIG.NUM_OUT_CLKS {2} \
    CONFIG.PRIM_IN_FREQ {25} \
    CONFIG.PRIM_SOURCE {Single_ended_clock_capable_pin} \
    CONFIG.USE_LOCKED {false} \
    CONFIG.USE_RESET {false} \
  ] $clk_wiz


  # Create instance: clk_wiz_1, and set properties
  set clk_wiz_1 [ create_bd_cell -type ip -vlnv xilinx.com:ip:clk_wiz clk_wiz_1 ]
  set_property -dict [list \
    CONFIG.CLKOUT1_REQUESTED_OUT_FREQ {125} \
    CONFIG.PRIM_IN_FREQ {25} \
    CONFIG.PRIM_SOURCE {Single_ended_clock_capable_pin} \
    CONFIG.USE_LOCKED {false} \
    CONFIG.USE_RESET {false} \
  ] $clk_wiz_1


  # Create instance: proc_sys_reset_0, and set properties
  set proc_sys_reset_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:proc_sys_reset proc_sys_reset_0 ]

  # Create instance: xlconcat_0, and set properties
  set xlconcat_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlconcat xlconcat_0 ]
  set_property CONFIG.NUM_PORTS {8} $xlconcat_0


  # Create interface connections
  connect_bd_intf_net -intf_net S00_AXI_1 [get_bd_intf_pins S00_AXI] [get_bd_intf_pins axi_interconnect_0/S00_AXI]
  connect_bd_intf_net -intf_net axi_dma_gem1_M_AXIS_CNTRL [get_bd_intf_pins axi_dma_gem1/M_AXIS_CNTRL] [get_bd_intf_pins axi_ethernet_gem1/s_axis_txc]
  connect_bd_intf_net -intf_net axi_dma_gem1_M_AXIS_MM2S [get_bd_intf_pins axi_dma_gem1/M_AXIS_MM2S] [get_bd_intf_pins axi_ethernet_gem1/s_axis_txd]
  connect_bd_intf_net -intf_net axi_dma_gem1_M_AXI_MM2S [get_bd_intf_pins axi_dma_gem1/M_AXI_MM2S] [get_bd_intf_pins axi_smc/S03_AXI]
  connect_bd_intf_net -intf_net axi_dma_gem1_M_AXI_S2MM [get_bd_intf_pins axi_dma_gem1/M_AXI_S2MM] [get_bd_intf_pins axi_smc/S04_AXI]
  connect_bd_intf_net -intf_net axi_dma_gem1_M_AXI_SG [get_bd_intf_pins axi_dma_gem1/M_AXI_SG] [get_bd_intf_pins axi_smc/S05_AXI]
  connect_bd_intf_net -intf_net axi_dma_gem2_M_AXIS_CNTRL [get_bd_intf_pins axi_dma_gem2/M_AXIS_CNTRL] [get_bd_intf_pins axi_ethernet_gem2/s_axis_txc]
  connect_bd_intf_net -intf_net axi_dma_gem2_M_AXIS_MM2S [get_bd_intf_pins axi_dma_gem2/M_AXIS_MM2S] [get_bd_intf_pins axi_ethernet_gem2/s_axis_txd]
  connect_bd_intf_net -intf_net axi_dma_gem2_M_AXI_MM2S [get_bd_intf_pins axi_dma_gem2/M_AXI_MM2S] [get_bd_intf_pins axi_smc/S00_AXI]
  connect_bd_intf_net -intf_net axi_dma_gem2_M_AXI_S2MM [get_bd_intf_pins axi_dma_gem2/M_AXI_S2MM] [get_bd_intf_pins axi_smc/S01_AXI]
  connect_bd_intf_net -intf_net axi_dma_gem2_M_AXI_SG [get_bd_intf_pins axi_dma_gem2/M_AXI_SG] [get_bd_intf_pins axi_smc/S02_AXI]
  connect_bd_intf_net -intf_net axi_ethernet_gem1_m_axis_rxd [get_bd_intf_pins axi_dma_gem1/S_AXIS_S2MM] [get_bd_intf_pins axi_ethernet_gem1/m_axis_rxd]
  connect_bd_intf_net -intf_net axi_ethernet_gem1_m_axis_rxs [get_bd_intf_pins axi_dma_gem1/S_AXIS_STS] [get_bd_intf_pins axi_ethernet_gem1/m_axis_rxs]
  connect_bd_intf_net -intf_net axi_ethernet_gem1_mdio [get_bd_intf_pins gem1_mdio] [get_bd_intf_pins axi_ethernet_gem1/mdio]
  connect_bd_intf_net -intf_net axi_ethernet_gem1_rgmii [get_bd_intf_pins gem1_rgmii] [get_bd_intf_pins axi_ethernet_gem1/rgmii]
  connect_bd_intf_net -intf_net axi_ethernet_gem2_m_axis_rxd [get_bd_intf_pins axi_dma_gem2/S_AXIS_S2MM] [get_bd_intf_pins axi_ethernet_gem2/m_axis_rxd]
  connect_bd_intf_net -intf_net axi_ethernet_gem2_m_axis_rxs [get_bd_intf_pins axi_dma_gem2/S_AXIS_STS] [get_bd_intf_pins axi_ethernet_gem2/m_axis_rxs]
  connect_bd_intf_net -intf_net axi_ethernet_gem2_mdio [get_bd_intf_pins gem2_mdio] [get_bd_intf_pins axi_ethernet_gem2/mdio]
  connect_bd_intf_net -intf_net axi_ethernet_gem2_rgmii [get_bd_intf_pins gem2_rgmii] [get_bd_intf_pins axi_ethernet_gem2/rgmii]
  connect_bd_intf_net -intf_net axi_interconnect_0_M00_AXI [get_bd_intf_pins axi_dma_gem2/S_AXI_LITE] [get_bd_intf_pins axi_interconnect_0/M00_AXI]
  connect_bd_intf_net -intf_net axi_interconnect_0_M01_AXI [get_bd_intf_pins axi_ethernet_gem2/s_axi] [get_bd_intf_pins axi_interconnect_0/M01_AXI]
  connect_bd_intf_net -intf_net axi_interconnect_0_M02_AXI [get_bd_intf_pins axi_dma_gem1/S_AXI_LITE] [get_bd_intf_pins axi_interconnect_0/M02_AXI]
  connect_bd_intf_net -intf_net axi_interconnect_0_M03_AXI [get_bd_intf_pins axi_ethernet_gem1/s_axi] [get_bd_intf_pins axi_interconnect_0/M03_AXI]
  connect_bd_intf_net -intf_net axi_smc_M00_AXI [get_bd_intf_pins M00_AXI] [get_bd_intf_pins axi_smc/M00_AXI]

  # Create port connections
  connect_bd_net -net axi_dma_gem1_mm2s_cntrl_reset_out_n [get_bd_pins axi_dma_gem1/mm2s_cntrl_reset_out_n] [get_bd_pins axi_ethernet_gem1/axi_txc_arstn]
  connect_bd_net -net axi_dma_gem1_mm2s_introut [get_bd_pins axi_dma_gem1/mm2s_introut] [get_bd_pins xlconcat_0/In4]
  connect_bd_net -net axi_dma_gem1_mm2s_prmry_reset_out_n [get_bd_pins axi_dma_gem1/mm2s_prmry_reset_out_n] [get_bd_pins axi_ethernet_gem1/axi_txd_arstn]
  connect_bd_net -net axi_dma_gem1_s2mm_introut [get_bd_pins axi_dma_gem1/s2mm_introut] [get_bd_pins xlconcat_0/In5]
  connect_bd_net -net axi_dma_gem1_s2mm_prmry_reset_out_n [get_bd_pins axi_dma_gem1/s2mm_prmry_reset_out_n] [get_bd_pins axi_ethernet_gem1/axi_rxd_arstn]
  connect_bd_net -net axi_dma_gem1_s2mm_sts_reset_out_n [get_bd_pins axi_dma_gem1/s2mm_sts_reset_out_n] [get_bd_pins axi_ethernet_gem1/axi_rxs_arstn]
  connect_bd_net -net axi_dma_gem2_mm2s_cntrl_reset_out_n [get_bd_pins axi_dma_gem2/mm2s_cntrl_reset_out_n] [get_bd_pins axi_ethernet_gem2/axi_txc_arstn]
  connect_bd_net -net axi_dma_gem2_mm2s_introut [get_bd_pins axi_dma_gem2/mm2s_introut] [get_bd_pins xlconcat_0/In2]
  connect_bd_net -net axi_dma_gem2_mm2s_prmry_reset_out_n [get_bd_pins axi_dma_gem2/mm2s_prmry_reset_out_n] [get_bd_pins axi_ethernet_gem2/axi_txd_arstn]
  connect_bd_net -net axi_dma_gem2_s2mm_introut [get_bd_pins axi_dma_gem2/s2mm_introut] [get_bd_pins xlconcat_0/In3]
  connect_bd_net -net axi_dma_gem2_s2mm_prmry_reset_out_n [get_bd_pins axi_dma_gem2/s2mm_prmry_reset_out_n] [get_bd_pins axi_ethernet_gem2/axi_rxd_arstn]
  connect_bd_net -net axi_dma_gem2_s2mm_sts_reset_out_n [get_bd_pins axi_dma_gem2/s2mm_sts_reset_out_n] [get_bd_pins axi_ethernet_gem2/axi_rxs_arstn]
  connect_bd_net -net axi_ethernet_gem1_interrupt [get_bd_pins axi_ethernet_gem1/interrupt] [get_bd_pins xlconcat_0/In7]
  connect_bd_net -net axi_ethernet_gem1_mac_irq [get_bd_pins axi_ethernet_gem1/mac_irq] [get_bd_pins xlconcat_0/In6]
  connect_bd_net -net axi_ethernet_gem1_phy_rst_n [get_bd_pins axi_ethernet_gem1/phy_rst_n] [get_bd_pins gem1_phy_rst_n]
  connect_bd_net -net axi_ethernet_gem2_interrupt [get_bd_pins axi_ethernet_gem2/interrupt] [get_bd_pins xlconcat_0/In1]
  connect_bd_net -net axi_ethernet_gem2_mac_irq [get_bd_pins axi_ethernet_gem2/mac_irq] [get_bd_pins xlconcat_0/In0]
  connect_bd_net -net axi_ethernet_gem2_phy_rst_n [get_bd_pins axi_ethernet_gem2/phy_rst_n] [get_bd_pins gem2_phy_rst_n]
  connect_bd_net -net clk_100MHz_1 [get_bd_pins gem2_clk_in] [get_bd_pins clk_wiz/clk_in1]
  connect_bd_net -net clk_wiz_0_clk_out1 [get_bd_pins m_axi_mm2s_aclk] [get_bd_pins axi_dma_gem2/m_axi_mm2s_aclk] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_out1 [get_bd_pins m_axi_mm2s_aclk] [get_bd_pins axi_dma_gem2/m_axi_s2mm_aclk] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_out1 [get_bd_pins m_axi_mm2s_aclk] [get_bd_pins axi_dma_gem2/m_axi_sg_aclk] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_out1 [get_bd_pins m_axi_mm2s_aclk] [get_bd_pins axi_dma_gem2/s_axi_lite_aclk] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_out1 [get_bd_pins m_axi_mm2s_aclk] [get_bd_pins axi_dma_gem1/m_axi_mm2s_aclk] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_out1 [get_bd_pins m_axi_mm2s_aclk] [get_bd_pins axi_dma_gem1/m_axi_s2mm_aclk] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_out1 [get_bd_pins m_axi_mm2s_aclk] [get_bd_pins axi_dma_gem1/m_axi_sg_aclk] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_out1 [get_bd_pins m_axi_mm2s_aclk] [get_bd_pins axi_dma_gem1/s_axi_lite_aclk] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_out1 [get_bd_pins m_axi_mm2s_aclk] [get_bd_pins axi_ethernet_gem2/axis_clk] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_out1 [get_bd_pins m_axi_mm2s_aclk] [get_bd_pins axi_ethernet_gem2/s_axi_lite_clk] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_out1 [get_bd_pins m_axi_mm2s_aclk] [get_bd_pins axi_ethernet_gem1/axis_clk] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_out1 [get_bd_pins m_axi_mm2s_aclk] [get_bd_pins axi_ethernet_gem1/s_axi_lite_clk] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_out1 [get_bd_pins m_axi_mm2s_aclk] [get_bd_pins axi_interconnect_0/ACLK] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_out1 [get_bd_pins m_axi_mm2s_aclk] [get_bd_pins axi_interconnect_0/M00_ACLK] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_out1 [get_bd_pins m_axi_mm2s_aclk] [get_bd_pins axi_interconnect_0/M01_ACLK] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_out1 [get_bd_pins m_axi_mm2s_aclk] [get_bd_pins axi_interconnect_0/M02_ACLK] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_out1 [get_bd_pins m_axi_mm2s_aclk] [get_bd_pins axi_interconnect_0/M03_ACLK] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_out1 [get_bd_pins m_axi_mm2s_aclk] [get_bd_pins axi_interconnect_0/S00_ACLK] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_out1 [get_bd_pins m_axi_mm2s_aclk] [get_bd_pins axi_smc/aclk] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_out1 [get_bd_pins m_axi_mm2s_aclk] [get_bd_pins proc_sys_reset_0/slowest_sync_clk] -boundary_type upper
  connect_bd_net -net clk_wiz_1_clk_out1 [get_bd_pins clk_wiz_1/clk_out1] [get_bd_pins axi_ethernet_gem1/gtx_clk]
  connect_bd_net -net clk_wiz_clk_out1 [get_bd_pins clk_wiz/clk_out1] [get_bd_pins axi_ethernet_gem2/gtx_clk]
  connect_bd_net -net clk_wiz_clk_out2 [get_bd_pins clk_wiz/clk_out2] [get_bd_pins axi_ethernet_gem2/ref_clk]
  connect_bd_net -net gem1_clk_in_1 [get_bd_pins gem1_clk_in] [get_bd_pins clk_wiz_1/clk_in1]
  connect_bd_net -net proc_sys_reset_0_interconnect_aresetn [get_bd_pins proc_sys_reset_0/interconnect_aresetn] [get_bd_pins axi_interconnect_0/ARESETN]
  connect_bd_net -net proc_sys_reset_0_peripheral_aresetn [get_bd_pins proc_sys_reset_0/peripheral_aresetn] [get_bd_pins axi_dma_gem2/axi_resetn] -boundary_type upper
  connect_bd_net -net proc_sys_reset_0_peripheral_aresetn [get_bd_pins proc_sys_reset_0/peripheral_aresetn] [get_bd_pins axi_dma_gem1/axi_resetn] -boundary_type upper
  connect_bd_net -net proc_sys_reset_0_peripheral_aresetn [get_bd_pins proc_sys_reset_0/peripheral_aresetn] [get_bd_pins axi_ethernet_gem2/s_axi_lite_resetn] -boundary_type upper
  connect_bd_net -net proc_sys_reset_0_peripheral_aresetn [get_bd_pins proc_sys_reset_0/peripheral_aresetn] [get_bd_pins axi_ethernet_gem1/s_axi_lite_resetn] -boundary_type upper
  connect_bd_net -net proc_sys_reset_0_peripheral_aresetn [get_bd_pins proc_sys_reset_0/peripheral_aresetn] [get_bd_pins axi_interconnect_0/M00_ARESETN] -boundary_type upper
  connect_bd_net -net proc_sys_reset_0_peripheral_aresetn [get_bd_pins proc_sys_reset_0/peripheral_aresetn] [get_bd_pins axi_interconnect_0/M01_ARESETN] -boundary_type upper
  connect_bd_net -net proc_sys_reset_0_peripheral_aresetn [get_bd_pins proc_sys_reset_0/peripheral_aresetn] [get_bd_pins axi_interconnect_0/M02_ARESETN] -boundary_type upper
  connect_bd_net -net proc_sys_reset_0_peripheral_aresetn [get_bd_pins proc_sys_reset_0/peripheral_aresetn] [get_bd_pins axi_interconnect_0/M03_ARESETN] -boundary_type upper
  connect_bd_net -net proc_sys_reset_0_peripheral_aresetn [get_bd_pins proc_sys_reset_0/peripheral_aresetn] [get_bd_pins axi_interconnect_0/S00_ARESETN] -boundary_type upper
  connect_bd_net -net proc_sys_reset_0_peripheral_aresetn [get_bd_pins proc_sys_reset_0/peripheral_aresetn] [get_bd_pins axi_smc/aresetn] -boundary_type upper
  connect_bd_net -net xlconcat_0_dout [get_bd_pins xlconcat_0/dout] [get_bd_pins dout]
  connect_bd_net -net zynq_ultra_ps_e_0_pl_resetn0 [get_bd_pins ext_reset_in] [get_bd_pins proc_sys_reset_0/ext_reset_in]

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
  set GPIO [ create_bd_intf_port -mode Master -vlnv xilinx.com:interface:gpio_rtl:1.0 GPIO ]

  set gem1_mdio [ create_bd_intf_port -mode Master -vlnv xilinx.com:interface:mdio_rtl:1.0 gem1_mdio ]

  set gem1_rgmii [ create_bd_intf_port -mode Master -vlnv xilinx.com:interface:rgmii_rtl:1.0 gem1_rgmii ]

  set gem2_mdio [ create_bd_intf_port -mode Master -vlnv xilinx.com:interface:mdio_rtl:1.0 gem2_mdio ]

  set gem2_rgmii [ create_bd_intf_port -mode Master -vlnv xilinx.com:interface:rgmii_rtl:1.0 gem2_rgmii ]

  set GPIO2 [ create_bd_intf_port -mode Master -vlnv xilinx.com:interface:gpio_rtl:1.0 GPIO2 ]


  # Create ports
  set TQ_CSn [ create_bd_port -dir O -from 0 -to 0 TQ_CSn ]
  set TQ_SCLK [ create_bd_port -dir O TQ_SCLK ]
  set TQ_SDI [ create_bd_port -dir O TQ_SDI ]
  set TQ_SDO [ create_bd_port -dir I TQ_SDO ]
  set dc_link_adc_cs_n [ create_bd_port -dir O -from 0 -to 0 dc_link_adc_cs_n ]
  set dc_link_adc_sclk [ create_bd_port -dir O dc_link_adc_sclk ]
  set dc_link_data_i [ create_bd_port -dir I dc_link_data_i ]
  set dc_link_data_v [ create_bd_port -dir I dc_link_data_v ]
  set fan_en_b [ create_bd_port -dir O -from 0 -to 0 fan_en_b ]
  set gate_drive_en [ create_bd_port -dir O -from 0 -to 0 gate_drive_en ]
  set gate_drive_phase_a [ create_bd_port -dir O gate_drive_phase_a ]
  set gate_drive_phase_b [ create_bd_port -dir O gate_drive_phase_b ]
  set gate_drive_phase_c [ create_bd_port -dir O gate_drive_phase_c ]
  set gem1_clk_in [ create_bd_port -dir I -type clk -freq_hz 25000000 gem1_clk_in ]
  set gem1_phy_rst_n [ create_bd_port -dir O -from 0 -to 0 -type rst gem1_phy_rst_n ]
  set gem2_clk_in [ create_bd_port -dir I -type clk -freq_hz 25000000 gem2_clk_in ]
  set gem2_phy_rst_n [ create_bd_port -dir O -from 0 -to 0 -type rst gem2_phy_rst_n ]
  set motor_adc_cs_n [ create_bd_port -dir O -from 0 -to 0 motor_adc_cs_n ]
  set motor_adc_sclk [ create_bd_port -dir O motor_adc_sclk ]
  set motor_pa_data_i [ create_bd_port -dir I motor_pa_data_i ]
  set motor_pa_data_v [ create_bd_port -dir I motor_pa_data_v ]
  set motor_pb_data_i [ create_bd_port -dir I motor_pb_data_i ]
  set motor_pb_data_v [ create_bd_port -dir I motor_pb_data_v ]
  set motor_pc_data_i [ create_bd_port -dir I motor_pc_data_i ]
  set motor_pc_data_v [ create_bd_port -dir I motor_pc_data_v ]
  set qei_se_A [ create_bd_port -dir I qei_se_A ]
  set qei_se_B [ create_bd_port -dir I qei_se_B ]
  set qei_se_I [ create_bd_port -dir I qei_se_I ]
  set CAN_MOSI [ create_bd_port -dir O CAN_MOSI ]
  set CAN_MISO [ create_bd_port -dir I CAN_MISO ]
  set CAN_SCLK [ create_bd_port -dir O CAN_SCLK ]
  set CAN_CSn [ create_bd_port -dir O -from 0 -to 0 CAN_CSn ]
  set CAN_RST [ create_bd_port -dir O -from 0 -to 0 CAN_RST ]
  set CAN_INT [ create_bd_port -dir I CAN_INT ]

  # Create instance: axi_gpio_0, and set properties
  set axi_gpio_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:axi_gpio axi_gpio_0 ]
  set_property -dict [list \
    CONFIG.C_ALL_INPUTS {0} \
    CONFIG.C_ALL_INPUTS_2 {0} \
    CONFIG.C_ALL_OUTPUTS {0} \
    CONFIG.C_ALL_OUTPUTS_2 {0} \
    CONFIG.C_GPIO2_WIDTH {2} \
    CONFIG.C_GPIO_WIDTH {8} \
    CONFIG.C_IS_DUAL {1} \
  ] $axi_gpio_0


  # Create instance: axi_interconnect_cntrl, and set properties
  set axi_interconnect_cntrl [ create_bd_cell -type ip -vlnv xilinx.com:ip:axi_interconnect axi_interconnect_cntrl ]
  set_property CONFIG.NUM_MI {9} $axi_interconnect_cntrl


  # Create instance: axi_quad_spi_0, and set properties
  set axi_quad_spi_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:axi_quad_spi axi_quad_spi_0 ]
  set_property -dict [list \
    CONFIG.C_SCK_RATIO {16} \
    CONFIG.C_SPI_MODE {0} \
    CONFIG.Multiples16 {2} \
    CONFIG.QSPI_BOARD_INTERFACE {Custom} \
  ] $axi_quad_spi_0


  # Create instance: ethernet_subsystem
  create_hier_cell_ethernet_subsystem [current_bd_instance .] ethernet_subsystem

  # Create instance: gate_driver
  create_hier_cell_gate_driver [current_bd_instance .] gate_driver

  # Create instance: hls_pwm_gen_0, and set properties
  set hls_pwm_gen_0 [ create_bd_cell -type ip -vlnv xilinx.com:hls:hls_pwm_gen hls_pwm_gen_0 ]

  # Create instance: hls_qei_0, and set properties
  set hls_qei_0 [ create_bd_cell -type ip -vlnv xilinx.com:hls:hls_qei_axi hls_qei_0 ]

  # Create instance: hls_svpwm_duty_0, and set properties
  set hls_svpwm_duty_0 [ create_bd_cell -type ip -vlnv xilinx.com:hls:hls_svpwm_duty hls_svpwm_duty_0 ]

  # Create instance: proc_sys_reset_0, and set properties
  set proc_sys_reset_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:proc_sys_reset proc_sys_reset_0 ]

  # Create instance: proc_sys_reset_1, and set properties
  set proc_sys_reset_1 [ create_bd_cell -type ip -vlnv xilinx.com:ip:proc_sys_reset proc_sys_reset_1 ]

  # Create instance: xlconcat_0, and set properties
  set xlconcat_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlconcat xlconcat_0 ]
  set_property CONFIG.NUM_PORTS {8} $xlconcat_0


  # Create instance: xlconcat_int, and set properties
  set xlconcat_int [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlconcat xlconcat_int ]
  set_property CONFIG.NUM_PORTS {8} $xlconcat_int


  # Create instance: xlslice_fan, and set properties
  set xlslice_fan [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlslice xlslice_fan ]
  set_property -dict [list \
    CONFIG.DIN_FROM {2} \
    CONFIG.DIN_TO {2} \
    CONFIG.DIN_WIDTH {3} \
  ] $xlslice_fan


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


  # Create instance: hls_foc_periodic_0, and set properties
  set hls_foc_periodic_0 [ create_bd_cell -type ip -vlnv xilinx.com:hls:hls_foc_periodic hls_foc_periodic_0 ]

  # Create instance: broadcast_i
  create_hier_cell_broadcast_i [current_bd_instance .] broadcast_i

  # Create instance: motor_control_0, and set properties
  set motor_control_0 [ create_bd_cell -type ip -vlnv user.org:user:motor_control motor_control_0 ]

  # Create instance: ADC
  create_hier_cell_ADC [current_bd_instance .] ADC

  # Create instance: clk_wiz_0, and set properties
  set clk_wiz_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:clk_wiz clk_wiz_0 ]
  set_property -dict [list \
    CONFIG.CLKOUT2_JITTER {133.814} \
    CONFIG.CLKOUT2_PHASE_ERROR {87.181} \
    CONFIG.CLKOUT2_REQUESTED_OUT_FREQ {48} \
    CONFIG.CLKOUT2_USED {true} \
    CONFIG.CLK_OUT1_PORT {clk_out_100M} \
    CONFIG.CLK_OUT2_PORT {clk_out_48M} \
    CONFIG.MMCM_CLKOUT1_DIVIDE {25} \
    CONFIG.NUM_OUT_CLKS {2} \
    CONFIG.PRIM_SOURCE {Global_buffer} \
    CONFIG.RESET_PORT {resetn} \
    CONFIG.RESET_TYPE {ACTIVE_LOW} \
  ] $clk_wiz_0


  # Create instance: util_vector_logic_0, and set properties
  set util_vector_logic_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:util_vector_logic util_vector_logic_0 ]
  set_property -dict [list \
    CONFIG.C_OPERATION {not} \
    CONFIG.C_SIZE {1} \
  ] $util_vector_logic_0

  # Create instance: axi_quad_spi_CAN, and set properties
  set axi_quad_spi_CAN [ create_bd_cell -type ip -vlnv xilinx.com:ip:axi_quad_spi axi_quad_spi_CAN ]
  set_property -dict [list \
    CONFIG.C_SCK_RATIO {16} \
    CONFIG.C_SPI_MODE {0} \
    CONFIG.Multiples16 {1} \
    CONFIG.QSPI_BOARD_INTERFACE {Custom} \
  ] $axi_quad_spi_CAN


  # Create instance: invert_can_int, and set properties
  set invert_can_int [ create_bd_cell -type ip -vlnv xilinx.com:ip:util_vector_logic invert_can_int ]
  set_property -dict [list \
    CONFIG.C_OPERATION {not} \
    CONFIG.C_SIZE {1} \
  ] $invert_can_int


  # Create instance: xlconstant_can_rstn, and set properties
  set xlconstant_can_rstn [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlconstant xlconstant_can_rstn ]

  # Create interface connections
  connect_bd_intf_net -intf_net ADC_n_dc_v_AXIS [get_bd_intf_pins ADC/dc_v_AXIS] [get_bd_intf_pins hls_svpwm_duty_0/strm_dc_link]
  connect_bd_intf_net -intf_net ADC_n_pa_i_AXIS [get_bd_intf_pins ADC/pa_i_AXIS] [get_bd_intf_pins broadcast_i/S_AXIS0]
  connect_bd_intf_net -intf_net ADC_n_pb_i_AXIS [get_bd_intf_pins ADC/pb_i_AXIS] [get_bd_intf_pins broadcast_i/S_AXIS1]
  connect_bd_intf_net -intf_net ADC_n_pc_i_AXIS [get_bd_intf_pins ADC/pc_i_AXIS] [get_bd_intf_pins broadcast_i/S_AXIS2]
  connect_bd_intf_net -intf_net axi_gpio_0_GPIO2 [get_bd_intf_ports GPIO2] [get_bd_intf_pins axi_gpio_0/GPIO2]
  connect_bd_intf_net -intf_net axi_interconnect_cntrl_M00_AXI [get_bd_intf_pins axi_interconnect_cntrl/M00_AXI] [get_bd_intf_pins ADC/S_AXI]
  connect_bd_intf_net -intf_net axi_interconnect_cntrl_M01_AXI [get_bd_intf_pins axi_interconnect_cntrl/M01_AXI] [get_bd_intf_pins hls_qei_0/s_axi_qei_args]
  connect_bd_intf_net -intf_net axi_interconnect_cntrl_M02_AXI [get_bd_intf_pins axi_interconnect_cntrl/M02_AXI] [get_bd_intf_pins motor_control_0/s_axi_cntrl]
  connect_bd_intf_net -intf_net axi_interconnect_cntrl_M03_AXI [get_bd_intf_pins axi_interconnect_cntrl/M03_AXI] [get_bd_intf_pins hls_foc_periodic_0/s_axi_foc_args]
  connect_bd_intf_net -intf_net axi_interconnect_cntrl_M04_AXI [get_bd_intf_pins axi_interconnect_cntrl/M04_AXI] [get_bd_intf_pins hls_pwm_gen_0/s_axi_pwm_args]
  connect_bd_intf_net -intf_net axi_interconnect_cntrl_M05_AXI [get_bd_intf_pins axi_interconnect_cntrl/M05_AXI] [get_bd_intf_pins hls_svpwm_duty_0/s_axi_pwm_args]
  connect_bd_intf_net -intf_net axi_interconnect_cntrl_M06_AXI [get_bd_intf_pins axi_interconnect_cntrl/M06_AXI] [get_bd_intf_pins axi_quad_spi_0/AXI_LITE]
  connect_bd_intf_net -intf_net axi_interconnect_cntrl_M07_AXI [get_bd_intf_pins axi_interconnect_cntrl/M07_AXI] [get_bd_intf_pins axi_gpio_0/S_AXI]
  connect_bd_intf_net -intf_net axi_interconnect_cntrl_M08_AXI [get_bd_intf_pins axi_interconnect_cntrl/M08_AXI] [get_bd_intf_pins axi_quad_spi_CAN/AXI_LITE]
  connect_bd_intf_net -intf_net broadcast_i_M00_AXIS0 [get_bd_intf_pins broadcast_i/M00_AXIS0] [get_bd_intf_pins motor_control_0/s_axis_phase_a_i]
  connect_bd_intf_net -intf_net broadcast_i_M00_AXIS1 [get_bd_intf_pins broadcast_i/M00_AXIS1] [get_bd_intf_pins motor_control_0/s_axis_phase_b_i]
  connect_bd_intf_net -intf_net broadcast_i_M00_AXIS2 [get_bd_intf_pins broadcast_i/M00_AXIS2] [get_bd_intf_pins motor_control_0/s_axis_phase_c_i]
  connect_bd_intf_net -intf_net broadcast_i_M01_AXIS [get_bd_intf_pins broadcast_i/M01_AXIS] [get_bd_intf_pins hls_foc_periodic_0/Ia]
  connect_bd_intf_net -intf_net broadcast_i_M01_AXIS1 [get_bd_intf_pins broadcast_i/M01_AXIS1] [get_bd_intf_pins hls_foc_periodic_0/Ib]
  connect_bd_intf_net -intf_net broadcast_i_M01_AXIS2 [get_bd_intf_pins broadcast_i/M01_AXIS2] [get_bd_intf_pins hls_foc_periodic_0/Ic]
  connect_bd_intf_net -intf_net ethernet_subsystem_M00_AXI [get_bd_intf_pins ethernet_subsystem/M00_AXI] [get_bd_intf_pins PS_0/S_AXI_HP0_FPD]
  connect_bd_intf_net -intf_net ethernet_subsystem_gem2_mdio [get_bd_intf_ports gem1_mdio] [get_bd_intf_pins ethernet_subsystem/gem1_mdio]
  connect_bd_intf_net -intf_net ethernet_subsystem_gem2_rgmii [get_bd_intf_ports gem1_rgmii] [get_bd_intf_pins ethernet_subsystem/gem1_rgmii]
  connect_bd_intf_net -intf_net ethernet_subsystem_gem3_mdio [get_bd_intf_ports gem2_mdio] [get_bd_intf_pins ethernet_subsystem/gem2_mdio]
  connect_bd_intf_net -intf_net ethernet_subsystem_gem3_rgmii [get_bd_intf_ports gem2_rgmii] [get_bd_intf_pins ethernet_subsystem/gem2_rgmii]
  connect_bd_intf_net -intf_net hls_foc_periodic_0_Va_cmd [get_bd_intf_pins hls_svpwm_duty_0/strm_Va_cmd] [get_bd_intf_pins hls_foc_periodic_0/Va_cmd]
  connect_bd_intf_net -intf_net hls_foc_periodic_0_Vb_cmd [get_bd_intf_pins hls_svpwm_duty_0/strm_Vb_cmd] [get_bd_intf_pins hls_foc_periodic_0/Vb_cmd]
  connect_bd_intf_net -intf_net hls_foc_periodic_0_Vc_cmd [get_bd_intf_pins hls_svpwm_duty_0/strm_Vc_cmd] [get_bd_intf_pins hls_foc_periodic_0/Vc_cmd]
  connect_bd_intf_net -intf_net hls_qei_0_qei_RPM_THETA_m [get_bd_intf_pins hls_foc_periodic_0/SPEED_THETA_m] [get_bd_intf_pins hls_qei_0/qei_RPM_THETA_m]
  connect_bd_intf_net -intf_net hls_svpwm_duty_0_strm_duty_ratio_a [get_bd_intf_pins hls_pwm_gen_0/strm_duty_ratio_a] [get_bd_intf_pins hls_svpwm_duty_0/strm_duty_ratio_a]
  connect_bd_intf_net -intf_net hls_svpwm_duty_0_strm_duty_ratio_b [get_bd_intf_pins hls_pwm_gen_0/strm_duty_ratio_b] [get_bd_intf_pins hls_svpwm_duty_0/strm_duty_ratio_b]
  connect_bd_intf_net -intf_net hls_svpwm_duty_0_strm_duty_ratio_c [get_bd_intf_pins hls_pwm_gen_0/strm_duty_ratio_c] [get_bd_intf_pins hls_svpwm_duty_0/strm_duty_ratio_c]
  connect_bd_intf_net -intf_net zynq_ultra_ps_e_0_M_AXI_HPM0_FPD [get_bd_intf_pins axi_interconnect_cntrl/S00_AXI] [get_bd_intf_pins PS_0/M_AXI_HPM0_FPD]
  connect_bd_intf_net -intf_net zynq_ultra_ps_e_0_M_AXI_HPM0_LPD [get_bd_intf_pins ethernet_subsystem/S00_AXI] [get_bd_intf_pins PS_0/M_AXI_HPM0_LPD]

  # Create port connections
  connect_bd_net -net ADC_n_dc_link_adc_cs_n [get_bd_pins ADC/dc_link_adc_cs_n] [get_bd_ports dc_link_adc_cs_n]
  connect_bd_net -net ADC_n_dc_link_oc_err [get_bd_pins ADC/dc_link_oc_err] [get_bd_pins motor_control_0/dc_link_oc_err]
  connect_bd_net -net ADC_n_dc_link_ov_err [get_bd_pins ADC/dc_link_ov_err] [get_bd_pins motor_control_0/dc_link_ov_err]
  connect_bd_net -net ADC_n_dc_link_uc_err [get_bd_pins ADC/dc_link_uc_err] [get_bd_pins motor_control_0/dc_link_uc_err]
  connect_bd_net -net ADC_n_dc_link_uv_err [get_bd_pins ADC/dc_link_uv_err] [get_bd_pins motor_control_0/dc_link_uv_err]
  connect_bd_net -net ADC_n_interrupt [get_bd_pins ADC/interrupt] [get_bd_pins xlconcat_int/In0]
  connect_bd_net -net ADC_n_motor_adc_cs_n [get_bd_pins ADC/motor_adc_cs_n] [get_bd_ports motor_adc_cs_n]
  connect_bd_net -net ADC_n_phase_a_oc_err [get_bd_pins ADC/phase_a_oc_err] [get_bd_pins motor_control_0/phase_a_oc_err]
  connect_bd_net -net ADC_n_phase_a_uc_err [get_bd_pins ADC/phase_a_uc_err] [get_bd_pins motor_control_0/phase_a_uc_err]
  connect_bd_net -net ADC_n_phase_b_oc_err [get_bd_pins ADC/phase_b_oc_err] [get_bd_pins motor_control_0/phase_b_oc_err]
  connect_bd_net -net ADC_n_phase_b_uc_err [get_bd_pins ADC/phase_b_uc_err] [get_bd_pins motor_control_0/phase_b_uc_err]
  connect_bd_net -net ADC_n_phase_c_oc_err [get_bd_pins ADC/phase_c_oc_err] [get_bd_pins motor_control_0/phase_c_oc_err]
  connect_bd_net -net ADC_n_phase_c_uc_err [get_bd_pins ADC/phase_c_uc_err] [get_bd_pins motor_control_0/phase_c_uc_err]
  connect_bd_net -net ARESETN_1 [get_bd_pins proc_sys_reset_0/interconnect_aresetn] [get_bd_pins axi_interconnect_cntrl/ARESETN]
  connect_bd_net -net A_0_1 [get_bd_ports qei_se_A] [get_bd_pins hls_qei_0/qei_A_dout]
  connect_bd_net -net B_0_1 [get_bd_ports qei_se_B] [get_bd_pins hls_qei_0/qei_B_dout]
  connect_bd_net -net CAN_INT_1 [get_bd_pins invert_can_int/Res] [get_bd_pins xlconcat_int/In6]
  connect_bd_net -net CAN_INT_2 [get_bd_ports CAN_INT] [get_bd_pins invert_can_int/Op1]
  connect_bd_net -net I_0_1 [get_bd_ports qei_se_I] [get_bd_pins hls_qei_0/qei_I_dout]
  connect_bd_net -net TQ_SDO_1 [get_bd_ports TQ_SDO] [get_bd_pins axi_quad_spi_0/io1_i]
  connect_bd_net -net axi_quad_spi_0_io0_o [get_bd_pins axi_quad_spi_0/io0_o] [get_bd_ports TQ_SDI]
  connect_bd_net -net axi_quad_spi_0_ip2intc_irpt [get_bd_pins axi_quad_spi_0/ip2intc_irpt] [get_bd_pins xlconcat_int/In2]
  connect_bd_net -net axi_quad_spi_0_sck_o [get_bd_pins axi_quad_spi_0/sck_o] [get_bd_ports TQ_SCLK]
  connect_bd_net -net axi_quad_spi_0_ss_o [get_bd_pins axi_quad_spi_0/ss_o] [get_bd_ports TQ_CSn]
  connect_bd_net -net axi_quad_spi_1_io0_o [get_bd_pins axi_quad_spi_CAN/io0_o] [get_bd_ports CAN_MOSI]
  connect_bd_net -net axi_quad_spi_1_ip2intc_irpt [get_bd_pins axi_quad_spi_CAN/ip2intc_irpt] [get_bd_pins xlconcat_int/In7]
  connect_bd_net -net axi_quad_spi_1_sck_o [get_bd_pins axi_quad_spi_CAN/sck_o] [get_bd_ports CAN_SCLK]
  connect_bd_net -net axi_quad_spi_1_ss_o [get_bd_pins axi_quad_spi_CAN/ss_o] [get_bd_ports CAN_CSn]
  connect_bd_net -net clk_wiz_0_clk_out_20M1 [get_bd_pins util_vector_logic_0/Res] [get_bd_pins proc_sys_reset_1/slowest_sync_clk] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_out_20M1 [get_bd_pins util_vector_logic_0/Res] [get_bd_pins ADC/SCLK] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_out_48M [get_bd_pins clk_wiz_0/clk_out_48M] [get_bd_pins util_vector_logic_0/Op1] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_out_48M [get_bd_pins clk_wiz_0/clk_out_48M] [get_bd_ports dc_link_adc_sclk] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_out_48M [get_bd_pins clk_wiz_0/clk_out_48M] [get_bd_ports motor_adc_sclk] -boundary_type upper
  connect_bd_net -net clk_wiz_0_locked [get_bd_pins clk_wiz_0/locked] [get_bd_pins proc_sys_reset_1/dcm_locked] -boundary_type upper
  connect_bd_net -net clk_wiz_0_locked [get_bd_pins clk_wiz_0/locked] [get_bd_pins proc_sys_reset_0/dcm_locked] -boundary_type upper
  connect_bd_net -net dc_link_data_a_1 [get_bd_ports dc_link_data_v] [get_bd_pins xlconcat_0/In6]
  connect_bd_net -net dc_link_data_b_1 [get_bd_ports dc_link_data_i] [get_bd_pins xlconcat_0/In7]
  connect_bd_net -net ethernet_subsystem_dout [get_bd_pins ethernet_subsystem/dout] [get_bd_pins PS_0/pl_ps_irq0]
  connect_bd_net -net ethernet_subsystem_gem2_phy_rst_n [get_bd_pins ethernet_subsystem/gem1_phy_rst_n] [get_bd_ports gem1_phy_rst_n]
  connect_bd_net -net ethernet_subsystem_gem3_phy_rst_n [get_bd_pins ethernet_subsystem/gem2_phy_rst_n] [get_bd_ports gem2_phy_rst_n]
  connect_bd_net -net gate_driver_A_gate_drive [get_bd_pins gate_driver/gate_drive_phase_a] [get_bd_ports gate_drive_phase_a]
  connect_bd_net -net gate_driver_B_gate_drive [get_bd_pins gate_driver/gate_drive_phase_b] [get_bd_ports gate_drive_phase_b]
  connect_bd_net -net gate_driver_C_gate_drive [get_bd_pins gate_driver/gate_drive_phase_c] [get_bd_ports gate_drive_phase_c]
  connect_bd_net -net gem1_clk_in_1 [get_bd_ports gem1_clk_in] [get_bd_pins ethernet_subsystem/gem1_clk_in]
  connect_bd_net -net gem2_clk_in_1 [get_bd_ports gem2_clk_in] [get_bd_pins ethernet_subsystem/gem2_clk_in]
  connect_bd_net -net hls_foc_periodic_0_interrupt [get_bd_pins hls_foc_periodic_0/interrupt] [get_bd_pins xlconcat_int/In4]
  #connect_bd_net -net hls_pwm_gen_0_interrupt [get_bd_pins hls_pwm_gen_0/interrupt] [get_bd_pins xlconcat_int/In6]
  connect_bd_net -net hls_pwm_gen_0_strm_pwm_h_a_din [get_bd_pins hls_pwm_gen_0/strm_pwm_h_a_din] [get_bd_pins gate_driver/phase_upper]
  connect_bd_net -net hls_pwm_gen_0_strm_pwm_h_b_din [get_bd_pins hls_pwm_gen_0/strm_pwm_h_b_din] [get_bd_pins gate_driver/phase_upper1]
  connect_bd_net -net hls_pwm_gen_0_strm_pwm_h_c_din [get_bd_pins hls_pwm_gen_0/strm_pwm_h_c_din] [get_bd_pins gate_driver/phase_upper2]
  connect_bd_net -net hls_pwm_gen_0_strm_pwm_l_a_din [get_bd_pins hls_pwm_gen_0/strm_pwm_l_a_din] [get_bd_pins gate_driver/phase_lower]
  connect_bd_net -net hls_pwm_gen_0_strm_pwm_l_b_din [get_bd_pins hls_pwm_gen_0/strm_pwm_l_b_din] [get_bd_pins gate_driver/phase_lower1]
  connect_bd_net -net hls_pwm_gen_0_strm_pwm_l_c_din [get_bd_pins hls_pwm_gen_0/strm_pwm_l_c_din] [get_bd_pins gate_driver/phase_lower2]
  connect_bd_net -net hls_pwm_gen_0_strm_pwm_sync_a_din [get_bd_pins hls_pwm_gen_0/strm_pwm_sync_a_din] [get_bd_pins ADC/motor_pwm]
  connect_bd_net -net hls_qei_0_interrupt [get_bd_pins hls_qei_0/interrupt] [get_bd_pins xlconcat_int/In3]
  connect_bd_net -net hls_svpwm_duty_0_interrupt [get_bd_pins hls_svpwm_duty_0/interrupt] [get_bd_pins xlconcat_int/In5]
  connect_bd_net -net io1_i_0_1 [get_bd_ports CAN_MISO] [get_bd_pins axi_quad_spi_CAN/io1_i]
  connect_bd_net -net motor_control_0_gate_drive_en [get_bd_pins motor_control_0/gate_drive_en] [get_bd_ports gate_drive_en]
  connect_bd_net -net motor_control_0_interrupt [get_bd_pins motor_control_0/interrupt] [get_bd_pins xlconcat_int/In1]
  connect_bd_net -net motor_pa_data_a_1 [get_bd_ports motor_pa_data_v] [get_bd_pins xlconcat_0/In0]
  connect_bd_net -net motor_pa_data_b_1 [get_bd_ports motor_pa_data_i] [get_bd_pins xlconcat_0/In1]
  connect_bd_net -net motor_pb_data_a_1 [get_bd_ports motor_pb_data_v] [get_bd_pins xlconcat_0/In2]
  connect_bd_net -net motor_pb_data_b_1 [get_bd_ports motor_pb_data_i] [get_bd_pins xlconcat_0/In3]
  connect_bd_net -net motor_pc_data_a_1 [get_bd_ports motor_pc_data_v] [get_bd_pins xlconcat_0/In4]
  connect_bd_net -net motor_pc_data_b_1 [get_bd_ports motor_pc_data_i] [get_bd_pins xlconcat_0/In5]
  connect_bd_net -net motor_pwm_1 [get_bd_pins xlconcat_0/dout] [get_bd_pins ADC/SDATA]
  connect_bd_net -net proc_sys_reset_1_peripheral_aresetn [get_bd_pins proc_sys_reset_1/peripheral_aresetn] [get_bd_pins ADC/SCLK_RESETn]
  connect_bd_net -net xlconcat_int_dout [get_bd_pins xlconcat_int/dout] [get_bd_pins PS_0/pl_ps_irq1]
  connect_bd_net -net xlconstant_0_dout [get_bd_pins xlconstant_can_rstn/dout] [get_bd_ports CAN_RST]
  connect_bd_net -net xlslice_0_Dout1 [get_bd_pins xlslice_fan/Dout] [get_bd_ports fan_en_b]
  connect_bd_net -net zynq_ultra_ps_e_0_emio_ttc0_wave_o [get_bd_pins PS_0/emio_ttc0_wave_o] [get_bd_pins xlslice_fan/Din]
  connect_bd_net -net zynq_ultra_ps_e_0_pl_clk0 [get_bd_pins clk_wiz_0/clk_out_100M] [get_bd_pins axi_gpio_0/s_axi_aclk] -boundary_type upper
  connect_bd_net -net zynq_ultra_ps_e_0_pl_clk0 [get_bd_pins clk_wiz_0/clk_out_100M] [get_bd_pins axi_interconnect_cntrl/ACLK] -boundary_type upper
  connect_bd_net -net zynq_ultra_ps_e_0_pl_clk0 [get_bd_pins clk_wiz_0/clk_out_100M] [get_bd_pins axi_interconnect_cntrl/M00_ACLK] -boundary_type upper
  connect_bd_net -net zynq_ultra_ps_e_0_pl_clk0 [get_bd_pins clk_wiz_0/clk_out_100M] [get_bd_pins axi_interconnect_cntrl/M01_ACLK] -boundary_type upper
  connect_bd_net -net zynq_ultra_ps_e_0_pl_clk0 [get_bd_pins clk_wiz_0/clk_out_100M] [get_bd_pins axi_interconnect_cntrl/M02_ACLK] -boundary_type upper
  connect_bd_net -net zynq_ultra_ps_e_0_pl_clk0 [get_bd_pins clk_wiz_0/clk_out_100M] [get_bd_pins axi_interconnect_cntrl/M03_ACLK] -boundary_type upper
  connect_bd_net -net zynq_ultra_ps_e_0_pl_clk0 [get_bd_pins clk_wiz_0/clk_out_100M] [get_bd_pins axi_interconnect_cntrl/M04_ACLK] -boundary_type upper
  connect_bd_net -net zynq_ultra_ps_e_0_pl_clk0 [get_bd_pins clk_wiz_0/clk_out_100M] [get_bd_pins axi_interconnect_cntrl/M05_ACLK] -boundary_type upper
  connect_bd_net -net zynq_ultra_ps_e_0_pl_clk0 [get_bd_pins clk_wiz_0/clk_out_100M] [get_bd_pins axi_interconnect_cntrl/S00_ACLK] -boundary_type upper
  connect_bd_net -net zynq_ultra_ps_e_0_pl_clk0 [get_bd_pins clk_wiz_0/clk_out_100M] [get_bd_pins axi_quad_spi_0/ext_spi_clk] -boundary_type upper
  connect_bd_net -net zynq_ultra_ps_e_0_pl_clk0 [get_bd_pins clk_wiz_0/clk_out_100M] [get_bd_pins axi_quad_spi_0/s_axi_aclk] -boundary_type upper
  connect_bd_net -net zynq_ultra_ps_e_0_pl_clk0 [get_bd_pins clk_wiz_0/clk_out_100M] [get_bd_pins ethernet_subsystem/m_axi_mm2s_aclk] -boundary_type upper
  connect_bd_net -net zynq_ultra_ps_e_0_pl_clk0 [get_bd_pins clk_wiz_0/clk_out_100M] [get_bd_pins gate_driver/sys_clk] -boundary_type upper
  connect_bd_net -net zynq_ultra_ps_e_0_pl_clk0 [get_bd_pins clk_wiz_0/clk_out_100M] [get_bd_pins hls_pwm_gen_0/ap_clk] -boundary_type upper
  connect_bd_net -net zynq_ultra_ps_e_0_pl_clk0 [get_bd_pins clk_wiz_0/clk_out_100M] [get_bd_pins hls_qei_0/ap_clk] -boundary_type upper
  connect_bd_net -net zynq_ultra_ps_e_0_pl_clk0 [get_bd_pins clk_wiz_0/clk_out_100M] [get_bd_pins hls_svpwm_duty_0/ap_clk] -boundary_type upper
  connect_bd_net -net zynq_ultra_ps_e_0_pl_clk0 [get_bd_pins clk_wiz_0/clk_out_100M] [get_bd_pins proc_sys_reset_0/slowest_sync_clk] -boundary_type upper
  connect_bd_net -net zynq_ultra_ps_e_0_pl_clk0 [get_bd_pins clk_wiz_0/clk_out_100M] [get_bd_pins PS_0/maxihpm0_fpd_aclk] -boundary_type upper
  connect_bd_net -net zynq_ultra_ps_e_0_pl_clk0 [get_bd_pins clk_wiz_0/clk_out_100M] [get_bd_pins PS_0/maxihpm0_lpd_aclk] -boundary_type upper
  connect_bd_net -net zynq_ultra_ps_e_0_pl_clk0 [get_bd_pins clk_wiz_0/clk_out_100M] [get_bd_pins PS_0/saxihp0_fpd_aclk] -boundary_type upper
  connect_bd_net -net zynq_ultra_ps_e_0_pl_clk0 [get_bd_pins clk_wiz_0/clk_out_100M] [get_bd_pins hls_foc_periodic_0/ap_clk] -boundary_type upper
  connect_bd_net -net zynq_ultra_ps_e_0_pl_clk0 [get_bd_pins clk_wiz_0/clk_out_100M] [get_bd_pins broadcast_i/aclk] -boundary_type upper
  connect_bd_net -net zynq_ultra_ps_e_0_pl_clk0 [get_bd_pins clk_wiz_0/clk_out_100M] [get_bd_pins motor_control_0/s_axi_cntrl_clk] -boundary_type upper
  connect_bd_net -net zynq_ultra_ps_e_0_pl_clk0 [get_bd_pins clk_wiz_0/clk_out_100M] [get_bd_pins axi_interconnect_cntrl/M06_ACLK] -boundary_type upper
  connect_bd_net -net zynq_ultra_ps_e_0_pl_clk0 [get_bd_pins clk_wiz_0/clk_out_100M] [get_bd_pins axi_interconnect_cntrl/M07_ACLK] -boundary_type upper
  connect_bd_net -net zynq_ultra_ps_e_0_pl_clk0 [get_bd_pins clk_wiz_0/clk_out_100M] [get_bd_pins ADC/CLK] -boundary_type upper
  connect_bd_net -net zynq_ultra_ps_e_0_pl_clk0 [get_bd_pins clk_wiz_0/clk_out_100M] [get_bd_pins axi_quad_spi_CAN/ext_spi_clk] -boundary_type upper
  connect_bd_net -net zynq_ultra_ps_e_0_pl_clk0 [get_bd_pins clk_wiz_0/clk_out_100M] [get_bd_pins axi_quad_spi_CAN/s_axi_aclk] -boundary_type upper
  connect_bd_net -net zynq_ultra_ps_e_0_pl_clk0 [get_bd_pins clk_wiz_0/clk_out_100M] [get_bd_pins axi_interconnect_cntrl/M08_ACLK] -boundary_type upper
  connect_bd_net -net zynq_ultra_ps_e_0_pl_clk1 [get_bd_pins PS_0/pl_clk0] [get_bd_pins clk_wiz_0/clk_in1]
  connect_bd_net -net zynq_ultra_ps_e_0_pl_resetn0 [get_bd_pins proc_sys_reset_0/peripheral_aresetn] [get_bd_pins axi_gpio_0/s_axi_aresetn] -boundary_type upper
  connect_bd_net -net zynq_ultra_ps_e_0_pl_resetn0 [get_bd_pins proc_sys_reset_0/peripheral_aresetn] [get_bd_pins axi_interconnect_cntrl/M00_ARESETN] -boundary_type upper
  connect_bd_net -net zynq_ultra_ps_e_0_pl_resetn0 [get_bd_pins proc_sys_reset_0/peripheral_aresetn] [get_bd_pins axi_interconnect_cntrl/M01_ARESETN] -boundary_type upper
  connect_bd_net -net zynq_ultra_ps_e_0_pl_resetn0 [get_bd_pins proc_sys_reset_0/peripheral_aresetn] [get_bd_pins axi_interconnect_cntrl/M02_ARESETN] -boundary_type upper
  connect_bd_net -net zynq_ultra_ps_e_0_pl_resetn0 [get_bd_pins proc_sys_reset_0/peripheral_aresetn] [get_bd_pins axi_interconnect_cntrl/M03_ARESETN] -boundary_type upper
  connect_bd_net -net zynq_ultra_ps_e_0_pl_resetn0 [get_bd_pins proc_sys_reset_0/peripheral_aresetn] [get_bd_pins axi_interconnect_cntrl/M04_ARESETN] -boundary_type upper
  connect_bd_net -net zynq_ultra_ps_e_0_pl_resetn0 [get_bd_pins proc_sys_reset_0/peripheral_aresetn] [get_bd_pins axi_interconnect_cntrl/M05_ARESETN] -boundary_type upper
  connect_bd_net -net zynq_ultra_ps_e_0_pl_resetn0 [get_bd_pins proc_sys_reset_0/peripheral_aresetn] [get_bd_pins axi_interconnect_cntrl/S00_ARESETN] -boundary_type upper
  connect_bd_net -net zynq_ultra_ps_e_0_pl_resetn0 [get_bd_pins proc_sys_reset_0/peripheral_aresetn] [get_bd_pins axi_quad_spi_0/s_axi_aresetn] -boundary_type upper
  connect_bd_net -net zynq_ultra_ps_e_0_pl_resetn0 [get_bd_pins proc_sys_reset_0/peripheral_aresetn] [get_bd_pins gate_driver/sys_rstn] -boundary_type upper
  connect_bd_net -net zynq_ultra_ps_e_0_pl_resetn0 [get_bd_pins proc_sys_reset_0/peripheral_aresetn] [get_bd_pins hls_pwm_gen_0/ap_rst_n] -boundary_type upper
  connect_bd_net -net zynq_ultra_ps_e_0_pl_resetn0 [get_bd_pins proc_sys_reset_0/peripheral_aresetn] [get_bd_pins hls_qei_0/ap_rst_n] -boundary_type upper
  connect_bd_net -net zynq_ultra_ps_e_0_pl_resetn0 [get_bd_pins proc_sys_reset_0/peripheral_aresetn] [get_bd_pins hls_svpwm_duty_0/ap_rst_n] -boundary_type upper
  connect_bd_net -net zynq_ultra_ps_e_0_pl_resetn0 [get_bd_pins proc_sys_reset_0/peripheral_aresetn] [get_bd_pins hls_foc_periodic_0/ap_rst_n] -boundary_type upper
  connect_bd_net -net zynq_ultra_ps_e_0_pl_resetn0 [get_bd_pins proc_sys_reset_0/peripheral_aresetn] [get_bd_pins broadcast_i/aresetn] -boundary_type upper
  connect_bd_net -net zynq_ultra_ps_e_0_pl_resetn0 [get_bd_pins proc_sys_reset_0/peripheral_aresetn] [get_bd_pins motor_control_0/s_axi_cntrl_resetn] -boundary_type upper
  connect_bd_net -net zynq_ultra_ps_e_0_pl_resetn0 [get_bd_pins proc_sys_reset_0/peripheral_aresetn] [get_bd_pins axi_interconnect_cntrl/M06_ARESETN] -boundary_type upper
  connect_bd_net -net zynq_ultra_ps_e_0_pl_resetn0 [get_bd_pins proc_sys_reset_0/peripheral_aresetn] [get_bd_pins axi_interconnect_cntrl/M07_ARESETN] -boundary_type upper
  connect_bd_net -net zynq_ultra_ps_e_0_pl_resetn0 [get_bd_pins proc_sys_reset_0/peripheral_aresetn] [get_bd_pins ADC/RESETn] -boundary_type upper
  connect_bd_net -net zynq_ultra_ps_e_0_pl_resetn0 [get_bd_pins proc_sys_reset_0/peripheral_aresetn] [get_bd_pins axi_quad_spi_CAN/s_axi_aresetn] -boundary_type upper
  connect_bd_net -net zynq_ultra_ps_e_0_pl_resetn0 [get_bd_pins proc_sys_reset_0/peripheral_aresetn] [get_bd_pins axi_interconnect_cntrl/M08_ARESETN] -boundary_type upper
  connect_bd_net -net zynq_ultra_ps_e_0_pl_resetn1 [get_bd_pins PS_0/pl_resetn0] [get_bd_pins ethernet_subsystem/ext_reset_in] -boundary_type upper
  connect_bd_net -net zynq_ultra_ps_e_0_pl_resetn1 [get_bd_pins PS_0/pl_resetn0] [get_bd_pins proc_sys_reset_0/ext_reset_in] -boundary_type upper
  connect_bd_net -net zynq_ultra_ps_e_0_pl_resetn1 [get_bd_pins PS_0/pl_resetn0] [get_bd_pins proc_sys_reset_1/ext_reset_in] -boundary_type upper
  connect_bd_net -net zynq_ultra_ps_e_0_pl_resetn1 [get_bd_pins PS_0/pl_resetn0] [get_bd_pins clk_wiz_0/resetn] -boundary_type upper

  # Create address segments
  assign_bd_address -offset 0xA0010000 -range 0x00010000 -target_address_space [get_bd_addr_spaces PS_0/Data] [get_bd_addr_segs ADC/adc_hub_phase_dc/S_AXI/Reg] -force
  assign_bd_address -offset 0x80010000 -range 0x00010000 -target_address_space [get_bd_addr_spaces PS_0/Data] [get_bd_addr_segs ethernet_subsystem/axi_dma_gem1/S_AXI_LITE/Reg] -force
  assign_bd_address -offset 0x80000000 -range 0x00010000 -target_address_space [get_bd_addr_spaces PS_0/Data] [get_bd_addr_segs ethernet_subsystem/axi_dma_gem2/S_AXI_LITE/Reg] -force
  assign_bd_address -offset 0x80080000 -range 0x00040000 -target_address_space [get_bd_addr_spaces PS_0/Data] [get_bd_addr_segs ethernet_subsystem/axi_ethernet_gem1/s_axi/Reg0] -force
  assign_bd_address -offset 0x80040000 -range 0x00040000 -target_address_space [get_bd_addr_spaces PS_0/Data] [get_bd_addr_segs ethernet_subsystem/axi_ethernet_gem2/s_axi/Reg0] -force
  assign_bd_address -offset 0xA0070000 -range 0x00010000 -target_address_space [get_bd_addr_spaces PS_0/Data] [get_bd_addr_segs axi_gpio_0/S_AXI/Reg] -force
  assign_bd_address -offset 0xA0060000 -range 0x00010000 -target_address_space [get_bd_addr_spaces PS_0/Data] [get_bd_addr_segs axi_quad_spi_0/AXI_LITE/Reg] -force
  assign_bd_address -offset 0xA0080000 -range 0x00010000 -target_address_space [get_bd_addr_spaces PS_0/Data] [get_bd_addr_segs axi_quad_spi_CAN/AXI_LITE/Reg] -force
  assign_bd_address -offset 0xA0000000 -range 0x00010000 -target_address_space [get_bd_addr_spaces PS_0/Data] [get_bd_addr_segs hls_foc_periodic_0/s_axi_foc_args/Reg] -force
  assign_bd_address -offset 0xA0040000 -range 0x00010000 -target_address_space [get_bd_addr_spaces PS_0/Data] [get_bd_addr_segs hls_pwm_gen_0/s_axi_pwm_args/Reg] -force
  assign_bd_address -offset 0xA0030000 -range 0x00010000 -target_address_space [get_bd_addr_spaces PS_0/Data] [get_bd_addr_segs hls_qei_0/s_axi_qei_args/Reg] -force
  assign_bd_address -offset 0xA0050000 -range 0x00010000 -target_address_space [get_bd_addr_spaces PS_0/Data] [get_bd_addr_segs hls_svpwm_duty_0/s_axi_pwm_args/Reg] -force
  assign_bd_address -offset 0xA0020000 -range 0x00001000 -target_address_space [get_bd_addr_spaces PS_0/Data] [get_bd_addr_segs motor_control_0/s_axi_cntrl/reg0] -force
  assign_bd_address -offset 0x00000000 -range 0x80000000 -target_address_space [get_bd_addr_spaces ethernet_subsystem/axi_dma_gem2/Data_MM2S] [get_bd_addr_segs PS_0/SAXIGP2/HP0_DDR_LOW] -force
  assign_bd_address -offset 0x00000000 -range 0x80000000 -target_address_space [get_bd_addr_spaces ethernet_subsystem/axi_dma_gem2/Data_S2MM] [get_bd_addr_segs PS_0/SAXIGP2/HP0_DDR_LOW] -force
  assign_bd_address -offset 0x00000000 -range 0x80000000 -target_address_space [get_bd_addr_spaces ethernet_subsystem/axi_dma_gem2/Data_SG] [get_bd_addr_segs PS_0/SAXIGP2/HP0_DDR_LOW] -force
  assign_bd_address -offset 0x00000000 -range 0x80000000 -target_address_space [get_bd_addr_spaces ethernet_subsystem/axi_dma_gem1/Data_MM2S] [get_bd_addr_segs PS_0/SAXIGP2/HP0_DDR_LOW] -force
  assign_bd_address -offset 0x00000000 -range 0x80000000 -target_address_space [get_bd_addr_spaces ethernet_subsystem/axi_dma_gem1/Data_S2MM] [get_bd_addr_segs PS_0/SAXIGP2/HP0_DDR_LOW] -force
  assign_bd_address -offset 0x00000000 -range 0x80000000 -target_address_space [get_bd_addr_spaces ethernet_subsystem/axi_dma_gem1/Data_SG] [get_bd_addr_segs PS_0/SAXIGP2/HP0_DDR_LOW] -force


  # Restore current instance
  current_bd_instance $oldCurInst

  # Create PFM attributes
  set_property PFM_NAME {xilinx:kd240_bist:kd240_bist:1.0} [get_files [current_bd_design].bd]
  set_property PFM.AXI_PORT {M_AXI_HPM1_FPD {memport "M_AXI_GP" sptag "" memory "" is_range "false"} S_AXI_HPC0_FPD {memport "S_AXI_HP" sptag "HPC0" memory "PS_0 HPC0_DDR_LOW" is_range "false"} S_AXI_HPC1_FPD {memport "S_AXI_HP" sptag "HPC1" memory "PS_0 HPC1_DDR_LOW" is_range "false"} S_AXI_HP1_FPD {memport "S_AXI_HP" sptag "HP1" memory "PS_0 HP1_DDR_LOW" is_range "false"} S_AXI_HP2_FPD {memport "S_AXI_HP" sptag "HP2" memory "PS_0 HP2_DDR_LOW" is_range "false"} S_AXI_HP3_FPD {memport "S_AXI_HP" sptag "HP3" memory "PS_0 HP3_DDR_LOW" is_range "false"} S_AXI_LPD {memport "MIG" sptag "LPD" memory "PS_0 LPD_DDR_LOW" is_range "false"}} [get_bd_cells /PS_0]
  set_property PFM.CLOCK {clk_out_100M {id "5" is_default "false" proc_sys_reset "/proc_sys_reset_0" status "fixed" freq_hz "99999000"} clk_out_48M {id "4" is_default "true" proc_sys_reset "/proc_sys_reset_1" status "fixed" freq_hz "47999520"}} [get_bd_cells /clk_wiz_0]


  validate_bd_design
  save_bd_design
}
# End of create_root_design()


##################################################################
# MAIN FLOW
##################################################################

create_root_design ""


