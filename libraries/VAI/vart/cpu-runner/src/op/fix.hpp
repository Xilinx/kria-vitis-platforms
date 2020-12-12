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

#include "cpu_op_base.hpp"

namespace vart {
namespace cpu {


template<typename DType, typename FixType = int32_t>
class Fix : public CPUOPBase {
public:
  enum RoundMode {
    STD_ROUND,
    DPU_ROUND,
    PY3_ROUND
  };

  enum InputTensorType {
    INPUT,
  };
  // input tensor name
  const static vector<string> ITName;

public:
  Fix(const xir::Subgraph* subg, const xir::Op* op,
      IMapTBs_t inputs, CPUTBPtr_t output);
  virtual ~Fix() = default;

  virtual void run() override;

  virtual void print_param() override;
  virtual void check_param() override;

  virtual void read() override;

  virtual uint64_t get_workload() override final;

protected:
  void float2fix();
  void fix2float();

private:
  void float2fix_normal();
  void float2fix_thread();

  void fix2float_normal();
  void fix2float_thread();

  inline void float2fix_std_round(int i) {
    data_in_ptr_[i] *= step_;
    DType tmp = (DType)round(data_in_ptr_[i]);
    if (data_in_ptr_[i] >= 0) {
      fix_rlt_ptr_[i] = static_cast<FixType>(
          std::min(tmp, upper_bound_base_));
    } else {
      fix_rlt_ptr_[i] = static_cast<FixType>(
          std::max(tmp, lower_bound_base_));
    }
  }
  inline void float2fix_dpu_round(int i) {
    data_in_ptr_[i] *= step_;
    DType tmp = (DType)DPURound<FixType>(data_in_ptr_[i],
        static_cast<FixType>(lower_bound_base_),
        static_cast<FixType>(upper_bound_base_) );
    if (data_in_ptr_[i] >= 0) {
      fix_rlt_ptr_[i] = static_cast<FixType>(
          std::min(tmp, upper_bound_base_));
    } else {
      fix_rlt_ptr_[i] = static_cast<FixType>(
          std::max(tmp, lower_bound_base_));
    }
  }
  inline void float2fix_py3_round(int i) {
    data_in_ptr_[i] *= step_;
    DType tmp = (DType)Py3Round(data_in_ptr_[i]);
    if (data_in_ptr_[i] >= 0) {
      fix_rlt_ptr_[i] = static_cast<FixType>(
          std::min(tmp, upper_bound_base_));
    } else {
      fix_rlt_ptr_[i] = static_cast<FixType>(
          std::max(tmp, lower_bound_base_));
    }
  }
  inline void fix2float_std_round(int i) {
    DType tmp = DType(fix_rlt_ptr_[i] / step_);
    DType lower_bound_mid = std::max(tmp, lower_bound_);
    data_out_ptr_[i] = std::min(lower_bound_mid, upper_bound_);
  }
  inline void fix2float_dpu_round(int i) {
    DType tmp = DType(fix_rlt_ptr_[i] / step_);
    DType lower_bound_mid = std::max(tmp, lower_bound_);
    data_out_ptr_[i] = std::min(lower_bound_mid, upper_bound_);
  }
  inline void fix2float_py3_round(int i) {
    DType tmp = DType(fix_rlt_ptr_[i] / step_);
    DType lower_bound_mid = std::max(tmp, lower_bound_);
    data_out_ptr_[i] = std::min(lower_bound_mid, upper_bound_);
  }

// NOTE: must be private
private:
  uint32_t THREAD_NUM;
  uint32_t THREAD_WORKLOAD;
  uint32_t FMAP_SIZE;

protected:
  bool const_flag_;

  vector<int> fmap_i_;
  vector<int> fmap_o_;

  bool if_signed_;
  int fix_point_;
  int bit_width_;
  string round_mode_;

  DType step_;
  DType lower_bound_base_;
  DType upper_bound_base_;
  DType lower_bound_;
  DType upper_bound_;

  // caculate buffer
  DType* data_in_ptr_{nullptr};
  DType* data_out_ptr_{nullptr};

  // NOTE: Special handle for fix op, we save fix data in fix op's output tb,
  // because we will do dump in tb uniformly, experiments show that is the best choice.
  // and also for simplify op template design, all fix data will be int32_t,
  // and it works very well.
  FixType *fix_rlt_ptr_{nullptr};
};

} // namespace cpu
} // namespace vart


