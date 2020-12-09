#gst-launch-1.0 fakesrc ! xlnxm2m xclbin-location=/mnt/dpu.xclbin kconfig=kernel.config ! fakesink



gst-launch-1.0 filesrc location=$1 blocksize=519168 num-buffers=1 !  \
videoparse width=416 height=416 framerate=30/1 format=16 ! \
ivas_xfilter kernels-config="./json_files/kernel_yolov3_voc_tf.json" ! \
ivas_xfilter kernels-config="../ivas_xboundingbox/kernel_boundingbox.json" ! \
filesink location=./yolov3_voc_tf_output_416_416.bgr

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
