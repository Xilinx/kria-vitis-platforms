# (C) Copyright 2020 - 2022 Xilinx, Inc.
# SPDX-License-Identifier: Apache-2.0

DIR_PRJ = $(shell pwd)
RM = rm -rf
VIVADO_ROOT := $(XILINX_VIVADO)
VIVADO:=${VIVADO_ROOT}/bin/vivado
XBU = xclbinutil
XBU_FLAGS = --remove-section BITSTREAM --force
CONFIG_FILE = system.cfg

# Kernel name
GAUSSIAN_KERNEL = gaussian_otsu_accel
KERNEL = preprocess_accel
CCA_KERNEL = cca_custom_accel

GAUSSIAN_KERNEL_XO = $(GAUSSIAN_KERNEL).xo
KERNEL_XO = $(KERNEL)/$(KERNEL).xo
CCA_KERNEL_XO = $(CCA_KERNEL).xo
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

XOCC_OPTS = -t ${TARGET} --platform ${PLATFORM} --save-temps --config $(CONFIG_FILE) --config ${DIR_PRJ}/system.cfg

XFOPENCV_INCDIR = ../../Vitis_Libraries/vision/L1/include/
KERNEL_XO_FLAGS =  -I$(XFOPENCV_INCDIR) -D__SDSVHLS__ -DHLS_NO_XIL_FPO_LIB --advanced.prop kernel.preprocess_accel.kernel_flags="-std=c++0x"
JOBS = 32

VPP_XO_FLAGS = -t hw --platform $(PLATFORM) \
				-j $(JOBS) $(KERNEL_XO_FLAGS)


defect_detect_xo = $(GAUSSIAN_KERNEL_XO)
defect_detect_xo += $(KERNEL_XO)
defect_detect_xo += $(CCA_KERNEL_XO)

.PHONY: all clean package

all : binary_container_1/defect-detect.xclbin package

# 	# Rules
$(GAUSSIAN_KERNEL_XO): xf_gaussian_otsu_accel.cpp
	@echo "Compiling Kernel: gaussian_otsu_accel"
	@mkdir -p $(@D)
	-@$(RM) $@
	$(VPP) -k $(GAUSSIAN_KERNEL) $(VPP_XO_FLAGS) -o '$@' '$<'
	-@$(RM) .Xil

$(KERNEL_XO): xf_preprocess_accel.cpp
	@echo "Compiling Kernel: pre-process_accel"
	@mkdir -p $(@D)
	-@$(RM) $@
	$(VPP) -k $(KERNEL) $(VPP_XO_FLAGS) -o '$@' '$<'
	-@$(RM) .Xil

$(CCA_KERNEL_XO): xf_cca_custom_accel.cpp
	@echo "Compiling Kernel: cca_accel"
	@mkdir -p $(@D)
	-@$(RM) $@
	$(VPP) -k $(CCA_KERNEL) $(VPP_XO_FLAGS) -o '$@' '$<'
	-@$(RM) .Xil

binary_container_1/defect-detect.xclbin: $(defect_detect_xo)
	$(VPP) $(XOCC_OPTS) -l --temp_dir binary_container_1 --log_dir binary_container_1/logs --remote_ip_cache binary_container_1/ip_cache -o "$@" $(+)
	$(XBU) $(XBU_FLAGS) --input $@ --output strip.xclbin

package:
	-@mkdir -p binary_container_1/sd_card
	cp ./binary_*/link/vivado/vpl/prj/prj*/sources_1/bd/*/hw_handoff/*.hwh ./binary_*/sd_card
	cp ./binary_*/link/vivado/vpl/prj/prj.runs/impl_1/*.bit ./binary_*/sd_card
	cp ./binary_*/*.xclbin ./binary_*/sd_card

#.PHONY: clean
clean:
	-$(RM) $(KERNEL) *.log _x *.jou v++* *.xclbin *.ini *.xsa
	-$(RM) $(GAUSSIAN_KERNEL_XO) *.compile_summary
	-$(RM) binary_container_1
	-$(RM) packaged* tmp_*
