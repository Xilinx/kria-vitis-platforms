# (C) Copyright 2020 - 2022 Xilinx, Inc.
# SPDX-License-Identifier: Apache-2.0

RM = rm -rf
VIVADO = $(XILINX_VIVADO)/bin/vivado

VIV_DESIGN = kv260_ispMipiRx_rpiMipiRx_DP

VIV_PRJ_DIR = project
VIV_SCRIPTS_DIR = scripts

VIV_XSA = $(VIV_PRJ_DIR)/$(VIV_DESIGN).xsa
VIV_SRC = $(VIV_SCRIPTS_DIR)/main.tcl
ISP_DIR= ./ip/isp_single_kv260
ISP_IP = isppipeline.prj 

.PHONY: help
help:
	@echo 'Usage:'
	@echo ''
	@echo '  make xsa'
	@echo '    Generate extensible xsa for platform generation'
	@echo ''

.PHONY: all
all: xsa

$(ISP_IP):
	make -C $(ISP_DIR) ip

xsa: $(VIV_XSA)
$(VIV_XSA): $(VIV_SRC) $(ISP_IP)
	$(VIVADO) -mode batch -notrace -source $(VIV_SRC)

.PHONY: clean
clean:
	$(RM) $(VIV_PRJ_DIR) vivado* .Xil *dynamic* *.log *.xpe
	make -C $(ISP_DIR) clean

