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
#include "CBLoad.hpp"

// constructor and deconstructor
CBLoad::CBLoad(int inst_type, int instid, vector<string>& inst_str,
               vector<uint32_t>& inst_val)
    : InstBase(inst_type, instid, inst_str, inst_val) {
  // debug
  debug_ = false;
  Helper<DPUVersion::DPUV3ME>::InstInit(inst_type_, root_debug_path_, debug_,
                                     opcode_, debug_path_);
  using TABLE = TableInterface<DPUVersion::DPUV3ME>::inst_table;
  // get CBLoad instruction field value
  length_ = field_val_[TABLE::CBLOAD_FIELD_LENGTH];
  channel_group_ = field_val_[TABLE::CBLOAD_FIELD_CHANNEL_GROUP];
  group_id_in_ = field_val_[TABLE::CBLOAD_FIELD_GROUP_ID_IN];
  bank_addr_in_ = field_val_[TABLE::CBLOAD_FIELD_BANK_ADDR_IN];
  bank_addr_out_ = field_val_[TABLE::CBLOAD_FIELD_BANK_ADDR_OUT];
  cb_sel_ = field_val_[TABLE::CBLOAD_FIELD_CB_SEL];

  int fmap_num = SimCfg::Instance().get_fmap_bank_group();
  UNI_LOG_CHECK(group_id_in_ < fmap_num, SIM_PARAMETER_FAILED)
      << "invalid group_id_in, only HBM bank group ids are allowed" << endl;
}

CBLoad::~CBLoad() {}

void CBLoad::Exec() {
  int pixel_parallel =
      ArchCfg::Instance().get_param().move_engine().pixel_parallel();
  int bank_id_start = group_id_in_ * pixel_parallel;
  int cb_bank_id_start =
      (SimCfg::Instance().get_fmap_bank_group() - 1) * pixel_parallel;

  auto cp_in =
      ArchCfg::Instance().get_param().move_engine().input_channel_parallel();
  auto cp_out =
      ArchCfg::Instance().get_param().move_engine().output_channel_parallel();
  int cp_scale = cp_out / cp_in;
  int cg_num = std::ceil(double(channel_group_) / cp_scale);
  std::vector<DPU_DATA_TYPE> tmp;
  int num_bytes = cp_out;

  for (auto bank_idx = 0; bank_idx < pixel_parallel; bank_idx++) {
    if (!(cb_sel_ & (0x1 << bank_idx))) continue;
    for (auto length_idx = 0; length_idx < length_; length_idx++) {
      for (auto cg_idx = 0; cg_idx < cg_num; cg_idx++) {
        if (cg_idx == cg_num - 1) {
          if (channel_group_ % cp_scale != 0) {
            num_bytes = cp_in * (channel_group_ % cp_scale);
          }
        } else
          num_bytes = cp_out;

        tmp.resize(num_bytes);
        int bank_addr_in =
            bank_addr_in_ + length_idx * channel_group_ + cg_idx * cp_scale;
        auto bank_in =
            Buffer<DPU_DATA_TYPE>::Instance().GetBank(bank_id_start + bank_idx);
        bank_in->Read(bank_addr_in, num_bytes, tmp.data());

        int bank_addr_out = bank_addr_out_ + length_idx * cg_num + cg_idx;
        auto bank_out = Buffer<DPU_DATA_TYPE>::Instance().GetBank(
            cb_bank_id_start + bank_idx);
        bank_out->Write(bank_addr_out, num_bytes, tmp.data());
      }
    }
  }
}

void CBLoad::GenInstStr(int inst_fmt) {
  ac_ = Helper<DPUVersion::DPUV3ME>::GetInstStr(inst_type_, inst_fmt, dpdon_,
                                             dpdby_, field_str_);
}
