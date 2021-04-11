# (C) Copyright 2020 - 2021 Xilinx, Inc.
# SPDX-License-Identifier: Apache-2.0

CP = cp -f

PWD = $(shell readlink -f .)

# the platform directory has to be an absolute path when passed to v++
PFM_DIR = $(PWD)/platforms
PFM_VER = 202110_1

# valid platforms / overlays
PFM_LIST = kv260_ispMipiRx_vcu_DP kv260_vcuDecode_vmixDP kv260_ispMipiRx_vmixDP kv260_ispMipiRx_DP
OVERLAY_LIST = smartcam aibox-reid defect-detect nlp-smartvision

# override platform name based on overlay
ifeq ($(OVERLAY),smartcam)
  override PFM = kv260_ispMipiRx_vcu_DP
endif
ifeq ($(OVERLAY),aibox-reid)
  override PFM = kv260_vcuDecode_vmixDP
endif
ifeq ($(OVERLAY),defect-detect)
  override PFM = kv260_ispMipiRx_vmixDP
endif
ifeq ($(OVERLAY),nlp-smartvision)
  override PFM = kv260_ispMipiRx_DP
endif

PFM_XPFM = $(PFM_DIR)/xilinx_$(PFM)_$(PFM_VER)/$(PFM).xpfm

VITIS_DIR = overlays/examples
VITIS_OVERLAY_DIR = $(VITIS_DIR)/$(OVERLAY)
VITIS_OVERLAY_BIT = $(VITIS_OVERLAY_DIR)/binary_container_1/link/int/system.bit

.PHONY: help
help:
	@echo 'Usage:'
	@echo ''
	@echo '  make overlay OVERLAY=<val>'
	@echo '    Build the Vitis application overlay.'
	@echo ''
	@echo '    Valid options for OVELRAY: ${OVERLAY_LIST}'
	@echo ''
	@echo '  make platform PFM=<val> JOBS=<n>'
	@echo '    Build the Vitis platform.'
	@echo ''
	@echo '    Valid options for PFM: ${PFM_LIST}'
	@echo '    JOBS: optional param to set number of synthesis jobs (default 8)'
	@echo ''
	@echo '  make clean'
	@echo '    Clean runs'
	@echo ''

.PHONY: overlay
overlay: $(VITIS_OVERLAY_BIT)
$(VITIS_OVERLAY_BIT): $(PFM_XPFM)
	@val=0; \
	for o in $(OVERLAY_LIST); do \
	  if [ "$$o" == "$(OVERLAY)" ]; then \
	    echo 'Build $(OVERLAY) Vitis overlay using platform $(PFM)'; \
	    $(MAKE) -C $(VITIS_OVERLAY_DIR) all PLATFORM=$(PFM_XPFM); \
	    val=1; \
	  fi \
	done; \
	if [ "$$val" -ne 1 ]; then \
	  echo 'Invalid parameter OVERLAY=$(OVERLAY). Choose one of: $(OVERLAY_LIST)'; \
	  false; \
	fi


.PHONY: platform
platform: $(PFM_XPFM)
$(PFM_XPFM):
	@val=0; \
	for p in $(PFM_LIST); do \
	  if [ "$$p" == "$(PFM)" ]; then \
	    echo 'Create Vitis platform $(PFM)'; \
	    $(MAKE) -C $(PFM_DIR) platform PLATFORM=$(PFM) VERSION=$(PFM_VER); \
	    val=1; \
	  fi \
	done; \
	if [ "$$val" -ne 1 ]; then \
	  echo 'Invalid parameter PFM=$(PFM). Choose one of: $(PFM_LIST)'; \
	  false; \
	fi

.PHONY: clean
clean:
	$(foreach o, $(OVERLAY_LIST), $(MAKE) -C $(VITIS_DIR)/$(o) clean;)
	$(foreach p, $(PFM_LIST), $(MAKE) -C $(PFM_DIR) clean PLATFORM=$(p) VERSION=$(PFM_VER);)
