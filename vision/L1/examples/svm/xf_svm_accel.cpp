/*
 * Copyright 2022 Xilinx, Inc.
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

#include "xf_svm_accel_config.h"

static constexpr int __XF_DEPTH =
    (IN_ARRAY_SIZE_1 * IN_ARRAY_SIZE_1 * (XF_PIXELWIDTH(IN_TYPE, NPPCX)) / 8) / (INPUT_PTR_WIDTH / 8);
static constexpr int __XF_DEPTH2 =
    (IN_ARRAY_SIZE_2 * IN_ARRAY_SIZE_2 * (XF_PIXELWIDTH(IN_TYPE, NPPCX)) / 8) / (INPUT_PTR_WIDTH / 8);

void svm_accel(ap_uint<INPUT_PTR_WIDTH>* img_in1,
               ap_uint<INPUT_PTR_WIDTH>* img_in2,
               unsigned short* params,
               unsigned char* fractional_out,
               ap_int<64>* result_out) {
// clang-format off
    #pragma HLS INTERFACE m_axi      port=img_in1        offset=slave  bundle=gmem0 depth=__XF_DEPTH
    #pragma HLS INTERFACE m_axi      port=img_in2        offset=slave  bundle=gmem1 depth=__XF_DEPTH2
    #pragma HLS INTERFACE m_axi      port=params         offset=slave  bundle=gmem2 depth=5
    #pragma HLS INTERFACE m_axi      port=fractional_out offset=slave  bundle=gmem3 depth=2
    #pragma HLS INTERFACE m_axi      port=result_out     offset=slave  bundle=gmem4 depth=2
    #pragma HLS INTERFACE s_axilite  port=return
    // clang-format on

    xf::cv::Mat<IN_TYPE, IN_ARRAY_SIZE_1, IN_ARRAY_SIZE_1, NPPCX, XF_CV_DEPTH_IN_1> imgInput1;
    xf::cv::Mat<IN_TYPE, IN_ARRAY_SIZE_2, IN_ARRAY_SIZE_2, NPPCX, XF_CV_DEPTH_IN_2> imgInput2;

    // Retrieve all the params:
    unsigned short index1 = params[0];
    unsigned short index2 = params[1];
    unsigned short frac1 = params[2];
    unsigned short frac2 = params[3];
    unsigned short n = params[4];

#pragma HLS DATAFLOW

    // Retrieve xf::cv::Mat objects from img_in data:
    xf::cv::Array2xfMat<INPUT_PTR_WIDTH, IN_TYPE, IN_ARRAY_SIZE_1, IN_ARRAY_SIZE_1, NPPCX, XF_CV_DEPTH_IN_1>(img_in1,
                                                                                                             imgInput1);
    xf::cv::Array2xfMat<INPUT_PTR_WIDTH, IN_TYPE, IN_ARRAY_SIZE_2, IN_ARRAY_SIZE_2, NPPCX, XF_CV_DEPTH_IN_2>(img_in2,
                                                                                                             imgInput2);

    // Run xfOpenCV kernel:
    xf::cv::SVM<IN_TYPE, IN_TYPE, OUTPUT_PTR_WIDTH, IN_ARRAY_SIZE_1, IN_ARRAY_SIZE_1, IN_ARRAY_SIZE_2, IN_ARRAY_SIZE_2,
                NPPCX, NO_OF_KERNEL_ELEMENTS, XF_CV_DEPTH_IN_1, XF_CV_DEPTH_IN_2>(
        imgInput1, imgInput2, index1, index2, frac1, frac2, n, fractional_out, result_out);

    return;
} // End of kernel
