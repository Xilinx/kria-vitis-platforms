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

aux_source_directory(. CPP_SRCS)
add_library(vitis_dpu SHARED ${CPP_SRCS})
target_link_libraries(vitis_dpu glog::glog json-c dl)
message("MY_DEPS is ${MY_DEPS}")

set_target_properties(vitis_dpu PROPERTIES
  VERSION "${PROJECT_VERSION}"
  SOVERSION "${PROJECT_VERSION_MAJOR}")
install(TARGETS vitis_dpu DESTINATION lib)
