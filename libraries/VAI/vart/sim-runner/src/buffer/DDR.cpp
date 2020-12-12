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
#include "buffer/DDR.hpp"
#include "SimCfg.hpp"
#include "UniLog/UniLog.hpp"
#include "util/Util.hpp"
#include "xir/graph/subgraph.hpp"
#include "xir/tensor/tensor.hpp"

char* DDR::GetAddr(int reg_id, uint64_t offset) {
  auto iter =
      std::find_if(ddr_buf_.begin(), ddr_buf_.end(), [reg_id](const Reg& rg) {
        if (rg.id == reg_id) return true;
        return false;
      });
  UNI_LOG_CHECK(iter != ddr_buf_.end(), SIM_OUT_OF_RANGE)
      << "ddr space for reg_id: " << reg_id << "is not allocated!";

  UNI_LOG_CHECK(offset < iter->size, SIM_OUT_OF_RANGE)
      << "ddr reg id: " << reg_id << ", offset: " << offset;

  return ((iter->data).data() + offset);
}

uint64_t DDR::GetSize(int reg_id) const {
  auto iter =
      std::find_if(ddr_buf_.begin(), ddr_buf_.end(), [reg_id](const Reg& rg) {
        if (rg.id == reg_id) return true;
        return false;
      });
  UNI_LOG_CHECK(iter != ddr_buf_.end(), SIM_OUT_OF_RANGE)
      << "ddr space for reg_id: " << reg_id << "is not allocated!";

  return iter->size;
}

void DDR::SaveReg(const std::string file) {
  ofstream ofs(file, std::ios::app);
  Util::ChkOpen(ofs, file);

  ofs << std::endl;
  ofs << "# memory size for each reg id" << std::endl;
  for (auto& reg : ddr_buf_) {
    ofs << "reg" << reg.id << " : 0x" << hex << reg.size << std::endl;
  }
  ofs << std::endl;
  ofs << "# memory type: 0-code, 1-parameter, 2-data" << std::endl;
  for (auto& reg : reg_id_to_context_type_) {
    ofs << "reg" << GetRegID(reg.first) << " : " << reg.second << std::endl;
  }
  ofs << std::endl;
  ofs << "# memory shared: 0-local, 1-shared" << std::endl;
  for (auto& reg : reg_id_to_shared_) {
    ofs << "reg" << GetRegID(reg.first) << " : " << reg.second << std::endl;
  }
  ofs.close();
}

void DDR::SaveDDR(std::string save_name, int fmt, bool skip) {
  auto hp_width_ = SimCfg::Instance().get_hp_width();
  UNI_LOG_CHECK(hp_width_ % sizeof(DPU_DATA_TYPE) == 0, SIM_PARAMETER_FAILED)
      << "hp_width doesn't meet the requirement!";

  std::string ori_name = save_name;
  if (!SimCfg::Instance().get_ddr_dump_split()) {
    save_name += Util::GetFileNameSuffix(fmt);
    Util::ChkFile(save_name, true);
  }

  for (auto& reg : ddr_buf_) {
    if (reg.data.size() == 0) continue;
    auto buf = reg.data;
    std::string reg_type;
    if (reg.id == 8)
      reg_type = reg_id_to_context_type_.at("REG_CODE");
    else
      reg_type = reg_id_to_context_type_.at("REG_" + std::to_string(reg.id));

    if (skip && ((reg_type == "CONST") || (reg_type == "CODE"))) continue;
    if (SimCfg::Instance().get_ddr_dump_split()) {
      save_name = ori_name + "_reg_" + std::to_string(reg.id) +
                  Util::GetFileNameSuffix(fmt);
      Util::ChkFile(save_name, true);
    }

    if (fmt <= DATA_FMT_HEX_CONT_BIGEND) {
      auto* data_dtype_ptr = reinterpret_cast<DPU_DATA_TYPE*>(buf.data());
      auto data_dtype_size = buf.size() / sizeof(DPU_DATA_TYPE);
      auto line_size =
          (fmt == DATA_FMT_DEC) ? 1 : (hp_width_ / sizeof(DPU_DATA_TYPE));
      Util::SaveData(fmt, save_name, data_dtype_ptr, data_dtype_size, line_size,
                     SM_APPEND);
    } else if (fmt == DATA_FMT_HEX_CONT_SMALLEND_DDRADDR) {
      Util::SaveHexContSmallEndDDRAddr(save_name, buf.data(), buf.size(),
                                       hp_width_, 0, reg.id, SM_APPEND);
    } else if (fmt == DATA_FMT_HEX_CONT_BIGEND_DDRADDR) {
      Util::SaveHexContBigEndDDRAddr(save_name, buf.data(), buf.size(),
                                     hp_width_, 0, reg.id, SM_APPEND);
    } else {
      UNI_LOG_FATAL(SIM_OUT_OF_RANGE) << "Not support fmt: " << fmt;
    }
  }
}

void DDR::SaveDDR(
    std::string save_name,
    const std::vector<std::tuple<int32_t, int32_t, int32_t>>& regs, int fmt,
    bool skip) {
  auto hp_width_ = SimCfg::Instance().get_hp_width();
  UNI_LOG_CHECK(hp_width_ % sizeof(DPU_DATA_TYPE) == 0, SIM_PARAMETER_FAILED)
      << "hp_width doesn't meet the requirement!";

  std::string ori_name = save_name;
  if (!SimCfg::Instance().get_ddr_dump_split()) {
    Util::ChkFile(save_name, true);
  }

  for (auto& entry : regs) {
    auto reg_id = std::get<0>(entry);
    auto ddr_offset = std::get<1>(entry) / hp_width_ * hp_width_;
    auto size = std::get<2>(entry);
    size += std::get<1>(entry) - ddr_offset;
    size = (size + hp_width_ - 1) / hp_width_ * hp_width_;

    auto iter = std::find_if(ddr_buf_.begin(), ddr_buf_.end(),
                             [&reg_id](const Reg& rg) {
                               if (rg.id == reg_id) return true;
                               return false;
                             });

    auto buf = iter->data;
    std::string reg_type;
    if (reg_id == 8)
      reg_type = reg_id_to_context_type_.at("REG_CODE");
    else
      reg_type = reg_id_to_context_type_.at("REG_" + std::to_string(reg_id));

    if (skip && ((reg_type == "CONST") || (reg_type == "CODE"))) continue;
    if (SimCfg::Instance().get_ddr_dump_split()) {
      save_name = ori_name + "_reg_" + std::to_string(reg_id) +
                  Util::GetFileNameSuffix(fmt);
      Util::ChkFile(save_name, true);
    }

    if (fmt <= DATA_FMT_HEX_CONT_BIGEND) {
      auto* data_dtype_ptr =
          reinterpret_cast<DPU_DATA_TYPE*>(buf.data() + ddr_offset);
      auto line_size =
          (fmt == DATA_FMT_DEC) ? 1 : (hp_width_ / sizeof(DPU_DATA_TYPE));
      Util::SaveData(fmt, save_name, data_dtype_ptr,
                     size / sizeof(DPU_DATA_TYPE), line_size, SM_APPEND);
    } else if (fmt == DATA_FMT_HEX_CONT_SMALLEND_DDRADDR) {
      Util::SaveHexContSmallEndDDRAddr(save_name, buf.data() + ddr_offset, size,
                                       hp_width_, ddr_offset, reg_id,
                                       SM_APPEND);
    } else if (fmt == DATA_FMT_HEX_CONT_BIGEND_DDRADDR) {
      Util::SaveHexContBigEndDDRAddr(save_name, buf.data() + ddr_offset, size,
                                     hp_width_, ddr_offset, reg_id, SM_APPEND);
    } else {
      UNI_LOG_FATAL(SIM_OUT_OF_RANGE) << "Not support fmt: " << fmt;
    }
  }
}

int DDR::GetRegID(std::string reg_id) {
  int id;
  if (reg_id == "REG_CODE")
    id = 8;  // default reg id for code
  else
    id = stoi(reg_id.substr(4));  // prefix: "REG_"
  return id;
}

void DDR::Initial(const xir::Subgraph* subg) {
  UNI_LOG_INFO << "DDR initializing...";
  auto reg_to_size =
      subg->get_attr<std::map<std::string, std::int32_t>>("reg_id_to_size");

  reg_id_to_context_type_ = subg->get_attr<std::map<std::string, std::string>>(
      "reg_id_to_context_type");
  reg_id_to_shared_ =
      subg->get_attr<std::map<std::string, std::string>>("reg_id_to_shared");

  ddr_buf_.clear();
  for (auto& e : reg_to_size) {
    struct Reg ddr;
    ddr.id = GetRegID(e.first);
    ddr.size = std::ceil((double)e.second / 4096) * 4096;
    ddr.data.resize(ddr.size);
    ddr_buf_.push_back(ddr);
  }

  if (subg->has_attr("reg_id_to_parameter_value")) {
    auto reg_to_data = subg->get_attr<std::map<std::string, std::vector<char>>>(
        "reg_id_to_parameter_value");
    for (auto& e : reg_to_data) {
      int reg_id = GetRegID(e.first);
      std::vector<char> dat = e.second;
      auto iter = std::find_if(ddr_buf_.begin(), ddr_buf_.end(),
                               [reg_id](const Reg& rg) {
                                 if (rg.id == reg_id) return true;
                                 return false;
                               });
      UNI_LOG_CHECK(iter != ddr_buf_.end(), SIM_OUT_OF_RANGE)
          << "ddr space for reg_id: " << reg_id << "is not allocated!";

      UNI_LOG_CHECK(dat.size() <= iter->size, SIM_OUT_OF_RANGE)
          << "ddr reg id: " << reg_id << ", initial size: " << dat.size();

      if (reg_id_to_context_type_.at(e.first) == "CONST") {
        std::copy(dat.begin(), dat.end(), iter->data.begin());
      }
    }
  }
}

void DDR::Initial() {
  UNI_LOG_INFO << "DDR initial with config file...";
  // get reg ID, ddr size, init type, initfile name from file
  RegConf();
  DDRConf();
}

void DDR::RegConf(const string& regcfg_file) {  // get reg config value
  LoadCfg lc(regcfg_file);
  const string HEX_CHARS = "xXabcdefABCDEF";
  for (int i = 0;; i++) {
    string key = "reg" + to_string(i);
    if (!lc.HasKey(key)) {
      break;
    }

    string val = lc[key];
    val.erase(val.begin());
    val.erase(val.end() - 1);

    auto v = Util::Split(val);

    struct Reg reg;
    reg.id = i;
    // get size value
    if (v[1].find_first_of(HEX_CHARS) == string::npos)
      reg.size = stoul(v[1], nullptr, 10);
    else
      reg.size = stoul(v[1], nullptr, 16);
    ddr_buf_.emplace_back(reg);
  }

  // get ddr config value
  for (auto& reg : ddr_buf_) {
    string reg_name;
    if (reg.id == 8) {
      reg_name = "REG_CODE";
      reg_id_to_context_type_.emplace(reg_name, "CODE");  // all taken as data
      reg_id_to_shared_.emplace(reg_name, "0");           // local
    } else {
      reg_name = "REG_" + std::to_string(reg.id);
      reg_id_to_context_type_.emplace(reg_name, "DATA");  // all taken as data
      reg_id_to_shared_.emplace(reg_name, "0");           // local
    }
  }
}

void DDR::DDRConf(const string& ddrcfg_file) {
  LoadCfg lc(ddrcfg_file);
  const string HEX_CHARS = "xXabcdefABCDEF";

  // only support all init
  string val = lc["all"];
  val.erase(val.begin());
  val.erase(val.end() - 1);

  auto v = Util::Split(val, ", ");

  // without init
  if (!Util::Str2Bool(v[0])) return;

  auto data_type = stoul(v[1]);

  UNI_LOG_CHECK(data_type == DATA_FMT_HEX_CONT_SMALLEND_DDRADDR ||
                    data_type >= DATA_FMT_MAX,
                SIM_OUT_OF_RANGE)
      << "unsupported ddr type!";

  // file init
  if (data_type == DATA_FMT_HEX_CONT_SMALLEND_DDRADDR) {
    init_ddr(v[2]);
  } else {
    auto m = Util::Split(v[3], "-");
    for (auto e : m) {
      auto reg_id = stoul(e);
      init_ddr(reg_id);  // init reg_id
    }
  }
}
// random
void DDR::init_ddr(const int32_t& id) {
  UNI_LOG_CHECK((size_t)id < ddr_buf_.size(), SIM_OUT_OF_RANGE)
      << "error: " << id << " >= " << ddr_buf_.size();
  ddr_buf_.at(id).data.resize(ddr_buf_.at(id).size);
  Util::Random(
      ddr_buf_.at(id).data.data(), static_cast<size_t>(ddr_buf_.at(id).size),
      static_cast<char>(-16), static_cast<char>(16), 12345);  // seed = 0
}

void DDR::init_ddr(const string& init_file) {
  std::fstream f(init_file);
  Util::ChkOpen(f, init_file);

  // read ddr init file, put related value into DDR according to its address
  // ddr init file format is as follows:
  // 1-0x0000000000 : 00000000000000000000000000000000
  while (!f.eof()) {
    string line;
    getline(f, line);
    if (line.empty()) continue;

    std::stringstream ss;
    ss.clear();
    ss.str("");
    string addr;
    char split;
    string value;
    ss << line;
    ss >> addr >> split >> value;
    auto m = Util::Split(addr, "-");
    auto reg_id = stoul(m[0]);
    auto offset = stoul(m[1], nullptr, 16);

    if (ddr_buf_.at(reg_id).data.size() == 0) {
      ddr_buf_.at(reg_id).data.resize(ddr_buf_.at(reg_id).size);
    }

    // get every value and save it into ddr_
    char* p = GetAddr(reg_id, offset);
    auto hp_width = SimCfg::Instance().get_hp_width();
    UNI_LOG_CHECK(value.size() == (size_t)2 * hp_width, SIM_PARAMETER_FAILED)
        << value.size() << " != " << 2 * hp_width;
    for (int j = 0; j < hp_width; j++) {
      int pos = (2 * hp_width - 2 * j - 2);
      p[j] = static_cast<char>(stoi(string(value, pos, 2), nullptr, 16));
    }
  }
  f.close();
}
