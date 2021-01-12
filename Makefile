CP = cp -f

PWD = $(shell readlink -f .)

# the platform directory has to be an absolute path when passed to v++
PFM_DIR = $(PWD)/platforms
PFM_VER = 202020_1
# platform
PFM_NAME ?= kv260_smartcamera
PFM_XPFM = $(PFM_DIR)/xilinx_$(PFM_NAME)_$(PFM_VER)/$(PFM_NAME).xpfm

ifeq ($(PFM_NAME),kv260_aibox)
  AA_NAME= kv260-aa2
else
  AA_NAME= kv260-aa1
endif

ifeq ($(PFM_NAME),kv260_aibox)
  SOM_NAME= som_aa2
else
  SOM_NAME= som_aa1
endif

PLNX_DIR = petalinux/xilinx-kv260-smartcamera-2020.2-final
PLNX_WIC = $(PLNX_DIR)/images/linux/petalinux-sdimage.wic

PLNX_AA_DIR = $(PLNX_DIR)/project-spec/meta-user/recipes-fw/$(AA_NAME)/files
PLNX_AA_BIT = $(PLNX_AA_DIR)/$(AA_NAME).bit
PLNX_AA_XCLBIN = $(PLNX_AA_DIR)/$(AA_NAME).xclbin
PLNX_AA_OBJS += $(PLNX_AA_BIT)
PLNX_AA_OBJS += $(PLNX_AA_XCLBIN)

VITIS_DIR = accelerators/examples
VITIS_AA_DIR = $(VITIS_DIR)/$(SOM_NAME)
VITIS_AA_BIT = $(VITIS_AA_DIR)/binary_container_1/link/int/system.bit
VITIS_AA_XCLBIN = $(VITIS_AA_DIR)/binary_container_1/dpu.xclbin
VITIS_AA_OBJS += $(VITIS_AA_BIT)
VITIS_AA_OBJS += $(VITIS_AA_XCLBIN)


.PHONY: help
help:
	@echo 'Usage:'
	@echo ''
	@echo '  make PFM_NAME=kv260_smartcamera sdcard'
	@echo '    Generate an SD card wic image using PetaLinux.'
	@echo ''
	@echo '    Note: This rule does *not* trigger any of the aa*-import rules. The user'
	@echo '          has to manually run those rules (if desired) to import previously'
	@echo '          generated AA firmware artifacts.'
	@echo ''
	@echo '  make PFM_NAME=kv260_smartcamera aa-import'
	@echo '    Import the AA bitstream and xclbin from the Vitis project into PetaLinux.'
	@echo '    This triggers the aa rule.'
	@echo ''
	@echo '  make PFM_NAME=kv260_smartcamera aa'
	@echo '    Build the AA Vitis overlay against the KV260 smartcamera Vitis platform.'
	@echo ''
	@echo '  make PFM_NAME=kv260_smartcamera platform'
	@echo '    Generate the KV260 smartcamera Vitis platform.'
	@echo ''
	@echo '  make PFM_NAME=kv260_smartcamera clean'
	@echo '    Clean runs'
	@echo ''

.PHONY: all
all: sdcard

.PHONY: sdcard
sdcard: $(PLNX_WIC)
$(PLNX_WIC):
	$(MAKE) -C $(PLNX_DIR) wic
	@echo 'The PetaLinux wic image is available at $(PLNX_WIC)'

.PHONY: aa-import
aa-import: $(PLNX_AA_OBJS)
$(PLNX_AA_OBJS): $(VITIS_AA_OBJS)
	@echo 'Copy AA xclbin and bitstream into PetaLinux project'
	@$(CP) $(VITIS_AA_BIT) $(PLNX_AA_BIT)
	@$(CP) $(VITIS_AA_XCLBIN) $(PLNX_AA_XCLBIN)

.PHONY: aa
aa: $(VITIS_AA_OBJS)
$(VITIS_AA_OBJS): $(PFM_XPFM)
	@echo 'Build AA Vitis overlay using platform $(PFM_NAME)'
	$(MAKE) -C $(VITIS_AA_DIR) all PLATFORM=$(PFM_XPFM)


.PHONY: platform
platform: $(PFM_XPFM)
$(PFM_XPFM):
	$(MAKE) -C $(PFM_DIR) platform PLATFORM=$(PFM_NAME) VERSION=$(PFM_VER)


.PHONY: clean
clean:
	$(MAKE) -C $(VITIS_AA_DIR) clean
	$(MAKE) -C $(PLNX_DIR) clean
	$(MAKE) -C $(PFM_DIR) clean PLATFORM=$(PFM_NAME) VERSION=$(PFM_VER)
