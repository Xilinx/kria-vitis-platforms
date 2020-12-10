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
#include "Thd.hpp"

// constructor and deconstructor
Thd::Thd(int inst_type, int instid, vector<string>& inst_str,
         vector<uint32_t>& inst_val)
    : InstBase(inst_type, instid, inst_str, inst_val) {
  // debug
  debug_ = debug_ && SimCfg::Instance().get_debug_instr(SimCfg::DBG_INSTR_THD);
  Helper<DPUVersion::DPU4F>::InstInit(inst_type_, root_debug_path_, debug_,
                                        opcode_, debug_path_);
  using TABLE = TableInterface<DPUVersion::DPU4F>::inst_table;
  // get threshold instruction field value
  channel_group_ = field_val_[TABLE::THD_FIELD_CHANNEL_GROUP];
  bank_addr_in_ = field_val_[TABLE::THD_FIELD_BANK_ADDR_IN];

  signed_mode_ = field_val_[TABLE::THD_FIELD_SIGNED_MODE];
  valid_pixel_parallel_ = field_val_[TABLE::THD_FIELD_VALID_PIXEL_PARALLEL];
  length_ = field_val_[TABLE::THD_FIELD_LENGTH];
  bank_addr_out_ = field_val_[TABLE::THD_FIELD_BANK_ADDR_OUT];

  stride_out_ = field_val_[TABLE::THD_FIELD_STRIDE_OUT];
  bank_id_in_ = field_val_[TABLE::THD_FIELD_BANK_ID_IN];
  jump_read_ = field_val_[TABLE::THD_FIELD_JUMP_READ];
  jump_read_endl_ = field_val_[TABLE::THD_FIELD_JUMP_READ_ENDL];

  stride_offset_out_ = field_val_[TABLE::THD_FIELD_STRIDE_OFFSET_OUT];
  bank_id_out_ = field_val_[TABLE::THD_FIELD_BANK_ID_OUT];
  jump_write_ = field_val_[TABLE::THD_FIELD_JUMP_WRITE];
  jump_write_endl_ = field_val_[TABLE::THD_FIELD_JUMP_WRITE_ENDL];

  param_addr_ = field_val_[TABLE::THD_FIELD_PARAM_ADDR];

  // other varialbes
  vpp_ = valid_pixel_parallel_;
  auto pp = ArchCfg::Instance().get_param().threshold_engine().pixel_parallel();
  UNI_LOG_CHECK(vpp_ <= static_cast<int32_t>(pp), SIM_PARAMETER_FAILED);
  pp_conv_ = ArchCfg::Instance().get_param().conv_engine().pixel_parallel();
  cg_ = channel_group_;
  cp_ = ArchCfg::Instance().get_param().threshold_engine().channel_parallel();
  ic_ = cg_ * cp_;
  src_w_ = length_;
  data_.resize(vpp_ * src_w_ * ic_);
  param_.resize(3 * ic_);
  rlt_dtype_.resize(vpp_ * src_w_ * ic_);
  base_.resize(ic_);
  delta_.resize(ic_);
  signal_bit_.resize(ic_);
}

Thd::~Thd() {}

// public funcs
void Thd::Exec() {
  // read threshold data
  read();
  debug_input_fmap();
  debug_input_tick();

  // calc threshold
  calc();

  // save threshold result
  save();
  debug_output_fmap();
  debug_output_tick();
}

// private funcs
void Thd::read() {
  // read data
  for (int j = 0; j < src_w_; j++) {
    for (int k = 0; k < vpp_; k++) {
      int bank_id = bank_id_in_ + k;
      int bank_id_base = bank_id_in_ / pp_conv_ * pp_conv_;
      int wrap_back = (bank_id - bank_id_base) / pp_conv_;
      auto bank = Buffer<DPU_DATA_TYPE>::Instance().GetBank(bank_id % pp_conv_ +
                                                            bank_id_base);
      bank_id = bank_id % pp_conv_ + bank_id_base;
      int jump_read = jump_read_;
      int bank_addr =
          bank_addr_in_ + j * jump_read + wrap_back * jump_read_endl_;
      int ddr_addr = k * src_w_ * ic_ + j * ic_;
      BankShell::read(true, bank_id, ic_, bank_addr, data_.data() + ddr_addr);
    }
  }
  // read param
  auto param_bank_group =
      ArchCfg::Instance().get_param().threshold_engine().param_bank();
  auto param_bank =
      Buffer<DPU_DATA_TYPE>::Instance().GetBank(param_bank_group, 0);
  for (auto idx_cg = 0; idx_cg < cg_; idx_cg++) {
    param_bank->Read(param_addr_ + 3 * idx_cg, 3 * cp_, param_.data());
    auto cp_offset = idx_cg * cp_;
    for (auto idx_oc = 0; idx_oc < cp_; idx_oc++) {
      param_union tmp;
      tmp.src.data0 = param_[3 * idx_oc];
      tmp.src.data1 = param_[3 * idx_oc + 1];
      tmp.src.data2 = param_[3 * idx_oc + 2];
      base_[cp_offset + idx_oc] = static_cast<int16_t>(tmp.dst.base);
      trans_delta trans = {static_cast<uint16_t>(tmp.dst.delta)};
      delta_[cp_offset + idx_oc] = static_cast<uint16_t>(trans.real_delta);
      signal_bit_[cp_offset + idx_oc] = static_cast<bool>(tmp.dst.signal_bit);
    }
  }
}

void Thd::calc() {
  for (int r = 0; r < vpp_; r++) {
    int base_ddr_addr = r * src_w_ * ic_;
    int base_bank_addr = r * src_w_ * ic_;
    for (int i = 0; i < src_w_; i++) {
      for (int j = 0; j < ic_; j++) {
        int rlt_addr = base_bank_addr + i * ic_ + j;
        // threshold compare
        auto data_addr = base_ddr_addr + i * ic_ + j;
        if (delta_[j]) {
          auto tmp = ceil((static_cast<double>(base_[j]) -
                           static_cast<double>(data_[data_addr]) * 4.0) /
                          delta_[j]);
          if (tmp > 8)
            rlt_dtype_[rlt_addr] = 0;
          else if (tmp < -7)
            rlt_dtype_[rlt_addr] = static_cast<DPU_DATA_TYPE>(15);
          else
            rlt_dtype_[rlt_addr] = static_cast<DPU_DATA_TYPE>(8 - tmp);
        } else {
          if (base_[j] <= data_[data_addr] * 4.0)
            rlt_dtype_[rlt_addr] = static_cast<DPU_DATA_TYPE>(15);
          else
            rlt_dtype_[rlt_addr] = static_cast<DPU_DATA_TYPE>(0);
        }
        if (signal_bit_[j]) rlt_dtype_[rlt_addr] = 15 - rlt_dtype_[rlt_addr];
        if (signed_mode_) rlt_dtype_[rlt_addr] -= 8;
      }
    }
  }
}

void Thd::save() {
  auto bank_id_out_base = bank_id_out_ - bank_id_out_ % pp_conv_;
  // bank id of the first bank in this bank_group
  auto bank_id_out_offset = bank_id_out_ % pp_conv_;
  auto bank_depth =
      Buffer<DPU_DATA_TYPE>::Instance().GetBank(bank_id_out_)->GetH();
  for (auto idx_h = 0; idx_h < vpp_; idx_h++) {
    auto bank_offset = static_cast<uint32_t>(
        floor((float)(idx_h + stride_offset_out_) / stride_out_));
    // how many times does it change the bank
    auto bank_group_offset = static_cast<uint32_t>(
        floor((float)(bank_id_out_offset + bank_offset) / pp_conv_));
    // how many times does it change the bank group
    auto bank_id_this_row =
        bank_id_out_base + (bank_id_out_offset + bank_offset) % pp_conv_;
    auto bank_addr_this_row =
        bank_addr_out_                                        // base_addr
        + bank_group_offset * stride_out_ * jump_write_endl_  // change bank
        + ((idx_h + stride_offset_out_) % stride_out_) *
              jump_write_endl_  // change stride offset
        ;
    // cout << "bank_id : " << bank_id_this_row << endl;
    for (int idx_w = 0; idx_w < length_; idx_w++) {
      auto bank_addr_this_pixel =
          (bank_addr_this_row + idx_w * jump_write_) % bank_depth;
      auto rlt_addr_this_pixel = idx_h * length_ * ic_ + idx_w * ic_;

      BankShell::write(false, bank_id_this_row, ic_, bank_addr_this_pixel,
                       rlt_dtype_.data() + rlt_addr_this_pixel);
    }
  }
}

// debug funcs
void Thd::debug_input_fmap() {
  if (!debug_) return;

  string head;
  string save_name;
  string save_name_base;
  string save_name_delta;
  string save_name_signal_bit;

  // make save file name
  stringstream ss;
  ss.clear();
  ss.str("");
  ss << debug_path_ << "thd_input_fmap." << instid_;
  ss >> save_name;
  ss.clear();
  ss.str("");
  ss << debug_path_ << "base_input_fmap." << instid_;
  ss >> save_name_base;
  ss.clear();
  ss.str("");
  ss << debug_path_ << "delta_input_fmap." << instid_;
  ss >> save_name_delta;
  ss.clear();
  ss.str("");
  ss << debug_path_ << "signal_bit_input_fmap." << instid_;
  ss >> save_name_signal_bit;

  // save thd data
  vector<DPU_DATA_TYPE> tmp(ic_ * src_w_);
  for (int r = 0; r < vpp_; r++) {
    int addr_offset = r * src_w_ * ic_;
    Calc::HWC2CHW(1, src_w_, ic_, data_.data() + addr_offset, tmp.data());
    ss.clear();
    ss.str("");
    ss << "parallel_" << r << ":";
    ss >> head;
    head += "\n";
    Util::SaveBin(save_name, head.data(), head.size(), SM_APPEND);
    for (int i = 0; i < ic_; i++) {
      // save input_channel title to file
      ss.clear();
      ss.str("");
      ss << "input_channel_" << i << ":";
      ss >> head;
      head += "\n";
      Util::SaveBin(save_name, head.data(), head.size(), SM_APPEND);
      int addr = i * src_w_;
      Util::SaveDec(save_name, tmp.data() + addr, src_w_, src_w_, SM_APPEND);
    }
  }
  // save base data
  for (int i = 0; i < channel_group_ * cp_; i++) {
    // save output_channel title to file
    ss.clear();
    ss.str("");
    ss << "output_channel_" << i << "-------------------------------------";
    ss >> head;
    head += "\n";
    Util::SaveBin(save_name_base, head.data(), head.size(), SM_APPEND);
    Util::SaveDec(save_name_base, base_.data() + i, 1, 1, SM_APPEND);
  }
  // save delta data
  for (int i = 0; i < channel_group_ * cp_; i++) {
    // save output_channel title to file
    ss.clear();
    ss.str("");
    ss << "output_channel_" << i << "-------------------------------------";
    ss >> head;
    head += "\n";
    Util::SaveBin(save_name_delta, head.data(), head.size(), SM_APPEND);
    Util::SaveDec(save_name_delta, delta_.data() + i, 1, 1, SM_APPEND);
  }
  // save signal_bit data
  vector<int> print_help;
  print_help.resize(ic_);
  for (int i = 0; i < cg_ * cp_; i++) {
    if (signal_bit_[i]) print_help[i] = 1;
    // save output_channel title to file
    ss.clear();
    ss.str("");
    ss << "output_channel_" << i << "-------------------------------------";
    ss >> head;
    head += "\n";
    Util::SaveBin(save_name_signal_bit, head.data(), head.size(), SM_APPEND);
    Util::SaveDec(save_name_signal_bit, print_help.data() + i, 1, 1, SM_APPEND);
  }
}

void Thd::debug_input_tick() {
  if (!debug_) return;

  string head;
  string save_name;

  // make save file name
  stringstream ss;
  ss.clear();
  ss.str("");
  ss << debug_path_ << "thd_input_tick." << instid_;
  ss >> save_name;

  for (int i = 0; i < cg_; i++) {
    for (int j = 0; j < src_w_; j++) {
      vector<DPU_DATA_TYPE> tmp(cp_);
      ss.clear();
      ss.str("");
      ss << "tick_" << (i * src_w_ + j) << ":";
      ss >> head;
      head += "\n";
      Util::SaveBin(save_name, head.data(), head.size(), SM_APPEND);
      for (int r = 0; r < vpp_; r++) {
        int off = r * src_w_ * ic_;  // offset_h
        ss.clear();
        ss.str("");
        ss << "parallel_" << r << ":";
        ss >> head;
        Util::SaveBin(save_name, head.data(), head.size(), SM_APPEND);
        for (int l = 0; l < cp_; l++) {
          int addr = off + j * cg_ * cp_ + i * cp_ + l;
          // cout << "addr: " << addr << " input: ";
          tmp[l] = data_[addr];
          // cout << +tmp[l] << endl;
        }
        Util::SaveHexContBigEnd(save_name,
                                reinterpret_cast<const char*>(tmp.data()),
                                tmp.size() * sizeof(DPU_DATA_TYPE),
                                tmp.size() * sizeof(DPU_DATA_TYPE), SM_APPEND);
      }
    }
  }
}

void Thd::debug_output_fmap() {
  if (!debug_) return;

  string head;
  string save_name;

  // make save file name
  stringstream ss;
  ss.clear();
  ss.str("");
  ss << debug_path_ << "thd_output_fmap." << instid_;
  ss >> save_name;

  // save thd data
  vector<DPU_DATA_TYPE> tmp(ic_ * 1 * src_w_);
  for (int r = 0; r < vpp_; r++) {
    int addr_off = r * src_w_ * ic_;
    Calc::HWC2CHW(1, src_w_, ic_, rlt_dtype_.data() + addr_off, tmp.data());
    ss.clear();
    ss.str("");
    ss << "parallel_" << r << ":";
    ss >> head;
    head += "\n";
    Util::SaveBin(save_name, head.data(), head.size(), SM_APPEND);
    for (int i = 0; i < ic_; i++) {
      // save input_channel title to file
      ss.clear();
      ss.str("");
      ss << "channel_" << i << ":";
      ss >> head;
      head += "\n";
      Util::SaveBin(save_name, head.data(), head.size(), SM_APPEND);

      int addr = i * src_w_;
      Util::SaveDec(save_name, tmp.data() + addr, src_w_, src_w_, SM_APPEND);
    }
  }
}

void Thd::debug_output_tick() {
  if (!debug_) return;

  string head;
  string save_name;

  // make save file name
  stringstream ss;
  ss.clear();
  ss.str("");
  ss << debug_path_ << "thd_output_tick." << instid_;
  ss >> save_name;

  for (int i = 0; i < cg_; i++) {       // idx_channel_group
    for (int j = 0; j < src_w_; j++) {  // idx_length
      vector<DPU_DATA_TYPE> tmp(cp_);
      ss.clear();
      ss.str("");
      ss << "tick_" << (i * src_w_ + j) << ":";
      ss >> head;
      head += "\n";
      Util::SaveBin(save_name, head.data(), head.size(), SM_APPEND);
      for (int r = 0; r < vpp_; r++) {  // idx_vpp
        int off = r * src_w_ * ic_;
        ss.clear();
        ss.str("");
        ss << "parallel_" << r << ":";
        ss >> head;
        Util::SaveBin(save_name, head.data(), head.size(), SM_APPEND);
        for (int k = 0; k < cp_; k++) {
          int addr = off + j * cg_ * cp_ + i * cp_ + k;
          // cout << "addr: " << addr;
          tmp[k] = rlt_dtype_[addr];
          // cout << " data: " << +tmp[k] << endl;
        }
        Util::SaveHexContBigEnd(save_name,
                                reinterpret_cast<const char*>(tmp.data()),
                                cp_ * sizeof(DPU_DATA_TYPE),
                                cp_ * sizeof(DPU_DATA_TYPE), SM_APPEND);
        tmp.clear();
      }
    }
  }
}

void Thd::GenInstStr(int inst_fmt) {
  ac_ = Helper<DPUVersion::DPU4F>::GetInstStr(inst_type_, inst_fmt, dpdon_,
                                                dpdby_, field_str_);
}
