CP = cp -f

PWD = $(shell readlink -f .)

# the platform directory has to be an absolute path when passed to v++
PFM_DIR = $(PWD)/platforms
PFM_SMARTCAMERA = $(PFM_DIR)/xilinx_kv260_smartcamera_202022_1/kv260_smartcamera.xpfm

PLNX_DIR = petalinux/xilinx-kv260-smartcamera-2020.2-final
PLNX_WIC = $(PLNX_DIR)/images/linux/petalinux-sdimage.wic
PLNX_AA1_DIR = $(PLNX_DIR)/project-spec/meta-user/recipes-fw/kv260-aa1/files
PLNX_AA1_BIT = $(PLNX_AA1_DIR)/kv260-aa1.bit
PLNX_AA1_XCLBIN = $(PLNX_AA1_DIR)/kv260-aa1.xclbin
PLNX_AA1_OBJS += $(PLNX_AA1_BIT)
PLNX_AA1_OBJS += $(PLNX_AA1_XCLBIN)

VITIS_DIR = accelerators/examples
VITIS_AA1_DIR = $(VITIS_DIR)/som_aa1
VITIS_AA1_BIT = $(VITIS_AA1_DIR)/binary_container_1/link/int/system.bit
VITIS_AA1_XCLBIN = $(VITIS_AA1_DIR)/binary_container_1/dpu.xclbin
VITIS_AA1_OBJS += $(VITIS_AA1_BIT)
VITIS_AA1_OBJS += $(VITIS_AA1_XCLBIN)

.PHONY: help
help:
	@echo 'Usage:'
	@echo ''
	@echo '  make sdcard'
	@echo '    Generate an SD card wic image for the AA1 smartcamera application using PetaLinux.'
	@echo '    This triggers the aa1-import rule.'
	@echo ''
	@echo '  make aa1-import'
	@echo '    Import the AA1 bitstream and xclbin from the Vitis project into PetaLinux.'
	@echo '    This triggers the aa1 rule.'
	@echo ''
	@echo '  make aa1'
	@echo '    Build the AA1 Vitis overlay against the KV260 smartcamera Vitis platform.'
	@echo ''

.PHONY: all
all: sdcard

.PHONY: sdcard
sdcard: $(PLNX_WIC)
$(PLNX_WIC): $(PLNX_AA1_OBJS)
	$(MAKE) -C $(PLNX_DIR) all
	@echo 'The PetaLinux wic image is available at $(PLNX_WIC)'

.PHONY: aa1-import
aa1-import: $(PLNX_AA1_OBJS)
$(PLNX_AA1_OBJS): $(VITIS_AA1_OBJS)
	@echo 'Copy AA1 xclbin and bitstream into PetaLinux project'
	@$(CP) $(VITIS_AA1_BIT) $(PLNX_AA1_BIT)
	@$(CP) $(VITIS_AA1_XCLBIN) $(PLNX_AA1_XCLBIN)
	$(MAKE) -C $(PLNX_DIR) image

.PHONY: aa1
aa1: $(VITIS_AA1_OBJS)
$(VITIS_AA1_OBJS):
	@echo 'Build AA1 Vitis overlay using platform $(PFM_SMARTCAMERA)'
	$(MAKE) -C $(VITIS_AA1_DIR) all PLATFORM=$(PFM_SMARTCAMERA)

.PHONY: clean
clean:
	$(MAKE) -C $(VITIS_AA1_DIR) clean
	$(MAKE) -C $(PLNX_DIR) clean

