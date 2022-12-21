# (C) Copyright 2020 - 2021 Xilinx, Inc.
# SPDX-License-Identifier: Apache-2.0

set ::PS_INST PS_0
set PS_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:zynq_ultra_ps_e PS_0 ]


apply_bd_automation -rule xilinx.com:bd_rule:zynq_ultra_ps_e -config {apply_board_preset "1" }  [get_bd_cells PS_0]

  set_property -dict [ list \
   CONFIG.PSU__CRF_APB__DPLL_FRAC_CFG__ENABLED  {1} \
   CONFIG.PSU__CRF_APB__VPLL_FRAC_CFG__ENABLED  {1} \
   CONFIG.PSU__CRL_APB__PL1_REF_CTRL__DIVISOR1 {1} \
   CONFIG.PSU__CRL_APB__PL1_REF_CTRL__SRCSEL {IOPLL} \
   CONFIG.PSU__CRL_APB__USB3__ENABLE {1} \
   CONFIG.PSU__FPGA_PL0_ENABLE {1} \
   CONFIG.PSU__GPIO_EMIO__PERIPHERAL__ENABLE {1} \
   CONFIG.PSU__GPIO_EMIO__PERIPHERAL__IO {92} \
   CONFIG.PSU__TTC0__WAVEOUT__ENABLE {1} \
   CONFIG.PSU__TTC0__WAVEOUT__IO {EMIO} \
   CONFIG.PSU__FPGA_PL0_ENABLE {1} \
   CONFIG.PSU__MAXIGP0__DATA_WIDTH {32} \
   CONFIG.PSU__MAXIGP1__DATA_WIDTH {32} \
   CONFIG.PSU__MAXIGP2__DATA_WIDTH {32} \
   CONFIG.PSU__NUM_FABRIC_RESETS {4} \
   CONFIG.PSU__SAXIGP0__DATA_WIDTH {128} \
   CONFIG.PSU__SAXIGP1__DATA_WIDTH {128} \
   CONFIG.PSU__SAXIGP2__DATA_WIDTH {128} \
   CONFIG.PSU__SAXIGP3__DATA_WIDTH {128} \
   CONFIG.PSU__SAXIGP4__DATA_WIDTH {128} \
   CONFIG.PSU__SAXIGP5__DATA_WIDTH {128} \
   CONFIG.PSU__SAXIGP6__DATA_WIDTH {128} \
   CONFIG.PSU__USE__IRQ0 {1} \
   CONFIG.PSU__USE__IRQ1 {1} \
   CONFIG.PSU__USE__M_AXI_GP0 {1} \
   CONFIG.PSU__USE__M_AXI_GP1 {1} \
   CONFIG.PSU__USE__M_AXI_GP2 {1} \
   CONFIG.PSU__USE__S_AXI_ACE {0} \
   CONFIG.PSU__USE__S_AXI_ACP {0} \
   CONFIG.PSU__USE__S_AXI_GP0 {1} \
   CONFIG.PSU__USE__S_AXI_GP1 {1} \
   CONFIG.PSU__USE__S_AXI_GP2 {1} \
   CONFIG.PSU__USE__S_AXI_GP3 {1} \
   CONFIG.PSU__USE__S_AXI_GP4 {1} \
   CONFIG.PSU__USE__S_AXI_GP5 {1} \
   CONFIG.PSU__USE__S_AXI_GP6 {1} \
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
  create_bd_intf_pin -mode Master -vlnv xilinx.com:interface:aximm_rtl:1.0 M00_AXI_VCU_DEC

  create_bd_intf_pin -mode Master -vlnv xilinx.com:interface:aximm_rtl:1.0 M00_AXI_VCU_EN

  create_bd_intf_pin -mode Master -vlnv xilinx.com:interface:aximm_rtl:1.0 M_AXI_VCU_MCU

  create_bd_intf_pin -mode Slave -vlnv xilinx.com:interface:aximm_rtl:1.0 S_AXI_LITE


  # Create pins
  create_bd_pin -dir I -type rst aresetn
  create_bd_pin -dir I -type clk m_axi_dec_aclk
  create_bd_pin -dir I -type clk pll_ref_clk
  create_bd_pin -dir I -type clk s_axi_lite_aclk
  create_bd_pin -dir O -type intr vcu_host_interrupt
  create_bd_pin -dir I -type rst vcu_resetn

  # Create instance: axi_interconnect_0, and set properties
  set axi_interconnect_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:axi_interconnect axi_interconnect_0 ]
  set_property -dict [ list \
   CONFIG.NUM_MI {1} \
 ] $axi_interconnect_0

  # Create instance: axi_register_slice_0, and set properties
  set axi_register_slice_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:axi_register_slice axi_register_slice_0 ]

  # Create instance: smartconnect_vcu_dec, and set properties
  set smartconnect_vcu_dec [ create_bd_cell -type ip -vlnv xilinx.com:ip:smartconnect smartconnect_vcu_dec ]

  # Create instance: smartconnect_vcu_en, and set properties
  set smartconnect_vcu_en [ create_bd_cell -type ip -vlnv xilinx.com:ip:smartconnect smartconnect_vcu_en ]

  # Create instance: vcu_0, and set properties
  set vcu_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:vcu vcu_0 ]
  set_property -dict [ list \
   CONFIG.DEC_CODING_TYPE {0} \
   CONFIG.DEC_COLOR_DEPTH {0} \
   CONFIG.DEC_COLOR_FORMAT {0} \
   CONFIG.DEC_FPS {1} \
   CONFIG.DEC_FRAME_SIZE {4} \
   CONFIG.ENABLE_DECODER {true} \
   CONFIG.ENC_BUFFER_EN {true} \
   CONFIG.ENC_BUFFER_MANUAL_OVERRIDE {1} \
   CONFIG.ENC_BUFFER_SIZE {253} \
   CONFIG.ENC_BUFFER_SIZE_ACTUAL {284} \
   CONFIG.ENC_BUFFER_TYPE {0} \
   CONFIG.ENC_CODING_TYPE {1} \
   CONFIG.ENC_COLOR_DEPTH {0} \
   CONFIG.ENC_COLOR_FORMAT {0} \
   CONFIG.ENC_FPS {1} \
   CONFIG.ENC_FRAME_SIZE {4} \
   CONFIG.ENC_MEM_BRAM_USED {0} \
   CONFIG.ENC_MEM_URAM_USED {284} \
   CONFIG.NO_OF_DEC_STREAMS {1} \
   CONFIG.NO_OF_STREAMS {1} \
   CONFIG.TABLE_NO {2} \
 ] $vcu_0

  # Create interface connections
  connect_bd_intf_net -intf_net Conn1 [get_bd_intf_pins M00_AXI_VCU_DEC] [get_bd_intf_pins smartconnect_vcu_dec/M00_AXI]
  connect_bd_intf_net -intf_net axi_interconnect_0_M00_AXI [get_bd_intf_pins M_AXI_VCU_MCU] [get_bd_intf_pins axi_interconnect_0/M00_AXI]
  connect_bd_intf_net -intf_net axi_register_slice_0_M_AXI [get_bd_intf_pins axi_interconnect_0/S00_AXI] [get_bd_intf_pins axi_register_slice_0/M_AXI]
  connect_bd_intf_net -intf_net smartconnect_0_M02_AXI [get_bd_intf_pins S_AXI_LITE] [get_bd_intf_pins vcu_0/S_AXI_LITE]
  connect_bd_intf_net -intf_net smartconnect_vcu_M00_AXI [get_bd_intf_pins M00_AXI_VCU_EN] [get_bd_intf_pins smartconnect_vcu_en/M00_AXI]
  connect_bd_intf_net -intf_net vcu_0_M_AXI_DEC0 [get_bd_intf_pins smartconnect_vcu_dec/S00_AXI] [get_bd_intf_pins vcu_0/M_AXI_DEC0]
  connect_bd_intf_net -intf_net vcu_0_M_AXI_DEC1 [get_bd_intf_pins smartconnect_vcu_dec/S01_AXI] [get_bd_intf_pins vcu_0/M_AXI_DEC1]
  connect_bd_intf_net -intf_net vcu_0_M_AXI_ENC0 [get_bd_intf_pins smartconnect_vcu_en/S00_AXI] [get_bd_intf_pins vcu_0/M_AXI_ENC0]
  connect_bd_intf_net -intf_net vcu_0_M_AXI_ENC1 [get_bd_intf_pins smartconnect_vcu_en/S01_AXI] [get_bd_intf_pins vcu_0/M_AXI_ENC1]
  connect_bd_intf_net -intf_net vcu_0_M_AXI_MCU [get_bd_intf_pins axi_register_slice_0/S_AXI] [get_bd_intf_pins vcu_0/M_AXI_MCU]

  # Create port connections
  connect_bd_net -net clk_wiz_0_clk_100M [get_bd_pins s_axi_lite_aclk] [get_bd_pins vcu_0/s_axi_lite_aclk]
  connect_bd_net -net clk_wiz_0_clk_300M [get_bd_pins m_axi_dec_aclk] [get_bd_pins axi_interconnect_0/ACLK] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_300M [get_bd_pins m_axi_dec_aclk] [get_bd_pins axi_interconnect_0/M00_ACLK] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_300M [get_bd_pins m_axi_dec_aclk] [get_bd_pins axi_interconnect_0/S00_ACLK] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_300M [get_bd_pins m_axi_dec_aclk] [get_bd_pins axi_register_slice_0/aclk] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_300M [get_bd_pins m_axi_dec_aclk] [get_bd_pins smartconnect_vcu_dec/aclk] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_300M [get_bd_pins m_axi_dec_aclk] [get_bd_pins smartconnect_vcu_en/aclk] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_300M [get_bd_pins m_axi_dec_aclk] [get_bd_pins vcu_0/m_axi_dec_aclk] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_300M [get_bd_pins m_axi_dec_aclk] [get_bd_pins vcu_0/m_axi_enc_aclk] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_300M [get_bd_pins m_axi_dec_aclk] [get_bd_pins vcu_0/m_axi_mcu_aclk] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_50M [get_bd_pins pll_ref_clk] [get_bd_pins vcu_0/pll_ref_clk]
  connect_bd_net -net proc_sys_reset_1_peripheral_aresetn [get_bd_pins aresetn] [get_bd_pins axi_interconnect_0/ARESETN] -boundary_type upper
  connect_bd_net -net proc_sys_reset_1_peripheral_aresetn [get_bd_pins aresetn] [get_bd_pins axi_interconnect_0/M00_ARESETN] -boundary_type upper
  connect_bd_net -net proc_sys_reset_1_peripheral_aresetn [get_bd_pins aresetn] [get_bd_pins axi_interconnect_0/S00_ARESETN] -boundary_type upper
  connect_bd_net -net proc_sys_reset_1_peripheral_aresetn [get_bd_pins aresetn] [get_bd_pins axi_register_slice_0/aresetn] -boundary_type upper
  connect_bd_net -net proc_sys_reset_1_peripheral_aresetn [get_bd_pins aresetn] [get_bd_pins smartconnect_vcu_dec/aresetn] -boundary_type upper
  connect_bd_net -net proc_sys_reset_1_peripheral_aresetn [get_bd_pins aresetn] [get_bd_pins smartconnect_vcu_en/aresetn] -boundary_type upper
  connect_bd_net -net proc_sys_reset_2_peripheral_aresetn [get_bd_pins vcu_resetn] [get_bd_pins vcu_0/vcu_resetn]
  connect_bd_net -net vcu_0_vcu_host_interrupt [get_bd_pins vcu_host_interrupt] [get_bd_pins vcu_0/vcu_host_interrupt]

  # Restore current instance
  current_bd_instance $oldCurInst
}

# Hierarchical cell: capture_pipeline_raspi
proc create_hier_cell_capture_pipeline_raspi { parentCell nameHier } {

  variable script_folder

  if { $parentCell eq "" || $nameHier eq "" } {
     catch {common::send_gid_msg -ssname BD::TCL -id 2092 -severity "ERROR" "create_hier_cell_capture_pipeline_raspi() - Empty argument(s)!"}
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
  create_bd_intf_pin -mode Slave -vlnv xilinx.com:interface:aximm_rtl:1.0 csirxss_s_axi

  create_bd_intf_pin -mode Master -vlnv xilinx.com:interface:aximm_rtl:1.0 m_axi_mm_video

  create_bd_intf_pin -mode Slave -vlnv xilinx.com:interface:mipi_phy_rtl:1.0 mipi_phy_if

  create_bd_intf_pin -mode Slave -vlnv xilinx.com:interface:aximm_rtl:1.0 s_axi_ctrl_dem

  create_bd_intf_pin -mode Slave -vlnv xilinx.com:interface:aximm_rtl:1.0 s_axi_ctrl_frmbuf

  create_bd_intf_pin -mode Slave -vlnv xilinx.com:interface:aximm_rtl:1.0 s_axi_ctrl_vpss


  # Create pins
  create_bd_pin -dir I -from 91 -to 0 Din
  create_bd_pin -dir I -type clk aclk
  create_bd_pin -dir I -type rst aresetn
  create_bd_pin -dir I -type clk clkoutphy_in
  create_bd_pin -dir O -type intr csirxss_csi_irq
  create_bd_pin -dir O -type intr dem_irq
  create_bd_pin -dir I -type clk dphy_clk_200M
  create_bd_pin -dir O -type intr frm_buf_irq
  create_bd_pin -dir I -type clk lite_aclk
  create_bd_pin -dir I -type rst lite_aresetn
  create_bd_pin -dir I pll_lock_in

  # Create instance: axis_data_fifo_cap, and set properties
  set axis_data_fifo_cap [ create_bd_cell -type ip -vlnv xilinx.com:ip:axis_data_fifo axis_data_fifo_cap ]
  set_property -dict [ list \
   CONFIG.FIFO_DEPTH {1024} \
 ] $axis_data_fifo_cap

  # Create instance: axis_subset_converter_0, and set properties
  set axis_subset_converter_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:axis_subset_converter axis_subset_converter_0 ]
  set_property -dict [ list \
   CONFIG.M_TDATA_NUM_BYTES {1} \
   CONFIG.M_TDEST_WIDTH {1} \
   CONFIG.S_TDATA_NUM_BYTES {2} \
   CONFIG.TDATA_REMAP {tdata[9:2]} \
 ] $axis_subset_converter_0

  # Create instance: mipi_csi2_rx_subsyst_0, and set properties
  set mipi_csi2_rx_subsyst_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:mipi_csi2_rx_subsystem mipi_csi2_rx_subsyst_0 ]
  set_property -dict [ list \
   CONFIG.AXIS_TDEST_WIDTH {4} \
   CONFIG.CLK_LANE_IO_LOC {D7} \
   CONFIG.CLK_LANE_IO_LOC_NAME {IO_L13P_T2L_N0_GC_QBC_66} \
   CONFIG.CMN_NUM_LANES {2} \
   CONFIG.CMN_NUM_PIXELS {1} \
   CONFIG.CMN_PXL_FORMAT {RAW10} \
   CONFIG.CMN_VC {All} \
   CONFIG.CSI_BUF_DEPTH {512} \
   CONFIG.C_CLK_LANE_IO_POSITION {26} \
   CONFIG.C_CSI_EN_ACTIVELANES {false} \
   CONFIG.C_CSI_EN_CRC {true} \
   CONFIG.C_CSI_FILTER_USERDATATYPE {false} \
   CONFIG.C_DATA_LANE0_IO_POSITION {28} \
   CONFIG.C_DATA_LANE1_IO_POSITION {30} \
   CONFIG.C_DATA_LANE2_IO_POSITION {19} \
   CONFIG.C_DATA_LANE3_IO_POSITION {21} \
   CONFIG.C_DPHY_LANES {2} \
   CONFIG.C_EN_BG0_PIN0 {false} \
   CONFIG.C_EN_BG1_PIN0 {false} \
   CONFIG.C_EN_CSI_V2_0 {false} \
   CONFIG.C_HS_LINE_RATE {912} \
   CONFIG.C_HS_SETTLE_NS {145} \
   CONFIG.C_STRETCH_LINE_RATE {1500} \
   CONFIG.DATA_LANE0_IO_LOC {E5} \
   CONFIG.DATA_LANE0_IO_LOC_NAME {IO_L14P_T2L_N2_GC_66} \
   CONFIG.DATA_LANE1_IO_LOC {G6} \
   CONFIG.DATA_LANE1_IO_LOC_NAME {IO_L15P_T2L_N4_AD11P_66} \
   CONFIG.DPHYRX_BOARD_INTERFACE {som240_1_connector_mipi_csi_raspi} \
   CONFIG.DPY_EN_REG_IF {false} \
   CONFIG.DPY_LINE_RATE {912} \
   CONFIG.HP_IO_BANK_SELECTION {66} \
   CONFIG.SupportLevel {0} \
 ] $mipi_csi2_rx_subsyst_0

  # Create instance: v_demosaic_0, and set properties
  set v_demosaic_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:v_demosaic v_demosaic_0 ]
  set_property -dict [ list \
   CONFIG.MAX_COLS {1920} \
   CONFIG.MAX_ROWS {1080} \
   CONFIG.USE_URAM {0} \
 ] $v_demosaic_0

  # Create instance: v_frmbuf_wr_0, and set properties
  set v_frmbuf_wr_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:v_frmbuf_wr v_frmbuf_wr_0 ]
  set_property -dict [ list \
   CONFIG.AXIMM_DATA_WIDTH {64} \
   CONFIG.C_M_AXI_MM_VIDEO_DATA_WIDTH {64} \
   CONFIG.HAS_BGR8 {0} \
   CONFIG.HAS_BGRX8 {0} \
   CONFIG.HAS_RGB8 {0} \
   CONFIG.HAS_UYVY8 {0} \
   CONFIG.HAS_YUV8 {0} \
   CONFIG.HAS_Y_UV8 {0} \
   CONFIG.HAS_Y_UV8_420 {1} \
   CONFIG.MAX_COLS {1920} \
   CONFIG.MAX_NR_PLANES {2} \
   CONFIG.MAX_ROWS {1080} \
   CONFIG.SAMPLES_PER_CLOCK {1} \
 ] $v_frmbuf_wr_0

  # Create instance: v_proc_ss_0, and set properties
  set v_proc_ss_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:v_proc_ss v_proc_ss_0 ]
  set_property -dict [ list \
   CONFIG.C_COLORSPACE_SUPPORT {0} \
   CONFIG.C_ENABLE_CSC {true} \
   CONFIG.C_MAX_COLS {1920} \
   CONFIG.C_MAX_DATA_WIDTH {8} \
   CONFIG.C_MAX_ROWS {1080} \
   CONFIG.C_SAMPLES_PER_CLK {1} \
   CONFIG.C_TOPOLOGY {0} \
 ] $v_proc_ss_0

  # Create instance: xlslice_4, and set properties
  set xlslice_4 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlslice xlslice_4 ]
  set_property -dict [ list \
   CONFIG.DIN_FROM {4} \
   CONFIG.DIN_TO {4} \
   CONFIG.DIN_WIDTH {92} \
   CONFIG.DOUT_WIDTH {1} \
 ] $xlslice_4

  # Create instance: xlslice_5, and set properties
  set xlslice_5 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlslice xlslice_5 ]
  set_property -dict [ list \
   CONFIG.DIN_FROM {5} \
   CONFIG.DIN_TO {5} \
   CONFIG.DIN_WIDTH {92} \
   CONFIG.DOUT_WIDTH {1} \
 ] $xlslice_5

  # Create instance: xlslice_6, and set properties
  set xlslice_6 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlslice xlslice_6 ]
  set_property -dict [ list \
   CONFIG.DIN_FROM {6} \
   CONFIG.DIN_TO {6} \
   CONFIG.DIN_WIDTH {92} \
   CONFIG.DOUT_WIDTH {1} \
 ] $xlslice_6

  # Create interface connections
  connect_bd_intf_net -intf_net Conn1 [get_bd_intf_pins s_axi_ctrl_dem] [get_bd_intf_pins v_demosaic_0/s_axi_CTRL]
  connect_bd_intf_net -intf_net Conn2 [get_bd_intf_pins s_axi_ctrl_vpss] [get_bd_intf_pins v_proc_ss_0/s_axi_ctrl]
  connect_bd_intf_net -intf_net axi_interconnect_0_M00_AXI [get_bd_intf_pins csirxss_s_axi] [get_bd_intf_pins mipi_csi2_rx_subsyst_0/csirxss_s_axi]
  connect_bd_intf_net -intf_net axi_interconnect_0_M11_AXI [get_bd_intf_pins s_axi_ctrl_frmbuf] [get_bd_intf_pins v_frmbuf_wr_0/s_axi_CTRL]
  connect_bd_intf_net -intf_net axis_data_fifo_cap_M_AXIS [get_bd_intf_pins axis_data_fifo_cap/M_AXIS] [get_bd_intf_pins axis_subset_converter_0/S_AXIS]
  connect_bd_intf_net -intf_net axis_subset_converter_0_M_AXIS [get_bd_intf_pins axis_subset_converter_0/M_AXIS] [get_bd_intf_pins v_demosaic_0/s_axis_video]
  connect_bd_intf_net -intf_net mipi_csi2_rx_subsyst_0_video_out [get_bd_intf_pins axis_data_fifo_cap/S_AXIS] [get_bd_intf_pins mipi_csi2_rx_subsyst_0/video_out]
  connect_bd_intf_net -intf_net mipi_phy_if_1 [get_bd_intf_pins mipi_phy_if] [get_bd_intf_pins mipi_csi2_rx_subsyst_0/mipi_phy_if]
  connect_bd_intf_net -intf_net v_demosaic_0_m_axis_video [get_bd_intf_pins v_demosaic_0/m_axis_video] [get_bd_intf_pins v_proc_ss_0/s_axis]
  connect_bd_intf_net -intf_net v_frmbuf_wr_1_m_axi_mm_video [get_bd_intf_pins m_axi_mm_video] [get_bd_intf_pins v_frmbuf_wr_0/m_axi_mm_video]
  connect_bd_intf_net -intf_net v_proc_ss_0_m_axis [get_bd_intf_pins v_frmbuf_wr_0/s_axis_video] [get_bd_intf_pins v_proc_ss_0/m_axis]

  # Create port connections
  connect_bd_net -net Din_1 [get_bd_pins Din] [get_bd_pins xlslice_4/Din] -boundary_type upper
  connect_bd_net -net Din_1 [get_bd_pins Din] [get_bd_pins xlslice_5/Din] -boundary_type upper
  connect_bd_net -net Din_1 [get_bd_pins Din] [get_bd_pins xlslice_6/Din] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_100M [get_bd_pins lite_aclk] [get_bd_pins mipi_csi2_rx_subsyst_0/lite_aclk]
  connect_bd_net -net clk_wiz_0_clk_200M [get_bd_pins dphy_clk_200M] [get_bd_pins mipi_csi2_rx_subsyst_0/dphy_clk_200M]
  connect_bd_net -net clk_wiz_0_clk_300M [get_bd_pins aclk] [get_bd_pins axis_data_fifo_cap/s_axis_aclk] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_300M [get_bd_pins aclk] [get_bd_pins axis_subset_converter_0/aclk] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_300M [get_bd_pins aclk] [get_bd_pins mipi_csi2_rx_subsyst_0/video_aclk] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_300M [get_bd_pins aclk] [get_bd_pins v_demosaic_0/ap_clk] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_300M [get_bd_pins aclk] [get_bd_pins v_frmbuf_wr_0/ap_clk] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_300M [get_bd_pins aclk] [get_bd_pins v_proc_ss_0/aclk_axis] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_300M [get_bd_pins aclk] [get_bd_pins v_proc_ss_0/aclk_ctrl] -boundary_type upper
  connect_bd_net -net clkoutphy_in_1 [get_bd_pins clkoutphy_in] [get_bd_pins mipi_csi2_rx_subsyst_0/clkoutphy_in]
  connect_bd_net -net mipi_csi2_rx_subsyst_0_csirxss_csi_irq [get_bd_pins csirxss_csi_irq] [get_bd_pins mipi_csi2_rx_subsyst_0/csirxss_csi_irq]
  connect_bd_net -net pll_lock_in_1 [get_bd_pins pll_lock_in] [get_bd_pins mipi_csi2_rx_subsyst_0/pll_lock_in]
  connect_bd_net -net proc_sys_reset_1_peripheral_aresetn [get_bd_pins aresetn] [get_bd_pins axis_data_fifo_cap/s_axis_aresetn] -boundary_type upper
  connect_bd_net -net proc_sys_reset_1_peripheral_aresetn [get_bd_pins aresetn] [get_bd_pins axis_subset_converter_0/aresetn] -boundary_type upper
  connect_bd_net -net proc_sys_reset_1_peripheral_aresetn [get_bd_pins aresetn] [get_bd_pins mipi_csi2_rx_subsyst_0/video_aresetn] -boundary_type upper
  connect_bd_net -net proc_sys_reset_2_peripheral_aresetn [get_bd_pins lite_aresetn] [get_bd_pins mipi_csi2_rx_subsyst_0/lite_aresetn]
  connect_bd_net -net v_demosaic_0_interrupt [get_bd_pins dem_irq] [get_bd_pins v_demosaic_0/interrupt]
  connect_bd_net -net v_frmbuf_wr_1_interrupt [get_bd_pins frm_buf_irq] [get_bd_pins v_frmbuf_wr_0/interrupt]
  connect_bd_net -net xlslice_1_Dout [get_bd_pins v_proc_ss_0/aresetn_ctrl] [get_bd_pins xlslice_5/Dout]
  connect_bd_net -net xlslice_2_Dout [get_bd_pins v_frmbuf_wr_0/ap_rst_n] [get_bd_pins xlslice_6/Dout]
  connect_bd_net -net xlslice_2_Dout1 [get_bd_pins v_demosaic_0/ap_rst_n] [get_bd_pins xlslice_4/Dout]

  # Restore current instance
  current_bd_instance $oldCurInst
}

# Hierarchical cell: capture_pipeline_isp
proc create_hier_cell_capture_pipeline_isp { parentCell nameHier } {

  variable script_folder

  if { $parentCell eq "" || $nameHier eq "" } {
     catch {common::send_gid_msg -ssname BD::TCL -id 2092 -severity "ERROR" "create_hier_cell_capture_pipeline_isp() - Empty argument(s)!"}
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
  create_bd_intf_pin -mode Slave -vlnv xilinx.com:interface:aximm_rtl:1.0 csirxss_s_axi

  create_bd_intf_pin -mode Master -vlnv xilinx.com:interface:aximm_rtl:1.0 m_axi_mm_video

  create_bd_intf_pin -mode Slave -vlnv xilinx.com:interface:mipi_phy_rtl:1.0 mipi_phy_if

  create_bd_intf_pin -mode Slave -vlnv xilinx.com:interface:aximm_rtl:1.0 s_axi_ctrl_frmbuf


  # Create pins
  create_bd_pin -dir I -from 91 -to 0 Din
  create_bd_pin -dir I -type clk aclk
  create_bd_pin -dir I -type rst aresetn
  create_bd_pin -dir O -type intr csirxss_csi_irq
  create_bd_pin -dir I -type clk dphy_clk_200M
  create_bd_pin -dir O -type intr interrupt
  create_bd_pin -dir I -type clk lite_aclk
  create_bd_pin -dir I -type rst lite_aresetn

  # Create instance: axis_data_fifo_cap, and set properties
  set axis_data_fifo_cap [ create_bd_cell -type ip -vlnv xilinx.com:ip:axis_data_fifo axis_data_fifo_cap ]
  set_property -dict [ list \
   CONFIG.FIFO_DEPTH {1024} \
 ] $axis_data_fifo_cap

  # Create instance: axis_subset_converter_0, and set properties
  set axis_subset_converter_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:axis_subset_converter axis_subset_converter_0 ]
  set_property -dict [ list \
   CONFIG.M_TDATA_NUM_BYTES {6} \
   CONFIG.M_TDEST_WIDTH {1} \
   CONFIG.S_TDATA_NUM_BYTES {4} \
   CONFIG.S_TDEST_WIDTH {10} \
   CONFIG.TDATA_REMAP {16'b0000000000000000,tdata[31:0]} \
   ] $axis_subset_converter_0

  # Create instance: mipi_csi2_rx_subsyst_0, and set properties
  set mipi_csi2_rx_subsyst_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:mipi_csi2_rx_subsystem mipi_csi2_rx_subsyst_0 ]
  set_property -dict [ list \
   CONFIG.AXIS_TDEST_WIDTH {4} \
   CONFIG.CLK_LANE_IO_LOC {G1} \
   CONFIG.CLK_LANE_IO_LOC_NAME {IO_L1P_T0L_N0_DBC_66} \
   CONFIG.CMN_NUM_LANES {4} \
   CONFIG.CMN_NUM_PIXELS {2} \
   CONFIG.CMN_PXL_FORMAT {YUV420_8bit} \
   CONFIG.CMN_VC {0} \
   CONFIG.CSI_BUF_DEPTH {4096} \
   CONFIG.C_CSI_EN_ACTIVELANES {false} \
   CONFIG.C_CSI_FILTER_USERDATATYPE {true} \
   CONFIG.C_DATA_LANE0_IO_POSITION {2} \
   CONFIG.C_DATA_LANE1_IO_POSITION {4} \
   CONFIG.C_DATA_LANE2_IO_POSITION {6} \
   CONFIG.C_DATA_LANE3_IO_POSITION {8} \
   CONFIG.C_DPHY_LANES {4} \
   CONFIG.C_EN_CSI_V2_0 {false} \
   CONFIG.C_HS_LINE_RATE {896} \
   CONFIG.C_HS_SETTLE_NS {146} \
   CONFIG.C_STRETCH_LINE_RATE {1500} \
   CONFIG.DATA_LANE0_IO_LOC {E1} \
   CONFIG.DATA_LANE0_IO_LOC_NAME {IO_L2P_T0L_N2_66} \
   CONFIG.DATA_LANE1_IO_LOC {F2} \
   CONFIG.DATA_LANE1_IO_LOC_NAME {IO_L3P_T0L_N4_AD15P_66} \
   CONFIG.DATA_LANE2_IO_LOC {G3} \
   CONFIG.DATA_LANE2_IO_LOC_NAME {IO_L4P_T0U_N6_DBC_AD7P_66} \
   CONFIG.DATA_LANE3_IO_LOC {E4} \
   CONFIG.DATA_LANE3_IO_LOC_NAME {IO_L5P_T0U_N8_AD14P_66} \
   CONFIG.DPHYRX_BOARD_INTERFACE {som240_1_connector_mipi_csi_isp} \
   CONFIG.DPY_EN_REG_IF {false} \
   CONFIG.DPY_LINE_RATE {896} \
   CONFIG.HP_IO_BANK_SELECTION {66} \
   CONFIG.SupportLevel {1} \
   CONFIG.YUV420_BUF_DPTH {4096} \
 ] $mipi_csi2_rx_subsyst_0

  # Create instance: v_frmbuf_wr_0, and set properties
  set v_frmbuf_wr_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:v_frmbuf_wr v_frmbuf_wr_0 ]
  set_property -dict [ list \
   CONFIG.AXIMM_DATA_WIDTH {128} \
   CONFIG.C_M_AXI_MM_VIDEO_DATA_WIDTH {128} \
   CONFIG.HAS_BGR8 {0} \
   CONFIG.HAS_BGRX8 {0} \
   CONFIG.HAS_RGB8 {0} \
   CONFIG.HAS_UYVY8 {0} \
   CONFIG.HAS_YUV8 {0} \
   CONFIG.HAS_Y_UV8 {0} \
   CONFIG.HAS_Y_UV8_420 {1} \
   CONFIG.MAX_COLS {3840} \
   CONFIG.MAX_NR_PLANES {2} \
   CONFIG.MAX_ROWS {2160} \
   CONFIG.SAMPLES_PER_CLOCK {2} \
 ] $v_frmbuf_wr_0

  # Create instance: xlslice_0, and set properties
  set xlslice_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlslice xlslice_0 ]
  set_property -dict [ list \
   CONFIG.DIN_FROM {0} \
   CONFIG.DIN_TO {0} \
   CONFIG.DIN_WIDTH {92} \
   CONFIG.DOUT_WIDTH {1} \
 ] $xlslice_0

  # Create interface connections
  connect_bd_intf_net -intf_net axi_interconnect_0_M00_AXI [get_bd_intf_pins csirxss_s_axi] [get_bd_intf_pins mipi_csi2_rx_subsyst_0/csirxss_s_axi]
  connect_bd_intf_net -intf_net axi_interconnect_0_M11_AXI [get_bd_intf_pins s_axi_ctrl_frmbuf] [get_bd_intf_pins v_frmbuf_wr_0/s_axi_CTRL]
  connect_bd_intf_net -intf_net axis_data_fifo_0_M_AXIS [get_bd_intf_pins axis_data_fifo_cap/M_AXIS] [get_bd_intf_pins axis_subset_converter_0/S_AXIS]
  connect_bd_intf_net -intf_net axis_subset_converter_0_M_AXIS [get_bd_intf_pins axis_subset_converter_0/M_AXIS] [get_bd_intf_pins v_frmbuf_wr_0/s_axis_video]
  connect_bd_intf_net -intf_net mipi_csi2_rx_subsyst_0_video_out [get_bd_intf_pins axis_data_fifo_cap/S_AXIS] [get_bd_intf_pins mipi_csi2_rx_subsyst_0/video_out]
  connect_bd_intf_net -intf_net mipi_phy_if_1 [get_bd_intf_pins mipi_phy_if] [get_bd_intf_pins mipi_csi2_rx_subsyst_0/mipi_phy_if]
  connect_bd_intf_net -intf_net v_frmbuf_wr_1_m_axi_mm_video [get_bd_intf_pins m_axi_mm_video] [get_bd_intf_pins v_frmbuf_wr_0/m_axi_mm_video]

  # Create port connections
  connect_bd_net -net Din_1 [get_bd_pins Din] [get_bd_pins xlslice_0/Din]
  connect_bd_net -net clk_wiz_0_clk_100M [get_bd_pins lite_aclk] [get_bd_pins mipi_csi2_rx_subsyst_0/lite_aclk]
  connect_bd_net -net clk_wiz_0_clk_200M [get_bd_pins dphy_clk_200M] [get_bd_pins mipi_csi2_rx_subsyst_0/dphy_clk_200M]
  connect_bd_net -net clk_wiz_0_clk_300M [get_bd_pins aclk] [get_bd_pins axis_data_fifo_cap/s_axis_aclk] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_300M [get_bd_pins aclk] [get_bd_pins axis_subset_converter_0/aclk] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_300M [get_bd_pins aclk] [get_bd_pins mipi_csi2_rx_subsyst_0/video_aclk] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_300M [get_bd_pins aclk] [get_bd_pins v_frmbuf_wr_0/ap_clk] -boundary_type upper
  connect_bd_net -net mipi_csi2_rx_subsyst_0_csirxss_csi_irq [get_bd_pins csirxss_csi_irq] [get_bd_pins mipi_csi2_rx_subsyst_0/csirxss_csi_irq]
  connect_bd_net -net proc_sys_reset_1_peripheral_aresetn [get_bd_pins aresetn] [get_bd_pins axis_data_fifo_cap/s_axis_aresetn] -boundary_type upper
  connect_bd_net -net proc_sys_reset_1_peripheral_aresetn [get_bd_pins aresetn] [get_bd_pins axis_subset_converter_0/aresetn] -boundary_type upper
  connect_bd_net -net proc_sys_reset_1_peripheral_aresetn [get_bd_pins aresetn] [get_bd_pins mipi_csi2_rx_subsyst_0/video_aresetn] -boundary_type upper
  connect_bd_net -net proc_sys_reset_2_peripheral_aresetn [get_bd_pins lite_aresetn] [get_bd_pins mipi_csi2_rx_subsyst_0/lite_aresetn]
  connect_bd_net -net v_frmbuf_wr_1_interrupt [get_bd_pins interrupt] [get_bd_pins v_frmbuf_wr_0/interrupt]
  connect_bd_net -net xlslice_2_Dout [get_bd_pins v_frmbuf_wr_0/ap_rst_n] [get_bd_pins xlslice_0/Dout]

  # Restore current instance
  current_bd_instance $oldCurInst
}

# Hierarchical cell: capture_pipeline_ias
proc create_hier_cell_capture_pipeline_ias { parentCell nameHier } {

  variable script_folder

  if { $parentCell eq "" || $nameHier eq "" } {
     catch {common::send_gid_msg -ssname BD::TCL -id 2092 -severity "ERROR" "create_hier_cell_capture_pipeline_ias() - Empty argument(s)!"}
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
  create_bd_intf_pin -mode Slave -vlnv xilinx.com:interface:aximm_rtl:1.0 csirxss_s_axi

  create_bd_intf_pin -mode Master -vlnv xilinx.com:interface:aximm_rtl:1.0 m_axi_mm_video

  create_bd_intf_pin -mode Slave -vlnv xilinx.com:interface:mipi_phy_rtl:1.0 mipi_phy_if

  create_bd_intf_pin -mode Slave -vlnv xilinx.com:interface:aximm_rtl:1.0 s_axi_ctrl_dem

  create_bd_intf_pin -mode Slave -vlnv xilinx.com:interface:aximm_rtl:1.0 s_axi_ctrl_frmbuf

  create_bd_intf_pin -mode Slave -vlnv xilinx.com:interface:aximm_rtl:1.0 s_axi_ctrl_vpss


  # Create pins
  create_bd_pin -dir I -from 91 -to 0 Din
  create_bd_pin -dir I -type clk aclk
  create_bd_pin -dir I -type rst aresetn
  create_bd_pin -dir O -type clk clkoutphy_out
  create_bd_pin -dir O -type intr csirxss_csi_irq
  create_bd_pin -dir O -type intr dem_irq
  create_bd_pin -dir I -type clk dphy_clk_200M
  create_bd_pin -dir O -type intr frm_buf_irq
  create_bd_pin -dir I -type clk lite_aclk
  create_bd_pin -dir I -type rst lite_aresetn
  create_bd_pin -dir O pll_lock_out

  # Create instance: axis_data_fifo_cap, and set properties
  set axis_data_fifo_cap [ create_bd_cell -type ip -vlnv xilinx.com:ip:axis_data_fifo axis_data_fifo_cap ]
  set_property -dict [ list \
   CONFIG.FIFO_DEPTH {1024} \
 ] $axis_data_fifo_cap

  # Create instance: axis_subset_converter_0, and set properties
  set axis_subset_converter_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:axis_subset_converter axis_subset_converter_0 ]
  set_property -dict [ list \
   CONFIG.M_TDATA_NUM_BYTES {2} \
   CONFIG.M_TDEST_WIDTH {1} \
   CONFIG.S_TDATA_NUM_BYTES {3} \
   CONFIG.TDATA_REMAP {tdata[19:12],tdata[9:2]} \
 ] $axis_subset_converter_0

  # Create instance: mipi_csi2_rx_subsyst_0, and set properties
  set mipi_csi2_rx_subsyst_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:mipi_csi2_rx_subsystem mipi_csi2_rx_subsyst_0 ]
  set_property -dict [ list \
   CONFIG.AXIS_TDEST_WIDTH {4} \
   CONFIG.CLK_LANE_IO_LOC {C1} \
   CONFIG.CLK_LANE_IO_LOC_NAME {IO_L7P_T1L_N0_QBC_AD13P_66} \
   CONFIG.CMN_NUM_LANES {4} \
   CONFIG.CMN_NUM_PIXELS {2} \
   CONFIG.CMN_PXL_FORMAT {RAW10} \
   CONFIG.CMN_VC {All} \
   CONFIG.CSI_BUF_DEPTH {4096} \
   CONFIG.C_CLK_LANE_IO_POSITION {13} \
   CONFIG.C_CSI_EN_ACTIVELANES {false} \
   CONFIG.C_CSI_FILTER_USERDATATYPE {true} \
   CONFIG.C_DATA_LANE0_IO_POSITION {15} \
   CONFIG.C_DATA_LANE1_IO_POSITION {17} \
   CONFIG.C_DATA_LANE2_IO_POSITION {19} \
   CONFIG.C_DATA_LANE3_IO_POSITION {21} \
   CONFIG.C_DPHY_LANES {4} \
   CONFIG.C_EN_BG0_PIN0 {false} \
   CONFIG.C_EN_CSI_V2_0 {false} \
   CONFIG.C_HS_LINE_RATE {880} \
   CONFIG.C_HS_SETTLE_NS {146} \
   CONFIG.C_STRETCH_LINE_RATE {1500} \
   CONFIG.DATA_LANE0_IO_LOC {A2} \
   CONFIG.DATA_LANE0_IO_LOC_NAME {IO_L8P_T1L_N2_AD5P_66} \
   CONFIG.DATA_LANE1_IO_LOC {B3} \
   CONFIG.DATA_LANE1_IO_LOC_NAME {IO_L9P_T1L_N4_AD12P_66} \
   CONFIG.DATA_LANE2_IO_LOC {B4} \
   CONFIG.DATA_LANE2_IO_LOC_NAME {IO_L10P_T1U_N6_QBC_AD4P_66} \
   CONFIG.DATA_LANE3_IO_LOC {D4} \
   CONFIG.DATA_LANE3_IO_LOC_NAME {IO_L11P_T1U_N8_GC_66} \
   CONFIG.DPHYRX_BOARD_INTERFACE {som240_1_connector_mipi_csi_ias} \
   CONFIG.DPY_EN_REG_IF {false} \
   CONFIG.DPY_LINE_RATE {880} \
   CONFIG.HP_IO_BANK_SELECTION {66} \
   CONFIG.SupportLevel {1} \
 ] $mipi_csi2_rx_subsyst_0

  # Create instance: v_demosaic_0, and set properties
  set v_demosaic_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:v_demosaic v_demosaic_0 ]
  set_property -dict [ list \
   CONFIG.SAMPLES_PER_CLOCK {2} \
 ] $v_demosaic_0

  # Create instance: v_frmbuf_wr_0, and set properties
  set v_frmbuf_wr_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:v_frmbuf_wr v_frmbuf_wr_0 ]
  set_property -dict [ list \
   CONFIG.AXIMM_DATA_WIDTH {128} \
   CONFIG.C_M_AXI_MM_VIDEO_DATA_WIDTH {128} \
   CONFIG.HAS_BGR8 {0} \
   CONFIG.HAS_BGRX8 {0} \
   CONFIG.HAS_RGB8 {0} \
   CONFIG.HAS_UYVY8 {0} \
   CONFIG.HAS_YUV8 {0} \
   CONFIG.HAS_Y_UV8 {0} \
   CONFIG.HAS_Y_UV8_420 {1} \
   CONFIG.MAX_COLS {3840} \
   CONFIG.MAX_NR_PLANES {2} \
   CONFIG.MAX_ROWS {2160} \
   CONFIG.SAMPLES_PER_CLOCK {2} \
 ] $v_frmbuf_wr_0

  # Create instance: v_proc_ss_0, and set properties
  set v_proc_ss_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:v_proc_ss v_proc_ss_0 ]
  set_property -dict [ list \
   CONFIG.C_COLORSPACE_SUPPORT {0} \
   CONFIG.C_ENABLE_CSC {true} \
   CONFIG.C_MAX_DATA_WIDTH {8} \
   CONFIG.C_SAMPLES_PER_CLK {2} \
   CONFIG.C_TOPOLOGY {0} \
 ] $v_proc_ss_0

  # Create instance: xlslice_1, and set properties
  set xlslice_1 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlslice xlslice_1 ]
  set_property -dict [ list \
   CONFIG.DIN_FROM {1} \
   CONFIG.DIN_TO {1} \
   CONFIG.DIN_WIDTH {92} \
   CONFIG.DOUT_WIDTH {1} \
 ] $xlslice_1

  # Create instance: xlslice_2, and set properties
  set xlslice_2 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlslice xlslice_2 ]
  set_property -dict [ list \
   CONFIG.DIN_FROM {2} \
   CONFIG.DIN_TO {2} \
   CONFIG.DIN_WIDTH {92} \
   CONFIG.DOUT_WIDTH {1} \
 ] $xlslice_2

  # Create instance: xlslice_3, and set properties
  set xlslice_3 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlslice xlslice_3 ]
  set_property -dict [ list \
   CONFIG.DIN_FROM {3} \
   CONFIG.DIN_TO {3} \
   CONFIG.DIN_WIDTH {92} \
   CONFIG.DOUT_WIDTH {1} \
 ] $xlslice_3

  # Create interface connections
  connect_bd_intf_net -intf_net Conn1 [get_bd_intf_pins s_axi_ctrl_dem] [get_bd_intf_pins v_demosaic_0/s_axi_CTRL]
  connect_bd_intf_net -intf_net Conn2 [get_bd_intf_pins s_axi_ctrl_vpss] [get_bd_intf_pins v_proc_ss_0/s_axi_ctrl]
  connect_bd_intf_net -intf_net axi_interconnect_0_M00_AXI [get_bd_intf_pins csirxss_s_axi] [get_bd_intf_pins mipi_csi2_rx_subsyst_0/csirxss_s_axi]
  connect_bd_intf_net -intf_net axi_interconnect_0_M11_AXI [get_bd_intf_pins s_axi_ctrl_frmbuf] [get_bd_intf_pins v_frmbuf_wr_0/s_axi_CTRL]
  connect_bd_intf_net -intf_net axis_data_fifo_cap_M_AXIS [get_bd_intf_pins axis_data_fifo_cap/M_AXIS] [get_bd_intf_pins axis_subset_converter_0/S_AXIS]
  connect_bd_intf_net -intf_net axis_subset_converter_0_M_AXIS [get_bd_intf_pins axis_subset_converter_0/M_AXIS] [get_bd_intf_pins v_demosaic_0/s_axis_video]
  connect_bd_intf_net -intf_net mipi_csi2_rx_subsyst_0_video_out [get_bd_intf_pins axis_data_fifo_cap/S_AXIS] [get_bd_intf_pins mipi_csi2_rx_subsyst_0/video_out]
  connect_bd_intf_net -intf_net mipi_phy_if_1 [get_bd_intf_pins mipi_phy_if] [get_bd_intf_pins mipi_csi2_rx_subsyst_0/mipi_phy_if]
  connect_bd_intf_net -intf_net v_demosaic_0_m_axis_video [get_bd_intf_pins v_demosaic_0/m_axis_video] [get_bd_intf_pins v_proc_ss_0/s_axis]
  connect_bd_intf_net -intf_net v_frmbuf_wr_1_m_axi_mm_video [get_bd_intf_pins m_axi_mm_video] [get_bd_intf_pins v_frmbuf_wr_0/m_axi_mm_video]
  connect_bd_intf_net -intf_net v_proc_ss_0_m_axis [get_bd_intf_pins v_frmbuf_wr_0/s_axis_video] [get_bd_intf_pins v_proc_ss_0/m_axis]

  # Create port connections
  connect_bd_net -net Din_1 [get_bd_pins Din] [get_bd_pins xlslice_1/Din] -boundary_type upper
  connect_bd_net -net Din_1 [get_bd_pins Din] [get_bd_pins xlslice_2/Din] -boundary_type upper
  connect_bd_net -net Din_1 [get_bd_pins Din] [get_bd_pins xlslice_3/Din] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_100M [get_bd_pins lite_aclk] [get_bd_pins mipi_csi2_rx_subsyst_0/lite_aclk]
  connect_bd_net -net clk_wiz_0_clk_200M [get_bd_pins dphy_clk_200M] [get_bd_pins mipi_csi2_rx_subsyst_0/dphy_clk_200M]
  connect_bd_net -net clk_wiz_0_clk_300M [get_bd_pins aclk] [get_bd_pins axis_data_fifo_cap/s_axis_aclk] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_300M [get_bd_pins aclk] [get_bd_pins axis_subset_converter_0/aclk] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_300M [get_bd_pins aclk] [get_bd_pins mipi_csi2_rx_subsyst_0/video_aclk] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_300M [get_bd_pins aclk] [get_bd_pins v_demosaic_0/ap_clk] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_300M [get_bd_pins aclk] [get_bd_pins v_frmbuf_wr_0/ap_clk] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_300M [get_bd_pins aclk] [get_bd_pins v_proc_ss_0/aclk_axis] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_300M [get_bd_pins aclk] [get_bd_pins v_proc_ss_0/aclk_ctrl] -boundary_type upper
  connect_bd_net -net mipi_csi2_rx_subsyst_0_clkoutphy_out [get_bd_pins clkoutphy_out] [get_bd_pins mipi_csi2_rx_subsyst_0/clkoutphy_out]
  connect_bd_net -net mipi_csi2_rx_subsyst_0_csirxss_csi_irq [get_bd_pins csirxss_csi_irq] [get_bd_pins mipi_csi2_rx_subsyst_0/csirxss_csi_irq]
  connect_bd_net -net mipi_csi2_rx_subsyst_0_pll_lock_out [get_bd_pins pll_lock_out] [get_bd_pins mipi_csi2_rx_subsyst_0/pll_lock_out]
  connect_bd_net -net proc_sys_reset_1_peripheral_aresetn [get_bd_pins aresetn] [get_bd_pins axis_data_fifo_cap/s_axis_aresetn] -boundary_type upper
  connect_bd_net -net proc_sys_reset_1_peripheral_aresetn [get_bd_pins aresetn] [get_bd_pins axis_subset_converter_0/aresetn] -boundary_type upper
  connect_bd_net -net proc_sys_reset_1_peripheral_aresetn [get_bd_pins aresetn] [get_bd_pins mipi_csi2_rx_subsyst_0/video_aresetn] -boundary_type upper
  connect_bd_net -net proc_sys_reset_2_peripheral_aresetn [get_bd_pins lite_aresetn] [get_bd_pins mipi_csi2_rx_subsyst_0/lite_aresetn]
  connect_bd_net -net v_demosaic_0_interrupt [get_bd_pins dem_irq] [get_bd_pins v_demosaic_0/interrupt]
  connect_bd_net -net v_frmbuf_wr_1_interrupt [get_bd_pins frm_buf_irq] [get_bd_pins v_frmbuf_wr_0/interrupt]
  connect_bd_net -net xlslice_1_Dout [get_bd_pins v_proc_ss_0/aresetn_ctrl] [get_bd_pins xlslice_2/Dout]
  connect_bd_net -net xlslice_2_Dout [get_bd_pins v_frmbuf_wr_0/ap_rst_n] [get_bd_pins xlslice_3/Dout]
  connect_bd_net -net xlslice_2_Dout1 [get_bd_pins v_demosaic_0/ap_rst_n] [get_bd_pins xlslice_1/Dout]

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

  set PMOD_GPIO [ create_bd_intf_port -mode Master -vlnv xilinx.com:interface:gpio_rtl:1.0 PMOD_GPIO ]

  set iic [ create_bd_intf_port -mode Master -vlnv xilinx.com:interface:iic_rtl:1.0 iic ]

  set mipi_phy_if_ias [ create_bd_intf_port -mode Slave -vlnv xilinx.com:interface:mipi_phy_rtl:1.0 mipi_phy_if_ias ]

  set mipi_phy_if_isp [ create_bd_intf_port -mode Slave -vlnv xilinx.com:interface:mipi_phy_rtl:1.0 mipi_phy_if_isp ]

  set mipi_phy_if_raspi [ create_bd_intf_port -mode Slave -vlnv xilinx.com:interface:mipi_phy_rtl:1.0 mipi_phy_if_raspi ]


  # Create ports
  set fan_en_b [ create_bd_port -dir O -from 0 -to 0 fan_en_b ]

  # Create instance: axi_gpio_0, and set properties
  set axi_gpio_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:axi_gpio axi_gpio_0 ]
  set_property -dict [ list \
   CONFIG.C_ALL_OUTPUTS {1} \
   CONFIG.C_DOUT_DEFAULT {0x0000000D} \
   CONFIG.C_GPIO_WIDTH {4} \
 ] $axi_gpio_0


  # Create instance: axi_gpio_1, and set properties
  set axi_gpio_1 [ create_bd_cell -type ip -vlnv xilinx.com:ip:axi_gpio axi_gpio_1 ]
  set_property -dict [ list \
   CONFIG.C_ALL_INPUTS {0} \
    CONFIG.C_GPIO_WIDTH {8} \
   CONFIG.C_IS_DUAL {0} \
 ] $axi_gpio_1

  # Create instance: axi_iic_0, and set properties
  set axi_iic_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:axi_iic axi_iic_0 ]
  set_property -dict [ list \
   CONFIG.IIC_BOARD_INTERFACE {Custom} \
   CONFIG.IIC_FREQ_KHZ {400} \
 ] $axi_iic_0

  # Create instance: axi_interconnect_ctrl_100, and set properties
  set axi_interconnect_ctrl_100 [ create_bd_cell -type ip -vlnv xilinx.com:ip:axi_interconnect axi_interconnect_ctrl_100 ]
  set_property -dict [ list \
   CONFIG.ENABLE_ADVANCED_OPTIONS {1} \
   CONFIG.NUM_MI {7} \
 ] $axi_interconnect_ctrl_100

  # Create instance: axi_interconnect_ctrl_300, and set properties
  set axi_interconnect_ctrl_300 [ create_bd_cell -type ip -vlnv xilinx.com:ip:axi_interconnect axi_interconnect_ctrl_300 ]
  set_property -dict [ list \
   CONFIG.NUM_MI {7} \
   CONFIG.NUM_SI {1} \
 ] $axi_interconnect_ctrl_300

  # Create instance: capture_pipeline_ias
  create_hier_cell_capture_pipeline_ias [current_bd_instance .] capture_pipeline_ias

  # Create instance: capture_pipeline_isp
  create_hier_cell_capture_pipeline_isp [current_bd_instance .] capture_pipeline_isp

  # Create instance: capture_pipeline_raspi
  create_hier_cell_capture_pipeline_raspi [current_bd_instance .] capture_pipeline_raspi

  # Create instance: clk_wiz_0, and set properties
  set clk_wiz_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:clk_wiz clk_wiz_0 ]
  set_property -dict [ list \
   CONFIG.CLKOUT1_JITTER {102.087} \
   CONFIG.CLKOUT1_PHASE_ERROR {87.181} \
   CONFIG.CLKOUT1_REQUESTED_OUT_FREQ {200.000} \
   CONFIG.CLKOUT2_JITTER {115.833} \
   CONFIG.CLKOUT2_PHASE_ERROR {87.181} \
   CONFIG.CLKOUT2_USED {true} \
   CONFIG.CLKOUT3_JITTER {94.863} \
   CONFIG.CLKOUT3_PHASE_ERROR {87.181} \
   CONFIG.CLKOUT3_REQUESTED_OUT_FREQ {300.000} \
   CONFIG.CLKOUT3_USED {true} \
   CONFIG.CLKOUT4_JITTER {132.685} \
   CONFIG.CLKOUT4_PHASE_ERROR {87.181} \
   CONFIG.CLKOUT4_REQUESTED_OUT_FREQ {50.000} \
   CONFIG.CLKOUT4_USED {true} \
   CONFIG.CLKOUT5_JITTER {102.087} \
   CONFIG.CLKOUT5_PHASE_ERROR {87.181} \
   CONFIG.CLKOUT5_REQUESTED_OUT_FREQ {100.000} \
   CONFIG.CLKOUT5_USED {false} \
   CONFIG.CLK_OUT1_PORT {clk_200M} \
   CONFIG.CLK_OUT2_PORT {clk_100M} \
   CONFIG.CLK_OUT3_PORT {clk_300M} \
   CONFIG.CLK_OUT4_PORT {clk_50M} \
   CONFIG.CLK_OUT5_PORT {clk_out5} \
   CONFIG.MMCM_CLKOUT0_DIVIDE_F {6.000} \
   CONFIG.MMCM_CLKOUT1_DIVIDE {12} \
   CONFIG.MMCM_CLKOUT2_DIVIDE {4} \
   CONFIG.MMCM_CLKOUT3_DIVIDE {24} \
   CONFIG.MMCM_CLKOUT4_DIVIDE {1} \
   CONFIG.MMCM_DIVCLK_DIVIDE {1} \
   CONFIG.NUM_OUT_CLKS {4} \
   CONFIG.PRIM_SOURCE {Global_buffer} \
   CONFIG.RESET_PORT {resetn} \
   CONFIG.RESET_TYPE {ACTIVE_LOW} \
 ] $clk_wiz_0

  # Create instance: proc_sys_reset_100MHz, and set properties
  set proc_sys_reset_100MHz [ create_bd_cell -type ip -vlnv xilinx.com:ip:proc_sys_reset proc_sys_reset_100MHz ]

  # Create instance: proc_sys_reset_300MHz, and set properties
  set proc_sys_reset_300MHz [ create_bd_cell -type ip -vlnv xilinx.com:ip:proc_sys_reset proc_sys_reset_300MHz ]

  # Create instance: smartconnect_cap, and set properties
  set smartconnect_cap [ create_bd_cell -type ip -vlnv xilinx.com:ip:smartconnect smartconnect_cap ]
  set_property -dict [ list \
   CONFIG.NUM_CLKS {1} \
   CONFIG.NUM_SI {3} \
 ] $smartconnect_cap

  # Create instance: vcu
  create_hier_cell_vcu [current_bd_instance .] vcu

  # Create instance: xlconcat_irq0, and set properties
  set xlconcat_irq0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlconcat xlconcat_irq0 ]
  set_property -dict [ list \
   CONFIG.NUM_PORTS {8} \
 ] $xlconcat_irq0

  # Create instance: xlconcat_irq1, and set properties
  set xlconcat_irq1 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlconcat xlconcat_irq1 ]
  set_property -dict [ list \
   CONFIG.NUM_PORTS {8} \
 ] $xlconcat_irq1

  # Create instance: xlslice_7, and set properties
  set xlslice_7 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlslice xlslice_7 ]
  set_property -dict [ list \
   CONFIG.DIN_FROM {7} \
   CONFIG.DIN_TO {7} \
   CONFIG.DIN_WIDTH {92} \
   CONFIG.DOUT_WIDTH {1} \
 ] $xlslice_7

  # Create instance: xlslice_fan, and set properties
  set xlslice_fan [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlslice xlslice_fan ]
  set_property -dict [ list \
   CONFIG.DIN_FROM {2} \
   CONFIG.DIN_TO {2} \
   CONFIG.DIN_WIDTH {3} \
   CONFIG.DOUT_WIDTH {1} \
 ] $xlslice_fan

  # Create interface connections
  connect_bd_intf_net -intf_net PS_0_M_AXI_HPM1_FPD [get_bd_intf_pins PS_0/M_AXI_HPM1_FPD] [get_bd_intf_pins axi_interconnect_ctrl_300/S00_AXI]
  connect_bd_intf_net -intf_net S00_AXI_1 [get_bd_intf_pins PS_0/M_AXI_HPM0_LPD] [get_bd_intf_pins axi_interconnect_ctrl_100/S00_AXI]
  connect_bd_intf_net -intf_net axi_gpio_0_GPIO [get_bd_intf_ports GPIO] [get_bd_intf_pins axi_gpio_0/GPIO]
  connect_bd_intf_net -intf_net axi_gpio_pmod_in_GPIO [get_bd_intf_ports PMOD_GPIO] [get_bd_intf_pins axi_gpio_1/GPIO]
  connect_bd_intf_net -intf_net axi_iic_0_IIC [get_bd_intf_ports iic] [get_bd_intf_pins axi_iic_0/IIC]
  connect_bd_intf_net -intf_net axi_interconnect_0_M00_AXI [get_bd_intf_pins axi_interconnect_ctrl_100/M00_AXI] [get_bd_intf_pins capture_pipeline_isp/csirxss_s_axi]
  connect_bd_intf_net -intf_net axi_interconnect_0_M00_AXI1 [get_bd_intf_pins axi_interconnect_ctrl_300/M00_AXI] [get_bd_intf_pins capture_pipeline_isp/s_axi_ctrl_frmbuf]
  connect_bd_intf_net -intf_net axi_interconnect_ctrl_100_M01_AXI [get_bd_intf_pins axi_interconnect_ctrl_100/M01_AXI] [get_bd_intf_pins vcu/S_AXI_LITE]
  connect_bd_intf_net -intf_net axi_interconnect_ctrl_100_M02_AXI [get_bd_intf_pins axi_gpio_1/S_AXI] [get_bd_intf_pins axi_interconnect_ctrl_100/M02_AXI]
  connect_bd_intf_net -intf_net axi_interconnect_ctrl_100_M04_AXI [get_bd_intf_pins axi_iic_0/S_AXI] [get_bd_intf_pins axi_interconnect_ctrl_100/M04_AXI]
  connect_bd_intf_net -intf_net axi_interconnect_ctrl_100_M05_AXI [get_bd_intf_pins axi_gpio_0/S_AXI] [get_bd_intf_pins axi_interconnect_ctrl_100/M05_AXI]
  connect_bd_intf_net -intf_net axi_interconnect_ctrl_100_M06_AXI [get_bd_intf_pins axi_interconnect_ctrl_100/M06_AXI] [get_bd_intf_pins capture_pipeline_ias/csirxss_s_axi]
  connect_bd_intf_net -intf_net axi_interconnect_ctrl_300_M01_AXI [get_bd_intf_pins axi_interconnect_ctrl_300/M01_AXI] [get_bd_intf_pins capture_pipeline_ias/s_axi_ctrl_frmbuf]
  connect_bd_intf_net -intf_net axi_interconnect_ctrl_300_M02_AXI [get_bd_intf_pins axi_interconnect_ctrl_300/M02_AXI] [get_bd_intf_pins capture_pipeline_ias/s_axi_ctrl_dem]
  connect_bd_intf_net -intf_net axi_interconnect_ctrl_300_M03_AXI [get_bd_intf_pins axi_interconnect_ctrl_300/M03_AXI] [get_bd_intf_pins capture_pipeline_ias/s_axi_ctrl_vpss]
  connect_bd_intf_net -intf_net capture_pipeline_ias_m_axi_mm_video [get_bd_intf_pins capture_pipeline_ias/m_axi_mm_video] [get_bd_intf_pins smartconnect_cap/S01_AXI]
  connect_bd_intf_net -intf_net capture_pipeline_m_axi_mm_video [get_bd_intf_pins capture_pipeline_isp/m_axi_mm_video] [get_bd_intf_pins smartconnect_cap/S00_AXI]
  connect_bd_intf_net -intf_net capture_pipeline_raspi_m_axi_mm_video [get_bd_intf_pins capture_pipeline_raspi/m_axi_mm_video] [get_bd_intf_pins smartconnect_cap/S02_AXI]
  connect_bd_intf_net -intf_net csirxss_s_axi_1 [get_bd_intf_pins axi_interconnect_ctrl_100/M03_AXI] [get_bd_intf_pins capture_pipeline_raspi/csirxss_s_axi]
  connect_bd_intf_net -intf_net mipi_phy_if_0_1 [get_bd_intf_ports mipi_phy_if_ias] [get_bd_intf_pins capture_pipeline_ias/mipi_phy_if]
  connect_bd_intf_net -intf_net mipi_phy_if_0_2 [get_bd_intf_ports mipi_phy_if_raspi] [get_bd_intf_pins capture_pipeline_raspi/mipi_phy_if]
  connect_bd_intf_net -intf_net mipi_phy_if_1 [get_bd_intf_ports mipi_phy_if_isp] [get_bd_intf_pins capture_pipeline_isp/mipi_phy_if]
  connect_bd_intf_net -intf_net s_axi_ctrl_dem_1 [get_bd_intf_pins axi_interconnect_ctrl_300/M05_AXI] [get_bd_intf_pins capture_pipeline_raspi/s_axi_ctrl_dem]
  connect_bd_intf_net -intf_net s_axi_ctrl_frmbuf_1 [get_bd_intf_pins axi_interconnect_ctrl_300/M04_AXI] [get_bd_intf_pins capture_pipeline_raspi/s_axi_ctrl_frmbuf]
  connect_bd_intf_net -intf_net s_axi_ctrl_vpss_1 [get_bd_intf_pins axi_interconnect_ctrl_300/M06_AXI] [get_bd_intf_pins capture_pipeline_raspi/s_axi_ctrl_vpss]
  connect_bd_intf_net -intf_net smartconnect_cap_M00_AXI [get_bd_intf_pins PS_0/S_AXI_HP0_FPD] [get_bd_intf_pins smartconnect_cap/M00_AXI]
  connect_bd_intf_net -intf_net vcu_M00_AXI [get_bd_intf_pins PS_0/S_AXI_HP2_FPD] [get_bd_intf_pins vcu/M00_AXI_VCU_DEC]
  connect_bd_intf_net -intf_net vcu_M00_AXI_VCU_EN [get_bd_intf_pins PS_0/S_AXI_HP1_FPD] [get_bd_intf_pins vcu/M00_AXI_VCU_EN]
  connect_bd_intf_net -intf_net vcu_M_AXI_VCU_MCU [get_bd_intf_pins PS_0/S_AXI_LPD] [get_bd_intf_pins vcu/M_AXI_VCU_MCU]

  # Create port connections
  connect_bd_net -net PS_0_emio_gpio_o [get_bd_pins PS_0/emio_gpio_o] [get_bd_pins capture_pipeline_ias/Din] -boundary_type upper
  connect_bd_net -net PS_0_emio_gpio_o [get_bd_pins PS_0/emio_gpio_o] [get_bd_pins capture_pipeline_isp/Din] -boundary_type upper
  connect_bd_net -net PS_0_emio_gpio_o [get_bd_pins PS_0/emio_gpio_o] [get_bd_pins capture_pipeline_raspi/Din] -boundary_type upper
  connect_bd_net -net PS_0_emio_gpio_o [get_bd_pins PS_0/emio_gpio_o] [get_bd_pins xlslice_7/Din] -boundary_type upper
  connect_bd_net -net PS_0_emio_ttc0_wave_o [get_bd_pins PS_0/emio_ttc0_wave_o] [get_bd_pins xlslice_fan/Din]
  connect_bd_net -net PS_0_pl_clk0 [get_bd_pins PS_0/pl_clk0] [get_bd_pins clk_wiz_0/clk_in1]
  connect_bd_net -net PS_0_pl_resetn0 [get_bd_pins PS_0/pl_resetn0] [get_bd_pins clk_wiz_0/resetn] -boundary_type upper
  connect_bd_net -net PS_0_pl_resetn0 [get_bd_pins PS_0/pl_resetn0] [get_bd_pins proc_sys_reset_100MHz/ext_reset_in] -boundary_type upper
  connect_bd_net -net PS_0_pl_resetn0 [get_bd_pins PS_0/pl_resetn0] [get_bd_pins proc_sys_reset_300MHz/ext_reset_in] -boundary_type upper
  connect_bd_net -net axi_iic_0_iic2intc_irpt [get_bd_pins axi_iic_0/iic2intc_irpt] [get_bd_pins xlconcat_irq1/In3]
  connect_bd_net -net capture_pipeline_ias_clkoutphy_out [get_bd_pins capture_pipeline_ias/clkoutphy_out] [get_bd_pins capture_pipeline_raspi/clkoutphy_in]
  connect_bd_net -net capture_pipeline_ias_csirxss_csi_irq [get_bd_pins capture_pipeline_ias/csirxss_csi_irq] [get_bd_pins xlconcat_irq0/In0]
  connect_bd_net -net capture_pipeline_ias_interrupt [get_bd_pins capture_pipeline_ias/frm_buf_irq] [get_bd_pins xlconcat_irq0/In1]
  connect_bd_net -net capture_pipeline_ias_interrupt1 [get_bd_pins capture_pipeline_ias/dem_irq] [get_bd_pins xlconcat_irq0/In2]
  connect_bd_net -net capture_pipeline_ias_pll_lock_out [get_bd_pins capture_pipeline_ias/pll_lock_out] [get_bd_pins capture_pipeline_raspi/pll_lock_in]
  connect_bd_net -net capture_pipeline_interrupt [get_bd_pins capture_pipeline_isp/interrupt] [get_bd_pins xlconcat_irq1/In1]
  connect_bd_net -net capture_pipeline_raspi_csirxss_csi_irq [get_bd_pins capture_pipeline_raspi/csirxss_csi_irq] [get_bd_pins xlconcat_irq0/In3]
  connect_bd_net -net capture_pipeline_raspi_dem_irq [get_bd_pins capture_pipeline_raspi/dem_irq] [get_bd_pins xlconcat_irq0/In5]
  connect_bd_net -net capture_pipeline_raspi_frm_buf_irq [get_bd_pins capture_pipeline_raspi/frm_buf_irq] [get_bd_pins xlconcat_irq0/In4]
  connect_bd_net -net clk_wiz_0_clk_100M [get_bd_pins PS_0/maxihpm0_lpd_aclk] [get_bd_pins axi_gpio_0/s_axi_aclk] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_100M [get_bd_pins PS_0/maxihpm0_lpd_aclk] [get_bd_pins axi_gpio_1/s_axi_aclk] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_100M [get_bd_pins PS_0/maxihpm0_lpd_aclk] [get_bd_pins axi_iic_0/s_axi_aclk] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_100M [get_bd_pins PS_0/maxihpm0_lpd_aclk] [get_bd_pins axi_interconnect_ctrl_100/ACLK] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_100M [get_bd_pins PS_0/maxihpm0_lpd_aclk] [get_bd_pins axi_interconnect_ctrl_100/M00_ACLK] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_100M [get_bd_pins PS_0/maxihpm0_lpd_aclk] [get_bd_pins axi_interconnect_ctrl_100/M01_ACLK] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_100M [get_bd_pins PS_0/maxihpm0_lpd_aclk] [get_bd_pins axi_interconnect_ctrl_100/M02_ACLK] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_100M [get_bd_pins PS_0/maxihpm0_lpd_aclk] [get_bd_pins axi_interconnect_ctrl_100/M03_ACLK] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_100M [get_bd_pins PS_0/maxihpm0_lpd_aclk] [get_bd_pins axi_interconnect_ctrl_100/M04_ACLK] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_100M [get_bd_pins PS_0/maxihpm0_lpd_aclk] [get_bd_pins axi_interconnect_ctrl_100/M05_ACLK] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_100M [get_bd_pins PS_0/maxihpm0_lpd_aclk] [get_bd_pins axi_interconnect_ctrl_100/M06_ACLK] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_100M [get_bd_pins PS_0/maxihpm0_lpd_aclk] [get_bd_pins axi_interconnect_ctrl_100/S00_ACLK] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_100M [get_bd_pins PS_0/maxihpm0_lpd_aclk] [get_bd_pins capture_pipeline_ias/lite_aclk] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_100M [get_bd_pins PS_0/maxihpm0_lpd_aclk] [get_bd_pins capture_pipeline_isp/lite_aclk] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_100M [get_bd_pins PS_0/maxihpm0_lpd_aclk] [get_bd_pins capture_pipeline_raspi/lite_aclk] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_100M [get_bd_pins PS_0/maxihpm0_lpd_aclk] [get_bd_pins clk_wiz_0/clk_100M] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_100M [get_bd_pins PS_0/maxihpm0_lpd_aclk] [get_bd_pins proc_sys_reset_100MHz/slowest_sync_clk] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_100M [get_bd_pins PS_0/maxihpm0_lpd_aclk] [get_bd_pins vcu/s_axi_lite_aclk] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_200M [get_bd_pins capture_pipeline_ias/dphy_clk_200M] [get_bd_pins capture_pipeline_isp/dphy_clk_200M] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_200M [get_bd_pins capture_pipeline_ias/dphy_clk_200M] [get_bd_pins capture_pipeline_raspi/dphy_clk_200M] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_200M [get_bd_pins capture_pipeline_ias/dphy_clk_200M] [get_bd_pins clk_wiz_0/clk_200M] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_300M [get_bd_pins PS_0/maxihpm0_fpd_aclk] [get_bd_pins PS_0/maxihpm1_fpd_aclk] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_300M [get_bd_pins PS_0/maxihpm0_fpd_aclk] [get_bd_pins PS_0/saxi_lpd_aclk] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_300M [get_bd_pins PS_0/maxihpm0_fpd_aclk] [get_bd_pins PS_0/saxihp0_fpd_aclk] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_300M [get_bd_pins PS_0/maxihpm0_fpd_aclk] [get_bd_pins PS_0/saxihp1_fpd_aclk] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_300M [get_bd_pins PS_0/maxihpm0_fpd_aclk] [get_bd_pins PS_0/saxihp2_fpd_aclk] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_300M [get_bd_pins PS_0/maxihpm0_fpd_aclk] [get_bd_pins PS_0/saxihp3_fpd_aclk] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_300M [get_bd_pins PS_0/maxihpm0_fpd_aclk] [get_bd_pins PS_0/saxihpc0_fpd_aclk] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_300M [get_bd_pins PS_0/maxihpm0_fpd_aclk] [get_bd_pins PS_0/saxihpc1_fpd_aclk] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_300M [get_bd_pins PS_0/maxihpm0_fpd_aclk] [get_bd_pins axi_interconnect_ctrl_300/ACLK] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_300M [get_bd_pins PS_0/maxihpm0_fpd_aclk] [get_bd_pins axi_interconnect_ctrl_300/M00_ACLK] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_300M [get_bd_pins PS_0/maxihpm0_fpd_aclk] [get_bd_pins axi_interconnect_ctrl_300/M01_ACLK] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_300M [get_bd_pins PS_0/maxihpm0_fpd_aclk] [get_bd_pins axi_interconnect_ctrl_300/M02_ACLK] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_300M [get_bd_pins PS_0/maxihpm0_fpd_aclk] [get_bd_pins axi_interconnect_ctrl_300/M03_ACLK] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_300M [get_bd_pins PS_0/maxihpm0_fpd_aclk] [get_bd_pins axi_interconnect_ctrl_300/M04_ACLK] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_300M [get_bd_pins PS_0/maxihpm0_fpd_aclk] [get_bd_pins axi_interconnect_ctrl_300/M05_ACLK] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_300M [get_bd_pins PS_0/maxihpm0_fpd_aclk] [get_bd_pins axi_interconnect_ctrl_300/M06_ACLK] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_300M [get_bd_pins PS_0/maxihpm0_fpd_aclk] [get_bd_pins axi_interconnect_ctrl_300/S00_ACLK] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_300M [get_bd_pins PS_0/maxihpm0_fpd_aclk] [get_bd_pins capture_pipeline_ias/aclk] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_300M [get_bd_pins PS_0/maxihpm0_fpd_aclk] [get_bd_pins capture_pipeline_isp/aclk] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_300M [get_bd_pins PS_0/maxihpm0_fpd_aclk] [get_bd_pins capture_pipeline_raspi/aclk] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_300M [get_bd_pins PS_0/maxihpm0_fpd_aclk] [get_bd_pins clk_wiz_0/clk_300M] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_300M [get_bd_pins PS_0/maxihpm0_fpd_aclk] [get_bd_pins proc_sys_reset_300MHz/slowest_sync_clk] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_300M [get_bd_pins PS_0/maxihpm0_fpd_aclk] [get_bd_pins smartconnect_cap/aclk] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_300M [get_bd_pins PS_0/maxihpm0_fpd_aclk] [get_bd_pins vcu/m_axi_dec_aclk] -boundary_type upper
  connect_bd_net -net clk_wiz_0_clk_50M [get_bd_pins clk_wiz_0/clk_50M] [get_bd_pins vcu/pll_ref_clk]
  connect_bd_net -net mipi_csi2_rx_subsyst_0_csirxss_csi_irq [get_bd_pins capture_pipeline_isp/csirxss_csi_irq] [get_bd_pins xlconcat_irq1/In0]
  connect_bd_net -net proc_sys_reset_1_interconnect_aresetn [get_bd_pins axi_interconnect_ctrl_300/ARESETN] [get_bd_pins proc_sys_reset_300MHz/interconnect_aresetn]
  connect_bd_net -net proc_sys_reset_1_peripheral_aresetn [get_bd_pins axi_interconnect_ctrl_300/M00_ARESETN] [get_bd_pins axi_interconnect_ctrl_300/M01_ARESETN] -boundary_type upper
  connect_bd_net -net proc_sys_reset_1_peripheral_aresetn [get_bd_pins axi_interconnect_ctrl_300/M00_ARESETN] [get_bd_pins axi_interconnect_ctrl_300/M02_ARESETN] -boundary_type upper
  connect_bd_net -net proc_sys_reset_1_peripheral_aresetn [get_bd_pins axi_interconnect_ctrl_300/M00_ARESETN] [get_bd_pins axi_interconnect_ctrl_300/M03_ARESETN] -boundary_type upper
  connect_bd_net -net proc_sys_reset_1_peripheral_aresetn [get_bd_pins axi_interconnect_ctrl_300/M00_ARESETN] [get_bd_pins axi_interconnect_ctrl_300/M04_ARESETN] -boundary_type upper
  connect_bd_net -net proc_sys_reset_1_peripheral_aresetn [get_bd_pins axi_interconnect_ctrl_300/M00_ARESETN] [get_bd_pins axi_interconnect_ctrl_300/M05_ARESETN] -boundary_type upper
  connect_bd_net -net proc_sys_reset_1_peripheral_aresetn [get_bd_pins axi_interconnect_ctrl_300/M00_ARESETN] [get_bd_pins axi_interconnect_ctrl_300/M06_ARESETN] -boundary_type upper
  connect_bd_net -net proc_sys_reset_1_peripheral_aresetn [get_bd_pins axi_interconnect_ctrl_300/M00_ARESETN] [get_bd_pins axi_interconnect_ctrl_300/S00_ARESETN] -boundary_type upper
  connect_bd_net -net proc_sys_reset_1_peripheral_aresetn [get_bd_pins axi_interconnect_ctrl_300/M00_ARESETN] [get_bd_pins capture_pipeline_ias/aresetn] -boundary_type upper
  connect_bd_net -net proc_sys_reset_1_peripheral_aresetn [get_bd_pins axi_interconnect_ctrl_300/M00_ARESETN] [get_bd_pins capture_pipeline_isp/aresetn] -boundary_type upper
  connect_bd_net -net proc_sys_reset_1_peripheral_aresetn [get_bd_pins axi_interconnect_ctrl_300/M00_ARESETN] [get_bd_pins capture_pipeline_raspi/aresetn] -boundary_type upper
  connect_bd_net -net proc_sys_reset_1_peripheral_aresetn [get_bd_pins axi_interconnect_ctrl_300/M00_ARESETN] [get_bd_pins proc_sys_reset_300MHz/peripheral_aresetn] -boundary_type upper
  connect_bd_net -net proc_sys_reset_1_peripheral_aresetn [get_bd_pins axi_interconnect_ctrl_300/M00_ARESETN] [get_bd_pins smartconnect_cap/aresetn] -boundary_type upper
  connect_bd_net -net proc_sys_reset_1_peripheral_aresetn [get_bd_pins axi_interconnect_ctrl_300/M00_ARESETN] [get_bd_pins vcu/aresetn] -boundary_type upper
  connect_bd_net -net proc_sys_reset_2_peripheral_aresetn [get_bd_pins axi_gpio_0/s_axi_aresetn] [get_bd_pins axi_gpio_1/s_axi_aresetn] -boundary_type upper
  connect_bd_net -net proc_sys_reset_2_peripheral_aresetn [get_bd_pins axi_gpio_0/s_axi_aresetn] [get_bd_pins axi_iic_0/s_axi_aresetn] -boundary_type upper
  connect_bd_net -net proc_sys_reset_2_peripheral_aresetn [get_bd_pins axi_gpio_0/s_axi_aresetn] [get_bd_pins axi_interconnect_ctrl_100/ARESETN] -boundary_type upper
  connect_bd_net -net proc_sys_reset_2_peripheral_aresetn [get_bd_pins axi_gpio_0/s_axi_aresetn] [get_bd_pins axi_interconnect_ctrl_100/M00_ARESETN] -boundary_type upper
  connect_bd_net -net proc_sys_reset_2_peripheral_aresetn [get_bd_pins axi_gpio_0/s_axi_aresetn] [get_bd_pins axi_interconnect_ctrl_100/M01_ARESETN] -boundary_type upper
  connect_bd_net -net proc_sys_reset_2_peripheral_aresetn [get_bd_pins axi_gpio_0/s_axi_aresetn] [get_bd_pins axi_interconnect_ctrl_100/M02_ARESETN] -boundary_type upper
  connect_bd_net -net proc_sys_reset_2_peripheral_aresetn [get_bd_pins axi_gpio_0/s_axi_aresetn] [get_bd_pins axi_interconnect_ctrl_100/M03_ARESETN] -boundary_type upper
  connect_bd_net -net proc_sys_reset_2_peripheral_aresetn [get_bd_pins axi_gpio_0/s_axi_aresetn] [get_bd_pins axi_interconnect_ctrl_100/M04_ARESETN] -boundary_type upper
  connect_bd_net -net proc_sys_reset_2_peripheral_aresetn [get_bd_pins axi_gpio_0/s_axi_aresetn] [get_bd_pins axi_interconnect_ctrl_100/M05_ARESETN] -boundary_type upper
  connect_bd_net -net proc_sys_reset_2_peripheral_aresetn [get_bd_pins axi_gpio_0/s_axi_aresetn] [get_bd_pins axi_interconnect_ctrl_100/M06_ARESETN] -boundary_type upper
  connect_bd_net -net proc_sys_reset_2_peripheral_aresetn [get_bd_pins axi_gpio_0/s_axi_aresetn] [get_bd_pins axi_interconnect_ctrl_100/S00_ARESETN] -boundary_type upper
  connect_bd_net -net proc_sys_reset_2_peripheral_aresetn [get_bd_pins axi_gpio_0/s_axi_aresetn] [get_bd_pins capture_pipeline_ias/lite_aresetn] -boundary_type upper
  connect_bd_net -net proc_sys_reset_2_peripheral_aresetn [get_bd_pins axi_gpio_0/s_axi_aresetn] [get_bd_pins capture_pipeline_isp/lite_aresetn] -boundary_type upper
  connect_bd_net -net proc_sys_reset_2_peripheral_aresetn [get_bd_pins axi_gpio_0/s_axi_aresetn] [get_bd_pins capture_pipeline_raspi/lite_aresetn] -boundary_type upper
  connect_bd_net -net proc_sys_reset_2_peripheral_aresetn [get_bd_pins axi_gpio_0/s_axi_aresetn] [get_bd_pins proc_sys_reset_100MHz/peripheral_aresetn] -boundary_type upper
  connect_bd_net -net vcu_0_vcu_host_interrupt [get_bd_pins vcu/vcu_host_interrupt] [get_bd_pins xlconcat_irq1/In2]
  connect_bd_net -net xlconcat_0_0_dout [get_bd_pins PS_0/pl_ps_irq1] [get_bd_pins xlconcat_irq1/dout]
  connect_bd_net -net xlconcat_0_1_dout [get_bd_pins PS_0/pl_ps_irq0] [get_bd_pins xlconcat_irq0/dout]
  connect_bd_net -net xlslice_0_Dout [get_bd_pins vcu/vcu_resetn] [get_bd_pins xlslice_7/Dout]
  connect_bd_net -net xlslice_fan_Dout [get_bd_ports fan_en_b] [get_bd_pins xlslice_fan/Dout]

  # Create address segments
  assign_bd_address -offset 0x80080000 -range 0x00010000 -target_address_space [get_bd_addr_spaces PS_0/Data] [get_bd_addr_segs axi_gpio_0/S_AXI/Reg] -force
  assign_bd_address -offset 0x80020000 -range 0x00010000 -target_address_space [get_bd_addr_spaces PS_0/Data] [get_bd_addr_segs axi_gpio_1/S_AXI/Reg] -force
  assign_bd_address -offset 0x80030000 -range 0x00010000 -target_address_space [get_bd_addr_spaces PS_0/Data] [get_bd_addr_segs axi_iic_0/S_AXI/Reg] -force
  assign_bd_address -offset 0x80000000 -range 0x00010000 -target_address_space [get_bd_addr_spaces PS_0/Data] [get_bd_addr_segs capture_pipeline_isp/mipi_csi2_rx_subsyst_0/csirxss_s_axi/Reg] -force
  assign_bd_address -offset 0x80010000 -range 0x00010000 -target_address_space [get_bd_addr_spaces PS_0/Data] [get_bd_addr_segs capture_pipeline_ias/mipi_csi2_rx_subsyst_0/csirxss_s_axi/Reg] -force
  assign_bd_address -offset 0x80050000 -range 0x00010000 -target_address_space [get_bd_addr_spaces PS_0/Data] [get_bd_addr_segs capture_pipeline_raspi/mipi_csi2_rx_subsyst_0/csirxss_s_axi/Reg] -force
  assign_bd_address -offset 0xB0000000 -range 0x00010000 -target_address_space [get_bd_addr_spaces PS_0/Data] [get_bd_addr_segs capture_pipeline_ias/v_demosaic_0/s_axi_CTRL/Reg] -force
  assign_bd_address -offset 0xB0030000 -range 0x00010000 -target_address_space [get_bd_addr_spaces PS_0/Data] [get_bd_addr_segs capture_pipeline_raspi/v_demosaic_0/s_axi_CTRL/Reg] -force
  assign_bd_address -offset 0xB0010000 -range 0x00010000 -target_address_space [get_bd_addr_spaces PS_0/Data] [get_bd_addr_segs capture_pipeline_isp/v_frmbuf_wr_0/s_axi_CTRL/Reg] -force
  assign_bd_address -offset 0xB0020000 -range 0x00010000 -target_address_space [get_bd_addr_spaces PS_0/Data] [get_bd_addr_segs capture_pipeline_ias/v_frmbuf_wr_0/s_axi_CTRL/Reg] -force
  assign_bd_address -offset 0xB00C0000 -range 0x00010000 -target_address_space [get_bd_addr_spaces PS_0/Data] [get_bd_addr_segs capture_pipeline_raspi/v_frmbuf_wr_0/s_axi_CTRL/Reg] -force
  assign_bd_address -offset 0xB0040000 -range 0x00040000 -target_address_space [get_bd_addr_spaces PS_0/Data] [get_bd_addr_segs capture_pipeline_ias/v_proc_ss_0/s_axi_ctrl/Reg] -force
  assign_bd_address -offset 0xB0080000 -range 0x00040000 -target_address_space [get_bd_addr_spaces PS_0/Data] [get_bd_addr_segs capture_pipeline_raspi/v_proc_ss_0/s_axi_ctrl/Reg] -force
  assign_bd_address -offset 0x80100000 -range 0x00100000 -target_address_space [get_bd_addr_spaces PS_0/Data] [get_bd_addr_segs vcu/vcu_0/S_AXI_LITE/Reg] -force
  assign_bd_address -offset 0x00000000 -range 0x80000000 -target_address_space [get_bd_addr_spaces capture_pipeline_ias/v_frmbuf_wr_0/Data_m_axi_mm_video] [get_bd_addr_segs PS_0/SAXIGP2/HP0_DDR_LOW] -force
  assign_bd_address -offset 0xC0000000 -range 0x20000000 -target_address_space [get_bd_addr_spaces capture_pipeline_ias/v_frmbuf_wr_0/Data_m_axi_mm_video] [get_bd_addr_segs PS_0/SAXIGP2/HP0_QSPI] -force
  assign_bd_address -offset 0x00000000 -range 0x80000000 -target_address_space [get_bd_addr_spaces capture_pipeline_isp/v_frmbuf_wr_0/Data_m_axi_mm_video] [get_bd_addr_segs PS_0/SAXIGP2/HP0_DDR_LOW] -force
  assign_bd_address -offset 0xC0000000 -range 0x20000000 -target_address_space [get_bd_addr_spaces capture_pipeline_isp/v_frmbuf_wr_0/Data_m_axi_mm_video] [get_bd_addr_segs PS_0/SAXIGP2/HP0_QSPI] -force
  assign_bd_address -offset 0x00000000 -range 0x80000000 -target_address_space [get_bd_addr_spaces capture_pipeline_raspi/v_frmbuf_wr_0/Data_m_axi_mm_video] [get_bd_addr_segs PS_0/SAXIGP2/HP0_DDR_LOW] -force
  assign_bd_address -offset 0xC0000000 -range 0x20000000 -target_address_space [get_bd_addr_spaces capture_pipeline_raspi/v_frmbuf_wr_0/Data_m_axi_mm_video] [get_bd_addr_segs PS_0/SAXIGP2/HP0_QSPI] -force
  assign_bd_address -offset 0x000800000000 -range 0x000800000000 -target_address_space [get_bd_addr_spaces vcu/vcu_0/Code] [get_bd_addr_segs PS_0/SAXIGP6/LPD_DDR_HIGH] -force
  assign_bd_address -offset 0x00000000 -range 0x80000000 -target_address_space [get_bd_addr_spaces vcu/vcu_0/Code] [get_bd_addr_segs PS_0/SAXIGP6/LPD_DDR_LOW] -force
  assign_bd_address -offset 0xFF000000 -range 0x01000000 -target_address_space [get_bd_addr_spaces vcu/vcu_0/Code] [get_bd_addr_segs PS_0/SAXIGP6/LPD_LPS_OCM] -force
  assign_bd_address -offset 0xC0000000 -range 0x20000000 -target_address_space [get_bd_addr_spaces vcu/vcu_0/Code] [get_bd_addr_segs PS_0/SAXIGP6/LPD_QSPI] -force
  assign_bd_address -offset 0x000800000000 -range 0x000800000000 -target_address_space [get_bd_addr_spaces vcu/vcu_0/DecData0] [get_bd_addr_segs PS_0/SAXIGP4/HP2_DDR_HIGH] -force
  assign_bd_address -offset 0x00000000 -range 0x80000000 -target_address_space [get_bd_addr_spaces vcu/vcu_0/DecData0] [get_bd_addr_segs PS_0/SAXIGP4/HP2_DDR_LOW] -force
  assign_bd_address -offset 0xFF000000 -range 0x01000000 -target_address_space [get_bd_addr_spaces vcu/vcu_0/DecData0] [get_bd_addr_segs PS_0/SAXIGP4/HP2_LPS_OCM] -force
  assign_bd_address -offset 0xC0000000 -range 0x20000000 -target_address_space [get_bd_addr_spaces vcu/vcu_0/DecData0] [get_bd_addr_segs PS_0/SAXIGP4/HP2_QSPI] -force
  assign_bd_address -offset 0x000800000000 -range 0x000800000000 -target_address_space [get_bd_addr_spaces vcu/vcu_0/DecData1] [get_bd_addr_segs PS_0/SAXIGP4/HP2_DDR_HIGH] -force
  assign_bd_address -offset 0x00000000 -range 0x80000000 -target_address_space [get_bd_addr_spaces vcu/vcu_0/DecData1] [get_bd_addr_segs PS_0/SAXIGP4/HP2_DDR_LOW] -force
  assign_bd_address -offset 0xFF000000 -range 0x01000000 -target_address_space [get_bd_addr_spaces vcu/vcu_0/DecData1] [get_bd_addr_segs PS_0/SAXIGP4/HP2_LPS_OCM] -force
  assign_bd_address -offset 0xC0000000 -range 0x20000000 -target_address_space [get_bd_addr_spaces vcu/vcu_0/DecData1] [get_bd_addr_segs PS_0/SAXIGP4/HP2_QSPI] -force
  assign_bd_address -offset 0x000800000000 -range 0x000800000000 -target_address_space [get_bd_addr_spaces vcu/vcu_0/EncData0] [get_bd_addr_segs PS_0/SAXIGP3/HP1_DDR_HIGH] -force
  assign_bd_address -offset 0x00000000 -range 0x80000000 -target_address_space [get_bd_addr_spaces vcu/vcu_0/EncData0] [get_bd_addr_segs PS_0/SAXIGP3/HP1_DDR_LOW] -force
  assign_bd_address -offset 0xFF000000 -range 0x01000000 -target_address_space [get_bd_addr_spaces vcu/vcu_0/EncData0] [get_bd_addr_segs PS_0/SAXIGP3/HP1_LPS_OCM] -force
  assign_bd_address -offset 0xC0000000 -range 0x20000000 -target_address_space [get_bd_addr_spaces vcu/vcu_0/EncData0] [get_bd_addr_segs PS_0/SAXIGP3/HP1_QSPI] -force
  assign_bd_address -offset 0x000800000000 -range 0x000800000000 -target_address_space [get_bd_addr_spaces vcu/vcu_0/EncData1] [get_bd_addr_segs PS_0/SAXIGP3/HP1_DDR_HIGH] -force
  assign_bd_address -offset 0x00000000 -range 0x80000000 -target_address_space [get_bd_addr_spaces vcu/vcu_0/EncData1] [get_bd_addr_segs PS_0/SAXIGP3/HP1_DDR_LOW] -force
  assign_bd_address -offset 0xFF000000 -range 0x01000000 -target_address_space [get_bd_addr_spaces vcu/vcu_0/EncData1] [get_bd_addr_segs PS_0/SAXIGP3/HP1_LPS_OCM] -force
  assign_bd_address -offset 0xC0000000 -range 0x20000000 -target_address_space [get_bd_addr_spaces vcu/vcu_0/EncData1] [get_bd_addr_segs PS_0/SAXIGP3/HP1_QSPI] -force

   # Restore current instance
  current_bd_instance $oldCurInst

  # Create PFM attributes
  set_property PFM_NAME {xilinx.com:kv260:kv260_bist:1.0} [get_files [current_bd_design].bd]
  set_property PFM.AXI_PORT {M_AXI_HPM0_FPD {memport "M_AXI_GP" sptag "" memory "" is_range "false"} S_AXI_HPC0_FPD {memport "S_AXI_HP" sptag "HPC0" memory "PS_0 HPC0_DDR_LOW" is_range "false"} S_AXI_HPC1_FPD {memport "S_AXI_HP" sptag "HPC1" memory "PS_0 HPC1_DDR_LOW" is_range "false"} S_AXI_HP3_FPD {memport "S_AXI_HP" sptag "HP3" memory "PS_0 HP3_DDR_LOW" is_range "false"}} [get_bd_cells /PS_0]
  set_property PFM.CLOCK {clk_100M {id "2" is_default "false" proc_sys_reset "/proc_sys_reset_100MHz" status "fixed"} clk_300M {id "0" is_default "true" proc_sys_reset "/proc_sys_reset_300MHz" status "fixed"}} [get_bd_cells /clk_wiz_0]


  validate_bd_design
  save_bd_design
}
# End of create_root_design()


##################################################################
# MAIN FLOW
##################################################################

create_root_design ""


