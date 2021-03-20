CP = cp -f

PWD = $(shell readlink -f .)

# the platform directory has to be an absolute path when passed to v++
PFM_DIR = $(PWD)/platforms
PFM_VER = 202022_1

# valid platforms / accelerators
PFM_LIST = kv260_smartcamera kv260_aibox kv260_defectdetect
AA_LIST = aa1 aa2 aa4

# override platform name based on accelerator
ifeq ($(AA),aa1)
  override PFM = kv260_smartcamera
endif
ifeq ($(AA),aa2)
  override PFM = kv260_aibox
endif
ifeq ($(AA),aa4)
  override PFM = kv260_defectdetect
endif

PFM_XPFM = $(PFM_DIR)/xilinx_$(PFM)_$(PFM_VER)/$(PFM).xpfm

VITIS_DIR = accelerators/examples
VITIS_AA_DIR = $(VITIS_DIR)/som_$(AA)
VITIS_AA_BIT = $(VITIS_AA_DIR)/binary_container_1/link/int/system.bit

.PHONY: help
help:
	@echo 'Usage:'
	@echo ''
	@echo '  make accelerator AA=<val>'
	@echo '    Build the Vitis accelerated application (AA) overlay.'
	@echo ''
	@echo '    Valid options for AA: ${AA_LIST}'
	@echo ''
	@echo '  make platform PFM=<val>'
	@echo '    Build the Vitis platform.'
	@echo ''
	@echo '    Valid options for PFM: ${PFM_LIST}'
	@echo ''
	@echo '  make clean'
	@echo '    Clean runs'
	@echo ''

.PHONY: accelerator
accelerator: $(VITIS_AA_BIT)
$(VITIS_AA_BIT): $(PFM_XPFM)
	@val=0; \
	for a in $(AA_LIST); do \
	  if [ "$$a" == "$(AA)" ]; then \
	    echo 'Build $(AA) Vitis overlay using platform $(PFM)'; \
	    $(MAKE) -C $(VITIS_AA_DIR) all PLATFORM=$(PFM_XPFM); \
	    val=1; \
	  fi \
	done; \
	if [ "$$val" -ne 1 ]; then \
	  echo 'Invalid parameter AA=$(AA). Choose one of: $(AA_LIST)'; \
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
	$(foreach a, $(AA_LIST), $(MAKE) -C $(VITIS_DIR)/som_$(a) clean;)
	$(foreach p, $(PFM_LIST), $(MAKE) -C $(PFM_DIR) clean PLATFORM=$(p) VERSION=$(PFM_VER);)
