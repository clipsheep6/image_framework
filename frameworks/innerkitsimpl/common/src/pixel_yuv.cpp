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

#include "pixel_yuv.h"

#include "image_utils.h"
#include "image_trace.h"
#include "image_type_converter.h"
#include "memory_manager.h"
#include "hilog/log.h"
#include "hitrace_meter.h"
#include "log_tags.h"
#include "media_errors.h"
#include "pubdef.h"
#include "pixel_yuv_utils.h"
#include "securec.h"
#include "image_log.h"
#include "image_mdk_common.h"
#include "image_system_properties.h"
#if !defined(IOS_PLATFORM) && !defined(A_PLATFORM)
#include "surface_buffer.h"
#endif

#undef LOG_DOMAIN
#define LOG_DOMAIN LOG_TAG_DOMAIN_ID_IMAGE

#undef LOG_TAG
#define LOG_TAG "PixelYuv"

namespace OHOS {
namespace Media {
using namespace std;

static const uint8_t NUM_2 = 2;
static const uint8_t NUM_3 = 3;
static const uint8_t NUM_4 = 4;
constexpr int32_t MAX_DIMENSION = INT32_MAX >> NUM_2;

struct TransInfos {
    SkMatrix matrix;
};

struct TransMemoryInfo {
    AllocatorType allocType;
    std::unique_ptr<AbsMemory> memory = nullptr;
};

static SkImageInfo ToSkImageInfo(ImageInfo &info, sk_sp<SkColorSpace> colorSpace)
{
    SkColorType colorType = ImageTypeConverter::ToSkColorType(info.pixelFormat);
    SkAlphaType alphaType = ImageTypeConverter::ToSkAlphaType(info.alphaType);
    IMAGE_LOGD("ToSkImageInfo w %{public}d, h %{public}d", info.size.width, info.size.height);
    IMAGE_LOGD(
        "ToSkImageInfo pf %{public}s, at %{public}s, skpf %{public}s, skat %{public}s",
        ImageTypeConverter::ToName(info.pixelFormat).c_str(),
        ImageTypeConverter::ToName(info.alphaType).c_str(),
        ImageTypeConverter::ToName(colorType).c_str(),
        ImageTypeConverter::ToName(alphaType).c_str()
    );
    return SkImageInfo::Make(info.size.width, info.size.height, colorType, alphaType, colorSpace);
}

static sk_sp<SkColorSpace> ToSkColorSpace(PixelMap *pixelmap)
{
#ifdef IMAGE_COLORSPACE_FLAG
    if (pixelmap->InnerGetGrColorSpacePtr() == nullptr) {
        return nullptr;
    }
    return pixelmap->InnerGetGrColorSpacePtr()->ToSkColorSpace();
#else
    return nullptr;
#endif
}

static bool isSameColorSpace(const OHOS::ColorManager::ColorSpace &src,
    const OHOS::ColorManager::ColorSpace &dst)
{
    auto skSrc = src.ToSkColorSpace();
    auto skDst = dst.ToSkColorSpace();
    return SkColorSpace::Equals(skSrc.get(), skDst.get());
}

PixelYuv::~PixelYuv()
{
    FreePixelMap();
}

const uint8_t *PixelYuv::GetPixel8(int32_t x, int32_t y)
{
    IMAGE_LOGE("GetPixel8 is not support on PixelYuv");
    return nullptr;
}

const uint16_t *PixelYuv::GetPixel16(int32_t x, int32_t y)
{
    IMAGE_LOGE("GetPixel16 is not support on PixelYuv");
    return nullptr;
}

const uint32_t *PixelYuv::GetPixel32(int32_t x, int32_t y)
{
    IMAGE_LOGE("GetPixel32 is not support on PixelYuv");
    return nullptr;
}

bool PixelYuv::GetARGB32Color(int32_t x, int32_t y, uint32_t &color)
{
    IMAGE_LOGE("GetARGB32Color is not support on PixelYuv");
    return false;
}

uint8_t PixelYuv::GetARGB32ColorA(uint32_t color)
{
    IMAGE_LOGE("GetARGB32ColorA is not support on PixelYuv");
    return 0;
}

uint8_t PixelYuv::GetARGB32ColorR(uint32_t color)
{
    IMAGE_LOGE("GetARGB32ColorR is not support on PixelYuv");
    return 0;
}

uint8_t PixelYuv::GetARGB32ColorG(uint32_t color)
{
    IMAGE_LOGE("GetARGB32ColorG is not support on PixelYuv");
    return 0;
}

uint8_t PixelYuv::GetARGB32ColorB(uint32_t color)
{
    IMAGE_LOGE("GetARGB32ColorB is not support on PixelYuv");
    return 0;
}

uint32_t PixelYuv::SetAlpha(const float percent)
{
    IMAGE_LOGE("SetAlpha is not support on PixelYuv");
    return ERR_IMAGE_DATA_UNSUPPORT;
}

uint32_t PixelYuv::getPixelBytesNumber()
{
    IMAGE_LOGE("getPixelBytesNumber is not support on PixelYuv");
    return ERR_IMAGE_DATA_UNSUPPORT;
}

int32_t PixelYuv::GetByteCount()
{
    IMAGE_LOGE("GetByteCount is not support on PixelYuv");
    return ERR_IMAGE_DATA_UNSUPPORT;
}

static int32_t GetYSize(int32_t width, int32_t height)
{
    return width * height;
}

#ifdef LIBYUV_ENABLE
static int32_t GetVOffset(int32_t width, int32_t height)
{
    return width * height + ((width + 1) / NUM_2) * ((height + 1) / NUM_2);
}
#endif

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

void PixelYuv::rotate(float degrees)
{
    uint32_t count = GetImageSize(imageInfo_.size.width, imageInfo_.size.height);
    Size desiredSize = {imageInfo_.size.width, imageInfo_.size.height};
    MemoryData memoryData = {nullptr, count, "Rotate ImageData", desiredSize};
    auto dstMemory = MemoryManager::CreateMemory(allocatorType_, memoryData);
    if (dstMemory == nullptr) {
        IMAGE_LOGE("rotate CreateMemory failed");
        return;
    }
#if !defined(IOS_PLATFORM) && !defined(A_PLATFORM)
    if (allocatorType_ == AllocatorType::DMA_ALLOC) {
        if (dstMemory->extend.data == nullptr) {
            IMAGE_LOGE("GendstTransInfo get surfacebuffer failed");
        }
    }
#endif
    uint8_t *dst = reinterpret_cast<uint8_t *>(dstMemory->data.data);
    YuvImageInfo srcInfo = {PixelYuvUtils::ConvertFormat(imageInfo_.pixelFormat),
        imageInfo_.size.width, imageInfo_.size.height};
    YuvImageInfo dstInfo;
    if (!PixelYuvUtils::YuvRotate(data_, srcInfo, dst, dstInfo, degrees)) {
        dstMemory->Release();
        return;
    }
    if (memcpy_s(data_, count, dst, count) != 0) {
        IMAGE_LOGE("rotate memcpy failed");
        return;
    }
    imageInfo_.size.width = dstInfo.width;
    imageInfo_.size.height = dstInfo.height;
    SetPixelsAddr(dstMemory->data.data, dstMemory->extend.data, dstMemory->data.size, dstMemory->GetType(), nullptr);
    SetImageInfo(imageInfo_, true);
    AssignYuvDataOnType(imageInfo_.pixelFormat, imageInfo_.size.width, imageInfo_.size.height);
    return;
}

uint32_t PixelYuv::crop(const Rect &rect)
{
    int32_t rectSize = GetYSize(rect.width, rect.height);
    int32_t pixelSize = GetYSize(imageInfo_.size.width, imageInfo_.size.height);
    if (rect.top < 0 || rect.left < 0 || rectSize > pixelSize) {
        IMAGE_LOGE("crop invalid param");
        return ERR_IMAGE_INVALID_PARAMETER;
    }
    Size desiredSize = {rect.width, rect.height};
    MemoryData memoryData = {nullptr, GetImageSize(rect.width, rect.height), "Trans ImageData", desiredSize};
    auto dstMemory = MemoryManager::CreateMemory(allocatorType_, memoryData);
    if (dstMemory == nullptr) {
        IMAGE_LOGE("crop CreateMemory failed");
        return ERR_IMAGE_CROP;
    }
#if !defined(IOS_PLATFORM) && !defined(A_PLATFORM)
    if (allocatorType_ == AllocatorType::DMA_ALLOC) {
        if (dstMemory->extend.data == nullptr) {
            IMAGE_LOGE("GendstTransInfo get surfacebuffer failed");
        }
    }
#endif
    YuvImageInfo srcInfo = {PixelYuvUtils::ConvertFormat(imageInfo_.pixelFormat),
        imageInfo_.size.width, imageInfo_.size.height};
    if (!PixelYuvUtils::YuvCrop(data_, srcInfo, (uint8_t *)dstMemory->data.data, rect)) {
        dstMemory->Release();
        return ERR_IMAGE_CROP;
    }
    imageInfo_.size.height = rect.height;
    imageInfo_.size.width = rect.width;

    SetPixelsAddr(dstMemory->data.data, nullptr, GetImageSize(rect.width, rect.height), dstMemory->GetType(), nullptr);
    AssignYuvDataOnType(imageInfo_.pixelFormat, imageInfo_.size.width, imageInfo_.size.height);
    return SUCCESS;
}

#ifndef LIBYUV_ENABLE
void PixelYuv::scale(float xAxis, float yAxis)
{
    ImageTrace imageTrace("PixelMap scale");
    return scale(xAxis, yAxis, AntiAliasingOption::NONE);
}

void PixelYuv::scale(float xAxis, float yAxis, const AntiAliasingOption &option)
{
    ImageTrace imageTrace("PixelMap scale");
    ImageInfo imageInfo;
    GetImageInfo(imageInfo);
    int32_t dstW = imageInfo.size.width * xAxis;
    int32_t dstH = imageInfo.size.height * yAxis;
    Size desiredSize = {dstW, dstH};
    MemoryData memoryData = {nullptr, GetImageSize(dstW, dstH), "Trans ImageData", desiredSize};
    auto dstMemory = MemoryManager::CreateMemory(allocatorType_, memoryData);
    if (dstMemory == nullptr) {
        IMAGE_LOGE("scale CreateMemory failed");
        return;
    }
#if !defined(IOS_PLATFORM) && !defined(A_PLATFORM)
    if (allocatorType_ == AllocatorType::DMA_ALLOC) {
        if (dstMemory->extend.data == nullptr) {
            IMAGE_LOGE("GendstTransInfo get surfacebuffer failed");
        }
    }
#endif
    uint8_t *yuvData = reinterpret_cast<uint8_t *>(dstMemory->data.data);
    YuvImageInfo srcInfo = {PixelYuvUtils::ConvertFormat(imageInfo.pixelFormat),
        imageInfo.size.width, imageInfo.size.height};
    YuvImageInfo dstInfo = {PixelYuvUtils::ConvertFormat(imageInfo.pixelFormat), dstW, dstH};
    if (PixelYuvUtils::YuvScale(data_, srcInfo, yuvData, dstInfo, PixelYuvUtils::YuvConvertOption(option)) != SUCCESS) {
        IMAGE_LOGE("ScaleYuv failed");
        return;
    }
    imageInfo.size.height = dstH;
    imageInfo.size.width = dstW;

    SetPixelsAddr(dstMemory->data.data, dstMemory->extend.data, dstMemory->data.size, dstMemory->GetType(), nullptr);
    SetImageInfo(imageInfo, true);
    AssignYuvDataOnType(imageInfo.pixelFormat, imageInfo.size.width, imageInfo.size.height);
}

bool PixelYuv::resize(float xAxis, float yAxis)
{
    ImageTrace imageTrace("PixelMap resize");
    ImageInfo imageInfo;
    GetImageInfo(imageInfo);
    int32_t dstW = imageInfo.size.width * xAxis;
    int32_t dstH = imageInfo.size.height * yAxis;
    Size desiredSize = {dstW, dstH};
    MemoryData memoryData = {nullptr, GetImageSize(dstW, dstH), "ressize ImageData", desiredSize};
    auto dstMemory = MemoryManager::CreateMemory(allocatorType_, memoryData);
    if (dstMemory == nullptr) {
        IMAGE_LOGE("resize CreateMemory failed");
        return false;
    }
#if !defined(IOS_PLATFORM) && !defined(A_PLATFORM)
    if (allocatorType_ == AllocatorType::DMA_ALLOC) {
        if (dstMemory->extend.data == nullptr) {
            IMAGE_LOGE("GendstTransInfo get surfacebuffer failed");
        }
    }
#endif
    uint8_t *yuvData = reinterpret_cast<uint8_t *>(dstMemory->data.data);
    YuvImageInfo srcInfo = {PixelYuvUtils::ConvertFormat(imageInfo.pixelFormat),
        imageInfo.size.width, imageInfo.size.height};
    YuvImageInfo dstInfo = {PixelYuvUtils::ConvertFormat(imageInfo.pixelFormat), dstW, dstH};
    if (PixelYuvUtils::YuvScale(data_, srcInfo, yuvData, dstInfo,
        PixelYuvUtils::YuvConvertOption(AntiAliasingOption::NONE)) != SUCCESS) {
        IMAGE_LOGE("resize failed");
        return false;
    }
    imageInfo.size.height = dstW;
    imageInfo.size.width = dstH;

    SetPixelsAddr(dstMemory->data.data, dstMemory->extend.data, dstMemory->data.size, dstMemory->GetType(), nullptr);
    SetImageInfo(imageInfo, true);
    AssignYuvDataOnType(imageInfo.pixelFormat, imageInfo.size.width, imageInfo.size.height);
    return true;
}

void PixelYuv::flip(bool xAxis, bool yAxis)
{
    if (xAxis == false && yAxis == false) {
        return;
    }
    int32_t srcW = imageInfo_.size.width;
    int32_t srcH = imageInfo_.size.height;
    PixelFormat format = imageInfo_.pixelFormat;
    const uint8_t *src = data_;
    Size desiredSize = {srcW, srcH};
    uint32_t pictureSize = GetImageSize(srcW, srcH);
    MemoryData memoryData = {nullptr, pictureSize, "flip ImageData", desiredSize};
    auto dstMemory = MemoryManager::CreateMemory(allocatorType_, memoryData);
    if (dstMemory == nullptr) {
        IMAGE_LOGE("flip CreateMemory failed");
        return;
    }
#if !defined(IOS_PLATFORM) && !defined(A_PLATFORM)
    if (allocatorType_ == AllocatorType::DMA_ALLOC) {
        if (dstMemory->extend.data == nullptr) {
            IMAGE_LOGE("GendstTransInfo get surfacebuffer failed");
        }
    }
#endif
    uint8_t *dst = reinterpret_cast<uint8_t *>(dstMemory->data.data);
    YuvImageInfo srcInfo = {PixelYuvUtils::ConvertFormat(format), srcW, srcH};
    if (xAxis) {
        if (!PixelYuvUtils::YuvFlip(src, srcInfo, dst, true)) {
            IMAGE_LOGE("flip yuv xAxis failed");
            return;
        }
    }
    if (yAxis) {
        if (!PixelYuvUtils::YuvFlip(src, srcInfo, dst, false)) {
            IMAGE_LOGE("flip yuv yAxis failed");
            return;
        }
    }
    SetPixelsAddr(dst, dstMemory->extend.data, dstMemory->data.size, dstMemory->GetType(), nullptr);
    AssignYuvDataOnType(format, srcW, srcH);
}

#else
void PixelYuv::scale(float xAxis, float yAxis)
{
    ScaleYuv420(xAxis, yAxis);
}

constexpr int32_t ANTIALIASING_SIZE = 350;

static bool IsSupportAntiAliasing(const ImageInfo& imageInfo, const AntiAliasingOption &option)
{
    return option != AntiAliasingOption::NONE && imageInfo.size.width <= ANTIALIASING_SIZE &&
            imageInfo.size.height <= ANTIALIASING_SIZE;
}

void PixelYuv::scale(float xAxis, float yAxis, const AntiAliasingOption &option)
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

bool PixelYuv::resize(float xAxis, float yAxis)
{
    ScaleYuv420(xAxis, yAxis);
    return true;
}

void PixelYuv::ScaleYuv420(float xAxis, float yAxis, const AntiAliasingOption &option)
{
    libyuv::FilterMode filterMode = libyuv::FilterMode::kFilterNone;
    PixelYuvUtils::ConvertYuvMode(filterMode, option);
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
    if (imageInfo.pixelFormat == PixelFormat::YU12 || imageInfo.pixelFormat == PixelFormat::YV12) {
        if (SUCCESS != libyuv::I420Scale(src, srcW, src + GetYSize(srcW, srcH), GetUStride(srcW),
            src + GetVOffset(srcW, srcH), GetUStride(srcW), srcW, srcH, dst, dstW,
            dst + GetYSize(dstW, dstH), GetUStride(dstW), dst + GetVOffset(dstW, dstH),
            GetUStride(dstW), dstW, dstH, filterMode)) {
            dstMemory->Release();
            IMAGE_LOGE("Scale Yuv420P failed");
            return;
        }
    }
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

bool PixelYuv::FlipXaxis(const uint8_t *src, uint8_t *dst, int32_t width, int32_t height, PixelFormat format)
{
    if (format == PixelFormat::YU12 || format == PixelFormat::YV12) {
        if (SUCCESS != libyuv::I420Copy(src, width, src + GetYSize(width, height), GetUStride(width),
            src + GetVOffset(width, height), GetUStride(width), dst, width,
            dst + GetYSize(width, height), GetUStride(width),
            dst + GetVOffset(width, height), GetUStride(width), width, -height)) {
            IMAGE_LOGE("Flip Yuv420P Copy failed");
            return false;
        }
    }
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

void PixelYuv::FlipYaxis(const uint8_t *src, uint8_t *dst, int32_t width, int32_t height, PixelFormat format)
{
    if (format == PixelFormat::YU12 || format == PixelFormat::YV12) {
        libyuv::I420Mirror(src, width, src + GetYSize(width, height), GetUStride(width),
            src + GetVOffset(width, height), GetUStride(width), dst, width,
            dst + GetYSize(width, height), GetUStride(width),
            dst + GetVOffset(width, height), GetUStride(width), width, height);
        }
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

void PixelYuv::flip(bool xAxis, bool yAxis)
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
#if !defined(IOS_PLATFORM) && !defined(A_PLATFORM)
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
#endif

uint32_t PixelYuv::WritePixels(const uint8_t *source, const uint64_t &bufferSize, const uint32_t &offset,
    const uint32_t &stride, const Rect &region)
{
    if (!CheckPixelsInput(source, bufferSize, offset, region)) {
        IMAGE_LOGE("write pixel by rect input parameter fail.");
        return ERR_IMAGE_INVALID_PARAMETER;
    }
    if (!IsEditable()) {
        IMAGE_LOGE("write pixel by rect PixelYuv data is not editable.");
        return ERR_IMAGE_PIXELMAP_NOT_ALLOW_MODIFY;
    }
    if (!ImageUtils::IsValidImageInfo(imageInfo_)) {
        IMAGE_LOGE("write pixel by rect current PixelYuv image info is invalid.");
        return ERR_IMAGE_WRITE_PIXELMAP_FAILED;
    }
    if (data_ == nullptr) {
        IMAGE_LOGE("write pixel by rect current pixel map data is null.");
        return ERR_IMAGE_WRITE_PIXELMAP_FAILED;
    }
    uint32_t bytesPerPixel = ImageUtils::GetPixelBytes(imageInfo_.pixelFormat);
    if (bytesPerPixel == 0) {
        IMAGE_LOGE("write pixel by rect get bytes by per pixel fail.");
        return ERR_IMAGE_WRITE_PIXELMAP_FAILED;
    }
    Position dstPosition{region.left, region.top};
    ImageInfo srcInfo;
    srcInfo.size.height = region.height;
    srcInfo.size.width = region.width;
    srcInfo.pixelFormat = imageInfo_.pixelFormat;
    if (!PixelYuvUtils::WriteYuvConvert(source + offset, srcInfo, data_, dstPosition, imageInfo_)) {
        IMAGE_LOGE("write pixel by rect call WriteYuvConvert fail.");
        return ERR_IMAGE_WRITE_PIXELMAP_FAILED;
    }
    return SUCCESS;
}

uint32_t PixelYuv::ReadPixels(const uint64_t &bufferSize, const uint32_t &offset, const uint32_t &stride,
    const Rect &region, uint8_t *dst)
{
    if (!CheckPixelsInput(dst, bufferSize, offset, region)) {
        IMAGE_LOGE("read pixels by rect input parameter fail.");
        return ERR_IMAGE_INVALID_PARAMETER;
    }

    if (data_ == nullptr) {
        IMAGE_LOGE("read pixels by rect this pixel data is null.");
        return ERR_IMAGE_READ_PIXELMAP_FAILED;
    }
    ImageInfo dstImageInfo;
    dstImageInfo.size.width = region.width;
    dstImageInfo.size.height = region.height;
    dstImageInfo.pixelFormat = imageInfo_.pixelFormat;

    Position srcPosition{region.left, region.top};
    if (!PixelYuvUtils::ReadYuvConvert((void *)data_, srcPosition, imageInfo_, dst + offset, dstImageInfo)) {
        IMAGE_LOGE("read pixels by rect call ReadPixelsConvert fail.");
        return ERR_IMAGE_READ_PIXELMAP_FAILED;
    }
    return SUCCESS;
}

void PixelYuv::translate(float xAxis, float yAxis)
{
    if (xAxis == 0 && yAxis == 0) {
        return;
    }
    int32_t width = imageInfo_.size.width;
    int32_t height = imageInfo_.size.height;

    int32_t dstSize = GetImageSize(width, height);
    MemoryData memoryData = {nullptr, dstSize, "translate ImageData", {width, height}};
    auto dstMemory = MemoryManager::CreateMemory(allocatorType_, memoryData);
    if (dstMemory == nullptr) {
        IMAGE_LOGE("translate CreateMemory failed");
        return;
    }
#if !defined(IOS_PLATFORM) && !defined(A_PLATFORM)
    if (allocatorType_ == AllocatorType::DMA_ALLOC) {
        if (dstMemory->extend.data == nullptr) {
            IMAGE_LOGE("GendstTransInfo get surfacebuffer failed");
        }
    }
#endif
    XYaxis xyAxis = {xAxis, yAxis};
    uint8_t *dst = reinterpret_cast<uint8_t *>(dstMemory->data.data);
    PixelYuvUtils::SetTranslateDataDefault(dst, width, height);
    if (!PixelYuvUtils::YuvTranslate(data_, imageInfo_, dst, xyAxis)) {
        dstMemory->Release();
        return;
    }
    imageInfo_.size.width = width;
    imageInfo_.size.height = height;

    SetPixelsAddr(dst, nullptr, dstSize, dstMemory->GetType(), nullptr);
}

uint32_t PixelYuv::ReadPixel(const Position &pos, uint32_t &dst)
{
    if (pos.x < 0 || pos.y < 0) {
        IMAGE_LOGE("read pixel by pos input invalid exception. [x(%{public}d), y(%{public}d)]", pos.x, pos.y);
        return ERR_IMAGE_INVALID_PARAMETER;
    }
    if (data_ == nullptr) {
        IMAGE_LOGE("read pixel by pos source data is null.");
        return ERR_IMAGE_READ_PIXELMAP_FAILED;
    }
    PixelYuvUtils::YuvReadPixel(data_, imageInfo_.size, imageInfo_.pixelFormat, pos, dst);
    return SUCCESS;
}

bool PixelYuv::WritePixels(const uint32_t &color)
{
    if (data_ == nullptr) {
        IMAGE_LOGE("erase pixels by color current pixel map data is null.");
        return false;
    }
    PixelYuvUtils::Yuv420WritePixels(imageInfo_.size, data_, imageInfo_.pixelFormat, color);
    return true;
}

uint32_t PixelYuv::WritePixel(const Position &pos, const uint32_t &color)
{
    if (pos.x < 0 || pos.y < 0) {
        IMAGE_LOGE("write pixel by pos but input position is invalid. [x(%{public}d), y(%{public}d)]", pos.x, pos.y);
        return ERR_IMAGE_INVALID_PARAMETER;
    }
    if (data_ == nullptr) {
        IMAGE_LOGE("write pixel by pos but current pixelmap data is nullptr.");
        return ERR_IMAGE_WRITE_PIXELMAP_FAILED;
    }
    PixelYuvUtils::YuvWritePixel(data_, imageInfo_.size, imageInfo_.pixelFormat, pos, color);
    return SUCCESS;
}

ColorYuv420 PixelYuv::GetYuv420Color(uint32_t x, uint32_t y)
{
    ColorYuv420 colorYuv;
    ImageInfo imageInfo;
    GetImageInfo(imageInfo);
    PixelFormat format = imageInfo.pixelFormat;
    colorYuv.colorY = PixelYuvUtils::GetYuv420Y(x, y, imageInfo.size.width, data_);
    colorYuv.colorU = PixelYuvUtils::GetYuv420U(x, y, imageInfo.size, format, data_);
    colorYuv.colorV = PixelYuvUtils::GetYuv420V(x, y, imageInfo.size, format, data_);
    return colorYuv;
}

void PixelYuv::SetPixelsAddr(void *addr, void *context, uint32_t size, AllocatorType type, CustomFreePixelMap func)
{
    if (data_ != nullptr) {
        IMAGE_LOGE("SetPixelsAddr release the existed data first");
        FreePixelMap();
    }
    if (type == AllocatorType::SHARE_MEM_ALLOC && context == nullptr) {
        IMAGE_LOGE("SetPixelsAddr error type %{public}d ", type);
    }
    data_ = static_cast<uint8_t *>(addr);
    context_ = context;
    pixelsSize_ = size;
    allocatorType_ = type;
    custFreePixelMap_ = func;
    if (type == AllocatorType::DMA_ALLOC && rowDataSize_ != 0) {
        SetImageInfo(imageInfo_, true);
    }
}

bool PixelYuv::CheckPixelsInput(const uint8_t *dst, const uint64_t &bufferSize, const uint32_t &offset,
    const Rect &region)
{
    if (dst == nullptr) {
        IMAGE_LOGE("CheckPixelsInput input dst address is null.");
        return false;
    }

    if (bufferSize == 0) {
        IMAGE_LOGE("CheckPixelsInput input buffer size is 0.");
        return false;
    }

    if (region.left < 0 || region.top < 0 || static_cast<uint64_t>(offset) > bufferSize) {
        IMAGE_LOGE("CheckPixelsInput left(%{public}d) or top(%{public}d) or offset(%{public}u) < 0.",
            region.left, region.top, offset);
        return false;
    }
    if (region.width <= 0 || region.height <= 0 || region.width > MAX_DIMENSION || region.height > MAX_DIMENSION) {
        IMAGE_LOGE("CheckPixelsInput width(%{public}d) or height(%{public}d) is < 0.", region.width,
            region.height);
        return false;
    }
    if (region.left > GetWidth() - region.width) {
        IMAGE_LOGE("CheckPixelsInput left(%{public}d) + width(%{public}d) is > PixelYuv width(%{public}d).",
            region.left, region.width, GetWidth());
        return false;
    }
    if (region.top > GetHeight() - region.height) {
        IMAGE_LOGE("CheckPixelsInput top(%{public}d) + height(%{public}d) is > PixelYuv height(%{public}d).",
            region.top, region.height, GetHeight());
        return false;
    }

    if (static_cast<uint64_t>(offset) > (bufferSize - NUM_3)) {
        IMAGE_LOGE(
            "CheckPixelsInput fail, height(%{public}d), width(%{public}d) "
            "offset(%{public}u), bufferSize:%{public}llu.",
            region.height, region.width, offset,
            static_cast<unsigned long long>(bufferSize));
        return false;
    }
    return true;
}

void PixelYuv::SetRowDataSizeForImageInfo(ImageInfo info)
{
    rowDataSize_ = info.size.width * NUM_3 / NUM_2;
    return;
}

uint32_t PixelYuv::GetImageSize(int32_t width, int32_t height)
{
    return width * height + ((width + 1) / NUM_2) * ((height + 1) / NUM_2) * NUM_2;
}

void PixelYuv::AssignYuvDataOnType(PixelFormat format, int32_t width, int32_t height)
{
    if (format == PixelFormat::YU12 || format == PixelFormat::YV12) {
        yuvDataInfo_.y_width = width;
        yuvDataInfo_.y_height = height;
        yuvDataInfo_.y_stride = width;
        yuvDataInfo_.u_stride = GetUStride(width);
        yuvDataInfo_.v_stride = GetUStride(width);
    }
    if (format == PixelFormat::NV12 || format == PixelFormat::NV21) {
        yuvDataInfo_.y_width = width;
        yuvDataInfo_.y_height = height;
        yuvDataInfo_.y_stride = width;
        yuvDataInfo_.uv_width = GetUVStride(width);
        yuvDataInfo_.uv_height = GetUVHeight(height);
        yuvDataInfo_.uv_stride = GetUVStride(width);
    }
}

bool PixelYuv::IsYuvFormat(PixelFormat format)
{
    return format == PixelFormat::NV21 || format == PixelFormat::NV12 ||
        format == PixelFormat::YU12 || format == PixelFormat::YV12;
}

#ifdef IMAGE_COLORSPACE_FLAG
static void ToImageInfo(ImageInfo &info, SkImageInfo &skInfo, bool sizeOnly = true)
{
    info.size.width = skInfo.width();
    info.size.height = skInfo.height();
    if (!sizeOnly) {
        info.alphaType = ImageTypeConverter::ToAlphaType(skInfo.alphaType());
        info.pixelFormat = ImageTypeConverter::ToPixelFormat(skInfo.colorType());
    }
}

bool PixelYuv::CheckColorSpace(const OHOS::ColorManager::ColorSpace &grColorSpace)
{
    auto grName = grColorSpace.GetColorSpaceName();
    if (grColorSpace_ != nullptr && isSameColorSpace(*grColorSpace_, grColorSpace)) {
        if (grColorSpace_->GetColorSpaceName() != grName) {
            InnerSetColorSpace(grColorSpace);
            IMAGE_LOGI("applyColorSpace inner set");
        }
        return true;
    }
    return false;
}

int32_t PixelYuv::ColorSpaceBGRAToYuv(uint8_t *bgraData, SkTransYuvInfo &dst, ImageInfo &imageInfo,
    PixelFormat &format, const OHOS::ColorManager::ColorSpace &grColorSpace)
{
    int32_t dstWidth = dst.info.width();
    int32_t dstHeight = dst.info.height();
    uint32_t pictureSize = GetImageSize(dstWidth, dstHeight);
    MemoryData memoryYuvData = {nullptr, pictureSize, "Trans ImageData", {dstWidth, dstHeight}};
    auto yuvMemory = MemoryManager::CreateMemory(allocatorType_, memoryYuvData);
    if (yuvMemory == nullptr) {
        yuvMemory->Release();
        IMAGE_LOGE("applyColorSpace CreateYuvMemory failed");
        return ERR_IMAGE_COLOR_CONVERT;
    }
    if (!PixelYuvUtils::BGRAToYuv420(bgraData, reinterpret_cast<uint8_t *>(yuvMemory->data.data),
        dstWidth, dstHeight, format)) {
            return ERR_IMAGE_COLOR_CONVERT;
        }
    imageInfo.pixelFormat = format;
    dst.info = ToSkImageInfo(imageInfo, grColorSpace.ToSkColorSpace());
    ToImageInfo(imageInfo, dst.info);
    auto grName = grColorSpace.GetColorSpaceName();
    grColorSpace_ = std::make_shared<OHOS::ColorManager::ColorSpace>(dst.info.refColorSpace(), grName);
    SetPixelsAddr(reinterpret_cast<void *>(yuvMemory->data.data), nullptr, pictureSize, yuvMemory->GetType(), nullptr);
    return SUCCESS;
}

uint32_t PixelYuv::ApplyColorSpace(const OHOS::ColorManager::ColorSpace &grColorSpace)
{
    if (CheckColorSpace(grColorSpace)) {
        return SUCCESS;
    }
    PixelFormat format = imageInfo_.pixelFormat;
    imageInfo_.pixelFormat = PixelFormat::BGRA_8888;
    SkTransYuvInfo src;
    src.info = ToSkImageInfo(imageInfo_, ToSkColorSpace(this));
    uint64_t rowStride = src.info.minRowBytes();
    uint8_t *srcData = data_;
#if !defined(_WIN32) && !defined(_APPLE) && !defined(IOS_PLATFORM) && !defined(A_PLATFORM)
    if (GetAllocatorType() == AllocatorType::DMA_ALLOC && GetFd() != nullptr) {
        SurfaceBuffer *sbBuffer = reinterpret_cast<SurfaceBuffer *>(GetFd());
        rowStride = sbBuffer->GetStride();
    }
    srcData = static_cast<uint8_t *>(GetWritablePixels());
#endif

    int32_t width = imageInfo_.size.width;
    int32_t height = imageInfo_.size.height;
    std::unique_ptr<uint8_t[]> RGBAdata = std::make_unique<uint8_t[]>(width * height * NUM_4);
    PixelYuvUtils::Yuv420ToBGRA(srcData, RGBAdata.get(), width, height, format);
    src.bitmap.installPixels(src.info, RGBAdata.get(), rowStride);
    // Build sk target infomation
    SkTransYuvInfo dst;
    dst.info = ToSkImageInfo(imageInfo_, grColorSpace.ToSkColorSpace());
    MemoryData memoryData = {nullptr, width * height * NUM_4, "ApplyColorSpace ImageData",
        {dst.info.width(), dst.info.height()}};
    auto dstMemory = MemoryManager::CreateMemory(allocatorType_, memoryData);
    if (dstMemory == nullptr) {
        IMAGE_LOGE("applyColorSpace CreateMemory failed");
        return ERR_IMAGE_COLOR_CONVERT;
    }
    // Transfor pixels by readPixels
    if (!src.bitmap.readPixels(dst.info, dstMemory->data.data, rowStride, 0, 0)) {
        dstMemory->Release();
        IMAGE_LOGE("ReadPixels failed");
        return ERR_IMAGE_COLOR_CONVERT;
    }

    uint8_t *bgraData = reinterpret_cast<uint8_t *>(dstMemory->data.data);
    if (ColorSpaceBGRAToYuv(bgraData, dst, imageInfo_, format, grColorSpace) != SUCCESS) {
        dstMemory->Release();
        IMAGE_LOGE("ColorSpaceBGRAToYuv failed");
        return ERR_IMAGE_COLOR_CONVERT;
    }
    SetImageInfo(imageInfo_, true);
    return SUCCESS;
}
#endif
} // namespace Media
} // namespace OHOS