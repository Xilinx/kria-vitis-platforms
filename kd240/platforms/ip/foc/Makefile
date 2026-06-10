# Copyright (C) 2022 - 2026, Advanced Micro Devices, Inc.
# SPDX-License-Identifier: Apache-2.0

RM = rm -rf
VITIS = $(XILINX_VITIS)/bin/vitis
CLKP ?= 10
XPART ?= xck24-ubva530-2LV-c


.PHONY: help
help:
	@echo 'Usage:'
	@echo ''
	@echo '  make ip'
	@echo '    Generate IP for FOC'
	@echo ''

.PHONY: all
all: ip

ip:  
	CLKP=$(CLKP) XPART=$(XPART) $(VITIS) -s run_hls.py

.PHONY: clean
clean:
	$(RM) workspace .ip_done *.log

