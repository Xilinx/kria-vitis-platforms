# Copyright (C) 2022, Advanced Micro Devices, Inc.
# SPDX-License-Identifier: Apache-2.0

source settings.tcl 

set VITIS_LIBS ../../../../../common/Vitis_Libraries/motorcontrol
set FOC_IP ${VITIS_LIBS}/L1/tests/FOC/IP_FOC

set PROJ "ip_foc_periodic.prj"
set SOLN "sol1"

if {![info exists CLKP]} {
  set CLKP 10
}

if {![info exists XPART]} {
  set XPART xck24-ubva530-2LV-c
}

open_project -reset $PROJ

add_files "${FOC_IP}/src/ip_foc.cpp" -cflags "-I${VITIS_LIBS}/L1/include/hw -I${FOC_IP}/src"
add_files -tb "${FOC_IP}/src/test_ip_foc.cpp" -cflags "-I${VITIS_LIBS}/L1/include/hw -I${FOC_IP}/src" 
set_top hls_foc_periodic_ap_fixed

open_solution -reset $SOLN

set_part $XPART
create_clock -period $CLKP
set_clock_uncertainty 1.25

csynth_design
config_export -ipname hls_foc_periodic
export_design -rtl verilog -format ip_catalog

exit
