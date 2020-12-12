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
#include "sim_tensor_buffer.hpp"
#include <algorithm>
#include "UniLog/UniLog.hpp"
#include "buffer/DDR.hpp"
#include "xir/tensor/tensor.hpp"

namespace vart {
namespace sim {

template <typename DType>
SimTensorBuffer<DType>::SimTensorBuffer(const xir::Tensor* tensor)
    : TensorBuffer(tensor) {
  buf_.resize(tensor->get_element_num());
}

template <typename DType>
std::pair<uint64_t, size_t> SimTensorBuffer<DType>::data(
    const std::vector<std::int32_t> index) {
  auto dim_num = tensor_->get_shape().size();
  UNI_LOG_CHECK(dim_num == 4, SIM_TENSOR_DIM_ERROR);

  auto dim = tensor_->get_shape();
  auto iter = std::find_if(index.rbegin(), index.rend(),
                           [](int x) { return (x != 0); });
  size_t idx = iter - index.rbegin();

  size_t chunk_size = 1;
  if (idx == 4)
    chunk_size = dim[3] * dim[2] * dim[1];
  else {
    for (auto i = idx; i > 0; i--) chunk_size *= dim[4 - i];
  }

  uint32_t offset = index[0];
  for (auto i = 1U; i < index.size(); i++) {
    offset = offset * dim[i] + index[i];
  }
  auto pdata = buf_.data() + offset;

  return {reinterpret_cast<uint64_t>(pdata), chunk_size};
}

template class SimTensorBuffer<int8_t>;
template class SimTensorBuffer<uint8_t>;
template class SimTensorBuffer<int16_t>;
template class SimTensorBuffer<uint16_t>;
template class SimTensorBuffer<int32_t>;
template class SimTensorBuffer<uint32_t>;
template class SimTensorBuffer<int64_t>;
template class SimTensorBuffer<uint64_t>;
template class SimTensorBuffer<float>;
template class SimTensorBuffer<double>;

}  // namespace sim
}  // namespace vart
