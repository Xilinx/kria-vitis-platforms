// Copyright 2019 Xilinx Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// Caffe
int main(int argc, char *argv[]) {
  cxxopts::Options options(argv[0], "Compiler for DPU");
  options.add_options()                                    //
      ("t,target", "target name",                          //
       cxxopts::value<string>()->default_value("dpuv2"))   //
      ("o,output-dir", "output dir",                       //
       cxxopts::value<string>()->default_value(            //
           PWD "./output"))                                //
      ("prototxt", "prototxt file",                        //
       cxxopts::value<string>()->default_value(""))        //
      ("caffemodel", "caffemodel file",                    //
       cxxopts::value<string>()->default_value(""))        //
      ("mode", "release/debug",                            //
       cxxopts::value<string>()->default_value("release")) //
      ("options", "more options",                          //
       cxxopts::value<string>()->default_value(""))        //
      ("h,help", "show help");
  auto results = options.parse(argc, argv);
  if (results["help"].count()) {
    cout << options.help() << endl;
    exit(0);
  }
  auto target = results["target"].as<std::string>();
  if (target == "xdnnv3") {
    //
  } else if (target == "dpuv2") {
    //
  }

  return 0;
}

// TensorFlow
int main(int argc, char *argv[]) {
  cxxopts::Options options(argv[0], "Compiler for DPU");
  options.add_options()                                    //
      ("t,target", "target name",                          //
       cxxopts::value<string>()->default_value("dpuv2"))   //
      ("o,output-dir", "output dir",                       //
       cxxopts::value<string>()->default_value(            //
           PWD "./output"))                                //
      ("model", "model file, pb",                          //
       cxxopts::value<string>()->default_value(""))        //
      ("mode", "release/debug",                            //
       cxxopts::value<string>()->default_value("release")) //
      ("options", "more options",                          //
       cxxopts::value<string>()->default_value(""))        //
      ("h,help", "show help");
  auto results = options.parse(argc, argv);
  if (results["help"].count()) {
    cout << options.help() << endl;
    exit(0);
  }
  auto target = results["target"].as<std::string>();
  if (target == "xdnnv3") {
    //
  } else if (target == "dpuv2") {
    //
  }

  return 0;
}

// XIR
int main(int argc, char *argv[]) {
  cxxopts::Options options(argv[0], "Compiler for DPU");
  options.add_options()                                    //
      ("t,target", "target name",                          //
       cxxopts::value<string>()->default_value("dpuv2"))   //
      ("o,output-dir", "output dir",                       //
       cxxopts::value<string>()->default_value(            //
           PWD "./output"))                                //
      ("model", "model file, xmodel",                      //
       cxxopts::value<string>()->default_value(""))        //
      ("mode", "release/debug",                            //
       cxxopts::value<string>()->default_value("release")) //
      ("options", "more options",                          //
       cxxopts::value<string>()->default_value(""))        //
      ("h,help", "show help");
  auto results = options.parse(argc, argv);
  if (results["help"].count()) {
    cout << options.help() << endl;
    exit(0);
  }
  auto target = results["target"].as<std::string>();
  if (target == "xdnnv3") {
    //
  } else if (target == "dpuv2") {
    //
  }

  return 0;
}
