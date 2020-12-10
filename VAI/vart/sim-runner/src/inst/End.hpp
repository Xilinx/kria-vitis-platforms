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
#ifndef __END_HPP__
#define __END_HPP__

#include "pub/InstBase.hpp"

template <DPUVersion T> class End : public InstBase {
public:
  explicit End(int inst_type, int instid, vector<string> &inst_str,
               vector<uint32_t> &inst_val);
  ~End() = default;

public:
  void Exec() override final;
  void GenInstStr(int inst_fmt) override final;
};

template <DPUVersion T>
End<T>::End(int inst_type, int instid, vector<string> &inst_str,
            vector<uint32_t> &inst_val)
    : InstBase(inst_type, instid, inst_str, inst_val) {
  Helper<T>::InstInit(inst_type_, root_debug_path_, false, opcode_,
                      debug_path_);
}

// template <DPUVersion T> End<T>::~End() {}

template <DPUVersion T> void End<T>::Exec() {}

template <DPUVersion T> void End<T>::GenInstStr(int inst_fmt) {
  ac_ = Helper<T>::GetInstStr(inst_type_, inst_fmt, dpdon_, dpdby_, field_str_);
}

#endif /* __END_HPP__ */
