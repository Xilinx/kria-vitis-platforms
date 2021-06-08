/*
 * Copyright 2019 Xilinx, Inc.
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

#include "xf_threshold_config.h"

extern "C" {
void gaussian_otsu_accel(
    ap_uint<GAUSSIAN_INPUT_PTR_WIDTH>* img_inp, ap_uint<GAUSSIAN_OUTPUT_PTR_WIDTH>* img_out, int rows, int cols, float sigma, unsigned char* Otsuval) {
// clang-format off
    #pragma HLS INTERFACE m_axi     port=img_inp  offset=slave bundle=gmem1
    #pragma HLS INTERFACE m_axi     port=img_out  offset=slave bundle=gmem2
	#pragma HLS INTERFACE m_axi     port=Otsuval  offset=slave bundle=gmem3
    #pragma HLS INTERFACE s_axilite port=sigma     
    #pragma HLS INTERFACE s_axilite port=rows     
    #pragma HLS INTERFACE s_axilite port=cols     
    #pragma HLS INTERFACE s_axilite port=return
    // clang-format on

    xf::cv::Mat<TYPE, HEIGHT, WIDTH, NPC1> in_mat(rows, cols);
// clang-format off
    #pragma HLS stream variable=in_mat.data depth=2
    // clang-format on
    xf::cv::Mat<TYPE, HEIGHT, WIDTH, NPC1> out_mat(rows, cols);

	xf::cv::Mat<TYPE, HEIGHT, WIDTH, NPC1> out_mat_otsu(rows, cols);
	xf::cv::Mat<TYPE, HEIGHT, WIDTH, NPC1> out_mat_ret(rows, cols);

// clang-format off
    #pragma HLS stream variable=out_mat.data depth=2
// clang-format on

// clang-format off
    #pragma HLS DATAFLOW
    // clang-format on

    xf::cv::Array2xfMat<GAUSSIAN_INPUT_PTR_WIDTH, TYPE, HEIGHT, WIDTH, NPC1>(img_inp, in_mat);

    xf::cv::GaussianBlur<FILTER_WIDTH, XF_BORDER_CONSTANT, TYPE, HEIGHT, WIDTH, NPC1>(in_mat, out_mat, sigma);

	xf::cv::duplicateMat<TYPE, HEIGHT, WIDTH, NPC1>(out_mat, out_mat_otsu, out_mat_ret);

	xf::cv::OtsuThreshold<OTSU_PIXEL_TYPE, HEIGHT, WIDTH, NPC1>(out_mat_otsu, *Otsuval);

    xf::cv::xfMat2Array<GAUSSIAN_OUTPUT_PTR_WIDTH, TYPE, HEIGHT, WIDTH, NPC1>(out_mat_ret, img_out);
}
}
