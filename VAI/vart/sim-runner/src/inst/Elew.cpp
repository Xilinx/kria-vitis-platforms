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
#include "Elew.hpp"
#include "ElewInit.hpp"

template <>
Elew<DPUVersion::DPUV2>::Elew(int inst_type, int inst_id,
                              vector<string>& inst_str,
                              vector<uint32_t>& inst_val)
    : InstBase(inst_type, inst_id, inst_str, inst_val) {
  // debug
  debug_ = debug_ && SimCfg::Instance().get_debug_instr(SimCfg::DBG_INSTR_ELEW);

  Helper<DPUVersion::DPUV2>::InstInit(inst_type_, root_debug_path_, debug_,
                                      opcode_, debug_path_);
  using TABLE = TableInterface<DPUVersion::DPUV2>::inst_table;
  // get elew instruction field value
  num_ = field_val_[TABLE::ELEW_FIELD_NUM];
  bank_id_out_ = field_val_[TABLE::ELEW_FIELD_BANK_ID_OUT];
  shift_write_ = field_val_[TABLE::ELEW_FIELD_SHIFT_WRITE];
  jump_write_ = field_val_[TABLE::ELEW_FIELD_JUMP_WRITE];
  channel_group_ = field_val_[TABLE::ELEW_FIELD_CHANNEL_GROUP];
  length_ = field_val_[TABLE::ELEW_FIELD_LENGTH];
  act_type_ = field_val_[TABLE::ELEW_FIELD_ACT_TYPE];
  bank_addr_out_ = field_val_[TABLE::ELEW_FIELD_BANK_ADDR_OUT];
  valid_pixel_parallel_ = field_val_[TABLE::ELEW_FIELD_VALID_PIXEL_PARALLEL];
  jump_write_endl_ = field_val_[TABLE::ELEW_FIELD_JUMP_WRITE_ENDL];
  elew_type_ = field_val_[TABLE::ELEW_FIELD_ELEW_TYPE];
  if(elew_type_ == 1){
    UNI_LOG_CHECK(num_ == 2, SIM_PARAMETER_FAILED)
        <<" when elew_type_ == 1, do mult, only support num = 2, but"
        << num_ <<" is given";
  }
  // NOTE: InstElewInit params
  using ELEWINIT = ElewInit<DPUVersion::DPUV2>;
  // you can only get the value one time, or will cause error
  for (int i = 0; i < num_; i++) {
    id_[i] = ELEWINIT::get_id(i);
    bank_id_in_[i] = ELEWINIT::get_bank_id_in(i);
    shift_read_[i] = ELEWINIT::get_shift_read(i);
    jump_read_[i] = ELEWINIT::get_jump_read(i);
    bank_addr_in_[i] = ELEWINIT::get_bank_addr_in(i);
    jump_read_endl_[i] = ELEWINIT::get_jump_read_endl(i);
  }

  // other varialbes
  vpp_ = valid_pixel_parallel_;
  auto pp = ArchCfg::Instance().get_param().eltwise_engine().pixel_parallel();
  UNI_LOG_CHECK((uint32_t)vpp_ <= pp, SIM_PARAMETER_FAILED);
  pp_conv_ = ArchCfg::Instance().get_param().conv_engine().pixel_parallel();

  cg_ = channel_group_;
  cp_ = ArchCfg::Instance().get_param().eltwise_engine().channel_parallel();
  ic_ = cg_ * cp_;
  src_h_ = num_;
  src_w_ = length_;
  data_.resize(vpp_ * src_h_ * src_w_ * ic_);
  rlt_dtype_.resize(vpp_ * src_w_ * ic_);
}

template <>
Elew<DPUVersion::DPUV3E>::Elew(int inst_type, int inst_id,
                               vector<string>& inst_str,
                               vector<uint32_t>& inst_val)
    : InstBase(inst_type, inst_id, inst_str, inst_val) {
  // debug
  debug_ = debug_ && SimCfg::Instance().get_debug_instr(SimCfg::DBG_INSTR_ELEW);

  Helper<DPUVersion::DPUV3E>::InstInit(inst_type_, root_debug_path_, debug_,
                                       opcode_, debug_path_);
  using TABLE = TableInterface<DPUVersion::DPUV3E>::inst_table;
  // get elew instruction field value
  num_ = field_val_[TABLE::ELEW_FIELD_NUM];
  bank_id_out_ = field_val_[TABLE::ELEW_FIELD_BANK_ID_OUT];
  shift_write_ = field_val_[TABLE::ELEW_FIELD_SHIFT_WRITE];
  jump_write_ = field_val_[TABLE::ELEW_FIELD_JUMP_WRITE];
  channel_group_ = field_val_[TABLE::ELEW_FIELD_CHANNEL_GROUP];
  length_ = field_val_[TABLE::ELEW_FIELD_LENGTH];
  act_type_ = field_val_[TABLE::ELEW_FIELD_ACT_TYPE];
  bank_addr_out_ = field_val_[TABLE::ELEW_FIELD_BANK_ADDR_OUT];
  valid_pixel_parallel_ = field_val_[TABLE::ELEW_FIELD_VALID_PIXEL_PARALLEL];
  jump_write_endl_ = field_val_[TABLE::ELEW_FIELD_JUMP_WRITE_ENDL];

  // NOTE: InstElewInit params
  using ELEWINIT = ElewInit<DPUVersion::DPUV3E>;
  // you can only get the value one time, or will cause error
  for (int i = 0; i < num_; i++) {
    id_[i] = ELEWINIT::get_id(i);
    bank_id_in_[i] = ELEWINIT::get_bank_id_in(i);
    shift_read_[i] = ELEWINIT::get_shift_read(i);
    jump_read_[i] = ELEWINIT::get_jump_read(i);
    bank_addr_in_[i] = ELEWINIT::get_bank_addr_in(i);
    jump_read_endl_[i] = ELEWINIT::get_jump_read_endl(i);
  }

  // other varialbes
  vpp_ = valid_pixel_parallel_;
  auto pp = ArchCfg::Instance().get_param().eltwise_engine().pixel_parallel();
  UNI_LOG_CHECK((uint32_t)vpp_ <= pp, SIM_PARAMETER_FAILED);
  pp_conv_ = ArchCfg::Instance().get_param().conv_engine().pixel_parallel();
  cg_ = channel_group_;
  cp_ = ArchCfg::Instance().get_param().eltwise_engine().channel_parallel();
  ic_ = cg_ * cp_;
  src_h_ = num_;
  src_w_ = length_;
  data_.resize(vpp_ * src_h_ * src_w_ * ic_);
  rlt_dtype_.resize(vpp_ * src_w_ * ic_);
}

template <>
Elew<DPUVersion::DPUV3ME>::Elew(int inst_type, int inst_id,
                                vector<string>& inst_str,
                                vector<uint32_t>& inst_val)
    : InstBase(inst_type, inst_id, inst_str, inst_val) {
  // debug
  debug_ = debug_ && SimCfg::Instance().get_debug_instr(SimCfg::DBG_INSTR_ELEW);

  Helper<DPUVersion::DPUV3ME>::InstInit(inst_type_, root_debug_path_, debug_,
                                        opcode_, debug_path_);
  using TABLE = TableInterface<DPUVersion::DPUV3ME>::inst_table;
  // get elew instruction field value
  num_ = field_val_[TABLE::ELEW_FIELD_NUM];
  bank_id_out_ = field_val_[TABLE::ELEW_FIELD_BANK_ID_OUT];
  shift_write_ = field_val_[TABLE::ELEW_FIELD_SHIFT_WRITE];
  jump_write_ = field_val_[TABLE::ELEW_FIELD_JUMP_WRITE];
  channel_group_ = field_val_[TABLE::ELEW_FIELD_CHANNEL_GROUP];
  length_ = field_val_[TABLE::ELEW_FIELD_LENGTH];
  act_type_ = field_val_[TABLE::ELEW_FIELD_ACT_TYPE];
  bank_addr_out_ = field_val_[TABLE::ELEW_FIELD_BANK_ADDR_OUT];
  valid_pixel_parallel_ = field_val_[TABLE::ELEW_FIELD_VALID_PIXEL_PARALLEL];
  jump_write_endl_ = field_val_[TABLE::ELEW_FIELD_JUMP_WRITE_ENDL];

  // NOTE: InstElewInit params
  using ELEWINIT = ElewInit<DPUVersion::DPUV3ME>;
  // you can only get the value one time, or will cause error
  for (int i = 0; i < num_; i++) {
    id_[i] = ELEWINIT::get_id(i);
    bank_id_in_[i] = ELEWINIT::get_bank_id_in(i);
    shift_read_[i] = ELEWINIT::get_shift_read(i);
    jump_read_[i] = ELEWINIT::get_jump_read(i);
    bank_addr_in_[i] = ELEWINIT::get_bank_addr_in(i);
    jump_read_endl_[i] = ELEWINIT::get_jump_read_endl(i);
  }

  // other varialbes
  vpp_ = valid_pixel_parallel_;
  auto pp = ArchCfg::Instance().get_param().eltwise_engine().pixel_parallel();
  UNI_LOG_CHECK((uint32_t)vpp_ <= pp, SIM_PARAMETER_FAILED);
  pp_conv_ = ArchCfg::Instance().get_param().conv_engine().pixel_parallel();
  cg_ = channel_group_;
  cp_ = ArchCfg::Instance().get_param().eltwise_engine().channel_parallel();
  ic_ = cg_ * cp_;
  src_h_ = num_;
  src_w_ = length_;
  data_.resize(vpp_ * src_h_ * src_w_ * ic_);
  rlt_dtype_.resize(vpp_ * src_w_ * ic_);
}

template <>
Elew<DPUVersion::XVDPU>::Elew(int inst_type, int inst_id,
                              vector<string>& inst_str,
                              vector<uint32_t>& inst_val)
    : InstBase(inst_type, inst_id, inst_str, inst_val) {
  // debug
  debug_ = debug_ && SimCfg::Instance().get_debug_instr(SimCfg::DBG_INSTR_ELEW);

  Helper<DPUVersion::XVDPU>::InstInit(inst_type_, root_debug_path_, debug_,
                                      opcode_, debug_path_);
  using TABLE = TableInterface<DPUVersion::XVDPU>::inst_table;
  // get elew instruction field value
  num_ = field_val_[TABLE::ELEW_FIELD_NUM];
  bank_id_out_ = field_val_[TABLE::ELEW_FIELD_BANK_ID_OUT];
  shift_write_ = field_val_[TABLE::ELEW_FIELD_SHIFT_WRITE];
  jump_write_ = field_val_[TABLE::ELEW_FIELD_JUMP_WRITE];
  channel_group_ = field_val_[TABLE::ELEW_FIELD_CHANNEL_GROUP];
  length_ = field_val_[TABLE::ELEW_FIELD_LENGTH];
  act_type_ = field_val_[TABLE::ELEW_FIELD_ACT_TYPE];
  bank_addr_out_ = field_val_[TABLE::ELEW_FIELD_BANK_ADDR_OUT];
  valid_pixel_parallel_ = field_val_[TABLE::ELEW_FIELD_VALID_PIXEL_PARALLEL];
  jump_write_endl_ = field_val_[TABLE::ELEW_FIELD_JUMP_WRITE_ENDL];

  // NOTE: InstElewInit params
  using ELEWINIT = ElewInit<DPUVersion::XVDPU>;
  // you can only get the value one time, or will cause error
  for (int i = 0; i < num_; i++) {
    id_[i] = ELEWINIT::get_id(i);
    bank_id_in_[i] = ELEWINIT::get_bank_id_in(i);
    shift_read_[i] = ELEWINIT::get_shift_read(i);
    jump_read_[i] = ELEWINIT::get_jump_read(i);
    bank_addr_in_[i] = ELEWINIT::get_bank_addr_in(i);
    jump_read_endl_[i] = ELEWINIT::get_jump_read_endl(i);
  }

  // other varialbes
  vpp_ = valid_pixel_parallel_;
  auto pp = ArchCfg::Instance().get_param().eltwise_engine().pixel_parallel();
  UNI_LOG_CHECK((uint32_t)vpp_ <= pp, SIM_PARAMETER_FAILED);
  pp_conv_ = ArchCfg::Instance().get_param().conv_engine().pixel_parallel();
  cg_ = channel_group_;
  cp_ = ArchCfg::Instance().get_param().eltwise_engine().channel_parallel();
  ic_ = cg_ * cp_;
  src_h_ = num_;
  src_w_ = length_;
  data_.resize(vpp_ * src_h_ * src_w_ * ic_);
  rlt_dtype_.resize(vpp_ * src_w_ * ic_);
}

template <>
Elew<DPUVersion::DPU4F>::Elew(int inst_type, int inst_id,
                              vector<string>& inst_str,
                              vector<uint32_t>& inst_val)
    : InstBase(inst_type, inst_id, inst_str, inst_val) {
  // debug
  debug_ = debug_ && SimCfg::Instance().get_debug_instr(SimCfg::DBG_INSTR_ELEW);

  Helper<DPUVersion::DPU4F>::InstInit(inst_type_, root_debug_path_, debug_,
                                      opcode_, debug_path_);
  using TABLE = TableInterface<DPUVersion::DPU4F>::inst_table;
  // get elew instruction field value
  num_ = field_val_[TABLE::ELEW_FIELD_NUM];
  bank_id_out_ = field_val_[TABLE::ELEW_FIELD_BANK_ID_OUT];
  shift_write_ = field_val_[TABLE::ELEW_FIELD_SHIFT_WRITE];
  jump_write_ = field_val_[TABLE::ELEW_FIELD_JUMP_WRITE];
  channel_group_ = field_val_[TABLE::ELEW_FIELD_CHANNEL_GROUP];
  length_ = field_val_[TABLE::ELEW_FIELD_LENGTH];
  act_type_ = field_val_[TABLE::ELEW_FIELD_ACT_TYPE];
  bank_addr_out_ = field_val_[TABLE::ELEW_FIELD_BANK_ADDR_OUT];
  valid_pixel_parallel_ = field_val_[TABLE::ELEW_FIELD_VALID_PIXEL_PARALLEL];
  jump_write_endl_ = field_val_[TABLE::ELEW_FIELD_JUMP_WRITE_ENDL];

  // NOTE: InstElewInit params
  using ELEWINIT = ElewInit<DPUVersion::DPU4F>;
  // you can only get the value one time, or will cause error
  for (int i = 0; i < num_; i++) {
    id_[i] = ELEWINIT::get_id(i);
    bank_id_in_[i] = ELEWINIT::get_bank_id_in(i);
    shift_read_[i] = ELEWINIT::get_shift_read(i);
    jump_read_[i] = ELEWINIT::get_jump_read(i);
    bank_addr_in_[i] = ELEWINIT::get_bank_addr_in(i);
    jump_read_endl_[i] = ELEWINIT::get_jump_read_endl(i);
  }

  // other varialbes
  vpp_ = valid_pixel_parallel_;
  auto pp = ArchCfg::Instance().get_param().eltwise_engine().pixel_parallel();
  UNI_LOG_CHECK((uint32_t)vpp_ <= pp, SIM_PARAMETER_FAILED);
  pp_conv_ = ArchCfg::Instance().get_param().conv_engine().pixel_parallel();
  cg_ = channel_group_;
  cp_ = ArchCfg::Instance().get_param().eltwise_engine().channel_parallel();
  ic_ = cg_ * cp_;
  src_h_ = num_;
  src_w_ = length_;
  data_.resize(vpp_ * src_h_ * src_w_ * ic_);
  rlt_dtype_.resize(vpp_ * src_w_ * ic_);
}

template <>
Elew<DPUVersion::DPUV4E>::Elew(int inst_type, int inst_id,
                               vector<string>& inst_str,
                               vector<uint32_t>& inst_val)
    : InstBase(inst_type, inst_id, inst_str, inst_val) {
  // debug
  debug_ = debug_ && SimCfg::Instance().get_debug_instr(SimCfg::DBG_INSTR_ELEW);

  Helper<DPUVersion::DPUV4E>::InstInit(inst_type_, root_debug_path_, debug_,
                                       opcode_, debug_path_);
  using TABLE = TableInterface<DPUVersion::DPUV4E>::inst_table;
  // get elew instruction field value
  num_ = field_val_[TABLE::ELEW_FIELD_NUM];
  bank_id_out_ = field_val_[TABLE::ELEW_FIELD_BANK_ID_OUT];
  shift_write_ = field_val_[TABLE::ELEW_FIELD_SHIFT_WRITE];
  jump_write_ = field_val_[TABLE::ELEW_FIELD_JUMP_WRITE];
  channel_group_ = field_val_[TABLE::ELEW_FIELD_CHANNEL_GROUP];
  length_ = field_val_[TABLE::ELEW_FIELD_LENGTH];
  act_type_ = field_val_[TABLE::ELEW_FIELD_ACT_TYPE];
  bank_addr_out_ = field_val_[TABLE::ELEW_FIELD_BANK_ADDR_OUT];
  valid_pixel_parallel_ = field_val_[TABLE::ELEW_FIELD_VALID_PIXEL_PARALLEL];
  jump_write_endl_ = field_val_[TABLE::ELEW_FIELD_JUMP_WRITE_ENDL];
  stride_out_ = field_val_[TABLE::ELEW_FIELD_STRIDE_OUT];
  stride_offset_out_ = field_val_[TABLE::ELEW_FIELD_STRIDE_OFFSET_OUT];

  // NOTE: InstElewInit params
  using ELEWINIT = ElewInit<DPUVersion::DPUV4E>;
  // you can only get the value one time, or will cause error
  for (int i = 0; i < num_; i++) {
    id_[i] = ELEWINIT::get_id(i);
    bank_id_in_[i] = ELEWINIT::get_bank_id_in(i);
    shift_read_[i] = ELEWINIT::get_shift_read(i);
    jump_read_[i] = ELEWINIT::get_jump_read(i);
    bank_addr_in_[i] = ELEWINIT::get_bank_addr_in(i);
    jump_read_endl_[i] = ELEWINIT::get_jump_read_endl(i);
  }

  // other varialbes
  vpp_ = valid_pixel_parallel_;
  auto pp = ArchCfg::Instance().get_param().eltwise_engine().pixel_parallel();
  pp_conv_ = ArchCfg::Instance().get_param().conv_engine().pixel_parallel();

  auto flag{true};
  for (auto bank_id_in : bank_id_in_)
    if (bank_id_in > 32) flag = false;
  if (flag) UNI_LOG_CHECK((uint32_t)vpp_ <= pp, SIM_PARAMETER_FAILED);

  cg_ = channel_group_;
  cp_ = ArchCfg::Instance().get_param().eltwise_engine().channel_parallel();
  ic_ = cg_ * cp_;
  src_h_ = num_;
  src_w_ = length_;
  data_.resize(vpp_ * src_h_ * src_w_ * ic_);
  rlt_dtype_.resize(vpp_ * src_w_ * ic_);
}

template <DPUVersion dv>
Elew<dv>::~Elew() {}

// public funcs
template <DPUVersion dv>
void Elew<dv>::Exec() {
  // read elew data
  read();
  debug_input_fmap();
  debug_input_tick();

  // calc elew
  calc();

  // save elew result
  save();
  debug_output_fmap();
  debug_output_tick();
}

// private funcs

template <DPUVersion T>
void Elew<T>::read() {
  for (int i = 0; i < src_h_; i++) {
    auto bank_depth =
        Buffer<DPU_DATA_TYPE>::Instance().GetBank(bank_id_in_[i])->GetH();
    for (int j = 0; j < src_w_; j++) {
      for (int k = 0; k < vpp_; k++) {
        int bank_id = bank_id_in_[i] + k;
        int bank_id_base = bank_id_in_[i] / pp_conv_ * pp_conv_;
        int wrap_back = (bank_id - bank_id_base) / pp_conv_;
        auto bank = Buffer<DPU_DATA_TYPE>::Instance().GetBank(
            bank_id % pp_conv_ + bank_id_base);
        UNI_LOG_CHECK(bank != nullptr, SIM_OUT_OF_RANGE)
            << "bank_id " << bank_id % pp_conv_ + bank_id_base
            << " out of range!" << endl;

        int jump_read = jump_read_[i];
        int bank_addr = (bank_addr_in_[i] + j * jump_read +
                         wrap_back * jump_read_endl_[i]) %
                        bank_depth;
        int ddr_addr = k * src_h_ * src_w_ * ic_ + i * src_w_ * ic_ + j * ic_;

        bank->Read(bank_addr, ic_, data_.data() + ddr_addr);
      }
    }
  }
}

template <>
void Elew<DPUVersion::DPU4F>::read() {
  for (int i = 0; i < src_h_; i++) {
    auto bank_depth =
        Buffer<DPU_DATA_TYPE>::Instance().GetBank(bank_id_in_[i])->GetH();
    for (int j = 0; j < src_w_; j++) {
      for (int k = 0; k < vpp_; k++) {
        int bank_id = bank_id_in_[i] + k;
        int bank_id_base = bank_id_in_[i] / pp_conv_ * pp_conv_;
        int wrap_back = (bank_id - bank_id_base) / pp_conv_;
        auto bank = Buffer<DPU_DATA_TYPE>::Instance().GetBank(
            bank_id % pp_conv_ + bank_id_base);
        UNI_LOG_CHECK(bank != nullptr, SIM_OUT_OF_RANGE)
            << "bank_id " << bank_id % pp_conv_ + bank_id_base
            << " out of range!" << endl;

        int jump_read = jump_read_[i];
        int bank_addr = (bank_addr_in_[i] + j * jump_read +
                         wrap_back * jump_read_endl_[i]) %
                        bank_depth;
        int ddr_addr = k * src_h_ * src_w_ * ic_ + i * src_w_ * ic_ + j * ic_;
        BankShell::read(true, bank_id % pp_conv_ + bank_id_base, ic_, bank_addr,
                        data_.data() + ddr_addr);
      }
    }
  }
}

template <DPUVersion T>
void Elew<T>::calc() {
  vector<double> shift_read_factor(src_h_, 0);
  for (auto i = 0; i < src_h_; i++)
    shift_read_factor[i] = pow(2, shift_read_[i]);
  double shift_write_factor = 0;
  if (shift_write_ >= 8)
    shift_write_factor = 1.0 / pow(2, shift_write_ - 8);
  else
    shift_write_factor = pow(2, shift_write_);

  for (int r = 0; r < vpp_; r++) {
    int base_ddr_addr = r * src_h_ * src_w_ * ic_;
    int base_bank_addr = r * src_w_ * ic_;
    for (int i = 0; i < src_w_; i++) {
      for (int j = 0; j < ic_; j++) {
        // elementwise add
        double tmp = elew_type_ == 0 ? 0 : 1;
        for (int k = 0; k < src_h_; k++) {
          int data_addr = base_ddr_addr + k * src_w_ * ic_ + i * ic_ + j;
          // NOTE: must convert data tyep to doule
          if(elew_type_ == 0){
            tmp += floor((double)data_[data_addr] * 4.0 / shift_read_factor[k]);
          }else if(elew_type_ == 1){
            tmp *= floor((double)data_[data_addr] * 4.0 / shift_read_factor[k]);
          }else{
            UNI_LOG_CHECK(elew_type_ < 0, SIM_PARAMETER_FAILED);
          }
        }
        tmp *= shift_write_factor;
        tmp /= elew_type_ == 0 ? 4.0 : 16.0;

        // trunc result
        if (act_type_ == Calc::RELU_TYPE_NONE) {
          // nothing to do
        } else if (act_type_ == Calc::RELU_TYPE_RELU) {
          tmp = (tmp < 0) ? 0 : tmp;
        } else if (act_type_ == Calc::RELU_TYPE_LEAKY_RELU) {
          tmp = (tmp < 0) ? (tmp * 26. / 256.) : tmp;
        } else {
          UNI_LOG_FATAL(SIM_PARAMETER_FAILED)
              << "Not support nonlinear type " << act_type_ << endl;
        }

        int rlt_addr = base_bank_addr + i * ic_ + j;
        rlt_dtype_[rlt_addr] = Calc::DPURound<DPU_DATA_TYPE>(tmp);
      }
    }
  }
}

template <DPUVersion T>
void Elew<T>::save() {
  auto bank_depth =
      Buffer<DPU_DATA_TYPE>::Instance().GetBank(bank_id_out_)->GetH();

  for (int k = 0; k < vpp_; k++) {
    int bank_id = bank_id_out_ + k;
    int bank_id_base = bank_id_out_ / pp_conv_ * pp_conv_;
    int wrap_back = (bank_id - bank_id_base) / pp_conv_;
    auto bank = Buffer<DPU_DATA_TYPE>::Instance().GetBank(bank_id % pp_conv_ +
                                                          bank_id_base);
    UNI_LOG_CHECK(bank != nullptr, SIM_OUT_OF_RANGE)
        << "bank_id " << bank_id % pp_conv_ + bank_id_base << " out of range!"
        << endl;

    for (int i = 0; i < length_; i++) {
      int bank_addr =
          (bank_addr_out_ + i * jump_write_ + wrap_back * jump_write_endl_) %
          bank_depth;
      int ddr_addr = k * src_w_ * ic_ + i * ic_;

      bank->Write(bank_addr, ic_, rlt_dtype_.data() + ddr_addr);
    }
  }
}

template <>
void Elew<DPUVersion::DPU4F>::save() {
  auto bank_depth =
      Buffer<DPU_DATA_TYPE>::Instance().GetBank(bank_id_out_)->GetH();

  for (int k = 0; k < vpp_; k++) {
    int bank_id = bank_id_out_ + k;
    int bank_id_base = bank_id_out_ / pp_conv_ * pp_conv_;
    int wrap_back = (bank_id - bank_id_base) / pp_conv_;
    auto bank = Buffer<DPU_DATA_TYPE>::Instance().GetBank(bank_id % pp_conv_ +
                                                          bank_id_base);
    UNI_LOG_CHECK(bank != nullptr, SIM_OUT_OF_RANGE)
        << "bank_id " << bank_id % pp_conv_ + bank_id_base << " out of range!"
        << endl;

    for (int i = 0; i < length_; i++) {
      int bank_addr =
          (bank_addr_out_ + i * jump_write_ + wrap_back * jump_write_endl_) %
          bank_depth;
      int ddr_addr = k * src_w_ * ic_ + i * ic_;
      BankShell::write(true, bank_id % pp_conv_ + bank_id_base, ic_, bank_addr,
                       rlt_dtype_.data() + ddr_addr);
    }
  }
}

template <>
void Elew<DPUVersion::DPUV4E>::save() {
  auto bank_depth =
      Buffer<DPU_DATA_TYPE>::Instance().GetBank(bank_id_out_)->GetH();

  for (int k = 0; k < vpp_; k++) {
    int stride_group = (k + stride_offset_out_) / stride_out_;
    int stride_offset = (k + stride_offset_out_) % stride_out_;
    int bank_id = bank_id_out_ + stride_group;
    int bank_id_base = bank_id_out_ / pp_conv_ * pp_conv_;
    int wrap_back = (bank_id - bank_id_base) / pp_conv_;
    auto bank = Buffer<DPU_DATA_TYPE>::Instance().GetBank(bank_id % pp_conv_ +
                                                          bank_id_base);
    UNI_LOG_CHECK(bank != nullptr, SIM_OUT_OF_RANGE)
        << "bank_id " << bank_id % pp_conv_ + bank_id_base << " out of range!"
        << endl;

    for (int i = 0; i < length_; i++) {
      int bank_addr =
          (bank_addr_out_ + i * jump_write_ + stride_offset * jump_write_endl_ +
           wrap_back * stride_out_ * jump_write_endl_) %
          bank_depth;
      int ddr_addr = k * src_w_ * ic_ + i * ic_;
      bank->Write(bank_addr, ic_, rlt_dtype_.data() + ddr_addr);
    }
  }
}

// debug funcs
template <DPUVersion T>
void Elew<T>::debug_input_fmap() {
  if (!debug_) return;

  string head;
  string save_name;

  // make save file name
  stringstream ss;
  ss.clear();
  ss.str("");
  ss << debug_path_ << "elew_input_fmap." << instid_;
  ss >> save_name;

  // save elew data
  vector<DPU_DATA_TYPE> tmp(ic_ * src_h_ * src_w_);
  for (int r = 0; r < vpp_; r++) {
    int addr_offset = r * src_h_ * src_w_ * ic_;
    Calc::HWC2CHW(src_h_, src_w_, ic_, data_.data() + addr_offset, tmp.data());
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
      for (int j = 0; j < src_h_; j++) {
        int addr = i * src_h_ * src_w_ + j * src_w_;
        Util::SaveDec(save_name, tmp.data() + addr, src_w_, src_w_, SM_APPEND);
      }
    }
  }
}

template <DPUVersion T>
void Elew<T>::debug_input_tick() {
  if (!debug_) return;

  string head;
  string save_name;

  // make save file name
  stringstream ss;
  ss.clear();
  ss.str("");
  ss << debug_path_ << "elew_input_tick." << instid_;
  ss >> save_name;

  for (int i = 0; i < cg_; i++) {
    for (int j = 0; j < src_w_; j++) {
      for (int k = 0; k < src_h_; k++) {
        vector<DPU_DATA_TYPE> tmp(cp_);
        ss.clear();
        ss.str("");
        ss << "tick_" << (i * src_w_ * src_h_ + j * src_h_ + k) << ":";
        ss >> head;
        head += "\n";
        Util::SaveBin(save_name, head.data(), head.size(), SM_APPEND);
        for (int r = 0; r < vpp_; r++) {
          int off = r * src_h_ * src_w_ * ic_;
          ss.clear();
          ss.str("");
          ss << "parallel_" << r << ":";
          ss >> head;
          Util::SaveBin(save_name, head.data(), head.size(), SM_APPEND);
          for (int l = 0; l < cp_; l++) {
            int addr =
                off + k * src_w_ * cg_ * cp_ + j * cg_ * cp_ + i * cp_ + l;
            tmp[l] = data_[addr];
          }
          Util::SaveHexContBigEnd(
              save_name, reinterpret_cast<const char*>(tmp.data()),
              tmp.size() * sizeof(DPU_DATA_TYPE),
              tmp.size() * sizeof(DPU_DATA_TYPE), SM_APPEND);
        }
      }
    }
  }
}

template <>
void Elew<DPUVersion::DPUV4E>::debug_input_tick() {
  if (!debug_) return;

  string head;
  string save_name;

  // make save file name
  stringstream ss;
  ss.clear();
  ss.str("");
  ss << debug_path_ << "eltwise_input_tick." << instid_;
  ss >> save_name;

  auto bank = Buffer<DPU_DATA_TYPE>::Instance().GetBank(bank_id_in_[0]);
  auto bank_depth = bank->GetH();
  auto bank_width = bank->GetW();
  auto pp = ArchCfg::Instance().get_param().eltwise_engine().pixel_parallel();

  for (auto idx_w = 0; idx_w < length_; idx_w++) {
    for (auto idx_num = 0; idx_num < num_; idx_num++) {
      auto bank_id_base = (bank_id_in_[idx_num] / pp_conv_) * pp_conv_;
      for (auto idx_hg = 0U; idx_hg < (vpp_ + pp - 1) / pp; idx_hg++) {
        for (auto idx_cg = 0; idx_cg < cg_; idx_cg++) {
          for (auto idx_pp = 0U;
               idx_pp < std::min(static_cast<uint32_t>(vpp_), pp); idx_pp++) {
            auto ori_offset =
                (((idx_hg * pp + idx_pp) * num_ + idx_num) * src_w_ + idx_w) *
                    ic_ +
                idx_cg * cp_;
            auto bank_id =
                bank_id_base +
                (bank_id_in_[idx_num] + idx_hg * pp + idx_pp) % pp_conv_;
            auto bank_addr =
                (bank_addr_in_[idx_num] +
                 ((bank_id_in_[idx_num] - bank_id_base + idx_hg * pp + idx_pp) /
                  pp_conv_) *
                     jump_read_endl_[idx_num] +
                 idx_w * jump_read_[idx_num] + idx_cg) %
                bank_depth;
            Util::SaveHexContSmallEndBankAddr(
                save_name,
                reinterpret_cast<const char*>(data_.data() + ori_offset), cp_,
                bank_width, bank_depth, bank_id, bank_addr, SM_APPEND);
          }
        }
      }
    }
  }
}

template <DPUVersion T>
void Elew<T>::debug_output_fmap() {
  if (!debug_) return;

  string head;
  string save_name;

  // make save file name
  stringstream ss;
  ss.clear();
  ss.str("");
  ss << debug_path_ << "elew_output_fmap." << instid_;
  ss >> save_name;

  // save elew data
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

template <DPUVersion T>
void Elew<T>::debug_output_tick() {
  if (!debug_) return;

  string head;
  string save_name;

  // make save file name
  stringstream ss;
  ss.clear();
  ss.str("");
  ss << debug_path_ << "elew_output_tick." << instid_;
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
        int off = r * src_w_ * ic_;
        ss.clear();
        ss.str("");
        ss << "parallel_" << r << ":";
        ss >> head;
        Util::SaveBin(save_name, head.data(), head.size(), SM_APPEND);
        for (int k = 0; k < cp_; k++) {
          int addr = off + j * cg_ * cp_ + i * cp_ + k;
          tmp[k] = rlt_dtype_[addr];
        }
        Util::SaveHexContBigEnd(save_name,
                                reinterpret_cast<const char*>(tmp.data()),
                                tmp.size() * sizeof(DPU_DATA_TYPE),
                                cp_ * sizeof(DPU_DATA_TYPE), SM_APPEND);
      }
    }
  }
}

template <>
void Elew<DPUVersion::DPUV4E>::debug_output_tick() {
  if (!debug_) return;

  string save_name;

  stringstream ss;
  ss.clear();
  ss.str("");
  ss << debug_path_ << "eltwise_result_tick." << instid_;
  ss >> save_name;

  auto* p = reinterpret_cast<const char*>(rlt_dtype_.data());
  auto bank_depth =
      Buffer<DPU_DATA_TYPE>::Instance().GetBank(bank_id_out_)->GetH();
  auto bank_id_out_offset = bank_id_out_ % pp_conv_;
  auto bank_id_out_base = bank_id_out_ - bank_id_out_offset;

  for (auto idx_cg = 0; idx_cg < cg_; idx_cg++) {     // i
    for (auto idx_w = 0; idx_w < length_; idx_w++) {  // j
      for (auto idx_h = 0; idx_h < vpp_; idx_h++) {
        auto bank_offset = (stride_offset_out_ + idx_h) / stride_out_;
        auto bank_group_offset = (bank_id_out_offset + bank_offset) / pp_conv_;
        auto bank_id_this_row =
            bank_id_out_base + (bank_id_out_offset + bank_offset) % pp_conv_;
        auto bank_addr_this_row =
            bank_addr_out_ +
            bank_group_offset * stride_out_ * jump_write_endl_ +
            ((idx_h + stride_offset_out_) % stride_out_) * jump_write_endl_;
        auto bank_addr_this_pixel =
            (bank_addr_this_row + idx_w * jump_write_ + idx_cg) % bank_depth;
        // c->w->h
        auto ddr_offset =
            (idx_h * length_ + idx_w) * ic_ * sizeof(DPU_DATA_TYPE) +
            idx_cg * cp_;
        Util::SaveHexContSmallEndBankAddr(save_name, p + ddr_offset, cp_, cp_,
                                          bank_depth, bank_id_this_row,
                                          bank_addr_this_pixel, SM_APPEND);
      }
    }
  }
}

template <DPUVersion T>
void Elew<T>::GenInstStr(int inst_fmt) {
  ac_ = Helper<T>::GetInstStr(inst_type_, inst_fmt, dpdon_, dpdby_, field_str_);
}
