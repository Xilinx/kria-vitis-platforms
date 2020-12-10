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
get_filename_component(COMPONENT_NAME "${CMAKE_CURRENT_SOURCE_DIR}" NAME)

aux_source_directory(src SRC_FILES)
aux_source_directory(src/op SRC_FILES)
aux_source_directory(src/alg SRC_FILES)
aux_source_directory(src/vis SRC_FILES)
aux_source_directory(src/api SRC_FILES)
aux_source_directory(src/sample SRC_FILES)

add_library(${COMPONENT_NAME} SHARED ${SRC_FILES})
set_target_properties(${COMPONENT_NAME} PROPERTIES
  VERSION "${PROJECT_VERSION}"
  SOVERSION "${PROJECT_VERSION_MAJOR}"
  LIBRARY_OUTPUT_NAME ${PROJECT_NAME}-${COMPONENT_NAME})
target_include_directories(${COMPONENT_NAME}
  PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
  PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/src
  ${CMAKE_CURRENT_SOURCE_DIR}/src/alg
  ${CMAKE_CURRENT_SOURCE_DIR}/src/op
  ${CMAKE_CURRENT_SOURCE_DIR}/src/vis
  ${CMAKE_CURRENT_SOURCE_DIR}/src/api
  ${CMAKE_CURRENT_SOURCE_DIR}/src/sample
)
target_link_libraries(${COMPONENT_NAME}
  PUBLIC util runner mem-manager xir::xir unilog::unilog)

install(
  TARGETS ${COMPONENT_NAME}
  EXPORT ${COMPONENT_NAME}-targets
  COMPONENT cpu
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib)
install(FILES
  include/cpu_std_inc.hpp
  include/cpu_base_inc.hpp
  include/cpu_util.hpp
  include/load_cfg.hpp
  include/cpu_cfg.hpp
  include/cpu_types.hpp
  include/cpu_dimension.hpp
  include/cpu_helper.hpp
  include/cpu_reg_func.hpp
  include/cpu_op_base.hpp
  include/cpu_tensor_buffer.hpp
  include/cpu_op_visitor.hpp
  include/cpu_runner.hpp
  COMPONENT cpu
  DESTINATION include/vart/cpu)

install(
  EXPORT ${COMPONENT_NAME}-targets
  NAMESPACE ${PROJECT_NAME}::
  COMPONENT cpu
  DESTINATION share/cmake/${PROJECT_NAME})

if(BUILD_TEST)
  add_subdirectory(test)
endif()
