# Copyright (C) 2022, Advanced Micro Devices, Inc.
# SPDX-License-Identifier: Apache-2.0

source settings.tcl

set VITIS_LIBS ../../../../../common/Vitis_Libraries/motorcontrol
set QEI_IP ${VITIS_LIBS}/L1/tests/FOC/IP_QEI

set PROJ "hls_qei.prj"
set SOLN "sol1"

if {![info exists CLKP]} {
  set CLKP 10
}

if {![info exists XPART]} {
  set XPART xck24-ubva530-2LV-c
}

open_project -reset $PROJ

add_files "${QEI_IP}/src/ip_qei.cpp" -cflags "-I${VITIS_LIBS}/L1/include/hw -I${QEI_IP}/src"
add_files -tb "${QEI_IP}/src/test_ip_qei.cpp" -cflags "-I${VITIS_LIBS}/L1/include/hw -I${QEI_IP}/src"
set_top hls_qei

open_solution -reset $SOLN

set_part $XPART
create_clock -period $CLKP
set_clock_uncertainty 1.25

csynth_design
export_design -rtl verilog -format ip_catalog

exit
