# (C) Copyright 2020 - 2021 Xilinx, Inc.
# SPDX-License-Identifier: Apache-2.0


set ::PS_INST PS_0
set PS_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:zynq_ultra_ps_e PS_0 ]


apply_bd_automation -rule xilinx.com:bd_rule:zynq_ultra_ps_e -config {apply_board_preset "1" }  [get_bd_cells PS_0]


set_property -dict [ list \
            CONFIG.PSU__CRF_APB__DPLL_FRAC_CFG__ENABLED  {1} \
CONFIG.PSU__CRF_APB__VPLL_FRAC_CFG__ENABLED  {1} \
CONFIG.PSU__CRL_APB__USB3__ENABLE {1} \
CONFIG.PSU__FPGA_PL0_ENABLE {1} \
CONFIG.PSU__GPIO_EMIO__PERIPHERAL__ENABLE {1} \
CONFIG.PSU__GPIO_EMIO__PERIPHERAL__IO {92} \
CONFIG.PSU__MAXIGP0__DATA_WIDTH {32} \
CONFIG.PSU__MAXIGP1__DATA_WIDTH {32} \
CONFIG.PSU__MAXIGP2__DATA_WIDTH {32} \
CONFIG.PSU__NUM_FABRIC_RESETS {4} \
CONFIG.PSU__SAXIGP0__DATA_WIDTH {128} \
CONFIG.PSU__SAXIGP1__DATA_WIDTH {128} \
CONFIG.PSU__SAXIGP2__DATA_WIDTH {64} \
CONFIG.PSU__SAXIGP3__DATA_WIDTH {128} \
CONFIG.PSU__SAXIGP4__DATA_WIDTH {128} \
CONFIG.PSU__SAXIGP5__DATA_WIDTH {32} \
CONFIG.PSU__SAXIGP6__DATA_WIDTH {32} \
CONFIG.PSU__USE__IRQ0 {1} \
CONFIG.PSU__USE__IRQ1 {1} \
CONFIG.PSU__USE__M_AXI_GP0  {1} \
CONFIG.PSU__USE__M_AXI_GP1  {1} \
CONFIG.PSU__USE__M_AXI_GP2  {1} \
CONFIG.PSU__USE__S_AXI_ACE  {0} \
CONFIG.PSU__USE__S_AXI_ACP  {0} \
CONFIG.PSU__USE__S_AXI_GP0  {1} \
CONFIG.PSU__USE__S_AXI_GP1  {1} \
CONFIG.PSU__USE__S_AXI_GP2  {1} \
CONFIG.PSU__USE__S_AXI_GP3  {1} \
CONFIG.PSU__USE__S_AXI_GP4  {1} \
CONFIG.PSU__USE__S_AXI_GP5  {1} \
CONFIG.PSU__USE__S_AXI_GP6  {1} \
CONFIG.PSU__USE__VIDEO {1} \
] $PS_0



##################################################################
# DESIGN PROCs
##################################################################


# Hierarchical cell: vcu
proc create_hier_cell_vcu { parentCell nameHier } {

  variable script_folder

  if { $parentCell eq "" || $nameHier eq "" } {
     catch {common::send_gid_msg -ssname BD::TCL -id 2092 -severity "ERROR" "create_hier_cell_vcu() - Empty argument(s)!"}
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
  create_bd_intf_pin -mode Master -vlnv xilinx.com:interface:aximm_rtl:1.0 M_AXI_DEC

  create_bd_intf_pin -mode Master -vlnv xilinx.com:interface:aximm_rtl:1.0 M_AXI_MCU

  create_bd_intf_pin -mode Slave -vlnv xilinx.com:interface:aximm_rtl:1.0 S_AXI_LITE


  # Create pins
  create_bd_pin -dir I -type clk ACLK
  create_bd_pin -dir I -type rst ARESETN
  create_bd_pin -dir I -from 91 -to 0 Din
  create_bd_pin -dir I -type rst M00_ARESETN
  create_bd_pin -dir I -type clk pll_ref_clk
  create_bd_pin -dir I -type clk s_axi_lite_aclk
  create_bd_pin -dir O -type intr vcu_host_interrupt

  # Create instance: axi_interconnect_vcu_dec, and set properties
  set axi_interconnect_vcu_dec [ create_bd_cell -type ip -vlnv xilinx.com:ip:axi_interconnect axi_interconnect_vcu_dec ]
  set_property -dict [ list \
   CONFIG.NUM_MI {1} \
   CONFIG.NUM_SI {2} \
 ] $axi_interconnect_vcu_dec

  # Create instance: axi_reg_slice_vdec, and set properties
  set axi_reg_slice_vdec [ create_bd_cell -type ip -vlnv xilinx.com:ip:axi_register_slice axi_reg_slice_vdec ]

  # Create instance: axi_reg_slice_vmcu, and set properties
  set axi_reg_slice_vmcu [ create_bd_cell -type ip -vlnv xilinx.com:ip:axi_register_slice axi_reg_slice_vmcu ]

  # Create instance: vcu_0, and set properties
  set vcu_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:vcu vcu_0 ]
  set_property -dict [ list \
   CONFIG.DEC_CODING_TYPE {0} \
   CONFIG.DEC_COLOR_DEPTH {0} \
   CONFIG.DEC_COLOR_FORMAT {0} \
   CONFIG.DEC_FPS {1} \
   CONFIG.DEC_FRAME_SIZE {3} \
   CONFIG.ENABLE_DECODER {true} \
   CONFIG.ENABLE_ENCODER {false} \
   CONFIG.ENC_BUFFER_EN {false} \
   CONFIG.ENC_BUFFER_MANUAL_OVERRIDE {0} \
   CONFIG.ENC_BUFFER_SIZE {0} \
   CONFIG.ENC_BUFFER_SIZE_ACTUAL {0} \
   CONFIG.ENC_BUFFER_TYPE {0} \
   CONFIG.ENC_CODING_TYPE {0} \
   CONFIG.ENC_COLOR_DEPTH {1} \
   CONFIG.ENC_COLOR_FORMAT {1} \
   CONFIG.ENC_FPS {3} \
   CONFIG.ENC_FRAME_SIZE {4} \
   CONFIG.ENC_MEM_BRAM_USED {0} \
   CONFIG.ENC_MEM_URAM_USED {0} \
   CONFIG.NO_OF_DEC_STREAMS {4} \
   CONFIG.NO_OF_STREAMS {1} \
   CONFIG.TABLE_NO {0} \
 ] $vcu_0

  # Create instance: xlslice_1, and set properties
  set xlslice_1 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlslice xlslice_1 ]
  set_property -dict [ list \
   CONFIG.DIN_FROM {1} \
   CONFIG.DIN_TO {1} \
   CONFIG.DIN_WIDTH {92} \
   CONFIG.DOUT_WIDTH {1} \
 ] $xlslice_1

  # Create interface connections
  connect_bd_intf_net -intf_net S00_AXI_1 [get_bd_intf_pins axi_interconnect_vcu_dec/S00_AXI] [get_bd_intf_pins vcu_0/M_AXI_DEC0]
  connect_bd_intf_net -intf_net S01_AXI_1 [get_bd_intf_pins axi_interconnect_vcu_dec/S01_AXI] [get_bd_intf_pins vcu_0/M_AXI_DEC1]
  connect_bd_intf_net -intf_net axi_interconnect_ctrl_M00_AXI [get_bd_intf_pins S_AXI_LITE] [get_bd_intf_pins vcu_0/S_AXI_LITE]
  connect_bd_intf_net -intf_net axi_interconnect_vcu_dec_M00_AXI [get_bd_intf_pins axi_interconnect_vcu_dec/M00_AXI] [get_bd_intf_pins axi_reg_slice_vdec/S_AXI]
  connect_bd_intf_net -intf_net axi_register_slice_0_M_AXI [get_bd_intf_pins M_AXI_DEC] [get_bd_intf_pins axi_reg_slice_vdec/M_AXI]
  connect_bd_intf_net -intf_net axi_register_slice_2_M_AXI [get_bd_intf_pins M_AXI_MCU] [get_bd_intf_pins axi_reg_slice_vmcu/M_AXI]
  connect_bd_intf_net -intf_net vcu_0_M_AXI_MCU [get_bd_intf_pins axi_reg_slice_vmcu/S_AXI] [get_bd_intf_pins vcu_0/M_AXI_MCU]

  # Create port connections
  connect_bd_net -net Din_1 [get_bd_pins Din] [get_bd_pins xlslice_1/Din]
  connect_bd_net -net clk_wiz_0_clk_100M [get_bd_pins s_axi_lite_aclk] [get_bd_pins vcu_0/s_axi_lite_aclk]
  connect_bd_net -net clk_wiz_0_clk_250M [get_bd_pins ACLK] [get_bd_pins axi_interconnect_vcu_dec/ACLK] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_250M [get_bd_pins ACLK] [get_bd_pins axi_interconnect_vcu_dec/M00_ACLK] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_250M [get_bd_pins ACLK] [get_bd_pins axi_interconnect_vcu_dec/S00_ACLK] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_250M [get_bd_pins ACLK] [get_bd_pins axi_interconnect_vcu_dec/S01_ACLK] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_250M [get_bd_pins ACLK] [get_bd_pins axi_reg_slice_vdec/aclk] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_250M [get_bd_pins ACLK] [get_bd_pins axi_reg_slice_vmcu/aclk] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_250M [get_bd_pins ACLK] [get_bd_pins vcu_0/m_axi_dec_aclk] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_250M [get_bd_pins ACLK] [get_bd_pins vcu_0/m_axi_mcu_aclk] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_50M [get_bd_pins pll_ref_clk] [get_bd_pins vcu_0/pll_ref_clk]
  connect_bd_net -net proc_sys_reset_1_interconnect_aresetn [get_bd_pins ARESETN] [get_bd_pins axi_interconnect_vcu_dec/ARESETN]
  connect_bd_net -net proc_sys_reset_1_peripheral_aresetn [get_bd_pins M00_ARESETN] [get_bd_pins axi_interconnect_vcu_dec/M00_ARESETN] -boundary_type upper
  connect_bd_net -net proc_sys_reset_1_peripheral_aresetn [get_bd_pins M00_ARESETN] [get_bd_pins axi_interconnect_vcu_dec/S00_ARESETN] -boundary_type upper
  connect_bd_net -net proc_sys_reset_1_peripheral_aresetn [get_bd_pins M00_ARESETN] [get_bd_pins axi_interconnect_vcu_dec/S01_ARESETN] -boundary_type upper
  connect_bd_net -net proc_sys_reset_1_peripheral_aresetn [get_bd_pins M00_ARESETN] [get_bd_pins axi_reg_slice_vdec/aresetn] -boundary_type upper
  connect_bd_net -net proc_sys_reset_1_peripheral_aresetn [get_bd_pins M00_ARESETN] [get_bd_pins axi_reg_slice_vmcu/aresetn] -boundary_type upper
  connect_bd_net -net vcu_0_vcu_host_interrupt [get_bd_pins vcu_host_interrupt] [get_bd_pins vcu_0/vcu_host_interrupt]
  connect_bd_net -net xlslice_0_Dout [get_bd_pins vcu_0/vcu_resetn] [get_bd_pins xlslice_1/Dout]

  # Restore current instance
  current_bd_instance $oldCurInst
}

# Hierarchical cell: display_pipeline
proc create_hier_cell_display_pipeline { parentCell nameHier } {

  variable script_folder

  if { $parentCell eq "" || $nameHier eq "" } {
     catch {common::send_gid_msg -ssname BD::TCL -id 2092 -severity "ERROR" "create_hier_cell_display_pipeline() - Empty argument(s)!"}
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

  create_bd_intf_pin -mode Slave -vlnv xilinx.com:interface:aximm_rtl:1.0 ctrl

  create_bd_intf_pin -mode Slave -vlnv xilinx.com:interface:aximm_rtl:1.0 s_axi_CTRL

  create_bd_intf_pin -mode Slave -vlnv xilinx.com:interface:aximm_rtl:1.0 s_axi_lite_0


  # Create pins
  create_bd_pin -dir I -from 91 -to 0 Din
  create_bd_pin -dir I -type clk clk
  create_bd_pin -dir I -type rst ext_reset_in
  create_bd_pin -dir O -type intr interrupt
  create_bd_pin -dir O -type intr irq
  create_bd_pin -dir I -type rst resetn
  create_bd_pin -dir I -type clk s_axi_aclk
  create_bd_pin -dir I -type rst s_axi_aresetn
  create_bd_pin -dir O vid_active_video
  create_bd_pin -dir O -from 35 -to 0 vid_data
  create_bd_pin -dir O vid_hsync
  create_bd_pin -dir O vid_vsync
  create_bd_pin -dir O video_clk

  # Create instance: axi_interconnect_0, and set properties
  set axi_interconnect_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:axi_interconnect axi_interconnect_0 ]
  set_property -dict [ list \
   CONFIG.NUM_MI {1} \
   CONFIG.NUM_SI {5} \
 ] $axi_interconnect_0

  # Create instance: axis_subset_converter_0, and set properties
  set axis_subset_converter_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:axis_subset_converter axis_subset_converter_0 ]
  set_property -dict [ list \
   CONFIG.M_TDATA_NUM_BYTES {2} \
   CONFIG.S_TDATA_NUM_BYTES {3} \
   CONFIG.TDATA_REMAP {tdata[15:0]} \
 ] $axis_subset_converter_0

  # Create instance: clk_wiz_0, and set properties
  set clk_wiz_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:clk_wiz clk_wiz_0 ]
  set_property -dict [ list \
   CONFIG.AUTO_PRIMITIVE {MMCM} \
   CONFIG.CLKOUT1_DRIVES {Buffer} \
   CONFIG.CLKOUT1_JITTER {160.489} \
   CONFIG.CLKOUT1_PHASE_ERROR {282.343} \
   CONFIG.CLKOUT1_REQUESTED_OUT_FREQ {262.75} \
   CONFIG.CLKOUT2_DRIVES {Buffer} \
   CONFIG.CLKOUT3_DRIVES {Buffer} \
   CONFIG.CLKOUT4_DRIVES {Buffer} \
   CONFIG.CLKOUT5_DRIVES {Buffer} \
   CONFIG.CLKOUT6_DRIVES {Buffer} \
   CONFIG.CLKOUT7_DRIVES {Buffer} \
   CONFIG.FEEDBACK_SOURCE {FDBK_AUTO} \
   CONFIG.MMCM_BANDWIDTH {OPTIMIZED} \
   CONFIG.MMCM_CLKFBOUT_MULT_F {59.000} \
   CONFIG.MMCM_CLKOUT0_DIVIDE_F {4.750} \
   CONFIG.MMCM_COMPENSATION {AUTO} \
   CONFIG.MMCM_DIVCLK_DIVIDE {13} \
   CONFIG.PRIMITIVE {MMCM} \
   CONFIG.PRIM_SOURCE {Global_buffer} \
   CONFIG.USE_DYN_RECONFIG {true} \
   CONFIG.USE_LOCKED {true} \
   CONFIG.USE_RESET {true} \
 ] $clk_wiz_0

  # Create instance: proc_sys_reset_0, and set properties
  set proc_sys_reset_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:proc_sys_reset proc_sys_reset_0 ]

  # Create instance: v_axi4s_vid_out_0, and set properties
  set v_axi4s_vid_out_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:v_axi4s_vid_out v_axi4s_vid_out_0 ]
  set_property -dict [ list \
   CONFIG.C_HAS_ASYNC_CLK {1} \
   CONFIG.C_NATIVE_COMPONENT_WIDTH {8} \
   CONFIG.C_S_AXIS_VIDEO_FORMAT {0} \
   CONFIG.C_VTG_MASTER_SLAVE {1} \
 ] $v_axi4s_vid_out_0

  # Create instance: v_mix_0, and set properties
  set v_mix_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:v_mix v_mix_0 ]
  set_property -dict [ list \
   CONFIG.LAYER1_VIDEO_FORMAT {19} \
   CONFIG.LAYER2_VIDEO_FORMAT {19} \
   CONFIG.LAYER3_VIDEO_FORMAT {19} \
   CONFIG.LAYER4_VIDEO_FORMAT {19} \
   CONFIG.LAYER5_ALPHA {true} \
   CONFIG.LAYER5_VIDEO_FORMAT {26} \
   CONFIG.MAX_COLS {3840} \
   CONFIG.MAX_ROWS {2160} \
   CONFIG.NR_LAYERS {6} \
   CONFIG.VIDEO_FORMAT {2} \
 ] $v_mix_0

  # Create instance: v_tc_0, and set properties
  set v_tc_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:v_tc v_tc_0 ]
  set_property -dict [ list \
   CONFIG.GEN_F0_VBLANK_HEND {960} \
   CONFIG.GEN_F0_VBLANK_HSTART {960} \
   CONFIG.GEN_F0_VFRAME_SIZE {1125} \
   CONFIG.GEN_F0_VSYNC_HEND {1004} \
   CONFIG.GEN_F0_VSYNC_HSTART {1004} \
   CONFIG.GEN_F0_VSYNC_VEND {1088} \
   CONFIG.GEN_F0_VSYNC_VSTART {1083} \
   CONFIG.GEN_F1_VBLANK_HEND {960} \
   CONFIG.GEN_F1_VBLANK_HSTART {960} \
   CONFIG.GEN_F1_VFRAME_SIZE {1125} \
   CONFIG.GEN_F1_VSYNC_HEND {1004} \
   CONFIG.GEN_F1_VSYNC_HSTART {1004} \
   CONFIG.GEN_F1_VSYNC_VEND {1088} \
   CONFIG.GEN_F1_VSYNC_VSTART {1083} \
   CONFIG.GEN_HACTIVE_SIZE {1920} \
   CONFIG.GEN_HFRAME_SIZE {2200} \
   CONFIG.GEN_HSYNC_END {2052} \
   CONFIG.GEN_HSYNC_START {2008} \
   CONFIG.GEN_VACTIVE_SIZE {1080} \
   CONFIG.VIDEO_MODE {1080p} \
   CONFIG.enable_detection {false} \
   CONFIG.max_clocks_per_line {8192} \
 ] $v_tc_0

  # Create instance: xlconcat_0, and set properties
  set xlconcat_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlconcat xlconcat_0 ]
  set_property -dict [ list \
   CONFIG.IN0_WIDTH {16} \
   CONFIG.IN1_WIDTH {8} \
   CONFIG.IN2_WIDTH {4} \
   CONFIG.IN3_WIDTH {8} \
   CONFIG.NUM_PORTS {4} \
 ] $xlconcat_0

  # Create instance: xlconstant_0, and set properties
  set xlconstant_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlconstant xlconstant_0 ]
  set_property -dict [ list \
   CONFIG.CONST_VAL {0} \
   CONFIG.CONST_WIDTH {24} \
 ] $xlconstant_0

  # Create instance: xlconstant_1, and set properties
  set xlconstant_1 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlconstant xlconstant_1 ]

  # Create instance: xlconstant_2, and set properties
  set xlconstant_2 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlconstant xlconstant_2 ]
  set_property -dict [ list \
   CONFIG.CONST_VAL {0} \
   CONFIG.CONST_WIDTH {16} \
 ] $xlconstant_2

  # Create instance: xlslice_0, and set properties
  set xlslice_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlslice xlslice_0 ]
  set_property -dict [ list \
   CONFIG.DIN_WIDTH {92} \
 ] $xlslice_0

  # Create instance: xlslice_15to8, and set properties
  set xlslice_15to8 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlslice xlslice_15to8 ]
  set_property -dict [ list \
   CONFIG.DIN_FROM {15} \
   CONFIG.DIN_TO {8} \
   CONFIG.DIN_WIDTH {16} \
   CONFIG.DOUT_WIDTH {8} \
 ] $xlslice_15to8

  # Create instance: xlslice_7to0, and set properties
  set xlslice_7to0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlslice xlslice_7to0 ]
  set_property -dict [ list \
   CONFIG.DIN_FROM {7} \
   CONFIG.DIN_TO {0} \
   CONFIG.DIN_WIDTH {16} \
   CONFIG.DOUT_WIDTH {8} \
 ] $xlslice_7to0

  # Create interface connections
  connect_bd_intf_net -intf_net Conn1 [get_bd_intf_pins ctrl] [get_bd_intf_pins v_tc_0/ctrl]
  connect_bd_intf_net -intf_net Conn2 [get_bd_intf_pins s_axi_CTRL] [get_bd_intf_pins v_mix_0/s_axi_CTRL]
  connect_bd_intf_net -intf_net Conn3 [get_bd_intf_pins M00_AXI] [get_bd_intf_pins axi_interconnect_0/M00_AXI]
  connect_bd_intf_net -intf_net Conn4 [get_bd_intf_pins s_axi_lite_0] [get_bd_intf_pins clk_wiz_0/s_axi_lite]
  connect_bd_intf_net -intf_net S00_AXI_1 [get_bd_intf_pins axi_interconnect_0/S00_AXI] [get_bd_intf_pins v_mix_0/m_axi_mm_video1]
  connect_bd_intf_net -intf_net S01_AXI_1 [get_bd_intf_pins axi_interconnect_0/S01_AXI] [get_bd_intf_pins v_mix_0/m_axi_mm_video2]
  connect_bd_intf_net -intf_net S02_AXI_1 [get_bd_intf_pins axi_interconnect_0/S02_AXI] [get_bd_intf_pins v_mix_0/m_axi_mm_video3]
  connect_bd_intf_net -intf_net S03_AXI_1 [get_bd_intf_pins axi_interconnect_0/S03_AXI] [get_bd_intf_pins v_mix_0/m_axi_mm_video4]
  connect_bd_intf_net -intf_net S04_AXI_1 [get_bd_intf_pins axi_interconnect_0/S04_AXI] [get_bd_intf_pins v_mix_0/m_axi_mm_video5]
  connect_bd_intf_net -intf_net axis_subset_converter_0_M_AXIS [get_bd_intf_pins axis_subset_converter_0/M_AXIS] [get_bd_intf_pins v_axi4s_vid_out_0/video_in]
  connect_bd_intf_net -intf_net v_mix_0_m_axis_video [get_bd_intf_pins axis_subset_converter_0/S_AXIS] [get_bd_intf_pins v_mix_0/m_axis_video]
  connect_bd_intf_net -intf_net v_tc_0_vtiming_out [get_bd_intf_pins v_axi4s_vid_out_0/vtiming_in] [get_bd_intf_pins v_tc_0/vtiming_out]

  # Create port connections
  connect_bd_net -net Din_1 [get_bd_pins Din] [get_bd_pins xlslice_0/Din]
  connect_bd_net -net clk_1 [get_bd_pins clk] [get_bd_pins axi_interconnect_0/ACLK] -boundary_type upper
  connect_bd_net -net clk_1 [get_bd_pins clk] [get_bd_pins axi_interconnect_0/M00_ACLK] -boundary_type upper
  connect_bd_net -net clk_1 [get_bd_pins clk] [get_bd_pins axi_interconnect_0/S00_ACLK] -boundary_type upper
  connect_bd_net -net clk_1 [get_bd_pins clk] [get_bd_pins axi_interconnect_0/S01_ACLK] -boundary_type upper
  connect_bd_net -net clk_1 [get_bd_pins clk] [get_bd_pins axi_interconnect_0/S02_ACLK] -boundary_type upper
  connect_bd_net -net clk_1 [get_bd_pins clk] [get_bd_pins axi_interconnect_0/S03_ACLK] -boundary_type upper
  connect_bd_net -net clk_1 [get_bd_pins clk] [get_bd_pins axi_interconnect_0/S04_ACLK] -boundary_type upper
  connect_bd_net -net clk_1 [get_bd_pins clk] [get_bd_pins axis_subset_converter_0/aclk] -boundary_type upper
  connect_bd_net -net clk_1 [get_bd_pins clk] [get_bd_pins clk_wiz_0/clk_in1] -boundary_type upper
  connect_bd_net -net clk_1 [get_bd_pins clk] [get_bd_pins v_axi4s_vid_out_0/aclk] -boundary_type upper
  connect_bd_net -net clk_1 [get_bd_pins clk] [get_bd_pins v_mix_0/ap_clk] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_out1 [get_bd_pins video_clk] [get_bd_pins clk_wiz_0/clk_out1] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_out1 [get_bd_pins video_clk] [get_bd_pins proc_sys_reset_0/slowest_sync_clk] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_out1 [get_bd_pins video_clk] [get_bd_pins v_axi4s_vid_out_0/vid_io_out_clk] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_out1 [get_bd_pins video_clk] [get_bd_pins v_tc_0/clk] -boundary_type upper
  connect_bd_net -net clk_wiz_0_locked [get_bd_pins clk_wiz_0/locked] [get_bd_pins proc_sys_reset_0/dcm_locked]
  connect_bd_net -net ext_reset_in_1 [get_bd_pins ext_reset_in] [get_bd_pins proc_sys_reset_0/ext_reset_in]
  connect_bd_net -net proc_sys_reset_0_peripheral_aresetn [get_bd_pins proc_sys_reset_0/peripheral_aresetn] [get_bd_pins v_tc_0/resetn]
  connect_bd_net -net proc_sys_reset_0_peripheral_reset [get_bd_pins proc_sys_reset_0/peripheral_reset] [get_bd_pins v_axi4s_vid_out_0/vid_io_out_reset]
  connect_bd_net -net resetn_1 [get_bd_pins resetn] [get_bd_pins axi_interconnect_0/ARESETN] -boundary_type upper
  connect_bd_net -net resetn_1 [get_bd_pins resetn] [get_bd_pins axi_interconnect_0/M00_ARESETN] -boundary_type upper
  connect_bd_net -net resetn_1 [get_bd_pins resetn] [get_bd_pins axi_interconnect_0/S00_ARESETN] -boundary_type upper
  connect_bd_net -net resetn_1 [get_bd_pins resetn] [get_bd_pins axi_interconnect_0/S01_ARESETN] -boundary_type upper
  connect_bd_net -net resetn_1 [get_bd_pins resetn] [get_bd_pins axi_interconnect_0/S02_ARESETN] -boundary_type upper
  connect_bd_net -net resetn_1 [get_bd_pins resetn] [get_bd_pins axi_interconnect_0/S03_ARESETN] -boundary_type upper
  connect_bd_net -net resetn_1 [get_bd_pins resetn] [get_bd_pins axi_interconnect_0/S04_ARESETN] -boundary_type upper
  connect_bd_net -net resetn_1 [get_bd_pins resetn] [get_bd_pins axis_subset_converter_0/aresetn] -boundary_type upper
  connect_bd_net -net resetn_1 [get_bd_pins resetn] [get_bd_pins v_axi4s_vid_out_0/aresetn] -boundary_type upper
  connect_bd_net -net s_axi_aclk_1 [get_bd_pins s_axi_aclk] [get_bd_pins clk_wiz_0/s_axi_aclk] -boundary_type upper
  connect_bd_net -net s_axi_aclk_1 [get_bd_pins s_axi_aclk] [get_bd_pins v_tc_0/s_axi_aclk] -boundary_type upper
  connect_bd_net -net s_axi_aresetn_1 [get_bd_pins s_axi_aresetn] [get_bd_pins clk_wiz_0/s_axi_aresetn] -boundary_type upper
  connect_bd_net -net s_axi_aresetn_1 [get_bd_pins s_axi_aresetn] [get_bd_pins v_tc_0/s_axi_aresetn] -boundary_type upper
  connect_bd_net -net v_axi4s_vid_out_0_sof_state_out [get_bd_pins v_axi4s_vid_out_0/sof_state_out] [get_bd_pins v_tc_0/sof_state]
  connect_bd_net -net v_axi4s_vid_out_0_vid_active_video [get_bd_pins vid_active_video] [get_bd_pins v_axi4s_vid_out_0/vid_active_video]
  connect_bd_net -net v_axi4s_vid_out_0_vid_data [get_bd_pins v_axi4s_vid_out_0/vid_data] [get_bd_pins xlslice_15to8/Din] -boundary_type upper
  connect_bd_net -net v_axi4s_vid_out_0_vid_data [get_bd_pins v_axi4s_vid_out_0/vid_data] [get_bd_pins xlslice_7to0/Din] -boundary_type upper
  connect_bd_net -net v_axi4s_vid_out_0_vid_hsync [get_bd_pins vid_hsync] [get_bd_pins v_axi4s_vid_out_0/vid_hsync]
  connect_bd_net -net v_axi4s_vid_out_0_vid_vsync [get_bd_pins vid_vsync] [get_bd_pins v_axi4s_vid_out_0/vid_vsync]
  connect_bd_net -net v_axi4s_vid_out_0_vtg_ce [get_bd_pins v_axi4s_vid_out_0/vtg_ce] [get_bd_pins v_tc_0/gen_clken]
  connect_bd_net -net v_mix_0_interrupt [get_bd_pins interrupt] [get_bd_pins v_mix_0/interrupt]
  connect_bd_net -net v_tc_0_irq [get_bd_pins irq] [get_bd_pins v_tc_0/irq]
  connect_bd_net -net xlconcat_0_dout [get_bd_pins vid_data] [get_bd_pins xlconcat_0/dout]
  connect_bd_net -net xlconstant_0_dout [get_bd_pins v_mix_0/s_axis_video_TDATA] [get_bd_pins v_mix_0/s_axis_video_TVALID] -boundary_type upper
  connect_bd_net -net xlconstant_0_dout [get_bd_pins v_mix_0/s_axis_video_TDATA] [get_bd_pins xlconstant_0/dout] -boundary_type upper
  connect_bd_net -net xlconstant_1_dout [get_bd_pins v_axi4s_vid_out_0/aclken] [get_bd_pins v_axi4s_vid_out_0/vid_io_out_ce] -boundary_type upper
  connect_bd_net -net xlconstant_1_dout [get_bd_pins v_axi4s_vid_out_0/aclken] [get_bd_pins v_tc_0/clken] -boundary_type upper
  connect_bd_net -net xlconstant_1_dout [get_bd_pins v_axi4s_vid_out_0/aclken] [get_bd_pins v_tc_0/s_axi_aclken] -boundary_type upper
  connect_bd_net -net xlconstant_1_dout [get_bd_pins v_axi4s_vid_out_0/aclken] [get_bd_pins xlconstant_1/dout] -boundary_type upper
  connect_bd_net -net xlconstant_2_dout [get_bd_pins xlconcat_0/In0] [get_bd_pins xlconcat_0/In2] -boundary_type upper
  connect_bd_net -net xlconstant_2_dout [get_bd_pins xlconcat_0/In0] [get_bd_pins xlconstant_2/dout] -boundary_type upper
  connect_bd_net -net xlslice_0_Dout [get_bd_pins v_mix_0/ap_rst_n] [get_bd_pins xlslice_0/Dout]
  connect_bd_net -net xlslice_15to8_Dout [get_bd_pins xlconcat_0/In3] [get_bd_pins xlslice_15to8/Dout]
  connect_bd_net -net xlslice_7to0_Dout [get_bd_pins xlconcat_0/In1] [get_bd_pins xlslice_7to0/Dout]

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

  # Create ports
  set fan_en_b [ create_bd_port -dir O -from 0 -to 0 fan_en_b ]


  # Create instance: axi_ic_accel_ctrl, and set properties
  set axi_ic_accel_ctrl [ create_bd_cell -type ip -vlnv xilinx.com:ip:axi_interconnect axi_ic_accel_ctrl ]
  set_property -dict [ list \
   CONFIG.NUM_MI {1} \
 ] $axi_ic_accel_ctrl

  # Create instance: axi_ic_ctrl_0, and set properties
  set axi_ic_ctrl_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:axi_interconnect axi_ic_ctrl_0 ]
  set_property -dict [ list \
   CONFIG.NUM_MI {3} \
 ] $axi_ic_ctrl_0

  # Create instance: axi_ic_ctrl_1, and set properties
  set axi_ic_ctrl_1 [ create_bd_cell -type ip -vlnv xilinx.com:ip:axi_interconnect axi_ic_ctrl_1 ]
  set_property -dict [ list \
   CONFIG.NUM_MI {1} \
 ] $axi_ic_ctrl_1

  # Create instance: axi_vip_0, and set properties
  set axi_vip_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:axi_vip axi_vip_0 ]
  set_property -dict [ list \
   CONFIG.INTERFACE_MODE {SLAVE} \
 ] $axi_vip_0

  # Create instance: clk_wiz_0, and set properties
  set clk_wiz_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:clk_wiz clk_wiz_0 ]
  set_property -dict [ list \
   CONFIG.CLKOUT1_JITTER {108.849} \
   CONFIG.CLKOUT1_PHASE_ERROR {92.673} \
   CONFIG.CLKOUT1_REQUESTED_OUT_FREQ {200.000} \
   CONFIG.CLKOUT2_JITTER {123.671} \
   CONFIG.CLKOUT2_PHASE_ERROR {92.673} \
   CONFIG.CLKOUT2_USED {true} \
   CONFIG.CLKOUT3_JITTER {102.699} \
   CONFIG.CLKOUT3_PHASE_ERROR {92.673} \
   CONFIG.CLKOUT3_REQUESTED_OUT_FREQ {275.000} \
   CONFIG.CLKOUT3_USED {true} \
   CONFIG.CLKOUT4_JITTER {142.810} \
   CONFIG.CLKOUT4_PHASE_ERROR {92.673} \
   CONFIG.CLKOUT4_REQUESTED_OUT_FREQ {50.000} \
   CONFIG.CLKOUT4_USED {true} \
   CONFIG.CLKOUT5_JITTER {90.570} \
   CONFIG.CLKOUT5_PHASE_ERROR {92.673} \
   CONFIG.CLKOUT5_REQUESTED_OUT_FREQ {550.000} \
   CONFIG.CLKOUT5_USED {true} \
   CONFIG.CLK_OUT1_PORT {clk_200M} \
   CONFIG.CLK_OUT2_PORT {clk_100M} \
   CONFIG.CLK_OUT3_PORT {clk_275M} \
   CONFIG.CLK_OUT4_PORT {clk_50M} \
   CONFIG.CLK_OUT5_PORT {clk_550M} \
   CONFIG.MMCM_CLKFBOUT_MULT_F {11.000} \
   CONFIG.MMCM_CLKOUT0_DIVIDE_F {5.500} \
   CONFIG.MMCM_CLKOUT1_DIVIDE {11} \
   CONFIG.MMCM_CLKOUT2_DIVIDE {4} \
   CONFIG.MMCM_CLKOUT3_DIVIDE {22} \
   CONFIG.MMCM_CLKOUT4_DIVIDE {2} \
   CONFIG.MMCM_DIVCLK_DIVIDE {1} \
   CONFIG.NUM_OUT_CLKS {5} \
   CONFIG.PRIM_SOURCE {Global_buffer} \
   CONFIG.RESET_PORT {resetn} \
   CONFIG.RESET_TYPE {ACTIVE_LOW} \
 ] $clk_wiz_0

  # Create instance: display_pipeline
  create_hier_cell_display_pipeline [current_bd_instance .] display_pipeline

  # Create instance: proc_sys_reset_100MHz, and set properties
  set proc_sys_reset_100MHz [ create_bd_cell -type ip -vlnv xilinx.com:ip:proc_sys_reset proc_sys_reset_100MHz ]

  # Create instance: proc_sys_reset_275MHz, and set properties
  set proc_sys_reset_275MHz [ create_bd_cell -type ip -vlnv xilinx.com:ip:proc_sys_reset proc_sys_reset_275MHz ]

  # Create instance: proc_sys_reset_550MHz, and set properties
  set proc_sys_reset_550MHz [ create_bd_cell -type ip -vlnv xilinx.com:ip:proc_sys_reset proc_sys_reset_550MHz ]

  # Create instance: vcu
  create_hier_cell_vcu [current_bd_instance .] vcu

  # Create instance: xlconcat_0, and set properties
  set xlconcat_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlconcat xlconcat_0 ]
  set_property -dict [ list \
   CONFIG.NUM_PORTS {3} \
 ] $xlconcat_0

  # Create instance: xlconstant_0_fan_en_b, and set properties
  set xlconstant_0_fan_en_b [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlconstant xlconstant_0_fan_en_b ]
  set_property -dict [ list \
   CONFIG.CONST_VAL {0} \
 ] $xlconstant_0_fan_en_b

  # Create interface connections
  connect_bd_intf_net -intf_net ${::PS_INST}_M_AXI_HPM0_FPD [get_bd_intf_pins ${::PS_INST}/M_AXI_HPM0_FPD] [get_bd_intf_pins axi_ic_accel_ctrl/S00_AXI]
  connect_bd_intf_net -intf_net ${::PS_INST}_M_AXI_HPM1_FPD [get_bd_intf_pins ${::PS_INST}/M_AXI_HPM1_FPD] [get_bd_intf_pins axi_ic_ctrl_1/S00_AXI]
  connect_bd_intf_net -intf_net axi_ic_accel_ctrl_M00_AXI [get_bd_intf_pins axi_ic_accel_ctrl/M00_AXI] [get_bd_intf_pins axi_vip_0/S_AXI]
  connect_bd_intf_net -intf_net axi_ic_ctrl_0_M02_AXI [get_bd_intf_pins axi_ic_ctrl_0/M02_AXI] [get_bd_intf_pins display_pipeline/s_axi_lite_0]
  connect_bd_intf_net -intf_net axi_ic_ctrl_1_M00_AXI [get_bd_intf_pins axi_ic_ctrl_1/M00_AXI] [get_bd_intf_pins display_pipeline/s_axi_CTRL]
  connect_bd_intf_net -intf_net axi_interconnect_ctrl_M00_AXI [get_bd_intf_pins axi_ic_ctrl_0/M00_AXI] [get_bd_intf_pins vcu/S_AXI_LITE]
  connect_bd_intf_net -intf_net axi_interconnect_ctrl_M01_AXI [get_bd_intf_pins axi_ic_ctrl_0/M01_AXI] [get_bd_intf_pins display_pipeline/ctrl]
  connect_bd_intf_net -intf_net axi_interconnect_vcu1_M00_AXI [get_bd_intf_pins ${::PS_INST}/S_AXI_HPC1_FPD] [get_bd_intf_pins vcu/M_AXI_DEC]
  connect_bd_intf_net -intf_net display_pipeline_M00_AXI [get_bd_intf_pins ${::PS_INST}/S_AXI_HP0_FPD] [get_bd_intf_pins display_pipeline/M00_AXI]
  connect_bd_intf_net -intf_net vcu_0_M_AXI_MCU [get_bd_intf_pins ${::PS_INST}/S_AXI_HP3_FPD] [get_bd_intf_pins vcu/M_AXI_MCU]
  connect_bd_intf_net -intf_net zynq_ultra_ps_e_0_M_AXI_HPM0_LPD [get_bd_intf_pins ${::PS_INST}/M_AXI_HPM0_LPD] [get_bd_intf_pins axi_ic_ctrl_0/S00_AXI]

  # Create port connections
  connect_bd_net -net ARESETN_1 [get_bd_pins axi_ic_ctrl_0/ARESETN] [get_bd_pins proc_sys_reset_100MHz/interconnect_aresetn]
  connect_bd_net -net ${::PS_INST}_emio_gpio_o [get_bd_pins ${::PS_INST}/emio_gpio_o] [get_bd_pins display_pipeline/Din] -boundary_type upper
  connect_bd_net -net ${::PS_INST}_emio_gpio_o [get_bd_pins ${::PS_INST}/emio_gpio_o] [get_bd_pins vcu/Din] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_100M [get_bd_pins ${::PS_INST}/maxihpm0_lpd_aclk] [get_bd_pins axi_ic_ctrl_0/ACLK] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_100M [get_bd_pins ${::PS_INST}/maxihpm0_lpd_aclk] [get_bd_pins axi_ic_ctrl_0/M00_ACLK] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_100M [get_bd_pins ${::PS_INST}/maxihpm0_lpd_aclk] [get_bd_pins axi_ic_ctrl_0/M01_ACLK] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_100M [get_bd_pins ${::PS_INST}/maxihpm0_lpd_aclk] [get_bd_pins axi_ic_ctrl_0/M02_ACLK] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_100M [get_bd_pins ${::PS_INST}/maxihpm0_lpd_aclk] [get_bd_pins axi_ic_ctrl_0/S00_ACLK] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_100M [get_bd_pins ${::PS_INST}/maxihpm0_lpd_aclk] [get_bd_pins clk_wiz_0/clk_100M] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_100M [get_bd_pins ${::PS_INST}/maxihpm0_lpd_aclk] [get_bd_pins display_pipeline/s_axi_aclk] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_100M [get_bd_pins ${::PS_INST}/maxihpm0_lpd_aclk] [get_bd_pins proc_sys_reset_100MHz/slowest_sync_clk] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_100M [get_bd_pins ${::PS_INST}/maxihpm0_lpd_aclk] [get_bd_pins vcu/s_axi_lite_aclk] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_275M [get_bd_pins ${::PS_INST}/maxihpm0_fpd_aclk] [get_bd_pins ${::PS_INST}/maxihpm1_fpd_aclk] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_275M [get_bd_pins ${::PS_INST}/maxihpm0_fpd_aclk] [get_bd_pins ${::PS_INST}/saxi_lpd_aclk] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_275M [get_bd_pins ${::PS_INST}/maxihpm0_fpd_aclk] [get_bd_pins ${::PS_INST}/saxihp0_fpd_aclk] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_275M [get_bd_pins ${::PS_INST}/maxihpm0_fpd_aclk] [get_bd_pins ${::PS_INST}/saxihp1_fpd_aclk] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_275M [get_bd_pins ${::PS_INST}/maxihpm0_fpd_aclk] [get_bd_pins ${::PS_INST}/saxihp2_fpd_aclk] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_275M [get_bd_pins ${::PS_INST}/maxihpm0_fpd_aclk] [get_bd_pins ${::PS_INST}/saxihp3_fpd_aclk] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_275M [get_bd_pins ${::PS_INST}/maxihpm0_fpd_aclk] [get_bd_pins ${::PS_INST}/saxihpc0_fpd_aclk] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_275M [get_bd_pins ${::PS_INST}/maxihpm0_fpd_aclk] [get_bd_pins ${::PS_INST}/saxihpc1_fpd_aclk] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_275M [get_bd_pins ${::PS_INST}/maxihpm0_fpd_aclk] [get_bd_pins axi_ic_accel_ctrl/ACLK] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_275M [get_bd_pins ${::PS_INST}/maxihpm0_fpd_aclk] [get_bd_pins axi_ic_accel_ctrl/M00_ACLK] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_275M [get_bd_pins ${::PS_INST}/maxihpm0_fpd_aclk] [get_bd_pins axi_ic_accel_ctrl/S00_ACLK] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_275M [get_bd_pins ${::PS_INST}/maxihpm0_fpd_aclk] [get_bd_pins axi_ic_ctrl_1/ACLK] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_275M [get_bd_pins ${::PS_INST}/maxihpm0_fpd_aclk] [get_bd_pins axi_ic_ctrl_1/M00_ACLK] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_275M [get_bd_pins ${::PS_INST}/maxihpm0_fpd_aclk] [get_bd_pins axi_ic_ctrl_1/S00_ACLK] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_275M [get_bd_pins ${::PS_INST}/maxihpm0_fpd_aclk] [get_bd_pins axi_vip_0/aclk] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_275M [get_bd_pins ${::PS_INST}/maxihpm0_fpd_aclk] [get_bd_pins clk_wiz_0/clk_275M] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_275M [get_bd_pins ${::PS_INST}/maxihpm0_fpd_aclk] [get_bd_pins display_pipeline/clk] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_275M [get_bd_pins ${::PS_INST}/maxihpm0_fpd_aclk] [get_bd_pins proc_sys_reset_275MHz/slowest_sync_clk] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_275M [get_bd_pins ${::PS_INST}/maxihpm0_fpd_aclk] [get_bd_pins vcu/ACLK] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_50M [get_bd_pins clk_wiz_0/clk_50M] [get_bd_pins vcu/pll_ref_clk]
  connect_bd_net -net clk_wiz_0_clk_550M [get_bd_pins clk_wiz_0/clk_550M] [get_bd_pins proc_sys_reset_550MHz/slowest_sync_clk]
  connect_bd_net -net display_pipeline_interrupt [get_bd_pins display_pipeline/interrupt] [get_bd_pins xlconcat_0/In0]
  connect_bd_net -net display_pipeline_irq [get_bd_pins display_pipeline/irq] [get_bd_pins xlconcat_0/In1]
  connect_bd_net -net display_pipeline_vid_active_video [get_bd_pins ${::PS_INST}/dp_live_video_in_de] [get_bd_pins display_pipeline/vid_active_video]
  connect_bd_net -net display_pipeline_vid_data [get_bd_pins ${::PS_INST}/dp_live_video_in_pixel1] [get_bd_pins display_pipeline/vid_data]
  connect_bd_net -net display_pipeline_vid_hsync [get_bd_pins ${::PS_INST}/dp_live_video_in_hsync] [get_bd_pins display_pipeline/vid_hsync]
  connect_bd_net -net display_pipeline_vid_vsync [get_bd_pins ${::PS_INST}/dp_live_video_in_vsync] [get_bd_pins display_pipeline/vid_vsync]
  connect_bd_net -net display_pipeline_video_clk [get_bd_pins ${::PS_INST}/dp_video_in_clk] [get_bd_pins display_pipeline/video_clk]
  connect_bd_net -net proc_sys_reset_1_interconnect_aresetn [get_bd_pins axi_ic_accel_ctrl/ARESETN] [get_bd_pins axi_ic_ctrl_1/ARESETN] -boundary_type upper
  connect_bd_net -net proc_sys_reset_1_interconnect_aresetn [get_bd_pins axi_ic_accel_ctrl/ARESETN] [get_bd_pins proc_sys_reset_275MHz/interconnect_aresetn] -boundary_type upper
  connect_bd_net -net proc_sys_reset_1_interconnect_aresetn [get_bd_pins axi_ic_accel_ctrl/ARESETN] [get_bd_pins vcu/ARESETN] -boundary_type upper
  connect_bd_net -net proc_sys_reset_1_peripheral_aresetn [get_bd_pins axi_ic_accel_ctrl/M00_ARESETN] [get_bd_pins axi_ic_accel_ctrl/S00_ARESETN] -boundary_type upper
  connect_bd_net -net proc_sys_reset_1_peripheral_aresetn [get_bd_pins axi_ic_accel_ctrl/M00_ARESETN] [get_bd_pins axi_ic_ctrl_1/M00_ARESETN] -boundary_type upper
  connect_bd_net -net proc_sys_reset_1_peripheral_aresetn [get_bd_pins axi_ic_accel_ctrl/M00_ARESETN] [get_bd_pins axi_ic_ctrl_1/S00_ARESETN] -boundary_type upper
  connect_bd_net -net proc_sys_reset_1_peripheral_aresetn [get_bd_pins axi_ic_accel_ctrl/M00_ARESETN] [get_bd_pins axi_vip_0/aresetn] -boundary_type upper
  connect_bd_net -net proc_sys_reset_1_peripheral_aresetn [get_bd_pins axi_ic_accel_ctrl/M00_ARESETN] [get_bd_pins display_pipeline/resetn] -boundary_type upper
  connect_bd_net -net proc_sys_reset_1_peripheral_aresetn [get_bd_pins axi_ic_accel_ctrl/M00_ARESETN] [get_bd_pins proc_sys_reset_275MHz/peripheral_aresetn] -boundary_type upper
  connect_bd_net -net proc_sys_reset_1_peripheral_aresetn [get_bd_pins axi_ic_accel_ctrl/M00_ARESETN] [get_bd_pins vcu/M00_ARESETN] -boundary_type upper
  connect_bd_net -net proc_sys_reset_2_peripheral_aresetn [get_bd_pins axi_ic_ctrl_0/M00_ARESETN] [get_bd_pins axi_ic_ctrl_0/M01_ARESETN] -boundary_type upper
  connect_bd_net -net proc_sys_reset_2_peripheral_aresetn [get_bd_pins axi_ic_ctrl_0/M00_ARESETN] [get_bd_pins axi_ic_ctrl_0/M02_ARESETN] -boundary_type upper
  connect_bd_net -net proc_sys_reset_2_peripheral_aresetn [get_bd_pins axi_ic_ctrl_0/M00_ARESETN] [get_bd_pins axi_ic_ctrl_0/S00_ARESETN] -boundary_type upper
  connect_bd_net -net proc_sys_reset_2_peripheral_aresetn [get_bd_pins axi_ic_ctrl_0/M00_ARESETN] [get_bd_pins display_pipeline/s_axi_aresetn] -boundary_type upper
  connect_bd_net -net proc_sys_reset_2_peripheral_aresetn [get_bd_pins axi_ic_ctrl_0/M00_ARESETN] [get_bd_pins proc_sys_reset_100MHz/peripheral_aresetn] -boundary_type upper
  connect_bd_net -net vcu_vcu_host_interrupt [get_bd_pins vcu/vcu_host_interrupt] [get_bd_pins xlconcat_0/In2]
  connect_bd_net -net xlconcat_0_dout [get_bd_pins ${::PS_INST}/pl_ps_irq1] [get_bd_pins xlconcat_0/dout]
  connect_bd_net -net xlconstant_0_fan_en_b_dout [get_bd_ports fan_en_b] [get_bd_pins xlconstant_0_fan_en_b/dout]
  connect_bd_net -net zynq_ultra_ps_e_0_pl_clk0 [get_bd_pins ${::PS_INST}/pl_clk0] [get_bd_pins clk_wiz_0/clk_in1]
  connect_bd_net -net zynq_ultra_ps_e_0_pl_resetn0 [get_bd_pins ${::PS_INST}/pl_resetn0] [get_bd_pins clk_wiz_0/resetn] -boundary_type upper
  connect_bd_net -net zynq_ultra_ps_e_0_pl_resetn0 [get_bd_pins ${::PS_INST}/pl_resetn0] [get_bd_pins display_pipeline/ext_reset_in] -boundary_type upper
  connect_bd_net -net zynq_ultra_ps_e_0_pl_resetn0 [get_bd_pins ${::PS_INST}/pl_resetn0] [get_bd_pins proc_sys_reset_100MHz/ext_reset_in] -boundary_type upper
  connect_bd_net -net zynq_ultra_ps_e_0_pl_resetn0 [get_bd_pins ${::PS_INST}/pl_resetn0] [get_bd_pins proc_sys_reset_275MHz/ext_reset_in] -boundary_type upper
  connect_bd_net -net zynq_ultra_ps_e_0_pl_resetn0 [get_bd_pins ${::PS_INST}/pl_resetn0] [get_bd_pins proc_sys_reset_550MHz/ext_reset_in] -boundary_type upper

  # Create address segments
  assign_bd_address -offset 0xA0000000 -range 0x00010000 -target_address_space [get_bd_addr_spaces ${::PS_INST}/Data] [get_bd_addr_segs axi_vip_0/S_AXI/Reg] -force
  assign_bd_address -offset 0x80000000 -range 0x00010000 -target_address_space [get_bd_addr_spaces ${::PS_INST}/Data] [get_bd_addr_segs display_pipeline/clk_wiz_0/s_axi_lite/Reg] -force
  assign_bd_address -offset 0xB0000000 -range 0x00010000 -target_address_space [get_bd_addr_spaces ${::PS_INST}/Data] [get_bd_addr_segs display_pipeline/v_mix_0/s_axi_CTRL/Reg] -force
  assign_bd_address -offset 0x80010000 -range 0x00010000 -target_address_space [get_bd_addr_spaces ${::PS_INST}/Data] [get_bd_addr_segs display_pipeline/v_tc_0/ctrl/Reg] -force
  assign_bd_address -offset 0x80100000 -range 0x00100000 -target_address_space [get_bd_addr_spaces ${::PS_INST}/Data] [get_bd_addr_segs vcu/vcu_0/S_AXI_LITE/Reg] -force
  assign_bd_address -offset 0x00000000 -range 0x80000000 -target_address_space [get_bd_addr_spaces display_pipeline/v_mix_0/Data_m_axi_mm_video1] [get_bd_addr_segs ${::PS_INST}/SAXIGP2/HP0_DDR_LOW] -force
  assign_bd_address -offset 0x00000000 -range 0x80000000 -target_address_space [get_bd_addr_spaces display_pipeline/v_mix_0/Data_m_axi_mm_video2] [get_bd_addr_segs ${::PS_INST}/SAXIGP2/HP0_DDR_LOW] -force
  assign_bd_address -offset 0x00000000 -range 0x80000000 -target_address_space [get_bd_addr_spaces display_pipeline/v_mix_0/Data_m_axi_mm_video3] [get_bd_addr_segs ${::PS_INST}/SAXIGP2/HP0_DDR_LOW] -force
  assign_bd_address -offset 0x00000000 -range 0x80000000 -target_address_space [get_bd_addr_spaces display_pipeline/v_mix_0/Data_m_axi_mm_video4] [get_bd_addr_segs ${::PS_INST}/SAXIGP2/HP0_DDR_LOW] -force
  assign_bd_address -offset 0x00000000 -range 0x80000000 -target_address_space [get_bd_addr_spaces display_pipeline/v_mix_0/Data_m_axi_mm_video5] [get_bd_addr_segs ${::PS_INST}/SAXIGP2/HP0_DDR_LOW] -force
  assign_bd_address -offset 0x00000000 -range 0x80000000 -target_address_space [get_bd_addr_spaces vcu/vcu_0/Code] [get_bd_addr_segs ${::PS_INST}/SAXIGP5/HP3_DDR_LOW] -force
  assign_bd_address -offset 0x00000000 -range 0x80000000 -target_address_space [get_bd_addr_spaces vcu/vcu_0/DecData0] [get_bd_addr_segs ${::PS_INST}/SAXIGP1/HPC1_DDR_LOW] -force
  assign_bd_address -offset 0x00000000 -range 0x80000000 -target_address_space [get_bd_addr_spaces vcu/vcu_0/DecData1] [get_bd_addr_segs ${::PS_INST}/SAXIGP1/HPC1_DDR_LOW] -force


  # Restore current instance
  current_bd_instance $oldCurInst

  # Create PFM attributes
  set_property PFM_NAME {xilinx.com:kv260:kv260_vcuDecode_vmixDP:1.0} [get_files [current_bd_design].bd]
  set_property PFM.AXI_PORT {S_AXI_HPC0_FPD {memport "S_AXI_HP" sptag "HPC0" memory "PS_0 HPC0_DDR_LOW"} S_AXI_HP1_FPD {memport "S_AXI_HP" sptag "HP1" memory "PS_0 HP1_DDR_LOW"} S_AXI_HP2_FPD {memport "S_AXI_HP" sptag "HP2" memory "PS_0 HP2_DDR_LOW"} S_AXI_LPD {memport "MIG" sptag "LPD" memory "PS_0 LPD_DDR_LOW"}} [get_bd_cells /PS_0]
  set_property PFM.IRQ {pl_ps_irq0 {id 0 range 7}} [get_bd_cells /PS_0]
  set_property PFM.CLOCK {clk_100M {id "2" is_default "false" proc_sys_reset "/proc_sys_reset_100MHz" status "fixed"} clk_275M {id "0" is_default "true" proc_sys_reset "/proc_sys_reset_275MHz" status "fixed"} clk_550M {id "1" is_default "false" proc_sys_reset "/proc_sys_reset_550MHz" status "fixed"}} [get_bd_cells /clk_wiz_0]
  set_property PFM.AXI_PORT {M01_AXI {memport "M_AXI_GP" sptag "M_AXI_HPM0" memory ""} M02_AXI {memport "M_AXI_GP" sptag "M_AXI_HPM0" memory ""} M03_AXI {memport "M_AXI_GP" sptag "M_AXI_HPM0" memory ""} M04_AXI {memport "M_AXI_GP" sptag "M_AXI_HPM0" memory ""} M05_AXI {memport "M_AXI_GP" sptag "M_AXI_HPM0" memory ""} M06_AXI {memport "M_AXI_GP" sptag "M_AXI_HPM0" memory ""} M07_AXI {memport "M_AXI_GP" sptag "M_AXI_HPM0" memory ""} M08_AXI {memport "M_AXI_GP" sptag "M_AXI_HPM0" memory ""} M09_AXI {memport "M_AXI_GP" sptag "M_AXI_HPM0" memory ""} M10_AXI {memport "M_AXI_GP" sptag "M_AXI_HPM0" memory ""} M11_AXI {memport "M_AXI_GP" sptag "M_AXI_HPM0" memory ""} M12_AXI {memport "M_AXI_GP" sptag "M_AXI_HPM0" memory ""} M13_AXI {memport "M_AXI_GP" sptag "M_AXI_HPM0" memory ""} M14_AXI {memport "M_AXI_GP" sptag "M_AXI_HPM0" memory ""} M15_AXI {memport "M_AXI_GP" sptag "M_AXI_HPM0" memory ""}} [get_bd_cells /axi_ic_accel_ctrl]



  validate_bd_design
  save_bd_design
}
# End of create_root_design()


##################################################################
# MAIN FLOW
##################################################################

create_root_design ""


