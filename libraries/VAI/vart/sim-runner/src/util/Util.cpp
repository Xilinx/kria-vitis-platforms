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
#include <dirent.h>

#include "Util.hpp"


// static public function
void Util::DelFileOrDir(const string &name)
{
    struct stat buf;
    const char* fname = name.c_str();

    if(lstat(fname, &buf) == -1) {
        UNI_LOG_FATAL(SIM_FAIL_OPEN_FAILED) << fname << " stat error: " << strerror(errno) << endl;
    }

    // handle symbol link
    if(S_ISLNK(buf.st_mode)) {
        unlink(fname);
        return;
    }

    // handle common file
    if(S_ISREG(buf.st_mode)) {
        remove(fname);
        return;
    }

    // handle not directory
    if(!S_ISDIR(buf.st_mode)) {
        remove(fname);
        return;
    }

    // handle directory
    DIR *dir = NULL;
    char next_fname[512];
    struct dirent *ptr;

    // open directory
    if((dir = opendir(fname)) == NULL) {
        UNI_LOG_FATAL(SIM_FILE_OPEN_FAILED) << fname << " opendir error: " << strerror(errno) << endl;
    }

    while((ptr = readdir(dir))) {
        if((strcmp(ptr->d_name, ".") == 0)
                || (strcmp(ptr->d_name, "..") == 0))
            continue;

        snprintf(next_fname, 512, "%s/%s", fname, ptr->d_name);
        DelFileOrDir(string(next_fname));
    }

    if(closedir(dir) == -1) {
        UNI_LOG_FATAL(SIM_FILE_OPEN_FAILED) << fname << " closedir error: " << strerror(errno) << endl;
    }

    if(rmdir(fname) == -1) {
        UNI_LOG_FATAL(SIM_FILE_OPEN_FAILED) << fname << " rmdir error: " << strerror(errno) << endl;
    }
}

void Util::ChkFile(const string &name, bool del_exists)
{
    // log folder exists, return
    if(access(name.c_str(), R_OK | W_OK | X_OK) == 0) {
        if(del_exists)
            DelFileOrDir(name);
        else
            return;
    }
}

void Util::ChkFolder(const string &name, bool del_exists)
{
    // log folder exists, return
    if(access(name.c_str(), R_OK | W_OK | X_OK) == 0) {
        if(del_exists)
            DelFileOrDir(name);
        else
            return;
    }

    // log folder doesn't exist, create it
    if(mkdir(name.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1) {
        UNI_LOG_FATAL(SIM_FILE_OPEN_FAILED) << "mkdir error: " << name << endl;
    }
}

uint64_t Util::GetFileSize(const string &fname)
{
    struct stat buf;
    if(stat(fname.c_str(), &buf) == -1) {
        UNI_LOG_FATAL(SIM_FILE_OPEN_FAILED) << fname << " stat error: " << strerror(errno) << endl;
    }

    return buf.st_size;
}

uint64_t Util::GetFileLine(const string &fname)
{
    std::fstream f(fname);

    ChkOpen(f, fname);

    uint64_t cnt = 0;
    while(!f.eof()) {
        string line;
        getline(f, line);
        if(line.empty())
            continue;
        cnt++;
    }

    return cnt;
}

uint64_t Util::LoadBin(const string &load_name, char *data, uint64_t size)
{
    auto fsize = GetFileSize(load_name);
    UNI_LOG_CHECK(fsize <= size, SIM_OUT_OF_RANGE) << "Buffer size (" << size
        << ") is less than file size(" << fsize << ")!" << endl;

    // get file length, allocate data buf
    std::fstream f(load_name, std::ios_base::in | std::ios_base::binary);
    ChkOpen(f, load_name);

    // read file contents
    f.read(data, size);
    f.close();

    return fsize;
}

uint64_t Util::LoadHexContSmallEnd(const string &load_name, char *data, uint64_t size, int line_size)
{
    string line;
    std::fstream f(load_name);
    vector<string> line_vec;

    ChkOpen(f, load_name);

    // read ddr init file, store it into vector line_vec
    // ddr init file format is as follows:
    // 0x0000000000000000 : 00000000000000000000000000000000
    while(!f.eof()) {
        getline(f, line);
        if(line.size()) {
            line_vec.push_back(line);
        }
    }
    f.close();
    UNI_LOG_INFO << "read ddr data lines: " << line_vec.size();

    // every line has line_size bytes
    UNI_LOG_CHECK(size >= line_vec.size() * line_size, SIM_PARAMETER_FAILED);

    for(auto i=0U; i<line_vec.size(); i++) {
        // get every value and save it into ddr_
        int base_addr = i*line_size;
        UNI_LOG_CHECK(line_vec[i].size() == (size_t)2*line_size, SIM_PARAMETER_FAILED);
        for(int j=0; j<line_size; j++) {
            int pos = 2*line_size - 2*j - 2;
            int i_val = stoi(string(line_vec[i], pos, 2), nullptr, 16);
            data[base_addr+j] = static_cast<char>(i_val);
        }
    }

    return (line_vec.size() * line_size);
}

uint64_t Util::LoadHexContBigEnd(const string &load_name, char *data, uint64_t size, int line_size)
{
    string line;
    std::fstream f(load_name);
    vector<string> line_vec;

    ChkOpen(f, load_name);

    // read ddr init file, store it into vector line_vec
    // ddr init file format is as follows:
    // 0x0000000000000000 : 00000000000000000000000000000000
    while(!f.eof()) {
        getline(f, line);
        if(line.size()) {
            line_vec.push_back(line);
        }
    }
    f.close();
    UNI_LOG_INFO << "read ddr data lines: " << line_vec.size();

    // every line has line_size bytes
    UNI_LOG_CHECK(size >= line_vec.size() * line_size, SIM_PARAMETER_FAILED);

    for(auto i=0U; i<line_vec.size(); i++) {
        // get every value and save it into ddr_
        int base_addr = i*line_size;
        for(auto j=0; j<line_size; j++) {
            int i_val = stoi(string(line_vec[i], 2*j, 2), nullptr, 16);
            data[base_addr+j] = static_cast<char>(i_val);
        }
    }

    return (line_vec.size() * line_size);
}

uint64_t Util::LoadHexContSmallEndBankAddr(const string &load_name, char *data, uint64_t size, int line_size)
{
    string line;
    std::fstream f(load_name);
    vector<string> line_vec;

    ChkOpen(f, load_name);

    // read ddr init file, store it into vector line_vec
    // ddr init file format is as follows:
    // 0x0000000000000000 : 00000000000000000000000000000000
    while(!f.eof()) {
        getline(f, line);
        if(line.size()) {
            line_vec.push_back(line);
        }
    }
    f.close();
    // NOTE: it will crash in server 40 when using the following code,
    // not know the reason.
    
    UNI_LOG_INFO << "read ddr data lines: " << line_vec.size();

    // every line has line_size bytes
    UNI_LOG_CHECK(size >= line_vec.size() * line_size, SIM_PARAMETER_FAILED);

    for(auto i=0U; i<line_vec.size(); i++) {
        // split ori data string into :
        // addr            split   value
        // 47-2047 : 00000000000000000000000000000000
        std::stringstream ss;
        ss.clear();
        ss.str("");
        string addr;
        char split;
        string value;
        ss << line_vec[i];
        ss >> addr >> split >> value;

        // get every value and save it into ddr_
        int base_addr = i*line_size;
        UNI_LOG_CHECK(value.size() == (size_t)2*line_size, SIM_PARAMETER_FAILED);
        for(int j=0; j<line_size; j++) {
            int pos = 2*line_size - 2*j - 2;
            int i_val = stoi(string(value, pos, 2), nullptr, 16);
            data[base_addr+j] = static_cast<char>(i_val);
        }
    }

    return (line_vec.size() * line_size);
}

uint64_t Util::LoadHexContBigEndBankAddr(const string &load_name, char *data, uint64_t size, int line_size)
{
    string line;
    std::fstream f(load_name);
    vector<string> line_vec;

    ChkOpen(f, load_name);

    // read ddr init file, store it into vector line_vec
    // ddr init file format is as follows:
    // 0x0000000000000000 : 00000000000000000000000000000000
    while(!f.eof()) {
        getline(f, line);
        if(line.size()) {
            line_vec.push_back(line);
        }
    }
    f.close();
    // NOTE: it will crash in server 40 when using the following code,
    // not know the reason.
    
    UNI_LOG_INFO << "read ddr data lines: " << line_vec.size();

    // every line has line_size bytes
    UNI_LOG_CHECK(size >= line_vec.size() * line_size, SIM_PARAMETER_FAILED);

    for(auto i=0U; i<line_vec.size(); i++) {
        // split ori data string into :
        // addr            split   value
        // 47-2047 : 00000000000000000000000000000000
        std::stringstream ss;
        ss.clear();
        ss.str("");
        string addr;
        char split;
        string value;
        ss << line_vec[i];
        ss >> addr >> split >> value;

        // get every value and save it into ddr_
        int base_addr = i*line_size;
        UNI_LOG_CHECK(value.size() == (size_t)2*line_size, SIM_PARAMETER_FAILED);
        for(int j=0; j<line_size; j++) {
            int i_val = stoi(string(value, 2*j, 2), nullptr, 16);
            data[base_addr+j] = static_cast<char>(i_val);
        }
    }

    return (line_vec.size() * line_size);
}

uint64_t Util::LoadHexContSmallEndDDRAddr(const string &load_name, char *data, uint64_t size, int line_size)
{
    string line;
    std::fstream f(load_name);
    vector<string> line_vec;

    ChkOpen(f, load_name);

    // read ddr init file, store it into vector line_vec
    // ddr init file format is as follows:
    // 0x0000000000000000 : 00000000000000000000000000000000
    while(!f.eof()) {
        getline(f, line);
        if(line.size()) {
            line_vec.push_back(line);
        }
    }
    f.close();

    UNI_LOG_INFO << "read ddr data lines: " << line_vec.size();

    // every line has line_size bytes
    auto nlines = (size / line_size) > line_vec.size()
        ? line_vec.size()
        : size/line_size;
    for(auto i=0U; i<nlines; i++) {
        // split ori data string into :
        // addr            split   value
        // 0x0000000000000000 : 00000000000000000000000000000000
        std::stringstream ss;
        ss.clear();
        ss.str("");
        string addr;
        char split;
        string value;
        ss << line_vec[i];
        ss >> addr >> split >> value;

        //LOG_EVERY_N(INFO, 1024) << addr << split << value;

        // get every value and save it into ddr_
        int base_addr = i*line_size;
        UNI_LOG_CHECK(value.size() == (size_t)2*line_size, SIM_PARAMETER_FAILED);
        for(int j=0; j<line_size; j++) {
            int pos = 2*line_size - 2*j - 2;
            int i_val = stoi(string(value, pos, 2), nullptr, 16);
            data[base_addr+j] = static_cast<char>(i_val);
        }
    }

    return (line_vec.size() * line_size);
}

uint64_t Util::LoadHexContBigEndDDRAddr(const string &load_name, char *data, uint64_t size, int line_size)
{
    string line;
    std::fstream f(load_name);
    vector<string> line_vec;

    ChkOpen(f, load_name);

    // read ddr init file, store it into vector line_vec
    // ddr init file format is as follows:
    // 0x0000000000000000 : 00000000000000000000000000000000
    while(!f.eof()) {
        getline(f, line);
        if(line.size()) {
            line_vec.push_back(line);
        }
    }
    f.close();

    UNI_LOG_INFO << "read ddr data lines: " << line_vec.size();

    // every line has line_size bytes
    auto nlines = (size / line_size) > line_vec.size()
        ? line_vec.size()
        : size/line_size;
    for(auto i=0U; i<nlines; i++) {
        // split ori data string into :
        // addr            split   value
        // 0x0000000000000000 : 00000000000000000000000000000000
        std::stringstream ss;
        ss.clear();
        ss.str("");
        string addr;
        char split;
        string value;
        ss << line_vec[i];
        ss >> addr >> split >> value;

        // get every value and save it into ddr_
        int base_addr = i*line_size;
        UNI_LOG_CHECK(value.size() == (size_t)2*line_size, SIM_PARAMETER_FAILED);
        for(int j=0; j<line_size; j++) {
            int i_val = stoi(string(value, 2*j, 2), nullptr, 16);
            data[base_addr+j] = static_cast<char>(i_val);
        }
    }

    return (line_vec.size() * line_size);
}

void Util::SaveBin(const string &save_name, const char* data, uint64_t size, int mode)
{
    // write into file
    std::ofstream f;

    if(mode == SM_TRUNC) {
        f.open(save_name, std::ios::trunc |
            std::ios::binary | std::ios::out);
    } else {
        f.open(save_name, std::ios::app |
            std::ios::binary | std::ios::out);
    }

    ChkOpen(f, save_name);

    f.write(data, size);
    f.close();
}

void Util::SaveHexContSmallEnd(const string &save_name, const char* data, uint64_t size, int line_size, int mode)
{
    // write into file
    std::ofstream f;

    if(mode == SM_TRUNC) {
        f.open(save_name, std::ios::trunc);
    } else {
        f.open(save_name, std::ios::app);
    }

    ChkOpen(f, save_name);

    // line related vars
    int line_num = size/line_size + 1;
    int line_left = size%line_size;

    // write line_num lines
    for(int i=0; i<line_num; i++) {
        // handle last line, it's special
        if(i==line_num-1 && line_left==0)
            break;

        // save every line_size bytes into a value string
        for(int j=line_size-1; j>=0; j--) {
            char tmp = 0;
            // handle last line, it's special
            if(i==line_num-1 && j>=line_left) {
                tmp = 0;
            } else {
                tmp = data[i*line_size + j];
            }

            uint8_t tmp_u8 = static_cast<uint8_t>(tmp);
            uint32_t tmp_u32 = static_cast<uint32_t>(tmp_u8);
            f << hex << setw(2) << setfill('0') << tmp_u32;
        }
        f << endl;
    }

    f.close();
}

void Util::SaveHexContBigEnd(const string &save_name, const char* data, uint64_t size, int line_size, int mode)
{
    // write into file
    std::ofstream f;

    if(mode == SM_TRUNC) {
        f.open(save_name, std::ios::trunc);
    } else {
        f.open(save_name, std::ios::app);
    }

    ChkOpen(f, save_name);

    // line related vars
    int line_num = size/line_size + 1;
    int line_left = size%line_size;

    // write line_num lines
    for(int i=0; i<line_num; i++) {
        // handle last line, it's special
        if(i==line_num-1 && line_left==0)
            break;

        // write line_size data every line
        for(int j=0; j<line_size; j++) {
            // handle last line, it's special
            if(i==line_num-1 && j>=line_left)
                break;

            // save data using related format
            uint8_t tmp_u8 = static_cast<uint8_t>(data[i*line_size + j]);
            uint32_t tmp_u32 = static_cast<uint32_t>(tmp_u8);
            f << hex << setw(2) << setfill('0') << tmp_u32;
        }
        f << endl;
    }

    f.close();
}

void Util::SaveHexContSmallEndBankAddr(const string &save_name, const char* data, uint64_t size,
        int line_size, int bank_height, int bank_id_base, int bank_addr_base, int mode)
{
    // write into file
    std::ofstream f;

    if(mode == SM_TRUNC) {
        f.open(save_name, std::ios::trunc);
    } else {
        f.open(save_name, std::ios::app);
    }

    ChkOpen(f, save_name);

    // line related vars
    int line_num = size/line_size + 1;
    int line_left = size%line_size;

    // write line_num lines
    for(int i=0; i<line_num; i++) {
        // handle last line, it's special
        if(i==line_num-1 && line_left==0)
            break;

        auto bank_id = bank_id_base + (bank_addr_base + i)/bank_height;
        auto bank_addr = (bank_addr_base + i)%bank_height;

        f << dec << setw(2) << setfill('0') << bank_id;
        f << "-";
        f << dec << setw(4) << setfill('0') << bank_addr;
        f << " : ";

        // write line_size data every line
        for(int j=line_size-1; j>=0; j--) {
            char tmp = 0;
            // handle last line, it's special
            if(i==line_num-1 && j>=line_left) {
                tmp = 0;
            } else {
                tmp = data[i*line_size + j];
            }

            uint8_t tmp_u8 = static_cast<uint8_t>(tmp);
            uint32_t tmp_u32 = static_cast<uint32_t>(tmp_u8);
            f << hex << setw(2) << setfill('0') << tmp_u32;
        }
        f << endl;
    }

    f.close();
}

void Util::SaveHexContBigEndBankAddr(const string &save_name, const char* data, uint64_t size,
        int line_size, int bank_height, int bank_id_base, int bank_addr_base, int mode)
{
    // write into file
    std::ofstream f;

    if(mode == SM_TRUNC) {
        f.open(save_name, std::ios::trunc);
    } else {
        f.open(save_name, std::ios::app);
    }

    ChkOpen(f, save_name);

    // line related vars
    int line_num = size/line_size + 1;
    int line_left = size%line_size;

    // write line_num lines
    for(int i=0; i<line_num; i++) {
        // handle last line, it's special
        if(i==line_num-1 && line_left==0)
            break;

        auto bank_id = bank_id_base + (bank_addr_base + i)/bank_height;
        auto bank_addr = (bank_addr_base + i)%bank_height;

        f << dec << setw(2) << setfill('0') << bank_id;
        f << "-";
        f << dec << setw(4) << setfill('0') << bank_addr;
        f << " : ";

        // write line_size data every line
        for(int j=0; j<line_size; j++) {
            // handle last line, it's special
            if(i==line_num-1 && j>=line_left)
                break;

            // save data using related format
            uint8_t tmp_u8 = static_cast<uint8_t>(data[i*line_size + j]);
            uint32_t tmp_u32 = static_cast<uint32_t>(tmp_u8);
            f << hex << setw(2) << setfill('0') << tmp_u32;
        }
        f << endl;
    }

    f.close();
}

void Util::SaveHexContSmallEndDDRAddr(const string &save_name, const char* data, uint64_t size,
        int line_size, uint64_t addr_offset, int reg_id, int mode)
{
    // open file
    std::ofstream f;

    if(mode == SM_TRUNC) {
        f.open(save_name, std::ios::trunc);
    } else {
        f.open(save_name, std::ios::app);
    }

    ChkOpen(f, save_name);

    // line related vars
    uint64_t addr = addr_offset;
    int line_num = size/line_size + 1;
    int line_left = size%line_size;

    // write line_num lines
    for(int i=0; i<line_num; i++) {
        // handle last line, it's special
        if(i==line_num-1 && line_left==0)
            break;

        // save addr value into addr string
        f << reg_id << "-0x" << hex << setw(16) << setfill('0') << addr << " : ";

        // save every line_size bytes into a value string
        for(int j=line_size-1; j>=0; j--) {
            char tmp = 0;
            // handle last line, it's special
            if(i==line_num-1 && j>=line_left) {
                tmp = 0;
            } else {
                tmp = data[i*line_size + j];
            }

            uint8_t tmp_u8 = static_cast<uint8_t>(tmp);
            uint32_t tmp_u32 = static_cast<uint32_t>(tmp_u8);
            f << hex << setw(2) << setfill('0') << tmp_u32;
        }

        // get the whole addr : value string
        f << endl;

        // update addr
        addr += line_size;
    }

    f.close();
}

void Util::SaveHexContBigEndDDRAddr(const string &save_name, const char* data, uint64_t size,
        int line_size, uint64_t addr_offset, int reg_id, int mode)
{
    // open file
    std::ofstream f;

    if(mode == SM_TRUNC) {
        f.open(save_name, std::ios::trunc);
    } else {
        f.open(save_name, std::ios::app);
    }

    ChkOpen(f, save_name);

    // line related vars
    uint64_t addr = addr_offset;
    int line_num = size/line_size + 1;
    int line_left = size%line_size;

    // write line_num lines
    for(int i=0; i<line_num; i++) {
        // handle last line, it's special
        if(i==line_num-1 && line_left==0)
            break;

        // save addr value into addr string
        f << reg_id << "-0x" << hex << setw(16) << setfill('0') << addr << " : ";

        // save every line_size bytes into a value string
        for(int j=0; j<line_size; j++) {
            // handle last line, it's special
            if(i==line_num-1 && j>=line_left)
                break;

            // save data using related format
            uint8_t tmp_u8 = static_cast<uint8_t>(data[i*line_size + j]);
            uint32_t tmp_u32 = static_cast<uint32_t>(tmp_u8);
            f << hex << setw(2) << setfill('0') << tmp_u32;
        }

        // get the whole addr : value string
        f << endl;

        // update addr
        addr += line_size;
    }

    f.close();
}

bool Util::Str2Bool(const string& str)
{
    string tmp;

    std::transform(str.begin(), str.end(), std::back_inserter(tmp), ::tolower);
    if(tmp == "true")
        return true;
    return false;
}

string Util::Bool2Str(bool flag)
{
    if(flag)
        return "true";
    return "false";
}

int64_t Util::Str2S64(const string& str) {
    const string HEX_CHARS = "xXabcdefABCDEF";

    int64_t val = 0;

    if(str.find_first_of(HEX_CHARS) == string::npos)
        val = stoll(str, nullptr, 10);
    else
        val = stoll(str, nullptr, 16);

    return val;
}

uint64_t Util::Str2U64(const string& str) {
    const string HEX_CHARS = "xXabcdefABCDEF";

    int64_t val = 0;

    if(str.find_first_of(HEX_CHARS) == string::npos)
        val = stoull(str, nullptr, 10);
    else
        val = stoull(str, nullptr, 16);

    return val;
}

uint32_t Util::FillMaskU32(int start, int num) {
    UNI_LOG_CHECK(start>=0 && start<32, SIM_OUT_OF_RANGE);
    UNI_LOG_CHECK(num>=0 && num<=32, SIM_OUT_OF_RANGE);

    uint32_t mask = 0x0U;

    for(auto pos=start; pos<start+num; pos++) {
        auto x = pos % 32;
        mask |= (0x1U << x);
    }

    return mask;
}

uint64_t Util::FillMaskU64(int start, int num) {
    UNI_LOG_CHECK(start>=0 && start<64, SIM_OUT_OF_RANGE);
    UNI_LOG_CHECK(num>=0 && num<=64, SIM_OUT_OF_RANGE);

    uint64_t mask = 0x0U;

    for(auto pos=start; pos<start+num; pos++) {
        auto x = pos % 64;
        mask |= (0x1U << x);
    }

    return mask;
}

uint64_t Util::AlignByN(uint64_t data, uint64_t align) {
    UNI_LOG_CHECK(align > 0, SIM_OUT_OF_RANGE);
    uint64_t rlt = ((data + align - 1) / align) * align;
    return rlt;
}

string Util::GetFileNameSuffix(int fmt) {
    string suffix;
    if(fmt == DATA_FMT_BIN)
        suffix = ".bin";
    else if(fmt == DATA_FMT_DEC)
        suffix = ".dec";
    else if(fmt == DATA_FMT_HEX_CONT_SMALLEND)
        suffix = ".txt";
    else if(fmt == DATA_FMT_HEX_CONT_BIGEND)
        suffix = ".txt";
    else if(fmt == DATA_FMT_HEX_CONT_SMALLEND_BANKADDR)
        suffix = ".txt";
    else if(fmt == DATA_FMT_HEX_CONT_BIGEND_BANKADDR)
        suffix = ".txt";
    else if(fmt == DATA_FMT_HEX_CONT_SMALLEND_DDRADDR)
        suffix = ".txt";
    else if(fmt == DATA_FMT_HEX_CONT_BIGEND_DDRADDR)
        suffix = ".txt";
    else {
        UNI_LOG_FATAL(SIM_OUT_OF_RANGE) << "Not support fmt " << fmt << endl;
    }

    return suffix;
}

string Util::Trim(const string &str)
{
    auto tmp = str;

    // erase whitespace before the string
    string::iterator it1;
    for(it1=tmp.begin(); it1<tmp.end(); it1++) {
        if(!isspace(*it1))
            break;
    }
    tmp.erase(0, it1 - tmp.begin());

    // erase whitespace after the string
    string::reverse_iterator it2;
    for(it2=tmp.rbegin(); it2<tmp.rend(); it2++) {
        if(!isspace(*it2))
            break;
    }
    tmp.erase(tmp.rend() - it2, it2 - tmp.rbegin());

    return tmp;
}

// split all words in the string
vector<string> Util::Split(const string &str, const string &delim, bool trim_flag)
{
    vector<string> result;

    string::size_type start = 0;
    string::size_type pos = 0;
    do {
        pos = str.find_first_of(delim, start);

        int len = (pos == string::npos) ? (str.size() - start) : (pos - start);
        if(len) {
            string substr = trim_flag
                ? Trim(str.substr(start, len))
                : str.substr(start, len);
            if(!substr.empty()) {
                result.emplace_back(substr);
            }
        }
        start = pos + 1;
    } while(pos != string::npos);

    return result;
}

// split first word in the string
string Util::SplitFirst(const string &str, const string &delim, bool trim_flag)
{
    vector<string> v = Split(str, delim, trim_flag);

    if(v.empty())
        return string("");
    return v[0];
}

