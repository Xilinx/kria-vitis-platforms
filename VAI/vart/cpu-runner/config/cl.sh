#!/bin/bash

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


if [ $# -eq 1 ];then
  SRC_XMODEL=$1
  DST_XMODEL=/group/dphi_arch/yushun/default.xmodel
  CL_MODE=float
elif [ $# -eq 2 ];then
  SRC_XMODEL=$1
  DST_XMODEL=/group/dphi_arch/yushun/default.xmodel
  CL_MODE=$2
elif [ $# -eq 3 ];then
  SRC_XMODEL=$1
  DST_XMODEL=$2
  CL_MODE=$3
else
  echo "param error! usage:"
  echo "./cl.sh {src_model} {dst_model} {compile_mode:float or fix}"
  exit 1
fi

set -x

XCOM=../../../xcompiler/src/xcompiler

if [ ${CL_MODE} = "float" ];then
  time ${XCOM} -m cpu_only -i ${SRC_XMODEL} -o ${DST_XMODEL}
elif [ ${CL_MODE} = "fix" ];then
  time ${XCOM} -m dpu -a DPUCZDI4G-4096 -i ${SRC_XMODEL} -o ${DST_XMODEL}
else
  echo "not support"
fi

echo "Whole graph successfully!"
