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

#include "UniLog/UniLog.hpp"
#include "cpu_util.hpp"
#include "cpu_cfg.hpp"

using namespace std;

class GlobInit {
private:
  int argc_;
  char** argv_;
  string program_;
  string debug_path_;

public:
  GlobInit(int argc, char **argv, const string &debug_path = "./log/") :
      argc_(argc),
      argv_(argv),
      program_(argv[0]),
      debug_path_(debug_path) {

    // open_glog();
    open_unilog();

    parse_cmd();
  }

  ~GlobInit() {
    // close_glog();
  }

  static unique_ptr<GlobInit> make(int argc, char **argv, const string &debug_path = "./log/") {
    return make_unique<GlobInit>(argc, argv, debug_path);
  }

  void set_path(const string &debug_path) {
    // reopen glog again
    close_glog();
    vart::cpu::DelFileOrDir(debug_path_);
    debug_path_ = debug_path;
    open_glog();
  }

private:
  void open_glog() {
    FLAGS_log_dir = debug_path_;
    FLAGS_minloglevel = 0; // also can use export GLOG_minloglevel=0 in terminal
    google::InitGoogleLogging(program_.data());
    // NOTE: WARNING means output debug info both in file and terminal
    FLAGS_stderrthreshold = google::WARNING;
    FLAGS_colorlogtostderr = true;
    FLAGS_logbufsecs = 0;
    FLAGS_v = 0;

    // check log folder,
    // if not exists, create it
    // if exists, rm all contents in it
    vart::cpu::ChkFolder(debug_path_, true);

    LOG(INFO) << "glog open, create folder:"
        << debug_path_ << endl;
  }

  void open_unilog() {
    // check log folder,
    // if not exists, create it
    // if exists, rm all contents in it
    vart::cpu::ChkFolder(debug_path_, true);

    UniLog::Initial(argv_[0], UNI_LOG_STD_FILE, UNI_LOG_LEVEL_INFO,
              UNI_LOG_STD_LEVEL_INFO);
  }

  void close_glog() {
    // shutdown glog
    LOG(INFO) << "glog close" << endl;
    google::ShutdownGoogleLogging();
  }

  void parse_cmd() {
    if(argc_ == 1) {
      LOG(WARNING) << "no command params!" << endl;
      return;
    }

    int opt = 0;
    int option_index = 0;
    const char *optstring = "x:c:m:i:f:s:vh";
    static struct option long_options[] = {
      {"xmodel_run_mode", required_argument, NULL, 1},
      {"cpu_run_mode", required_argument, NULL, 2},
      {"model_name", required_argument, NULL, 3},
      {"input_name", required_argument, NULL, 4},
      {"random_flag", required_argument, NULL, 5},
      {"random_seed", required_argument, NULL, 6},
      {"version", required_argument, NULL, 11},
      {"help", required_argument, NULL, 12},
      {NULL, 0, NULL, 0},
    };

    while((opt = getopt_long(argc_, argv_, optstring, long_options, &option_index)) != -1) {
      switch(opt) {
      case 'x':
      case 1:
        vart::cpu::CPUCfg::Instance().set_xmodel_run_mode(optarg);
        break;
      case 'c':
      case 2:
        vart::cpu::CPUCfg::Instance().set_cpu_run_mode(stoul(optarg));
        break;
      case 'm':
      case 3:
        vart::cpu::CPUCfg::Instance().set_model_name(optarg);
        break;
      case 'i':
      case 4:
        vart::cpu::CPUCfg::Instance().set_input_name(optarg);
        break;
      case 'f':
      case 5:
        vart::cpu::CPUCfg::Instance().set_input_random_flag(vart::cpu::Str2Bool(optarg));
        break;
      case 's':
      case 6:
        vart::cpu::CPUCfg::Instance().set_input_random_seed(stoul(optarg));
        break;
      case 'v':
      case 11:
        cout << "v1.2" << endl;
        exit(1);
      case 'h':
      case 12:
      default:
        cmd_helper();
        exit(1);
      }
    }
  }

  void cmd_helper() {
    cout << argv_[0] << " usage:" << endl
      << "\t-x xmodel run mode(cpu/sim/dpu)" << endl
      << "\t-c cpu run mode(0/1/2/3)" << endl
      << "\t-m model name" << endl
      << "\t-i input name" << endl
      << "\t-f input random flag" << endl
      << "\t-s input random seed" << endl
      << "\t-v version" << endl
      << "\t-h help" << endl
      << "\t--xmodel_run_mode run mode(cpu/sim/dpu)" << endl
      << "\t--cpu_run_mode run mode(0/1/2/3)" << endl
      << "\t--model_name model name" << endl
      << "\t--input_name input name" << endl
      << "\t--random_flag input random flag" << endl
      << "\t--random_seed input random seed" << endl
      << "\t--version version" << endl
      << "\t--help help" << endl;
  }

};

