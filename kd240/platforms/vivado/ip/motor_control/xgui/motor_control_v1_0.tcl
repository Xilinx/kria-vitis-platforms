# Definitional proc to organize widgets for parameters.
proc init_gui { IPINST } {
  ipgui::add_param $IPINST -name "Component_Name"
  ipgui::add_param $IPINST -name "C_S_AXI_CNTRL_DATA_WIDTH"
  ipgui::add_param $IPINST -name "C_S_AXI_CNTRL_ADDR_WIDTH"
  ipgui::add_param $IPINST -name "C_S_AXIS_TDATA_WIDTH"
  ipgui::add_param $IPINST -name "Q_INTEGER"
  ipgui::add_param $IPINST -name "Q_FRACTIONAL"

}

proc update_PARAM_VALUE.C_S_AXIS_TDATA_WIDTH { PARAM_VALUE.C_S_AXIS_TDATA_WIDTH } {
	# Procedure called to update C_S_AXIS_TDATA_WIDTH when any of the dependent parameters in the arguments change
}

proc validate_PARAM_VALUE.C_S_AXIS_TDATA_WIDTH { PARAM_VALUE.C_S_AXIS_TDATA_WIDTH } {
	# Procedure called to validate C_S_AXIS_TDATA_WIDTH
	return true
}

proc update_PARAM_VALUE.C_S_AXI_CNTRL_ADDR_WIDTH { PARAM_VALUE.C_S_AXI_CNTRL_ADDR_WIDTH } {
	# Procedure called to update C_S_AXI_CNTRL_ADDR_WIDTH when any of the dependent parameters in the arguments change
}

proc validate_PARAM_VALUE.C_S_AXI_CNTRL_ADDR_WIDTH { PARAM_VALUE.C_S_AXI_CNTRL_ADDR_WIDTH } {
	# Procedure called to validate C_S_AXI_CNTRL_ADDR_WIDTH
	return true
}

proc update_PARAM_VALUE.C_S_AXI_CNTRL_DATA_WIDTH { PARAM_VALUE.C_S_AXI_CNTRL_DATA_WIDTH } {
	# Procedure called to update C_S_AXI_CNTRL_DATA_WIDTH when any of the dependent parameters in the arguments change
}

proc validate_PARAM_VALUE.C_S_AXI_CNTRL_DATA_WIDTH { PARAM_VALUE.C_S_AXI_CNTRL_DATA_WIDTH } {
	# Procedure called to validate C_S_AXI_CNTRL_DATA_WIDTH
	return true
}

proc update_PARAM_VALUE.Q_FRACTIONAL { PARAM_VALUE.Q_FRACTIONAL } {
	# Procedure called to update Q_FRACTIONAL when any of the dependent parameters in the arguments change
}

proc validate_PARAM_VALUE.Q_FRACTIONAL { PARAM_VALUE.Q_FRACTIONAL } {
	# Procedure called to validate Q_FRACTIONAL
	return true
}

proc update_PARAM_VALUE.Q_INTEGER { PARAM_VALUE.Q_INTEGER } {
	# Procedure called to update Q_INTEGER when any of the dependent parameters in the arguments change
}

proc validate_PARAM_VALUE.Q_INTEGER { PARAM_VALUE.Q_INTEGER } {
	# Procedure called to validate Q_INTEGER
	return true
}


proc update_MODELPARAM_VALUE.C_S_AXI_CNTRL_DATA_WIDTH { MODELPARAM_VALUE.C_S_AXI_CNTRL_DATA_WIDTH PARAM_VALUE.C_S_AXI_CNTRL_DATA_WIDTH } {
	# Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
	set_property value [get_property value ${PARAM_VALUE.C_S_AXI_CNTRL_DATA_WIDTH}] ${MODELPARAM_VALUE.C_S_AXI_CNTRL_DATA_WIDTH}
}

proc update_MODELPARAM_VALUE.C_S_AXI_CNTRL_ADDR_WIDTH { MODELPARAM_VALUE.C_S_AXI_CNTRL_ADDR_WIDTH PARAM_VALUE.C_S_AXI_CNTRL_ADDR_WIDTH } {
	# Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
	set_property value [get_property value ${PARAM_VALUE.C_S_AXI_CNTRL_ADDR_WIDTH}] ${MODELPARAM_VALUE.C_S_AXI_CNTRL_ADDR_WIDTH}
}

proc update_MODELPARAM_VALUE.C_S_AXIS_TDATA_WIDTH { MODELPARAM_VALUE.C_S_AXIS_TDATA_WIDTH PARAM_VALUE.C_S_AXIS_TDATA_WIDTH } {
	# Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
	set_property value [get_property value ${PARAM_VALUE.C_S_AXIS_TDATA_WIDTH}] ${MODELPARAM_VALUE.C_S_AXIS_TDATA_WIDTH}
}

proc update_MODELPARAM_VALUE.Q_INTEGER { MODELPARAM_VALUE.Q_INTEGER PARAM_VALUE.Q_INTEGER } {
	# Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
	set_property value [get_property value ${PARAM_VALUE.Q_INTEGER}] ${MODELPARAM_VALUE.Q_INTEGER}
}

proc update_MODELPARAM_VALUE.Q_FRACTIONAL { MODELPARAM_VALUE.Q_FRACTIONAL PARAM_VALUE.Q_FRACTIONAL } {
	# Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
	set_property value [get_property value ${PARAM_VALUE.Q_FRACTIONAL}] ${MODELPARAM_VALUE.Q_FRACTIONAL}
}

