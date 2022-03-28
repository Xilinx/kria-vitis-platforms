# (C) Copyright 2020 - 2021 Xilinx, Inc.
# SPDX-License-Identifier: Apache-2.0
##################################################################
# DESIGN PROCs
##################################################################


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

  create_bd_intf_pin -mode Slave -vlnv xilinx.com:interface:aximm_rtl:1.0 s_axi_ctrl_0

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
   CONFIG.FIFO_MEMORY_TYPE {ultra} \
 ] $axis_data_fifo_cap

  # Create instance: axis_subset_converter_0, and set properties
  set axis_subset_converter_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:axis_subset_converter axis_subset_converter_0 ]
  set_property -dict [ list \
   CONFIG.M_TDATA_NUM_BYTES {3} \
   CONFIG.M_TDEST_WIDTH {1} \
   CONFIG.S_TDATA_NUM_BYTES {2} \
   CONFIG.S_TDEST_WIDTH {10} \
   CONFIG.TDATA_REMAP {8'b00000000,tdata[15:0]} \
   CONFIG.TDEST_REMAP {1'b0} \
 ] $axis_subset_converter_0

  # Create instance: mipi_csi2_rx_subsyst_0, and set properties
  set mipi_csi2_rx_subsyst_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:mipi_csi2_rx_subsystem mipi_csi2_rx_subsyst_0 ]
  set_property -dict [ list \
   CONFIG.CLK_LANE_IO_LOC {G1} \
   CONFIG.CLK_LANE_IO_LOC_NAME {IO_L1P_T0L_N0_DBC_66} \
   CONFIG.CMN_NUM_LANES {4} \
   CONFIG.CMN_PXL_FORMAT {YUV422_8bit} \
   CONFIG.CMN_VC {0} \
   CONFIG.CSI_BUF_DEPTH {4096} \
   CONFIG.C_CSI_FILTER_USERDATATYPE {true} \
   CONFIG.C_DPHY_LANES {4} \
   CONFIG.C_HS_LINE_RATE {896} \
   CONFIG.C_HS_SETTLE_NS {146} \
   CONFIG.DATA_LANE0_IO_LOC {E1} \
   CONFIG.DATA_LANE0_IO_LOC_NAME {IO_L2P_T0L_N2_66} \
   CONFIG.DATA_LANE1_IO_LOC {F2} \
   CONFIG.DATA_LANE1_IO_LOC_NAME {IO_L3P_T0L_N4_AD15P_66} \
   CONFIG.DATA_LANE2_IO_LOC {G3} \
   CONFIG.DATA_LANE2_IO_LOC_NAME {IO_L4P_T0U_N6_DBC_AD7P_66} \
   CONFIG.DATA_LANE3_IO_LOC {E4} \
   CONFIG.DATA_LANE3_IO_LOC_NAME {IO_L5P_T0U_N8_AD14P_66} \
   CONFIG.DPHYRX_BOARD_INTERFACE {som240_1_connector_mipi_csi_isp} \
   CONFIG.DPY_LINE_RATE {896} \
   CONFIG.HP_IO_BANK_SELECTION {66} \
   CONFIG.SupportLevel {1} \
   CONFIG.YUV420_BUF_DPTH {4096} \
 ] $mipi_csi2_rx_subsyst_0

  # Create instance: v_frmbuf_wr_0, and set properties
  set v_frmbuf_wr_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:v_frmbuf_wr v_frmbuf_wr_0 ]
  set_property -dict [ list \
   CONFIG.AXIMM_DATA_WIDTH {64} \
   CONFIG.C_M_AXI_MM_VIDEO_DATA_WIDTH {64} \
   CONFIG.HAS_BGR8 {1} \
   CONFIG.HAS_BGRX8 {1} \
   CONFIG.HAS_RGB8 {1} \
   CONFIG.HAS_RGBX8 {1} \
   CONFIG.HAS_UYVY8 {0} \
   CONFIG.HAS_YUV8 {0} \
   CONFIG.HAS_Y_UV8 {0} \
   CONFIG.HAS_Y_UV8_420 {1} \
   CONFIG.MAX_COLS {3840} \
   CONFIG.MAX_NR_PLANES {2} \
   CONFIG.MAX_ROWS {2160} \
   CONFIG.SAMPLES_PER_CLOCK {1} \
 ] $v_frmbuf_wr_0

  # Create instance: v_proc_ss_0, and set properties
  set v_proc_ss_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:v_proc_ss v_proc_ss_0 ]
  set_property -dict [ list \
   CONFIG.C_ENABLE_CSC {true} \
   CONFIG.C_MAX_DATA_WIDTH {8} \
   CONFIG.C_SAMPLES_PER_CLK {1} \
   CONFIG.C_TOPOLOGY {0} \
 ] $v_proc_ss_0

  # Create instance: xlslice_0, and set properties
  set xlslice_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlslice xlslice_0 ]
  set_property -dict [ list \
   CONFIG.DIN_FROM {0} \
   CONFIG.DIN_TO {0} \
   CONFIG.DIN_WIDTH {92} \
   CONFIG.DOUT_WIDTH {1} \
 ] $xlslice_0

  # Create instance: xlslice_7, and set properties
  set xlslice_7 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlslice xlslice_7 ]
  set_property -dict [ list \
   CONFIG.DIN_FROM {7} \
   CONFIG.DIN_TO {7} \
   CONFIG.DIN_WIDTH {92} \
   CONFIG.DOUT_WIDTH {1} \
 ] $xlslice_7

  # Create interface connections
  connect_bd_intf_net -intf_net Conn1 [get_bd_intf_pins s_axi_ctrl_0] [get_bd_intf_pins v_proc_ss_0/s_axi_ctrl]
  connect_bd_intf_net -intf_net axi_interconnect_0_M11_AXI [get_bd_intf_pins s_axi_ctrl_frmbuf] [get_bd_intf_pins v_frmbuf_wr_0/s_axi_CTRL]
  connect_bd_intf_net -intf_net axis_data_fifo_0_M_AXIS [get_bd_intf_pins axis_data_fifo_cap/M_AXIS] [get_bd_intf_pins axis_subset_converter_0/S_AXIS]
  connect_bd_intf_net -intf_net axis_subset_converter_0_M_AXIS [get_bd_intf_pins axis_subset_converter_0/M_AXIS] [get_bd_intf_pins v_proc_ss_0/s_axis]
  connect_bd_intf_net -intf_net csirxss_s_axi_1 [get_bd_intf_pins csirxss_s_axi] [get_bd_intf_pins mipi_csi2_rx_subsyst_0/csirxss_s_axi]
  connect_bd_intf_net -intf_net mipi_csi2_rx_subsyst_0_video_out [get_bd_intf_pins axis_data_fifo_cap/S_AXIS] [get_bd_intf_pins mipi_csi2_rx_subsyst_0/video_out]
  connect_bd_intf_net -intf_net mipi_phy_if_1 [get_bd_intf_pins mipi_phy_if] [get_bd_intf_pins mipi_csi2_rx_subsyst_0/mipi_phy_if]
  connect_bd_intf_net -intf_net v_frmbuf_wr_1_m_axi_mm_video [get_bd_intf_pins m_axi_mm_video] [get_bd_intf_pins v_frmbuf_wr_0/m_axi_mm_video]
  connect_bd_intf_net -intf_net v_proc_ss_0_m_axis [get_bd_intf_pins v_frmbuf_wr_0/s_axis_video] [get_bd_intf_pins v_proc_ss_0/m_axis]

  # Create port connections
  connect_bd_net -net Din_1 [get_bd_pins Din] [get_bd_pins xlslice_0/Din] [get_bd_pins xlslice_7/Din]
  connect_bd_net -net clk_wiz_0_clk_100M [get_bd_pins lite_aclk] [get_bd_pins mipi_csi2_rx_subsyst_0/lite_aclk]
  connect_bd_net -net clk_wiz_0_clk_200M [get_bd_pins dphy_clk_200M] [get_bd_pins mipi_csi2_rx_subsyst_0/dphy_clk_200M]
  connect_bd_net -net clk_wiz_0_clk_300M [get_bd_pins aclk] [get_bd_pins axis_data_fifo_cap/s_axis_aclk] [get_bd_pins axis_subset_converter_0/aclk] [get_bd_pins mipi_csi2_rx_subsyst_0/video_aclk] [get_bd_pins v_frmbuf_wr_0/ap_clk] [get_bd_pins v_proc_ss_0/aclk_axis] [get_bd_pins v_proc_ss_0/aclk_ctrl]
  connect_bd_net -net mipi_csi2_rx_subsyst_0_csirxss_csi_irq [get_bd_pins csirxss_csi_irq] [get_bd_pins mipi_csi2_rx_subsyst_0/csirxss_csi_irq]
  connect_bd_net -net proc_sys_reset_1_peripheral_aresetn [get_bd_pins aresetn] [get_bd_pins axis_data_fifo_cap/s_axis_aresetn] [get_bd_pins axis_subset_converter_0/aresetn] [get_bd_pins mipi_csi2_rx_subsyst_0/video_aresetn]
  connect_bd_net -net proc_sys_reset_2_peripheral_aresetn [get_bd_pins lite_aresetn] [get_bd_pins mipi_csi2_rx_subsyst_0/lite_aresetn]
  connect_bd_net -net v_frmbuf_wr_1_interrupt [get_bd_pins interrupt] [get_bd_pins v_frmbuf_wr_0/interrupt]
  connect_bd_net -net xlslice_1_Dout [get_bd_pins v_proc_ss_0/aresetn_ctrl] [get_bd_pins xlslice_7/Dout]
  connect_bd_net -net xlslice_2_Dout [get_bd_pins v_frmbuf_wr_0/ap_rst_n] [get_bd_pins xlslice_0/Dout]

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
  set iic [ create_bd_intf_port -mode Master -vlnv xilinx.com:interface:iic_rtl:1.0 iic ]

  set mipi_phy_if_isp [ create_bd_intf_port -mode Slave -vlnv xilinx.com:interface:mipi_phy_rtl:1.0 mipi_phy_if_isp ]


  # Create ports
  set ap1302_rst_b [ create_bd_port -dir O -from 0 -to 0 ap1302_rst_b ]
  set ap1302_standby [ create_bd_port -dir O -from 0 -to 0 ap1302_standby ]
  set fan_en_b [ create_bd_port -dir O -from 0 -to 0 fan_en_b ]

  # Create instance: PS_0, and set properties
  set PS_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:zynq_ultra_ps_e PS_0 ]
  
  apply_bd_automation -rule xilinx.com:bd_rule:zynq_ultra_ps_e -config {apply_board_preset "1" }  [get_bd_cells PS_0]
  
  set_property -dict [ list \
  CONFIG.PSU__CRF_APB__DPLL_FRAC_CFG__ENABLED  {1} \
  CONFIG.PSU__CRF_APB__VPLL_FRAC_CFG__ENABLED  {1} \
  CONFIG.PSU__CRL_APB__PL1_REF_CTRL__DIVISOR1 {1} \
  CONFIG.PSU__CRL_APB__PL1_REF_CTRL__SRCSEL {IOPLL} \
  CONFIG.PSU__CRL_APB__USB3__ENABLE {1} \
  CONFIG.PSU__FPGA_PL0_ENABLE {1} \
  CONFIG.PSU__FPGA_PL1_ENABLE {1} \
  CONFIG.PSU__GPIO_EMIO__PERIPHERAL__ENABLE {1} \
  CONFIG.PSU__GPIO_EMIO__PERIPHERAL__IO {92} \
  CONFIG.PSU__TTC0__WAVEOUT__ENABLE {1} \
  CONFIG.PSU__TTC0__WAVEOUT__IO {EMIO} \
  CONFIG.PSU__MAXIGP0__DATA_WIDTH {32} \
  CONFIG.PSU__MAXIGP1__DATA_WIDTH {32} \
  CONFIG.PSU__MAXIGP2__DATA_WIDTH {32} \
  CONFIG.PSU__NUM_FABRIC_RESETS {1} \
  CONFIG.PSU__SAXIGP0__DATA_WIDTH {128} \
  CONFIG.PSU__SAXIGP1__DATA_WIDTH {128} \
  CONFIG.PSU__SAXIGP2__DATA_WIDTH {128} \
  CONFIG.PSU__SAXIGP3__DATA_WIDTH {128} \
  CONFIG.PSU__SAXIGP4__DATA_WIDTH {128} \
  CONFIG.PSU__SAXIGP5__DATA_WIDTH {128} \
  CONFIG.PSU__SAXIGP6__DATA_WIDTH {128} \
  CONFIG.PSU__USE__IRQ0 {1} \
  CONFIG.PSU__USE__IRQ1 {1} \
  CONFIG.PSU__USE__M_AXI_GP0  {0} \
  CONFIG.PSU__USE__M_AXI_GP1  {1} \
  CONFIG.PSU__USE__M_AXI_GP2  {1} \
  CONFIG.PSU__USE__S_AXI_ACE  {0} \
  CONFIG.PSU__USE__S_AXI_ACP  {0} \
  CONFIG.PSU__USE__S_AXI_GP0  {0} \
  CONFIG.PSU__USE__S_AXI_GP1  {0} \
  CONFIG.PSU__USE__S_AXI_GP2  {1} \
  CONFIG.PSU__USE__S_AXI_GP3  {0} \
  CONFIG.PSU__USE__S_AXI_GP4  {0} \
  CONFIG.PSU__USE__S_AXI_GP5  {0} \
  CONFIG.PSU__USE__S_AXI_GP6  {0} \
] $PS_0

  # Create instance: axi_iic_0, and set properties
  set axi_iic_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:axi_iic axi_iic_0 ]
  set_property -dict [ list \
   CONFIG.IIC_BOARD_INTERFACE {som240_1_connector_hda_iic_switch} \
   CONFIG.IIC_FREQ_KHZ {400} \
 ] $axi_iic_0

  # Create instance: axi_interconnect_ctrl_100, and set properties
  set axi_interconnect_ctrl_100 [ create_bd_cell -type ip -vlnv xilinx.com:ip:axi_interconnect axi_interconnect_ctrl_100 ]
  set_property -dict [ list \
   CONFIG.ENABLE_ADVANCED_OPTIONS {1} \
   CONFIG.NUM_MI {2} \
 ] $axi_interconnect_ctrl_100

  # Create instance: axi_interconnect_ctrl_300, and set properties
  set axi_interconnect_ctrl_300 [ create_bd_cell -type ip -vlnv xilinx.com:ip:axi_interconnect axi_interconnect_ctrl_300 ]
  set_property -dict [ list \
   CONFIG.NUM_MI {2} \
   CONFIG.NUM_SI {1} \
 ] $axi_interconnect_ctrl_300

  # Create instance: capture_pipeline_isp
  create_hier_cell_capture_pipeline_isp [current_bd_instance .] capture_pipeline_isp

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
   CONFIG.CLKOUT5_JITTER {83.769} \
   CONFIG.CLKOUT5_PHASE_ERROR {87.181} \
   CONFIG.CLKOUT5_REQUESTED_OUT_FREQ {600.000} \
   CONFIG.CLKOUT5_USED {true} \
   CONFIG.CLK_OUT1_PORT {clk_200M} \
   CONFIG.CLK_OUT2_PORT {clk_100M} \
   CONFIG.CLK_OUT3_PORT {clk_300M} \
   CONFIG.CLK_OUT4_PORT {clk_50M} \
   CONFIG.CLK_OUT5_PORT {clk_600M} \
   CONFIG.MMCM_CLKOUT0_DIVIDE_F {6.000} \
   CONFIG.MMCM_CLKOUT1_DIVIDE {12} \
   CONFIG.MMCM_CLKOUT2_DIVIDE {4} \
   CONFIG.MMCM_CLKOUT3_DIVIDE {24} \
   CONFIG.MMCM_CLKOUT4_DIVIDE {2} \
   CONFIG.MMCM_DIVCLK_DIVIDE {1} \
   CONFIG.NUM_OUT_CLKS {5} \
   CONFIG.PRIM_SOURCE {Global_buffer} \
   CONFIG.RESET_PORT {resetn} \
   CONFIG.RESET_TYPE {ACTIVE_LOW} \
 ] $clk_wiz_0

  # Create instance: proc_sys_reset_100MHz, and set properties
  set proc_sys_reset_100MHz [ create_bd_cell -type ip -vlnv xilinx.com:ip:proc_sys_reset proc_sys_reset_100MHz ]

  # Create instance: proc_sys_reset_300MHz, and set properties
  set proc_sys_reset_300MHz [ create_bd_cell -type ip -vlnv xilinx.com:ip:proc_sys_reset proc_sys_reset_300MHz ]

  # Create instance: proc_sys_reset_600MHz, and set properties
  set proc_sys_reset_600MHz [ create_bd_cell -type ip -vlnv xilinx.com:ip:proc_sys_reset proc_sys_reset_600MHz ]

  # Create instance: smartconnect_cap, and set properties
  set smartconnect_cap [ create_bd_cell -type ip -vlnv xilinx.com:ip:smartconnect smartconnect_cap ]
  set_property -dict [ list \
   CONFIG.NUM_CLKS {1} \
   CONFIG.NUM_SI {1} \
 ] $smartconnect_cap

  # Create instance: xlconcat_irq1, and set properties
  set xlconcat_irq1 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlconcat xlconcat_irq1 ]
  set_property -dict [ list \
   CONFIG.NUM_PORTS {4} \
 ] $xlconcat_irq1

  # Create instance: xlconstant_0, and set properties
  set xlconstant_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlconstant xlconstant_0 ]
  set_property -dict [ list \
   CONFIG.CONST_VAL {0} \
 ] $xlconstant_0

  # Create instance: xlconstant_1, and set properties
  set xlconstant_1 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlconstant xlconstant_1 ]
  set_property -dict [ list \
   CONFIG.CONST_VAL {0} \
 ] $xlconstant_1

  # Create instance: xlslice_0, and set properties
  set xlslice_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlslice xlslice_0 ]
  set_property -dict [ list \
   CONFIG.DIN_FROM {1} \
   CONFIG.DIN_TO {1} \
   CONFIG.DIN_WIDTH {92} \
   CONFIG.DOUT_WIDTH {1} \
 ] $xlslice_0
 
  # Create instance: xlslice_ttc_0, and set properties
  set xlslice_ttc_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlslice xlslice_ttc_0 ]
  set_property -dict [ list \
   CONFIG.DIN_FROM {2} \
   CONFIG.DIN_TO {2} \
   CONFIG.DIN_WIDTH {3} \
   CONFIG.DOUT_WIDTH {1} \
 ] $xlslice_ttc_0
 

  # Create interface connections
  connect_bd_intf_net -intf_net PS_0_M_AXI_HPM1_FPD [get_bd_intf_pins PS_0/M_AXI_HPM1_FPD] [get_bd_intf_pins axi_interconnect_ctrl_300/S00_AXI]
  connect_bd_intf_net -intf_net S00_AXI_1 [get_bd_intf_pins PS_0/M_AXI_HPM0_LPD] [get_bd_intf_pins axi_interconnect_ctrl_100/S00_AXI]
  connect_bd_intf_net -intf_net axi_iic_0_IIC [get_bd_intf_ports iic] [get_bd_intf_pins axi_iic_0/IIC]
  connect_bd_intf_net -intf_net axi_interconnect_0_M00_AXI [get_bd_intf_pins axi_interconnect_ctrl_100/M00_AXI] [get_bd_intf_pins capture_pipeline_isp/csirxss_s_axi]
  connect_bd_intf_net -intf_net axi_interconnect_0_M00_AXI1 [get_bd_intf_pins axi_interconnect_ctrl_300/M00_AXI] [get_bd_intf_pins capture_pipeline_isp/s_axi_ctrl_frmbuf]
  connect_bd_intf_net -intf_net axi_interconnect_ctrl_100_M01_AXI [get_bd_intf_pins axi_iic_0/S_AXI] [get_bd_intf_pins axi_interconnect_ctrl_100/M01_AXI]
  connect_bd_intf_net -intf_net axi_interconnect_ctrl_300_M01_AXI [get_bd_intf_pins axi_interconnect_ctrl_300/M01_AXI] [get_bd_intf_pins capture_pipeline_isp/s_axi_ctrl_0]
  connect_bd_intf_net -intf_net capture_pipeline_m_axi_mm_video [get_bd_intf_pins capture_pipeline_isp/m_axi_mm_video] [get_bd_intf_pins smartconnect_cap/S00_AXI]
  connect_bd_intf_net -intf_net mipi_phy_if_1 [get_bd_intf_ports mipi_phy_if_isp] [get_bd_intf_pins capture_pipeline_isp/mipi_phy_if]
  connect_bd_intf_net -intf_net smartconnect_cap_M00_AXI [get_bd_intf_pins PS_0/S_AXI_HP0_FPD] [get_bd_intf_pins smartconnect_cap/M00_AXI]

  # Create port connections
  connect_bd_net -net PS_0_emio_gpio_o [get_bd_pins PS_0/emio_gpio_o] [get_bd_pins capture_pipeline_isp/Din] [get_bd_pins xlslice_0/Din]
  connect_bd_net -net PS_0_pl_clk0 [get_bd_pins PS_0/pl_clk0] [get_bd_pins clk_wiz_0/clk_in1]
  connect_bd_net -net PS_0_pl_resetn0 [get_bd_pins PS_0/pl_resetn0] [get_bd_pins clk_wiz_0/resetn] [get_bd_pins proc_sys_reset_100MHz/ext_reset_in] [get_bd_pins proc_sys_reset_300MHz/ext_reset_in] [get_bd_pins proc_sys_reset_600MHz/ext_reset_in]
  connect_bd_net -net axi_iic_0_iic2intc_irpt [get_bd_pins axi_iic_0/iic2intc_irpt] [get_bd_pins xlconcat_irq1/In3]
  connect_bd_net -net capture_pipeline_interrupt [get_bd_pins capture_pipeline_isp/interrupt] [get_bd_pins xlconcat_irq1/In1]
  connect_bd_net -net clk_wiz_0_clk_100M [get_bd_pins PS_0/maxihpm0_lpd_aclk] [get_bd_pins axi_iic_0/s_axi_aclk] [get_bd_pins axi_interconnect_ctrl_100/ACLK] [get_bd_pins axi_interconnect_ctrl_100/M00_ACLK] [get_bd_pins axi_interconnect_ctrl_100/M01_ACLK] [get_bd_pins axi_interconnect_ctrl_100/S00_ACLK] [get_bd_pins capture_pipeline_isp/lite_aclk] [get_bd_pins clk_wiz_0/clk_100M] [get_bd_pins proc_sys_reset_100MHz/slowest_sync_clk]
  connect_bd_net -net clk_wiz_0_clk_200M [get_bd_pins capture_pipeline_isp/dphy_clk_200M] [get_bd_pins clk_wiz_0/clk_200M]
  connect_bd_net -net clk_wiz_0_clk_300M [get_bd_pins PS_0/maxihpm1_fpd_aclk] [get_bd_pins PS_0/saxihp0_fpd_aclk] [get_bd_pins axi_interconnect_ctrl_300/ACLK] [get_bd_pins axi_interconnect_ctrl_300/M00_ACLK] [get_bd_pins axi_interconnect_ctrl_300/M01_ACLK] [get_bd_pins axi_interconnect_ctrl_300/S00_ACLK] [get_bd_pins capture_pipeline_isp/aclk] [get_bd_pins clk_wiz_0/clk_300M] [get_bd_pins proc_sys_reset_300MHz/slowest_sync_clk] [get_bd_pins smartconnect_cap/aclk]
  connect_bd_net -net clk_wiz_0_clk_600M [get_bd_pins clk_wiz_0/clk_600M] [get_bd_pins proc_sys_reset_600MHz/slowest_sync_clk]
  connect_bd_net -net mipi_csi2_rx_subsyst_0_csirxss_csi_irq [get_bd_pins capture_pipeline_isp/csirxss_csi_irq] [get_bd_pins xlconcat_irq1/In0]
  connect_bd_net -net proc_sys_reset_1_interconnect_aresetn [get_bd_pins axi_interconnect_ctrl_300/ARESETN] [get_bd_pins proc_sys_reset_300MHz/interconnect_aresetn]
  connect_bd_net -net proc_sys_reset_1_peripheral_aresetn [get_bd_pins axi_interconnect_ctrl_300/M00_ARESETN] [get_bd_pins axi_interconnect_ctrl_300/M01_ARESETN] [get_bd_pins axi_interconnect_ctrl_300/S00_ARESETN] [get_bd_pins capture_pipeline_isp/aresetn] [get_bd_pins proc_sys_reset_300MHz/peripheral_aresetn] [get_bd_pins smartconnect_cap/aresetn]
  connect_bd_net -net proc_sys_reset_2_peripheral_aresetn [get_bd_pins axi_iic_0/s_axi_aresetn] [get_bd_pins axi_interconnect_ctrl_100/ARESETN] [get_bd_pins axi_interconnect_ctrl_100/M00_ARESETN] [get_bd_pins axi_interconnect_ctrl_100/M01_ARESETN] [get_bd_pins axi_interconnect_ctrl_100/S00_ARESETN] [get_bd_pins capture_pipeline_isp/lite_aresetn] [get_bd_pins proc_sys_reset_100MHz/peripheral_aresetn]
  connect_bd_net -net xlconcat_0_0_dout [get_bd_pins PS_0/pl_ps_irq1] [get_bd_pins xlconcat_irq1/dout]
  connect_bd_net -net xlconstant_0_dout [get_bd_pins xlconcat_irq1/In2] [get_bd_pins xlconstant_0/dout]
  connect_bd_net -net xlconstant_1_dout [get_bd_ports ap1302_standby] [get_bd_pins xlconstant_1/dout]
  connect_bd_net -net xlslice_0_Dout [get_bd_ports ap1302_rst_b] [get_bd_pins xlslice_0/Dout]
  connect_bd_net -net xlslice_ttc_0_Dout [get_bd_ports fan_en_b] [get_bd_pins xlslice_ttc_0/Dout]
  connect_bd_net -net zynq_ultra_ps_e_0_emio_ttc0_wave_o [get_bd_pins xlslice_ttc_0/Din] [get_bd_pins PS_0/emio_ttc0_wave_o]

  # Create address segments
  assign_bd_address -offset 0x80030000 -range 0x00010000 -target_address_space [get_bd_addr_spaces PS_0/Data] [get_bd_addr_segs axi_iic_0/S_AXI/Reg] -force
  assign_bd_address -offset 0x80000000 -range 0x00001000 -target_address_space [get_bd_addr_spaces PS_0/Data] [get_bd_addr_segs capture_pipeline_isp/mipi_csi2_rx_subsyst_0/csirxss_s_axi/Reg] -force
  assign_bd_address -offset 0xB0010000 -range 0x00010000 -target_address_space [get_bd_addr_spaces PS_0/Data] [get_bd_addr_segs capture_pipeline_isp/v_frmbuf_wr_0/s_axi_CTRL/Reg] -force
  assign_bd_address -offset 0xB0100000 -range 0x00040000 -target_address_space [get_bd_addr_spaces PS_0/Data] [get_bd_addr_segs capture_pipeline_isp/v_proc_ss_0/s_axi_ctrl/Reg] -force
  assign_bd_address -offset 0x00000000 -range 0x80000000 -target_address_space [get_bd_addr_spaces capture_pipeline_isp/v_frmbuf_wr_0/Data_m_axi_mm_video] [get_bd_addr_segs PS_0/SAXIGP2/HP0_DDR_LOW] -force
  assign_bd_address -offset 0xC0000000 -range 0x20000000 -target_address_space [get_bd_addr_spaces capture_pipeline_isp/v_frmbuf_wr_0/Data_m_axi_mm_video] [get_bd_addr_segs PS_0/SAXIGP2/HP0_QSPI] -force


  # Restore current instance
  current_bd_instance $oldCurInst

  # Create PFM attributes
  set_property PFM_NAME {xilinx.com:kv260:kv260_ispMipiRx_DP:1.0} [get_files [current_bd_design].bd]
  set_property PFM.AXI_PORT {M_AXI_HPM0_FPD {memport "M_AXI_GP" sptag "" memory "" is_range "false"} S_AXI_HPC0_FPD {memport "S_AXI_HPC" sptag "HPC0" memory "PS_0 HPC0_DDR_LOW" is_range "false"} S_AXI_HPC1_FPD {memport "S_AXI_HPC" sptag "HPC1" memory "PS_0 HPC1_DDR_LOW" is_range "false"} S_AXI_HP1_FPD {memport "S_AXI_HP" sptag "HP1" memory "PS_0 HP1_DDR_LOW" is_range "false"} S_AXI_HP2_FPD {memport "S_AXI_HP" sptag "HP2" memory "PS_0 HP2_DDR_LOW" is_range "false"} S_AXI_HP3_FPD {memport "S_AXI_HP" sptag "HP3" memory "PS_0 HP3_DDR_LOW" is_range "false"}} [get_bd_cells /PS_0]
  set_property PFM.IRQ {pl_ps_irq0 {id 0 range 7}} [get_bd_cells /PS_0]
  set_property PFM.CLOCK {clk_100M {id "2" is_default "false" proc_sys_reset "/proc_sys_reset_100MHz" status "fixed" freq_hz "99999000"} clk_300M {id "0" is_default "true" proc_sys_reset "/proc_sys_reset_300MHz" status "fixed" freq_hz "299997000"} clk_600M {id "1" is_default "false" proc_sys_reset "/proc_sys_reset_600MHz" status "fixed" freq_hz "599994000"}} [get_bd_cells /clk_wiz_0]


  validate_bd_design
  save_bd_design
}
# End of create_root_design()


##################################################################
# MAIN FLOW
##################################################################

create_root_design ""


