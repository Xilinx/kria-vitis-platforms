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

BOOST_SRC=../../../../ext/boost
BOOST_LIB=../../../../ext/boost/lib
export XILINX_SDACCEL=../../../../ext/sdx_build
INCLUDE="-I ${BOOST_SRC} -I ${XILINX_SDACCEL}/include -I ${XILINX_SDACCEL}/runtime/include/1_2 -I ../"
g++ -O3 -std=c++11 $INCLUDE -L ${BOOST_LIB} -L ../lib -L ${XILINX_SDACCEL}/runtime/lib/x86_64 main.cpp -lstdc++ -lxfdnn -lboost_system -lboost_thread -lxilinxopencl -o test.exe
