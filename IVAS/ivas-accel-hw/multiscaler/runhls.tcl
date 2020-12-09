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
open_project ex_synth_v_multi_scaler_0_0
set_top v_multi_scaler
add_files ./v_multi_scaler_config.h
add_files ./v_multi_scaler.h
add_files ./v_multi_scaler.cpp
add_files ./v_dma.cpp
add_files ./v_vresampler.cpp
add_files ./v_hresampler.cpp
add_files ./v_vscaler.cpp
add_files ./v_hscaler.cpp
add_files ./v_csc.cpp
add_files ./hls_video.h
add_files ./hls_opencv.h
add_files ./hls/hls_axi_io.h
add_files ./hls/hls_video_arithm.h
add_files ./hls/hls_video_core.h
add_files ./hls/hls_video_fast.h
add_files ./hls/hls_video_haar.h
add_files ./hls/hls_video_harris.h
add_files ./hls/hls_video_histogram.h
add_files ./hls/hls_video_hough.h
add_files ./hls/hls_video_imgbase.h
add_files ./hls/hls_video_imgproc.h
add_files ./hls/hls_video_io.h
add_files ./hls/hls_video_mem.h
add_files ./hls/hls_video_stereobm.h
add_files ./hls/hls_video_types.h
add_files ./hls/hls_video_undistort.h

open_solution "prj"
set_part {xczu9eg-ffvb1156-2-e}
create_clock -period 4 -name ap_clk

                 

config_rtl -encoding onehot -reset control -reset_level high
config_schedule -effort medium -verbose



config_rtl -prefix ex_synth_v_multi_scaler_0_0_
csynth_design
export_design -format ip_catalog -vendor xilinx.com -library ip -version 2.0
exit
