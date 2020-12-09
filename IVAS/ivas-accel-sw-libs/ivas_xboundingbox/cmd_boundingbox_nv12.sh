gst-launch-1.0 filesrc location=$1 blocksize=259584  num-buffers=1 \
! videoparse width=416 height=416 framerate=30/1 format=nv12 \
! tee name=t \
t.src_0 \
	! queue \
	!  videoconvert ! "video/x-raw, format=BGR" \
	! ivas_xfilter name="kernel1" kernels-config="../ivas_xdpuinfer/json_files/kernel_yolov3_voc.json" \
	! scalem.sink_master ivas_xmetaaffixer name=scalem scalem.src_master  ! fakesink \
t.src_1 \
	! scalem.sink_slave_0 scalem.src_slave_0 \
	! queue \
	! ivas_xfilter name="kernel2" kernels-config="./kernel_boundingbox.json" \
	! filesink location=output_416_416.nv12

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
