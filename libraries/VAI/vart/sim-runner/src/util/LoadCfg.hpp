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
#ifndef __LOADCFG_HPP__
#define __LOADCFG_HPP__

#include <string>
#include <unordered_map>

using std::string;
using std::unordered_map;

class LoadCfg {
public:
    const static string HEX_CHARS;

public:
    enum DataType {
        SCALAR,
        VECTOR,
        MAX
    };

public:
    LoadCfg(const string& fname = "config.txt");
    ~LoadCfg();

public:
    int GetKeyNum() const { return cfg_.size(); }
    string operator [] (const string& key);
    bool HasKey(const string& key);

    template<typename T>
    T GetValue(const string& key, const T& dft_val, bool use_dft_val = true);

    vector<string> GetVector(const string& key);

private:
    void read_file();
    void parse();

    void check_key(const string& key);
    void check_value(const string& value);


private:
    string fname_;

    unordered_map<string, string> cfg_;
};

#endif /* __LOADCFG_HPP__*/

