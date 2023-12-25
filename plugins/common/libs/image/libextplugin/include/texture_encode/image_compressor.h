/*
 * Copyright (C) 2023 Huawei Device Co., Ltd.
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

#ifndef PLUGINS_COMMON_LIBS_IMAGE_LIBEXTPLUGIN_INCLUDE_TEXTURE_ENCODE_IMAGE_COMPRESSOR_H
#define PLUGINS_COMMON_LIBS_IMAGE_LIBEXTPLUGIN_INCLUDE_TEXTURE_ENCODE_IMAGE_COMPRESSOR_H
#include <atomic>
#include <chrono>
#include <fstream>
#include <iostream>
#include <mutex>
#include <pthread.h>
#include <set>
#include <stdio.h>
#include <string.h>
#include <sys/syscall.h>
#include <unistd.h>

#define USE_OPENCL_WRAPPER
#include "opencl_wrapper.h"

namespace OHOS {
namespace Media {
constexpr int TEXTURE_HEAD_BYTES = 16;
constexpr int MAGIC_FILE_CONSTANT = 0x5CA1AB13;
constexpr int DIM = 4;
constexpr int MAX_WIDTH_HEIGHT = 100000;

constexpr int PATITION_MODE_NUMS = 73;
constexpr int BIT_MAP_NUM = 2;
constexpr int TIMEUS_UNIT = 1000;

class ImageCompressor {
public:
    static std ::shared_ptr<ImageCompressor> GetInstance();
    static const int32_t releaseTimeMs = TIMEUS_UNIT;

    bool CreateKernel();
    bool TextureEncodeCL(uint8_t *data, int32_t stride, int32_t width, int32_t height, uint8_t *buffer);
    void ReleaseResource();
private:
    static std::shared_ptr<ImageCompressor> instance_;
    static std::mutex instanceMutex_;
    size_t sourceSize_;

    bool clOk_;
    bool switch_;
    void Init();

    static const int32_t maxSize_ = MAX_WIDTH_HEIGHT;

    cl_context context_;
    cl_command_queue queue_;
    cl_kernel kernel_;

    std::string ReadSourceCode(const char* fileName);
    cl_program LoadShader(cl_context context);
    void GenAstcHeader(uint8_t *buffer, uint8_t blockX, uint8_t blockY, uint32_t dimX, uint32_t dimY);

    std::string compileOption_;
};
} // namespace Media
} // namespace OHOS
#endif