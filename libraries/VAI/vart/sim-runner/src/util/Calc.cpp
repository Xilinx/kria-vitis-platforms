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
#include "Calc.hpp"

const array<string, Calc::RELU_TYPE_MAX> Calc::ReluTypeStr = {
    "none",
    "relu",
    "prelu",
    "bn",
    "bn_relu",
};

string Calc::GetTDMName(int method) {
    if(method == NSTRIDE_2_NSTRIDE) {
        return "NSTRIDE_2_NSTRIDE";
    } else if(method == NSTRIDE_2_YSTRIDE) {
        return "NSTRIDE_2_YSTRIDE";
    } else if(method == YSTRIDE_2_NSTRIDE) {
        return "YSTRIDE_2_NSTRIDE";
    } else if(method == YSTRIDE_2_YSTRIDE) {
        return "YSTRIDE_2_YSTRIDE";
    } else {
        UNI_LOG_FATAL(SIM_OUT_OF_RANGE) << "No such TransDimMethod " << method << endl;
    }
}
