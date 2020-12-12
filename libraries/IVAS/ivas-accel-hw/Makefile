PLATFORM_FILE := /proj/ips-ssw-git/saurabhs/repositories/ivas_8ch_crop_reid/platform/zcu104_vcu/platform_repo/xilinx_zcu104_vcu_202010_1/export/xilinx_zcu104_vcu_202010_1/xilinx_zcu104_vcu_202010_1.xpfm

TARGET := hw

XOCCFLAGS := --platform $(PLATFORM_FILE) -t $(TARGET) -s -g 

MULTISCALER_FLAGS := --kernel v_multi_scaler -I. -I./multiscaler/hls

.PHONY: clean

all: v_multi_scaler.xo

v_multi_scaler.xo: multiscaler/v_multi_scaler.cpp multiscaler/v_hresampler.cpp multiscaler/v_hscaler.cpp multiscaler/v_dma.cpp multiscaler/v_csc.cpp multiscaler/v_vresampler.cpp multiscaler/v_vscaler.cpp
	v++ $(XOCCFLAGS) $(MULTISCALER_FLAGS) -c -o xo/$@ $^

clean:
	$(RM) -r xo/* *_x .Xil sd_card* *.xclbin *.ltx *.log *.info packaged_kernel* tmp_kernel* vivado* pfm_sw dpu_conf.vh
	$(RM) *summary* *.str *.hwh
