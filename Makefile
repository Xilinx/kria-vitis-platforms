# Copyright (C) 2020 - 2022 Xilinx, Inc.
# Copyright (C) 2023 - 2024 Advanced Micro Devices, Inc.
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
PFM_VER = 202410_1
PFM_NAME = xilinx_$(PFM)_$(PFM_VER)
PFM_XPFM = $(PFM_DIR)/$(PFM_NAME)/$(PFM).xpfm
PFM_PRJ_DIR = $(PFM_DIR)/xsct/$(PFM)/$(PFM)/export/$(PFM)
PFM_TCL = $(PWD)/common/scripts/pfm.tcl

VIV_DIR = $(PFM_DIR)/$(PFM)
VIV_XSA = $(VIV_DIR)/project/$(PFM).xsa
VIV_BIT = $(VIV_DIR)/project/$(PFM).runs/impl_1/$(PFM)_wrapper.bit

VITIS_DIR = $(BOARD)/overlays
VITIS_OVERLAY_DIR = $(VITIS_DIR)/$(OVERLAY)
VITIS_OVERLAY_BIT = $(VITIS_OVERLAY_DIR)/binary_container_1/link/int/system.bit
VITIS_OVERLAY_XCLBIN = $(VITIS_OVERLAY_DIR)/binary_container_1/system.xclbin

BIF_TEMPLATE = $(PWD)/common/firmware/template.bif
JSON_TEMPLATE = $(PWD)/common/firmware/shell.json

FW_TOP_DIR = $(PWD)/$(BOARD)/firmware
FW_TMP = $(FW_TOP_DIR)/tmp
FW_DIR = $(FW_TMP)/$(FW)/lib/firmware/xilinx/$(FW)
FW_BIF = $(FW_TMP)/$(FW).bif
FW_BIN = $(FW_TMP)/$(FW).bin
FW_XCLBIN = $(FW_TMP)/$(FW).xclbin
FW_DTSI = $(FW_DT_DIR)/dtsi/$(FW).dtsi
FW_DTBO = $(FW_DT_DIR)/dtsi/$(FW).dtbo
FW_DEPS = $(FW_DTBO) $(FW_BIN) $(JSON_TEMPLATE)

ifdef OVERLAY
FW_BIT = $(VITIS_OVERLAY_BIT)
FW_DT_DIR = $(VITIS_OVERLAY_DIR)
FW_DEPS += $(FW_XCLBIN)
else
FW_BIT = $(VIV_BIT)
FW_DT_DIR = $(VIV_DIR)
endif

.PHONY: help
help:
	@echo 'Usage:'
	@echo ''
	@echo '  make firmware FW=<val>'
	@echo '    Build the firmware artifacts for a given platform or overlay.'
	@echo ''
	@echo '    FW: valid options'
	@echo -n '        '
	@echo '${FW_LIST}' | sed -r 's/ /\n        /g'
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

.PHONY: check-firmware
check-firmware:
	@valid=0; \
	for f in $(FW_LIST); do \
	  if [ "$$f" = "$(FW)" ]; then \
	    valid=1; \
	    break; \
	  fi \
	done; \
	if [ "$$valid" -ne 1 ]; then \
	  echo 'Invalid parameter FW=$(FW). Choose one of:'; \
	  echo -n '        '; \
	  echo '${FW_LIST}' | sed -r 's/ /\n        /g'; \
	  echo ''; \
	  exit 1; \
	fi

.PHONY: firmware
firmware: check-firmware $(FW_DEPS)
	@echo 'Generate $(FW) firmware artifacts and copy to $(FW_DIR)'
	mkdir -p $(FW_DIR)
	cp -f $(filter-out $<,$^) $(FW_DIR)
	cd $(FW_TMP)/$(FW) && md5sum ./lib/firmware/xilinx/$(FW)/* > checksum.md5
	tar -czf $(FW_TOP_DIR)/$(FW).tar.gz -C $(FW_TMP) $(FW)
	rm -rf $(FW_TMP)

$(FW_DTBO): $(FW_DTSI)
	dtc -I dts -O dtb -o $@ $<

$(FW_BIN): $(FW_BIF)
	mkdir -p $(FW_TMP)
	bootgen -image $< -arch zynqmp -o $@ -w

$(FW_BIF): $(FW_BIT) $(BIF_TEMPLATE)
	mkdir -p $(FW_TMP)
	sed 's#@BIT@#$<#' <$(BIF_TEMPLATE) >$@

$(FW_XCLBIN): $(VITIS_OVERLAY_XCLBIN)
	mkdir -p $(FW_TMP)
	cp -f $< $@

.PHONY: check-overlay
check-overlay:
	@valid=0; \
	for o in $(OVERLAY_LIST); do \
	  if [ "$$o" = "$(OVERLAY)" ]; then \
	    valid=1; \
	    break; \
	  fi \
	done; \
	if [ "$$valid" -ne 1 ]; then \
	  echo 'Invalid parameter OVERLAY=$(OVERLAY). Choose one of'; \
	  echo -n '        '; \
	  echo '${OVERLAY_LIST}' | sed -r 's/ /\n        /g'; \
	  echo ''; \
	  exit 1; \
	fi

.PHONY: overlay
overlay: check-overlay $(VITIS_OVERLAY_XCLBIN)
$(VITIS_OVERLAY_XCLBIN): $(VITIS_OVERLAY_BIT)
$(VITIS_OVERLAY_BIT): $(PFM_XPFM)
	@echo 'Build $(OVERLAY) Vitis overlay using platform $(PFM)'
	$(MAKE) -C $(VITIS_OVERLAY_DIR) all PLATFORM=$(PFM_XPFM)

.PHONY: check-platform
check-platform:
	@valid=0; \
	for p in $(PFM_LIST); do \
	  if [ "$$p" = "$(PFM)" ]; then \
	    valid=1; \
	    break; \
	  fi \
	done; \
	if [ "$$valid" -ne 1 ]; then \
	  echo 'Invalid parameter PFM=$(PFM). Choose one of:'; \
	  echo -n '        '; \
	  echo '${PFM_LIST}' | sed -r 's/ /\n        /g'; \
	  echo ''; \
	  exit 1; \
	fi

.PHONY: platform
platform: check-platform $(PFM_XPFM)
$(PFM_XPFM): $(VIV_XSA)
	@echo 'Create Vitis platform $(PFM)'
	cd $(PFM_DIR) && $(XSCT) $(PFM_TCL) -xsa $(VIV_XSA)
	$(CP) $(PFM_PRJ_DIR) $(PFM_DIR)/$(PFM_NAME)

$(VIV_BIT): $(VIV_XSA)
$(VIV_XSA):
	make -C $(VIV_DIR) xsa JOBS=$(JOBS)

.PHONY: clean
clean:
	$(foreach o, $(OVERLAY_LIST), $(MAKE) -C $(VITIS_DIR)/$(o) clean;)
	$(foreach p, $(PFM_LIST), $(MAKE) -C $(VIV_DIR) clean;)
