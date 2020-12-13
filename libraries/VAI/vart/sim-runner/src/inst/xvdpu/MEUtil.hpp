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

#include "Util.hpp"
#include "MECfg.hpp"

//PKT_TYPE define pkt type, used in save out, 
//PKT_TYPE_CTRL save out to txt as uint32_t
//PKT_TYPE_DATA save out to txt as int32_t
enum PKT_TYPE{
  PKT_TYPE_MIN = 0,
  PKT_TYPE_CTRL = PKT_TYPE_MIN,
  PKT_TYPE_DATA = 1,
};
inline uint32_t addOddParityBits(uint32_t in){
    uint32_t odd = 1;
    for(int i=0; i<31; i++){
       odd ^= (in >> i);
    }
    return (in | (odd << 31));
}

//there is no hardened functionality relying on the pkt_type filed.
inline uint32_t genStreamPktHeader(unsigned int streamid, unsigned int pkt_type, unsigned int src_row, unsigned int src_col){
  assert((((streamid>>5) & 0x11111111) == 0) && "streamid 5 bits valid");
  assert((((pkt_type>>3) & 0x11111111) == 0) && "pkt_type 3 bits valid");
  assert((((src_row>>5) & 0x11111111) == 0) && "src_row 5 bits valid");
  assert((((src_col>>7) & 0x11111111) == 0) && "src_col 7 bits valid");
  uint32_t header = 0x00000000;
  header |= streamid;
  header |= (pkt_type << 12);
  header |= (src_row << 16);
  header |= (src_col << 21);
  return addOddParityBits(header); 
}

/*
 *data_beat:
 *  00->1, 01->2, 10->3, 11->4 
 *option: 
 *  00: write without return packet, 
 *  01: read with return packet, 
 *  10: write with return packet 
 *  11: reserved
 */
inline uint32_t genCtrlPktHeader(unsigned int address, unsigned int data_beat, unsigned int option, unsigned int id4return)
{
  assert((((address>>18) & 0x11111111) == 0) && "address 18 bits valid");
  assert((((data_beat>>3) & 0x11111111) == 0) && "data_beat 2 bits valid");
  assert((((option>>2) & 0x11111111) == 0) && "option 2 bits valid");
  assert((((id4return>>5) & 0x11111111) == 0) && "id4return 5 bits valid");
  uint32_t header = 0x00000000;
  header |= address;
  header |= ((data_beat-1) << 18);
  header |= (option << 20);
  header |= (id4return << 24);
  return addOddParityBits(header); 
}

inline uint32_t BDlengthReg(uint32_t length, bool enable_pkt)
{
  assert((((length>>13) & 0x11111111) == 0) && "length only 13 bits valid");
  
  uint32_t ret = 0x00000000;
  ret |= (length-1);
  if (enable_pkt) ret |= (1<<27); // enable packets
  //ret |= (1<<30); // enable AB_mode
  ret |= (1<<31);
  return ret; 
}
inline uint32_t BDbaseAddressReg(uint32_t baseAddress, unsigned int lock, bool is_input=true)
{
  assert((((baseAddress>>13) & 0x11111111) == 0) && "baseaddress 13 bits valid");
  assert((((lock>>4) & 0x11111111) == 0) && "lock bits valid");
  
  uint32_t ret = 0x00000000;
  ret |= baseAddress;
  ret |= (1 << 16);
  ret |= ((!is_input) << 17);
  ret |= (1 << 18);
  ret |= (1 << 19);
  ret |= ((is_input) << 20);
  ret |= (1 << 21);
  ret |= (lock << 22);
  return ret; 
}


string uint2hex(uint32_t* tmp, int len, int WIDTH=4){
   assert(WIDTH > 0);
   stringstream ss;
   ss.clear();
   ss.str("");
    int row = len/WIDTH;
    row += (len%WIDTH == 0) ? 0 : 1;
    for(int i=0; i<row-1; i++){
        ss<<"TDATA : ";
        int row_base = i*WIDTH;
        for(int j=0; j<WIDTH;j++){
          ss<<hex<<setw(8)<<setfill('0')<<tmp[row_base+3-j];
        }
        ss<<"\n";
    }
    ss<<"TLAST : ";
    int mod = len%WIDTH;
    mod = (mod == 0) ? WIDTH : mod;
    for(int i=0; i<WIDTH-mod; i++){
        ss<<hex<<setw(8)<<setfill('0')<<0;
    }
    for(int i=0; i<mod; i++){
        ss<<hex<<setw(8)<<setfill('0')<<tmp[len-i-1];
    }
   ss<<"\n";
   return ss.str();
}

string uint2str(uint32_t* pkts, int len, PKT_TYPE type, int WIDTH=4){
  assert(type == PKT_TYPE_CTRL || type == PKT_TYPE_DATA);
  assert(WIDTH > 0);
   stringstream ss;
   ss.clear();
   ss.str("");
  int row = len/WIDTH;
  row += (len%WIDTH == 0) ? 0 : 1;
  for(int i=0; i<row; i++){
    if(i == row-1) ss<<"TLAST"<<endl;
    for(int j=0; j<WIDTH; j++){
        int idx = i*WIDTH+j;
        if(idx < len){
            /*
            if(type == PKT_TYPE_CTRL) ss<<pkts[idx]<<" ";
            else if(type == PKT_TYPE_DATA) ss<<(int32_t)pkts[idx]<<" ";
            */
            ss<<(int32_t)pkts[idx]<<" ";
        }
    }
    ss<<endl;
  }
  return ss.str();
}



class ChangeBDLengthPkt{
public:
    explicit ChangeBDLengthPkt(unsigned int ID, unsigned src_row, unsigned src_col,
                              unsigned BD, unsigned int length, bool AorB, bool enable_pkt):
                              ID_(ID), src_row_(src_row), src_col_(src_col), BD_(BD),
                              length_(length), AorB_(AorB), enable_pkt_(enable_pkt)
                              { genPackets(); }
    string to_string(){
        return uint2str(pkts_.data(), pkts_.size(), PKT_TYPE_CTRL);
    }
    string to_hex(){
        return uint2hex(pkts_.data(), pkts_.size());
    }
private:
    void genPackets(){
        pkts_.push_back( genStreamPktHeader(ID_, 0, src_row_, src_col_));
        unsigned int BDRegAddr = 0x1D014 + (0x20*BD_);
        pkts_.push_back( genCtrlPktHeader(BDRegAddr, 2, 0, 0));  
        pkts_.push_back( 0x00000000 | (AorB_ << 16));
        pkts_.push_back( BDlengthReg(length_, enable_pkt_)); 
    }

private:
    unsigned int ID_;
    unsigned src_row_;
    unsigned src_col_;
    unsigned BD_;
    unsigned int length_;
    bool AorB_;
    bool enable_pkt_;
    vector<uint32_t> pkts_;
};

class ChangeBDBaseAddrPkt{
public:
    explicit ChangeBDBaseAddrPkt(unsigned int ID, unsigned src_row, unsigned src_col,
                              unsigned BD, unsigned int baseAddr, unsigned int lock, bool is_input=true):
                              ID_(ID), src_row_(src_row), src_col_(src_col), BD_(BD),
                              baseAddr_(baseAddr), lock_(lock), is_input_(is_input)
                              { genPackets(); }
    string to_string(){
        return uint2str(pkts_.data(), pkts_.size(), PKT_TYPE_CTRL);
    }
    string to_hex(){
        return uint2hex(pkts_.data(), pkts_.size());
    }
private:
    void genPackets(){
        pkts_.push_back( genStreamPktHeader(ID_, 0, src_row_, src_col_));
        unsigned int BDRegAddr = 0x1D000 + (0x20*BD_);
        pkts_.push_back( genCtrlPktHeader(BDRegAddr, 2, 0, 0));  // only 1 word needed, another for align
        pkts_.push_back( BDbaseAddressReg(baseAddr_, lock_, is_input_));
        pkts_.push_back( 0u); //for align
    }

private:
    unsigned int ID_;
    unsigned src_row_;
    unsigned src_col_;
    unsigned BD_;
    unsigned int baseAddr_;
    unsigned int lock_;
    bool is_input_;
    vector<uint32_t> pkts_;
};

class ChangeBDStartQueuePkt{
public:
    explicit ChangeBDStartQueuePkt(unsigned int ID, unsigned src_row, unsigned src_col,
                              unsigned BD, unsigned int channel, bool isS2MM):
                              ID_(ID), src_row_(src_row), src_col_(src_col), BD_(BD),
                              channel_(channel), S2MMorMM2S_(isS2MM)
                              { genPackets(); }
    string to_string(){
        return uint2str(pkts_.data(), pkts_.size(), PKT_TYPE_CTRL);
    }
    string to_hex(){
        return uint2hex(pkts_.data(), pkts_.size());
    }
private:
    void genPackets(){
        assert(channel_ == 0 || channel_ == 1);
        pkts_.push_back( genStreamPktHeader(ID_, 0, src_row_, src_col_));
        unsigned int DMACtrlAddr = 0x1DE00 + 0x8 * channel_ + 0x10*S2MMorMM2S_;
        pkts_.push_back( genCtrlPktHeader(DMACtrlAddr, 2, 0, 0));  
        //enable
        pkts_.push_back( 0x00000001);
        //start queue
        pkts_.push_back( 0x00000000 | BD_); 
    }

private:
    unsigned int ID_;
    unsigned src_row_;
    unsigned src_col_;
    unsigned BD_;
    unsigned int channel_;
    bool S2MMorMM2S_;
    vector<uint32_t> pkts_;
};

class SetParamPkt{
public:
    explicit SetParamPkt(unsigned int ID, unsigned src_row, unsigned src_col,
                         unsigned int addr, const int32_t* data, unsigned int size):
                              ID_(ID), src_row_(src_row), src_col_(src_col),
                              addr_(addr){
                                assert(size>0 && size<5);
                                genPackets(data, size); }
    string to_string(){
        return uint2str(pkts_.data(), pkts_.size(), PKT_TYPE_CTRL);
    }
    string to_hex(){
        return uint2hex(pkts_.data(), pkts_.size());
    }
private:
    void genPackets(const int32_t* data, int size){
        pkts_.push_back( genStreamPktHeader(ID_, 0, src_row_, src_col_));
        pkts_.push_back( genCtrlPktHeader(addr_, size, 0, 0)); 
        uint32_t * uptr = const_cast<uint32_t*>(reinterpret_cast<const uint32_t*>(data));
        for(auto i=0; i<size; i++){
            pkts_.push_back(uptr[i]);
        }
    }

private:
    unsigned int ID_;
    unsigned src_row_;
    unsigned src_col_;
    unsigned int addr_;
    vector<uint32_t> pkts_;
};

class FillWindowPkt{
public:
    explicit FillWindowPkt(unsigned int ID, unsigned src_row, unsigned src_col,
                         unsigned int size, const char* data):
                              ID_(ID), src_row_(src_row), src_col_(src_col)
    {
        assert(size%4 == 0 && "data write to Data Mem must aligned to 4B");
        genPackets(data, size); 
    }
    string to_string(){
        return uint2str(pkts_.data(), pkts_.size(), PKT_TYPE_DATA);
    }
    string to_hex(){
        return uint2hex(pkts_.data(), pkts_.size());
    }
private:
    void genPackets(const char* data, int size){
        pkts_.push_back( genStreamPktHeader(ID_, 0, src_row_, src_col_));
        uint32_t *uptr = const_cast<uint32_t*>(reinterpret_cast<const uint32_t*>(data));
        for(int i=0; i<size/4; i++){
            pkts_.push_back(uptr[i]);
        }
    }

private:
    unsigned int ID_;
    unsigned src_row_;
    unsigned src_col_;
    vector<uint32_t> pkts_;
};



class MEUtil {
private:
    MEUtil() {}
    ~MEUtil() {}
public:
    template<typename T>
    static void appendImg2IntFile(const string &fname, const T* img, int len, int line);
    static void appendImg2HexFile(const string &fname, const char* img, int len, int line);
    
    static string sendCtrlPkts(int portidx, int isPing, int isrltPing, uint32_t img_wnd_bytes, uint32_t wgt_wnd_bytes, uint32_t rlt_wnd_bytes);
    static void setBDAddrLen(int portidx, uint32_t img_wnd_bytes, uint32_t wgt_wnd_bytes, uint32_t rlt_wnd_bytes, uint32_t rlt_wnd_bytes_offset, vector<string> &retstr);
    static void startInputPkts(int isPing, vector<string> &retstr);

    static void startRltPkts(int isPing, uint32_t rlt_wnd_bytes, vector<string> &retstr);
    static void setDataMem(unsigned int addr, int32_t *param, int len, vector<string> &retstr);
    static void appendStr2File(const string &filename, const string &data);
    static uint64_t calCycle(const uint32_t out_loop, const uint32_t inner_loop, const uint32_t exec_type);
    static void appendTlastList(const string &fname, const vector<uint64_t> execlist);
};

/*
 * return pkts contain uint32_t[4]
 * pkts[0]: Stream Packet Header, ID
 * pkts[1]: Control Packet Header to control BD
 * pkts[2]: genrated by AorB, Addr_A: AorB=0, Addr_B: AorB=1;
 * pkts[3]: ctrl register contents generated by length and enable_pkt
 */

/*
 * return pkts contain uint32_t[3]
 * pkts[0]: Stream Packet Header, ID
 * pkts[1]: Control Packet Header to control BD
 * pkts[2]: genrated by baseAddr_A and lock_A, since we only use A mode, no need to modify Addr_B
 */


 /* return pkts contain uint32_t[4]
 * pkts[0]: Stream Packet Header, ID
 * pkts[1]: Control Packet Header to control BD, channel->0/1, S2MMorMM2S means write or read data memory
 * pkts[2]: valid BD, default is 0x0000001
 * pkts[3]: Start BD
 */

/* renturn pkts , [stream header]->[control header]->[data] 
 * size in [1:4]
 * pkts[0]: stream header
 * pkts[1]: control header
 * pkts[2:+size]: data
 */

/* renturn pkts , [stream header]->[data] 
 * size in [1:4]
 * pkts[0]: stream header
 * pkts[1:+(size/4)]: data, packets 4 char to 1 uint32_t
 */

/*
 * print stream packets to std cout in hex format
 * packets type | ctrlPktChangeBDLength | ctrlPktChangeBaseAddr | ctrlPktStartQueue | paramCtrlPackets  |   dataPackets
 *          len |   4                   |   3                   |   4               |   4               |   len
 */

