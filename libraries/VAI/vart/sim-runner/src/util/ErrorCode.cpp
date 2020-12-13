/*
 * Copyright 2019 Xilinx Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "UniLog/ErrorCode.hpp"

REGISTER_ERROR_CODE(SIM_OUT_OF_RANGE, "idx out of range!", "");
REGISTER_ERROR_CODE(SIM_FILE_OPEN_FAILED, "file open failed!", "");
REGISTER_ERROR_CODE(SIM_CFG_FMT_NO_MATCH, "config format is incorrect!", "");
REGISTER_ERROR_CODE(SIM_NULL_PTR, "null pointer", "");
REGISTER_ERROR_CODE(SIM_PARAMETER_FAILED, "parameter failed", "");
REGISTER_ERROR_CODE(SIM_TENSOR_LOCATION_ERROR, "tensor location is incorrect!",
                    "");
REGISTER_ERROR_CODE(SIM_TENSOR_DIM_ERROR, "tensor dim size is incorrect!", "");
REGISTER_ERROR_CODE(SIM_ATTR_ERROR, "attribute must supply!", "");
REGISTER_ERROR_CODE(SIM_ALIGN_ERROR, "error in meet align requirement!", "");
REGISTER_ERROR_CODE(SIM_EXCEED_HARDWARE_LIMIT,
                    "error in exceed hardware limit!", "");
REGISTER_ERROR_CODE(SIM_TENSOR_NOT_IN_DDR,
                    "This tensor is not existing in DDR.", "");
REGISTER_ERROR_CODE(SIM_TENSOR_STRIDE_UNMATCH, "The stride is unmatching.", "");
