/*
 * Copyright 2020 Xilinx, Inc.
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
#include "xf_pp_pipeline_config.h"
void pp_pipeline_accel(ap_uint<INPUT_PTR_WIDTH>* img_inp_y,  // Y Input image pointer
			 ap_uint<INPUT_PTR_WIDTH>* img_inp_uv, // UV Input image pointer
                         ap_uint<OUTPUT_PTR_WIDTH>* img_out, // output image pointer
                         float params[2 * XF_CHANNELS(IN_TYPE, NPC)],
                         int in_img_width,
                         int in_img_height,
                         int in_img_linestride,
                         int out_img_width,      // Final Output image width
                         int out_img_height,     // Final Output image height
                         int out_img_linestride) { // Final Output image line stride
// clang-format off
#pragma HLS INTERFACE m_axi     port=img_inp_y  offset=slave bundle=gmem1
#pragma HLS INTERFACE m_axi     port=img_inp_uv  offset=slave bundle=gmem2
#pragma HLS INTERFACE m_axi     port=img_out  offset=slave bundle=gmem3
#pragma HLS INTERFACE m_axi     port=params  offset=slave bundle=gmem4
#pragma HLS INTERFACE s_axilite port=in_img_width     
#pragma HLS INTERFACE s_axilite port=in_img_height     
#pragma HLS INTERFACE s_axilite port=in_img_linestride     
#pragma HLS INTERFACE s_axilite port=out_img_width     
#pragma HLS INTERFACE s_axilite port=out_img_height     
#pragma HLS INTERFACE s_axilite port=out_img_linestride     
#pragma HLS INTERFACE s_axilite port=return
    // clang-format on
    xf::cv::Mat<XF_8UC1, HEIGHT, WIDTH, NPC> imgInput_y(in_img_height, in_img_width);
#pragma HLS stream variable = imgInput_y.data depth = 2
    xf::cv::Mat<XF_8UC2, HEIGHT/2, WIDTH/2, NPC> imgInput_uv(in_img_height/2, in_img_width/2);	
#pragma HLS stream variable = imgInput_uv.data depth = 2
    xf::cv::Mat<XF_8UC3, HEIGHT, WIDTH, NPC> rgb_mat(in_img_height, in_img_width);
#pragma HLS stream variable = rgb_mat.data depth = 2
 
#if BGR2RGB
    xf::cv::Mat<XF_8UC3, HEIGHT, WIDTH, NPC> ch_swap_mat(in_img_height, in_img_width);
#endif
    xf::cv::Mat<XF_8UC3, NEWHEIGHT, NEWWIDTH, NPC> resize_out_mat(out_img_height, out_img_width);
#pragma HLS stream variable = resize_out_mat.data depth = 2
    xf::cv::Mat<OUT_TYPE, NEWHEIGHT, NEWWIDTH, NPC> out_mat(out_img_height, out_img_width);
// clang-format off
#pragma HLS stream variable = out_mat.data depth = 2
    xf::cv::accel_utils obj_iny, obj_inuv;
#pragma HLS DATAFLOW
    // clang-format on
    obj_iny.Array2xfMat<INPUT_PTR_WIDTH, XF_8UC1, HEIGHT, WIDTH, NPC>(img_inp_y, imgInput_y, in_img_linestride);
    obj_inuv.Array2xfMat<INPUT_PTR_WIDTH, XF_8UC2, HEIGHT/2, WIDTH/2, NPC> (img_inp_uv, imgInput_uv, in_img_linestride/2);
    xf::cv::nv122bgr<XF_8UC1, XF_8UC2, XF_8UC3, HEIGHT, WIDTH, NPC, NPC>(imgInput_y, imgInput_uv, rgb_mat);
 
#if BGR2RGB
    xf::cv::bgr2rgb<IN_TYPE, OUT_TYPE, HEIGHT, WIDTH, NPC>(rgb_mat, ch_swap_mat);
    xf::cv::resize<INTERPOLATION, IN_TYPE, HEIGHT, WIDTH, NEWHEIGHT, NEWWIDTH, NPC, MAXDOWNSCALE>(ch_swap_mat,
                                                                                                  resize_out_mat);
#else
    xf::cv::resize<INTERPOLATION, IN_TYPE, HEIGHT, WIDTH, NEWHEIGHT, NEWWIDTH, NPC, MAXDOWNSCALE>(rgb_mat,
                                                                                                  resize_out_mat);
#endif
    xf::cv::preProcess<IN_TYPE, OUT_TYPE, NEWHEIGHT, NEWWIDTH, NPC, WIDTH_A, IBITS_A, WIDTH_B, IBITS_B, WIDTH_OUT,
                       IBITS_OUT>(resize_out_mat, out_mat, params);
    xf::cv::xfMat2Array<OUTPUT_PTR_WIDTH, OUT_TYPE, NEWHEIGHT, NEWWIDTH, NPC>(out_mat, img_out, out_img_linestride);
}
