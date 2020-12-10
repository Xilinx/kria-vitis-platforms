##
 # Copyright 2019 Xilinx Inc.
 #
 # Licensed under the Apache License, Version 2.0 (the "License");
 # you may not use this file except in compliance with the License.
 # You may obtain a copy of the License at
 #
 #     http://www.apache.org/licenses/LICENSE-2.0
 #
 # Unless required by applicable law or agreed to in writing, software
 # distributed under the License is distributed on an "AS IS" BASIS,
 # WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 # See the License for the specific language governing permissions and
 # limitations under the License.
##

#Usage:
# make -j 10 XBLASOPTS="-D ZMQ -D CBLAS"
# LIBRARY_PATH=/usr/lib/x86_64-linux-gnu make -j 10 CXX="/tools/batonroot/rodin/devkits/lnx64/gcc-4.8.0/bin/g++" XBLASOPTS="-O3"

###########
# Runtime
###########
XBLAS_SRC := \
	xblas.cpp \
	xdnn.cpp \
	xmlrt.cpp \
	xdnn_util.cpp \
	xdnn_ops.cpp \
	xdnn_reg_write_mgr.cpp \
	xdnn_fcweightsload_cpp_infer.cpp \
	cJSON.cpp \
	xdnn_runner.cpp \
  ../vitis/src/dpu/dpu_runner.cpp \
  ../vitis/src/dpu/tensor.cpp \
  ../vitis/src/dpu/tensor_buffer.cpp \

XFDNN_LIB_V3=libxfdnn.so

VERSION=version.h

XBLASOPTS :=-O3 -D MKL -D XBLAS -D ZMQ
#XBLASOPTS :=-O3 -D CBLAS -D ZMQ -D XBLAS
#XBLASOPTS :=-g -O0 -D MKL  -D ZMQ -D XBLAS -D DEBUG

###
# Is XRT present
###
ifeq (,/opt/xilinx/xrt/include/version.h)
$(error XILINX_XRT is not present on system)
endif

#auto detect version of XRT_BUILD (2.3.x)
#ERROR if not 2019.2+
INFO:= $(shell cat /opt/xilinx/xrt/include/version.h | grep xrt_build_version\\[ | sed 's/[^0-9.]*//g')
MAJOR:= $(shell echo $(INFO) | cut -f1 -d ".")
MINOR:= $(shell echo $(INFO) | cut -f2 -d ".")
MAJOR_GT:= $(shell expr $(MAJOR) \> 2)
MAJOR_EQ:= $(shell expr $(MAJOR) = 2)
MINOR_20191:= $(shell expr $(MINOR) = 2)
MINOR_:= $(shell expr $(MINOR) \>= 3)

# Enable XRT 2019.1 flag
ENABLE_XRT_20191 := 0
# Validate 2019.1/2019.2+ XRT
ifeq (1,$(MAJOR_EQ))
	ifeq (1,$(MINOR_20191))
		ENABLE_XRT_20191 := 1
	else
		ifeq (0,$(MINOR_))
			$(error Invalid XRT Version)
		endif
	endif
else
	ifeq (0,$(MAJOR_GT))
		$(error Invalid XRT Version)
	endif
endif

XILINX_XRT := /opt/xilinx/xrt/

ifeq (1,$(ENABLE_XRT_20191))
XBLASOPTS += -D XRT_20191
endif
###
# Add XRT include
###
INCLUDE += -I$(XILINX_XRT)/include

###
# Add common include
###
INCLUDE += -I$(CONDA_PREFIX)/include -I$(CONDA_PREFIX)/include/xip/butler  -I ../vitis/include

CXX := g++ #$(GXX)

## Runtime
CCOPTS := -D __USE_XOPEN2K8 $(XBLASOPTS) -std=c++11 -fPIC -static-libgcc -static-libstdc++

#####################
#        Linker
#####################
# common
LIB_PATHS := -L $(CONDA_PREFIX)/lib -L $(XILINX_XRT)/lib


# Runtime
LD_OPT := -shared -pthread -fPIC

ifneq (,$(findstring SANITIZE,$(XBLASOPTS)))
#CCOPTS += -fno-omit-frame-pointer -lubsan -lasan -fuse-ld=gold -fsanitize=address,undefined,bounds-strict  -fsanitize-recover=address
#CCOPTS += -fno-omit-frame-pointer -lubsan -lasan -fuse-ld=gold -fsanitize=address,undefined,bounds-strict -fsanitize-recover=address
#CXX := g++-6
CCOPTS += -fsanitize=address -fno-omit-frame-pointer
LD_OPT += -fsanitize=address
endif

XFDNN_OBJS_V3 := $(addprefix objs/v3/,$(addsuffix .o,$(basename $(XBLAS_SRC))))

LIBS := -lhdf5_cpp -lxilinxopencl -lxrt_core -lbutler -lglog -ljson-c

###
# Enable MKL
###
ifneq (,$(findstring MKL,$(XBLASOPTS)))
LIBS += -lmkl_rt
endif

###
# Enable CBLAS
###
ifneq (,$(findstring CBLAS,$(XBLASOPTS)))
LIBS += -lopenblas
endif

###
# Enable ZMQ
###
ifneq (,$(findstring ZMQ,$(XBLASOPTS)))
LIBS += -lzmq
endif

#####################
#      Rules/Targets
#####################

.DEFAULT_GOAL:= all

.PHONY: all

all: $(VERSION) $(XFDNN_LIB_V3)
	@echo "Good job! I know you don't hear it enough!"

$(VERSION):
	cp /opt/xilinx/xrt/include/version.h $@
	echo "const char *git_commit = \"$(shell git rev-parse HEAD)\";" >> $@

$(XFDNN_LIB_V3): $(XFDNN_OBJS_V3)
	mkdir -p lib
	$(CXX) $(LD_OPT) -o lib/$@ $(XFDNN_OBJS_V3) $(LIB_PATHS) $(LIBS)
	chmod a+rx lib
	chmod a+r lib/*

objs/v3/%.o: %.cpp
	@mkdir -p objs/v3/$(dir $<)
	$(CXX) $(CCOPTS) -c -o $@ $(INCLUDE) $(LIB_PATHS) $(LIBS) $<

clean:
	rm -rf objs lib bin version.h
