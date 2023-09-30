# (C) Copyright 2020 - 2022 Xilinx, Inc.
# SPDX-License-Identifier: Apache-2.0

DIR_PRJ = $(shell pwd)
RM = rm -rf
VIVADO_ROOT := $(XILINX_VIVADO)
VIVADO:=${VIVADO_ROOT}/bin/vivado
XBU = xclbinutil
XBU_FLAGS = --remove-section BITSTREAM --force

# Kernel name
KERNEL = pp_pipeline_accel
KERNEL_XO = $(KERNEL)/$(KERNEL).xo
TARGET = hw
#---------------------------------------------------------------------
# vitis common setup
.PHONY: help
help::
	@echo ""
	@echo "Makefile Usage:"
	@echo ""
	@echo "make all PLATFORM=<FPGA platform>"
	@echo "Command to generate the xclbin and bit"
	@echo ""
	@echo "make clean"
	@echo "Command to remove all the generated files"
	@echo ""

# # v++ flags
VPP ?= v++

XOCC_OPTS = -t ${TARGET} --platform ${PLATFORM} --save-temps --config ${DIR_PRJ}/prj_conf/prj_config_1dpu

dpu_HDLSRCS=kernel_xml/dpu/kernel.xml\
	     scripts/package_dpu_kernel.tcl\
	     scripts/gen_dpu_xo.tcl\
	     ../dpu_ip/Vitis/dpu/hdl/DPUCZDX8G.v\
	     ../dpu_ip/Vitis/dpu/inc/arch_def.vh\
	     ../dpu_ip/Vitis/dpu/xdc/*.xdc\
	     ../dpu_ip/DPUCZDX8G_*/hdl/DPUCZDX8G_*_dpu.sv\
	     ../dpu_ip/DPUCZDX8G_*/inc/function.vh\
             ../dpu_ip/DPUCZDX8G_*/inc/arch_para.vh

dpu_TCL=scripts/gen_dpu_xo.tcl
DPU_KERN_NAME = DPUCZDX8G
dpu_xo = binary_container_1/dpu.xo

JOBS = 32
VPP_XO_FLAGS = -t hw --platform $(PLATFORM) \
	--report_level estimate -j $(JOBS) $(KERNEL_XO_FLAGS)

dpu_pp_xo = $(dpu_xo)

.PHONY: all clean package

all : binary_container_1/system.xclbin package

# 	# Rules
$(KERNEL_XO): xf_pp_pipeline_accel.cpp
	@mkdir -p $(@D)
	-@$(RM) $@
	$(VPP) $(VPP_XO_FLAGS) -o $@ $<
	-@$(RM) .Xil

binary_container_1/dpu.xo: $(dpu_HDLSRCS)
	@mkdir -p $(@D)
	-@$(RM) $@
	$(VIVADO) -mode batch -source $(dpu_TCL) -tclargs $@ $(DPU_KERN_NAME) ${TARGET} mpsoc

binary_container_1/system.xclbin: $(dpu_pp_xo)
	$(VPP) $(XOCC_OPTS) -l --temp_dir binary_container_1 --log_dir binary_container_1/logs --remote_ip_cache binary_container_1/ip_cache -o "$@" $(+)
	$(XBU) $(XBU_FLAGS) --input $@ --output strip.xclbin
	@mv strip.xclbin $@

package:
	-@mkdir -p binary_container_1/sd_card
	cp ./binary_*/link/vivado/vpl/prj/prj*/sources_1/bd/*/hw_handoff/*.hwh ./binary_*/sd_card
	cp ./binary_*/link/vivado/vpl/prj/prj.gen/sources_1/bd/*/ip/*_DPUCZDX8G_1_0/arch.json ./binary_*/sd_card
	cp ./binary_*/link/vivado/vpl/prj/prj.runs/impl_1/*.bit ./binary_*/sd_card
	cp ./binary_*/*.xclbin ./binary_*/sd_card

#.PHONY: clean
clean:
	-$(RM) $(KERNEL) *.log _x *.jou v++* *.xclbin *.ini *.xsa
	-$(RM) binary_container_1
	-$(RM) DPU_PP packaged* tmp_*


