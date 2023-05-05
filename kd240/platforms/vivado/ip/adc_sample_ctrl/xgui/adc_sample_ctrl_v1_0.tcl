# Definitional proc to organize widgets for parameters.
proc init_gui { IPINST } {
  ipgui::add_param $IPINST -name "Component_Name"
  #Adding Page
  set Page_0 [ipgui::add_page $IPINST -name "Page 0"]
  ipgui::add_param $IPINST -name "ADC_SCLKS_PER_SAMPLE" -parent ${Page_0}
  ipgui::add_param $IPINST -name "CLK_PERIOD_NS" -parent ${Page_0}
  ipgui::add_param $IPINST -name "MAX_CHANGE_CLKS" -parent ${Page_0}
  ipgui::add_param $IPINST -name "SCLK_PERIOD_NS" -parent ${Page_0}


}

proc update_PARAM_VALUE.ADC_SCLKS_PER_SAMPLE { PARAM_VALUE.ADC_SCLKS_PER_SAMPLE } {
	# Procedure called to update ADC_SCLKS_PER_SAMPLE when any of the dependent parameters in the arguments change
}

proc validate_PARAM_VALUE.ADC_SCLKS_PER_SAMPLE { PARAM_VALUE.ADC_SCLKS_PER_SAMPLE } {
	# Procedure called to validate ADC_SCLKS_PER_SAMPLE
	return true
}

proc update_PARAM_VALUE.CLK_PERIOD_NS { PARAM_VALUE.CLK_PERIOD_NS } {
	# Procedure called to update CLK_PERIOD_NS when any of the dependent parameters in the arguments change
}

proc validate_PARAM_VALUE.CLK_PERIOD_NS { PARAM_VALUE.CLK_PERIOD_NS } {
	# Procedure called to validate CLK_PERIOD_NS
	return true
}

proc update_PARAM_VALUE.MAX_CHANGE_CLKS { PARAM_VALUE.MAX_CHANGE_CLKS } {
	# Procedure called to update MAX_CHANGE_CLKS when any of the dependent parameters in the arguments change
}

proc validate_PARAM_VALUE.MAX_CHANGE_CLKS { PARAM_VALUE.MAX_CHANGE_CLKS } {
	# Procedure called to validate MAX_CHANGE_CLKS
	return true
}

proc update_PARAM_VALUE.SCLK_PERIOD_NS { PARAM_VALUE.SCLK_PERIOD_NS } {
	# Procedure called to update SCLK_PERIOD_NS when any of the dependent parameters in the arguments change
}

proc validate_PARAM_VALUE.SCLK_PERIOD_NS { PARAM_VALUE.SCLK_PERIOD_NS } {
	# Procedure called to validate SCLK_PERIOD_NS
	return true
}


proc update_MODELPARAM_VALUE.ADC_SCLKS_PER_SAMPLE { MODELPARAM_VALUE.ADC_SCLKS_PER_SAMPLE PARAM_VALUE.ADC_SCLKS_PER_SAMPLE } {
	# Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
	set_property value [get_property value ${PARAM_VALUE.ADC_SCLKS_PER_SAMPLE}] ${MODELPARAM_VALUE.ADC_SCLKS_PER_SAMPLE}
}

proc update_MODELPARAM_VALUE.MAX_CHANGE_CLKS { MODELPARAM_VALUE.MAX_CHANGE_CLKS PARAM_VALUE.MAX_CHANGE_CLKS } {
	# Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
	set_property value [get_property value ${PARAM_VALUE.MAX_CHANGE_CLKS}] ${MODELPARAM_VALUE.MAX_CHANGE_CLKS}
}

proc update_MODELPARAM_VALUE.CLK_PERIOD_NS { MODELPARAM_VALUE.CLK_PERIOD_NS PARAM_VALUE.CLK_PERIOD_NS } {
	# Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
	set_property value [get_property value ${PARAM_VALUE.CLK_PERIOD_NS}] ${MODELPARAM_VALUE.CLK_PERIOD_NS}
}

proc update_MODELPARAM_VALUE.SCLK_PERIOD_NS { MODELPARAM_VALUE.SCLK_PERIOD_NS PARAM_VALUE.SCLK_PERIOD_NS } {
	# Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
	set_property value [get_property value ${PARAM_VALUE.SCLK_PERIOD_NS}] ${MODELPARAM_VALUE.SCLK_PERIOD_NS}
}

