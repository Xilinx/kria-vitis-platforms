# (C) Copyright 2020 - 2021 Xilinx, Inc.
# SPDX-License-Identifier: Apache-2.0

RM = rm -rf
VIVADO = $(XILINX_VIVADO)/bin/vivado
JOBS ?= 8

VIV_DESIGN = kv260_ispMipiRx_vmixDP

VIV_PRJ_DIR = project
VIV_SCRIPTS_DIR = scripts

VIV_XSA = $(VIV_PRJ_DIR)/$(VIV_DESIGN).xsa
VIV_SRC = $(VIV_SCRIPTS_DIR)/main.tcl

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
$(VIV_XSA): $(VIV_SRC)
	$(VIVADO) -mode batch -notrace -source $(VIV_SRC) -tclargs -jobs $(JOBS)

.PHONY: clean
clean:
	$(RM) $(VIV_PRJ_DIR) vivado* .Xil *dynamic* *.log *.xpe

