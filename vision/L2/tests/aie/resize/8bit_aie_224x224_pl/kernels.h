/*
 * Copyright (C) 2019-2022, Xilinx, Inc.
 * Copyright (C) 2022-2023, Advanced Micro Devices, Inc.
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

#ifndef _RESIZE_RUNNER_H
#define _RESIZE_RUNNER_H

#include <adf.h>
#include <adf/io_buffer/io_buffer.h>
#include <adf/stream/types.h>
#include "config.h"
#include <stdlib.h>
#include <stdint.h>

template <int WIDTH_IN, int HEIGHT_IN, int WIDTH_OUT, int HEIGHT_OUT, int IMG_HEIGHT_OUT>
class ResizeRunner {
    uint16_t (&mPos)[WIDTH_OUT];
    uint8_t (&mwtsX)[WIDTH_OUT << 2];
    uint8_t (&mwtsY)[IMG_HEIGHT_OUT];

   public:
    ResizeRunner(uint16_t (&pos)[WIDTH_OUT], uint8_t (&wtsx)[WIDTH_OUT << 2], uint8_t (&wtsy)[IMG_HEIGHT_OUT])
        : mPos(pos), mwtsX(wtsx), mwtsY(wtsy) {}
    void run(adf::input_buffer<uint8_t>& input, adf::output_buffer<uint8_t>& output);
    static void registerKernelClass() {
        REGISTER_FUNCTION(ResizeRunner::run);
        REGISTER_PARAMETER(mPos);
        REGISTER_PARAMETER(mwtsX);
        REGISTER_PARAMETER(mwtsY);
    }
};

#endif
