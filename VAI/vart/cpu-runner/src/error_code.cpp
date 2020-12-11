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

REGISTER_ERROR_CODE(VART_NULL_PTR, "nullptr!", "");
REGISTER_ERROR_CODE(VART_DIM_ERROR, "dim error!", "");
REGISTER_ERROR_CODE(VART_IDX_ERROR, "index error!", "");
REGISTER_ERROR_CODE(VART_SIZE_ERROR, "size error!", "");
REGISTER_ERROR_CODE(VART_INVALID_VALUE, "invalid value!", "");
REGISTER_ERROR_CODE(VART_NOT_FOUND, "not found!", "");
REGISTER_ERROR_CODE(VART_FOUND, "found!", "");

REGISTER_ERROR_CODE(VART_FILE_ERROR, "file operation error!", "");
REGISTER_ERROR_CODE(VART_DATA_TYPE_ERROR, "data type error!", "");
REGISTER_ERROR_CODE(VART_INVALIDE_PARAM, "invalid param!", "");
REGISTER_ERROR_CODE(VART_NOT_SUPPORT, "not support!", "");
REGISTER_ERROR_CODE(VART_BUF_SIZE_ERROR, "buffer size error!", "");
REGISTER_ERROR_CODE(VART_EXEC_ERROR, "exec error!", "");

REGISTER_ERROR_CODE(VART_CPU_TENSOR_BUFFER_OPERATION_FAIL,
                    "cpu tensor buffer operation failed! ", "");
