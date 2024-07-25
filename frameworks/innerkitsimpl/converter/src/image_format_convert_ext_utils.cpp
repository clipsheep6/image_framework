/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "image_format_convert_ext_utils.h"

#include <cmath>
#include <cstring>
#include <iostream>
#include <map>
#include "hilog/log.h"
#include "image_convert_tools.h"
#include "image_log.h"
#include "log_tags.h"
#include "securec.h"

namespace {
constexpr uint32_t NUM_0 = 0;
constexpr uint32_t NUM_1 = 1;
constexpr uint32_t NUM_2 = 2;
constexpr uint32_t NUM_3 = 3;
constexpr uint32_t NUM_4 = 4;

constexpr uint32_t EVEN_ODD_DIVISOR = 2;
constexpr uint32_t TWO_SLICES = 2;
constexpr uint32_t BYTES_PER_PIXEL_RGB565 = 2;
constexpr uint32_t BYTES_PER_PIXEL_RGB = 3;
constexpr uint32_t BYTES_PER_PIXEL_RGBA = 4;
constexpr uint32_t BYTES_PER_PIXEL_BGRA = 4;
constexpr int32_t PIXEL_MAP_MAX_RAM_SIZE = 600 * 1024 * 1024;
}

#undef LOG_TAG
#define LOG_TAG "ImageFormatConvertExt"
namespace OHOS {
namespace Media {
static bool NV12ToRGBANoManual(const uint8_t *srcBuffer, const YUVDataInfo &yDInfo, uint8_t **destBuffer,
    ColorSpace colorSpace)
{
    uint32_t yPlaneSize = yDInfo.yWidth * yDInfo.yHeight;
    uint32_t uvPlaneWidth = (yDInfo.yWidth + 1) / EVEN_ODD_DIVISOR;
    uint32_t uvPlaneHeight = (yDInfo.yHeight + 1) / EVEN_ODD_DIVISOR;
    uint32_t uPlaneSize = uvPlaneWidth * uvPlaneHeight;
    uint32_t vPlaneSize = uPlaneSize;
    uint32_t yStride = yDInfo.yWidth;
    uint32_t i420BufferSize = yPlaneSize + uPlaneSize + vPlaneSize;

    uint8_t *i420Buffer = new (std::nothrow) uint8_t[i420BufferSize];
    if (i420Buffer == nullptr) {
        IMAGE_LOGE("Dynamically allocating memory for i420 buffer failed!");
        return false;
    }

    auto &converter = ConverterHandle::GetInstance().GetHandle();
    uint8_t *ySrc = i420Buffer;
    uint8_t *uSrc = i420Buffer + yPlaneSize;
    uint8_t *vSrc = i420Buffer + yPlaneSize + uPlaneSize;
    uint32_t uStride = uvPlaneWidth;
    uint32_t vStride = uvPlaneWidth;

    int32_t ret = converter.NV12ToI420(srcBuffer + yDInfo.yOffset, yDInfo.yStride, srcBuffer + yDInfo.uvOffset,
        yDInfo.uvStride, ySrc, yStride, uSrc, uStride, vSrc, vStride, yDInfo.yWidth, yDInfo.yHeight);
    if (ret != 0) {
        IMAGE_LOGE("NV12ToI420 failed, ret = %{public}d!", ret);
        delete[] i420Buffer;
        return false;
    }
    uint32_t rgbaStride = yDInfo.yWidth * BYTES_PER_PIXEL_RGBA;
    ret = converter.I420ToABGR(ySrc, yStride, uSrc, uStride, vSrc, vStride, *destBuffer, rgbaStride,
        yDInfo.yWidth, yDInfo.yHeight);
    delete[] i420Buffer;
    if (ret != 0) {
        IMAGE_LOGE("I420ToABGR failed, ret = %{public}d!", ret);
        return false;
    }
    return true;
}

static bool NV12ToBGRANoManual(const uint8_t *srcBuffer, const YUVDataInfo &yDInfo, uint8_t **destBuffer,
    ColorSpace colorSpace)
{
    uint32_t bgraStride = yDInfo.yWidth * BYTES_PER_PIXEL_BGRA;

    auto &converter = ConverterHandle::GetInstance().GetHandle();
    int32_t ret = converter.NV12ToARGB(srcBuffer + yDInfo.yOffset, yDInfo.yStride, srcBuffer + yDInfo.uvOffset,
        yDInfo.uvStride, *destBuffer, bgraStride, yDInfo.yWidth, yDInfo.yHeight);
    if (ret != 0) {
        IMAGE_LOGE("NV12ToARGB failed, ret = %{public}d!", ret);
        return false;
    }
    return true;
}

static bool NV21ToRGBAMatrix(const uint8_t *srcBuffer, const YUVDataInfo &yDInfo, uint8_t **destBuffer,
    ColorSpace colorSpace)
{
    uint32_t yu12BufferSize = yDInfo.yWidth * yDInfo.yHeight + yDInfo.uvWidth * yDInfo.uvHeight * EVEN_ODD_DIVISOR;
    if (yu12BufferSize <= 0 || yu12BufferSize > PIXEL_MAP_MAX_RAM_SIZE) {
        IMAGE_LOGE("Invalid destination buffer size calculation!");
        return false;
    }
    uint8_t *yu12Buffer = new (std::nothrow) uint8_t[yu12BufferSize];
    if (yu12Buffer == nullptr) {
        IMAGE_LOGE("apply space for I420 buffer failed!");
        return false;
    }

    auto &converter = ConverterHandle::GetInstance().GetHandle();
    uint8_t *srcVU = const_cast<uint8_t *>(srcBuffer) + yDInfo.uvOffset;
    uint8_t *srcY = const_cast<uint8_t *>(srcBuffer) + yDInfo.yOffset;
    int srcStrideVu = (yDInfo.uvStride + 1) / EVEN_ODD_DIVISOR * EVEN_ODD_DIVISOR;
    uint8_t *dstU = yu12Buffer + yDInfo.yWidth * yDInfo.yHeight;
    int dstStrideU = (yDInfo.yWidth + 1) / EVEN_ODD_DIVISOR;
    uint8_t *dstV = yu12Buffer + yDInfo.yWidth * yDInfo.yHeight +
        ((yDInfo.yWidth + NUM_1) / EVEN_ODD_DIVISOR * (yDInfo.yHeight + 1) / EVEN_ODD_DIVISOR);
    int dstStrideV = (yDInfo.yWidth + 1) / EVEN_ODD_DIVISOR;

    int ret = converter.NV21ToI420(srcY, yDInfo.yStride, srcVU, srcStrideVu, yu12Buffer, yDInfo.yWidth, dstU,
        dstStrideU, dstV, dstStrideV, yDInfo.yWidth, yDInfo.yHeight);
    if (ret != 0) {
        IMAGE_LOGE("NV21ToI420 failed, ret = %{public}d!", ret);
        delete[] yu12Buffer;
        return false;
    }
    ret = converter.I420ToABGR(yu12Buffer, yDInfo.yWidth, dstU, dstStrideU, dstV, dstStrideV, *destBuffer,
        yDInfo.yWidth * NUM_4, yDInfo.yWidth, yDInfo.yHeight);
    delete[] yu12Buffer;
    if (ret != 0) {
        IMAGE_LOGE("I420ToABGR failed, ret = %{public}d!", ret);
        return false;
    }
    return true;
}

static bool NV21ToRGB565Matrix(const uint8_t *srcBuffer, const YUVDataInfo &yDInfo, uint8_t **destBuffer,
    ColorSpace colorSpace)
{
    uint32_t yu12BufferSize = yDInfo.yWidth * yDInfo.yHeight + yDInfo.uvWidth * yDInfo.uvHeight * EVEN_ODD_DIVISOR;
    if (yu12BufferSize <= 0 || yu12BufferSize > PIXEL_MAP_MAX_RAM_SIZE) {
        IMAGE_LOGE("Invalid destination buffer size calculation!");
        return false;
    }
    uint8_t *yu12Buffer = new (std::nothrow) uint8_t[yu12BufferSize];
    if (yu12Buffer == nullptr) {
        IMAGE_LOGE("apply space for I420 buffer failed!");
        return false;
    }
    auto &converter = ConverterHandle::GetInstance().GetHandle();
    const uint8_t *srcY = srcBuffer + yDInfo.yOffset;
    const uint8_t *srcVU = srcBuffer + yDInfo.uvOffset;
    int srcStrideVu = (yDInfo.uvStride + 1) / EVEN_ODD_DIVISOR * EVEN_ODD_DIVISOR;
    uint8_t *dstU = yu12Buffer + yDInfo.yWidth * yDInfo.yHeight;
    int dstStrideU = yDInfo.uvStride / EVEN_ODD_DIVISOR;
    uint8_t *dstV = yu12Buffer + yDInfo.yWidth * yDInfo.yHeight + yDInfo.uvWidth * yDInfo.uvHeight;
    int dstStrideV = yDInfo.uvStride / EVEN_ODD_DIVISOR;

    int ret = converter.NV21ToI420(srcY, yDInfo.yStride, srcVU, srcStrideVu, yu12Buffer, yDInfo.yWidth, dstU,
        dstStrideU, dstV, dstStrideV, yDInfo.yWidth, yDInfo.yHeight);
    if (ret != 0) {
        IMAGE_LOGE("NV21ToI420 failed, ret = %{public}d!", ret);
        delete[] yu12Buffer;
        return false;
    }
    ret = converter.I420ToRGB565Matrix(yu12Buffer, yDInfo.yWidth, dstU, dstStrideU, dstV, dstStrideV, *destBuffer,
        yDInfo.yWidth * NUM_2, static_cast<OHOS::OpenSourceLibyuv::ColorSpace>(colorSpace),
        yDInfo.yWidth, yDInfo.yHeight);
    delete[] yu12Buffer;
    if (ret != 0) {
        IMAGE_LOGE("I420ToRGB565Matrix failed, ret = %{public}d!", ret);
        return false;
    }
    return true;
}

static bool NV21ToI420ToBGRA(const uint8_t *srcBuffer, const YUVDataInfo &yDInfo, uint8_t **destBuffer)
{
    auto &converter = ConverterHandle::GetInstance().GetHandle();
    uint8_t *srcY = const_cast<uint8_t *>(srcBuffer) + yDInfo.yOffset;
    uint8_t *srcVU = const_cast<uint8_t *>(srcBuffer) + yDInfo.uvOffset;
    int srcStrideVu = (yDInfo.uvStride + 1) / EVEN_ODD_DIVISOR * EVEN_ODD_DIVISOR;

    int ret = converter.NV21ToARGB(srcY, yDInfo.yStride, srcVU, srcStrideVu, *destBuffer,
        yDInfo.yWidth * NUM_4, yDInfo.yWidth, yDInfo.yHeight);
    if (ret != 0) {
        IMAGE_LOGE("NV21ToARGB failed, ret= %{public}d!", ret);
        return false;
    }
    return true;
}

static bool NV21ToNV12Auto(const uint8_t *srcBuffer, const YUVDataInfo &yDInfo, uint8_t **destBuffer,
    const size_t &destBufferSize)
{
    int32_t width = yDInfo.yWidth;
    int32_t height = yDInfo.yHeight;

    if (destBufferSize <=0 || destBufferSize > PIXEL_MAP_MAX_RAM_SIZE) {
        IMAGE_LOGE("Invalid destination buffer size calculation!");
        return false;
    }
    uint8_t *tempBuffer = new (std::nothrow) uint8_t[destBufferSize];
    if (tempBuffer == nullptr) {
        return false;
    }
    uint8_t *tempY = tempBuffer;
    uint8_t *tempU = tempBuffer + width * height;
    uint8_t *tempV = tempU + ((width + 1) / EVEN_ODD_DIVISOR) * ((height + 1) / EVEN_ODD_DIVISOR);
    auto &converter = ConverterHandle::GetInstance().GetHandle();
    int32_t tempRet =
        converter.NV21ToI420(srcBuffer + yDInfo.yOffset, yDInfo.yStride, srcBuffer + yDInfo.uvOffset, yDInfo.uvStride,
        tempY, width, tempU, (width + 1) / EVEN_ODD_DIVISOR, tempV, (width + 1) / EVEN_ODD_DIVISOR, width, height);
    if (tempRet != 0) {
        delete[] tempBuffer;
        return false;
    }
    tempRet =
        converter.I420ToNV12(tempY, width, tempU, (width + 1) / EVEN_ODD_DIVISOR, tempV, (width + 1) / EVEN_ODD_DIVISOR,
        *destBuffer, width, *destBuffer + width * height, ((width + 1) / EVEN_ODD_DIVISOR) * TWO_SLICES, width, height);
    delete[] tempBuffer;
    return tempRet == 0 ? true : false;
}

static bool NV12ToNV21Auto(const uint8_t *srcBuffer, const YUVDataInfo &yDInfo, uint8_t **destBuffer,
    const size_t &destBufferSize)
{
    if (destBufferSize <= 0 || destBufferSize > PIXEL_MAP_MAX_RAM_SIZE) {
        IMAGE_LOGE("Invalid destination buffer size calculation!");
        return false;
    }
    uint8_t *tempBuffer = new (std::nothrow) uint8_t[destBufferSize];
    if (tempBuffer == nullptr) {
        return false;
    }
    int32_t width = yDInfo.yWidth;
    int32_t height = yDInfo.yHeight;
    uint8_t *tempY = tempBuffer;
    uint8_t *tempU = tempBuffer + width * height;
    uint8_t *tempV = tempU + ((width + 1) / EVEN_ODD_DIVISOR) * ((height + 1) / EVEN_ODD_DIVISOR);
    auto &converter = ConverterHandle::GetInstance().GetHandle();
    int32_t tempRet =
        converter.NV12ToI420(srcBuffer + yDInfo.yOffset, yDInfo.yStride, srcBuffer + yDInfo.uvOffset, yDInfo.uvStride,
        tempY, width, tempU, (width + 1) / EVEN_ODD_DIVISOR, tempV, (width + 1) / EVEN_ODD_DIVISOR, width, height);
    if (tempRet != 0) {
        delete[] tempBuffer;
        return false;
    }
    int32_t result =
        converter.I420ToNV21(tempY, width, tempU, (width + 1) / EVEN_ODD_DIVISOR, tempV, (width + 1) / EVEN_ODD_DIVISOR,
        *destBuffer, width, *destBuffer + width * height, ((width + 1) / EVEN_ODD_DIVISOR) * TWO_SLICES, width, height);
    delete[] tempBuffer;
    return result == 0 ? true : false;
}

bool ImageFormatConvertExtUtils::NV12ToRGB565(const uint8_t *srcBuffer, const YUVDataInfo &yDInfo,
    uint8_t **destBuffer, size_t &destBufferSize, [[maybe_unused]] ColorSpace colorSpace)
{
    if (srcBuffer == nullptr || destBuffer == nullptr || yDInfo.yWidth == 0 || yDInfo.yHeight == 0 ||
        yDInfo.uvWidth == 0 || yDInfo.uvHeight == 0) {
        return false;
    }

    destBufferSize = static_cast<size_t>(yDInfo.yWidth * yDInfo.yHeight * BYTES_PER_PIXEL_RGB565);
    if (destBufferSize <= 0 || destBufferSize > PIXEL_MAP_MAX_RAM_SIZE) {
        IMAGE_LOGE("Invalid destination buffer size calculation!");
        return false;
    }
    *destBuffer = new (std::nothrow) uint8_t[destBufferSize]();
    if (*destBuffer == nullptr) {
        IMAGE_LOGE("apply space for dest buffer failed!");
        return false;
    }

    const uint8_t *srcY = srcBuffer + yDInfo.yOffset;
    const uint8_t *srcUV = srcBuffer + yDInfo.uvOffset;

    int dstStrideRGB565 = yDInfo.yWidth * BYTES_PER_PIXEL_RGB565;
    int widthEven = (yDInfo.yWidth + 1) / EVEN_ODD_DIVISOR * TWO_SLICES;
    auto &converter = ConverterHandle::GetInstance().GetHandle();
    int32_t result = converter.NV12ToRGB565(srcY, yDInfo.yStride, srcUV, widthEven, *destBuffer, dstStrideRGB565,
        yDInfo.yWidth, yDInfo.yHeight);
    if (result) {
        delete[](*destBuffer);
        IMAGE_LOGE("NV12 auto conversion to RGB565 failed!");
        return false;
    }
    return true;
}

bool ImageFormatConvertExtUtils::NV21ToNV12(const uint8_t *srcBuffer, const YUVDataInfo &yDInfo,
    uint8_t **destBuffer, size_t &destBufferSize, [[maybe_unused]] ColorSpace colorSpace)
{
    if (srcBuffer == nullptr || destBuffer == nullptr || yDInfo.yWidth == 0 || yDInfo.yHeight == 0 ||
        yDInfo.uvWidth == 0 || yDInfo.uvHeight == 0) {
        return false;
    }

    destBufferSize = yDInfo.yWidth * yDInfo.yHeight + yDInfo.uvWidth * TWO_SLICES * yDInfo.uvHeight;
    if (destBufferSize <= 0 || destBufferSize > PIXEL_MAP_MAX_RAM_SIZE) {
        IMAGE_LOGE("Invalid destination buffer size calculation!");
        return false;
    }
    *destBuffer = new (std::nothrow) uint8_t[destBufferSize]();
    if (*destBuffer == nullptr) {
        IMAGE_LOGE("apply space for dest buffer failed!");
        return false;
    }
    bool result = NV21ToNV12Auto(srcBuffer, yDInfo, destBuffer, destBufferSize);
    if (!result) {
        delete[](*destBuffer);
        IMAGE_LOGE("NV21 conversion to NV12 failed!");
    }
    return result;
}

bool ImageFormatConvertExtUtils::NV12ToNV21(const uint8_t *srcBuffer, const YUVDataInfo &yDInfo,
    uint8_t **destBuffer, size_t &destBufferSize, [[maybe_unused]] ColorSpace colorSpace)
{
    if (srcBuffer == nullptr || destBuffer == nullptr || yDInfo.yWidth == 0 || yDInfo.yHeight == 0 ||
        yDInfo.uvWidth == 0 || yDInfo.uvHeight == 0) {
        return false;
    }
    destBufferSize = yDInfo.yWidth * yDInfo.yHeight + yDInfo.yWidth * EVEN_ODD_DIVISOR * yDInfo.uvHeight;
    if (destBufferSize <= 0 || destBufferSize > PIXEL_MAP_MAX_RAM_SIZE) {
        IMAGE_LOGE("Invalid destination buffer size calculation!");
        return false;
    }
    *destBuffer = new (std::nothrow) uint8_t[destBufferSize]();
    if (*destBuffer == nullptr) {
        IMAGE_LOGE("apply space for dest buffer failed!");
        return false;
    }
    bool result = NV12ToNV21Auto(srcBuffer, yDInfo, destBuffer, destBufferSize);
    if (!result) {
        delete[](*destBuffer);
        IMAGE_LOGE("NV12 conversion to NV21 failed!");
    }
    return result;
}

bool ImageFormatConvertExtUtils::BGRAToNV12(const uint8_t *srcBuffer, const RGBDataInfo &rgbInfo,
    uint8_t **destBuffer, size_t &destBufferSize, [[maybe_unused]] ColorSpace colorSpace)
{
    if (srcBuffer == nullptr || destBuffer == nullptr || rgbInfo.width < 0 || rgbInfo.height < 0) {
        return false;
    }
    size_t destPlaneSizeY = rgbInfo.width * rgbInfo.height;
    size_t srcPlaneSizeUV = ((rgbInfo.width + NUM_1) / NUM_2) * ((rgbInfo.height + NUM_1) / NUM_2);
    destBufferSize = static_cast<size_t>(destPlaneSizeY + srcPlaneSizeUV * NUM_2);
    if (destBufferSize <= NUM_0 || destBufferSize > PIXEL_MAP_MAX_RAM_SIZE) {
        IMAGE_LOGE("Invalid destination buffer size calculation!");
        return false;
    }
    *destBuffer = new (std::nothrow) uint8_t[destBufferSize]();
    if (*destBuffer == nullptr) {
        IMAGE_LOGE("apply space for dest buffer failed!");
        return false;
    }
    uint8_t *nv12Y = *destBuffer;
    uint8_t *nv12UV = *destBuffer + destPlaneSizeY;
    auto &converter = ConverterHandle::GetInstance().GetHandle();
    converter.ARGBToNV12(srcBuffer, rgbInfo.width * NUM_4, nv12Y, rgbInfo.width, nv12UV,
        (rgbInfo.width + NUM_1) / NUM_2 * NUM_2, rgbInfo.width, rgbInfo.height);
    return true;
}

bool ImageFormatConvertExtUtils::RGB565ToNV12(const uint8_t *srcBuffer, const RGBDataInfo &rgbInfo,
    uint8_t **destBuffer, size_t &destBufferSize, [[maybe_unused]] ColorSpace colorSpace)
{
    if (srcBuffer == nullptr || destBuffer == nullptr || rgbInfo.width < 0 || rgbInfo.height < 0) {
        IMAGE_LOGE("Input parameters not compliant!");
        return false;
    }
    destBufferSize = static_cast<size_t>(rgbInfo.width * rgbInfo.height +
        ((rgbInfo.width + NUM_1) / NUM_2) * ((rgbInfo.height + NUM_1) / NUM_2) * NUM_2);
    if (destBufferSize <= NUM_0 || destBufferSize > PIXEL_MAP_MAX_RAM_SIZE) {
        IMAGE_LOGE("Invalid destination buffer size calculation!");
        return false;
    }
    *destBuffer = new (std::nothrow) uint8_t[destBufferSize]();
    if (*destBuffer == nullptr) {
        IMAGE_LOGE("apply space for dest buffer failed!");
        return false;
    }
    uint32_t yu12BufferSize = rgbInfo.width * rgbInfo.height +
        ((rgbInfo.width + NUM_1) / NUM_2) * ((rgbInfo.height + NUM_1) / NUM_2) * NUM_2;
    if (yu12BufferSize <= 0 || yu12BufferSize > PIXEL_MAP_MAX_RAM_SIZE) {
        IMAGE_LOGE("Invalid destination buffer size calculation!");
        return false;
    }
    uint8_t *yu12Buffer = new (std::nothrow) uint8_t[yu12BufferSize];
    if (yu12Buffer == nullptr) {
        IMAGE_LOGE("apply space for dest buffer failed!");
        return false;
    }
    auto &converter = ConverterHandle::GetInstance().GetHandle();
    converter.RGB565ToI420(srcBuffer, rgbInfo.width * NUM_2, yu12Buffer, rgbInfo.width,
        yu12Buffer + rgbInfo.width * rgbInfo.height, (rgbInfo.width + NUM_1) / NUM_2,
        yu12Buffer + rgbInfo.width * rgbInfo.height +
        ((rgbInfo.width + NUM_1) / NUM_2) * ((rgbInfo.height + NUM_1) / NUM_2),
        (rgbInfo.width + NUM_1) / NUM_2, rgbInfo.width, rgbInfo.height);

    converter.I420ToNV12(yu12Buffer, rgbInfo.width, yu12Buffer + rgbInfo.width * rgbInfo.height,
        (rgbInfo.width + NUM_1) / NUM_2,
        yu12Buffer + rgbInfo.width * rgbInfo.height +
        ((rgbInfo.width + NUM_1) / NUM_2) * ((rgbInfo.height + NUM_1) / NUM_2),
        (rgbInfo.width + NUM_1) / NUM_2, *destBuffer, rgbInfo.width,
        *destBuffer + rgbInfo.width * rgbInfo.height, ((rgbInfo.width + NUM_1) / NUM_2) * NUM_2, rgbInfo.width,
        rgbInfo.height);
    delete[] yu12Buffer;
    return true;
}

bool ImageFormatConvertExtUtils::RGB565ToNV21(const uint8_t *srcBuffer, const RGBDataInfo &rgbInfo,
    uint8_t **destBuffer, size_t &destBufferSize, [[maybe_unused]] ColorSpace colorSpace)
{
    if (srcBuffer == nullptr || destBuffer == nullptr || rgbInfo.width < 0 || rgbInfo.height < 0) {
        IMAGE_LOGE("Input parameters not compliant!");
        return false;
    }
    destBufferSize = static_cast<size_t>(rgbInfo.width * rgbInfo.height +
        ((rgbInfo.width + NUM_1) / NUM_2) * ((rgbInfo.height + NUM_1) / NUM_2) * NUM_2);
    if (destBufferSize <= NUM_0 || destBufferSize > PIXEL_MAP_MAX_RAM_SIZE) {
        IMAGE_LOGE("Invalid destination buffer size calculation!");
        return false;
    }
    *destBuffer = new (std::nothrow) uint8_t[destBufferSize]();
    if (*destBuffer == nullptr) {
        IMAGE_LOGE("apply space for dest buffer failed!");
        return false;
    }
    uint32_t yu12BufferSize = rgbInfo.width * rgbInfo.height +
        ((rgbInfo.width + NUM_1) / NUM_2) * ((rgbInfo.height + NUM_1) / NUM_2) * NUM_2;
    std::vector<uint8_t> yu12Buffer(yu12BufferSize, 0);

    auto &converter = ConverterHandle::GetInstance().GetHandle();
    converter.RGB565ToI420(srcBuffer, rgbInfo.width * NUM_2, yu12Buffer.data(), rgbInfo.width,
        yu12Buffer.data() + rgbInfo.width * rgbInfo.height, (rgbInfo.width + NUM_1) / NUM_2,
        yu12Buffer.data() + rgbInfo.width * rgbInfo.height +
        ((rgbInfo.width + NUM_1) / NUM_2) * ((rgbInfo.height + NUM_1) / NUM_2),
        (rgbInfo.width + NUM_1) / NUM_2, rgbInfo.width, rgbInfo.height);
    converter.I420ToNV21(yu12Buffer.data(), rgbInfo.width, yu12Buffer.data() + rgbInfo.width * rgbInfo.height,
        (rgbInfo.width + NUM_1) / NUM_2,
        yu12Buffer.data() + rgbInfo.width * rgbInfo.height +
        ((rgbInfo.width + NUM_1) / NUM_2) * ((rgbInfo.height + NUM_1) / NUM_2),
        (rgbInfo.width + NUM_1) / NUM_2, *destBuffer, rgbInfo.width,
        *destBuffer + rgbInfo.width * rgbInfo.height, ((rgbInfo.width + NUM_1) / NUM_2) * NUM_2, rgbInfo.width,
        rgbInfo.height);
    return true;
}

bool ImageFormatConvertExtUtils::BGRAToNV21(const uint8_t *srcBuffer, const RGBDataInfo &rgbInfo,
    uint8_t **destBuffer, size_t &destBufferSize, [[maybe_unused]] ColorSpace colorSpace)
{
    if (srcBuffer == nullptr || destBuffer == nullptr || rgbInfo.width < NUM_0 || rgbInfo.height < NUM_0) {
        return false;
    }
    const int32_t destPlaneSizeY = rgbInfo.width * rgbInfo.height;
    const int32_t destPlaneSizeVU = ((rgbInfo.width + NUM_1) / NUM_2) * ((rgbInfo.height + NUM_1) / NUM_2);
    destBufferSize = static_cast<size_t>(destPlaneSizeY + destPlaneSizeVU * NUM_2);
    if (destBufferSize == 0 || destBufferSize > PIXEL_MAP_MAX_RAM_SIZE) {
        IMAGE_LOGE("Invalid destination buffer size calculation!");
        return false;
    }
    *destBuffer = new (std::nothrow) uint8_t[destBufferSize]();
    if (*destBuffer == nullptr) {
        IMAGE_LOGE("apply space for dest buffer failed!");
        return false;
    }
    uint8_t *nv21Y = *destBuffer;
    uint8_t *nv21VU = *destBuffer + destPlaneSizeY;
    auto &converter = ConverterHandle::GetInstance().GetHandle();
    converter.ARGBToNV21(srcBuffer, rgbInfo.width * NUM_4, nv21Y, rgbInfo.width, nv21VU,
        (rgbInfo.width + NUM_1) / NUM_2 * NUM_2, rgbInfo.width, rgbInfo.height);
    return true;
}

bool ImageFormatConvertExtUtils::NV12ToRGBA(const uint8_t *data, const YUVDataInfo &yDInfo, uint8_t **destBuffer,
    size_t &destBufferSize, [[maybe_unused]] ColorSpace colorSpace)
{
    if (data == nullptr || destBuffer == nullptr) {
        IMAGE_LOGE("Input buffer pointer data or destBuffer is null!");
        return false;
    }

    destBufferSize = static_cast<size_t>(yDInfo.yWidth * yDInfo.yHeight * BYTES_PER_PIXEL_RGBA);
    if (destBufferSize == 0 || destBufferSize > PIXEL_MAP_MAX_RAM_SIZE) {
        IMAGE_LOGE("Invalid destination buffer size is 0!");
        return false;
    }

    *destBuffer = new (std::nothrow) uint8_t[destBufferSize]();
    if (*destBuffer == nullptr) {
        IMAGE_LOGE("Dynamically allocating memory for destination buffer failed!");
        return false;
    }
    bool bRet = NV12ToRGBANoManual(data, yDInfo, destBuffer, colorSpace);
    if (!bRet) {
        IMAGE_LOGE("NV12ToRGBA failed!");
        delete[](*destBuffer);
        *destBuffer = nullptr;
        return false;
    }

    return true;
}

bool ImageFormatConvertExtUtils::NV12ToBGRA(const uint8_t *data, const YUVDataInfo &yDInfo, uint8_t **destBuffer,
    size_t &destBufferSize, [[maybe_unused]] ColorSpace colorSpace)
{
    if (data == nullptr || destBuffer == nullptr) {
        IMAGE_LOGE("Input buffer pointer data or destBuffer is null!");
        return false;
    }

    destBufferSize = static_cast<size_t>(yDInfo.yWidth * yDInfo.yHeight * BYTES_PER_PIXEL_BGRA);
    if (destBufferSize == 0 || destBufferSize > PIXEL_MAP_MAX_RAM_SIZE) {
        IMAGE_LOGE("Invalid destination buffer size is 0!");
        return false;
    }
    *destBuffer = new (std::nothrow) uint8_t[destBufferSize]();
    if (*destBuffer == nullptr) {
        IMAGE_LOGE("Dynamically allocating memory for destination buffer failed!");
        return false;
    }
    bool bRet = NV12ToBGRANoManual(data, yDInfo, destBuffer, colorSpace);
    if (!bRet) {
        IMAGE_LOGE("NV12ToBGRA failed!");
        return false;
    }

    return true;
}

bool ImageFormatConvertExtUtils::RGBAToNV12(const uint8_t *srcBuffer, const RGBDataInfo &rgbInfo,
    uint8_t **destBuffer, size_t &destBufferSize, [[maybe_unused]] ColorSpace colorSpace)
{
    if (srcBuffer == nullptr || destBuffer == nullptr || rgbInfo.width < NUM_0 || rgbInfo.height < NUM_0) {
        return false;
    }
    size_t destPlaneSizeY = rgbInfo.width * rgbInfo.height;
    size_t destPlaneSizeUV = ((rgbInfo.width + NUM_1) / NUM_2) * ((rgbInfo.height + NUM_1) / NUM_2);
    destBufferSize = destPlaneSizeY + destPlaneSizeUV * NUM_2;
    if (destBufferSize <= NUM_0 || destBufferSize > PIXEL_MAP_MAX_RAM_SIZE) {
        IMAGE_LOGE("Invalid destination buffer size calculation!");
        return false;
    }
    *destBuffer = new (std::nothrow) uint8_t[destBufferSize]();
    if (*destBuffer == nullptr) {
        IMAGE_LOGE("apply space for dest buffer failed!");
        return false;
    }
    const uint32_t i420BufferSize = static_cast<size_t>(rgbInfo.width * rgbInfo.height +
        ((rgbInfo.width + NUM_1) / NUM_2) * ((rgbInfo.height + NUM_1) / NUM_2) * NUM_2);
    if (i420BufferSize <= NUM_0 || i420BufferSize > PIXEL_MAP_MAX_RAM_SIZE) {
        IMAGE_LOGE("Invalid destination buffer size calculation!");
        return false;
    }
    uint8_t *i420Buffer = new (std::nothrow) uint8_t[i420BufferSize];
    if (i420Buffer == nullptr) {
        IMAGE_LOGE("apply space for dest buffer failed!");
        return false;
    }
    uint8_t *i420Y = i420Buffer;
    uint8_t *i420U = i420Buffer + rgbInfo.width * rgbInfo.height;
    uint8_t *i420V = i420Buffer + rgbInfo.width * rgbInfo.height +
        ((rgbInfo.width + NUM_1) / NUM_2) * ((rgbInfo.height + NUM_1) / NUM_2);
    uint8_t *nv12Y = *destBuffer;
    uint8_t *nv12UV = *destBuffer + rgbInfo.width * rgbInfo.height;
    auto &converter = ConverterHandle::GetInstance().GetHandle();
    converter.ABGRToI420(srcBuffer, rgbInfo.stride, i420Y, rgbInfo.width, i420U,
        (rgbInfo.width + NUM_1) / NUM_2, i420V, (rgbInfo.width + NUM_1) / NUM_2, rgbInfo.width, rgbInfo.height);
    converter.I420ToNV12(i420Y, rgbInfo.width, i420U, (rgbInfo.width + NUM_1) / NUM_2, i420V,
        (rgbInfo.width + NUM_1) / NUM_2, nv12Y, rgbInfo.width, nv12UV, (rgbInfo.width + NUM_1) / NUM_2 * NUM_2,
        rgbInfo.width, rgbInfo.height);
    delete[] i420Buffer;
    return true;
}

bool ImageFormatConvertExtUtils::RGBAToNV21(const uint8_t *srcBuffer, const RGBDataInfo &rgbInfo,
    uint8_t **destBuffer, size_t &destBufferSize, [[maybe_unused]] ColorSpace colorSpace)
{
    if (srcBuffer == nullptr || destBuffer == nullptr || rgbInfo.width < NUM_0 || rgbInfo.height < NUM_0) {
        return false;
    }
    size_t destPlaneSizeY = rgbInfo.width * rgbInfo.height;
    size_t destPlaneSizeVU = ((rgbInfo.width + NUM_1) / NUM_2) * ((rgbInfo.height + NUM_1) / NUM_2);
    destBufferSize = static_cast<size_t>(destPlaneSizeY + destPlaneSizeVU * NUM_2);
    if (destBufferSize <= NUM_0 || destBufferSize > PIXEL_MAP_MAX_RAM_SIZE) {
        IMAGE_LOGE("Invalid destination buffer size calculation!");
        return false;
    }
    *destBuffer = new (std::nothrow) uint8_t[destBufferSize]();
    if (*destBuffer == nullptr) {
        IMAGE_LOGE("apply space for dest buffer failed!");
        return false;
    }

    const uint32_t i420BufferSize = static_cast<size_t>(rgbInfo.width * rgbInfo.height +
        ((rgbInfo.width + NUM_1) / NUM_2) * ((rgbInfo.height + NUM_1) / NUM_2) * NUM_2);
    if (i420BufferSize <= NUM_0 || i420BufferSize > PIXEL_MAP_MAX_RAM_SIZE) {
        IMAGE_LOGE("Invalid destination buffer size calculation!");
        return false;
    }
    uint8_t *i420Buffer = new (std::nothrow) uint8_t[i420BufferSize];
    if (i420Buffer == nullptr) {
        IMAGE_LOGE("apply space for dest buffer failed!");
        return false;
    }
    uint8_t *i420Y = i420Buffer;
    uint8_t *i420U = i420Buffer + rgbInfo.width * rgbInfo.height;
    uint8_t *i420V = i420Buffer + rgbInfo.width * rgbInfo.height +
        ((rgbInfo.width + NUM_1) / NUM_2) * ((rgbInfo.height + NUM_1) / NUM_2);
    uint8_t *nv21Y = *destBuffer;
    uint8_t *nv21VU = *destBuffer + rgbInfo.width * rgbInfo.height;
    auto &converter = ConverterHandle::GetInstance().GetHandle();
    converter.ABGRToI420(srcBuffer, rgbInfo.stride, i420Y, rgbInfo.width, i420U,
        (rgbInfo.width + NUM_1) / NUM_2, i420V, (rgbInfo.width + NUM_1) / NUM_2, rgbInfo.width, rgbInfo.height);
    converter.I420ToNV21(i420Y, rgbInfo.width, i420U, (rgbInfo.width + NUM_1) / NUM_2, i420V,
        (rgbInfo.width + NUM_1) / NUM_2, nv21Y, rgbInfo.width, nv21VU, (rgbInfo.width + NUM_1) / NUM_2 * NUM_2,
        rgbInfo.width, rgbInfo.height);
    delete[] i420Buffer;
    return true;
}

bool ImageFormatConvertExtUtils::RGBToNV21(const uint8_t *srcBuffer, const RGBDataInfo &rgbInfo,
    uint8_t **destBuffer, size_t &destBufferSize, [[maybe_unused]] ColorSpace colorSpace)
{
    if (srcBuffer == nullptr || destBuffer == nullptr || rgbInfo.width < NUM_0 || rgbInfo.height < NUM_0) {
        return false;
    }
    destBufferSize = static_cast<size_t>(rgbInfo.width * rgbInfo.height +
        ((rgbInfo.width + NUM_1) / NUM_2 * ((rgbInfo.height + NUM_1) / NUM_2) * NUM_2));
    if (destBufferSize <= NUM_0 || destBufferSize > PIXEL_MAP_MAX_RAM_SIZE) {
        IMAGE_LOGE("Invalid destination buffer size calculation!");
        return false;
    }
    *destBuffer = new (std::nothrow) uint8_t[destBufferSize]();
    if (*destBuffer == nullptr) {
        IMAGE_LOGE("apply space for dest buffer failed!");
        return false;
    }
    const uint32_t yu12BufferSize = static_cast<size_t>(rgbInfo.width * rgbInfo.height +
        ((rgbInfo.width + NUM_1) / NUM_2) * ((rgbInfo.height + NUM_1) / NUM_2) * NUM_2);
    if (yu12BufferSize <= NUM_0 || yu12BufferSize > PIXEL_MAP_MAX_RAM_SIZE) {
        IMAGE_LOGE("Invalid destination buffer size calculation!");
        return false;
    }
    uint8_t *yu12Buffer = new (std::nothrow) uint8_t[yu12BufferSize];
    if (yu12Buffer == nullptr) {
        IMAGE_LOGE("apply space for I420 buffer failed!");
        return false;
    }
    uint8_t *I420Y = yu12Buffer;
    uint8_t *I420U = yu12Buffer + rgbInfo.height * rgbInfo.width;
    uint8_t *I420V = yu12Buffer + rgbInfo.height * rgbInfo.width +
        ((rgbInfo.width + NUM_1) / NUM_2) * ((rgbInfo.height + NUM_1) / NUM_2);
    auto &converter = ConverterHandle::GetInstance().GetHandle();
    converter.RGB24ToI420(srcBuffer, rgbInfo.width * NUM_3, I420Y, rgbInfo.width, I420U,
        (rgbInfo.width + NUM_1) / NUM_2, I420V, (rgbInfo.width + NUM_1) / NUM_2, rgbInfo.width, rgbInfo.height);
    converter.I420ToNV21(I420Y, rgbInfo.width, I420U, (rgbInfo.width + NUM_1) / NUM_2, I420V,
        (rgbInfo.width + NUM_1) / NUM_2, *destBuffer, rgbInfo.width,
        *destBuffer + rgbInfo.width * rgbInfo.height + NUM_1, (rgbInfo.width + NUM_1) / NUM_2 * NUM_2,
        rgbInfo.width, rgbInfo.height);
    delete[] yu12Buffer;
    return true;
}

static bool NV12ToRGBNoManual(const uint8_t *srcBuffer, const YUVDataInfo &yDInfo, uint8_t **destBuffer,
    ColorSpace colorSpace)
{
    auto &converter = ConverterHandle::GetInstance().GetHandle();
    int32_t ret = converter.NV12ToRAW(srcBuffer + yDInfo.yOffset, yDInfo.yStride, srcBuffer + yDInfo.uvOffset,
        yDInfo.uvStride, *destBuffer, yDInfo.yStride * BYTES_PER_PIXEL_RGB,
        yDInfo.yWidth, yDInfo.yHeight);
    if (ret != 0) {
        IMAGE_LOGE("NV12ToRGB24Matrix failed, ret = %{public}d!", ret);
        return false;
    }
    return true;
}

bool ImageFormatConvertExtUtils::NV12ToRGB(const uint8_t *data, const YUVDataInfo &yDInfo, uint8_t **destBuffer,
    size_t &destBufferSize, [[maybe_unused]] ColorSpace colorSpace)
{
    if (data == nullptr || destBuffer == nullptr) {
        IMAGE_LOGE("Input buffer pointer data or destBuffer is null!");
        return false;
    }

    destBufferSize = static_cast<size_t>(yDInfo.yWidth * yDInfo.yHeight * BYTES_PER_PIXEL_RGB);
    if (destBufferSize == 0 || destBufferSize > PIXEL_MAP_MAX_RAM_SIZE) {
        IMAGE_LOGE("Invalid destination buffer size is 0!");
        return false;
    }

    (*destBuffer) = new (std::nothrow) uint8_t[destBufferSize]();
    if (*destBuffer == nullptr) {
        IMAGE_LOGE("Dynamically allocating memory for destination buffer failed!");
        return false;
    }

    bool bRet = NV12ToRGBNoManual(data, yDInfo, destBuffer, colorSpace);
    if (!bRet) {
        IMAGE_LOGE("NV12ToRGB failed!");
        delete[](*destBuffer);
        *destBuffer = nullptr;
        return false;
    }
    return true;
}

bool ImageFormatConvertExtUtils::NV21ToRGB(const uint8_t *data, const YUVDataInfo &yDInfo, uint8_t **destBuffer,
    size_t &destBufferSize, ColorSpace colorSpace)
{
    if (data == nullptr || destBuffer == nullptr || yDInfo.yWidth == 0 || yDInfo.yHeight == 0 || yDInfo.uvWidth == 0 ||
        yDInfo.uvHeight == 0) {
        return false;
    }

    destBufferSize = static_cast<size_t>(yDInfo.yWidth * yDInfo.yHeight * BYTES_PER_PIXEL_RGB);
    if (destBufferSize == 0 || destBufferSize > PIXEL_MAP_MAX_RAM_SIZE) {
        IMAGE_LOGE("Invalid destination buffer size is 0!");
        return false;
    }
    *destBuffer = new (std::nothrow) uint8_t[destBufferSize]();
    if (*destBuffer == nullptr) {
        IMAGE_LOGE("apply space for dest buffer failed!");
        return false;
    }
    auto &converter = ConverterHandle::GetInstance().GetHandle();
    converter.NV21ToRAW(data + yDInfo.yOffset, yDInfo.yStride,
        data + yDInfo.uvOffset, yDInfo.uvStride,
        *destBuffer, yDInfo.yWidth * BYTES_PER_PIXEL_RGB,
        yDInfo.yWidth, yDInfo.yHeight);
    return true;
}

bool ImageFormatConvertExtUtils::NV21ToRGBA(const uint8_t *srcBuffer, const YUVDataInfo &yDInfo,
    uint8_t **destBuffer, size_t &destBufferSize, [[maybe_unused]] ColorSpace colorSpace)
{
    if (srcBuffer == nullptr || destBuffer == nullptr || yDInfo.yWidth == 0 || yDInfo.yHeight == 0 ||
        yDInfo.uvWidth == 0 || yDInfo.uvHeight == 0) {
        IMAGE_LOGE("Input parameters not compliant!");
        return false;
    }

    destBufferSize = static_cast<size_t>(yDInfo.yWidth * yDInfo.yHeight * BYTES_PER_PIXEL_RGBA);
    if (destBufferSize == 0 || destBufferSize > PIXEL_MAP_MAX_RAM_SIZE) {
        IMAGE_LOGE("Invalid destination buffer size is 0!");
        return false;
    }
    *destBuffer = new (std::nothrow) uint8_t[destBufferSize]();
    if (*destBuffer == nullptr) {
        IMAGE_LOGE("Apply space for dest buffer failed!");
        return false;
    }
    bool bRet = NV21ToRGBAMatrix(srcBuffer, yDInfo, destBuffer, colorSpace);
    if (!bRet) {
        IMAGE_LOGE("NV21ToRGBA failed!");
        delete[] destBuffer;
        *destBuffer = nullptr;
        return bRet;
    }
    return true;
}

bool ImageFormatConvertExtUtils::NV21ToBGRA(const uint8_t *srcBuffer, const YUVDataInfo &yDInfo,
    uint8_t **destBuffer, size_t &destBufferSize, [[maybe_unused]] ColorSpace colorSpace)
{
    if (srcBuffer == nullptr || destBuffer == nullptr || yDInfo.yWidth == 0 || yDInfo.yHeight == 0 ||
        yDInfo.uvWidth == 0 || yDInfo.uvHeight == 0) {
        IMAGE_LOGE("Input parameters not compliant!");
        return false;
    }
    destBufferSize = static_cast<size_t>(yDInfo.yWidth * yDInfo.yHeight * BYTES_PER_PIXEL_BGRA);
    if (destBufferSize == 0 || destBufferSize > PIXEL_MAP_MAX_RAM_SIZE) {
        IMAGE_LOGE("Invalid destination buffer size is 0!");
        return false;
    }
    *destBuffer = new (std::nothrow) uint8_t[destBufferSize]();
    if (*destBuffer == nullptr) {
        IMAGE_LOGE("Apply space for dest buffer failed!");
        return false;
    }

    bool bRet = NV21ToI420ToBGRA(srcBuffer, yDInfo, destBuffer);
    if (!bRet) {
        IMAGE_LOGE("NV21ToBGRA failed!");
        delete[] destBuffer;
        *destBuffer = nullptr;
        return bRet;
    }
    return true;
}

bool ImageFormatConvertExtUtils::NV21ToRGB565(const uint8_t *srcBuffer, const YUVDataInfo &yDInfo,
    uint8_t **destBuffer, size_t &destBufferSize, ColorSpace colorSpace)
{
    if (srcBuffer == nullptr || destBuffer == nullptr || yDInfo.yWidth == 0 || yDInfo.yHeight == 0 ||
        yDInfo.uvWidth == 0 || yDInfo.uvHeight == 0) {
        IMAGE_LOGE("Input parameters not compliant!");
        return false;
    }

    destBufferSize = static_cast<size_t>(yDInfo.yWidth * yDInfo.yHeight * BYTES_PER_PIXEL_RGB565);
    if (destBufferSize == 0 || destBufferSize > PIXEL_MAP_MAX_RAM_SIZE) {
        IMAGE_LOGE("Invalid destination buffer size is 0!");
        return false;
    }
    *destBuffer = new (std::nothrow) uint8_t[destBufferSize]();
    if (*destBuffer == nullptr) {
        IMAGE_LOGE("apply space for dest buffer failed!");
        return false;
    }
    bool bRet = NV21ToRGB565Matrix(srcBuffer, yDInfo, destBuffer, colorSpace);
    if (!bRet) {
        IMAGE_LOGE("NV21ToRGB565 failed!");
        delete[] destBuffer;
        *destBuffer = nullptr;
        return bRet;
    }
    return true;
}

bool ImageFormatConvertExtUtils::RGBToNV12(const uint8_t *srcBuffer, const RGBDataInfo &rgbInfo,
    uint8_t **destBuffer, size_t &destBufferSize, [[maybe_unused]] ColorSpace colorSpace)
{
    if (srcBuffer == nullptr || destBuffer == nullptr || rgbInfo.width < NUM_0 || rgbInfo.height < NUM_0) {
        return false;
    }
    destBufferSize = static_cast<size_t>(rgbInfo.width * rgbInfo.height +
        (rgbInfo.width + NUM_1) / NUM_2 * (rgbInfo.height + NUM_1) / NUM_2 * NUM_2);
    if (destBufferSize <= NUM_0 || destBufferSize > PIXEL_MAP_MAX_RAM_SIZE) {
        IMAGE_LOGE("Invalid destination buffer size calculation!");
        return false;
    }
    (*destBuffer) = new (std::nothrow) uint8_t[destBufferSize]();
    if (*destBuffer == nullptr) {
        IMAGE_LOGE("apply space for dest buffer failed!");
        return false;
    }

    uint8_t *yu12Buffer(new (std::nothrow) uint8_t[rgbInfo.width * rgbInfo.height +
        (rgbInfo.width + NUM_1) / NUM_2 * (rgbInfo.height + NUM_1) / NUM_2 * NUM_2]());
    if (yu12Buffer == nullptr) {
        IMAGE_LOGE("apply space for I420 buffer failed!");
        return false;
    }
    auto &converter = ConverterHandle::GetInstance().GetHandle();
    converter.RGB24ToI420(srcBuffer, rgbInfo.width * NUM_3, yu12Buffer, rgbInfo.width,
        yu12Buffer + rgbInfo.width * rgbInfo.height, (rgbInfo.width + NUM_1) / NUM_2,
        yu12Buffer + rgbInfo.width * rgbInfo.height +
        (rgbInfo.width + NUM_1) / NUM_2 * (rgbInfo.height + NUM_1) / NUM_2,
        (rgbInfo.width + NUM_1) / NUM_2, rgbInfo.width, rgbInfo.height);
    converter.I420ToNV12(yu12Buffer, rgbInfo.width, yu12Buffer + rgbInfo.width * rgbInfo.height,
        (rgbInfo.width + NUM_1) / NUM_2,
        yu12Buffer + rgbInfo.width * rgbInfo.height +
        (rgbInfo.width + NUM_1) / NUM_2 * (rgbInfo.height + NUM_1) / NUM_2,
        (rgbInfo.width + NUM_1) / NUM_2, *destBuffer, rgbInfo.width,
        *destBuffer + rgbInfo.width * rgbInfo.height + NUM_1, (rgbInfo.width + NUM_1) / NUM_2 * NUM_2,
        rgbInfo.width, rgbInfo.height);
    return true;
}

static bool RGBToI420(SrcConvertParam &srcParam, I420Info &i420)
{
    auto &converter = ConverterHandle::GetInstance().GetHandle();
    switch (srcParam.format) {
        case PixelFormat::RGB_888:
            converter.RGB24ToI420(srcParam.slice[0], srcParam.stride[0], i420.I420Y, i420.yStride, i420.I420U,
                i420.uStride, i420.I420V, i420.vStride, srcParam.width, srcParam.height);
            break;
        case PixelFormat::RGB_565:
            converter.RGB565ToI420(srcParam.slice[0], srcParam.stride[0], i420.I420Y, i420.yStride, i420.I420U,
                i420.uStride, i420.I420V, i420.vStride, srcParam.width, srcParam.height);
            break;
        case PixelFormat::RGBA_8888:
            converter.ABGRToI420(srcParam.slice[0], srcParam.stride[0], i420.I420Y, i420.yStride, i420.I420U,
                i420.uStride, i420.I420V, i420.vStride, srcParam.width, srcParam.height);
            break;
        default:
            return false;
    }
    return true;
}

static bool I420ToYuv(I420Info &i420, DstConvertParam &dstParam)
{
    auto &converter = ConverterHandle::GetInstance().GetHandle();
    switch (dstParam.format) {
        case PixelFormat::NV12:
            converter.I420ToNV12(i420.I420Y, i420.yStride, i420.I420U, i420.uStride, i420.I420V, i420.vStride,
                dstParam.slice[0], dstParam.stride[0], dstParam.slice[1], dstParam.stride[1],
                dstParam.width, dstParam.height);
        case PixelFormat::NV21:
            converter.I420ToNV21(i420.I420Y, i420.yStride, i420.I420U, i420.uStride, i420.I420V, i420.vStride,
                dstParam.slice[0], dstParam.stride[0], dstParam.slice[1], dstParam.stride[1],
                dstParam.width, dstParam.height);
            break;
        }
        default:
            return false;
    }
}

static void RGBToYuvParam(const RGBDataInfo &rgbInfo, SrcConvertParam &srcParam, DstConvertParam &dstParam,
                          DstConvertDataInfo &dstInfo)
{
    srcParam.slice[0] = srcParam.buffer;
    srcParam.stride[0] = static_cast<int>(rgbInfo.stride);
    if (dstInfo.allocType == AllocatorType::DMA_ALLOC) {
        dstParam.stride[0] = static_cast<int>(dstInfo.yStride);
        dstParam.stride[1] = static_cast<int>(dstInfo.uvStride);
        dstParam.slice[0] = dstInfo.buffer + dstInfo.yOffset;
        dstParam.slice[1] = dstInfo.buffer + dstInfo.uvOffset;
    } else {
        IMAGE_LOGE("yyytest YuvToRGBParam CalcRGBStride start");
        int uvStride = (dstParam.width % EVEN_ODD_DIVISOR == 0) ? (dstParam.width) : (dstParam.width + 1);
        dstParam.stride[0] = static_cast<int>(dstParam.width);
        dstParam.stride[1] = static_cast<int>(uvStride);
        dstParam.slice[0] = dstInfo.buffer;
        dstParam.slice[1] = dstInfo.buffer + dstParam.width * dstParam.height;
    }
}

static bool RGBToI420ToYuvParam(const RGBDataInfo &rgbInfo, SrcConvertParam &srcParam, I420Info &i420Info,
                                DstConvertParam &dstParam, DstConvertDataInfo &dstInfo)
{

    RGBToYuvParam(rgbInfo, srcParam, dstParam, dstInfo);

    i420Info.yStride = rgbInfo.width;
    i420Info.uStride = (rgbInfo.width + NUM_1) / NUM_2;
    i420Info.vStride = (rgbInfo.width + NUM_1) / NUM_2;
    i420Info.uvHeight = ((i420Info.height + NUM_1) / NUM_2) * NUM_2;
    const uint32_t i420BufferSize = static_cast<size_t>(i420Info.yStride * i420Info.height +
        i420Info.uStride * i420Info.uvHeight * NUM_2);
    if (i420BufferSize <= NUM_0 || i420BufferSize > PIXEL_MAP_MAX_RAM_SIZE) {
        IMAGE_LOGE("Invalid destination buffer size calculation!");
        return false;
    }
    uint8_t *i420Buffer = new (std::nothrow) uint8_t[i420BufferSize];
    if (i420Buffer == nullptr) {
        IMAGE_LOGE("apply space for I420 buffer failed!");
        return false;
    }
    i420Info.I420Y = i420Buffer;
    i420Info.I420U = i420Info.I420Y + rgbInfo.height * i420Info.yStride;
    i420Info.I420V = i420Info.I420U + i420Info.uStride * ((rgbInfo.height + NUM_1) / NUM_2)};
    return true;
}

static bool RGBToI420ToYuv(const uint8_t *srcBuffer, const RGBDataInfo &rgbInfo, PixelFormat srcFormat,
                           DstConvertDataInfo &dstInfo, PixelFormat dstFormat)
{
    SrcConvertParam srcParam = {rgbInfo.width, rgbInfo.width};
    srcParam.buffer = srcBuffer;
    srcParam.format = srcFormat;

    DstConvertParam dstParam = {rgbInfo.width, rgbInfo.width};
    dstParam.format = dstFormat;

    I420Info i420Info = {rgbInfo.width, rgbInfo.width};

    if (!RGBToI420ToYuvParam(rgbInfo, srcParam, dstParam, i420Info, dstInfo)) {
        IMAGE_LOGE("yyytest RGB conversion to YUV failed!");
        return false;
    }
    auto bRet = RGBToI420(srcParam, i420Info);
    if (!bRet) {
        delete[] i420Info.I420Y;
        return false;
    }
    bRet = I420ToYuv(i420Info, dstParam);
    delete[] i420Info.I420Y;
    IMAGE_LOGE("yyytest RGBToI420ToYuv succ!");
    return bRet;
}

static bool RGBToYuvConverter(SrcConvertParam &srcParam, DstConvertParam &dstParam)
{
    if (srcParam.format != PixelFormat::BGRA_8888) {
        return false;
    }
    auto &converter = ConverterHandle::GetInstance().GetHandle();
    switch (dstParam.format) {
        case PixelFormat::NV12:
            converter.ARGBToNV12(srcParam.slice[0], srcParam.stride[0],
                dstParam.slice[0], dstParam.stride[0], dstParam.slice[0], dstParam.stride[0],
                dstParam.width, dstParam.height);
            break;
        case PixelFormat::NV21:
            converter.ARGBToNV21(srcParam.slice[0], srcParam.stride[0],
                dstParam.slice[0], dstParam.stride[0], dstParam.slice[0], dstParam.stride[0],
                dstParam.width, dstParam.height);
            break;
        default:
            return false;
    }
}

static bool RGBToYuv(const uint8_t *srcBuffer, const RGBDataInfo &rgbInfo, PixelFormat srcFormat,
                     DstConvertDataInfo &dstInfo, PixelFormat dstFormat)
{

    SrcConvertParam srcParam = {rgbInfo.width, rgbInfo.width};
    srcParam.buffer = srcBuffer;
    srcParam.format = srcFormat;

    DstConvertParam dstParam = {rgbInfo.width, rgbInfo.width};
    dstParam.format = dstFormat;

    if (!RGBToYuvParam(rgbInfo, srcParam, dstParam, dstInfo)) {
        IMAGE_LOGE("yyytest RGB conversion to YUV failed!");
        return false;
    }
    return RGBToYuvConverter(srcParam, dstParam);
}

bool ImageFormatConvertExtUtils::BGRAToNV21New(const uint8_t *srcBuffer, const RGBDataInfo &rgbInfo,
                                               DstConvertDataInfo &dstInfo,
                                               [[maybe_unused]] ColorSpace colorSpace)
{
    return RGBToYuv(srcBuffer, rgbInfo, PixelFormat::BGRA_8888, dstInfo, PixelFormat::NV21);
}

bool ImageFormatConvertExtUtils::BGRAToNV12New(const uint8_t *srcBuffer, const RGBDataInfo &rgbInfo,
                                               DstConvertDataInfo &dstInfo,
                                               [[maybe_unused]] ColorSpace colorSpace)
{
    return RGBToYuv(srcBuffer, rgbInfo, PixelFormat::BGRA_8888, dstInfo, PixelFormat::NV12);
}

bool ImageFormatConvertExtUtils::RGB565ToNV21New(const uint8_t *srcBuffer, const RGBDataInfo &rgbInfo,
                                                 DstConvertDataInfo &dstInfo,
                                                 [[maybe_unused]] ColorSpace colorSpace)
{
    return RGBToI420ToYuv(srcBuffer, rgbInfo, PixelFormat::RGB_565, dstInfo, PixelFormat::NV21);
}

bool ImageFormatConvertExtUtils::RGB565ToNV12New(const uint8_t *srcBuffer, const RGBDataInfo &rgbInfo,
                                                 DstConvertDataInfo &dstInfo,
                                                 [[maybe_unused]] ColorSpace colorSpace)
{
    return RGBToI420ToYuv(srcBuffer, rgbInfo, PixelFormat::RGB_565, dstInfo, PixelFormat::NV12);
}

bool ImageFormatConvertExtUtils::RGBToNV21New(const uint8_t *srcBuffer, const RGBDataInfo &rgbInfo,
                                              DstConvertDataInfo &dstInfo,
                                              [[maybe_unused]] ColorSpace colorSpace)
{
    return RGBToI420ToYuv(srcBuffer, rgbInfo, PixelFormat::RGB_888, dstInfo, PixelFormat::NV21);
}

bool ImageFormatConvertExtUtils::RGBToNV12New(const uint8_t *srcBuffer, const RGBDataInfo &rgbInfo,
                                              DstConvertDataInfo &dstInfo,
                                              [[maybe_unused]] ColorSpace colorSpace)
{
    return RGBToI420ToYuv(srcBuffer, rgbInfo, PixelFormat::RGB_888, dstInfo, PixelFormat::NV12);
}

bool ImageFormatConvertExtUtils::RGBAToNV21New(const uint8_t *srcBuffer, const RGBDataInfo &rgbInfo,
                                               DstConvertDataInfo &dstInfo,
                                               [[maybe_unused]] ColorSpace colorSpace)
{
    return RGBToI420ToYuv(srcBuffer, rgbInfo, PixelFormat::RGBA_8888, dstInfo, PixelFormat::NV21);
}

bool ImageFormatConvertExtUtils::RGBAToNV12New(const uint8_t *srcBuffer, const RGBDataInfo &rgbInfo,
                                               DstConvertDataInfo &dstInfo,
                                               [[maybe_unused]] ColorSpace colorSpace)
{
    return RGBToI420ToYuv(srcBuffer, rgbInfo, PixelFormat::RGBA_8888, dstInfo, PixelFormat::NV12);
}

static bool YuvToI420(SrcConvertParam &srcParam, I420Info &i420)
{
    auto &converter = ConverterHandle::GetInstance().GetHandle();
    switch (srcParam.format) {
        case PixelFormat::NV12:
            converter.NV21ToI420(srcParam.slice[0], srcParam.stride[0], srcParam.slice[1], srcParam.stride[1],
                i420.I420Y, i420.yStride, i420.I420U, i420.uStride, i420.I420V, i420.vStride,
                i420.width, i420.height);
             break;
        case PixelFormat::NV21:
            converter.NV12ToI420(srcParam.slice[0], srcParam.stride[0], srcParam.slice[1], srcParam.stride[1],
                i420.I420Y, i420.yStride, i420.I420U, i420.uStride, i420.I420V, i420.vStride,
                i420.width, i420.height);
            break;
        default:
            return false;
    }
    return true;
}

static bool I420ToRGB(I420Info &i420, DstConvertParam &dstParam, [[maybe_unused]]ColorSpace colorSpace)
{
    auto &converter = ConverterHandle::GetInstance().GetHandle();
    switch (dstParam.format) {
        case PixelFormat::RGBA_8888:
            converter.I420ToABGR(i420.I420Y, i420.yStride, i420.I420U, i420.uStride, i420.I420V, i420.vStride,
                dstParam.slice[0], dstParam.stride[0], dstParam.width, dstParam.height);
            break;
        case PixelFormat::RGB_565:
            converter.I420ToRGB565Matrix(i420.I420Y, i420.yStride, i420.I420U, i420.uStride, i420.I420V, i420.vStride,
                dstParam.slice[0], dstParam.stride[0], static_cast<OHOS::OpenSourceLibyuv::ColorSpace>(colorSpace),
                dstParam.width, dstParam.height);
            break;
        default:
            return false;
    }
    return true;
}

static void YuvToRGBParam(const YUVDataInfo &yuvInfo, SrcConvertParam &srcParam,
                          DstConvertParam &dstParam, DstConvertDataInfo &dstInfo)
{

    srcParam.slice[0] = srcParam.buffer + yuvInfo.yOffset;
    srcParam.slice[1] = srcParam.buffer + yuvInfo.uvOffset;
    srcParam.stride[0] = static_cast<int>(yuvInfo.yStride);
    srcParam.stride[1] = static_cast<int>(yuvInfo.uvStride);

    if (dstInfo.allocType == AllocatorType::DMA_ALLOC) {
        dstParam.stride[0] = static_cast<int>(dstInfo.yStride);
        dstParam.slice[0] = dstInfo.buffer + dstInfo.yOffset;
    } else {
        IMAGE_LOGE("yyytest YuvToRGBParam CalcRGBStride start");
        dstParam.stride[0] = static_cast<int>(dstParam.width);
        dstParam.slice[0] = dstInfo.buffer;
    }
}

static bool YuvToI420ToRGBParam(const YUVDataInfo &yuvInfo, SrcConvertParam &srcParam, I420Info &i420Info,
                                DstConvertParam &dstParam, DstConvertDataInfo &dstInfo)
{
    YuvToRGBParam(yuvInfo, srcParam, dstParam, dstInfo);
    i420Info.yStride = rgbInfo.width;
    i420Info.uStride = (rgbInfo.width + NUM_1) / NUM_2;
    i420Info.vStride = (rgbInfo.width + NUM_1) / NUM_2;
    i420Info.uvHeight = ((i420Info.height + NUM_1) / NUM_2) * NUM_2;
    const uint32_t i420BufferSize = static_cast<size_t>(i420Info.yStride * i420Info.height +
        i420Info.uStride * i420Info.uvHeight * NUM_2);
    if (i420BufferSize <= NUM_0 || i420BufferSize > PIXEL_MAP_MAX_RAM_SIZE) {
        IMAGE_LOGE("Invalid destination buffer size calculation!");
        return false;
    }
    uint8_t *i420Buffer = new (std::nothrow) uint8_t[i420BufferSize];
    if (i420Buffer == nullptr) {
        IMAGE_LOGE("apply space for I420 buffer failed!");
        return false;
    }
    i420Info.I420Y = i420Buffer;
    i420Info.I420U = i420Info.I420Y + rgbInfo.height * i420Info.yStride;
    i420Info.I420V = i420Info.I420U + i420Info.uStride * ((rgbInfo.height + NUM_1) / NUM_2)
    return true;
}

static bool YuvTo420ToRGB(const uint8_t *srcBuffer, const YUVDataInfo &yuvInfo, PixelFormat srcFormat,
                          DstConvertDataInfo &dstInfo, PixelFormat dstFormat,
                          [[maybe_unused]]ColorSpace colorSpace)
{
    SrcConvertParam srcParam = {rgbInfo.width, rgbInfo.width};
    srcParam.buffer = srcBuffer;
    srcParam.format = srcFormat;

    DstConvertParam dstParam = {rgbInfo.width, rgbInfo.width};
    dstParam.format = dstFormat;

    I420Info i420Info = {rgbInfo.width, rgbInfo.width};

    if (!YuvToI420ToRGBParam(rgbInfo, srcParam, dstParam, i420Info, dstInfo)) {
        IMAGE_LOGE("yyytest RGB conversion to YUV failed!");
        return false;
    }
    auto bRet = YuvToI420(srcParam, i420Info);
    if (!bRet) {
        delete[] i420Info.I420Y;
        return false;
    }
    bRet = I420ToRGB(i420Info, dstParam, colorSpace);
    delete[] i420Info.I420Y;
    IMAGE_LOGE("yyytest RGBToI420ToYuv succ!");
    return bRet;
}

static bool YuvToRGBConverter(SrcConvertParam &srcParam, DstConvertParam &dstParam)
{
    auto &converter = ConverterHandle::GetInstance().GetHandle();
    if (srcParam.format == PixelFormat::NV21) {
        switch (dstParam.format) {
            case PixelFormat::RGB_888:
                converter.NV21ToRAW(srcParam.slice[0], srcParam.stride[0], srcParam.slice[1], srcParam.stride[1],
                    dstParam.slice[0], dstParam.stride[0], dstParam.width, dstParam.height);
                break;
            case PixelFormat::BGRA_8888:
                converter.NV21ToARGB(srcParam.slice[0], srcParam.stride[0], srcParam.slice[1], srcParam.stride[1],
                    dstParam.slice[0], dstParam.stride[0], dstParam.width, dstParam.height);
                break;
            default:
                return false;
        }
    } else if (srcParam.format == PixelFormat::NV12) {
        switch (dstParam.format) {
            case PixelFormat::RGB_888:
                converter.NV12ToRAW(srcParam.slice[0], srcParam.stride[0], srcParam.slice[1], srcParam.stride[1],
                    dstParam.slice[0], dstParam.stride[0], dstParam.width, dstParam.height);
                break;
            case PixelFormat::BGRA_8888:
                converter.NV12ToARGB(srcParam.slice[0], srcParam.stride[0], srcParam.slice[1], srcParam.stride[1],
                    dstParam.slice[0], dstParam.stride[0], dstParam.width, dstParam.height);
                break;
            default:
                return false;
        }
    }
    return true;
}

static bool YuvToRGB(const uint8_t *srcBuffer, const YUVDataInfo &yuvInfo, PixelFormat srcFormat,
                     DstConvertDataInfo &dstInfo, PixelFormat dstFormat)
{
    SrcConvertParam srcParam = {yuvInfo.width, yuvInfo.width};
    srcParam.buffer = srcBuffer;
    srcParam.format = srcFormat;

    DstConvertParam dstParam = {yuvInfo.width, yuvInfo.width};
    dstParam.format = dstFormat;

    if (!YuvToRGBParam(yuvInfo, srcParam, dstParam, dstInfo)) {
        IMAGE_LOGE("yyytest RGB conversion to YUV failed!");
        return false;
    }

    return YuvToRGBConverter(srcParam, dstParam);
}

bool ImageFormatConvertExtUtils::NV21ToRGBNew(const uint8_t *srcBuffer, const YUVDataInfo &yDInfo,
                                              DstConvertDataInfo &dstInfo,
                                              [[maybe_unused]]ColorSpace colorSpace)
{
    return YuvToRGB(srcBuffer, yuvInfo, PixelFormat::NV21, dstInfo, PixelFormat::RGB_888);
}

bool ImageFormatConvertExtUtils::NV12ToRGBNew(const uint8_t *srcBuffer, const YUVDataInfo &yDInfo,
                                              DstConvertDataInfo &dstInfo,
                                              [[maybe_unused]]ColorSpace colorSpace)
{
    return YuvToRGB(srcBuffer, yuvInfo, PixelFormat::NV12, dstInfo, PixelFormat::RGB_888);
}

bool ImageFormatConvertExtUtils::NV21ToRGBANew(const uint8_t *srcBuffer, const YUVDataInfo &yDInfo,
                                               DstConvertDataInfo &dstInfo,
                                               [[maybe_unused]]ColorSpace colorSpace)
{
    return YuvTo420ToRGB(srcBuffer, yuvInfo, PixelFormat::NV21, dstInfo, PixelFormat::RGBA_8888);
}

bool ImageFormatConvertExtUtils::NV12ToRGBANew(const uint8_t *srcBuffer, const YUVDataInfo &yDInfo,
                                               DstConvertDataInfo &dstInfo,
                                               [[maybe_unused]]ColorSpace colorSpace)
{                                               
    return YuvTo420ToRGB(srcBuffer, yuvInfo, PixelFormat::NV12, dstInfo, PixelFormat::RGBA_8888);
}

bool ImageFormatConvertExtUtils::NV21ToBGRANew(const uint8_t *srcBuffer, const YUVDataInfo &yDInfo,
                                               DstConvertDataInfo &dstInfo,
                                               [[maybe_unused]]ColorSpace colorSpace)
{
    return YuvToRGB(srcBuffer, yuvInfo, PixelFormat::NV21, dstInfo, PixelFormat::BGRA_8888);
}

bool ImageFormatConvertExtUtils::NV12ToBGRANew(const uint8_t *srcBuffer, const YUVDataInfo &yDInfo,
                                               DstConvertDataInfo &dstInfo,
                                               [[maybe_unused]]ColorSpace colorSpace)
{
    return YuvToRGB(srcBuffer, yuvInfo, PixelFormat::NV12, dstInfo, PixelFormat::BGRA_8888);
} 

bool ImageFormatConvertExtUtils::NV21ToRGB565New(const uint8_t *srcBuffer, const YUVDataInfo &yDInfo,
                                                 DstConvertDataInfo &dstInfo,
                                                 [[maybe_unused]]ColorSpace colorSpace)
{
    return YuvTo420ToRGB(srcBuffer, yuvInfo, PixelFormat::NV21, dstInfo, PixelFormat::BGRA_565);
}

bool ImageFormatConvertExtUtils::NV12ToRGB565New(const uint8_t *srcBuffer, const YUVDataInfo &yDInfo,
                                                 DstConvertDataInfo &dstInfo,
                                                 [[maybe_unused]]ColorSpace colorSpace)
{
    return YuvTo420ToRGB(srcBuffer, yuvInfo, PixelFormat::NV12, dstInfo, PixelFormat::BGRA_565);
}

static bool YuvToYuv(const uint8_t *srcBuffer, const YUVDataInfo &yuvInfo, PixelFormat srcFormat,
                     DstConvertDataInfo &dstInfo, PixelFormat dstFormat) {
    return false;
}

bool ImageFormatConvertExtUtils::NV21ToNV12New(const uint8_t *srcBuffer, const YUVDataInfo &yDInfo,
                                               DstConvertDataInfo &dstInfo,
                                               [[maybe_unused]]ColorSpace colorSpace)
{
    return YuvToYuv(srcBuffer, yuvInfo, PixelFormat::NV21, dstInfo, PixelFormat::NV12);
}

bool ImageFormatConvertExtUtils::NV12ToNV21New(const uint8_t *srcBuffer, const YUVDataInfo &yDInfo,
                                               DstConvertDataInfo &dstInfo,
                                               [[maybe_unused]]ColorSpace colorSpace)
{
    return YuvToYuv(srcBuffer, yuvInfo, PixelFormat::NV12, dstInfo, PixelFormat::NV21);
}

} // namespace Media
} // namespace OHOS