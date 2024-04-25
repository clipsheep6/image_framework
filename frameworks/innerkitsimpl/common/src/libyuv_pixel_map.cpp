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

#include "libyuv_pixel_map.h"

#include "image_utils.h"
#include "image_trace.h"
#include "image_type_converter.h"
#include "memory_manager.h"
#include "hitrace_meter.h"
#include "libyuv_pixel_map_utils.h"
#include "media_errors.h"
#include "pubdef.h"
#include "pixel_yuv_utils.h"
#include "securec.h"
#include "image_log.h"
#include "image_mdk_common.h"
#include "image_system_properties.h"
#if !defined(IOS_PLATFORM) && !defined(ANDROID_PLATFORM)
#include "surface_buffer.h"
#endif

#undef LOG_DOMAIN
#define LOG_DOMAIN LOG_TAG_DOMAIN_ID_IMAGE

#undef LOG_TAG
#define LOG_TAG "LibYuvPixelMap"

namespace OHOS {
namespace Media {
using namespace std;

static const uint8_t NUM_2 = 2;
constexpr int32_t MAX_DIMENSION = INT32_MAX >> NUM_2;
constexpr int32_t ANTIALIASING_SIZE = 350;

static int32_t GetVOffset(int32_t width, int32_t height)
{
    return width * height + ((width + 1) / NUM_2) * ((height + 1) / NUM_2);
}

void LibYuvPixelMap::scale(float xAxis, float yAxis)
{
    ScaleYuv420(xAxis, yAxis);
}

static bool IsSupportAntiAliasing(const ImageInfo& imageInfo, const AntiAliasingOption &option)
{
    return option != AntiAliasingOption::NONE && imageInfo.size.width <= ANTIALIASING_SIZE &&
            imageInfo.size.height <= ANTIALIASING_SIZE;
}

void LibYuvPixelMap::scale(float xAxis, float yAxis, const AntiAliasingOption &option)
{
    ImageInfo imageInfo;
    GetImageInfo(imageInfo);
    AntiAliasingOption operation = AntiAliasingOption::NONE;
    if (ImageSystemProperties::GetAntiAliasingEnabled() && IsSupportAntiAliasing(imageInfo, option)) {
        operation = AntiAliasingOption::MEDIUM;
    } else {
        operation = option;
    }
    ScaleYuv420(xAxis, yAxis, operation);
}

bool LibYuvPixelMap::resize(float xAxis, float yAxis)
{
    ScaleYuv420(xAxis, yAxis);
    return true;
}

void LibYuvPixelMap::ScaleYuv420(float xAxis, float yAxis, const AntiAliasingOption &option)
{
    libyuv::FilterMode filterMode = libyuv::FilterMode::kFilterNone;
    LibYuvPixelMapUtils::ConvertYuvMode(filterMode, option);
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
    uint8_t *dst = reinterpret_cast<uint8_t *>(dstMemory->data.data);
    if (imageInfo.pixelFormat == PixelFormat::NV12 || imageInfo.pixelFormat == PixelFormat::NV21) {
        uint32_t srcHalfW = GetUStride(srcW);
        uint32_t srcHalfH = GetUVHeight(srcH);
        uint32_t dstHalfW = GetUStride(dstW);
        uint32_t dstHalfH = GetUVHeight(dstH);

        // resize y_plane
        libyuv::ScalePlane(src, srcW, srcW, srcH, dst, dstW, dstW, dstH, filterMode);
        //Whether the row width is odd or even, U and Z are equal in size
        uint32_t srcUSize = GetUStride(srcW) * GetUVHeight(srcH);
        uint32_t dstUSize = GetUStride(dstW) * GetUVHeight(dstH);
        // Split VUplane
        std::unique_ptr<uint8_t[]> uvData = std::make_unique<uint8_t[]>(NUM_2 * srcUSize);

        // NV21
        uint8_t *vData = uvData.get();
        uint8_t *uData = uvData.get() + srcUSize;
        // If it's in NV12 format，swap u and v
        if (imageInfo.pixelFormat == PixelFormat::NV12) {
            uint8_t *tempSwap = vData;
            vData = uData;
            uData = tempSwap;
        }

        const uint8_t *src_uv = src + GetYSize(srcW, srcH);
        libyuv::SplitUVPlane(src_uv, NUM_2 * srcHalfW, vData, srcHalfW, uData, srcHalfW, srcHalfW, srcHalfH);

        // malloc memory to store temp u v
        std::unique_ptr<uint8_t[]> tempUVData = std::make_unique<uint8_t[]>(NUM_2 * srcUSize);
        uint8_t *tempVData = tempUVData.get();
        uint8_t *tempUData = tempUVData.get() + dstUSize;
        if (imageInfo.pixelFormat == PixelFormat::NV12) {
            uint8_t *tempSwap = tempVData;
            tempVData = tempUData;
            tempUData = tempSwap;
        }

        // resize u and v
        libyuv::ScalePlane(uData, srcHalfW, srcHalfW, srcHalfH, tempUData, dstHalfW, dstHalfW, dstHalfH, filterMode);
        libyuv::ScalePlane(vData, srcHalfW, srcHalfW, srcHalfH, tempVData, dstHalfW, dstHalfW, dstHalfH, filterMode);

        uint8_t *dst_uv = dst + GetYSize(dstW, dstH);
        libyuv::MergeUVPlane(tempVData, dstHalfW, tempUData, dstHalfW, dst_uv, NUM_2 * dstHalfW, dstHalfW, dstHalfH);

        uData = vData = nullptr;
        tempUData = tempVData = nullptr;

        SetPixelsAddr(reinterpret_cast<void *>(dst), dstMemory->extend.data,
            dstMemory->data.size, dstMemory->GetType(), nullptr);
        imageInfo.size.width = dstW;
        imageInfo.size.height = dstH;
        SetImageInfo(imageInfo, true);
    }
}

bool LibYuvPixelMap::FlipXaxis(const uint8_t *src, uint8_t *dst, int32_t width, int32_t height, PixelFormat format)
{
    if (format == PixelFormat::NV21 || format == PixelFormat::NV12) {
        libyuv::NV12ToI420(src, width, src + GetYSize(width, height), GetUVStride(width),
            dst, width, dst + GetYSize(width, height), GetUStride(width),
            dst + GetVOffset(width, height), GetUStride(width), width, height);
        if (SUCCESS != libyuv::I420Copy(dst, width, dst + GetYSize(width, height), GetUStride(width),
            dst + GetVOffset(width, height), GetUStride(width), const_cast<uint8_t *>(src), width,
            const_cast<uint8_t *>(src) + GetYSize(width, height), GetUStride(width),
            const_cast<uint8_t *>(src) + GetVOffset(width, height), GetUStride(width), width, -height)) {
            IMAGE_LOGE("Flip Yuv420SP Copy failed");
            return false;
        }
        libyuv::I420ToNV12(src, width, src + GetYSize(width, height), GetUStride(width),
            src + GetVOffset(width, height), GetUStride(width), dst, width,
            dst + GetYSize(width, height), GetUVStride(width), width, height);
    }
    return true;
}

void LibYuvPixelMap::FlipYaxis(const uint8_t *src, uint8_t *dst, int32_t width, int32_t height, PixelFormat format)
{
    if (format == PixelFormat::NV21 || format == PixelFormat::NV12) {
        libyuv::NV12ToI420(src, width, src + GetYSize(width, height), GetUVStride(width), dst, width,
            dst + GetYSize(width, height), GetUStride(width), dst + GetVOffset(width, height), GetUStride(width),
            width, height);
        libyuv::I420Mirror(dst, width, dst + GetYSize(width, height), GetUStride(width),
            dst + GetVOffset(width, height), GetUStride(width), const_cast<uint8_t *>(src),
            width, const_cast<uint8_t *>(src) + GetYSize(width, height), GetUStride(width),
            const_cast<uint8_t *>(src) + GetVOffset(width, height), GetUStride(width), width, height);
        libyuv::I420ToNV12(src, width, src + GetYSize(width, height), GetUStride(width),
            src + GetVOffset(width, height), GetUStride(width), dst, width,
            dst + GetYSize(width, height), GetUVStride(width), width, height);
    }
}

void LibYuvPixelMap::flip(bool xAxis, bool yAxis)
{
    ImageInfo imageInfo;
    GetImageInfo(imageInfo);
    uint8_t *dst = nullptr;
    const uint8_t *src = data_;
    int32_t width = imageInfo.size.width;
    int32_t height = imageInfo.size.height;
    uint32_t pictureSize = GetImageSize(width, height);
    MemoryData memoryData = {nullptr, pictureSize, "flip ImageData", {width, height}};
    auto dstMemory = MemoryManager::CreateMemory(allocatorType_, memoryData);
    if (dstMemory == nullptr) {
        IMAGE_LOGE("flip CreateMemory failed");
        return;
    }
#if !defined(IOS_PLATFORM) && !defined(ANDROID_PLATFORM)
    if (allocatorType_ == AllocatorType::DMA_ALLOC) {
        if (dstMemory->extend.data == nullptr) {
            IMAGE_LOGE("GendstTransInfo get surfacebuffer failed");
        }
    }
#endif
    dst = reinterpret_cast<uint8_t *>(dstMemory->data.data);
    if (xAxis) {
        if (!FlipXaxis(src, dst, width, height, imageInfo.pixelFormat)) {
            IMAGE_LOGE("FlipXaxis failed");
            dstMemory->Release();
            return;
        }
    }
    if (yAxis) {
        FlipYaxis(src, dst, width, height, imageInfo.pixelFormat);
    }
    SetPixelsAddr(dst, dstMemory->extend.data, dstMemory->data.size, dstMemory->GetType(), nullptr);
    AssignYuvDataOnType(imageInfo.pixelFormat, width, height);
}
} // namespace Media
} // namespace OHOS