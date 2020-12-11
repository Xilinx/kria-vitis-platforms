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


if [ $# -ne 3 ];then
  echo "param error! usage:"
  echo "./cmp.sh {model_path} {model_name} {input_name}"
  exit 1
fi

#set -x

ROOT_PATH=$1
MODEL_NAME=$2
INPUT_NAME=$3

# run fixed model using cpu mode
echo "Running fixed model using cpu mode..."
./test_vaie --model_path=${ROOT_PATH} --model_name=${MODEL_NAME} --input_name=${INPUT_NAME} --run_mode=cpu
rm -rf fix_log/
mv log/ fix_log/

# run fixed model using sim mode
echo "Running fixed model using sim mode..."
./test_vaie --model_path=${ROOT_PATH} --model_name=${MODEL_NAME} --input_name=${INPUT_NAME} --run_mode=sim
rm -rf sim_log/
mv log/ sim_log/

#compare fix_log and sim_log 
subg_num=$(ls -l fix_log | grep "^d" | wc -l)
for((i=0;i<${subg_num};i++));do
  FIX_SUBG_OUTPUT=$(ls fix_log/subg_${i}_o_*.bin)
  SIM_SUBG_OUTPUT=$(ls sim_log/subg_${i}_o_*.bin)

  # check file existed!
  if [ ! -e ${FIX_SUBG_OUTPUT} ];then
    echo "Failed: ${FIX_SUBG_OUTPUT} not found!"
    exit 1
  fi
  if [ ! -e ${SIM_SUBG_OUTPUT} ];then
    echo "Failed: ${SIM_SUBG_OUTPUT} not found!"
    exit 1
  fi

  # check file equal
  cmp -s ${FIX_SUBG_OUTPUT} ${SIM_SUBG_OUTPUT}
  if [ $? -ne 0 ];then
    echo "Checking subg${i} failed:"
    echo "  ${FIX_SUBG_OUTPUT}"
    echo "  ${SIM_SUBG_OUTPUT}"
    exit 1
  fi
  echo "Checking subg${i} successfully!"

done

echo "Whole graph successfully!"
