# Copyright (C) 2022, Advanced Micro Devices, Inc.
# SPDX-License-Identifier: Apache-2.0

RM = rm -rf
HLS = vitis_hls
CLKP ?= 10
XPART ?= xck24-ubva530-2LV-c


.PHONY: help
help:
	@echo 'Usage:'
	@echo ''
	@echo '  make ip'
	@echo '    Generate IP for PWM GEN'
	@echo ''

.PHONY: all
all: ip

ip:  
	@echo 'set CLKP $(CLKP)' >> ./settings.tcl
	@echo 'set XPART $(XPART)' >> ./settings.tcl
	$(HLS) -f run_hls.tcl

.PHONY: clean
clean:
	$(RM) settings.tcl *.prj *.log

