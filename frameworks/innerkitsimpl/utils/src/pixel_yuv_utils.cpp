/*
 * Copyright (C) 2024 Huawei Device Co., Ltd.
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

#include "pixel_yuv_utils.h"

#include <sys/stat.h>
#include <cerrno>
#include <climits>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <string>
#include <fstream>
#include <sstream>

#include "__config"
#include "image_log.h"
#include "ios"
#include "istream"
#include "media_errors.h"
#include "new"
#include "plugin_server.h"
#include "singleton.h"
#include "string"
#include "type_traits"
#include "vector"
#include "image_trace.h"
#include "hitrace_meter.h"
#include "image_system_properties.h"
#include "memory_manager.h"
#include "securec.h"
#if !defined(IOS_PLATFORM) && !defined(A_PLATFORM)
#include "surface_buffer.h"
#else
#include "refbase.h"
#endif

#undef LOG_DOMAIN
#define LOG_DOMAIN LOG_TAG_DOMAIN_ID_IMAGE

#undef LOG_TAG
#define LOG_TAG "PixelYuvUtils"

namespace OHOS {
namespace Media {

static const uint8_t NUM_2 = 2;
static const uint8_t NUM_3 = 3;
static const uint8_t NUM_4 = 4;
#ifndef LIBYUV_ENABLE
static const uint8_t NUM_5 = 5;
static const uint8_t NUM_6 = 6;
static const uint8_t NUM_7 = 7;
static const uint8_t NUM_8 = 8;
static const int32_t degrees360 = 360;
constexpr int32_t INT_16 = 16;
constexpr int32_t INT_128 = 128;
constexpr int32_t INT_219 = 219;
constexpr int32_t INT_224 = 224;
constexpr int32_t INT_255 = 255;
const float YUV_TO_RGB_PARAM_1 = 1.402;
const float YUV_TO_RGB_PARAM_2 = 0.344136;
const float YUV_TO_RGB_PARAM_3 = 0.714136;
const float YUV_TO_RGB_PARAM_4 = 1.772;
const float RGB_TO_YUV_PARAM_1 = 0.2627;
const float RGB_TO_YUV_PARAM_2 = 0.678;
const float RGB_TO_YUV_PARAM_3 = 0.0593;
const float RGB_TO_YUV_PARAM_4 = 0.5;
const float RGB_TO_YUV_PARAM_5 = 0.9407;
const float RGB_TO_YUV_PARAM_6 = 0.7373;
// Define pixel yuv malloc max size 600MB
constexpr int32_t PIXEL_YUV_MAX_RAM_SIZE = 600 * 1024 * 1024;
#endif
static const int32_t degrees90 = 90;
static const int32_t degrees180 = 180;
static const int32_t degrees270 = 270;
constexpr uint8_t Y_SHIFT = 16;
constexpr uint8_t U_SHIFT = 8;
constexpr uint8_t V_SHIFT = 0;
constexpr uint8_t YUV_MASK = 0xFF;
constexpr uint8_t Y_DEFAULT = 0x10;
constexpr uint8_t UV_DEFAULT = 0x80;

static int32_t GetYSize(int32_t width, int32_t height)
{
    return width * height;
}

static int32_t GetVOffset(int32_t width, int32_t height)
{
    return width * height + ((width + 1) / NUM_2) * ((height + 1) / NUM_2);
}

// The stride of u and v are the same, Yuv420P u, v single planer
static int32_t GetUStride(int32_t width)
{
    return (width + 1) / NUM_2;
}

static int32_t GetUVHeight(int32_t height)
{
    return (height + 1) / NUM_2;
}

// Yuv420SP, u、 v blend planer
static int32_t GetUVStride(int32_t width)
{
    return (width + 1) / NUM_2 * NUM_2;
}

static void WriteDataI420Convert(const uint8_t *srcPixels, const Size &size, uint8_t *dstPixels,
    Position dstPos, const Size &dstSize)
{
    dstPos.y = GetUVStride(dstPos.y);
    const uint8_t *srcU = srcPixels + GetYSize(size.width, size.height);
    const uint8_t *srcV = srcPixels + GetVOffset(size.width, size.height);
    uint8_t *dstU = dstPixels + GetYSize(dstSize.width, dstSize.height);
    uint8_t *dstV = dstPixels + GetVOffset(dstSize.width, dstSize.height);
    for (int32_t i = 0; i < size.height; i++) {
        if (memcpy_s(dstPixels + (dstPos.y + i) * dstSize.width + dstPos.x, size.width,
            srcPixels + i * size.width, size.width) != EOK) {
            IMAGE_LOGE("WriteDataI420Convert memcpy yplane failed");
            return;
        }
    }
    for (int32_t i = 0; i < GetUVHeight(size.height); i++) {
        if (memcpy_s(dstU + (dstPos.y / NUM_2 + i) * GetUStride(dstSize.width) + dstPos.x / NUM_2,
            GetUStride(size.width), srcU + i * GetUStride(size.width), GetUStride(size.width)) != 0 ||
            memcpy_s(dstV + (dstPos.y / NUM_2 + i) * GetUStride(dstSize.width) + dstPos.x / NUM_2,
            GetUStride(size.width), srcV + i * GetUStride(size.width), GetUStride(size.width)) != 0) {
            IMAGE_LOGE("WriteDataI420Convert memcpy uplane or vplane failed");
            return;
        }
    }
}

static void SwapUV(uint8_t *dstPixels, int32_t width, int32_t heigth)
{
    uint8_t *uvData = dstPixels + GetYSize(width, heigth);
    for (int32_t i = 0; i < GetUVStride(width) * GetUVHeight(heigth); i += NUM_2) {
        std::swap(uvData[i], uvData[i + 1]);
    }
}

static void WriteDataNV12Convert(uint8_t *srcPixels, const Size &size, uint8_t *dstPixels,
    Position dstPos, const Size &dstSize)
{
    if (size.width == size.height) {
        SwapUV(srcPixels, size.width, size.height);
    }
    dstPos.y = GetUVStride(dstPos.y);
    for (int i = 0; i < size.height; i++) {
        if (memcpy_s(dstPixels + (dstPos.y + i) * dstSize.width + dstPos.x, size.width,
            srcPixels + i * size.width, size.width) != 0) {
            IMAGE_LOGE("WriteDataNV12Convert memcpy yplane failed");
            return;
        }
    }
    for (int i = 0; i < GetUVHeight(size.height); ++i) {
        if (memcpy_s(dstPixels + ((dstPos.y) / NUM_2 + i) * GetUVStride(dstSize.width) + dstPos.x +
            GetYSize(dstSize.width, dstSize.height), GetUVStride(size.width),
            srcPixels + GetYSize(size.width, size.height) + i * GetUVStride(size.width),
            GetUVStride(size.width)) != 0) {
            IMAGE_LOGE("WriteDataNV12Convert memcpy uplane or vplane failed");
            return;
        }
    }
}

bool PixelYuvUtils::WriteYuvConvert(const void *srcPixels, const ImageInfo &srcInfo, void *dstPixels,
    const Position &dstPos, const ImageInfo &dstInfo)
{
    if (srcPixels == nullptr || dstPixels == nullptr) {
        IMAGE_LOGE("src or dst pixels invalid.");
        return false;
    }
    switch (dstInfo.pixelFormat) {
        case PixelFormat::YU12:
            WriteDataI420Convert((uint8_t *)srcPixels, srcInfo.size, static_cast<uint8_t *>(dstPixels), dstPos,
                dstInfo.size);
            return true;
        case PixelFormat::YV12:
            WriteDataI420Convert((uint8_t *)srcPixels, srcInfo.size, static_cast<uint8_t *>(dstPixels), dstPos,
                dstInfo.size);
            return true;
        case PixelFormat::NV21:
            WriteDataNV12Convert((uint8_t *)srcPixels, srcInfo.size, static_cast<uint8_t *>(dstPixels), dstPos,
                dstInfo.size);
            return true;
        case PixelFormat::NV12:
            WriteDataNV12Convert((uint8_t *)srcPixels, srcInfo.size, static_cast<uint8_t *>(dstPixels), dstPos,
                dstInfo.size);
            return true;
        default:
            return false;
    }
}

static bool CropYU12(const uint8_t *srcPixels, const Size &size, uint8_t *dstPixels, Rect rect)
{
    rect.top = GetUVStride(rect.top);
    for (int i = 0; i < size.height; i++) {
        if (memcpy_s(dstPixels + i * rect.width, rect.width,
            srcPixels + (rect.top + i) * size.width + rect.left, rect.width) != 0) {
            IMAGE_LOGE("CropYU12 memcpy yplane failed");
            return false;
        }
    }
    for (int i = 0; i < GetUVHeight(rect.height); i++) {
        if (memcpy_s(dstPixels + GetYSize(rect.width, rect.height) + i * GetUStride(rect.width),
            GetUStride(rect.width), srcPixels + GetYSize(size.width, size.height) + (rect.top / NUM_2 + i) *
            GetUStride(size.width) + rect.left / NUM_2, GetUStride(rect.width)) != 0 ||
            memcpy_s(dstPixels + GetVOffset(rect.width, rect.height) +
            i * GetUStride(rect.width), GetUStride(rect.width), srcPixels + GetVOffset(size.width, size.height) +
            ((rect.top / NUM_2) + i) * GetUStride(size.width) + rect.left / NUM_2, GetUStride(rect.width)) != 0) {
            IMAGE_LOGE("CropYU12 memcpy uplane or vplane failed");
            return false;
            }
    }
    return true;
}

static bool CropNV21(const uint8_t *srcPixels, const Size &size, uint8_t *dstPixels, Rect rect)
{
    rect.top = GetUVStride(rect.top);
    for (int32_t i = 0; i < rect.height; i++) {
        if (memcpy_s(dstPixels + i * rect.width, rect.width,
            srcPixels + (rect.top + i) * size.width + rect.left, rect.width) != 0) {
            IMAGE_LOGE("CropNV21 memcpy yplane failed");
            return false;
        }
    }
    const uint8_t *line = srcPixels + GetYSize(size.width, size.height) + rect.left;
    for (int32_t i = 0; i < GetUVHeight(rect.height); ++i) {
        if (memcpy_s(dstPixels + GetYSize(rect.width, rect.height) + i * GetUVStride(rect.width),
            GetUVStride(rect.width),
            line + ((rect.top / NUM_2) + i) * GetUVStride(size.width),
            GetUVStride(rect.width)) != 0) {
            IMAGE_LOGE("CropNV21 memcpy uvplane failed");
            return false;
        }
    }
    if (rect.width == rect.height) {
        SwapUV(dstPixels, rect.width, rect.height);
    }
    return true;
}

bool PixelYuvUtils::YuvCrop(const uint8_t *srcPixels, const Size &size, uint8_t *dstPixels, const Rect &rect,
    const PixelFormat &srcFormat)
{
    switch (srcFormat) {
        case PixelFormat::YV12:
            CropYU12(srcPixels, size, dstPixels, rect);
            break;
        case PixelFormat::YU12:
            CropYU12(srcPixels, size, dstPixels, rect);
            break;
        case PixelFormat::NV21:
            CropNV21(srcPixels, size, dstPixels, rect);
            break;
        case PixelFormat::NV12:
            CropNV21(srcPixels, size, dstPixels, rect);
            break;
        default:
            return false;
    }
    return true;
}

bool PixelYuvUtils::ReadYuvConvert(const void *srcPixels, const Position &srcPos, const ImageInfo &srcInfo,
    void *dstPixels, const ImageInfo &dstInfo)
{
    if (srcPixels == nullptr || dstPixels == nullptr) {
        IMAGE_LOGE("src or dst pixels invalid.");
        return false;
    }
    Rect rect;
    rect.left = srcPos.x;
    rect.top = srcPos.y;
    rect.width = dstInfo.size.width;
    rect.height = dstInfo.size.height;
    if (!YuvCrop((uint8_t *)srcPixels, srcInfo.size, static_cast<uint8_t *>(dstPixels), rect, srcInfo.pixelFormat)) {
        return false;
    }
    return true;
}

static bool I420AndYV12Transfers(const uint8_t *src, uint8_t *dst, int32_t width, int32_t height)
{
    uint32_t ySize = GetYSize(width, height);
    uint32_t uvDataSize = GetUStride(width) * GetUVHeight(height);
    if (memcpy_s(dst, ySize, src, ySize) != EOK ||
        memcpy_s(dst + ySize, uvDataSize, src + ySize + uvDataSize, uvDataSize) != EOK ||
        memcpy_s(dst + ySize + uvDataSize, uvDataSize, src + ySize, uvDataSize) != EOK) {
        IMAGE_LOGE("I420AndYV12Transfers memcpy failed");
        return false;
    }
    return true;
}

void PixelYuvUtils::SetTranslateDataDefault(uint8_t *srcPixels, int32_t width, int32_t height)
{
    int32_t ySize = GetYSize(width, height);
    int32_t uvSize = GetUStride(width) * GetUVHeight(height) * NUM_2;
    if (memset_s(srcPixels, ySize, Y_DEFAULT, ySize) != EOK ||
        memset_s(srcPixels + ySize, uvSize, UV_DEFAULT, uvSize) != EOK) {
        IMAGE_LOGW("set translate default color failed");
    }
}

uint8_t PixelYuvUtils::GetYuv420Y(uint32_t x, uint32_t y, int32_t width, const uint8_t *in)
{
    return *(in + y * width + x);
}

uint8_t PixelYuvUtils::GetYuv420U(uint32_t x, uint32_t y, int32_t width, int32_t height, PixelFormat format,
    const uint8_t *in)
{
    switch (format) {
        case PixelFormat::NV21:
            if (width & 1) {
                return *(in + y / NUM_2 * NUM_2 + GetYSize(width, height) + (y / NUM_2) * (width - 1) + (x & ~1) + 1);
            }
            return *(in + GetYSize(width, height) + (y / NUM_2) * width + (x & ~1) + 1);
        case PixelFormat::NV12:
            if (width & 1) {
                return *(in + y / NUM_2 * NUM_2 + GetYSize(width, height) + (y / NUM_2) * (width - 1) + (x & ~1));
            }
            return *(in + GetYSize(width, height) + (y / NUM_2) * width + (x & ~1));
        case PixelFormat::YU12:
            if (width & 1) {
                return *(in + y / NUM_2 + GetYSize(width, height) + (y / NUM_2) * (width / NUM_2) + (x / NUM_2));
            }
            return *(in + GetYSize(width, height) + (y / NUM_2) * (width / NUM_2) + (x / NUM_2));
        case PixelFormat::YV12:
            if (width & 1) {
                return *(in + height / NUM_2 + y / NUM_2 + GetYSize(width, height) +
                            (width / NUM_2) * (height / NUM_2) + (y / NUM_2) * (width / NUM_2) + (x / NUM_2));
            }
            return *(in + GetYSize(width, height) + (width / NUM_2) * (height / NUM_2) +
                        (y / NUM_2) * (width / NUM_2) + (x / NUM_2));
        default:
            break;
    }
    return SUCCESS;
}

uint8_t PixelYuvUtils::GetYuv420V(uint32_t x, uint32_t y, int32_t width, int32_t height, PixelFormat format,
    const uint8_t *in)
{
    switch (format) {
        case PixelFormat::NV21:
            if (width & 1) {
                return *(in + y / NUM_2 * NUM_2 + GetYSize(width, height) + (y / NUM_2) * (width - 1) + (x & ~1));
            }
            return *(in + GetYSize(width, height) + (y / NUM_2) * width + (x & ~1));
        case PixelFormat::NV12:
            if (width & 1) {
                return *(in + y / NUM_2 * NUM_2 + GetYSize(width, height) + (y / NUM_2) * (width - 1) + (x & ~1) + 1);
            }
            return *(in + GetYSize(width, height) + (y / NUM_2) * width + (x & ~1) + 1);
        case PixelFormat::YU12:
            if (width & 1) {
                return *(in + height / NUM_2 + y / NUM_2 + GetYSize(width, height) +
                            (width / NUM_2) * (height / NUM_2) + (y / NUM_2) * (width / NUM_2) + (x / NUM_2));
            }
            return *(in + GetYSize(width, height) + (width / NUM_2) * (height / NUM_2) +
                        (y / NUM_2) * (width / NUM_2) + (x / NUM_2));
            break;
        case PixelFormat::YV12:
            if (width & 1) {
                return *(in + y / NUM_2 + GetYSize(width, height) + (y / NUM_2) * (width / NUM_2) + (x / NUM_2));
            }
            return *(in + GetYSize(width, height) + (y / NUM_2) * (width / NUM_2) + (x / NUM_2));
        default:
            break;
    }
    return SUCCESS;
}

static bool YuvRotateConvert(Size &size, int32_t degrees, int32_t &dstWidth, int32_t &dstHeight)
{
    switch (degrees) {
        case 0:
            return true;
        case degrees90:
            dstWidth = size.height;
            dstHeight = size.width;
            break;
        case degrees180:
            break;
        case degrees270:
            dstWidth = size.height;
            dstHeight = size.width;
            break;
        default:
            break;
    }
    return false;
}

static void MergeUV(const uint8_t *srcU, const uint8_t *srcV, uint8_t *dstUV, int32_t width)
{
    for (int32_t x = 0; x < width - 1; x += NUM_2) {
        dstUV[0] = srcU[x];
        dstUV[1] = srcV[x];
        dstUV[NUM_2] = srcU[x + 1];
        dstUV[NUM_3] = srcV[x + 1];
        dstUV += NUM_4;
    }
    if (width & 1) {
        dstUV[0] = srcU[width - 1];
        dstUV[1] = srcV[width - 1];
    }
}

static void MergeUVPlane(const uint8_t *srcU, int32_t srcStrideU, const uint8_t *srcV, int32_t srcStrideV,
    uint8_t *dstUV, int32_t dstStrideUV, int32_t width, int32_t height)
{
    if (width <= 0 || height == 0) {
        return;
    }
    // Coalesce rows.
    if (srcStrideU == width && srcStrideV == width && dstStrideUV == width * NUM_2) {
        width *= height;
        height = 1;
        srcStrideU = srcStrideV = dstStrideUV = 0;
    }

    for (int32_t y = 0; y < height; ++y) {
        // Merge a row of U and V into a row of UV.
        MergeUV(srcU, srcV, dstUV, width);
        srcU += srcStrideU;
        srcV += srcStrideV;
        dstUV += dstStrideUV;
    }
}

static void MirrorRow(const uint8_t *src, uint8_t *dst, int32_t width)
{
    src += width - 1;
    for (int x = 0; x < width - 1; x += NUM_2) {
        dst[x] = src[0];
        dst[x + 1] = src[-1];
        src -= NUM_2;
    }
    if (width & 1) {
        dst[width - 1] = src[0];
    }
}

static uint32_t GetImageSize(int32_t width, int32_t height)
{
    return width * height + ((width + 1) / NUM_2) * ((height + 1) / NUM_2) * NUM_2;
}

#ifdef LIBYUV_ENABLE
bool PixelYuvUtils::BGRAToYuv420(const uint8_t *src, uint8_t *dst, int srcW, int srcH, PixelFormat pixelFormat)
{
    uint32_t pictureSize = GetImageSize(srcW, srcH);
    std::unique_ptr<uint8_t[]> temp = std::make_unique<uint8_t[]>(pictureSize);
    libyuv::ARGBToI420(src, srcW * NUM_4, temp.get(), srcW, temp.get() + GetYSize(srcW, srcH), GetUStride(srcW),
                      temp.get() + GetVOffset(srcW, srcH), GetUStride(srcW), srcW, srcH);
    int32_t r = 0;
    switch (pixelFormat) {
        case PixelFormat::NV12:
            r = libyuv::I420ToNV12(temp.get(), srcW, temp.get() + GetYSize(srcW, srcH), GetUStride(srcW),
                    temp.get() + GetVOffset(srcW, srcH), GetUStride(srcW), dst, srcW,
                    dst + GetYSize(srcW, srcH), GetUVStride(srcW), srcW, srcH);
            break;
        case PixelFormat::NV21:
            r = libyuv::I420ToNV21(temp.get(), srcW, temp.get() + GetYSize(srcW, srcH), GetUStride(srcW),
                    temp.get() + GetVOffset(srcW, srcH), GetUStride(srcW), dst, srcW,
                    dst + GetYSize(srcW, srcH), GetUVStride(srcW), srcW, srcH);
            break;
        default:
            break;
    }
    return r == 0;
}

bool PixelYuvUtils::Yuv420ToBGRA(const uint8_t *sample, uint8_t *dst_argb,
    int32_t width, int32_t height, PixelFormat pixelFormat)
{
    int alignedSrcW = (width + 1) & ~1;
    const uint8_t *src = nullptr;
    const uint8_t *src_uv = nullptr;
    const uint32_t dstStrideARGB = width * NUM_4;
    switch (pixelFormat) {
        case PixelFormat::NV12:
            src = sample;
            src_uv = sample + alignedSrcW * height;
            return 0 == libyuv::NV12ToARGB(src, width, src_uv, alignedSrcW, dst_argb, dstStrideARGB, width, height);
        case PixelFormat::NV21:
            src = sample;
            src_uv = sample + alignedSrcW * height;
            return 0 == libyuv::NV21ToARGB(src, width, src_uv, alignedSrcW, dst_argb, dstStrideARGB, width, height);
        default:
            break;
    }
    return false;
}

bool PixelYuvUtils::Yuv420ToARGB(const uint8_t *sample, uint8_t *dst_argb,
    int32_t width, int32_t height, PixelFormat pixelFormat)
{
    std::unique_ptr<uint8_t[]> temp = std::make_unique<uint8_t[]>(width * height * NUM_4);
    int32_t r = Yuv420ToBGRA(sample, temp.get(), width, height, pixelFormat);
    if (r != 0) {
        return false;
    }
    return 0 == libyuv::ARGBToBGRA(temp.get(), width * NUM_4, dst_argb, width * NUM_4, width, height);
}
#else

static bool I420ToNV12(const uint8_t *srcPixels, uint8_t *dstPixels, int32_t width, int32_t height)
{
    int32_t halfWidth = GetUStride(width);
    int32_t halfHeight = GetUVHeight(height);
    const uint8_t *srcV = srcPixels + GetVOffset(width, height);
    const uint8_t *srcU = srcPixels + GetYSize(width, height);
    uint8_t *dstUV = dstPixels + GetYSize(width, height);
    int32_t dstStrideUV = GetUVStride(width);
    // Negative height means invert the image.
    if (memcpy_s(dstPixels, GetYSize(width, height), srcPixels, GetYSize(width, height)) != 0) {
        IMAGE_LOGE("I420ToNV12 memcpy failed");
        return false;
    }
    MergeUVPlane(srcU, halfWidth, srcV, halfWidth, dstUV, dstStrideUV, halfWidth, halfHeight);
    return true;
}

static bool I420ToNV21(const uint8_t *srcPixels, uint8_t *dstPixels, int32_t width, int32_t height)
{
    int32_t halfWidth = GetUStride(width);
    int32_t halfHeight = GetUVHeight(height);
    const uint8_t *srcU = srcPixels + GetVOffset(width, height);
    const uint8_t *srcV = srcPixels + GetYSize(width, height);
    uint8_t *dstUV = dstPixels + GetYSize(width, height);
    int32_t dstStrideUV = GetUVStride(width);
    if (memcpy_s(dstPixels, GetYSize(width, height), srcPixels, GetYSize(width, height)) != 0) {
        IMAGE_LOGE("I420ToNV21 memcpy failed");
        return false;
    }
    MergeUVPlane(srcU, halfWidth, srcV, halfWidth, dstUV, dstStrideUV,
        halfWidth, halfHeight);
    return true;
}

static void TranslateRGBToYuv(const uint8_t *src, uint8_t *dst, Size &size,  size_t &i, UVPos &uvPos)
{
    for (int x = 0; x < size.width; x += NUM_2) {
        uint8_t b = src[NUM_4 * i];
        uint8_t g = src[NUM_4 * i + 1];
        uint8_t r = src[NUM_4 * i + NUM_2];

        uint8_t y = dst[i++] = INT_16 + INT_219 * (RGB_TO_YUV_PARAM_1 * r + RGB_TO_YUV_PARAM_2 * g +
            RGB_TO_YUV_PARAM_3 * b) / INT_255;

        dst[uvPos.upos++] = INT_128 + INT_224 * (RGB_TO_YUV_PARAM_4 * (b - y) / RGB_TO_YUV_PARAM_5) / INT_255;
        dst[uvPos.vpos++] = INT_128 + INT_224 * (RGB_TO_YUV_PARAM_4 * (r - y) / RGB_TO_YUV_PARAM_6) / INT_255;

        if ((size.width & 1) && x == size.width - 1) {
            // nothing to do
        } else {
            dst[i++] = INT_16 + INT_219 * (RGB_TO_YUV_PARAM_1 * r + RGB_TO_YUV_PARAM_2 * g +
                RGB_TO_YUV_PARAM_3 * b) / INT_255;
        }
    }
}

static bool BGRAToI420(const uint8_t *src, uint8_t *dst, int32_t width, int32_t height)
{
    if (!src || !dst || width <= 0 || height == 0) {
        return false;
    }

    size_t i = 0;
    Size size = {width, height};
    UVPos uvPos;
    uvPos.upos = GetYSize(size.width, size.height);
    uvPos.vpos = GetVOffset(size.width, size.height);

    for (int32_t line = 0; line < height; ++line) {
        if (!(line % NUM_2)) {
            TranslateRGBToYuv(src, dst, size, i, uvPos);
        } else {
            for (int32_t x = 0; x < width; x += 1) {
                uint8_t b = src[NUM_4 * i];
                uint8_t g = src[NUM_4 * i + 1];
                uint8_t r = src[NUM_4 * i + NUM_2];

                dst[i++] = INT_16 + INT_219 * (RGB_TO_YUV_PARAM_1 * r + RGB_TO_YUV_PARAM_2 * g +
                    RGB_TO_YUV_PARAM_3 * b) / INT_255;
            }
        }
    }
    return true;
}

bool PixelYuvUtils::BGRAToYuv420(const uint8_t *src, uint8_t *dst, int32_t width, int32_t height,
    PixelFormat pixelFormat)
{
    uint32_t pictureSize = GetImageSize(width, height);
    if (pictureSize == 0 || pictureSize > PIXEL_YUV_MAX_RAM_SIZE) {
        IMAGE_LOGE("BGRAToYuv420 parameter pictureSize:[%{public}u] error.", pictureSize);
        return false;
    }
    std::unique_ptr<uint8_t[]> temp = std::make_unique<uint8_t[]>(pictureSize);
    bool result = BGRAToI420(src, temp.get(), width, height);
    if (!result) {
        return false;
    }
    switch (pixelFormat) {
        case PixelFormat::NV12:
            I420ToNV12(temp.get(), dst, width, height);
            break;
        case PixelFormat::NV21:
            I420ToNV21(temp.get(), dst, width, height);
            break;
        case PixelFormat::YU12:
            if (memcpy_s(dst, pictureSize, temp.get(), pictureSize) != 0) {
                IMAGE_LOGE("BGRAToYuv420 to YU12 memcpy failed.");
                return false;
            }
            return true;
        case PixelFormat::YV12:
            I420AndYV12Transfers(temp.get(), dst, width, height);
            break;
        default:
            break;
    }
    return true;
}

bool PixelYuvUtils::Yuv420ToBGRA(const uint8_t *in, uint8_t *out, int32_t width, int32_t height,
    PixelFormat pixelFormat)
{
    if (!in || !out || width <= 0 || height == 0) {
        return false;
    }
    for (int32_t i = 0; i < height; i++) {
        for (int32_t j = 0; j < width; j++) {
            uint8_t Y = GetYuv420Y(j, i, width, in);
            uint8_t U = GetYuv420U(j, i, width, height, pixelFormat, in);
            uint8_t V = GetYuv420V(j, i, width, height, pixelFormat, in);

            int32_t colorR = Y + YUV_TO_RGB_PARAM_1 * (V - INT_128);
            int32_t colorG = Y - YUV_TO_RGB_PARAM_2 * (U - INT_128) - YUV_TO_RGB_PARAM_3 * (V - INT_128);
            int32_t colorB = Y + YUV_TO_RGB_PARAM_4 * (U - INT_128);

            colorR = colorR > INT_255 ? INT_255 : (colorR < 0 ? 0 : colorR);
            colorG = colorG > INT_255 ? INT_255 : (colorG < 0 ? 0 : colorG);
            colorB = colorB > INT_255 ? INT_255 : (colorB < 0 ? 0 : colorB);

            *out++ = colorB;
            *out++ = colorG;
            *out++ = colorR;
            *out++ = 0xFF;
        }
    }
    return true;
}

bool PixelYuvUtils::Yuv420ToARGB(const uint8_t *in, uint8_t *out, int32_t width, int32_t height,
    PixelFormat pixelFormat)
{
    if (!in || !out || width <= 0 || height == 0) {
        return false;
    }
    for (int32_t i = 0; i < height; i++) {
        for (int32_t j = 0; j < width; j++) {
            uint8_t Y = GetYuv420Y(j, i, width, in);
            uint8_t U = GetYuv420U(j, i, width, height, pixelFormat, in);
            uint8_t V = GetYuv420V(j, i, width, height, pixelFormat, in);

            int32_t colorR = Y + YUV_TO_RGB_PARAM_1 * (V - INT_128);
            int32_t colorG = Y - YUV_TO_RGB_PARAM_2 * (U - INT_128) - YUV_TO_RGB_PARAM_3 * (V - INT_128);
            int32_t colorB = Y + YUV_TO_RGB_PARAM_4 * (U - INT_128);

            colorR = colorR > INT_255 ? INT_255 : (colorR < 0 ? 0 : colorR);
            colorG = colorG > INT_255 ? INT_255 : (colorG < 0 ? 0 : colorG);
            colorB = colorB > INT_255 ? INT_255 : (colorB < 0 ? 0 : colorB);

            *out++ = 0xFF;
            *out++ = colorR;
            *out++ = colorG;
            *out++ = colorB;
        }
    }
    return true;
}
#endif

static void CopyPlane(const uint8_t *srcY, int32_t srcStrideY, uint8_t *dstY, int32_t dstStrideY,
    int32_t width, int32_t height)
{
    if (width <= 0 || height == 0) {
        return;
    }
    // Coalesce rows.
    if (srcStrideY == width && dstStrideY == width) {
        width *= height;
        height = 1;
        srcStrideY = dstStrideY = 0;
    }
    // Nothing to do.
    if (srcY == dstY && srcStrideY == dstStrideY) {
        return;
    }

    // Copy plane
    for (int32_t y = 0; y < height; ++y) {
        if (memcpy_s(dstY, width, srcY, width) != 0) {
            IMAGE_LOGE("CopyPlane memcpy failed.");
        }
        srcY += srcStrideY;
        dstY += dstStrideY;
    }
}

int32_t PixelYuvUtils::I420Copy(const uint8_t *srcY, int32_t srcStrideY, const uint8_t *srcU, int32_t srcStrideU,
    const uint8_t *srcV, int32_t srcStrideV, uint8_t *dstY, int32_t dstStrideY, uint8_t *dstU,
    int32_t dstStrideU, uint8_t *dstV, int32_t dstStrideV, int32_t width, int32_t height)
{
    int32_t halfwidth = GetUStride(width);
    int32_t halfheight = GetUVHeight(height);
    if ((!srcY && dstY) || !srcU || !srcV || !dstU || !dstV || width <= 0 || height == 0) {
        return ERR_IMAGE_INVALID_PARAMETER;
    }
    // Negative height means invert the image.
    if (height < 0) {
        height = -height;
        halfheight = GetUVHeight(height);
        srcY = srcY + (height - 1) * srcStrideY;
        srcU = srcU + (halfheight - 1) * srcStrideU;
        srcV = srcV + (halfheight - 1) * srcStrideV;
        srcStrideY = -srcStrideY;
        srcStrideU = -srcStrideU;
        srcStrideV = -srcStrideV;
    }

    if (dstY) {
        CopyPlane(srcY, srcStrideY, dstY, dstStrideY, width, height);
    }
    // Copy UV planes.
    CopyPlane(srcU, srcStrideU, dstU, dstStrideU, halfwidth, halfheight);
    CopyPlane(srcV, srcStrideV, dstV, dstStrideV, halfwidth, halfheight);
    return SUCCESS;
}

static void MirrorPlane(const uint8_t *srcY, int32_t srcStrideY, uint8_t *dstY, int32_t dstStrideY,
    int32_t width, int32_t height)
{
    // Negative height means invert the image.
    if (height < 0) {
        height = -height;
        srcY = srcY + (height - 1) * srcStrideY;
        srcStrideY = -srcStrideY;
    }

    // Mirror plane
    for (int32_t y = 0; y < height; ++y) {
        MirrorRow(srcY, dstY, width);
        srcY += srcStrideY;
        dstY += dstStrideY;
    }
}

static void MirrorUVRow(const uint8_t *srcUV, uint8_t *dstUV, int32_t width)
{
    srcUV += (width - 1) << 1;
    for (int32_t x = 0; x < width; ++x) {
        dstUV[0] = srcUV[0];
        dstUV[1] = srcUV[1];
        srcUV -= NUM_2;
        dstUV += NUM_2;
    }
}

static void MirrorUVPlane(const uint8_t *srcUV, int32_t srcStrideUV, uint8_t *dstUV, int32_t dstStrideUV,
    int32_t width, int32_t height)
{
    // Negative height means invert the image.
    if (height < 0) {
        height = -height;
        srcUV = srcUV + (height - 1) * srcStrideUV;
        srcStrideUV = -srcStrideUV;
    }
    // MirrorUV plane
    for (int32_t y = 0; y < height; ++y) {
        MirrorUVRow(srcUV, dstUV, width);
        srcUV += srcStrideUV;
        dstUV += dstStrideUV;
    }
}

int32_t PixelYuvUtils::I420Mirror(const uint8_t *srcY, int32_t srcStrideY, const uint8_t *srcU, int32_t srcStrideU,
    const uint8_t *srcV, int32_t srcStrideV, uint8_t *dstY, int32_t dstStrideY,
    uint8_t *dstU, int32_t dstStrideU, uint8_t *dstV, int32_t dstStrideV,
    int32_t width, int32_t height)
{
    int32_t halfwidth = GetUStride(width);
    int32_t halfheight = GetUVHeight(height);

    if (!srcY || !srcU || !srcV || !dstU || !dstV || width <= 0 || height == 0) {
        return ERR_IMAGE_INVALID_PARAMETER;
    }

    // Negative height means invert the image.
    if (height < 0) {
        height = -height;
        halfheight = GetUVHeight(height);
        srcY = srcY + (height - 1) * srcStrideY;
        srcU = srcU + (halfheight - 1) * srcStrideU;
        srcV = srcV + (halfheight - 1) * srcStrideV;
        srcStrideY = -srcStrideY;
        srcStrideU = -srcStrideU;
        srcStrideV = -srcStrideV;
    }

    if (dstY) {
        MirrorPlane(srcY, srcStrideY, dstY, dstStrideY, width, height);
    }
    MirrorPlane(srcU, srcStrideU, dstU, dstStrideU, halfwidth, halfheight);
    MirrorPlane(srcV, srcStrideV, dstV, dstStrideV, halfwidth, halfheight);
    return SUCCESS;
}

int32_t PixelYuvUtils::NV12Mirror(const uint8_t *srcY, int32_t srcStrideY, const uint8_t *srcUV, int32_t srcStrideUV,
    uint8_t *dstY, int32_t dstStrideY, uint8_t *dstUV, int32_t dstStrideUV, int32_t width, int32_t height)
{
    int32_t halfwidth = GetUStride(width);
    int32_t halfheight = GetUVHeight(height);

    if (!srcY || !srcUV || !dstUV || width <= 0 || height == 0) {
        return ERR_IMAGE_INVALID_PARAMETER;
    }

    // Negative height means invert the image.
    if (height < 0) {
        height = -height;
        halfheight = GetUVHeight(height);
        srcY = srcY + (height - 1) * srcStrideY;
        srcUV = srcUV + (halfheight - 1) * srcStrideUV;
        srcStrideY = -srcStrideY;
        srcStrideUV = -srcStrideUV;
    }

    if (dstY) {
        MirrorPlane(srcY, srcStrideY, dstY, dstStrideY, width, height);
    }
    MirrorUVPlane(srcUV, srcStrideUV, dstUV, dstStrideUV, halfwidth, halfheight);
    return SUCCESS;
}

int32_t PixelYuvUtils::NV12Copy(const uint8_t *srcY, int32_t srcStrideY, const uint8_t *srcUV, int32_t srcStrideUV,
    uint8_t *dstY, int32_t dstStrideY, uint8_t *dstUV, int32_t dstStrideUV, int32_t width, int32_t height)
{
    int32_t halfwidth = GetUStride(width);
    int32_t halfheight = GetUVHeight(height);

    if (!srcY || !dstY || !srcUV || !dstUV || width <= 0 || height == 0) {
        return ERR_IMAGE_INVALID_PARAMETER;
    }

    // Negative height means invert the image.
    if (height < 0) {
        height = -height;
        halfheight = GetUVHeight(height);
        srcY = srcY + (height - 1) * srcStrideY;
        srcUV = srcUV + (halfheight - 1) * srcStrideUV;
        srcStrideY = -srcStrideY;
        srcStrideUV = -srcStrideUV;
    }
    CopyPlane(srcY, srcStrideY, dstY, dstStrideY, width, height);
    CopyPlane(srcUV, srcStrideUV, dstUV, dstStrideUV, halfwidth * NUM_2, halfheight);
    return SUCCESS;
}

#ifdef LIBYUV_ENABLE
static bool YuvRotateConvert(Size &size, int32_t degrees, int32_t &dstWidth, int32_t &dstHeight,
    libyuv::RotationMode &rotateNum)
{
    switch (degrees) {
        case 0:
            return true;
        case degrees90:
            dstWidth = size.height;
            dstHeight = size.width;
            rotateNum = libyuv::RotationMode::kRotate90;
            break;
        case degrees180:
            rotateNum = libyuv::RotationMode::kRotate180;
            break;
        case degrees270:
            dstWidth = size.height;
            dstHeight = size.width;
            rotateNum = libyuv::RotationMode::kRotate270;
            break;
        default:
            return false;
    }
    return true;
}

static bool NV21Rotate(const uint8_t *src, uint8_t *dstData, PixelSize &size,
    uint8_t *dstPixels, libyuv::RotationMode &rotateNum)
{
    if (libyuv::NV12ToI420Rotate(src, size.srcW, src + GetYSize(size.srcW, size.srcH),
            GetUVStride(size.srcW), dstData, size.dstW,
            dstData + GetVOffset(size.dstW, size.dstH),
            GetUStride(size.dstW), dstData + GetYSize(size.dstW, size.dstH),
            GetUStride(size.dstW), size.srcW, size.srcH, rotateNum) == -1) {
        IMAGE_LOGE("NV21 to I420 rotate failed");
        return false;
    }
    if (libyuv::I420ToNV21(dstData, size.dstW, dstData + GetYSize(size.dstW, size.dstH), GetUStride(size.dstW),
            dstData + GetVOffset(size.dstW, size.dstH), GetUStride(size.dstW),
            dstPixels, size.dstW, dstPixels + GetYSize(size.dstW, size.dstH),
            GetUVStride(size.dstW), size.dstW, size.dstH) == -1) {
        IMAGE_LOGE("I420 to NV21 failed");
        return false;
    }
    return true;
}

static bool NV12Rotate(const uint8_t *src, uint8_t *dstData, PixelSize &size,
    uint8_t *dstPixels, libyuv::RotationMode &rotateNum)
{
    if (libyuv::NV12ToI420Rotate(src, size.srcW, src + GetYSize(size.srcW, size.srcH),
            GetUVStride(size.srcW), dstData, size.dstW,
            dstData + GetYSize(size.dstW, size.dstH), GetUStride(size.dstW),
            dstData + GetVOffset(size.dstW, size.dstH), GetUStride(size.dstW),
            size.srcW, size.srcH, rotateNum) == -1) {
        IMAGE_LOGE("NV12 to I420 rotate failed");
        return false;
    }
    if (libyuv::I420ToNV12(dstData, size.dstW, dstData + GetYSize(size.dstW, size.dstH),
            GetUStride(size.dstW), dstData + GetVOffset(size.dstW, size.dstH),
            GetUStride(size.dstW), dstPixels, size.dstW, dstPixels + GetYSize(size.dstW, size.dstH),
            GetUVStride(size.dstW), size.dstW, size.dstH) == -1) {
        IMAGE_LOGE("I420 to NV21 failed");
        return false;
    }
    return true;
}

static bool I420Rotate(const uint8_t *src, PixelSize &size, uint8_t *dstPixels, libyuv::RotationMode &rotateNum)
{
    if (libyuv::I420Rotate(src, size.srcW, src + GetYSize(size.srcW, size.srcH),
            GetUStride(size.srcW), src + GetVOffset(size.srcW, size.srcH),
            GetUStride(size.srcW), dstPixels, size.dstW, dstPixels + GetYSize(size.dstW, size.dstH),
            GetUStride(size.dstW), dstPixels + GetVOffset(size.dstW, size.dstH),
            GetUStride(size.dstW), size.srcW, size.srcH, rotateNum) == -1) {
            IMAGE_LOGE("I420 rotate failed");
        return false;
    }
    return true;
}

bool PixelYuvUtils::YuvRotate(const uint8_t *srcPixels, Size &size, uint8_t *dstPixels, int32_t degrees,
    const PixelFormat &format)
{
    libyuv::RotationMode rotateNum = libyuv::RotationMode::kRotate0;
    int32_t dstWidth = size.width;
    int32_t dstHeight = size.height;
    if (YuvRotateConvert(size, degrees, dstWidth, dstHeight, rotateNum)) {
        IMAGE_LOGI("Rotate degress is 0, don't need rotate");
        return true;
    }
    MemoryData memoryData = {nullptr, GetImageSize(dstWidth, dstHeight), "YuvRotate ImageData", {dstWidth, dstHeight}};
    auto m = MemoryManager::CreateMemory(allocatorType_, memoryData);
    if (m == nullptr) {
        IMAGE_LOGE("translate CreateMemory failed");
        return false;
    }
#if !defined(IOS_PLATFORM) && !defined(A_PLATFORM)
    if (allocatorType_ == AllocatorType::DMA_ALLOC) {
        if (m->extend.data == nullptr) {
            IMAGE_LOGE("GendstTransInfo get surfacebuffer failed");
        }
    }
#endif
    uint8_t *dstData = reinterpret_cast<uint8_t *>(m->data.data);
    PixelSize pixelSize = {size.width, size.height, dstWidth, dstHeight};
    switch (format) {
        case PixelFormat::NV21:
            if (!NV21Rotate(srcPixels, dstData, pixelSize, dstPixels, rotateNum)) {
                m->Release();
                return false;
            }
        case PixelFormat::NV12:
            if (!NV12Rotate(srcPixels, dstData, pixelSize, dstPixels, rotateNum)) {
                m->Release();
                return false;
            }
        case PixelFormat::YV12:
        case PixelFormat::YU12:
            if (!I420Rotate(srcPixels, pixelSize, dstPixels, rotateNum)) {
                m->Release();
                return false;
            }
        default:
            return false;
    }
    size.width = dstWidth;
    size.height = dstHeight;
    return true;
}

void PixelYuvUtils::ConvertYuvMode(libyuv::FilterMode &filterMode, const AntiAliasingOption &option)
{
    switch (option) {
        case AntiAliasingOption::NONE:
            filterMode = libyuv::FilterMode::kFilterNone;
            break;
        case AntiAliasingOption::LOW:
            filterMode = libyuv::FilterMode::kFilterLinear;
            break;
        case AntiAliasingOption::MEDIUM:
            filterMode = libyuv::FilterMode::kFilterBilinear;
            break;
        case AntiAliasingOption::HIGH:
            filterMode = libyuv::FilterMode::kFilterBox;
            break;
        default:
            break;
    }
}
#else

static bool NV12ToI420(const uint8_t *srcPixels, const Size &size, uint8_t *dstPixels)
{
    if (srcPixels == nullptr || dstPixels == nullptr || size.width <= 0 || size.height <= 0) {
        IMAGE_LOGE("NV12ToI420 param error!");
        return false;
    }
    const uint8_t *srcUV = srcPixels + GetYSize(size.width, size.height);
    uint8_t *destU = dstPixels + GetYSize(size.width, size.height);
    uint8_t *destV = dstPixels + GetVOffset(size.width, size.height);
    if (memcpy_s(dstPixels, GetYSize(size.width, size.height), srcPixels, GetYSize(size.width, size.height)) != 0) {
        IMAGE_LOGE("NV12ToI420 memcpy failed");
        return false;
    }
    for (int32_t i = 0; i < GetUStride(size.width) * GetUVHeight(size.height); i++) {
        destU[i] = srcUV[i * NUM_2];
        destV[i] = srcUV[i * NUM_2 + 1];
    }
    return true;
}

static bool NV21ToI420(const uint8_t *srcPixels, const Size &size, uint8_t *dstPixels)
{
    if (srcPixels == nullptr || dstPixels == nullptr || size.width <= 0 || size.height <= 0) {
        IMAGE_LOGE("NV21ToI420 param error!");
        return false;
    }
    const uint8_t *srcUV = srcPixels + GetYSize(size.width, size.height);
    uint8_t *destV = dstPixels + GetYSize(size.width, size.height);
    uint8_t *destU = dstPixels + GetVOffset(size.width, size.height);
    if (memcpy_s(dstPixels, GetYSize(size.width, size.height), srcPixels, GetYSize(size.width, size.height)) != 0) {
        IMAGE_LOGE("NV21ToI420 memcpy failed");
        return false;
    }
    for (int32_t i = 0; i < GetUStride(size.width) * GetUVHeight(size.height); i++) {
        destU[i] = srcUV[i * NUM_2];
        destV[i] = srcUV[i * NUM_2 + 1];
    }
    return true;
}

static void TransposeW8(const uint8_t *src, int32_t srcStride, uint8_t *dst, int32_t dstStride, int32_t width)
{
    for (int32_t i = 0; i < width; ++i) {
        dst[0] = src[0 * srcStride];
        dst[1] = src[1 * srcStride];
        dst[NUM_2] = src[NUM_2 * srcStride];
        dst[NUM_3] = src[NUM_3 * srcStride];
        dst[NUM_4] = src[NUM_4 * srcStride];
        dst[NUM_5] = src[NUM_5 * srcStride];
        dst[NUM_6] = src[NUM_6 * srcStride];
        dst[NUM_7] = src[NUM_7 * srcStride];
        ++src;
        dst += dstStride;
    }
}

static void TransposePlane(const uint8_t *src, int32_t srcStride, uint8_t *dst, int32_t dstStride,
    const Size &half)
{
    int32_t i = half.height;
    while (i >= NUM_8) {
        TransposeW8(src, srcStride, dst, dstStride, half.width);
        src += NUM_8 * srcStride; // Go down 8 rows.
        dst += NUM_8;             // Move over 8 columns.
        i -= NUM_8;
    }

    if (i > 0) {
        for (int32_t k = 0; k < half.width; ++k) {
            for (int32_t j = 0; j < i; ++j) {
                dst[k * dstStride + j] = src[j * srcStride + k];
            }
        }
    }
}

static void RotatePlane270(const uint8_t *src, int32_t srcStride, uint8_t *dst, int32_t dstStride,
    const Size &half)
{
    dst += dstStride * (half.width - 1);
    dstStride = -dstStride;
    TransposePlane(src, srcStride, dst, dstStride, half);
}

static void RotatePlane180(const uint8_t *src, int32_t srcStride, uint8_t *dst, int32_t dstStride,
    const Size &half)
{
    // Swap top and bottom row and mirror the content. Uses a temporary row.
    std::unique_ptr<uint8_t[]> row = std::make_unique<uint8_t[]>(half.width);

    const uint8_t *src_bot = src + srcStride * (half.height - 1);
    uint8_t *dst_bot = dst + dstStride * (half.height - 1);
    int32_t half_height = GetUVHeight(half.height);
    for (int32_t y = 0; y < half_height; ++y) {
        if (memcpy_s(row.get(), half.width, src, half.width) != 0) {
            IMAGE_LOGE("RotatePlane180 memcpy failed.");
            return;
        }
        MirrorRow(src_bot, dst, half.width);
        MirrorRow(row.get(), dst_bot, half.width);
        src += srcStride;
        dst += dstStride;
        src_bot -= srcStride;
        dst_bot -= dstStride;
    }
}

static void RotatePlane90(const uint8_t *src, int32_t srcStride, uint8_t *dst,
    int32_t dstStride, const Size &half)
{
    src += srcStride * (half.height - 1);
    srcStride = -srcStride;
    TransposePlane(src, srcStride, dst, dstStride, half);
}

static bool YV12Rotate(const uint8_t *srcPixels, Size &size, uint8_t *dstPixels, int32_t degrees)
{
    int32_t dstWidth = size.width;
    int32_t dstHeight = size.height;
    if (degrees < 0) {
        degrees += degrees360;
    }
    if (YuvRotateConvert(size, degrees, dstWidth, dstHeight)) {
        IMAGE_LOGI("Rotate degress is 0, don't need rotate");
        return true;
    }
    const uint8_t *srcU = srcPixels + GetYSize(size.width, size.height);
    const uint8_t *srcV = srcPixels + GetVOffset(size.width, size.height);
    int32_t srcStrideU = GetUStride(size.width);
    int32_t srcStrideV = GetUStride(size.width);
    uint8_t *dstU = dstPixels + GetYSize(dstWidth, dstHeight);
    uint8_t *dstV = dstPixels + GetVOffset(dstWidth, dstHeight);
    int32_t dstStrideU = GetUStride(dstWidth);
    int32_t dstStrideV = GetUStride(dstWidth);
    Size halfSize = {GetUStride(size.width), GetUVHeight(size.height)};
    switch (degrees) {
        case 0:
            return false;
        case degrees90:
            RotatePlane90(srcPixels, size.width, dstPixels, dstWidth, size);
            RotatePlane90(srcU, srcStrideU, dstU, dstStrideU, halfSize);
            RotatePlane90(srcV, srcStrideV, dstV, dstStrideV, halfSize);
            break;
        case degrees180:
            RotatePlane180(srcPixels, size.width, dstPixels, dstWidth, size);
            RotatePlane180(srcU, srcStrideU, dstU, dstStrideU, halfSize);
            RotatePlane180(srcV, srcStrideV, dstV, dstStrideV, halfSize);
            break;
        case degrees270:
            RotatePlane270(srcPixels, size.width, dstPixels, dstWidth, size);
            RotatePlane270(srcU, srcStrideU, dstU, dstStrideU, halfSize);
            RotatePlane270(srcV, srcStrideV, dstV, dstStrideV, halfSize);
            break;
        default:
            return false;
    }
    size.height = dstHeight;
    size.width = dstWidth;
    return true;
}

bool PixelYuvUtils::YuvRotate(const uint8_t *srcPixels, Size &size, uint8_t *dstPixels, int32_t degrees,
    const PixelFormat &format)
{
    if (degrees < 0) {
        degrees = degrees360 + degrees;
    }
    switch (format) {
        case PixelFormat::YU12:
        case PixelFormat::YV12:
            if (!YV12Rotate(srcPixels, size, dstPixels, degrees)) {
                return false;
            }
            return true;
        case PixelFormat::NV21:
            NV21ToI420(srcPixels, size, dstPixels);
            break;
        case PixelFormat::NV12:
            NV12ToI420(srcPixels, size, dstPixels);
            break;
        default:
            return false;
    }
    size_t count = GetImageSize(size.width, size.height);
    if (count == 0 || count > PIXEL_YUV_MAX_RAM_SIZE) {
        IMAGE_LOGE("YuvRotate parameter count:[%{public}zu] error.", count);
        return false;
    }
    std::unique_ptr<uint8_t[]> src = std::make_unique<uint8_t[]>(count);
    if (!YV12Rotate(dstPixels, size, src.get(), degrees)) {
        return false;
    }
    switch (format) {
        case PixelFormat::NV21:
            I420ToNV21(src.get(), dstPixels, size.width, size.height);
            return true;
        case PixelFormat::NV12:
            I420ToNV12(src.get(), dstPixels, size.width, size.height);
            return true;
        case PixelFormat::YV12:
        case PixelFormat::YU12:
            return true;
        default:
            return false;
    }
}
#endif
static void Yuv420PWritePixels(Size &size, uint8_t *srcPixels, const uint32_t &color, bool isI420)
{
    uint8_t colorY = (color >> Y_SHIFT) & YUV_MASK;
    uint8_t colorU = (color >> U_SHIFT) & YUV_MASK;
    uint8_t colorV = (color >> V_SHIFT) & YUV_MASK;

    uint8_t *srcU;
    uint8_t *srcV;
    if (isI420) {
        srcU = srcPixels + GetYSize(size.width, size.height);
        srcV = srcPixels + GetVOffset(size.width, size.height);
    } else {
        srcV = srcPixels + GetYSize(size.width, size.height);
        srcU = srcPixels + GetVOffset(size.width, size.height);
    }

    for (int32_t i = 0; i < size.height; i++) {
        for (int32_t j = 0; j < size.width; j++) {
            *(srcPixels + i * size.width + j) = colorY;
        }
    }
    for (int32_t i = 0; i < GetUVHeight(size.height); i++) {
        for (int32_t j = 0; j < GetUStride(size.width); j++) {
            *(srcU + i * GetUStride(size.width) + j) = colorU;
            *(srcV + i * GetUStride(size.width) + j) = colorV;
        }
    }
}

static void Yuv420SPWritePixels(Size &size, uint8_t *srcPixels, const uint32_t &color, bool isNV12)
{
    uint8_t colorY = (color >> Y_SHIFT) & YUV_MASK;
    uint8_t colorU = (color >> U_SHIFT) & YUV_MASK;
    uint8_t colorV = (color >> V_SHIFT) & YUV_MASK;

    uint8_t *srcUV = srcPixels + GetYSize(size.width, size.height);

    for (int32_t y = 0; y < size.height; y++) {
        for (int32_t x = 0; x < size.width; x++) {
            *(srcPixels + y * size.width + x) = colorY;
        }
    }

    for (int32_t y = 0; y < size.height / NUM_2; y++) {
        for (int32_t x = 0; x < size.width; x++) {
            if (isNV12) {
                *(srcUV + (y * size.width + x)) = colorU;
                *(srcUV + (y * size.width + x) + 1) = colorV;
            } else {
                *(srcUV + (y * size.width + x)) = colorV;
                *(srcUV + (y * size.width + x) + 1) = colorU;
            }
        }
    }
}

bool PixelYuvUtils::Yuv420WritePixels(Size &size, uint8_t *srcPixels, const PixelFormat &format, const uint32_t &color)
{
    switch (format) {
        case PixelFormat::YU12:
        case PixelFormat::YV12: {
            bool isI420 = (format == PixelFormat::YU12 ? true : false);
            Yuv420PWritePixels(size, srcPixels, color, isI420);
            return true;
        }
        case PixelFormat::NV21:
        case PixelFormat::NV12: {
            bool isNV12 = (format == PixelFormat::NV12 ? true : false);
            Yuv420SPWritePixels(size, srcPixels, color, isNV12);
            return true;
        }
        default:
            return false;
    }
}

static void Yuv420PReadPixel(const uint8_t *srcPixels, Size &size, const Position &pos, uint32_t &dst, bool isI420)
{
    const uint8_t *srcU;
    const uint8_t *srcV;

    if (isI420) {
        srcU = srcPixels + GetYSize(size.width, size.height);
        srcV = srcPixels + GetVOffset(size.width, size.height);
    } else {
        srcV = srcPixels + GetYSize(size.width, size.height);
        srcU = srcPixels + GetVOffset(size.width, size.height);
    }

    uint8_t colorY = *(srcPixels + (pos.y * size.width + pos.x));
    uint8_t colorU = *(srcU + ((pos.y / NUM_2) * GetUStride(size.width) + pos.x / NUM_2));
    uint8_t colorV = *(srcV + ((pos.y / NUM_2) * GetUStride(size.width) + pos.x / NUM_2));

    uint32_t yuvColor = (colorY << Y_SHIFT) | (colorU << U_SHIFT) | (colorV << V_SHIFT);
    dst = yuvColor;
}

static void Yuv420SPReadPixel(const uint8_t *srcPixels, Size &size, const Position &pos, uint32_t &dst, bool isNV12)
{
    const uint8_t *srcUV = srcPixels + GetYSize(size.width, size.height);
    uint8_t colorY = *(srcPixels + (pos.y * size.width + pos.x));
    uint8_t colorU = 0;
    uint8_t colorV = 0;

    if (isNV12) {
        colorU = *(srcUV + (pos.y / NUM_2) * GetUStride(size.width) + pos.x / NUM_2);
        colorV = *(srcUV + (pos.y / NUM_2) * GetUStride(size.width) + pos.x / NUM_2 + 1);
    } else {
        colorV = *(srcUV + (pos.y / NUM_2) * GetUStride(size.width) + pos.x / NUM_2);
        colorU = *(srcUV + (pos.y / NUM_2) * GetUStride(size.width) + pos.x / NUM_2 + 1);
    }

    uint32_t yuvColor = (colorY << Y_SHIFT) | (colorU << U_SHIFT) | (colorV << V_SHIFT);
    dst = yuvColor;
}


bool PixelYuvUtils::YuvReadPixel(const uint8_t *srcPixels, Size &size, const PixelFormat &format, const Position &pos,
    uint32_t &dst)
{
    switch (format) {
        case PixelFormat::YU12:
        case PixelFormat::YV12: {
            bool isI420 = (format == PixelFormat::YU12) ? true : false;
            Yuv420PReadPixel(srcPixels, size, pos, dst, isI420);
            return true;
        }
        case PixelFormat::NV21:
        case PixelFormat::NV12: {
            bool isNV12 = (format == PixelFormat::NV12) ? true : false;
            Yuv420SPReadPixel(srcPixels, size, pos, dst, isNV12);
            return true;
        }
        default:
            return false;
    }
}

static void Yuv420PWritePixel(uint8_t *srcPixels, Size &size, const Position &pos,
    const uint32_t &color, bool isI420)
{
    uint8_t *srcU;
    uint8_t *srcV;

    uint8_t colorY = (color >> Y_SHIFT) & YUV_MASK;
    uint8_t colorU = (color >> U_SHIFT) & YUV_MASK;
    uint8_t colorV = (color >> V_SHIFT) & YUV_MASK;

    if (isI420) {
        srcU = srcPixels + GetYSize(size.width, size.height);
        srcV = srcPixels + GetVOffset(size.width, size.height);
    } else {
        srcV = srcPixels + GetYSize(size.width, size.height);
        srcU = srcPixels + GetVOffset(size.width, size.height);
    }

    *(srcPixels + (pos.y * size.width + pos.x)) = colorY;
    *(srcU + ((pos.y / NUM_2) * GetUStride(size.width) + pos.x / NUM_2)) = colorU;
    *(srcV + ((pos.y / NUM_2) * GetUStride(size.width) + pos.x / NUM_2)) = colorV;
}

static void Yuv420SPWritePixel(uint8_t *srcPixels, Size &size, const Position &pos,
    const uint32_t &color, bool isNV12)
{
    uint8_t *srcUV = srcPixels + GetYSize(size.width, size.height);
    uint8_t colorY = (color >> Y_SHIFT) & YUV_MASK;
    uint8_t colorU = (color >> U_SHIFT) & YUV_MASK;
    uint8_t colorV = (color >> V_SHIFT) & YUV_MASK;

    *(srcPixels + (pos.y * size.width + pos.x)) = colorY;

    if (isNV12) {
        *(srcUV + (pos.y / NUM_2) * GetUStride(size.width) + pos.x / NUM_2) = colorU;
        *(srcUV + (pos.y / NUM_2) * GetUStride(size.width) + pos.x / NUM_2 + 1) = colorV;
    } else {
        *(srcUV + (pos.y / NUM_2) * GetUStride(size.width) + pos.x / NUM_2) = colorV;
        *(srcUV + (pos.y / NUM_2) * GetUStride(size.width) + pos.x / NUM_2 + 1) = colorU;
    }
}

bool PixelYuvUtils::YuvWritePixel(uint8_t *srcPixels, Size &size, const PixelFormat &format, const Position &pos,
    const uint32_t &color)
{
    switch (format) {
        case PixelFormat::YU12:
        case PixelFormat::YV12: {
            bool isI420 = (format == PixelFormat::YU12) ? true : false;
            Yuv420PWritePixel(srcPixels, size, pos, color, isI420);
            return true;
        }
        case PixelFormat::NV21:
        case PixelFormat::NV12: {
            bool isNV12 = (format == PixelFormat::NV12) ? true : false;
            Yuv420SPWritePixel(srcPixels, size, pos, color, isNV12);
            return true;
        }
        default:
            return false;
    }
}

static void Yuv420PTranslate(const uint8_t *srcPixels, Size &size, uint8_t *dstPixels, float xAxis, float yAxis,
    bool isI420)
{
    int32_t dstWidth = size.width;
    int32_t dstHeight = size.height;
    const uint8_t *srcU;
    const uint8_t *srcV;
    uint8_t *dstU;
    uint8_t *dstV;
    if (isI420) {
        srcU = srcPixels + GetYSize(size.width, size.height);
        srcV = srcPixels + GetVOffset(size.width, size.height);
        dstU = dstPixels + GetYSize(dstWidth, dstHeight);
        dstV = dstPixels + GetVOffset(dstWidth, dstHeight);
    } else {
        srcV = srcPixels + GetYSize(size.width, size.height);
        srcU = srcPixels + GetVOffset(size.width, size.height);
        dstV = dstPixels + GetYSize(dstWidth, dstHeight);
        dstU = dstPixels + GetVOffset(dstWidth, dstHeight);
    }

    for (int32_t y = 0; y < size.height; y++) {
        for (int32_t x = 0; x < size.width; x++) {
            int32_t newX = x + xAxis;
            int32_t newY = y + yAxis;
            if (newX >= 0 && newY >= 0 && newX < size.width && newY < size.height) {
                *(dstPixels + newY * dstWidth + newX) = *(srcPixels + y * size.width + x);
            }
        }
    }

    for (int32_t y = 0; y < GetUVHeight(size.height); y++) {
        for (int32_t x = 0; x < GetUStride(size.width); x++) {
            int32_t newX = x + xAxis / NUM_2;
            int32_t newY = y + yAxis / NUM_2;
            if (newX >= 0 && newY >= 0 && newX < size.width / NUM_2 && newY < size.height / NUM_2) {
                *(dstU + newY * GetUStride(dstWidth) + newX) = *(srcU + y * GetUStride(size.width) + x);
                *(dstV + newY * GetUStride(dstWidth) + newX) = *(srcV + y * GetUStride(size.width) + x);
            }
        }
    }
}

static void Yuv420SPTranslate(const uint8_t *srcPixels, Size &size, uint8_t *dstPixels, float xAxis, float yAxis)
{
    int32_t dstWidth = size.width;
    int32_t dstHeight = size.height;

    const uint8_t *srcUV = srcPixels + GetYSize(size.width, size.height);
    uint8_t *dstUV = dstPixels + GetYSize(dstWidth, dstHeight);

    for (int32_t y = 0; y < size.height; y++) {
        for (int32_t x = 0; x < size.width; x++) {
            int32_t newX = x + xAxis;
            int32_t newY = y + yAxis;
            if (newX >= 0 && newY >= 0 && newX < size.width && newY < size.height) {
                *(dstPixels + newY * dstWidth + newX) = *(srcPixels + y * size.width + x);
            }
        }
    }

    for (int32_t y = 0; y < GetUVHeight(size.height); y++) {
        for (int32_t x = 0; x < GetUStride(size.width); x++) {
            int32_t newX = x + xAxis / NUM_2;
            int32_t newY = y + yAxis / NUM_2;
            if (newX >= 0 && newX < size.width / NUM_2 && newY >= 0 && newY < size.height / NUM_2) {
                *(dstUV + (newY * dstWidth + newX * NUM_2)) = *(srcUV + (y * size.width + x * NUM_2));
                *(dstUV + (newY * dstWidth + newX * NUM_2) + 1) = *(srcUV + (y * size.width + x * NUM_2) + 1);
            }
        }
    }
}

bool PixelYuvUtils::YuvTranslate(const uint8_t *srcPixels, Size &size, uint8_t *dstPixels, const PixelFormat &format,
    float xAxis, float yAxis)
{
    switch (format) {
        case PixelFormat::YU12:
        case PixelFormat::YV12: {
            bool isI420 = (format == PixelFormat::YU12) ? true : false;
            Yuv420PTranslate(srcPixels, size, dstPixels, xAxis, yAxis, isI420);
            return true;
        }
        case PixelFormat::NV21:
        case PixelFormat::NV12: {
            Yuv420SPTranslate(srcPixels, size, dstPixels, xAxis, yAxis);
            return true;
        }
        default:
            return false;
    }
}

} // namespace Media
} // namespace OHOS