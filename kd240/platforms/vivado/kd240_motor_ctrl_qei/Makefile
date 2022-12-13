# Copyright (C) 2022, Advanced Micro Devices, Inc.
# SPDX-License-Identifier: Apache-2.0

RM = rm -rf
VIVADO = $(XILINX_VIVADO)/bin/vivado
JOBS ?= 8

VIV_DESIGN = kd240_motor_ctrl_qei

VIV_PRJ_DIR = project
VIV_SCRIPTS_DIR = scripts

VIV_XSA = $(VIV_PRJ_DIR)/$(VIV_DESIGN).xsa
VIV_SRC = $(VIV_SCRIPTS_DIR)/main.tcl
QEI_DIR= ../ip/qei
QEI_IP = $(QEI_DIR)/*.prj/sol1/impl/export.zip
FOC_DIR= ../ip/foc
FOC_IP = $(FOC_DIR)/*.prj/sol1/impl/export.zip
SVPWM_DIR= ../ip/svpwm
SVPWM_IP = $(SVPWM_DIR)/*.prj/sol1/impl/export.zip


.PHONY: help
help:
	@echo 'Usage:'
	@echo ''
	@echo '  make xsa'
	@echo '    Generate extensible xsa for platform generation'
	@echo ''

.PHONY: all
all: xsa

xsa: $(VIV_XSA)
$(VIV_XSA): $(VIV_SRC) $(QEI_IP) $(FOC_IP) $(SVPWM_IP)
	$(VIVADO) -mode batch -notrace -source $(VIV_SRC) -tclargs -jobs $(JOBS)

$(QEI_IP):
	make -C $(QEI_DIR) ip 

$(FOC_IP):
	make -C $(FOC_DIR) ip 

$(SVPWM_IP):
	make -C $(SVPWM_DIR) ip 

.PHONY: clean
clean:
	$(RM) $(VIV_PRJ_DIR) vivado* *dynamic* *.log *.xpe
	
.PHONY: clean_ip
clean_ip:	
	make -C $(QEI_DIR) clean
	make -C $(FOC_DIR) clean
	make -C $(SVPWM_DIR) clean

