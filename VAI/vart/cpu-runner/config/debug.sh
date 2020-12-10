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
  DST_FOLD=/group/dphi_arch/yushun/debug/
elif [ $# -eq 2 ];then
  SRC_XMODEL=$1
  DST_FOLD=$2
else
  echo "param error! usage:"
  echo "./debug.sh {src_model} {dst_fold}"
  exit 1
fi

set -x

DUMP_IMG=/home/wyushun/build/build.Ubuntu.18.04.x86_64.Debug/xir/test/dump_img.bin
XIR_CAT=/home/wyushun/build/build.Ubuntu.18.04.x86_64.Debug/xir/test/xir_cat.bin

# copy origin xmodel
rm -rf ${DST_FOLD}
mkdir ${DST_FOLD}
cp ${SRC_XMODEL} ${DST_FOLD}

# dump source xmodel image
${DUMP_IMG} -i ${SRC_XMODEL} -o ${DST_FOLD}/src_xmodel.svg

# dump xmodel's contents into a file
${XIR_CAT} ${SRC_XMODEL} -t ${DST_FOLD}/src_xmodel.txt -p ${DST_FOLD}/src_xmodel.png


echo "successfully!"

