# Copyright (C) 2020 - 2022 Xilinx, Inc.
# Copyright (C) 2023 Advanced Micro Devices, Inc.
# SPDX-License-Identifier: Apache-2.0

CP = cp -rf
PWD = $(shell readlink -f .)
MKDIR = mkdir -p
RM = rm -rf
XSCT = $(XILINX_VITIS)/bin/xsct
JOBS ?= 8

BOARD_LIST = k26 kv260 kr260 kd240
BOARD = $(word 1,$(subst _, ,$(PFM)))
$(foreach b,$(BOARD_LIST),$(eval include $(b)/$(b).mk))

# the platform directory has to be an absolute path when passed to v++
PFM_DIR = $(PWD)/$(BOARD)/platforms
PFM_VER = 202320_1
PFM_NAME = xilinx_$(PFM)_$(PFM_VER)
PFM_XPFM = $(PFM_DIR)/$(PFM_NAME)/$(PFM).xpfm
PFM_PRJ_DIR = $(PFM_DIR)/xsct/$(PFM)/$(PFM)/export/$(PFM)
PFM_TCL = $(PWD)/common/scripts/pfm.tcl

VIV_DIR = $(PFM_DIR)/vivado/$(PFM)
VIV_XSA = $(VIV_DIR)/project/$(PFM).xsa

VITIS_DIR = $(BOARD)/overlays/examples
VITIS_OVERLAY_DIR = $(VITIS_DIR)/$(OVERLAY)
VITIS_OVERLAY_BIT = $(VITIS_OVERLAY_DIR)/binary_container_1/link/int/system.bit

.PHONY: help
help:
	@echo 'Usage:'
	@echo ''
	@echo '  make overlay OVERLAY=<val>'
	@echo '    Build the Vitis application overlay.'
	@echo ''
	@echo '    OVERLAY: valid options'
	@echo -n '        '
	@echo '${OVERLAY_LIST}' | sed -r 's/ /\n        /g'
	@echo ''
	@echo '  make platform PFM=<val> JOBS=<n>'
	@echo '    Build the Vitis platform.'
	@echo ''
	@echo '    JOBS: optional param to set number of synthesis jobs (default 8)'
	@echo '    PFM: valid options'
	@echo -n '        '
	@echo '${PFM_LIST}' | sed -r 's/ /\n        /g'
	@echo ''
	@echo '  make clean'
	@echo '    Clean runs'
	@echo ''

.PHONY: overlay
overlay: $(VITIS_OVERLAY_BIT)
$(VITIS_OVERLAY_BIT): $(PFM_XPFM)
	@valid=0; \
	for o in $(OVERLAY_LIST); do \
	  if [ "$$o" = "$(OVERLAY)" ]; then \
	    valid=1; \
	    break; \
	  fi \
	done; \
	if [ "$$valid" -ne 1 ]; then \
	  echo 'Invalid parameter OVERLAY=$(OVERLAY). Choose one of: $(OVERLAY_LIST)'; \
	  exit 1; \
	fi; \
	echo 'Build $(OVERLAY) Vitis overlay using platform $(PFM)'; \
	$(MAKE) -C $(VITIS_OVERLAY_DIR) all PLATFORM=$(PFM_XPFM)

.PHONY: platform
platform: $(PFM_XPFM)
$(PFM_XPFM): $(VIV_XSA)
	@valid=0; \
	for p in $(PFM_LIST); do \
	  if [ "$$p" = "$(PFM)" ]; then \
	    valid=1; \
	    break; \
	  fi \
	done; \
	if [ "$$valid" -ne 1 ]; then \
	  echo 'Invalid parameter PFM=$(PFM). Choose one of: $(PFM_LIST)'; \
	  exit 1; \
	fi; \
	echo 'Create Vitis platform $(PFM)'; \
	cd $(PFM_DIR); \
	$(XSCT) $(PFM_TCL) -xsa $(VIV_XSA); \
	$(CP) $(PFM_PRJ_DIR) $(PFM_NAME); \
	cd -

$(VIV_XSA):
	make -C $(VIV_DIR) xsa JOBS=$(JOBS)

.PHONY: clean
clean:
	$(foreach o, $(OVERLAY_LIST), $(MAKE) -C $(VITIS_DIR)/$(o) clean;)
	$(foreach p, $(PFM_LIST), $(MAKE) -C $(VIV_DIR) clean;)
