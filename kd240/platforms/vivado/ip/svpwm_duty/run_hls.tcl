# Copyright (C) 2022, Advanced Micro Devices, Inc.
# SPDX-License-Identifier: Apache-2.0

source settings.tcl 

set VITIS_LIBS ../../../../../common/Vitis_Libraries/motorcontrol
set SVPWM_IP ${VITIS_LIBS}/L1/tests/IP_SVPWM

set PROJ "ip_svpwm_duty.prj"
set SOLN "sol1"

if {![info exists CLKP]} {
  set CLKP 10
}

if {![info exists XPART]} {
  set XPART xck24-ubva530-2LV-c
}

open_project -reset $PROJ

add_files "${SVPWM_IP}/src/ip_svpwm.cpp" -cflags "-I${VITIS_LIBS}/L1/include/hw -I${SVPWM_IP}/src"
add_files -tb "${SVPWM_IP}/src/test_svpwm.cpp" -cflags "-I${VITIS_LIBS}/L1/include/hw -I${SVPWM_IP}/src" 
set_top hls_svpwm_duty

open_solution -reset $SOLN

set_part $XPART
create_clock -period $CLKP
set_clock_uncertainty 1.25

csynth_design
config_export -ipname hls_svpwm_duty
export_design -rtl verilog -format ip_catalog

exit