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
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include")
set (CMAKE_CXX_STANDARD 14)
set (CMAKE_C_STANDARD 99)
if (CMAKE_CROSSCOMPILING)
  if(NOT EXISTS ${CMAKE_BINARY_DIR}/sysroot.txt)
    file(WRITE ${CMAKE_BINARY_DIR}/sysroot.txt $ENV{OECORE_TARGET_SYSROOT})
  endif()
  file(STRINGS ${CMAKE_BINARY_DIR}/sysroot.txt OECORE_TARGET_SYSROOT LIMIT_INPUT 4096)
  if(OECORE_TARGET_SYSROOT STREQUAL "")
    message(FATAL_ERROR "NO OECORE_TARGET_SYSROOT")
  endif()
  include_directories(${CMAKE_INSTALL_PREFIX}/include)
  file(RELATIVE_PATH TMP ${OECORE_TARGET_SYSROOT} ${CMAKE_INSTALL_PREFIX})
  link_directories(/${TMP}/lib)
  link_directories(${CMAKE_INSTALL_PREFIX}/lib)
  set(CMAKE_INSTALL_RPATH "/${TMP}/lib")
else()
  link_directories(${CMAKE_INSTALL_PREFIX}/lib)
  include_directories(${CMAKE_INSTALL_PREFIX}/include)
endif()
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -ggdb -O0 -fno-inline")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -Wall -s")
set(CMAKE_SHARED_LINKER_FLAGS  "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--no-undefined")

# UniLogger
if(CMAKE_BUILD_TYPE MATCHES "Release")
  ADD_DEFINITIONS(-DUNI_LOG_NDEBUG)
endif()
