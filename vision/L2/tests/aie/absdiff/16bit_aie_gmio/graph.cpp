/*
 * Copyright 2021 Xilinx, Inc.
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

#include "graph.h"

myGraph absdiff_graph;

#if defined(__AIESIM__) || defined(__X86SIM__)
#include <common/xf_aie_utils.hpp>
int main(int argc, char** argv) {
    int BLOCK_SIZE_in_Bytes = TILE_WINDOW_SIZE;

    int16_t* inputData = (int16_t*)GMIO::malloc(BLOCK_SIZE_in_Bytes);
    int16_t* inputData1 = (int16_t*)GMIO::malloc(BLOCK_SIZE_in_Bytes);
    int16_t* outputData = (int16_t*)GMIO::malloc(BLOCK_SIZE_in_Bytes);

    memset(inputData, 0, BLOCK_SIZE_in_Bytes);
    memset(inputData1, 0, BLOCK_SIZE_in_Bytes);
    xf::cv::aie::xfSetTileWidth(inputData, TILE_WIDTH);
    xf::cv::aie::xfSetTileHeight(inputData, TILE_HEIGHT);
    xf::cv::aie::xfSetTileWidth(inputData1, TILE_WIDTH);
    xf::cv::aie::xfSetTileHeight(inputData1, TILE_HEIGHT);

    int16_t* dataIn = (int16_t*)xf::cv::aie::xfGetImgDataPtr(inputData);
    int16_t* dataIn1 = (int16_t*)xf::cv::aie::xfGetImgDataPtr(inputData1);
    for (int i = 0; i < TILE_ELEMENTS; i++) {
        dataIn[i] = rand() % 256;
        dataIn1[i] = rand() % 256;
    }

    absdiff_graph.init();

    absdiff_graph.run(1);

    absdiff_graph.inprt1.gm2aie_nb(inputData, BLOCK_SIZE_in_Bytes);
    absdiff_graph.inprt2.gm2aie_nb(inputData1, BLOCK_SIZE_in_Bytes);
    absdiff_graph.outprt.aie2gm_nb(outputData, BLOCK_SIZE_in_Bytes);
    absdiff_graph.outprt.wait();

    // Compare the results
    int acceptableError = 0;
    int errCount = 0;
    int16_t* dataOut = (int16_t*)xf::cv::aie::xfGetImgDataPtr(outputData);
    for (int i = 0; i < TILE_ELEMENTS; i++) {
        int cValue = abs(dataIn[i] - dataIn1[i]);
        if (abs(dataOut[i] - cValue) > acceptableError) errCount++;
    }
    if (errCount) {
        std::cout << "Test failed!" << std::endl;
        exit(-1);
    }
    std::cout << "Test passed" << std::endl;

    absdiff_graph.end();
    return 0;
}
#endif
