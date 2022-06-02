# (C) Copyright 2022 Xilinx, Inc.
# SPDX-License-Identifier: Apache-2.0

##################################################################
# DESIGN PROCs
##################################################################

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

  # Create instance: proc_sys_reset_0, and set properties
  set proc_sys_reset_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:proc_sys_reset proc_sys_reset_0 ]

  # Create instance: xlslice_0, and set properties
  set xlslice_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlslice xlslice_0 ]
  set_property -dict [ list \
   CONFIG.DIN_FROM {2} \
   CONFIG.DIN_TO {2} \
   CONFIG.DIN_WIDTH {3} \
   CONFIG.DOUT_WIDTH {1} \
 ] $xlslice_0

  set ::PS_INST zynq_ultra_ps_e_0
  set zynq_ultra_ps_e_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:zynq_ultra_ps_e zynq_ultra_ps_e_0 ] 
  
  apply_bd_automation -rule xilinx.com:bd_rule:zynq_ultra_ps_e -config {apply_board_preset "1"} [get_bd_cells zynq_ultra_ps_e_0] 
  
  set_property -dict [ list \
  CONFIG.PSU__CRF_APB__DPLL_FRAC_CFG__ENABLED  {1} \
  CONFIG.PSU__CRF_APB__VPLL_FRAC_CFG__ENABLED  {1} \
  CONFIG.PSU__CRL_APB__USB3__ENABLE {1} \
  CONFIG.PSU__FPGA_PL0_ENABLE {1} \
  CONFIG.PSU__GPIO1_MIO__PERIPHERAL__ENABLE {1} \
  CONFIG.PSU__NUM_FABRIC_RESETS {4} \
  CONFIG.PSU__SD0__PERIPHERAL__ENABLE {0} \
  CONFIG.PSU__TTC0__WAVEOUT__ENABLE {1} \
  CONFIG.PSU__TTC0__WAVEOUT__IO {EMIO} \
  CONFIG.PSU__UART1__BAUD_RATE {115200} \
  CONFIG.PSU__UART1__PERIPHERAL__ENABLE {1} \
  CONFIG.PSU__UART1__PERIPHERAL__IO {MIO 36 .. 37} \
  CONFIG.PSU__DPAUX__PERIPHERAL__IO {MIO 27 .. 30} \
  CONFIG.PSU__DP__LANE_SEL {None} \
  CONFIG.PSU__DISPLAYPORT__PERIPHERAL__ENABLE {1} \
  CONFIG.PSU__USE__IRQ0 {1} \
  CONFIG.PSU__USE__M_AXI_GP0  {0} \
  CONFIG.PSU__USE__M_AXI_GP1  {0} \
  CONFIG.PSU__USE__M_AXI_GP2  {0} \
  CONFIG.PSU__USE__S_AXI_ACE  {0} \
  CONFIG.PSU__USE__S_AXI_ACP  {0} \
  CONFIG.PSU__USE__S_AXI_GP0  {0} \
  CONFIG.PSU__USE__S_AXI_GP1  {0} \
  CONFIG.PSU__USE__S_AXI_GP2  {0} \
  CONFIG.PSU__USE__S_AXI_GP3  {0} \
  CONFIG.PSU__USE__S_AXI_GP4  {0} \
  CONFIG.PSU__USE__S_AXI_GP5  {0} \
  CONFIG.PSU__USE__S_AXI_GP6  {0} \
  ] [get_bd_cells zynq_ultra_ps_e_0] 

  # Create port connections
  connect_bd_net -net xlslice_0_Dout [get_bd_ports fan_en_b] [get_bd_pins xlslice_0/Dout]
  connect_bd_net -net zynq_ultra_ps_e_0_emio_ttc0_wave_o [get_bd_pins xlslice_0/Din] [get_bd_pins zynq_ultra_ps_e_0/emio_ttc0_wave_o]
  connect_bd_net -net zynq_ultra_ps_e_0_pl_clk0 [get_bd_pins proc_sys_reset_0/slowest_sync_clk] [get_bd_pins zynq_ultra_ps_e_0/pl_clk0]
  connect_bd_net -net zynq_ultra_ps_e_0_pl_resetn0 [get_bd_pins proc_sys_reset_0/ext_reset_in] [get_bd_pins zynq_ultra_ps_e_0/pl_resetn0]

  # Create address segments


  # Restore current instance
  current_bd_instance $oldCurInst

  # Create PFM attributes
   set_property PFM_NAME {xilinx.com:k26:k26_base_starter_kit:1.0} [get_files [current_bd_design].bd]
   set_property PFM.AXI_PORT {M_AXI_HPM0_FPD {memport "M_AXI_GP" sptag "" memory "" is_range "false"} M_AXI_HPM1_FPD {memport "M_AXI_GP" sptag "" memory "" is_range "false"} M_AXI_HPM0_LPD {memport "M_AXI_GP" sptag "" memory "" is_range "false"} S_AXI_HPC0_FPD {memport "S_AXI_HP" sptag "HPC0" memory "zynq_ultra_ps_e_0 HPC0_DDR_LOW" is_range "false"} S_AXI_HPC1_FPD {memport "S_AXI_HP" sptag "HPC1" memory "zynq_ultra_ps_e_0 HPC1_DDR_LOW" is_range "false"} S_AXI_HP0_FPD {memport "S_AXI_HP" sptag "HP0" memory "zynq_ultra_ps_e_0 HP0_DDR_LOW" is_range "false"} S_AXI_HP1_FPD {memport "S_AXI_HP" sptag "HP1" memory "zynq_ultra_ps_e_0 HP1_DDR_LOW" is_range "false"} S_AXI_HP2_FPD {memport "S_AXI_HP" sptag "HP2" memory "zynq_ultra_ps_e_0 HP2_DDR_LOW" is_range "false"} S_AXI_HP3_FPD {memport "S_AXI_HP" sptag "HP3" memory "zynq_ultra_ps_e_0 HP3_DDR_LOW" is_range "false"} S_AXI_LPD {memport "MIG" sptag "LPD" memory "zynq_ultra_ps_e_0 LPD_DDR_LOW" is_range "false"}} [get_bd_cells /zynq_ultra_ps_e_0]
  set_property PFM.CLOCK {pl_clk0 {id "0" is_default "true" proc_sys_reset "proc_sys_reset_0" status "fixed" freq_hz "99999001"}} [get_bd_cells /zynq_ultra_ps_e_0]
  set_property PFM.IRQ {pl_ps_irq0 {id 0 range 7}} [get_bd_cells /zynq_ultra_ps_e_0]


  validate_bd_design
  save_bd_design
}
# End of create_root_design()


##################################################################
# MAIN FLOW
##################################################################

create_root_design ""


