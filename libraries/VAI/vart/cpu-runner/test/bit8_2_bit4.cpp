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
#include "vart/util_4bit.hpp"

int main(int argc, char* argv[]) {
  auto gi = GlobInit::make(argc, argv, "./log/");

  CHECK(argc == 2) << ", " << argc << " != 2, please try:\n./bit8_2_bit4 fname";

  auto fname = argv[1];
  auto fsize = vart::cpu::GetFileSize(fname);

  // debug info
  cout << "Loading input file " << fname << ", size is " << fsize << endl;

  vector<int8_t> buf_bit8(fsize);
  vector<int8_t> buf_bit4(fsize / 2);

  vart::cpu::LoadBin(fname, (char*)buf_bit8.data(), fsize);

  // TODO
  // vart::cpu::dt_2_signed4bit<int8_t>(buf_bit8.data(), buf_bit4.data(),
  // fsize);

  vart::cpu::SaveBin("bit4.bin", (char*)buf_bit4.data(), buf_bit4.size(),
                     vart::cpu::SM_TRUNC);

  return 0;
}
