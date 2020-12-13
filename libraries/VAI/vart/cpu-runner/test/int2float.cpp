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
#include "cpu_util.hpp"
#include "glob_init.hpp"

int main(int argc, char* argv[]) {
  auto gi = GlobInit::make(argc, argv, "./log/");

  CHECK(argc == 4) << ", " << argc
    << " != 4, please try:\n./int2float fname bit_width fix_point";

  auto bit_width = stoi(argv[2]);
  auto fix_point = stoi(argv[3]);
  auto step = pow(2, -fix_point);
  float lower_bound_base = -pow(2, bit_width - 1);
  float upper_bound_base = pow(2, bit_width - 1) - 1;
  float lower_bound = -pow(2, bit_width - 1) * step;
  float upper_bound = pow(2, bit_width - 1) * step - step;

  auto fname = argv[1];
  auto fsize = vart::cpu::GetFileSize(fname);

  // debug info
  cout << "Loading input file " << fname << ", size is " << fsize << endl;
  cout << "bit_width = " << bit_width << endl;
  cout << "fix_point = " << fix_point << endl;
  cout << "lower_bound_base = " << lower_bound_base << endl;
  cout << "upper_bound_base = " << upper_bound_base << endl;
  cout << "lower_bound = " << lower_bound << endl;
  cout << "upper_bound = " << upper_bound << endl;

  vector<char> fix_buf(fsize);
  vector<float> float_buf(fsize);
  vart::cpu::LoadBin(fname, fix_buf.data(), fsize);

  for (auto i = 0U; i < fsize; i++) {
    auto tmp = std::max(float(fix_buf[i] * step), lower_bound);
    float_buf[i] = std::min(tmp, upper_bound);
  }

  vart::cpu::SaveDec("input_float.txt", float_buf.data(), float_buf.size(), 1,
                     vart::cpu::SM_TRUNC);
  vart::cpu::SaveBin("input_float.bin", (char*)float_buf.data(),
                     float_buf.size() * sizeof(float), vart::cpu::SM_TRUNC);

  return 0;
}
