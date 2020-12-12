#
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
#
cmake_minimum_required(VERSION 3.5)
project(vitis-runtime-api VERSION 1.1.0 LANGUAGES C CXX)

include_directories(${CMAKE_SOURCE_DIR}/include/dpu)
if(DEFINED ENV{CONDA_PREFIX})
  include_directories($ENV{CONDA_PREFIX}/include)
  link_directories($ENV{CONDA_PREFIX}/lib)
endif()
install(DIRECTORY ${CMAKE_SOURCE_DIR}/include/dpu
    ${CMAKE_SOURCE_DIR}/include/vip
 DESTINATION include/vitis)
include(cmake/XilinxCommon.cmake)

set(CMAKE_MACOSX_RPATH 1)

add_subdirectory(src)
add_subdirectory(test)
