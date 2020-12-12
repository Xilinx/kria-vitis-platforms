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

#pragma once

#include <glog/logging.h>
#include <UniLog/UniLog.hpp>

#include "xir/attrs/attrs.hpp"
#include "xir/graph/graph.hpp"
#include "xir/graph/subgraph.hpp"
#include "xir/op/op.hpp"
#include "xir/op/op_def.hpp"
#include "xir/tensor/tensor.hpp"

namespace xir {
class Attrs;
class Op;
class Tensor;
class Graph;
class Subgraph;
}  // namespace xir

#include "vart/runner.hpp"
#include "vart/tensor_buffer.hpp"

#include "cpu_std_inc.hpp"
#include "cpu_util.hpp"
#include "cpu_types.hpp"
#include "cpu_dimension.hpp"
#include "cpu_cfg.hpp"
#include "cpu_helper.hpp"

