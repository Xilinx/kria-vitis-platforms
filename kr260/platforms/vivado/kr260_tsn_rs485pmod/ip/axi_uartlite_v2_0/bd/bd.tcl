
proc init {cellpath otherInfo } {
# cell initialization here
# Not needed, can be removed
set paramList "C_S_AXI_ACLK_FREQ_HZ_d"
set cell_handle [get_bd_cells $cellpath]
bd::mark_propagate_overrideable $cell_handle $paramList
set clk_pin_handle [get_bd_pins $cellpath/s_axi_aclk]
set_property CONFIG.FREQ_HZ 100000000 $clk_pin_handle
}


proc post_propagate {cellpath otherInfo } {
    set cell_handle [get_bd_cells $cellpath]
    ##set intf_handle [get_bd_intf_pins $cellpath/aclk]
   
    ## Assign AXI clock frequency to parameter 
    set clk_pin_handle [get_bd_pins $cellpath/s_axi_aclk]
    set freq [get_property CONFIG.FREQ_HZ $clk_pin_handle]
    if { $freq == "" } {
      set_property MSG.ERROR "The AXI Uart Lite AXI CLOCK Frequency is not propagated from AXI Interface" $cell_handle
    } else {
      set freq_Hz [expr int($freq)] 
      set freq_MHz [expr $freq_Hz/1000000.0]
      set_property CONFIG.C_S_AXI_ACLK_FREQ_HZ_d $freq_MHz $cell_handle
    }

    # standard parameter propagation here

}
proc post_config_ip {cellpath otherInfo } {
}
