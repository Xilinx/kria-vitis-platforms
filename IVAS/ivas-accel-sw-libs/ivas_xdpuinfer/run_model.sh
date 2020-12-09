#!/bin/bash
echo model     : $0
echo inputfile : $1
echo width     : $2
echo height    : $3
echo num-buff  : $4



if [ "$#" -ne 5 ]; then
  echo "Illegal number of parameters"
  echo "Usage : run_model.sh <modelname> <inputfile> <width> <height> <num-buffers>"
  echo "Ex : ./run_model.sh resnet50 ~/Videos/test_model_224_224.bgr 224 224 1000"
  echo "Ex : ./run_model.sh resnet18 ~/Videos/test_model_224_224.bgr 224 224 1000"
  echo "Ex : ./run_model.sh densebox_320_320 ~/Videos/densebox_320_320.bgr 320 320 1000"
  echo "Ex : ./run_model.sh yolov3_voc_tf ~/Videos/yolov3_416_416.bgr 416 416 1000"
  echo "Ex : ./run_model.sh yolov3_voc ~/Videos/yolov3_416_416.bgr 416 416 1000"
  echo "Ex : ./run_model.sh ssd_adas_pruned_0_95 ~/Videos/traffic4_480_360.bgr 480 360 1000"
  echo "Ex : ./run_model.sh refinedet_pruned_0_96 ~/Videos/refinedet_480_360.bgr 480 360 1"
  echo "Ex : ./run_model.sh yolov3_adas_pruned_0_9 ~/Videos/yolov3_adas_pruned_0_9_512_256.bgr 512 256 1000"
  echo "Ex : ./run_model.sh resnet_v1_50_tf ~/Videos/test_model_224_224.bgr 224 224 1000"
  echo "Ex : ./run_model.sh ssd_mobilenet_v2 ~/Videos/traffic4_480_360.bgr 480 360 1000"
  echo "Ex : ./run_model.sh ssd_mobilenet_v2_coco_tf ~/Videos/traffic4_300_300.bgr 300 300 200"
  echo "Ex : ./run_model.sh yolov2_voc ~/Videos/yolov2_448_448.bgr 448 448 100"
  echo "Ex : ./run_model.sh yolov2_voc_pruned_0_77 ~/Videos/yolov2_448_448.bgr 448 448 100"
  exit -1;
fi

buffer_size=$(($3*$4*3))
echo $buffer_size

pipe="filesrc location=$2 blocksize=$buffer_size num-buffers=$5 ! videoparse width=$3 height=$4 framerate=30/1 format=16 ! ivas_xfilter name="kernel1" kernels-config="./json_files/kernel_$1.json" ! fakesink"
pipe="gst-launch-1.0 "$pipe
echo $pipe
$pipe

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
