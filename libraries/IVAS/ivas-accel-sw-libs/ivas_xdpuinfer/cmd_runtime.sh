#gst-launch-1.0 fakesrc ! xlnxm2m xclbin-location=/mnt/dpu.xclbin kconfig=kernel.config ! fakesink


#gst-launch-1.0 videotestsrc !  \
#"video/x-raw, width=300, height=300, format=BGR, framerate=30/1" ! \
#ivas_xfilter name="kernel1" kernels-config="./json_files/kernel_testresnet50.json" ! \
#filesink location=./resnet_output_224_224.bgr

#gst-launch-1.0 videotestsrc num-buffers=2 !  \
gst-launch-1.0 filesrc location="/home/siva/Videos/test_model_224_224.bgr" blocksize=150528 num-buffers=4 !  \
"video/x-raw, width=224, height=224, format=BGR, framerate=30/1" ! \
ivas_xfilter name="kernel1" kernels-config="./json_files/kernel_addruntimemeta.json" ! \
ivas_xfilter name="kernel2" kernels-config="./json_files/kernel_runtime.json" ! \
fakesink

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
