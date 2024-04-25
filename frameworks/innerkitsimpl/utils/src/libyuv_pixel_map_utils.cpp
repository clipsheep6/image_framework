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

#include "image_log.h"
#include "ios"
#include "istream"
#include "image_trace.h"
#include "image_system_properties.h"
#include "libyuv_pixel_map_utils.h"
#include "media_errors.h"
#include "memory_manager.h"
#include "securec.h"
#if !defined(IOS_PLATFORM) && !defined(ANDROID_PLATFORM)
#include "surface_buffer.h"
#endif

#undef LOG_DOMAIN
#define LOG_DOMAIN LOG_TAG_DOMAIN_ID_IMAGE

#undef LOG_TAG
#define LOG_TAG "LibYuvPixelMapUtils"

namespace OHOS {
namespace Media {

static const uint8_t NUM_2 = 2;
static const uint8_t NUM_4 = 4;

static const int32_t degrees90 = 90;
static const int32_t degrees180 = 180;
static const int32_t degrees270 = 270;

static int32_t GetVOffset(int32_t width, int32_t height)
{
    return width * height + ((width + 1) / NUM_2) * ((height + 1) / NUM_2);
}

bool LibYuvPixelMapUtils::BGRAToYuv420(const uint8_t *src, uint8_t *dst, int srcW, int srcH, PixelFormat pixelFormat)
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

bool LibYuvPixelMapUtils::Yuv420ToBGRA(const uint8_t *sample, uint8_t *dst_argb,
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

bool LibYuvPixelMapUtils::Yuv420ToARGB(const uint8_t *sample, uint8_t *dst_argb,
    int32_t width, int32_t height, PixelFormat pixelFormat)
{
    std::unique_ptr<uint8_t[]> temp = std::make_unique<uint8_t[]>(width * height * NUM_4);
    int32_t r = Yuv420ToBGRA(sample, temp.get(), width, height, pixelFormat);
    if (r != 0) {
        return false;
    }
    return 0 == libyuv::ARGBToBGRA(temp.get(), width * NUM_4, dst_argb, width * NUM_4, width, height);
}

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

bool LibYuvPixelMapUtils::YuvRotate(const uint8_t *srcPixels, Size &size, uint8_t *dstPixels, int32_t degrees,
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
#if !defined(IOS_PLATFORM) && !defined(ANDROID_PLATFORM)
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
        default:
            return false;
    }
    size.width = dstWidth;
    size.height = dstHeight;
    return true;
}

void LibYuvPixelMapUtils::ConvertYuvMode(libyuv::FilterMode &filterMode, const AntiAliasingOption &option)
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

void LibYuvPixelMapUtils::ScaleYuv420(float xAxis, float yAxis, const AntiAliasingOption &option)
{
    OpenSourceLibyuv::FilterMode filterMode = OpenSourceLibyuv::FilterMode::kFilterNone;
    ConvertYuvMode(filterMode, option);
    ImageInfo imageInfo;
    GetImageInfo(imageInfo);
    int32_t srcW = imageInfo.size.width;
    int32_t srcH = imageInfo.size.height;
    int32_t dstW = srcW * xAxis;
    int32_t dstH = srcH * yAxis;
    const uint8_t *src = data_;
    uint32_t pictureSize = GetImageSize(dstW, dstH);
    Size desiredSize = {dstW, dstH};
    MemoryData memoryData = {nullptr, pictureSize, "ScaleYuv420 ImageData", desiredSize};
    auto dstMemory = MemoryManager::CreateMemory(allocatorType_, memoryData);
    if (dstMemory == nullptr) {
        IMAGE_LOGE("ScaleYuv420 CreateMemory failed");
        return;
    }
    auto converter = ConverterHandle::GetInstance().GetHandle();
    uint8_t *dst = reinterpret_cast<uint8_t *>(dstMemory->data.data);
    uint32_t srcHalfW = GetUStride(srcW);
    uint32_t srcHalfH = GetUVHeight(srcH);
    uint32_t dstHalfW = GetUStride(dstW);
    uint32_t dstHalfH = GetUVHeight(dstH);
    // resize y_plane
    converter.ScalePlane(src, srcW, srcW, srcH, dst, dstW, dstW, dstH, filterMode);
    //Whether the row width is odd or even, U and Z are equal in size
    uint32_t srcUSize = GetUStride(srcW) * GetUVHeight(srcH);
    uint32_t dstUSize = GetUStride(dstW) * GetUVHeight(dstH);
    NV12SizeInfo srcNV12SizeInfo = {srcUSize, srcW, srcH, srcHalfW, srcHalfH};
    NV12SizeInfo dstNV12SizeInfo = {dstUSize, dstW, dstH, dstHalfW, dstHalfH};
    ScaleYuv420(imageInfo, srcNV12SizeInfo, dstNV12SizeInfo, src, dst, filterMode);
    SetPixelsAddr(reinterpret_cast<void *>(dst), dstMemory->extend.data,
        dstMemory->data.size, dstMemory->GetType(), nullptr);
}

void LibYuvPixelMapUtils::ScaleYuv420(ImageInfo &imageInfo, NV12SizeInfo srcNV12SizeInfo, NV12SizeInfo dstNV12SizeInfo,
    const uint8_t *src, uint8_t *dst, OpenSourceLibyuv::FilterMode filterMode)
{
    // Split VUplane
    std::unique_ptr<uint8_t[]> uvData = std::make_unique<uint8_t[]>(NUM_2 * srcNV12SizeInfo.uSize);

    // NV21
    uint8_t *vData = uvData.get();
    uint8_t *uData = uvData.get() + srcNV12SizeInfo.uSize;
    // If it's in NV12 format，swap u and v
    if (imageInfo.pixelFormat == PixelFormat::NV12) {
        uint8_t *tempSwap = vData;
        vData = uData;
        uData = tempSwap;
    }

    const uint8_t *src_uv = src + GetYSize(srcNV12SizeInfo.width, srcNV12SizeInfo.height);
    auto converter = ConverterHandle::GetInstance().GetHandle();
    converter.SplitUVPlane(src_uv, NUM_2 * srcNV12SizeInfo.halfWidth, vData, srcNV12SizeInfo.halfWidth, uData,
        srcNV12SizeInfo.halfWidth, srcNV12SizeInfo.halfWidth, srcNV12SizeInfo.halfHeight);

    // malloc memory to store temp u v
    std::unique_ptr<uint8_t[]> tempUVData = std::make_unique<uint8_t[]>(NUM_2 * srcNV12SizeInfo.uSize);
    uint8_t *tempVData = tempUVData.get();
    uint8_t *tempUData = tempUVData.get() + dstNV12SizeInfo.uSize;
    if (imageInfo.pixelFormat == PixelFormat::NV12) {
        uint8_t *tempSwap = tempVData;
        tempVData = tempUData;
        tempUData = tempSwap;
    }

    // resize u and v
    converter.ScalePlane(uData, srcNV12SizeInfo.halfWidth, srcNV12SizeInfo.halfWidth, srcNV12SizeInfo.halfHeight,
        tempUData, dstNV12SizeInfo.halfWidth, dstNV12SizeInfo.halfWidth, dstNV12SizeInfo.halfHeight, filterMode);
    converter.ScalePlane(vData, srcNV12SizeInfo.halfWidth, srcNV12SizeInfo.halfWidth, srcNV12SizeInfo.halfHeight,
        tempVData, dstNV12SizeInfo.halfWidth, dstNV12SizeInfo.halfWidth, dstNV12SizeInfo.halfHeight, filterMode);

    uint8_t *dst_uv = dst + GetYSize(dstNV12SizeInfo.width, dstNV12SizeInfo.height);
    converter.MergeUVPlane(tempVData, dstNV12SizeInfo.halfWidth, tempUData, dstNV12SizeInfo.halfWidth, dst_uv,
        NUM_2 * dstNV12SizeInfo.halfWidth, dstNV12SizeInfo.halfWidth, dstNV12SizeInfo.halfHeight);

    uData = vData = nullptr;
    tempUData = tempVData = nullptr;
    imageInfo.size.width = dstNV12SizeInfo.width;
    imageInfo.size.height = dstNV12SizeInfo.height;
    SetImageInfo(imageInfo, true);
}

bool LibYuvPixelMapUtils::FlipXaxis(const uint8_t *src, uint8_t *dst, int32_t width, int32_t height, PixelFormat format)
{
    auto converter = ConverterHandle::GetInstance().GetHandle();
    if (format == PixelFormat::NV21 || format == PixelFormat::NV12) {
        converter.NV12ToI420(src, width, src + GetYSize(width, height), GetUVStride(width),
            dst, width, dst + GetYSize(width, height), GetUStride(width),
            dst + GetVOffset(width, height), GetUStride(width), width, height);
        if (SUCCESS != converter.I420Copy(dst, width, dst + GetYSize(width, height), GetUStride(width),
            dst + GetVOffset(width, height), GetUStride(width), const_cast<uint8_t *>(src), width,
            const_cast<uint8_t *>(src) + GetYSize(width, height), GetUStride(width),
            const_cast<uint8_t *>(src) + GetVOffset(width, height), GetUStride(width), width, -height)) {
            IMAGE_LOGE("Flip Yuv420SP Copy failed");
            return false;
        }
        converter.I420ToNV12(src, width, src + GetYSize(width, height), GetUStride(width),
            src + GetVOffset(width, height), GetUStride(width), dst, width,
            dst + GetYSize(width, height), GetUVStride(width), width, height);
    }
    return true;
}

void LibYuvPixelMapUtils::FlipYaxis(const uint8_t *src, uint8_t *dst, int32_t width, int32_t height, PixelFormat format)
{
    auto converter = ConverterHandle::GetInstance().GetHandle();
    if (format == PixelFormat::NV21 || format == PixelFormat::NV12) {
        converter.NV12ToI420(src, width, src + GetYSize(width, height), GetUVStride(width), dst, width,
            dst + GetYSize(width, height), GetUStride(width), dst + GetVOffset(width, height), GetUStride(width),
            width, height);
        converter.I420Mirror(dst, width, dst + GetYSize(width, height), GetUStride(width),
            dst + GetVOffset(width, height), GetUStride(width), const_cast<uint8_t *>(src),
            width, const_cast<uint8_t *>(src) + GetYSize(width, height), GetUStride(width),
            const_cast<uint8_t *>(src) + GetVOffset(width, height), GetUStride(width), width, height);
        converter.I420ToNV12(src, width, src + GetYSize(width, height), GetUStride(width),
            src + GetVOffset(width, height), GetUStride(width), dst, width,
            dst + GetYSize(width, height), GetUVStride(width), width, height);
    }
}

} // namespace Media
} // namespace OHOS