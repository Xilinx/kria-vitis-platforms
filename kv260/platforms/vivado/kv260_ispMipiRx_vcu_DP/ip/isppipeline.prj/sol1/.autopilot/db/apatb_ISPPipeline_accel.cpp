#include <systemc>
#include <iostream>
#include <cstdlib>
#include <cstddef>
#include <stdint.h>
#include "SysCFileHandler.h"
#include "ap_int.h"
#include "ap_fixed.h"
#include <complex>
#include <stdbool.h>
#include "autopilot_cbe.h"
#include "hls_stream.h"
#include "hls_half.h"
#include "hls_signal_handler.h"

using namespace std;
using namespace sc_core;
using namespace sc_dt;

// wrapc file define:
#define AUTOTB_TVIN_width "../tv/cdatafile/c.ISPPipeline_accel.autotvin_width.dat"
#define AUTOTB_TVOUT_width "../tv/cdatafile/c.ISPPipeline_accel.autotvout_width.dat"
#define AUTOTB_TVIN_height "../tv/cdatafile/c.ISPPipeline_accel.autotvin_height.dat"
#define AUTOTB_TVOUT_height "../tv/cdatafile/c.ISPPipeline_accel.autotvout_height.dat"
#define AUTOTB_TVIN_s_axis_video_V_data_V "../tv/cdatafile/c.ISPPipeline_accel.autotvin_s_axis_video_V_data_V.dat"
#define AUTOTB_TVOUT_s_axis_video_V_data_V "../tv/cdatafile/c.ISPPipeline_accel.autotvout_s_axis_video_V_data_V.dat"
#define AUTOTB_TVIN_s_axis_video_V_keep_V "../tv/cdatafile/c.ISPPipeline_accel.autotvin_s_axis_video_V_keep_V.dat"
#define AUTOTB_TVOUT_s_axis_video_V_keep_V "../tv/cdatafile/c.ISPPipeline_accel.autotvout_s_axis_video_V_keep_V.dat"
#define AUTOTB_TVIN_s_axis_video_V_strb_V "../tv/cdatafile/c.ISPPipeline_accel.autotvin_s_axis_video_V_strb_V.dat"
#define AUTOTB_TVOUT_s_axis_video_V_strb_V "../tv/cdatafile/c.ISPPipeline_accel.autotvout_s_axis_video_V_strb_V.dat"
#define AUTOTB_TVIN_s_axis_video_V_user_V "../tv/cdatafile/c.ISPPipeline_accel.autotvin_s_axis_video_V_user_V.dat"
#define AUTOTB_TVOUT_s_axis_video_V_user_V "../tv/cdatafile/c.ISPPipeline_accel.autotvout_s_axis_video_V_user_V.dat"
#define AUTOTB_TVIN_s_axis_video_V_last_V "../tv/cdatafile/c.ISPPipeline_accel.autotvin_s_axis_video_V_last_V.dat"
#define AUTOTB_TVOUT_s_axis_video_V_last_V "../tv/cdatafile/c.ISPPipeline_accel.autotvout_s_axis_video_V_last_V.dat"
#define AUTOTB_TVIN_s_axis_video_V_id_V "../tv/cdatafile/c.ISPPipeline_accel.autotvin_s_axis_video_V_id_V.dat"
#define AUTOTB_TVOUT_s_axis_video_V_id_V "../tv/cdatafile/c.ISPPipeline_accel.autotvout_s_axis_video_V_id_V.dat"
#define AUTOTB_TVIN_s_axis_video_V_dest_V "../tv/cdatafile/c.ISPPipeline_accel.autotvin_s_axis_video_V_dest_V.dat"
#define AUTOTB_TVOUT_s_axis_video_V_dest_V "../tv/cdatafile/c.ISPPipeline_accel.autotvout_s_axis_video_V_dest_V.dat"
#define WRAPC_STREAM_SIZE_IN_s_axis_video_V_data_V "../tv/stream_size/stream_size_in_s_axis_video_V_data_V.dat"
#define WRAPC_STREAM_INGRESS_STATUS_s_axis_video_V_data_V "../tv/stream_size/stream_ingress_status_s_axis_video_V_data_V.dat"
#define WRAPC_STREAM_SIZE_IN_s_axis_video_V_keep_V "../tv/stream_size/stream_size_in_s_axis_video_V_keep_V.dat"
#define WRAPC_STREAM_INGRESS_STATUS_s_axis_video_V_keep_V "../tv/stream_size/stream_ingress_status_s_axis_video_V_keep_V.dat"
#define WRAPC_STREAM_SIZE_IN_s_axis_video_V_strb_V "../tv/stream_size/stream_size_in_s_axis_video_V_strb_V.dat"
#define WRAPC_STREAM_INGRESS_STATUS_s_axis_video_V_strb_V "../tv/stream_size/stream_ingress_status_s_axis_video_V_strb_V.dat"
#define WRAPC_STREAM_SIZE_IN_s_axis_video_V_user_V "../tv/stream_size/stream_size_in_s_axis_video_V_user_V.dat"
#define WRAPC_STREAM_INGRESS_STATUS_s_axis_video_V_user_V "../tv/stream_size/stream_ingress_status_s_axis_video_V_user_V.dat"
#define WRAPC_STREAM_SIZE_IN_s_axis_video_V_last_V "../tv/stream_size/stream_size_in_s_axis_video_V_last_V.dat"
#define WRAPC_STREAM_INGRESS_STATUS_s_axis_video_V_last_V "../tv/stream_size/stream_ingress_status_s_axis_video_V_last_V.dat"
#define WRAPC_STREAM_SIZE_IN_s_axis_video_V_id_V "../tv/stream_size/stream_size_in_s_axis_video_V_id_V.dat"
#define WRAPC_STREAM_INGRESS_STATUS_s_axis_video_V_id_V "../tv/stream_size/stream_ingress_status_s_axis_video_V_id_V.dat"
#define WRAPC_STREAM_SIZE_IN_s_axis_video_V_dest_V "../tv/stream_size/stream_size_in_s_axis_video_V_dest_V.dat"
#define WRAPC_STREAM_INGRESS_STATUS_s_axis_video_V_dest_V "../tv/stream_size/stream_ingress_status_s_axis_video_V_dest_V.dat"
#define AUTOTB_TVIN_m_axis_video_V_data_V "../tv/cdatafile/c.ISPPipeline_accel.autotvin_m_axis_video_V_data_V.dat"
#define AUTOTB_TVOUT_m_axis_video_V_data_V "../tv/cdatafile/c.ISPPipeline_accel.autotvout_m_axis_video_V_data_V.dat"
#define AUTOTB_TVIN_m_axis_video_V_keep_V "../tv/cdatafile/c.ISPPipeline_accel.autotvin_m_axis_video_V_keep_V.dat"
#define AUTOTB_TVOUT_m_axis_video_V_keep_V "../tv/cdatafile/c.ISPPipeline_accel.autotvout_m_axis_video_V_keep_V.dat"
#define AUTOTB_TVIN_m_axis_video_V_strb_V "../tv/cdatafile/c.ISPPipeline_accel.autotvin_m_axis_video_V_strb_V.dat"
#define AUTOTB_TVOUT_m_axis_video_V_strb_V "../tv/cdatafile/c.ISPPipeline_accel.autotvout_m_axis_video_V_strb_V.dat"
#define AUTOTB_TVIN_m_axis_video_V_user_V "../tv/cdatafile/c.ISPPipeline_accel.autotvin_m_axis_video_V_user_V.dat"
#define AUTOTB_TVOUT_m_axis_video_V_user_V "../tv/cdatafile/c.ISPPipeline_accel.autotvout_m_axis_video_V_user_V.dat"
#define AUTOTB_TVIN_m_axis_video_V_last_V "../tv/cdatafile/c.ISPPipeline_accel.autotvin_m_axis_video_V_last_V.dat"
#define AUTOTB_TVOUT_m_axis_video_V_last_V "../tv/cdatafile/c.ISPPipeline_accel.autotvout_m_axis_video_V_last_V.dat"
#define AUTOTB_TVIN_m_axis_video_V_id_V "../tv/cdatafile/c.ISPPipeline_accel.autotvin_m_axis_video_V_id_V.dat"
#define AUTOTB_TVOUT_m_axis_video_V_id_V "../tv/cdatafile/c.ISPPipeline_accel.autotvout_m_axis_video_V_id_V.dat"
#define AUTOTB_TVIN_m_axis_video_V_dest_V "../tv/cdatafile/c.ISPPipeline_accel.autotvin_m_axis_video_V_dest_V.dat"
#define AUTOTB_TVOUT_m_axis_video_V_dest_V "../tv/cdatafile/c.ISPPipeline_accel.autotvout_m_axis_video_V_dest_V.dat"
#define WRAPC_STREAM_SIZE_OUT_m_axis_video_V_data_V "../tv/stream_size/stream_size_out_m_axis_video_V_data_V.dat"
#define WRAPC_STREAM_EGRESS_STATUS_m_axis_video_V_data_V "../tv/stream_size/stream_egress_status_m_axis_video_V_data_V.dat"
#define WRAPC_STREAM_SIZE_OUT_m_axis_video_V_keep_V "../tv/stream_size/stream_size_out_m_axis_video_V_keep_V.dat"
#define WRAPC_STREAM_EGRESS_STATUS_m_axis_video_V_keep_V "../tv/stream_size/stream_egress_status_m_axis_video_V_keep_V.dat"
#define WRAPC_STREAM_SIZE_OUT_m_axis_video_V_strb_V "../tv/stream_size/stream_size_out_m_axis_video_V_strb_V.dat"
#define WRAPC_STREAM_EGRESS_STATUS_m_axis_video_V_strb_V "../tv/stream_size/stream_egress_status_m_axis_video_V_strb_V.dat"
#define WRAPC_STREAM_SIZE_OUT_m_axis_video_V_user_V "../tv/stream_size/stream_size_out_m_axis_video_V_user_V.dat"
#define WRAPC_STREAM_EGRESS_STATUS_m_axis_video_V_user_V "../tv/stream_size/stream_egress_status_m_axis_video_V_user_V.dat"
#define WRAPC_STREAM_SIZE_OUT_m_axis_video_V_last_V "../tv/stream_size/stream_size_out_m_axis_video_V_last_V.dat"
#define WRAPC_STREAM_EGRESS_STATUS_m_axis_video_V_last_V "../tv/stream_size/stream_egress_status_m_axis_video_V_last_V.dat"
#define WRAPC_STREAM_SIZE_OUT_m_axis_video_V_id_V "../tv/stream_size/stream_size_out_m_axis_video_V_id_V.dat"
#define WRAPC_STREAM_EGRESS_STATUS_m_axis_video_V_id_V "../tv/stream_size/stream_egress_status_m_axis_video_V_id_V.dat"
#define WRAPC_STREAM_SIZE_OUT_m_axis_video_V_dest_V "../tv/stream_size/stream_size_out_m_axis_video_V_dest_V.dat"
#define WRAPC_STREAM_EGRESS_STATUS_m_axis_video_V_dest_V "../tv/stream_size/stream_egress_status_m_axis_video_V_dest_V.dat"
#define AUTOTB_TVIN_rgain "../tv/cdatafile/c.ISPPipeline_accel.autotvin_rgain.dat"
#define AUTOTB_TVOUT_rgain "../tv/cdatafile/c.ISPPipeline_accel.autotvout_rgain.dat"
#define AUTOTB_TVIN_bgain "../tv/cdatafile/c.ISPPipeline_accel.autotvin_bgain.dat"
#define AUTOTB_TVOUT_bgain "../tv/cdatafile/c.ISPPipeline_accel.autotvout_bgain.dat"
#define AUTOTB_TVIN_gamma_lut "../tv/cdatafile/c.ISPPipeline_accel.autotvin_gamma_lut.dat"
#define AUTOTB_TVOUT_gamma_lut "../tv/cdatafile/c.ISPPipeline_accel.autotvout_gamma_lut.dat"
#define AUTOTB_TVIN_mode_reg "../tv/cdatafile/c.ISPPipeline_accel.autotvin_mode_reg.dat"
#define AUTOTB_TVOUT_mode_reg "../tv/cdatafile/c.ISPPipeline_accel.autotvout_mode_reg.dat"
#define AUTOTB_TVIN_pawb "../tv/cdatafile/c.ISPPipeline_accel.autotvin_pawb.dat"
#define AUTOTB_TVOUT_pawb "../tv/cdatafile/c.ISPPipeline_accel.autotvout_pawb.dat"

#define INTER_TCL "../tv/cdatafile/ref.tcl"

// tvout file define:
#define AUTOTB_TVOUT_PC_width "../tv/rtldatafile/rtl.ISPPipeline_accel.autotvout_width.dat"
#define AUTOTB_TVOUT_PC_height "../tv/rtldatafile/rtl.ISPPipeline_accel.autotvout_height.dat"
#define AUTOTB_TVOUT_PC_s_axis_video_V_data_V "../tv/rtldatafile/rtl.ISPPipeline_accel.autotvout_s_axis_video_V_data_V.dat"
#define AUTOTB_TVOUT_PC_s_axis_video_V_keep_V "../tv/rtldatafile/rtl.ISPPipeline_accel.autotvout_s_axis_video_V_keep_V.dat"
#define AUTOTB_TVOUT_PC_s_axis_video_V_strb_V "../tv/rtldatafile/rtl.ISPPipeline_accel.autotvout_s_axis_video_V_strb_V.dat"
#define AUTOTB_TVOUT_PC_s_axis_video_V_user_V "../tv/rtldatafile/rtl.ISPPipeline_accel.autotvout_s_axis_video_V_user_V.dat"
#define AUTOTB_TVOUT_PC_s_axis_video_V_last_V "../tv/rtldatafile/rtl.ISPPipeline_accel.autotvout_s_axis_video_V_last_V.dat"
#define AUTOTB_TVOUT_PC_s_axis_video_V_id_V "../tv/rtldatafile/rtl.ISPPipeline_accel.autotvout_s_axis_video_V_id_V.dat"
#define AUTOTB_TVOUT_PC_s_axis_video_V_dest_V "../tv/rtldatafile/rtl.ISPPipeline_accel.autotvout_s_axis_video_V_dest_V.dat"
#define AUTOTB_TVOUT_PC_m_axis_video_V_data_V "../tv/rtldatafile/rtl.ISPPipeline_accel.autotvout_m_axis_video_V_data_V.dat"
#define AUTOTB_TVOUT_PC_m_axis_video_V_keep_V "../tv/rtldatafile/rtl.ISPPipeline_accel.autotvout_m_axis_video_V_keep_V.dat"
#define AUTOTB_TVOUT_PC_m_axis_video_V_strb_V "../tv/rtldatafile/rtl.ISPPipeline_accel.autotvout_m_axis_video_V_strb_V.dat"
#define AUTOTB_TVOUT_PC_m_axis_video_V_user_V "../tv/rtldatafile/rtl.ISPPipeline_accel.autotvout_m_axis_video_V_user_V.dat"
#define AUTOTB_TVOUT_PC_m_axis_video_V_last_V "../tv/rtldatafile/rtl.ISPPipeline_accel.autotvout_m_axis_video_V_last_V.dat"
#define AUTOTB_TVOUT_PC_m_axis_video_V_id_V "../tv/rtldatafile/rtl.ISPPipeline_accel.autotvout_m_axis_video_V_id_V.dat"
#define AUTOTB_TVOUT_PC_m_axis_video_V_dest_V "../tv/rtldatafile/rtl.ISPPipeline_accel.autotvout_m_axis_video_V_dest_V.dat"
#define AUTOTB_TVOUT_PC_rgain "../tv/rtldatafile/rtl.ISPPipeline_accel.autotvout_rgain.dat"
#define AUTOTB_TVOUT_PC_bgain "../tv/rtldatafile/rtl.ISPPipeline_accel.autotvout_bgain.dat"
#define AUTOTB_TVOUT_PC_gamma_lut "../tv/rtldatafile/rtl.ISPPipeline_accel.autotvout_gamma_lut.dat"
#define AUTOTB_TVOUT_PC_mode_reg "../tv/rtldatafile/rtl.ISPPipeline_accel.autotvout_mode_reg.dat"
#define AUTOTB_TVOUT_PC_pawb "../tv/rtldatafile/rtl.ISPPipeline_accel.autotvout_pawb.dat"


static const bool little_endian()
{
  int a = 1;
  return *(char*)&a == 1;
}

inline static void rev_endian(char* p, size_t nbytes)
{
  std::reverse(p, p+nbytes);
}

template<size_t bit_width>
struct transaction {
  typedef uint64_t depth_t;
  static const size_t wbytes = (bit_width+7)>>3;
  static const size_t dbytes = sizeof(depth_t);
  const depth_t depth;
  const size_t vbytes;
  const size_t tbytes;
  char * const p;
  typedef char (*p_dat)[wbytes];
  p_dat vp;

  transaction(depth_t depth)
    : depth(depth), vbytes(wbytes*depth), tbytes(dbytes+vbytes),
      p(new char[tbytes]) {
    *(depth_t*)p = depth;
    rev_endian(p, dbytes);
    vp = (p_dat) (p+dbytes);
  }

  void reorder() {
    rev_endian(p, dbytes);
    p_dat vp = (p_dat) (p+dbytes);
    for (depth_t i = 0; i < depth; ++i) {
      rev_endian(vp[i], wbytes);
    }
  }

  template<size_t psize>
  void import(char* param, depth_t num, int64_t offset) {
    param -= offset*psize;
    for (depth_t i = 0; i < num; ++i) {
      memcpy(vp[i], param, wbytes);
      param += psize;
      if (little_endian()) {
        rev_endian(vp[i], wbytes);
      }
    }
    vp += num;
  }

  template<size_t psize>
  void send(char* param, depth_t num) {
    for (depth_t i = 0; i < num; ++i) {
      memcpy(param, vp[i], wbytes);
      param += psize;
    }
    vp += num;
  }

  template<size_t psize>
  void send(char* param, depth_t num, int64_t skip) {
    for (depth_t i = 0; i < num; ++i) {
      memcpy(param, vp[skip+i], wbytes);
      param += psize;
    }
  }

  ~transaction() { if (p) { delete[] p; } }
};


inline static const std::string begin_str(int num)
{
  return std::string("[[transaction]]           ")
         .append(std::to_string(num))
         .append("\n");
}

inline static const std::string end_str()
{
  return std::string("[[/transaction]]\n");
}

const std::string formatData(unsigned char *pos, size_t wbits)
{
  bool LE = little_endian();
  size_t wbytes = (wbits+7)>>3;
  size_t i = LE ? wbytes-1 : 0;
  auto next = [&] () {
    auto c = pos[i];
    LE ? --i : ++i;
    return c;
  };
  std::ostringstream ss;
  ss << "0x";
  if (int t = (wbits & 0x7)) {
    if (t <= 4) {
      unsigned char mask = (1<<t)-1;
      ss << std::hex << std::setfill('0') << std::setw(1)
         << (int) (next() & mask);
      wbytes -= 1;
    }
  }
  for (size_t i = 0; i < wbytes; ++i) {
    ss << std::hex << std::setfill('0') << std::setw(2) << (int)next();
  }
  ss.put('\n');
  return ss.str();
}

static bool RTLOutputCheckAndReplacement(std::string &data)
{
  bool changed = false;
  for (size_t i = 2; i < data.size(); ++i) {
    if (data[i] == 'X' || data[i] == 'x') {
      data[i] = '0';
      changed = true;
    }
  }
  return changed;
}

struct SimException : public std::exception {
  const char *msg;
  const size_t line;
  SimException(const char *msg, const size_t line)
    : msg(msg), line(line)
  {
  }
};

template<size_t bit_width>
class PostCheck
{
  static const char *bad;
  static const char *err;
  std::fstream stream;
  std::string s;

  void send(char *p, ap_uint<bit_width> &data, size_t l, size_t rest)
  {
    if (rest == 0) {
      if (!little_endian()) {
        const size_t wbytes = (bit_width+7)>>3;
        rev_endian(p-wbytes, wbytes);
      }
    } else if (rest < 8) {
      *p = data.range(l+rest-1, l).to_uint();
      send(p+1, data, l+rest, 0);
    } else {
      *p = data.range(l+8-1, l).to_uint();
      send(p+1, data, l+8, rest-8);
    }
  }

  void readline()
  {
    std::getline(stream, s);
    if (stream.eof()) {
      throw SimException(bad, __LINE__);
    }
  }

public:
  char *param;
  size_t psize;
  size_t depth;

  PostCheck(const char *file)
  {
    stream.open(file);
    if (stream.fail()) {
      throw SimException(err, __LINE__);
    } else {
      readline();
      if (s != "[[[runtime]]]") {
        throw SimException(bad, __LINE__);
      }
    }
  }

  ~PostCheck() noexcept(false)
  {
    stream.close();
  }

  void run(size_t AESL_transaction_pc, size_t skip)
  {
    if (stream.peek() == '[') {
      readline();
    }

    for (size_t i = 0; i < skip; ++i) {
      readline();
    }

    bool foundX = false;
    for (size_t i = 0; i < depth; ++i) {
      readline();
      foundX |= RTLOutputCheckAndReplacement(s);
      ap_uint<bit_width> data(s.c_str(), 16);
      send(param+i*psize, data, 0, bit_width);
    }
    if (foundX) {
      std::cerr << "WARNING: [SIM 212-201] RTL produces unknown value "
                << "'x' or 'X' on some port, possible cause: "
                << "There are uninitialized variables in the design.\n";
    }

    if (stream.peek() == '[') {
      readline();
    }
  }
};

template<size_t bit_width>
const char* PostCheck<bit_width>::bad = "Bad TV file";

template<size_t bit_width>
const char* PostCheck<bit_width>::err = "Error on TV file";
      
class INTER_TCL_FILE {
  public:
INTER_TCL_FILE(const char* name) {
  mName = name; 
  width_depth = 0;
  height_depth = 0;
  s_axis_video_V_data_V_depth = 0;
  s_axis_video_V_keep_V_depth = 0;
  s_axis_video_V_strb_V_depth = 0;
  s_axis_video_V_user_V_depth = 0;
  s_axis_video_V_last_V_depth = 0;
  s_axis_video_V_id_V_depth = 0;
  s_axis_video_V_dest_V_depth = 0;
  m_axis_video_V_data_V_depth = 0;
  m_axis_video_V_keep_V_depth = 0;
  m_axis_video_V_strb_V_depth = 0;
  m_axis_video_V_user_V_depth = 0;
  m_axis_video_V_last_V_depth = 0;
  m_axis_video_V_id_V_depth = 0;
  m_axis_video_V_dest_V_depth = 0;
  rgain_depth = 0;
  bgain_depth = 0;
  gamma_lut_depth = 0;
  mode_reg_depth = 0;
  pawb_depth = 0;
  trans_num =0;
}
~INTER_TCL_FILE() {
  mFile.open(mName);
  if (!mFile.good()) {
    cout << "Failed to open file ref.tcl" << endl;
    exit (1); 
  }
  string total_list = get_depth_list();
  mFile << "set depth_list {\n";
  mFile << total_list;
  mFile << "}\n";
  mFile << "set trans_num "<<trans_num<<endl;
  mFile.close();
}
string get_depth_list () {
  stringstream total_list;
  total_list << "{width " << width_depth << "}\n";
  total_list << "{height " << height_depth << "}\n";
  total_list << "{s_axis_video_V_data_V " << s_axis_video_V_data_V_depth << "}\n";
  total_list << "{s_axis_video_V_keep_V " << s_axis_video_V_keep_V_depth << "}\n";
  total_list << "{s_axis_video_V_strb_V " << s_axis_video_V_strb_V_depth << "}\n";
  total_list << "{s_axis_video_V_user_V " << s_axis_video_V_user_V_depth << "}\n";
  total_list << "{s_axis_video_V_last_V " << s_axis_video_V_last_V_depth << "}\n";
  total_list << "{s_axis_video_V_id_V " << s_axis_video_V_id_V_depth << "}\n";
  total_list << "{s_axis_video_V_dest_V " << s_axis_video_V_dest_V_depth << "}\n";
  total_list << "{m_axis_video_V_data_V " << m_axis_video_V_data_V_depth << "}\n";
  total_list << "{m_axis_video_V_keep_V " << m_axis_video_V_keep_V_depth << "}\n";
  total_list << "{m_axis_video_V_strb_V " << m_axis_video_V_strb_V_depth << "}\n";
  total_list << "{m_axis_video_V_user_V " << m_axis_video_V_user_V_depth << "}\n";
  total_list << "{m_axis_video_V_last_V " << m_axis_video_V_last_V_depth << "}\n";
  total_list << "{m_axis_video_V_id_V " << m_axis_video_V_id_V_depth << "}\n";
  total_list << "{m_axis_video_V_dest_V " << m_axis_video_V_dest_V_depth << "}\n";
  total_list << "{rgain " << rgain_depth << "}\n";
  total_list << "{bgain " << bgain_depth << "}\n";
  total_list << "{gamma_lut " << gamma_lut_depth << "}\n";
  total_list << "{mode_reg " << mode_reg_depth << "}\n";
  total_list << "{pawb " << pawb_depth << "}\n";
  return total_list.str();
}
void set_num (int num , int* class_num) {
  (*class_num) = (*class_num) > num ? (*class_num) : num;
}
void set_string(std::string list, std::string* class_list) {
  (*class_list) = list;
}
  public:
    int width_depth;
    int height_depth;
    int s_axis_video_V_data_V_depth;
    int s_axis_video_V_keep_V_depth;
    int s_axis_video_V_strb_V_depth;
    int s_axis_video_V_user_V_depth;
    int s_axis_video_V_last_V_depth;
    int s_axis_video_V_id_V_depth;
    int s_axis_video_V_dest_V_depth;
    int m_axis_video_V_data_V_depth;
    int m_axis_video_V_keep_V_depth;
    int m_axis_video_V_strb_V_depth;
    int m_axis_video_V_user_V_depth;
    int m_axis_video_V_last_V_depth;
    int m_axis_video_V_id_V_depth;
    int m_axis_video_V_dest_V_depth;
    int rgain_depth;
    int bgain_depth;
    int gamma_lut_depth;
    int mode_reg_depth;
    int pawb_depth;
    int trans_num;
  private:
    ofstream mFile;
    const char* mName;
};


extern "C" void ISPPipeline_accel_hw_stub_wrapper(short, short, volatile void *, volatile void *, volatile void *, volatile void *, volatile void *, volatile void *, volatile void *, volatile void *, volatile void *, volatile void *, volatile void *, volatile void *, volatile void *, volatile void *, short, short, volatile void *, char, short);

extern "C" void apatb_ISPPipeline_accel_hw(short __xlx_apatb_param_width, short __xlx_apatb_param_height, volatile void * __xlx_apatb_param_s_axis_video_V_data_V, volatile void * __xlx_apatb_param_s_axis_video_V_keep_V, volatile void * __xlx_apatb_param_s_axis_video_V_strb_V, volatile void * __xlx_apatb_param_s_axis_video_V_user_V, volatile void * __xlx_apatb_param_s_axis_video_V_last_V, volatile void * __xlx_apatb_param_s_axis_video_V_id_V, volatile void * __xlx_apatb_param_s_axis_video_V_dest_V, volatile void * __xlx_apatb_param_m_axis_video_V_data_V, volatile void * __xlx_apatb_param_m_axis_video_V_keep_V, volatile void * __xlx_apatb_param_m_axis_video_V_strb_V, volatile void * __xlx_apatb_param_m_axis_video_V_user_V, volatile void * __xlx_apatb_param_m_axis_video_V_last_V, volatile void * __xlx_apatb_param_m_axis_video_V_id_V, volatile void * __xlx_apatb_param_m_axis_video_V_dest_V, short __xlx_apatb_param_rgain, short __xlx_apatb_param_bgain, volatile void * __xlx_apatb_param_gamma_lut, char __xlx_apatb_param_mode_reg, short __xlx_apatb_param_pawb) {
  refine_signal_handler();
  fstream wrapc_switch_file_token;
  wrapc_switch_file_token.open(".hls_cosim_wrapc_switch.log");
static AESL_FILE_HANDLER aesl_fh;
  int AESL_i;
  if (wrapc_switch_file_token.good())
  {

    CodeState = ENTER_WRAPC_PC;
    static unsigned AESL_transaction_pc = 0;
    string AESL_token;
    string AESL_num;
long __xlx_apatb_param_s_axis_video_stream_buf_final_size;
{
      static ifstream rtl_tv_out_file;
      if (!rtl_tv_out_file.is_open()) {
        rtl_tv_out_file.open(WRAPC_STREAM_SIZE_IN_s_axis_video_V_data_V);
        if (rtl_tv_out_file.good()) {
          rtl_tv_out_file >> AESL_token;
          if (AESL_token != "[[[runtime]]]")
            exit(1);
        }
      }
  
      if (rtl_tv_out_file.good()) {
        rtl_tv_out_file >> AESL_token; 
        rtl_tv_out_file >> AESL_num;  // transaction number
        if (AESL_token != "[[transaction]]") {
          cerr << "Unexpected token: " << AESL_token << endl;
          exit(1);
        }
        if (atoi(AESL_num.c_str()) == AESL_transaction_pc) {
          rtl_tv_out_file >> AESL_token; //data
          while (AESL_token != "[[/transaction]]"){__xlx_apatb_param_s_axis_video_stream_buf_final_size = atoi(AESL_token.c_str());

            rtl_tv_out_file >> AESL_token; //data or [[/transaction]]
            if (AESL_token == "[[[/runtime]]]" || rtl_tv_out_file.eof())
              exit(1);
          }
        } // end transaction
      } // end file is good
    } // end post check logic bolck
  for (long i = 0; i < __xlx_apatb_param_s_axis_video_stream_buf_final_size; ++i) {
((hls::stream<short>*)__xlx_apatb_param_s_axis_video_V_data_V)->read();
((hls::stream<char>*)__xlx_apatb_param_s_axis_video_V_keep_V)->read();
((hls::stream<char>*)__xlx_apatb_param_s_axis_video_V_strb_V)->read();
((hls::stream<char>*)__xlx_apatb_param_s_axis_video_V_user_V)->read();
((hls::stream<char>*)__xlx_apatb_param_s_axis_video_V_last_V)->read();
((hls::stream<char>*)__xlx_apatb_param_s_axis_video_V_id_V)->read();
((hls::stream<char>*)__xlx_apatb_param_s_axis_video_V_dest_V)->read();
}
{unsigned xlx_stream_m_axis_video_size = 0;

          std::vector<sc_bv<24> > m_axis_video_V_data_V_pc_buffer_Copy;
{
      static ifstream rtl_tv_out_file;
      if (!rtl_tv_out_file.is_open()) {
        rtl_tv_out_file.open(AUTOTB_TVOUT_PC_m_axis_video_V_data_V);
        if (rtl_tv_out_file.good()) {
          rtl_tv_out_file >> AESL_token;
          if (AESL_token != "[[[runtime]]]")
            exit(1);
        }
      }
  
      if (rtl_tv_out_file.good()) {
        rtl_tv_out_file >> AESL_token; 
        rtl_tv_out_file >> AESL_num;  // transaction number
        if (AESL_token != "[[transaction]]") {
          cerr << "Unexpected token: " << AESL_token << endl;
          exit(1);
        }
        if (atoi(AESL_num.c_str()) == AESL_transaction_pc) {
          std::vector<sc_bv<24> > m_axis_video_V_data_V_pc_buffer;
          int i = 0;
          bool has_unknown_value = false;
          rtl_tv_out_file >> AESL_token; //data
          while (AESL_token != "[[/transaction]]"){

            has_unknown_value |= RTLOutputCheckAndReplacement(AESL_token);
  
            // push token into output port buffer
            if (AESL_token != "") {
              m_axis_video_V_data_V_pc_buffer.push_back(AESL_token.c_str());
              i++;
            }
  
            rtl_tv_out_file >> AESL_token; //data or [[/transaction]]
            if (AESL_token == "[[[/runtime]]]" || rtl_tv_out_file.eof())
              exit(1);
          }
          if (has_unknown_value) {
            cerr << "WARNING: [SIM 212-201] RTL produces unknown value 'x' or 'X' on port " 
                 << "m_axis_video" << ", possible cause: There are uninitialized variables in the C design."
                 << endl; 
          }
  
          if (i > 0) {xlx_stream_m_axis_video_size=m_axis_video_V_data_V_pc_buffer.size();
m_axis_video_V_data_V_pc_buffer_Copy=m_axis_video_V_data_V_pc_buffer;
}
        } // end transaction
      } // end file is good
    } // end post check logic bolck
  
          std::vector<sc_bv<24> > m_axis_video_V_keep_V_pc_buffer_Copy;
{
      static ifstream rtl_tv_out_file;
      if (!rtl_tv_out_file.is_open()) {
        rtl_tv_out_file.open(AUTOTB_TVOUT_PC_m_axis_video_V_keep_V);
        if (rtl_tv_out_file.good()) {
          rtl_tv_out_file >> AESL_token;
          if (AESL_token != "[[[runtime]]]")
            exit(1);
        }
      }
  
      if (rtl_tv_out_file.good()) {
        rtl_tv_out_file >> AESL_token; 
        rtl_tv_out_file >> AESL_num;  // transaction number
        if (AESL_token != "[[transaction]]") {
          cerr << "Unexpected token: " << AESL_token << endl;
          exit(1);
        }
        if (atoi(AESL_num.c_str()) == AESL_transaction_pc) {
          std::vector<sc_bv<24> > m_axis_video_V_keep_V_pc_buffer;
          int i = 0;
          bool has_unknown_value = false;
          rtl_tv_out_file >> AESL_token; //data
          while (AESL_token != "[[/transaction]]"){

            has_unknown_value |= RTLOutputCheckAndReplacement(AESL_token);
  
            // push token into output port buffer
            if (AESL_token != "") {
              m_axis_video_V_keep_V_pc_buffer.push_back(AESL_token.c_str());
              i++;
            }
  
            rtl_tv_out_file >> AESL_token; //data or [[/transaction]]
            if (AESL_token == "[[[/runtime]]]" || rtl_tv_out_file.eof())
              exit(1);
          }
          if (has_unknown_value) {
            cerr << "WARNING: [SIM 212-201] RTL produces unknown value 'x' or 'X' on port " 
                 << "m_axis_video" << ", possible cause: There are uninitialized variables in the C design."
                 << endl; 
          }
  
          if (i > 0) {xlx_stream_m_axis_video_size=m_axis_video_V_keep_V_pc_buffer.size();
m_axis_video_V_keep_V_pc_buffer_Copy=m_axis_video_V_keep_V_pc_buffer;
}
        } // end transaction
      } // end file is good
    } // end post check logic bolck
  
          std::vector<sc_bv<24> > m_axis_video_V_strb_V_pc_buffer_Copy;
{
      static ifstream rtl_tv_out_file;
      if (!rtl_tv_out_file.is_open()) {
        rtl_tv_out_file.open(AUTOTB_TVOUT_PC_m_axis_video_V_strb_V);
        if (rtl_tv_out_file.good()) {
          rtl_tv_out_file >> AESL_token;
          if (AESL_token != "[[[runtime]]]")
            exit(1);
        }
      }
  
      if (rtl_tv_out_file.good()) {
        rtl_tv_out_file >> AESL_token; 
        rtl_tv_out_file >> AESL_num;  // transaction number
        if (AESL_token != "[[transaction]]") {
          cerr << "Unexpected token: " << AESL_token << endl;
          exit(1);
        }
        if (atoi(AESL_num.c_str()) == AESL_transaction_pc) {
          std::vector<sc_bv<24> > m_axis_video_V_strb_V_pc_buffer;
          int i = 0;
          bool has_unknown_value = false;
          rtl_tv_out_file >> AESL_token; //data
          while (AESL_token != "[[/transaction]]"){

            has_unknown_value |= RTLOutputCheckAndReplacement(AESL_token);
  
            // push token into output port buffer
            if (AESL_token != "") {
              m_axis_video_V_strb_V_pc_buffer.push_back(AESL_token.c_str());
              i++;
            }
  
            rtl_tv_out_file >> AESL_token; //data or [[/transaction]]
            if (AESL_token == "[[[/runtime]]]" || rtl_tv_out_file.eof())
              exit(1);
          }
          if (has_unknown_value) {
            cerr << "WARNING: [SIM 212-201] RTL produces unknown value 'x' or 'X' on port " 
                 << "m_axis_video" << ", possible cause: There are uninitialized variables in the C design."
                 << endl; 
          }
  
          if (i > 0) {xlx_stream_m_axis_video_size=m_axis_video_V_strb_V_pc_buffer.size();
m_axis_video_V_strb_V_pc_buffer_Copy=m_axis_video_V_strb_V_pc_buffer;
}
        } // end transaction
      } // end file is good
    } // end post check logic bolck
  
          std::vector<sc_bv<24> > m_axis_video_V_user_V_pc_buffer_Copy;
{
      static ifstream rtl_tv_out_file;
      if (!rtl_tv_out_file.is_open()) {
        rtl_tv_out_file.open(AUTOTB_TVOUT_PC_m_axis_video_V_user_V);
        if (rtl_tv_out_file.good()) {
          rtl_tv_out_file >> AESL_token;
          if (AESL_token != "[[[runtime]]]")
            exit(1);
        }
      }
  
      if (rtl_tv_out_file.good()) {
        rtl_tv_out_file >> AESL_token; 
        rtl_tv_out_file >> AESL_num;  // transaction number
        if (AESL_token != "[[transaction]]") {
          cerr << "Unexpected token: " << AESL_token << endl;
          exit(1);
        }
        if (atoi(AESL_num.c_str()) == AESL_transaction_pc) {
          std::vector<sc_bv<24> > m_axis_video_V_user_V_pc_buffer;
          int i = 0;
          bool has_unknown_value = false;
          rtl_tv_out_file >> AESL_token; //data
          while (AESL_token != "[[/transaction]]"){

            has_unknown_value |= RTLOutputCheckAndReplacement(AESL_token);
  
            // push token into output port buffer
            if (AESL_token != "") {
              m_axis_video_V_user_V_pc_buffer.push_back(AESL_token.c_str());
              i++;
            }
  
            rtl_tv_out_file >> AESL_token; //data or [[/transaction]]
            if (AESL_token == "[[[/runtime]]]" || rtl_tv_out_file.eof())
              exit(1);
          }
          if (has_unknown_value) {
            cerr << "WARNING: [SIM 212-201] RTL produces unknown value 'x' or 'X' on port " 
                 << "m_axis_video" << ", possible cause: There are uninitialized variables in the C design."
                 << endl; 
          }
  
          if (i > 0) {xlx_stream_m_axis_video_size=m_axis_video_V_user_V_pc_buffer.size();
m_axis_video_V_user_V_pc_buffer_Copy=m_axis_video_V_user_V_pc_buffer;
}
        } // end transaction
      } // end file is good
    } // end post check logic bolck
  
          std::vector<sc_bv<24> > m_axis_video_V_last_V_pc_buffer_Copy;
{
      static ifstream rtl_tv_out_file;
      if (!rtl_tv_out_file.is_open()) {
        rtl_tv_out_file.open(AUTOTB_TVOUT_PC_m_axis_video_V_last_V);
        if (rtl_tv_out_file.good()) {
          rtl_tv_out_file >> AESL_token;
          if (AESL_token != "[[[runtime]]]")
            exit(1);
        }
      }
  
      if (rtl_tv_out_file.good()) {
        rtl_tv_out_file >> AESL_token; 
        rtl_tv_out_file >> AESL_num;  // transaction number
        if (AESL_token != "[[transaction]]") {
          cerr << "Unexpected token: " << AESL_token << endl;
          exit(1);
        }
        if (atoi(AESL_num.c_str()) == AESL_transaction_pc) {
          std::vector<sc_bv<24> > m_axis_video_V_last_V_pc_buffer;
          int i = 0;
          bool has_unknown_value = false;
          rtl_tv_out_file >> AESL_token; //data
          while (AESL_token != "[[/transaction]]"){

            has_unknown_value |= RTLOutputCheckAndReplacement(AESL_token);
  
            // push token into output port buffer
            if (AESL_token != "") {
              m_axis_video_V_last_V_pc_buffer.push_back(AESL_token.c_str());
              i++;
            }
  
            rtl_tv_out_file >> AESL_token; //data or [[/transaction]]
            if (AESL_token == "[[[/runtime]]]" || rtl_tv_out_file.eof())
              exit(1);
          }
          if (has_unknown_value) {
            cerr << "WARNING: [SIM 212-201] RTL produces unknown value 'x' or 'X' on port " 
                 << "m_axis_video" << ", possible cause: There are uninitialized variables in the C design."
                 << endl; 
          }
  
          if (i > 0) {xlx_stream_m_axis_video_size=m_axis_video_V_last_V_pc_buffer.size();
m_axis_video_V_last_V_pc_buffer_Copy=m_axis_video_V_last_V_pc_buffer;
}
        } // end transaction
      } // end file is good
    } // end post check logic bolck
  
          std::vector<sc_bv<24> > m_axis_video_V_id_V_pc_buffer_Copy;
{
      static ifstream rtl_tv_out_file;
      if (!rtl_tv_out_file.is_open()) {
        rtl_tv_out_file.open(AUTOTB_TVOUT_PC_m_axis_video_V_id_V);
        if (rtl_tv_out_file.good()) {
          rtl_tv_out_file >> AESL_token;
          if (AESL_token != "[[[runtime]]]")
            exit(1);
        }
      }
  
      if (rtl_tv_out_file.good()) {
        rtl_tv_out_file >> AESL_token; 
        rtl_tv_out_file >> AESL_num;  // transaction number
        if (AESL_token != "[[transaction]]") {
          cerr << "Unexpected token: " << AESL_token << endl;
          exit(1);
        }
        if (atoi(AESL_num.c_str()) == AESL_transaction_pc) {
          std::vector<sc_bv<24> > m_axis_video_V_id_V_pc_buffer;
          int i = 0;
          bool has_unknown_value = false;
          rtl_tv_out_file >> AESL_token; //data
          while (AESL_token != "[[/transaction]]"){

            has_unknown_value |= RTLOutputCheckAndReplacement(AESL_token);
  
            // push token into output port buffer
            if (AESL_token != "") {
              m_axis_video_V_id_V_pc_buffer.push_back(AESL_token.c_str());
              i++;
            }
  
            rtl_tv_out_file >> AESL_token; //data or [[/transaction]]
            if (AESL_token == "[[[/runtime]]]" || rtl_tv_out_file.eof())
              exit(1);
          }
          if (has_unknown_value) {
            cerr << "WARNING: [SIM 212-201] RTL produces unknown value 'x' or 'X' on port " 
                 << "m_axis_video" << ", possible cause: There are uninitialized variables in the C design."
                 << endl; 
          }
  
          if (i > 0) {xlx_stream_m_axis_video_size=m_axis_video_V_id_V_pc_buffer.size();
m_axis_video_V_id_V_pc_buffer_Copy=m_axis_video_V_id_V_pc_buffer;
}
        } // end transaction
      } // end file is good
    } // end post check logic bolck
  
          std::vector<sc_bv<24> > m_axis_video_V_dest_V_pc_buffer_Copy;
{
      static ifstream rtl_tv_out_file;
      if (!rtl_tv_out_file.is_open()) {
        rtl_tv_out_file.open(AUTOTB_TVOUT_PC_m_axis_video_V_dest_V);
        if (rtl_tv_out_file.good()) {
          rtl_tv_out_file >> AESL_token;
          if (AESL_token != "[[[runtime]]]")
            exit(1);
        }
      }
  
      if (rtl_tv_out_file.good()) {
        rtl_tv_out_file >> AESL_token; 
        rtl_tv_out_file >> AESL_num;  // transaction number
        if (AESL_token != "[[transaction]]") {
          cerr << "Unexpected token: " << AESL_token << endl;
          exit(1);
        }
        if (atoi(AESL_num.c_str()) == AESL_transaction_pc) {
          std::vector<sc_bv<24> > m_axis_video_V_dest_V_pc_buffer;
          int i = 0;
          bool has_unknown_value = false;
          rtl_tv_out_file >> AESL_token; //data
          while (AESL_token != "[[/transaction]]"){

            has_unknown_value |= RTLOutputCheckAndReplacement(AESL_token);
  
            // push token into output port buffer
            if (AESL_token != "") {
              m_axis_video_V_dest_V_pc_buffer.push_back(AESL_token.c_str());
              i++;
            }
  
            rtl_tv_out_file >> AESL_token; //data or [[/transaction]]
            if (AESL_token == "[[[/runtime]]]" || rtl_tv_out_file.eof())
              exit(1);
          }
          if (has_unknown_value) {
            cerr << "WARNING: [SIM 212-201] RTL produces unknown value 'x' or 'X' on port " 
                 << "m_axis_video" << ", possible cause: There are uninitialized variables in the C design."
                 << endl; 
          }
  
          if (i > 0) {xlx_stream_m_axis_video_size=m_axis_video_V_dest_V_pc_buffer.size();
m_axis_video_V_dest_V_pc_buffer_Copy=m_axis_video_V_dest_V_pc_buffer;
}
        } // end transaction
      } // end file is good
    } // end post check logic bolck
  for (int j = 0, e = xlx_stream_m_axis_video_size; j != e; ++j) {
int xlx_stream_elt_data;
((int*)&xlx_stream_elt_data)[0] = m_axis_video_V_data_V_pc_buffer_Copy[j].to_int64();
((hls::stream<int>*)__xlx_apatb_param_m_axis_video_V_data_V)->write(xlx_stream_elt_data);
char xlx_stream_elt_keep;
((char*)&xlx_stream_elt_keep)[0] = m_axis_video_V_keep_V_pc_buffer_Copy[j].to_int64();
((hls::stream<char>*)__xlx_apatb_param_m_axis_video_V_keep_V)->write(xlx_stream_elt_keep);
char xlx_stream_elt_strb;
((char*)&xlx_stream_elt_strb)[0] = m_axis_video_V_strb_V_pc_buffer_Copy[j].to_int64();
((hls::stream<char>*)__xlx_apatb_param_m_axis_video_V_strb_V)->write(xlx_stream_elt_strb);
char xlx_stream_elt_user;
((char*)&xlx_stream_elt_user)[0] = m_axis_video_V_user_V_pc_buffer_Copy[j].to_uint64();
((hls::stream<char>*)__xlx_apatb_param_m_axis_video_V_user_V)->write(xlx_stream_elt_user);
char xlx_stream_elt_last;
((char*)&xlx_stream_elt_last)[0] = m_axis_video_V_last_V_pc_buffer_Copy[j].to_uint64();
((hls::stream<char>*)__xlx_apatb_param_m_axis_video_V_last_V)->write(xlx_stream_elt_last);
char xlx_stream_elt_id;
((char*)&xlx_stream_elt_id)[0] = m_axis_video_V_id_V_pc_buffer_Copy[j].to_uint64();
((hls::stream<char>*)__xlx_apatb_param_m_axis_video_V_id_V)->write(xlx_stream_elt_id);
char xlx_stream_elt_dest;
((char*)&xlx_stream_elt_dest)[0] = m_axis_video_V_dest_V_pc_buffer_Copy[j].to_uint64();
((hls::stream<char>*)__xlx_apatb_param_m_axis_video_V_dest_V)->write(xlx_stream_elt_dest);
}}

    AESL_transaction_pc++;
    return ;
  }
static unsigned AESL_transaction;
static INTER_TCL_FILE tcl_file(INTER_TCL);
std::vector<char> __xlx_sprintf_buffer(1024);
CodeState = ENTER_WRAPC;
aesl_fh.touch(WRAPC_STREAM_SIZE_IN_s_axis_video_V_data_V);
aesl_fh.touch(WRAPC_STREAM_INGRESS_STATUS_s_axis_video_V_data_V);
aesl_fh.touch(WRAPC_STREAM_SIZE_IN_s_axis_video_V_keep_V);
aesl_fh.touch(WRAPC_STREAM_INGRESS_STATUS_s_axis_video_V_keep_V);
aesl_fh.touch(WRAPC_STREAM_SIZE_IN_s_axis_video_V_strb_V);
aesl_fh.touch(WRAPC_STREAM_INGRESS_STATUS_s_axis_video_V_strb_V);
aesl_fh.touch(WRAPC_STREAM_SIZE_IN_s_axis_video_V_user_V);
aesl_fh.touch(WRAPC_STREAM_INGRESS_STATUS_s_axis_video_V_user_V);
aesl_fh.touch(WRAPC_STREAM_SIZE_IN_s_axis_video_V_last_V);
aesl_fh.touch(WRAPC_STREAM_INGRESS_STATUS_s_axis_video_V_last_V);
aesl_fh.touch(WRAPC_STREAM_SIZE_IN_s_axis_video_V_id_V);
aesl_fh.touch(WRAPC_STREAM_INGRESS_STATUS_s_axis_video_V_id_V);
aesl_fh.touch(WRAPC_STREAM_SIZE_IN_s_axis_video_V_dest_V);
aesl_fh.touch(WRAPC_STREAM_INGRESS_STATUS_s_axis_video_V_dest_V);
aesl_fh.touch(WRAPC_STREAM_SIZE_OUT_m_axis_video_V_data_V);
aesl_fh.touch(WRAPC_STREAM_EGRESS_STATUS_m_axis_video_V_data_V);
aesl_fh.touch(WRAPC_STREAM_SIZE_OUT_m_axis_video_V_keep_V);
aesl_fh.touch(WRAPC_STREAM_EGRESS_STATUS_m_axis_video_V_keep_V);
aesl_fh.touch(WRAPC_STREAM_SIZE_OUT_m_axis_video_V_strb_V);
aesl_fh.touch(WRAPC_STREAM_EGRESS_STATUS_m_axis_video_V_strb_V);
aesl_fh.touch(WRAPC_STREAM_SIZE_OUT_m_axis_video_V_user_V);
aesl_fh.touch(WRAPC_STREAM_EGRESS_STATUS_m_axis_video_V_user_V);
aesl_fh.touch(WRAPC_STREAM_SIZE_OUT_m_axis_video_V_last_V);
aesl_fh.touch(WRAPC_STREAM_EGRESS_STATUS_m_axis_video_V_last_V);
aesl_fh.touch(WRAPC_STREAM_SIZE_OUT_m_axis_video_V_id_V);
aesl_fh.touch(WRAPC_STREAM_EGRESS_STATUS_m_axis_video_V_id_V);
aesl_fh.touch(WRAPC_STREAM_SIZE_OUT_m_axis_video_V_dest_V);
aesl_fh.touch(WRAPC_STREAM_EGRESS_STATUS_m_axis_video_V_dest_V);
CodeState = DUMP_INPUTS;
// data
std::vector<short> __xlx_apatb_param_s_axis_video_V_data_V_stream_buf;
{
  while (!((hls::stream<short>*)__xlx_apatb_param_s_axis_video_V_data_V)->empty())
    __xlx_apatb_param_s_axis_video_V_data_V_stream_buf.push_back(((hls::stream<short>*)__xlx_apatb_param_s_axis_video_V_data_V)->read());
  for (int i = 0; i < __xlx_apatb_param_s_axis_video_V_data_V_stream_buf.size(); ++i)
    ((hls::stream<short>*)__xlx_apatb_param_s_axis_video_V_data_V)->write(__xlx_apatb_param_s_axis_video_V_data_V_stream_buf[i]);
  }
long __xlx_apatb_param_s_axis_video_stream_buf_size = ((hls::stream<short>*)__xlx_apatb_param_s_axis_video_V_data_V)->size();
// keep
std::vector<char> __xlx_apatb_param_s_axis_video_V_keep_V_stream_buf;
{
  while (!((hls::stream<char>*)__xlx_apatb_param_s_axis_video_V_keep_V)->empty())
    __xlx_apatb_param_s_axis_video_V_keep_V_stream_buf.push_back(((hls::stream<char>*)__xlx_apatb_param_s_axis_video_V_keep_V)->read());
  for (int i = 0; i < __xlx_apatb_param_s_axis_video_V_keep_V_stream_buf.size(); ++i)
    ((hls::stream<char>*)__xlx_apatb_param_s_axis_video_V_keep_V)->write(__xlx_apatb_param_s_axis_video_V_keep_V_stream_buf[i]);
  }
// strb
std::vector<char> __xlx_apatb_param_s_axis_video_V_strb_V_stream_buf;
{
  while (!((hls::stream<char>*)__xlx_apatb_param_s_axis_video_V_strb_V)->empty())
    __xlx_apatb_param_s_axis_video_V_strb_V_stream_buf.push_back(((hls::stream<char>*)__xlx_apatb_param_s_axis_video_V_strb_V)->read());
  for (int i = 0; i < __xlx_apatb_param_s_axis_video_V_strb_V_stream_buf.size(); ++i)
    ((hls::stream<char>*)__xlx_apatb_param_s_axis_video_V_strb_V)->write(__xlx_apatb_param_s_axis_video_V_strb_V_stream_buf[i]);
  }
// user
std::vector<char> __xlx_apatb_param_s_axis_video_V_user_V_stream_buf;
{
  while (!((hls::stream<char>*)__xlx_apatb_param_s_axis_video_V_user_V)->empty())
    __xlx_apatb_param_s_axis_video_V_user_V_stream_buf.push_back(((hls::stream<char>*)__xlx_apatb_param_s_axis_video_V_user_V)->read());
  for (int i = 0; i < __xlx_apatb_param_s_axis_video_V_user_V_stream_buf.size(); ++i)
    ((hls::stream<char>*)__xlx_apatb_param_s_axis_video_V_user_V)->write(__xlx_apatb_param_s_axis_video_V_user_V_stream_buf[i]);
  }
// last
std::vector<char> __xlx_apatb_param_s_axis_video_V_last_V_stream_buf;
{
  while (!((hls::stream<char>*)__xlx_apatb_param_s_axis_video_V_last_V)->empty())
    __xlx_apatb_param_s_axis_video_V_last_V_stream_buf.push_back(((hls::stream<char>*)__xlx_apatb_param_s_axis_video_V_last_V)->read());
  for (int i = 0; i < __xlx_apatb_param_s_axis_video_V_last_V_stream_buf.size(); ++i)
    ((hls::stream<char>*)__xlx_apatb_param_s_axis_video_V_last_V)->write(__xlx_apatb_param_s_axis_video_V_last_V_stream_buf[i]);
  }
// id
std::vector<char> __xlx_apatb_param_s_axis_video_V_id_V_stream_buf;
{
  while (!((hls::stream<char>*)__xlx_apatb_param_s_axis_video_V_id_V)->empty())
    __xlx_apatb_param_s_axis_video_V_id_V_stream_buf.push_back(((hls::stream<char>*)__xlx_apatb_param_s_axis_video_V_id_V)->read());
  for (int i = 0; i < __xlx_apatb_param_s_axis_video_V_id_V_stream_buf.size(); ++i)
    ((hls::stream<char>*)__xlx_apatb_param_s_axis_video_V_id_V)->write(__xlx_apatb_param_s_axis_video_V_id_V_stream_buf[i]);
  }
// dest
std::vector<char> __xlx_apatb_param_s_axis_video_V_dest_V_stream_buf;
{
  while (!((hls::stream<char>*)__xlx_apatb_param_s_axis_video_V_dest_V)->empty())
    __xlx_apatb_param_s_axis_video_V_dest_V_stream_buf.push_back(((hls::stream<char>*)__xlx_apatb_param_s_axis_video_V_dest_V)->read());
  for (int i = 0; i < __xlx_apatb_param_s_axis_video_V_dest_V_stream_buf.size(); ++i)
    ((hls::stream<char>*)__xlx_apatb_param_s_axis_video_V_dest_V)->write(__xlx_apatb_param_s_axis_video_V_dest_V_stream_buf[i]);
  }
// data
std::vector<int> __xlx_apatb_param_m_axis_video_V_data_V_stream_buf;
long __xlx_apatb_param_m_axis_video_stream_buf_size = ((hls::stream<int>*)__xlx_apatb_param_m_axis_video_V_data_V)->size();
// keep
std::vector<char> __xlx_apatb_param_m_axis_video_V_keep_V_stream_buf;
// strb
std::vector<char> __xlx_apatb_param_m_axis_video_V_strb_V_stream_buf;
// user
std::vector<char> __xlx_apatb_param_m_axis_video_V_user_V_stream_buf;
// last
std::vector<char> __xlx_apatb_param_m_axis_video_V_last_V_stream_buf;
// id
std::vector<char> __xlx_apatb_param_m_axis_video_V_id_V_stream_buf;
// dest
std::vector<char> __xlx_apatb_param_m_axis_video_V_dest_V_stream_buf;
unsigned __xlx_offset_byte_param_gamma_lut = 0;
#ifdef USE_BINARY_TV_FILE
{
aesl_fh.touch(AUTOTB_TVIN_gamma_lut, 'b');
transaction<8> tr(768);
  __xlx_offset_byte_param_gamma_lut = 0*1;
  if (__xlx_apatb_param_gamma_lut) {
tr.import<1>((char*)__xlx_apatb_param_gamma_lut, 768, 0);
  }
aesl_fh.write(AUTOTB_TVIN_gamma_lut, tr.p, tr.tbytes);
}

  tcl_file.set_num(768, &tcl_file.gamma_lut_depth);
#else
// print gamma_lut Transactions
{
aesl_fh.write(AUTOTB_TVIN_gamma_lut, begin_str(AESL_transaction));
{
  __xlx_offset_byte_param_gamma_lut = 0*1;
if (__xlx_apatb_param_gamma_lut) {
for (size_t i = 0; i < 768; ++i) {
unsigned char *pos = (unsigned char*)__xlx_apatb_param_gamma_lut + i * 1;
std::string s = formatData(pos, 8);
aesl_fh.write(AUTOTB_TVIN_gamma_lut, s);
}
}
}

  tcl_file.set_num(768, &tcl_file.gamma_lut_depth);
aesl_fh.write(AUTOTB_TVIN_gamma_lut, end_str());
}

#endif
// print width Transactions
{
aesl_fh.write(AUTOTB_TVIN_width, begin_str(AESL_transaction));
{
auto *pos = (unsigned char*)&__xlx_apatb_param_width;
aesl_fh.write(AUTOTB_TVIN_width, formatData(pos, 16));
}
  tcl_file.set_num(1, &tcl_file.width_depth);
aesl_fh.write(AUTOTB_TVIN_width, end_str());
}

// print height Transactions
{
aesl_fh.write(AUTOTB_TVIN_height, begin_str(AESL_transaction));
{
auto *pos = (unsigned char*)&__xlx_apatb_param_height;
aesl_fh.write(AUTOTB_TVIN_height, formatData(pos, 16));
}
  tcl_file.set_num(1, &tcl_file.height_depth);
aesl_fh.write(AUTOTB_TVIN_height, end_str());
}

// print rgain Transactions
{
aesl_fh.write(AUTOTB_TVIN_rgain, begin_str(AESL_transaction));
{
auto *pos = (unsigned char*)&__xlx_apatb_param_rgain;
aesl_fh.write(AUTOTB_TVIN_rgain, formatData(pos, 16));
}
  tcl_file.set_num(1, &tcl_file.rgain_depth);
aesl_fh.write(AUTOTB_TVIN_rgain, end_str());
}

// print bgain Transactions
{
aesl_fh.write(AUTOTB_TVIN_bgain, begin_str(AESL_transaction));
{
auto *pos = (unsigned char*)&__xlx_apatb_param_bgain;
aesl_fh.write(AUTOTB_TVIN_bgain, formatData(pos, 16));
}
  tcl_file.set_num(1, &tcl_file.bgain_depth);
aesl_fh.write(AUTOTB_TVIN_bgain, end_str());
}

// print mode_reg Transactions
{
aesl_fh.write(AUTOTB_TVIN_mode_reg, begin_str(AESL_transaction));
{
auto *pos = (unsigned char*)&__xlx_apatb_param_mode_reg;
aesl_fh.write(AUTOTB_TVIN_mode_reg, formatData(pos, 8));
}
  tcl_file.set_num(1, &tcl_file.mode_reg_depth);
aesl_fh.write(AUTOTB_TVIN_mode_reg, end_str());
}

// print pawb Transactions
{
aesl_fh.write(AUTOTB_TVIN_pawb, begin_str(AESL_transaction));
{
auto *pos = (unsigned char*)&__xlx_apatb_param_pawb;
aesl_fh.write(AUTOTB_TVIN_pawb, formatData(pos, 16));
}
  tcl_file.set_num(1, &tcl_file.pawb_depth);
aesl_fh.write(AUTOTB_TVIN_pawb, end_str());
}

CodeState = CALL_C_DUT;
ISPPipeline_accel_hw_stub_wrapper(__xlx_apatb_param_width, __xlx_apatb_param_height, __xlx_apatb_param_s_axis_video_V_data_V, __xlx_apatb_param_s_axis_video_V_keep_V, __xlx_apatb_param_s_axis_video_V_strb_V, __xlx_apatb_param_s_axis_video_V_user_V, __xlx_apatb_param_s_axis_video_V_last_V, __xlx_apatb_param_s_axis_video_V_id_V, __xlx_apatb_param_s_axis_video_V_dest_V, __xlx_apatb_param_m_axis_video_V_data_V, __xlx_apatb_param_m_axis_video_V_keep_V, __xlx_apatb_param_m_axis_video_V_strb_V, __xlx_apatb_param_m_axis_video_V_user_V, __xlx_apatb_param_m_axis_video_V_last_V, __xlx_apatb_param_m_axis_video_V_id_V, __xlx_apatb_param_m_axis_video_V_dest_V, __xlx_apatb_param_rgain, __xlx_apatb_param_bgain, __xlx_apatb_param_gamma_lut, __xlx_apatb_param_mode_reg, __xlx_apatb_param_pawb);
CodeState = DUMP_OUTPUTS;
long __xlx_apatb_param_s_axis_video_stream_buf_final_size = __xlx_apatb_param_s_axis_video_stream_buf_size - ((hls::stream<short>*)__xlx_apatb_param_s_axis_video_V_data_V)->size();
aesl_fh.write(AUTOTB_TVIN_s_axis_video_V_data_V, begin_str(AESL_transaction));
aesl_fh.write(AUTOTB_TVIN_s_axis_video_V_keep_V, begin_str(AESL_transaction));
aesl_fh.write(AUTOTB_TVIN_s_axis_video_V_strb_V, begin_str(AESL_transaction));
aesl_fh.write(AUTOTB_TVIN_s_axis_video_V_user_V, begin_str(AESL_transaction));
aesl_fh.write(AUTOTB_TVIN_s_axis_video_V_last_V, begin_str(AESL_transaction));
aesl_fh.write(AUTOTB_TVIN_s_axis_video_V_id_V, begin_str(AESL_transaction));
aesl_fh.write(AUTOTB_TVIN_s_axis_video_V_dest_V, begin_str(AESL_transaction));
for (int i = 0; i < __xlx_apatb_param_s_axis_video_stream_buf_final_size; ++i) {
  std::string s;
s = formatData((unsigned char*)(__xlx_apatb_param_s_axis_video_V_data_V_stream_buf.data()+i), 16);
aesl_fh.write(AUTOTB_TVIN_s_axis_video_V_data_V, s);
s = formatData((unsigned char*)(__xlx_apatb_param_s_axis_video_V_keep_V_stream_buf.data()+i), 2);
aesl_fh.write(AUTOTB_TVIN_s_axis_video_V_keep_V, s);
s = formatData((unsigned char*)(__xlx_apatb_param_s_axis_video_V_strb_V_stream_buf.data()+i), 2);
aesl_fh.write(AUTOTB_TVIN_s_axis_video_V_strb_V, s);
s = formatData((unsigned char*)(__xlx_apatb_param_s_axis_video_V_user_V_stream_buf.data()+i), 1);
aesl_fh.write(AUTOTB_TVIN_s_axis_video_V_user_V, s);
s = formatData((unsigned char*)(__xlx_apatb_param_s_axis_video_V_last_V_stream_buf.data()+i), 1);
aesl_fh.write(AUTOTB_TVIN_s_axis_video_V_last_V, s);
s = formatData((unsigned char*)(__xlx_apatb_param_s_axis_video_V_id_V_stream_buf.data()+i), 1);
aesl_fh.write(AUTOTB_TVIN_s_axis_video_V_id_V, s);
s = formatData((unsigned char*)(__xlx_apatb_param_s_axis_video_V_dest_V_stream_buf.data()+i), 1);
aesl_fh.write(AUTOTB_TVIN_s_axis_video_V_dest_V, s);
}
tcl_file.set_num(__xlx_apatb_param_s_axis_video_stream_buf_final_size, &tcl_file.s_axis_video_V_data_V_depth);
aesl_fh.write(AUTOTB_TVIN_s_axis_video_V_data_V, end_str());
tcl_file.set_num(__xlx_apatb_param_s_axis_video_stream_buf_final_size, &tcl_file.s_axis_video_V_keep_V_depth);
aesl_fh.write(AUTOTB_TVIN_s_axis_video_V_keep_V, end_str());
tcl_file.set_num(__xlx_apatb_param_s_axis_video_stream_buf_final_size, &tcl_file.s_axis_video_V_strb_V_depth);
aesl_fh.write(AUTOTB_TVIN_s_axis_video_V_strb_V, end_str());
tcl_file.set_num(__xlx_apatb_param_s_axis_video_stream_buf_final_size, &tcl_file.s_axis_video_V_user_V_depth);
aesl_fh.write(AUTOTB_TVIN_s_axis_video_V_user_V, end_str());
tcl_file.set_num(__xlx_apatb_param_s_axis_video_stream_buf_final_size, &tcl_file.s_axis_video_V_last_V_depth);
aesl_fh.write(AUTOTB_TVIN_s_axis_video_V_last_V, end_str());
tcl_file.set_num(__xlx_apatb_param_s_axis_video_stream_buf_final_size, &tcl_file.s_axis_video_V_id_V_depth);
aesl_fh.write(AUTOTB_TVIN_s_axis_video_V_id_V, end_str());
tcl_file.set_num(__xlx_apatb_param_s_axis_video_stream_buf_final_size, &tcl_file.s_axis_video_V_dest_V_depth);
aesl_fh.write(AUTOTB_TVIN_s_axis_video_V_dest_V, end_str());

// dump stream ingress status to file

// dump stream ingress status to file
{
aesl_fh.write(WRAPC_STREAM_INGRESS_STATUS_s_axis_video_V_data_V, begin_str(AESL_transaction));
if (__xlx_apatb_param_s_axis_video_stream_buf_final_size > 0) {
  long s_axis_video_V_data_V_stream_ingress_size = __xlx_apatb_param_s_axis_video_stream_buf_size;
sprintf(__xlx_sprintf_buffer.data(), "%d\n", s_axis_video_V_data_V_stream_ingress_size);
 aesl_fh.write(WRAPC_STREAM_INGRESS_STATUS_s_axis_video_V_data_V, __xlx_sprintf_buffer.data());
  for (int j = 0, e = __xlx_apatb_param_s_axis_video_stream_buf_final_size; j != e; j++) {
    s_axis_video_V_data_V_stream_ingress_size--;
sprintf(__xlx_sprintf_buffer.data(), "%d\n", s_axis_video_V_data_V_stream_ingress_size);
 aesl_fh.write(WRAPC_STREAM_INGRESS_STATUS_s_axis_video_V_data_V, __xlx_sprintf_buffer.data());
  }
} else {
  long s_axis_video_V_data_V_stream_ingress_size = 0;
sprintf(__xlx_sprintf_buffer.data(), "%d\n", s_axis_video_V_data_V_stream_ingress_size);
 aesl_fh.write(WRAPC_STREAM_INGRESS_STATUS_s_axis_video_V_data_V, __xlx_sprintf_buffer.data());
}
aesl_fh.write(WRAPC_STREAM_INGRESS_STATUS_s_axis_video_V_data_V, end_str());
}

// dump stream ingress status to file
{
aesl_fh.write(WRAPC_STREAM_INGRESS_STATUS_s_axis_video_V_keep_V, begin_str(AESL_transaction));
if (__xlx_apatb_param_s_axis_video_stream_buf_final_size > 0) {
  long s_axis_video_V_keep_V_stream_ingress_size = __xlx_apatb_param_s_axis_video_stream_buf_size;
sprintf(__xlx_sprintf_buffer.data(), "%d\n", s_axis_video_V_keep_V_stream_ingress_size);
 aesl_fh.write(WRAPC_STREAM_INGRESS_STATUS_s_axis_video_V_keep_V, __xlx_sprintf_buffer.data());
  for (int j = 0, e = __xlx_apatb_param_s_axis_video_stream_buf_final_size; j != e; j++) {
    s_axis_video_V_keep_V_stream_ingress_size--;
sprintf(__xlx_sprintf_buffer.data(), "%d\n", s_axis_video_V_keep_V_stream_ingress_size);
 aesl_fh.write(WRAPC_STREAM_INGRESS_STATUS_s_axis_video_V_keep_V, __xlx_sprintf_buffer.data());
  }
} else {
  long s_axis_video_V_keep_V_stream_ingress_size = 0;
sprintf(__xlx_sprintf_buffer.data(), "%d\n", s_axis_video_V_keep_V_stream_ingress_size);
 aesl_fh.write(WRAPC_STREAM_INGRESS_STATUS_s_axis_video_V_keep_V, __xlx_sprintf_buffer.data());
}
aesl_fh.write(WRAPC_STREAM_INGRESS_STATUS_s_axis_video_V_keep_V, end_str());
}

// dump stream ingress status to file
{
aesl_fh.write(WRAPC_STREAM_INGRESS_STATUS_s_axis_video_V_strb_V, begin_str(AESL_transaction));
if (__xlx_apatb_param_s_axis_video_stream_buf_final_size > 0) {
  long s_axis_video_V_strb_V_stream_ingress_size = __xlx_apatb_param_s_axis_video_stream_buf_size;
sprintf(__xlx_sprintf_buffer.data(), "%d\n", s_axis_video_V_strb_V_stream_ingress_size);
 aesl_fh.write(WRAPC_STREAM_INGRESS_STATUS_s_axis_video_V_strb_V, __xlx_sprintf_buffer.data());
  for (int j = 0, e = __xlx_apatb_param_s_axis_video_stream_buf_final_size; j != e; j++) {
    s_axis_video_V_strb_V_stream_ingress_size--;
sprintf(__xlx_sprintf_buffer.data(), "%d\n", s_axis_video_V_strb_V_stream_ingress_size);
 aesl_fh.write(WRAPC_STREAM_INGRESS_STATUS_s_axis_video_V_strb_V, __xlx_sprintf_buffer.data());
  }
} else {
  long s_axis_video_V_strb_V_stream_ingress_size = 0;
sprintf(__xlx_sprintf_buffer.data(), "%d\n", s_axis_video_V_strb_V_stream_ingress_size);
 aesl_fh.write(WRAPC_STREAM_INGRESS_STATUS_s_axis_video_V_strb_V, __xlx_sprintf_buffer.data());
}
aesl_fh.write(WRAPC_STREAM_INGRESS_STATUS_s_axis_video_V_strb_V, end_str());
}

// dump stream ingress status to file
{
aesl_fh.write(WRAPC_STREAM_INGRESS_STATUS_s_axis_video_V_user_V, begin_str(AESL_transaction));
if (__xlx_apatb_param_s_axis_video_stream_buf_final_size > 0) {
  long s_axis_video_V_user_V_stream_ingress_size = __xlx_apatb_param_s_axis_video_stream_buf_size;
sprintf(__xlx_sprintf_buffer.data(), "%d\n", s_axis_video_V_user_V_stream_ingress_size);
 aesl_fh.write(WRAPC_STREAM_INGRESS_STATUS_s_axis_video_V_user_V, __xlx_sprintf_buffer.data());
  for (int j = 0, e = __xlx_apatb_param_s_axis_video_stream_buf_final_size; j != e; j++) {
    s_axis_video_V_user_V_stream_ingress_size--;
sprintf(__xlx_sprintf_buffer.data(), "%d\n", s_axis_video_V_user_V_stream_ingress_size);
 aesl_fh.write(WRAPC_STREAM_INGRESS_STATUS_s_axis_video_V_user_V, __xlx_sprintf_buffer.data());
  }
} else {
  long s_axis_video_V_user_V_stream_ingress_size = 0;
sprintf(__xlx_sprintf_buffer.data(), "%d\n", s_axis_video_V_user_V_stream_ingress_size);
 aesl_fh.write(WRAPC_STREAM_INGRESS_STATUS_s_axis_video_V_user_V, __xlx_sprintf_buffer.data());
}
aesl_fh.write(WRAPC_STREAM_INGRESS_STATUS_s_axis_video_V_user_V, end_str());
}

// dump stream ingress status to file
{
aesl_fh.write(WRAPC_STREAM_INGRESS_STATUS_s_axis_video_V_last_V, begin_str(AESL_transaction));
if (__xlx_apatb_param_s_axis_video_stream_buf_final_size > 0) {
  long s_axis_video_V_last_V_stream_ingress_size = __xlx_apatb_param_s_axis_video_stream_buf_size;
sprintf(__xlx_sprintf_buffer.data(), "%d\n", s_axis_video_V_last_V_stream_ingress_size);
 aesl_fh.write(WRAPC_STREAM_INGRESS_STATUS_s_axis_video_V_last_V, __xlx_sprintf_buffer.data());
  for (int j = 0, e = __xlx_apatb_param_s_axis_video_stream_buf_final_size; j != e; j++) {
    s_axis_video_V_last_V_stream_ingress_size--;
sprintf(__xlx_sprintf_buffer.data(), "%d\n", s_axis_video_V_last_V_stream_ingress_size);
 aesl_fh.write(WRAPC_STREAM_INGRESS_STATUS_s_axis_video_V_last_V, __xlx_sprintf_buffer.data());
  }
} else {
  long s_axis_video_V_last_V_stream_ingress_size = 0;
sprintf(__xlx_sprintf_buffer.data(), "%d\n", s_axis_video_V_last_V_stream_ingress_size);
 aesl_fh.write(WRAPC_STREAM_INGRESS_STATUS_s_axis_video_V_last_V, __xlx_sprintf_buffer.data());
}
aesl_fh.write(WRAPC_STREAM_INGRESS_STATUS_s_axis_video_V_last_V, end_str());
}

// dump stream ingress status to file
{
aesl_fh.write(WRAPC_STREAM_INGRESS_STATUS_s_axis_video_V_id_V, begin_str(AESL_transaction));
if (__xlx_apatb_param_s_axis_video_stream_buf_final_size > 0) {
  long s_axis_video_V_id_V_stream_ingress_size = __xlx_apatb_param_s_axis_video_stream_buf_size;
sprintf(__xlx_sprintf_buffer.data(), "%d\n", s_axis_video_V_id_V_stream_ingress_size);
 aesl_fh.write(WRAPC_STREAM_INGRESS_STATUS_s_axis_video_V_id_V, __xlx_sprintf_buffer.data());
  for (int j = 0, e = __xlx_apatb_param_s_axis_video_stream_buf_final_size; j != e; j++) {
    s_axis_video_V_id_V_stream_ingress_size--;
sprintf(__xlx_sprintf_buffer.data(), "%d\n", s_axis_video_V_id_V_stream_ingress_size);
 aesl_fh.write(WRAPC_STREAM_INGRESS_STATUS_s_axis_video_V_id_V, __xlx_sprintf_buffer.data());
  }
} else {
  long s_axis_video_V_id_V_stream_ingress_size = 0;
sprintf(__xlx_sprintf_buffer.data(), "%d\n", s_axis_video_V_id_V_stream_ingress_size);
 aesl_fh.write(WRAPC_STREAM_INGRESS_STATUS_s_axis_video_V_id_V, __xlx_sprintf_buffer.data());
}
aesl_fh.write(WRAPC_STREAM_INGRESS_STATUS_s_axis_video_V_id_V, end_str());
}

// dump stream ingress status to file
{
aesl_fh.write(WRAPC_STREAM_INGRESS_STATUS_s_axis_video_V_dest_V, begin_str(AESL_transaction));
if (__xlx_apatb_param_s_axis_video_stream_buf_final_size > 0) {
  long s_axis_video_V_dest_V_stream_ingress_size = __xlx_apatb_param_s_axis_video_stream_buf_size;
sprintf(__xlx_sprintf_buffer.data(), "%d\n", s_axis_video_V_dest_V_stream_ingress_size);
 aesl_fh.write(WRAPC_STREAM_INGRESS_STATUS_s_axis_video_V_dest_V, __xlx_sprintf_buffer.data());
  for (int j = 0, e = __xlx_apatb_param_s_axis_video_stream_buf_final_size; j != e; j++) {
    s_axis_video_V_dest_V_stream_ingress_size--;
sprintf(__xlx_sprintf_buffer.data(), "%d\n", s_axis_video_V_dest_V_stream_ingress_size);
 aesl_fh.write(WRAPC_STREAM_INGRESS_STATUS_s_axis_video_V_dest_V, __xlx_sprintf_buffer.data());
  }
} else {
  long s_axis_video_V_dest_V_stream_ingress_size = 0;
sprintf(__xlx_sprintf_buffer.data(), "%d\n", s_axis_video_V_dest_V_stream_ingress_size);
 aesl_fh.write(WRAPC_STREAM_INGRESS_STATUS_s_axis_video_V_dest_V, __xlx_sprintf_buffer.data());
}
aesl_fh.write(WRAPC_STREAM_INGRESS_STATUS_s_axis_video_V_dest_V, end_str());
}
{
aesl_fh.write(WRAPC_STREAM_SIZE_IN_s_axis_video_V_data_V, begin_str(AESL_transaction));
sprintf(__xlx_sprintf_buffer.data(), "%d\n", __xlx_apatb_param_s_axis_video_stream_buf_final_size);
 aesl_fh.write(WRAPC_STREAM_SIZE_IN_s_axis_video_V_data_V, __xlx_sprintf_buffer.data());
aesl_fh.write(WRAPC_STREAM_SIZE_IN_s_axis_video_V_data_V, end_str());
}
{
aesl_fh.write(WRAPC_STREAM_SIZE_IN_s_axis_video_V_keep_V, begin_str(AESL_transaction));
sprintf(__xlx_sprintf_buffer.data(), "%d\n", __xlx_apatb_param_s_axis_video_stream_buf_final_size);
 aesl_fh.write(WRAPC_STREAM_SIZE_IN_s_axis_video_V_keep_V, __xlx_sprintf_buffer.data());
aesl_fh.write(WRAPC_STREAM_SIZE_IN_s_axis_video_V_keep_V, end_str());
}
{
aesl_fh.write(WRAPC_STREAM_SIZE_IN_s_axis_video_V_strb_V, begin_str(AESL_transaction));
sprintf(__xlx_sprintf_buffer.data(), "%d\n", __xlx_apatb_param_s_axis_video_stream_buf_final_size);
 aesl_fh.write(WRAPC_STREAM_SIZE_IN_s_axis_video_V_strb_V, __xlx_sprintf_buffer.data());
aesl_fh.write(WRAPC_STREAM_SIZE_IN_s_axis_video_V_strb_V, end_str());
}
{
aesl_fh.write(WRAPC_STREAM_SIZE_IN_s_axis_video_V_user_V, begin_str(AESL_transaction));
sprintf(__xlx_sprintf_buffer.data(), "%d\n", __xlx_apatb_param_s_axis_video_stream_buf_final_size);
 aesl_fh.write(WRAPC_STREAM_SIZE_IN_s_axis_video_V_user_V, __xlx_sprintf_buffer.data());
aesl_fh.write(WRAPC_STREAM_SIZE_IN_s_axis_video_V_user_V, end_str());
}
{
aesl_fh.write(WRAPC_STREAM_SIZE_IN_s_axis_video_V_last_V, begin_str(AESL_transaction));
sprintf(__xlx_sprintf_buffer.data(), "%d\n", __xlx_apatb_param_s_axis_video_stream_buf_final_size);
 aesl_fh.write(WRAPC_STREAM_SIZE_IN_s_axis_video_V_last_V, __xlx_sprintf_buffer.data());
aesl_fh.write(WRAPC_STREAM_SIZE_IN_s_axis_video_V_last_V, end_str());
}
{
aesl_fh.write(WRAPC_STREAM_SIZE_IN_s_axis_video_V_id_V, begin_str(AESL_transaction));
sprintf(__xlx_sprintf_buffer.data(), "%d\n", __xlx_apatb_param_s_axis_video_stream_buf_final_size);
 aesl_fh.write(WRAPC_STREAM_SIZE_IN_s_axis_video_V_id_V, __xlx_sprintf_buffer.data());
aesl_fh.write(WRAPC_STREAM_SIZE_IN_s_axis_video_V_id_V, end_str());
}
{
aesl_fh.write(WRAPC_STREAM_SIZE_IN_s_axis_video_V_dest_V, begin_str(AESL_transaction));
sprintf(__xlx_sprintf_buffer.data(), "%d\n", __xlx_apatb_param_s_axis_video_stream_buf_final_size);
 aesl_fh.write(WRAPC_STREAM_SIZE_IN_s_axis_video_V_dest_V, __xlx_sprintf_buffer.data());
aesl_fh.write(WRAPC_STREAM_SIZE_IN_s_axis_video_V_dest_V, end_str());
}

//********************** dump C output stream *******************
long __xlx_apatb_param_m_axis_video_stream_buf_final_size = ((hls::stream<int>*)__xlx_apatb_param_m_axis_video_V_data_V)->size() - __xlx_apatb_param_m_axis_video_stream_buf_size;
{
  while (!((hls::stream<int>*)__xlx_apatb_param_m_axis_video_V_data_V)->empty())
    __xlx_apatb_param_m_axis_video_V_data_V_stream_buf.push_back(((hls::stream<int>*)__xlx_apatb_param_m_axis_video_V_data_V)->read());
  for (int i = 0; i < __xlx_apatb_param_m_axis_video_V_data_V_stream_buf.size(); ++i)
    ((hls::stream<int>*)__xlx_apatb_param_m_axis_video_V_data_V)->write(__xlx_apatb_param_m_axis_video_V_data_V_stream_buf[i]);
  }
{
  while (!((hls::stream<char>*)__xlx_apatb_param_m_axis_video_V_keep_V)->empty())
    __xlx_apatb_param_m_axis_video_V_keep_V_stream_buf.push_back(((hls::stream<char>*)__xlx_apatb_param_m_axis_video_V_keep_V)->read());
  for (int i = 0; i < __xlx_apatb_param_m_axis_video_V_keep_V_stream_buf.size(); ++i)
    ((hls::stream<char>*)__xlx_apatb_param_m_axis_video_V_keep_V)->write(__xlx_apatb_param_m_axis_video_V_keep_V_stream_buf[i]);
  }
{
  while (!((hls::stream<char>*)__xlx_apatb_param_m_axis_video_V_strb_V)->empty())
    __xlx_apatb_param_m_axis_video_V_strb_V_stream_buf.push_back(((hls::stream<char>*)__xlx_apatb_param_m_axis_video_V_strb_V)->read());
  for (int i = 0; i < __xlx_apatb_param_m_axis_video_V_strb_V_stream_buf.size(); ++i)
    ((hls::stream<char>*)__xlx_apatb_param_m_axis_video_V_strb_V)->write(__xlx_apatb_param_m_axis_video_V_strb_V_stream_buf[i]);
  }
{
  while (!((hls::stream<char>*)__xlx_apatb_param_m_axis_video_V_user_V)->empty())
    __xlx_apatb_param_m_axis_video_V_user_V_stream_buf.push_back(((hls::stream<char>*)__xlx_apatb_param_m_axis_video_V_user_V)->read());
  for (int i = 0; i < __xlx_apatb_param_m_axis_video_V_user_V_stream_buf.size(); ++i)
    ((hls::stream<char>*)__xlx_apatb_param_m_axis_video_V_user_V)->write(__xlx_apatb_param_m_axis_video_V_user_V_stream_buf[i]);
  }
{
  while (!((hls::stream<char>*)__xlx_apatb_param_m_axis_video_V_last_V)->empty())
    __xlx_apatb_param_m_axis_video_V_last_V_stream_buf.push_back(((hls::stream<char>*)__xlx_apatb_param_m_axis_video_V_last_V)->read());
  for (int i = 0; i < __xlx_apatb_param_m_axis_video_V_last_V_stream_buf.size(); ++i)
    ((hls::stream<char>*)__xlx_apatb_param_m_axis_video_V_last_V)->write(__xlx_apatb_param_m_axis_video_V_last_V_stream_buf[i]);
  }
{
  while (!((hls::stream<char>*)__xlx_apatb_param_m_axis_video_V_id_V)->empty())
    __xlx_apatb_param_m_axis_video_V_id_V_stream_buf.push_back(((hls::stream<char>*)__xlx_apatb_param_m_axis_video_V_id_V)->read());
  for (int i = 0; i < __xlx_apatb_param_m_axis_video_V_id_V_stream_buf.size(); ++i)
    ((hls::stream<char>*)__xlx_apatb_param_m_axis_video_V_id_V)->write(__xlx_apatb_param_m_axis_video_V_id_V_stream_buf[i]);
  }
{
  while (!((hls::stream<char>*)__xlx_apatb_param_m_axis_video_V_dest_V)->empty())
    __xlx_apatb_param_m_axis_video_V_dest_V_stream_buf.push_back(((hls::stream<char>*)__xlx_apatb_param_m_axis_video_V_dest_V)->read());
  for (int i = 0; i < __xlx_apatb_param_m_axis_video_V_dest_V_stream_buf.size(); ++i)
    ((hls::stream<char>*)__xlx_apatb_param_m_axis_video_V_dest_V)->write(__xlx_apatb_param_m_axis_video_V_dest_V_stream_buf[i]);
  }
aesl_fh.write(AUTOTB_TVOUT_m_axis_video_V_data_V, begin_str(AESL_transaction));
aesl_fh.write(AUTOTB_TVOUT_m_axis_video_V_keep_V, begin_str(AESL_transaction));
aesl_fh.write(AUTOTB_TVOUT_m_axis_video_V_strb_V, begin_str(AESL_transaction));
aesl_fh.write(AUTOTB_TVOUT_m_axis_video_V_user_V, begin_str(AESL_transaction));
aesl_fh.write(AUTOTB_TVOUT_m_axis_video_V_last_V, begin_str(AESL_transaction));
aesl_fh.write(AUTOTB_TVOUT_m_axis_video_V_id_V, begin_str(AESL_transaction));
aesl_fh.write(AUTOTB_TVOUT_m_axis_video_V_dest_V, begin_str(AESL_transaction));
for (int i = 0; i < __xlx_apatb_param_m_axis_video_stream_buf_final_size; ++i) {
  std::string s;
s = formatData((unsigned char*)(__xlx_apatb_param_m_axis_video_V_data_V_stream_buf.data()+__xlx_apatb_param_m_axis_video_stream_buf_size+i), 24);
aesl_fh.write(AUTOTB_TVOUT_m_axis_video_V_data_V, s);
s = formatData((unsigned char*)(__xlx_apatb_param_m_axis_video_V_keep_V_stream_buf.data()+__xlx_apatb_param_m_axis_video_stream_buf_size+i), 3);
aesl_fh.write(AUTOTB_TVOUT_m_axis_video_V_keep_V, s);
s = formatData((unsigned char*)(__xlx_apatb_param_m_axis_video_V_strb_V_stream_buf.data()+__xlx_apatb_param_m_axis_video_stream_buf_size+i), 3);
aesl_fh.write(AUTOTB_TVOUT_m_axis_video_V_strb_V, s);
s = formatData((unsigned char*)(__xlx_apatb_param_m_axis_video_V_user_V_stream_buf.data()+__xlx_apatb_param_m_axis_video_stream_buf_size+i), 1);
aesl_fh.write(AUTOTB_TVOUT_m_axis_video_V_user_V, s);
s = formatData((unsigned char*)(__xlx_apatb_param_m_axis_video_V_last_V_stream_buf.data()+__xlx_apatb_param_m_axis_video_stream_buf_size+i), 1);
aesl_fh.write(AUTOTB_TVOUT_m_axis_video_V_last_V, s);
s = formatData((unsigned char*)(__xlx_apatb_param_m_axis_video_V_id_V_stream_buf.data()+__xlx_apatb_param_m_axis_video_stream_buf_size+i), 1);
aesl_fh.write(AUTOTB_TVOUT_m_axis_video_V_id_V, s);
s = formatData((unsigned char*)(__xlx_apatb_param_m_axis_video_V_dest_V_stream_buf.data()+__xlx_apatb_param_m_axis_video_stream_buf_size+i), 1);
aesl_fh.write(AUTOTB_TVOUT_m_axis_video_V_dest_V, s);
}
tcl_file.set_num(__xlx_apatb_param_m_axis_video_stream_buf_final_size, &tcl_file.m_axis_video_V_data_V_depth);
aesl_fh.write(AUTOTB_TVOUT_m_axis_video_V_data_V, end_str());
tcl_file.set_num(__xlx_apatb_param_m_axis_video_stream_buf_final_size, &tcl_file.m_axis_video_V_keep_V_depth);
aesl_fh.write(AUTOTB_TVOUT_m_axis_video_V_keep_V, end_str());
tcl_file.set_num(__xlx_apatb_param_m_axis_video_stream_buf_final_size, &tcl_file.m_axis_video_V_strb_V_depth);
aesl_fh.write(AUTOTB_TVOUT_m_axis_video_V_strb_V, end_str());
tcl_file.set_num(__xlx_apatb_param_m_axis_video_stream_buf_final_size, &tcl_file.m_axis_video_V_user_V_depth);
aesl_fh.write(AUTOTB_TVOUT_m_axis_video_V_user_V, end_str());
tcl_file.set_num(__xlx_apatb_param_m_axis_video_stream_buf_final_size, &tcl_file.m_axis_video_V_last_V_depth);
aesl_fh.write(AUTOTB_TVOUT_m_axis_video_V_last_V, end_str());
tcl_file.set_num(__xlx_apatb_param_m_axis_video_stream_buf_final_size, &tcl_file.m_axis_video_V_id_V_depth);
aesl_fh.write(AUTOTB_TVOUT_m_axis_video_V_id_V, end_str());
tcl_file.set_num(__xlx_apatb_param_m_axis_video_stream_buf_final_size, &tcl_file.m_axis_video_V_dest_V_depth);
aesl_fh.write(AUTOTB_TVOUT_m_axis_video_V_dest_V, end_str());
{
aesl_fh.write(WRAPC_STREAM_SIZE_OUT_m_axis_video_V_data_V, begin_str(AESL_transaction));
sprintf(__xlx_sprintf_buffer.data(), "%d\n", __xlx_apatb_param_m_axis_video_stream_buf_final_size);
 aesl_fh.write(WRAPC_STREAM_SIZE_OUT_m_axis_video_V_data_V, __xlx_sprintf_buffer.data());
aesl_fh.write(WRAPC_STREAM_SIZE_OUT_m_axis_video_V_data_V, end_str());
}
{
aesl_fh.write(WRAPC_STREAM_SIZE_OUT_m_axis_video_V_keep_V, begin_str(AESL_transaction));
sprintf(__xlx_sprintf_buffer.data(), "%d\n", __xlx_apatb_param_m_axis_video_stream_buf_final_size);
 aesl_fh.write(WRAPC_STREAM_SIZE_OUT_m_axis_video_V_keep_V, __xlx_sprintf_buffer.data());
aesl_fh.write(WRAPC_STREAM_SIZE_OUT_m_axis_video_V_keep_V, end_str());
}
{
aesl_fh.write(WRAPC_STREAM_SIZE_OUT_m_axis_video_V_strb_V, begin_str(AESL_transaction));
sprintf(__xlx_sprintf_buffer.data(), "%d\n", __xlx_apatb_param_m_axis_video_stream_buf_final_size);
 aesl_fh.write(WRAPC_STREAM_SIZE_OUT_m_axis_video_V_strb_V, __xlx_sprintf_buffer.data());
aesl_fh.write(WRAPC_STREAM_SIZE_OUT_m_axis_video_V_strb_V, end_str());
}
{
aesl_fh.write(WRAPC_STREAM_SIZE_OUT_m_axis_video_V_user_V, begin_str(AESL_transaction));
sprintf(__xlx_sprintf_buffer.data(), "%d\n", __xlx_apatb_param_m_axis_video_stream_buf_final_size);
 aesl_fh.write(WRAPC_STREAM_SIZE_OUT_m_axis_video_V_user_V, __xlx_sprintf_buffer.data());
aesl_fh.write(WRAPC_STREAM_SIZE_OUT_m_axis_video_V_user_V, end_str());
}
{
aesl_fh.write(WRAPC_STREAM_SIZE_OUT_m_axis_video_V_last_V, begin_str(AESL_transaction));
sprintf(__xlx_sprintf_buffer.data(), "%d\n", __xlx_apatb_param_m_axis_video_stream_buf_final_size);
 aesl_fh.write(WRAPC_STREAM_SIZE_OUT_m_axis_video_V_last_V, __xlx_sprintf_buffer.data());
aesl_fh.write(WRAPC_STREAM_SIZE_OUT_m_axis_video_V_last_V, end_str());
}
{
aesl_fh.write(WRAPC_STREAM_SIZE_OUT_m_axis_video_V_id_V, begin_str(AESL_transaction));
sprintf(__xlx_sprintf_buffer.data(), "%d\n", __xlx_apatb_param_m_axis_video_stream_buf_final_size);
 aesl_fh.write(WRAPC_STREAM_SIZE_OUT_m_axis_video_V_id_V, __xlx_sprintf_buffer.data());
aesl_fh.write(WRAPC_STREAM_SIZE_OUT_m_axis_video_V_id_V, end_str());
}
{
aesl_fh.write(WRAPC_STREAM_SIZE_OUT_m_axis_video_V_dest_V, begin_str(AESL_transaction));
sprintf(__xlx_sprintf_buffer.data(), "%d\n", __xlx_apatb_param_m_axis_video_stream_buf_final_size);
 aesl_fh.write(WRAPC_STREAM_SIZE_OUT_m_axis_video_V_dest_V, __xlx_sprintf_buffer.data());
aesl_fh.write(WRAPC_STREAM_SIZE_OUT_m_axis_video_V_dest_V, end_str());
}
CodeState = DELETE_CHAR_BUFFERS;
AESL_transaction++;
tcl_file.set_num(AESL_transaction , &tcl_file.trans_num);
}
