#gst-launch-1.0 fakesrc ! xlnxm2m xclbin-location=/mnt/dpu.xclbin kconfig=kernel.config ! fakesink



gst-launch-1.0 filesrc location=$1 blocksize=150528 num-buffers=1 !  \
videoparse width=224 height=224 framerate=30/1 format=16 ! \
ivas_xfilter name="kernel1" kernels-config="./json_files/kernel_resnet50.json" ! \
filesink location=./resnet_output_224_224.bgr

########################################################################
 # Copyright 2020 Xilinx, Inc.
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
##########################################################################
