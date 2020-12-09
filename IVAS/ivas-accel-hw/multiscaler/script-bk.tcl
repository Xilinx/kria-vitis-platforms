#
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
#

# Define variables for your HLS kernel:
set projName prj
set krnlName v_multi_scaler
set krnlFile v_multi_scaler.cpp


open_project $projName
set_top v_multi_scaler

add_files ./v_multi_scaler_config.h
add_files ./v_multi_scaler.h
add_files ./hls_opencv.h
add_files ./hls_video.h
add_files ./v_vscaler.cpp
add_files ./v_vresampler.cpp
add_files ./v_hscaler.cpp
add_files ./v_hresampler.cpp
add_files ./v_dma.cpp
add_files ./v_csc.cpp
add_files ./v_multi_scaler.cpp

open_solution "solution1"
set_part {xczu7ev-ffvc1156-2-e}
create_clock -period 3.33 -name default
config_rtl -encoding onehot -reset control -reset_level high
#config_interface -default_slave_interface none
#config_vitis -optimization_level none -target vitis
config_sdx -optimization_level none -target xocc
config_flow -target vitis
config_export -vivado_optimization_level 0 -vivado_phys_opt none
config_compile -name_max_length 256 -pipeline_loops 64
config_schedule -enable_dsp_full_reg

#csim_design -ldflags {-L. -lxilvid_x64 -L/proj/xtools/dsv/projects/sdx_libs/cpp_libs/opencv/opencv3/lib64 -lopencv_core -lopencv_imgproc -lopencv_highgui} -argv {.} -O
#config_schedule -effort medium -verbose
#csim_design -ldflags {-L. -lxilvid_x64} -argv {.} -O -compiler gcc
csynth_design
#cosim_design -O -compiler gcc -ldflags {-L. -lxilvid_x64} -argv {.}
#export_design -rtl verilog
#export_design -rtl verilog -format ip_catalog -xo ./$krnlName.xo
export_design -rtl verilog -format xo -output ./$krnlName.xo
exit
