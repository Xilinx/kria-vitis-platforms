CP = cp -f

PWD = $(shell readlink -f .)

# the platform directory has to be an absolute path when passed to v++
PFM_DIR = $(PWD)/platforms
PFM_VER = 202022_1

# valid platforms / accelerators
PFM_LIST = kv260_smartcamera kv260_aibox
AA_LIST = aa1 aa2

# override platform name based on accelerator
ifeq ($(AA),aa1)
  override PFM = kv260_smartcamera
endif
ifeq ($(AA),aa2)
  override PFM = kv260_aibox
endif

PFM_XPFM = $(PFM_DIR)/xilinx_$(PFM)_$(PFM_VER)/$(PFM).xpfm

PLNX_DIR = petalinux/xilinx-kv260-smartcamera-2020.2-final
PLNX_WIC = $(PLNX_DIR)/images/linux/petalinux-sdimage.wic

PLNX_FW_NAME = kv260-$(AA)
PLNX_FW_DIR = $(PLNX_DIR)/project-spec/meta-user/recipes-fw/$(PLNX_FW_NAME)/files
PLNX_FW_BIT = $(PLNX_FW_DIR)/$(PLNX_FW_NAME).bit
PLNX_FW_XCLBIN = $(PLNX_FW_DIR)/$(PLNX_FW_NAME).xclbin
PLNX_FW_OBJS += $(PLNX_FW_BIT)
PLNX_FW_OBJS += $(PLNX_FW_XCLBIN)

VITIS_DIR = accelerators/examples
VITIS_AA_DIR = $(VITIS_DIR)/som_$(AA)
VITIS_AA_BIT = $(VITIS_AA_DIR)/binary_container_1/link/int/system.bit
VITIS_AA_XCLBIN = $(VITIS_AA_DIR)/binary_container_1/dpu.xclbin
VITIS_AA_OBJS += $(VITIS_AA_BIT)
VITIS_AA_OBJS += $(VITIS_AA_XCLBIN)

.PHONY: help
help:
	@echo 'Usage:'
	@echo ''
	@echo '  make sdcard'
	@echo '    Generate an SD card wic image using PetaLinux.'
	@echo ''
	@echo '    Note: The user has to manually run the fw-import rules (if desired) to'
	@echo '          import previously generated AA firmware artifacts.'
	@echo ''
	@echo '  make fw-import AA=<val>'
	@echo '    Import the AA bitstream and xclbin from the Vitis project into PetaLinux.'
	@echo ''
	@echo '    Valid options for AA: ${AA_LIST}'
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

.PHONY: all
all: sdcard

.PHONY: sdcard
sdcard: $(PLNX_WIC)
$(PLNX_WIC):
	$(MAKE) -C $(PLNX_DIR) wic
	@echo 'The PetaLinux wic image is available at $(PLNX_WIC)'

.PHONY: fw-import
fw-import: $(PLNX_FW_OBJS)
$(PLNX_FW_OBJS): $(VITIS_AA_OBJS)
	@echo 'Copy $(PLNX_FW_NAME) xclbin and bitstream into PetaLinux project'
	@$(CP) $(VITIS_AA_BIT) $(PLNX_FW_BIT)
	@$(CP) $(VITIS_AA_XCLBIN) $(PLNX_FW_XCLBIN)

.PHONY: accelerator
accelerator: $(VITIS_AA_OBJS)
$(VITIS_AA_OBJS): $(PFM_XPFM)
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
	$(MAKE) -C $(PLNX_DIR) clean
	$(foreach a, $(AA_LIST), $(MAKE) -C $(VITIS_DIR)/som_$(a) clean;)
	$(foreach p, $(PFM_LIST), $(MAKE) -C $(PFM_DIR) clean PLATFORM=$(p) VERSION=$(PFM_VER);)
