# (C) Copyright 2020 - 2022 Xilinx, Inc.
# SPDX-License-Identifier: Apache-2.0

RM = rm -rf
HLS = $(XILINX_VITIS)/bin/vitis
CLKP ?= 3.3
XPART ?= xck26-sfvc784-2LV-c


.PHONY: help
help:
	@echo 'Usage:'
	@echo ''
	@echo '  make ip'
	@echo '    Generate IP for Single Sensor ISP'
	@echo ''

.PHONY: all
all: ip

ip:  
	CLKP=$(CLKP) XPART=$(XPART) $(HLS) -s run_hls.py

.PHONY: clean
clean:
	$(RM) settings.tcl *.prj *.log isppipeline.prj

