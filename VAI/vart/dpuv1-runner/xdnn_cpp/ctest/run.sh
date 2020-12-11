#!/usr/bin/env bash
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

. ../../../../overlaybins/setup.sh $1
export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:../lib

#export XBLAS_EMIT_PROFILING_INFO=1
export XDNN_GLOBAL_SCALE_A=10000
export XDNN_GLOBAL_SCALE_B=30

./test.exe ../../../../overlaybins/$1/overlay_0.xclbin googlenet.fpgaaddr.64.txt googlenet.8b.json data synset_words.txt
