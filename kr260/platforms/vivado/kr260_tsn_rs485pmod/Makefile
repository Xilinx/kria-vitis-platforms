# (C) Copyright 2020 - 2022 Xilinx, Inc.
# SPDX-License-Identifier: Apache-2.0

RM = rm -rf
CP = cp -rf
VIVADO = $(XILINX_VIVADO)/bin/vivado
JOBS ?= 8

IP = ip/axi_uartlite_v2_0

VIV_DESIGN = kr260_tsn_rs485pmod

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

ip: $(IP)
$(IP): $(XILINX_VIVADO)/data/ip/xilinx/axi_uartlite_v2_0
	$(CP) $(XILINX_VIVADO)/data/ip/xilinx/axi_uartlite_v2_0/ ./ip
	patch -d ip/axi_uartlite_v2_0 -p1 -i ../axi_uartlite_v2_0_rs485.patch
	

xsa: $(IP) $(VIV_XSA)
$(VIV_XSA): $(VIV_SRC)
	$(VIVADO) -mode batch -notrace -source $(VIV_SRC) -tclargs -jobs $(JOBS)

.PHONY: clean
clean:
	$(RM) $(VIV_PRJ_DIR) vivado* .Xil *dynamic* *.log *.xpe	 ip/axi_uartlite_v2_0

