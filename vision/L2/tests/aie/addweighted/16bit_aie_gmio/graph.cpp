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

addweightedGraph mygraph;

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

    float alpha = 2.0f;
    float beta = 3.0f;
    float gamma = 4.0f;
    mygraph.init();
    mygraph.update(mygraph.alpha, alpha);
    mygraph.update(mygraph.beta, beta);
    mygraph.update(mygraph.gamma, gamma);
    mygraph.run(1);

    mygraph.in1.gm2aie_nb(inputData, BLOCK_SIZE_in_Bytes);
    mygraph.in2.gm2aie_nb(inputData1, BLOCK_SIZE_in_Bytes);
    mygraph.out.aie2gm_nb(outputData, BLOCK_SIZE_in_Bytes);
    mygraph.out.wait();

    // Compare the results
    int acceptableError = 1;
    int errCount = 0;
    int16_t* dataOut = (int16_t*)xf::cv::aie::xfGetImgDataPtr(outputData);
    for (int i = 0; i < TILE_ELEMENTS; i++) {
        float cValue = alpha * dataIn[i] + beta * dataIn1[i] + gamma;
        if (abs(dataOut[i] - cValue) > acceptableError) errCount++;
    }
    if (errCount) {
        std::cout << "Test failed!" << std::endl;
        exit(-1);
    }
    std::cout << "Test passed" << std::endl;

    mygraph.end();

    return 0;
}

#endif
