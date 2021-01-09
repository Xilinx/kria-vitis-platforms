CP = cp -f

PWD = $(shell readlink -f .)

# the platform directory has to be an absolute path when passed to v++
PFM_DIR = $(PWD)/platforms
PFM_VER = 202020_1
# smartcamera platform
PFM_SC_NAME = kv260_smartcamera
PFM_SC_XPFM = $(PFM_DIR)/xilinx_$(PFM_SC_NAME)_$(PFM_VER)/$(PFM_SC_NAME).xpfm
# aibox platform
PFM_AB_NAME = kv260_aibox
PFM_AB_XPFM = $(PFM_DIR)/xilinx_$(PFM_AB_NAME)_$(PFM_VER)/$(PFM_AB_NAME).xpfm

PLNX_DIR = petalinux/xilinx-kv260-smartcamera-2020.2-final
PLNX_WIC = $(PLNX_DIR)/images/linux/petalinux-sdimage.wic
# aa1
PLNX_AA1_DIR = $(PLNX_DIR)/project-spec/meta-user/recipes-fw/kv260-aa1/files
PLNX_AA1_BIT = $(PLNX_AA1_DIR)/kv260-aa1.bit
PLNX_AA1_XCLBIN = $(PLNX_AA1_DIR)/kv260-aa1.xclbin
PLNX_AA1_OBJS += $(PLNX_AA1_BIT)
PLNX_AA1_OBJS += $(PLNX_AA1_XCLBIN)
# aa2
PLNX_AA2_DIR = $(PLNX_DIR)/project-spec/meta-user/recipes-fw/kv260-aa2/files
PLNX_AA2_BIT = $(PLNX_AA2_DIR)/kv260-aa2.bit
PLNX_AA2_XCLBIN = $(PLNX_AA2_DIR)/kv260-aa2.xclbin
PLNX_AA2_OBJS += $(PLNX_AA2_BIT)
PLNX_AA2_OBJS += $(PLNX_AA2_XCLBIN)

VITIS_DIR = accelerators/examples
# aa1
VITIS_AA1_DIR = $(VITIS_DIR)/som_aa1
VITIS_AA1_BIT = $(VITIS_AA1_DIR)/binary_container_1/link/int/system.bit
VITIS_AA1_XCLBIN = $(VITIS_AA1_DIR)/binary_container_1/dpu.xclbin
VITIS_AA1_OBJS += $(VITIS_AA1_BIT)
VITIS_AA1_OBJS += $(VITIS_AA1_XCLBIN)
# aa2
VITIS_AA2_DIR = $(VITIS_DIR)/som_aa2
VITIS_AA2_BIT = $(VITIS_AA2_DIR)/binary_container_1/link/int/system.bit
VITIS_AA2_XCLBIN = $(VITIS_AA2_DIR)/binary_container_1/dpu.xclbin
VITIS_AA2_OBJS += $(VITIS_AA2_BIT)
VITIS_AA2_OBJS += $(VITIS_AA2_XCLBIN)

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
	@echo '  make aa2-import'
	@echo '    Import the AA2 bitstream and xclbin from the Vitis project into PetaLinux.'
	@echo '    This triggers the aa2 rule.'
	@echo ''
	@echo '  make aa1'
	@echo '    Build the AA1 Vitis overlay against the KV260 smartcamera Vitis platform.'
	@echo ''
	@echo '  make aa2'
	@echo '    Build the AA2 Vitis overlay against the KV260 aibox Vitis platform.'
	@echo ''
	@echo '  make smartcamera'
	@echo '    Generate the KV260 smartcamera Vitis platform'
	@echo ''
	@echo '  make aibox'
	@echo '    Generate the KV260 aibox Vitis platform'
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

.PHONY: aa2-import
aa2-import: $(PLNX_AA2_OBJS)
$(PLNX_AA2_OBJS): $(VITIS_AA2_OBJS)
	@echo 'Copy AA2 xclbin and bitstream into PetaLinux project'
	@$(CP) $(VITIS_AA2_BIT) $(PLNX_AA2_BIT)
	@$(CP) $(VITIS_AA2_XCLBIN) $(PLNX_AA2_XCLBIN)
	$(MAKE) -C $(PLNX_DIR) image

.PHONY: aa1
aa1: $(VITIS_AA1_OBJS)
$(VITIS_AA1_OBJS): $(PFM_SC_XPFM)
	@echo 'Build AA1 Vitis overlay using platform $(PFM_SC_NAME)'
	$(MAKE) -C $(VITIS_AA1_DIR) all PLATFORM=$(PFM_SC_XPFM)

.PHONY: aa2
aa2: $(VITIS_AA2_OBJS)
$(VITIS_AA2_OBJS): $(PFM_AB_XPFM)
	@echo 'Build AA2 Vitis overlay using platform $(PFM_AB_NAME)'
	$(MAKE) -C $(VITIS_AA2_DIR) all PLATFORM=$(PFM_AB_XPFM)

.PHONY: smartcamera
smartcamera: $(PFM_SC_XPFM)
$(PFM_SC_XPFM):
	$(MAKE) -C $(PFM_DIR) platform PLATFORM=$(PFM_SC_NAME) VERSION=$(PFM_VER)

.PHONY: aibox
aibox: $(PFM_AB_XPFM)
$(PFM_AB_XPFM):
	$(MAKE) -C $(PFM_DIR) platform PLATFORM=$(PFM_AB_NAME) VERSION=$(PFM_VER)

.PHONY: clean
clean:
	$(MAKE) -C $(VITIS_AA1_DIR) clean
	$(MAKE) -C $(VITIS_AA2_DIR) clean
	$(MAKE) -C $(PLNX_DIR) clean
	$(MAKE) -C $(PFM_DIR) clean PLATFORM=$(PFM_SC_NAME) VERSION=$(PFM_VER)
	$(MAKE) -C $(PFM_DIR) clean PLATFORM=$(PFM_AB_NAME) VERSION=$(PFM_VER)

