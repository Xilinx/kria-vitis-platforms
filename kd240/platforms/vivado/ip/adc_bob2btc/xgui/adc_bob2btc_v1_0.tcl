# Copyright (C) 2023, Advanced Micro Devices, Inc.
# SPDX-License-Identifier: Apache-2.0

# Definitional proc to organize widgets for parameters.
proc init_gui { IPINST } {

#Adding Page
  set Page_0 [ipgui::add_page $IPINST -name "Page 0" -display_name {Page 0}]
  set_property tooltip {Page 0} ${Page_0}

  set Component_Name  [  ipgui::add_param $IPINST -name "Component_Name" -parent ${Page_0} -display_name {Component Name}]
  set_property tooltip {Component Name} ${Component_Name}

  set NUM_CHANNELS [ipgui::add_param $IPINST -parent ${Page_0} -name "NUM_CHANNELS" -display_name {Number of Lanes} -widget comboBox]
  set_property tooltip {Number of Analog to Digital Converters to be Controlled} ${NUM_CHANNELS}

  variable table
  set table [ipgui::add_table $IPINST  -name "Lane Table" -rows "17" -columns "2" -parent ${Page_0}]

  ipgui::add_static_text  $IPINST -name Lane -parent $table -text "Lane" -center_align true
  ipgui::add_static_text  $IPINST -name ADC_Bits -parent $table -text "ADC Bits" -center_align true

  set_property cell_location "0,0" [ipgui::get_textspec Lane -of $IPINST]
  set_property cell_location "0,1" [ipgui::get_textspec ADC_Bits -of $IPINST]

  # Lane 0
  ipgui::add_static_text  $IPINST -name Lane0 -parent  $table -text "0" -center_align true
  set_property cell_location "1,0" [ipgui::get_textspec Lane0 -of $IPINST]

  set L0_ADC_BITS [ipgui::add_param $IPINST -parent $table -name L0_ADC_BITS]
  set_property cell_location "1,1" [ipgui::get_guiparamspec L0_ADC_BITS -of $IPINST]

  # Lane 1
  ipgui::add_static_text  $IPINST -name Lane1 -parent  $table -text "1" -center_align true
  set_property cell_location "2,0" [ipgui::get_textspec Lane1 -of $IPINST]

  set L1_ADC_BITS [ipgui::add_param $IPINST -parent $table -name L1_ADC_BITS]
  set_property cell_location "2,1" [ipgui::get_guiparamspec L1_ADC_BITS -of $IPINST]

  # Lane 2
  ipgui::add_static_text  $IPINST -name Lane2 -parent  $table -text "2" -center_align true
  set_property cell_location "3,0" [ipgui::get_textspec Lane2 -of $IPINST]

  set L2_ADC_BITS [ipgui::add_param $IPINST -parent $table -name L2_ADC_BITS]
  set_property cell_location "3,1" [ipgui::get_guiparamspec L2_ADC_BITS -of $IPINST]

  # Lane 3
  ipgui::add_static_text  $IPINST -name Lane3 -parent  $table -text "3" -center_align true
  set_property cell_location "4,0" [ipgui::get_textspec Lane3 -of $IPINST]

  set L3_ADC_BITS [ipgui::add_param $IPINST -parent $table -name L3_ADC_BITS]
  set_property cell_location "4,1" [ipgui::get_guiparamspec L3_ADC_BITS -of $IPINST]

  # Lane 4
  ipgui::add_static_text  $IPINST -name Lane4 -parent  $table -text "4" -center_align true
  set_property cell_location "5,0" [ipgui::get_textspec Lane4 -of $IPINST]

  set L4_ADC_BITS [ipgui::add_param $IPINST -parent $table -name L4_ADC_BITS]
  set_property cell_location "5,1" [ipgui::get_guiparamspec L4_ADC_BITS -of $IPINST]

  # Lane 5
  ipgui::add_static_text  $IPINST -name Lane5 -parent  $table -text "5" -center_align true
  set_property cell_location "6,0" [ipgui::get_textspec Lane5 -of $IPINST]

  set L5_ADC_BITS [ipgui::add_param $IPINST -parent $table -name L5_ADC_BITS]
  set_property cell_location "6,1" [ipgui::get_guiparamspec L5_ADC_BITS -of $IPINST]

  # Lane 6
  ipgui::add_static_text  $IPINST -name Lane6 -parent  $table -text "6" -center_align true
  set_property cell_location "7,0" [ipgui::get_textspec Lane6 -of $IPINST]

  set L6_ADC_BITS [ipgui::add_param $IPINST -parent $table -name L6_ADC_BITS]
  set_property cell_location "7,1" [ipgui::get_guiparamspec L6_ADC_BITS -of $IPINST]

  # Lane 7
  ipgui::add_static_text  $IPINST -name Lane7 -parent  $table -text "7" -center_align true
  set_property cell_location "8,0" [ipgui::get_textspec Lane7 -of $IPINST]

  set L7_ADC_BITS [ipgui::add_param $IPINST -parent $table -name L7_ADC_BITS]
  set_property cell_location "8,1" [ipgui::get_guiparamspec L7_ADC_BITS -of $IPINST]

  # Lane 8
  ipgui::add_static_text  $IPINST -name Lane8 -parent  $table -text "8" -center_align true
  set_property cell_location "9,0" [ipgui::get_textspec Lane8 -of $IPINST]

  set L8_ADC_BITS [ipgui::add_param $IPINST -parent $table -name L8_ADC_BITS]
  set_property cell_location "9,1" [ipgui::get_guiparamspec L8_ADC_BITS -of $IPINST]

  # Lane 9
  ipgui::add_static_text  $IPINST -name Lane9 -parent  $table -text "9" -center_align true
  set_property cell_location "10,0" [ipgui::get_textspec Lane9 -of $IPINST]

  set L9_ADC_BITS [ipgui::add_param $IPINST -parent $table -name L9_ADC_BITS]
  set_property cell_location "10,1" [ipgui::get_guiparamspec L9_ADC_BITS -of $IPINST]

  # Lane 10
  ipgui::add_static_text  $IPINST -name Lane10 -parent  $table -text "10" -center_align true
  set_property cell_location "11,0" [ipgui::get_textspec Lane10 -of $IPINST]

  set L10_ADC_BITS [ipgui::add_param $IPINST -parent $table -name L10_ADC_BITS]
  set_property cell_location "11,1" [ipgui::get_guiparamspec L10_ADC_BITS -of $IPINST]

  # Lane 11
  ipgui::add_static_text  $IPINST -name Lane11 -parent  $table -text "11" -center_align true
  set_property cell_location "12,0" [ipgui::get_textspec Lane11 -of $IPINST]

  set L11_ADC_BITS [ipgui::add_param $IPINST -parent $table -name L11_ADC_BITS]
  set_property cell_location "12,1" [ipgui::get_guiparamspec L11_ADC_BITS -of $IPINST]

  # Lane 12
  ipgui::add_static_text  $IPINST -name Lane12 -parent  $table -text "12" -center_align true
  set_property cell_location "13,0" [ipgui::get_textspec Lane12 -of $IPINST]

  set L12_ADC_BITS [ipgui::add_param $IPINST -parent $table -name L12_ADC_BITS]
  set_property cell_location "13,1" [ipgui::get_guiparamspec L12_ADC_BITS -of $IPINST]

  # Lane 13
  ipgui::add_static_text  $IPINST -name Lane13 -parent  $table -text "13" -center_align true
  set_property cell_location "14,0" [ipgui::get_textspec Lane13 -of $IPINST]

  set L13_ADC_BITS [ipgui::add_param $IPINST -parent $table -name L13_ADC_BITS]
  set_property cell_location "14,1" [ipgui::get_guiparamspec L13_ADC_BITS -of $IPINST]

  # Lane 14
  ipgui::add_static_text  $IPINST -name Lane14 -parent  $table -text "14" -center_align true
  set_property cell_location "15,0" [ipgui::get_textspec Lane14 -of $IPINST]

  set L14_ADC_BITS [ipgui::add_param $IPINST -parent $table -name L14_ADC_BITS]
  set_property cell_location "15,1" [ipgui::get_guiparamspec L14_ADC_BITS -of $IPINST]

  # Lane 15
  ipgui::add_static_text  $IPINST -name Lane15 -parent  $table -text "15" -center_align true
  set_property cell_location "16,0" [ipgui::get_textspec Lane15 -of $IPINST]

  set L15_ADC_BITS [ipgui::add_param $IPINST -parent $table -name L15_ADC_BITS]
  set_property cell_location "16,1" [ipgui::get_guiparamspec L15_ADC_BITS -of $IPINST]

}

proc update_PARAM_VALUE.NUM_CHANNELS { PARAM_VALUE.NUM_CHANNELS } {
	# Procedure called to update NUM_CHANNELS when any of the dependent parameters in the arguments change
}

proc validate_PARAM_VALUE.NUM_CHANNELS { PARAM_VALUE.NUM_CHANNELS } {
	# Procedure called to validate NUM_CHANNELS
	return true
}

proc update_PARAM_VALUE.L0_ADC_BITS { PARAM_VALUE.L0_ADC_BITS } {
  # Procedure called to update ADC_BITS when any of the dependent parameters in the arguments change
  set_property range "2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24" ${PARAM_VALUE.L0_ADC_BITS}
  set_property enabled true ${PARAM_VALUE.L0_ADC_BITS}

}

proc validate_PARAM_VALUE.L0_ADC_BITS { PARAM_VALUE.L0_ADC_BITS } {
	# Procedure called to validate ADC_BITS
	return true
}

proc update_PARAM_VALUE.L1_ADC_BITS { PARAM_VALUE.L1_ADC_BITS } {
  # Procedure called to update ADC_BITS when any of the dependent parameters in the arguments change
  set_property range "2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24" ${PARAM_VALUE.L1_ADC_BITS}
  set_property enabled true ${PARAM_VALUE.L1_ADC_BITS}

}

proc validate_PARAM_VALUE.L1_ADC_BITS { PARAM_VALUE.L1_ADC_BITS } {
	# Procedure called to validate ADC_BITS
	return true
}

proc update_PARAM_VALUE.L2_ADC_BITS { PARAM_VALUE.L2_ADC_BITS } {
  # Procedure called to update ADC_BITS when any of the dependent parameters in the arguments change
  set_property range "2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24" ${PARAM_VALUE.L2_ADC_BITS}
  set_property enabled true ${PARAM_VALUE.L2_ADC_BITS}

}

proc validate_PARAM_VALUE.L2_ADC_BITS { PARAM_VALUE.L2_ADC_BITS } {
	# Procedure called to validate ADC_BITS
	return true
}

proc update_PARAM_VALUE.L3_ADC_BITS { PARAM_VALUE.L3_ADC_BITS } {
  # Procedure called to update ADC_BITS when any of the dependent parameters in the arguments change
  set_property range "2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24" ${PARAM_VALUE.L3_ADC_BITS}
  set_property enabled true ${PARAM_VALUE.L3_ADC_BITS}

}

proc validate_PARAM_VALUE.L3_ADC_BITS { PARAM_VALUE.L3_ADC_BITS } {
	# Procedure called to validate ADC_BITS
	return true
}

proc update_PARAM_VALUE.L4_ADC_BITS { PARAM_VALUE.L4_ADC_BITS } {
  # Procedure called to update ADC_BITS when any of the dependent parameters in the arguments change
  set_property range "2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24" ${PARAM_VALUE.L4_ADC_BITS}
  set_property enabled true ${PARAM_VALUE.L4_ADC_BITS}

}

proc validate_PARAM_VALUE.L4_ADC_BITS { PARAM_VALUE.L4_ADC_BITS } {
	# Procedure called to validate ADC_BITS
	return true
}

proc update_PARAM_VALUE.L5_ADC_BITS { PARAM_VALUE.L5_ADC_BITS } {
  # Procedure called to update ADC_BITS when any of the dependent parameters in the arguments change
  set_property range "2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24" ${PARAM_VALUE.L5_ADC_BITS}
  set_property enabled true ${PARAM_VALUE.L5_ADC_BITS}

}

proc validate_PARAM_VALUE.L5_ADC_BITS { PARAM_VALUE.L5_ADC_BITS } {
	# Procedure called to validate ADC_BITS
	return true
}

proc update_PARAM_VALUE.L6_ADC_BITS { PARAM_VALUE.L6_ADC_BITS } {
  # Procedure called to update ADC_BITS when any of the dependent parameters in the arguments change
  set_property range "2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24" ${PARAM_VALUE.L6_ADC_BITS}
  set_property enabled true ${PARAM_VALUE.L6_ADC_BITS}

}

proc validate_PARAM_VALUE.L6_ADC_BITS { PARAM_VALUE.L6_ADC_BITS } {
	# Procedure called to validate ADC_BITS
	return true
}

proc update_PARAM_VALUE.L7_ADC_BITS { PARAM_VALUE.L7_ADC_BITS } {
  # Procedure called to update ADC_BITS when any of the dependent parameters in the arguments change
  set_property range "2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24" ${PARAM_VALUE.L7_ADC_BITS}
  set_property enabled true ${PARAM_VALUE.L7_ADC_BITS}

}

proc validate_PARAM_VALUE.L7_ADC_BITS { PARAM_VALUE.L7_ADC_BITS } {
	# Procedure called to validate ADC_BITS
	return true
}

proc update_PARAM_VALUE.L8_ADC_BITS { PARAM_VALUE.L8_ADC_BITS } {
  # Procedure called to update ADC_BITS when any of the dependent parameters in the arguments change
  set_property range "2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24" ${PARAM_VALUE.L8_ADC_BITS}
  set_property enabled true ${PARAM_VALUE.L8_ADC_BITS}

}

proc validate_PARAM_VALUE.L8_ADC_BITS { PARAM_VALUE.L8_ADC_BITS } {
	# Procedure called to validate ADC_BITS
	return true
}

proc update_PARAM_VALUE.L9_ADC_BITS { PARAM_VALUE.L9_ADC_BITS } {
  # Procedure called to update ADC_BITS when any of the dependent parameters in the arguments change
  set_property range "2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24" ${PARAM_VALUE.L9_ADC_BITS}
  set_property enabled true ${PARAM_VALUE.L9_ADC_BITS}

}

proc validate_PARAM_VALUE.L9_ADC_BITS { PARAM_VALUE.L9_ADC_BITS } {
	# Procedure called to validate ADC_BITS
	return true
}

proc update_PARAM_VALUE.L10_ADC_BITS { PARAM_VALUE.L10_ADC_BITS } {
  # Procedure called to update ADC_BITS when any of the dependent parameters in the arguments change
  set_property range "2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24" ${PARAM_VALUE.L10_ADC_BITS}
  set_property enabled true ${PARAM_VALUE.L10_ADC_BITS}

}

proc validate_PARAM_VALUE.L10_ADC_BITS { PARAM_VALUE.L10_ADC_BITS } {
	# Procedure called to validate ADC_BITS
	return true
}

proc update_PARAM_VALUE.L11_ADC_BITS { PARAM_VALUE.L11_ADC_BITS } {
  # Procedure called to update ADC_BITS when any of the dependent parameters in the arguments change
  set_property range "2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24" ${PARAM_VALUE.L11_ADC_BITS}
  set_property enabled true ${PARAM_VALUE.L11_ADC_BITS}

}

proc validate_PARAM_VALUE.L11_ADC_BITS { PARAM_VALUE.L11_ADC_BITS } {
	# Procedure called to validate ADC_BITS
	return true
}

proc update_PARAM_VALUE.L12_ADC_BITS { PARAM_VALUE.L12_ADC_BITS } {
  # Procedure called to update ADC_BITS when any of the dependent parameters in the arguments change
  set_property range "2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24" ${PARAM_VALUE.L12_ADC_BITS}
  set_property enabled true ${PARAM_VALUE.L12_ADC_BITS}

}

proc validate_PARAM_VALUE.L12_ADC_BITS { PARAM_VALUE.L12_ADC_BITS } {
	# Procedure called to validate ADC_BITS
	return true
}

proc update_PARAM_VALUE.L13_ADC_BITS { PARAM_VALUE.L13_ADC_BITS } {
  # Procedure called to update ADC_BITS when any of the dependent parameters in the arguments change
  set_property range "2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24" ${PARAM_VALUE.L13_ADC_BITS}
  set_property enabled true ${PARAM_VALUE.L13_ADC_BITS}

}

proc validate_PARAM_VALUE.L13_ADC_BITS { PARAM_VALUE.L13_ADC_BITS } {
	# Procedure called to validate ADC_BITS
	return true
}

proc update_PARAM_VALUE.L14_ADC_BITS { PARAM_VALUE.L14_ADC_BITS } {
  # Procedure called to update ADC_BITS when any of the dependent parameters in the arguments change
  set_property range "2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24" ${PARAM_VALUE.L14_ADC_BITS}
  set_property enabled true ${PARAM_VALUE.L14_ADC_BITS}

}

proc validate_PARAM_VALUE.L14_ADC_BITS { PARAM_VALUE.L14_ADC_BITS } {
	# Procedure called to validate ADC_BITS
	return true
}

proc update_PARAM_VALUE.L15_ADC_BITS { PARAM_VALUE.L15_ADC_BITS } {
  # Procedure called to update ADC_BITS when any of the dependent parameters in the arguments change
  set_property range "2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24" ${PARAM_VALUE.L15_ADC_BITS}
  set_property enabled true ${PARAM_VALUE.L15_ADC_BITS}

}

proc validate_PARAM_VALUE.L15_ADC_BITS { PARAM_VALUE.L15_ADC_BITS } {
	# Procedure called to validate ADC_BITS
	return true
}

####### MODELPARAMS #########
proc update_MODELPARAM_VALUE.NUM_CHANNELS { MODELPARAM_VALUE.NUM_CHANNELS PARAM_VALUE.NUM_CHANNELS } {
	# Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
	set_property value [get_property value ${PARAM_VALUE.NUM_CHANNELS}] ${MODELPARAM_VALUE.NUM_CHANNELS}
}

proc update_MODELPARAM_VALUE.L0_ADC_BITS { MODELPARAM_VALUE.L0_ADC_BITS PARAM_VALUE.L0_ADC_BITS } {
  # Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
  set_property value [get_property value ${PARAM_VALUE.L0_ADC_BITS}] ${MODELPARAM_VALUE.L0_ADC_BITS}
}

proc update_MODELPARAM_VALUE.L1_ADC_BITS { MODELPARAM_VALUE.L1_ADC_BITS PARAM_VALUE.L1_ADC_BITS } {
  # Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
  set_property value [get_property value ${PARAM_VALUE.L1_ADC_BITS}] ${MODELPARAM_VALUE.L1_ADC_BITS}
}

proc update_MODELPARAM_VALUE.L2_ADC_BITS { MODELPARAM_VALUE.L2_ADC_BITS PARAM_VALUE.L2_ADC_BITS } {
  # Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
  set_property value [get_property value ${PARAM_VALUE.L2_ADC_BITS}] ${MODELPARAM_VALUE.L2_ADC_BITS}
}

proc update_MODELPARAM_VALUE.L3_ADC_BITS { MODELPARAM_VALUE.L3_ADC_BITS PARAM_VALUE.L3_ADC_BITS } {
  # Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
  set_property value [get_property value ${PARAM_VALUE.L3_ADC_BITS}] ${MODELPARAM_VALUE.L3_ADC_BITS}
}

proc update_MODELPARAM_VALUE.L4_ADC_BITS { MODELPARAM_VALUE.L4_ADC_BITS PARAM_VALUE.L4_ADC_BITS } {
  # Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
  set_property value [get_property value ${PARAM_VALUE.L4_ADC_BITS}] ${MODELPARAM_VALUE.L4_ADC_BITS}
}

proc update_MODELPARAM_VALUE.L5_ADC_BITS { MODELPARAM_VALUE.L5_ADC_BITS PARAM_VALUE.L5_ADC_BITS } {
  # Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
  set_property value [get_property value ${PARAM_VALUE.L5_ADC_BITS}] ${MODELPARAM_VALUE.L5_ADC_BITS}
}

proc update_MODELPARAM_VALUE.L6_ADC_BITS { MODELPARAM_VALUE.L6_ADC_BITS PARAM_VALUE.L6_ADC_BITS } {
  # Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
  set_property value [get_property value ${PARAM_VALUE.L6_ADC_BITS}] ${MODELPARAM_VALUE.L6_ADC_BITS}
}

proc update_MODELPARAM_VALUE.L7_ADC_BITS { MODELPARAM_VALUE.L7_ADC_BITS PARAM_VALUE.L7_ADC_BITS } {
  # Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
  set_property value [get_property value ${PARAM_VALUE.L7_ADC_BITS}] ${MODELPARAM_VALUE.L7_ADC_BITS}
}

proc update_MODELPARAM_VALUE.L8_ADC_BITS { MODELPARAM_VALUE.L8_ADC_BITS PARAM_VALUE.L8_ADC_BITS } {
  # Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
  set_property value [get_property value ${PARAM_VALUE.L8_ADC_BITS}] ${MODELPARAM_VALUE.L8_ADC_BITS}
}

proc update_MODELPARAM_VALUE.L9_ADC_BITS { MODELPARAM_VALUE.L9_ADC_BITS PARAM_VALUE.L9_ADC_BITS } {
  # Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
  set_property value [get_property value ${PARAM_VALUE.L9_ADC_BITS}] ${MODELPARAM_VALUE.L9_ADC_BITS}
}

proc update_MODELPARAM_VALUE.L10_ADC_BITS { MODELPARAM_VALUE.L10_ADC_BITS PARAM_VALUE.L10_ADC_BITS } {
  # Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
  set_property value [get_property value ${PARAM_VALUE.L10_ADC_BITS}] ${MODELPARAM_VALUE.L10_ADC_BITS}
}

proc update_MODELPARAM_VALUE.L11_ADC_BITS { MODELPARAM_VALUE.L11_ADC_BITS PARAM_VALUE.L11_ADC_BITS } {
  # Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
  set_property value [get_property value ${PARAM_VALUE.L11_ADC_BITS}] ${MODELPARAM_VALUE.L11_ADC_BITS}
}

proc update_MODELPARAM_VALUE.L12_ADC_BITS { MODELPARAM_VALUE.L12_ADC_BITS PARAM_VALUE.L12_ADC_BITS } {
  # Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
  set_property value [get_property value ${PARAM_VALUE.L12_ADC_BITS}] ${MODELPARAM_VALUE.L12_ADC_BITS}
}

proc update_MODELPARAM_VALUE.L13_ADC_BITS { MODELPARAM_VALUE.L13_ADC_BITS PARAM_VALUE.L13_ADC_BITS } {
  # Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
  set_property value [get_property value ${PARAM_VALUE.L13_ADC_BITS}] ${MODELPARAM_VALUE.L13_ADC_BITS}
}

proc update_MODELPARAM_VALUE.L14_ADC_BITS { MODELPARAM_VALUE.L14_ADC_BITS PARAM_VALUE.L14_ADC_BITS } {
  # Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
  set_property value [get_property value ${PARAM_VALUE.L14_ADC_BITS}] ${MODELPARAM_VALUE.L14_ADC_BITS}
}

proc update_MODELPARAM_VALUE.L15_ADC_BITS { MODELPARAM_VALUE.L15_ADC_BITS PARAM_VALUE.L15_ADC_BITS } {
  # Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
  set_property value [get_property value ${PARAM_VALUE.L15_ADC_BITS}] ${MODELPARAM_VALUE.L15_ADC_BITS}
}

#############################################3
proc update_gui_for_PARAM_VALUE.NUM_CHANNELS {PARAM_VALUE.NUM_CHANNELS IPINST} {

  set num_channels [get_property value ${PARAM_VALUE.NUM_CHANNELS}]

  set list {1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16}
  set table [ipgui::get_tablespec -of $IPINST "Lane Table"]
#  set index [tcl::lsearch $list [exp $num_channels - 1]]
  set index [tcl::lsearch $list $num_channels]
  set list [tcl::lreplace $list 0 $index]
  set list [regsub -all " " $list ","]
  set_property hidden_rows "$list" $table

}
