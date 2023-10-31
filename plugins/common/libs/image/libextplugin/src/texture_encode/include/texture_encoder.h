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
#ifndef TEXTURE_ENCODER_H
#define TEXTURE_ENCODER_H

#define OPEN_HARMONY_LIB

#ifndef HLC_SHARED_LIB_API
#define HLC_SHARED_LIB_API
#endif

#include <stddef.h>
#include <stdbool.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>

#ifdef OPEN_HARMORY_LIB
#include "abs_image_encoder.h"
#include "hilog/log.h"
#include "log_tags.h"
#include "media_errors.h"
#define SUCCESS OHOS::Media::SUCCESS
#define ERROR OHOS::Media::ERROR

#else
enum PixelFormat {
    UNKNOWN = 0,
    ARGB_8888 = 1,  // Each pixel is stored on 4 bytes.
    RGB_565 = 2,    // Each pixel is stored on 2 bytes
    RGBA_8888 = 3,
    BGRA_8888 = 4,
    RGB_888 = 5,
    ALPHA_8 = 6,
    RGBA_F16 = 7,
    NV21 = 8,  // Each pixel is sorted on 3/2 bytes.
    NV12 = 9,
    CMYK = 10,
};

const int32_t ERR_MEDIA_INVALID_VALUE = -1;
const uint32_t SUCCESS = 0;
const uint32_t ERROR = (30 << 21) | (1 << 16);
#endif

#define TEXTURE_HEAD_BYTES (16)
const uint8_t g_blockSize[][2] = {
    {4, 4},
    {5, 4},
    {5, 5},
    {6, 5},
    {6, 6},
    {8, 5},
    {8, 6},
    {10, 5},
    {10, 6},
    {8, 8},
    {10, 8},
    {10, 10},
    {12, 10},
    {12, 12}
};
#define BLOCK_CLASS ((sizeof(g_blockSize) / sizeof(g_blockSize[0])))

typedef struct TextureEncodeOptionsType {
    int32_t width_;
    int32_t height_;
    uint8_t blockX_;
    uint8_t blockY_;
} TextureEncodeOption;

HLC_SHARED_LIB_API uint32_t GetVersion(char *version, uint32_t strLen);

HLC_SHARED_LIB_API uint32_t TextureEncode(void *handle,
    uint8_t *data, int32_t stride, OHOS::Media::PixelFormat pixelFormat,
    uint8_t *buffer, uint32_t memBytes, uint32_t &encBytes);

HLC_SHARED_LIB_API uint32_t TextureDestory(void *handle);

#endif // JPEG_ENCODER_H