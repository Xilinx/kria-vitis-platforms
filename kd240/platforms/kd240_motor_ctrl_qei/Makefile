# Copyright (C) 2022 - 2023, Advanced Micro Devices, Inc.
# SPDX-License-Identifier: Apache-2.0

RM = rm -rf
VIVADO = $(XILINX_VIVADO)/bin/vivado
JOBS ?= 8

VIV_DESIGN = kd240_motor_ctrl_qei

VIV_PRJ_DIR = project
VIV_SCRIPTS_DIR = scripts

VIV_XSA = $(VIV_PRJ_DIR)/$(VIV_DESIGN).xsa
VIV_SRC = $(VIV_SCRIPTS_DIR)/main.tcl

VIV_TIMING_ERROR = "CRITICAL WARNING: \[Timing 38-282\] The design failed to meet the timing requirements"

QEI_DIR= ../ip/qei
QEI_IP = $(QEI_DIR)/*.prj/sol1/impl/export.zip
FOC_DIR= ../ip/foc
FOC_IP = $(FOC_DIR)/*.prj/sol1/impl/export.zip
SVPWM_DIR= ../ip/svpwm_duty
SVPWM_IP = $(SVPWM_DIR)/*.prj/sol1/impl/export.zip
PWM_GEN_DIR= ../ip/pwm_gen
PWM_GEN_IP = $(PWM_GEN_DIR)/*.prj/sol1/impl/export.zip

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
$(VIV_XSA): $(VIV_SRC) $(QEI_IP) $(FOC_IP) $(SVPWM_IP) $(PWM_GEN_IP)
	$(VIVADO) -mode batch -notrace -source $(VIV_SRC) -tclargs -jobs $(JOBS)
	@grep $(VIV_TIMING_ERROR) vivado.log; if [ $$? = 0 ]; then exit 1; fi

$(QEI_IP):
	make -C $(QEI_DIR) ip 

$(FOC_IP):
	make -C $(FOC_DIR) ip 

$(SVPWM_IP):
	make -C $(SVPWM_DIR) ip 

$(PWM_GEN_IP):
	make -C $(PWM_GEN_DIR) ip 

.PHONY: clean
clean:
	$(RM) $(VIV_PRJ_DIR) vivado* *dynamic* *.log *.xpe
	
.PHONY: clean_ip
clean_ip:	
	make -C $(QEI_DIR) clean
	make -C $(FOC_DIR) clean
	make -C $(SVPWM_DIR) clean
	make -C $(PWM_GEN_DIR) clean

.PHONY: clean_all
clean_all: clean clean_ip

