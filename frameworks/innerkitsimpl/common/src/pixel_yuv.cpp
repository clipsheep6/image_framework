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
#include "include/core/SkBitmap.h"
#include "include/core/SkCanvas.h"
#include "include/core/SkImage.h"
#include "hilog/log.h"
#include "hitrace_meter.h"
#include "log_tags.h"
#include "media_errors.h"
#include "pubdef.h"
#include "securec.h"
#include "image_log.h"
#include "image_mdk_common.h"
#include "image_system_properties.h"

#include "surface_buffer.h"

#ifdef LIBYUV_ENABLE
#include "libyuv.h"
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
static const int32_t degrees90 = 90;   // 90度角
static const int32_t degrees180 = 180; // 180度角
static const int32_t degrees270 = 270; // 270度角
static const int32_t degrees360 = 360; // 360度角
constexpr int32_t MAX_DIMENSION = INT32_MAX >> NUM_2;
constexpr uint8_t Y_SHIFT = 16;
constexpr uint8_t U_SHIFT = 8;
constexpr uint8_t V_SHIFT = 0;
constexpr uint8_t YUV420_MASK = 0xFF;
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

struct TransInfos {
    SkMatrix matrix;
};

struct SkTransInfo {
    SkRect r;
    SkImageInfo info;
    SkBitmap bitmap;
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

PixelYUV::~PixelYUV()
{
    FreePixelMap();
}

const uint8_t *PixelYUV::GetPixel8(int32_t x, int32_t y)
{
    IMAGE_LOGE("GetPixel8 is not support on PixelYUV");
    return nullptr;
}

const uint16_t *PixelYUV::GetPixel16(int32_t x, int32_t y)
{
    IMAGE_LOGE("GetPixel16 is not support on PixelYUV");
    return nullptr;
}

const uint32_t *PixelYUV::GetPixel32(int32_t x, int32_t y)
{
    IMAGE_LOGE("GetPixel32 is not support on PixelYUV");
    return nullptr;
}

bool PixelYUV::GetARGB32Color(int32_t x, int32_t y, uint32_t &color)
{
    IMAGE_LOGE("GetARGB32Color is not support on PixelYUV");
    return false;
}

uint8_t PixelYUV::GetARGB32ColorA(uint32_t color)
{
    IMAGE_LOGE("GetARGB32ColorA is not support on PixelYUV");
    return 0;
}

uint8_t PixelYUV::GetARGB32ColorR(uint32_t color)
{
    IMAGE_LOGE("GetARGB32ColorR is not support on PixelYUV");
    return 0;
}

uint8_t PixelYUV::GetARGB32ColorG(uint32_t color)
{
    IMAGE_LOGE("GetARGB32ColorG is not support on PixelYUV");
    return 0;
}

uint8_t PixelYUV::GetARGB32ColorB(uint32_t color)
{
    IMAGE_LOGE("GetARGB32ColorB is not support on PixelYUV");
    return 0;
}

uint32_t PixelYUV::SetAlpha(const float percent)
{
    IMAGE_LOGE("SetAlpha is not support on PixelYUV");
    return ERR_IMAGE_DATA_UNSUPPORT;
}

uint32_t PixelYUV::getPixelBytesNumber()
{
    IMAGE_LOGE("getPixelBytesNumber is not support on PixelYUV");
    return ERR_IMAGE_DATA_UNSUPPORT;
}

int32_t PixelYUV::GetByteCount()
{
    IMAGE_LOGE("GetByteCount is not support on PixelYUV");
    return ERR_IMAGE_DATA_UNSUPPORT;
}

void PixelYUV::rotate(float degrees)
{
    uint32_t count = GetImageSize(imageInfo_.size.width, imageInfo_.size.height);
    Size desiredSize = {imageInfo_.size.width, imageInfo_.size.height};
    MemoryData memoryData = {nullptr, count, "Rotate ImageData", desiredSize};
    auto m = MemoryManager::CreateMemory(allocatorType_, memoryData);
    if (m == nullptr) {
        IMAGE_LOGE("rotate CreateMemory failed");
        return;
    }
#if !defined(IOS_PLATFORM) && !defined(A_PLATFORM)
    if (allocatorType_ == AllocatorType::DMA_ALLOC) {
        if (m->extend.data == nullptr) {
            IMAGE_LOGE("GendstTransInfo get surfacebuffer failed");
        }
    }
#endif
    uint8_t *dst = reinterpret_cast<uint8_t *>(m->data.data);
    if (!YUVRotate(data_, imageInfo_.size, dst, degrees, imageInfo_.pixelFormat)) {
        m->Release();
        return;
    }
    if (memcpy_s(data_, count, dst, count) != 0) {
        IMAGE_LOGE("rotate memcpy failed");
        return;
    }
    assignYUVDataOnType(imageInfo_.pixelFormat, imageInfo_.size.width, imageInfo_.size.height);
    return;
}

uint32_t PixelYUV::crop(const Rect &rect)
{
    ImageInfo imageInfo;
    GetImageInfo(imageInfo);
    Size desiredSize = {rect.width, rect.height};
    MemoryData memoryData = {nullptr, GetImageSize(rect.width, rect.height), "Trans ImageData", desiredSize};
    auto m = MemoryManager::CreateMemory(allocatorType_, memoryData);
    if (m == nullptr) {
        IMAGE_LOGE("crop CreateMemory failed");
        return ERR_IMAGE_CROP;
    }
#if !defined(IOS_PLATFORM) && !defined(A_PLATFORM)
    if (allocatorType_ == AllocatorType::DMA_ALLOC) {
        if (m->extend.data == nullptr) {
            IMAGE_LOGE("GendstTransInfo get surfacebuffer failed");
        }
    }
#endif
    if (!YUVCrop(data_, imageInfo_.size, (uint8_t *)m->data.data, rect, imageInfo.pixelFormat)) {
        m->Release();
        return ERR_IMAGE_CROP;
    }
    imageInfo.size.height = rect.height;
    imageInfo.size.width = rect.width;

    SetPixelsAddr(m->data.data, m->extend.data, m->data.size, m->GetType(), nullptr);
    SetImageInfo(imageInfo, true);
    assignYUVDataOnType(imageInfo.pixelFormat, imageInfo.size.width, imageInfo.size.height);
    return SUCCESS;
}

bool PixelYUV::scaleConvert(MemoryData memoryData, ImageInfo imageInfo, PixelFormat format)
{
    bool option = (imageInfo.pixelFormat != PixelFormat::UNKNOWN) ? true : false;
    option = isYUVImage(imageInfo.pixelFormat);
    if (option) {
        uint32_t pictureSize = imageInfo.size.width * imageInfo.size.height * NUM_4;
        memoryData = {nullptr, pictureSize, "scale RGB ImageData", {imageInfo.size.width, imageInfo.size.height}};
        auto m = MemoryManager::CreateMemory(allocatorType_, memoryData);
        if (m == nullptr) {
            IMAGE_LOGE("scaleConvert CreateMemory failed");
            return false;
        }
#if !defined(IOS_PLATFORM) && !defined(A_PLATFORM)
        if (allocatorType_ == AllocatorType::DMA_ALLOC) {
            if (m->extend.data == nullptr) {
                IMAGE_LOGE("GendstTransInfo get surfacebuffer failed");
            }
        }
#endif
        uint8_t *argbData = reinterpret_cast<uint8_t *>(m->data.data);
        if (!YUV420ToBGRA(data_, argbData, imageInfo.size.width, imageInfo.size.height, imageInfo.pixelFormat)) {
            m->Release();
            return false;
        }
        format = imageInfo.pixelFormat;
        imageInfo.pixelFormat = PixelFormat::BGRA_8888;
        SetPixelsAddr(argbData, nullptr, pictureSize, m->GetType(), nullptr);
    } else {
        uint32_t pictureSize = GetImageSize(imageInfo.size.width, imageInfo.size.height);
        memoryData = {nullptr, pictureSize, "scale YUV ImageData", {imageInfo.size.width, imageInfo.size.height}};
        auto m = MemoryManager::CreateMemory(allocatorType_, memoryData);
        if (m == nullptr) {
            IMAGE_LOGE("scaleConvert CreateMemory failed");
            return false;
        }
#if !defined(IOS_PLATFORM) && !defined(A_PLATFORM)
        if (allocatorType_ == AllocatorType::DMA_ALLOC) {
            if (m->extend.data == nullptr) {
                IMAGE_LOGE("GendstTransInfo get surfacebuffer failed");
            }
        }
#endif
        uint8_t *YUVData = reinterpret_cast<uint8_t *>(m->data.data);
        if (BGRAToYUV420(data_, YUVData, imageInfo.size.width, imageInfo.size.height, format)) {
            m->Release();
            return false;
        }
        SetPixelsAddr(YUVData, nullptr, pictureSize, m->GetType(), nullptr);
    }
    return true;
}

#ifndef LIBYUV_ENABLE
void PixelYUV::scale(float xAxis, float yAxis)
{
    ImageTrace imageTrace("PixelMap scale");
    return scale(xAxis, yAxis, AntiAliasingOption::NONE);
}

void PixelYUV::scale(float xAxis, float yAxis, const AntiAliasingOption &option)
{
    ImageTrace imageTrace("PixelMap scale");
    TransInfos infos;
    infos.matrix.setScale(xAxis, yAxis);
    PixelFormat format = PixelFormat::UNKNOWN;
    MemoryData memoryData;

    bool ret = scaleConvert(memoryData, imageInfo_, format);
    if (!ret) {
        IMAGE_LOGE("scaleConvert failed");
        return;
    }

    if (!DoTranslation(infos, option)) {
        IMAGE_LOGE("scale falied");
    }

    ret = scaleConvert(memoryData, imageInfo_, format);
    if (!ret) {
        IMAGE_LOGE("scaleConvert failed");
        return;
    }
    imageInfo_.pixelFormat = format;
    assignYUVDataOnType(imageInfo_.pixelFormat, imageInfo_.size.width, imageInfo_.size.height);
}

bool PixelYUV::resize(float xAxis, float yAxis)
{
    ImageTrace imageTrace("PixelMap resize");
    TransInfos infos;
    infos.matrix.setScale(xAxis, yAxis);
    PixelFormat format = PixelFormat::UNKNOWN;
    MemoryData memoryData;
    bool ret = scaleConvert(memoryData, imageInfo_, format);
    if (!ret) {
        IMAGE_LOGE("scaleConvert failed");
        return false;
    }
    if (!DoTranslation(infos)) {
        IMAGE_LOGE("resize falied");
        return false;
    }
    ret = scaleConvert(memoryData, imageInfo_, format);
    if (!ret) {
        IMAGE_LOGE("scaleConvert failed");
        return false;
    }
    imageInfo_.pixelFormat = format;
    assignYUVDataOnType(imageInfo_.pixelFormat, imageInfo_.size.width, imageInfo_.size.height);
    return true;
}

static int32_t getUOffset(int32_t width, int32_t height)
{
    return width * height;
}

static int32_t getVOffset(int32_t width, int32_t height)
{
    return width * height + ((width + 1) / NUM_2) * ((height + 1) / NUM_2);
}

//The stride of u and v are the same,YUV420P u, v single planer
static int32_t getUStride(int32_t width)
{
    return (width + 1) / NUM_2;
}

static int32_t getUVHeight(int32_t height)
{
    return (height + 1) / NUM_2;
}

//YUV420SP,u、v blend planer
static int32_t getUVStride(int32_t width)
{
    return (width + 1) / NUM_2 * NUM_2;
}

void PixelYUV::flipYUVXaixs(const uint8_t *src, uint8_t *dst, PixelFormat format, Size &size, int32_t &ret)
{
    switch (format) {
        case PixelFormat::YU12:
        case PixelFormat::YV12:
            ret = I420Copy(src, size.width, src + getUOffset(size.width, size.height),
                getUStride(size.width), src + getVOffset(size.width, size.height), getUStride(size.width), dst,
                size.width, dst + getUOffset(size.width, size.height), getUStride(size.width),
                dst + getVOffset(size.width, size.height), getUStride(size.width), size.width, -size.height);
            break;
        case PixelFormat::NV12:
        case PixelFormat::NV21:
            ret = NV12Copy(src, size.width, src + getUOffset(size.width, size.height),
                getUVStride(size.width), dst, size.width, dst + getUOffset(size.width, size.height),
                getUVStride(size.width), size.width, -size.height);
            break;
        default:
            break;
    }
}

void PixelYUV::flipYUVYaixs(const uint8_t *src, uint8_t *dst, PixelFormat format, Size &size, int32_t &ret)
{
    switch (format) {
        case PixelFormat::YU12:
        case PixelFormat::YV12:
            ret = I420Mirror(src, size.width, src + getUOffset(size.width, size.height),
                getUStride(size.width), src + getVOffset(size.width, size.height), getUStride(size.width),
                dst, size.width, dst + getUOffset(size.width, size.height), getUStride(size.width),
                dst + getVOffset(size.width, size.height), getUStride(size.width),
                size.width, size.height);
            break;
        case PixelFormat::NV12:
        case PixelFormat::NV21:
            ret = NV12Mirror(src, size.width, src +getUOffset(size.width, size.height),
                getUVStride(size.width), dst, size.width, dst + getUOffset(size.width, size.height),
                getUVStride(size.width), size.width, size.height);
            break;
        default:
            break;
    }
}

void PixelYUV::flip(bool xAxis, bool yAxis)
{
    int32_t srcW = imageInfo_.size.width;
    int32_t srcH = imageInfo_.size.height;
    PixelFormat format = imageInfo_.pixelFormat;
    const uint8_t *src = data_;
    uint8_t *dst = nullptr;
    Size desiredSize = {imageInfo_.size.width, imageInfo_.size.height};
    uint32_t pictureSize = GetImageSize(srcW, srcH);
    MemoryData memoryData = {nullptr, pictureSize, "flip ImageData", desiredSize};
    auto m = MemoryManager::CreateMemory(allocatorType_, memoryData);
    if (m == nullptr) {
        IMAGE_LOGE("flip CreateMemory failed");
        return;
    }
#if !defined(IOS_PLATFORM) && !defined(A_PLATFORM)
    if (allocatorType_ == AllocatorType::DMA_ALLOC) {
        if (m->extend.data == nullptr) {
            IMAGE_LOGE("GendstTransInfo get surfacebuffer failed");
        }
    }
#endif
    dst = reinterpret_cast<uint8_t *>(m->data.data);
    int32_t ret = SUCCESS;
    if (xAxis) {
        flipYUVXaixs(src, dst, format, imageInfo_.size, ret);
    }
    if (yAxis) {
        flipYUVYaixs(src, dst, format, imageInfo_.size, ret);
    }
    if (ret != SUCCESS) {
        IMAGE_LOGE("YUV flipYUV failed");
        m->Release();
        return;
    }
    SetPixelsAddr(dst, m->extend.data, m->data.size, m->GetType(), nullptr);
    assignYUVDataOnType(format, srcW, srcH);
}

#else
void PixelYUV::scale(float xAxis, float yAxis)
{
    scaleYUV420(xAxis, yAxis);
}

constexpr int32_t ANTIALIASING_SIZE = 350;

static bool IsSupportAntiAliasing(const ImageInfo& imageInfo, const AntiAliasingOption &option)
{
    return option != AntiAliasingOption::NONE && imageInfo.size.width <= ANTIALIASING_SIZE &&
            imageInfo.size.height <= ANTIALIASING_SIZE;
}

void PixelYUV::scale(float xAxis, float yAxis, const AntiAliasingOption &option)
{
    ImageInfo imageInfo;
    GetImageInfo(imageInfo);
    AntiAliasingOption operation = AntiAliasingOption::NONE;
    if (ImageSystemProperties::GetAntiAliasingEnabled() && IsSupportAntiAliasing(imageInfo, option)) {
        operation = AntiAliasingOption::MEDIUM;
    } else {
        operation = option;
    }
    scaleYUV420(xAxis, yAxis, operation);
}

bool PixelYUV::resize(float xAxis, float yAxis)
{
    scaleYUV420(xAxis, yAxis);
    return true;
}
static void PixelYUV::ConvertYuvMode(libyuv::FilterMode filterMode, const AntiAliasingOption &option)
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

void PixelYUV::scaleYUV420(float xAxis, float yAxis, const AntiAliasingOption &option)
{
    libyuv::FilterMode filterMode = libyuv::FilterMode::kFilterNone;
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
    MemoryData memoryData = {nullptr, pictureSize, "scaleYUV420 ImageData", desiredSize};
    auto m = MemoryManager::CreateMemory(allocatorType_, memoryData);
    if (m == nullptr) {
        IMAGE_LOGE("ScaleYUV420 CreateMemory failed");
        return;
    }
    uint8_t *dst = reinterpret_cast<uint8_t *>(m->data.data);
    if (imageInfo.pixelFormat == PixelFormat::YU12 || imageInfo.pixelFormat == PixelFormat::YV12) {
        if (SUCCESS != libyuv::I420Scale(src, srcW, src + getUOffset(srcW, srcH), getUStride(srcW),
            src + getVOffset(srcW, srcH), getUStride(srcW), srcW, srcH, dst, dstW,
            dst + getUOffset(dstW, dstH), getUStride(dstW), dst + getVOffSet(dstW, dstH),
            getUStride(dstW), dstW, dstH, filterMode)) {
            m->Release();
            IMAGE_LOGE("Scale YUV420P failed");
            return;
        }
    }
    if (imageInfo.pixelFormat == PixelFormat::NV12 || imageInfo.pixelFormat == PixelFormat::NV21) {
        uint32_t srcHalfW = getUStride(srcW);
        uint32_t srcHalfH = getUVHeight(srcH);
        uint32_t dstHalfW = getUStride(dstW);
        uint32_t dstHalfH = getUVHeight(dstH);

        // resize y_plane
        if (SUCCESS != libyuv::ScalePlane(src, srcW, srcW, srcH, dst, dstW, dstW, dstH, filterMode)) {
            m->Release();
            IMAGE_LOGE("Scale YUV420SP, resize y_plane failed");
            return;
        }
        //Whether the row width is odd or even, U and Z are equal in size
        uint32_t srcUSize = getUStride(srcW) * getUVHeight(srcH);
        uint32_t dstUSize = getUStride(dstW) * getUVHeight(dstH);
        // Split VUplane
        uint8_t *uvData = static_cast<uint8_t *>(malloc(NUM_2 * srcUSize));

        // NV21
        uint8_t *vData = uvData;
        uint8_t *uData = uvData + srcUSize;
        // If it's in NV12 format，swap u and v
        if (imageInfo.pixelFormat == PixelFormat::NV12) {
            uint8_t *tempSwap = vData;
            vData = uData;
            uData = tempSwap;
        }

        const uint8_t *src_uv = src + getUOffset(srcW, srcH);
        libyuv::SplitUVPlane(src_uv, NUM_2 * srcHalfW, vData, srcHalfW, uData, srcHalfW, srcHalfW, srcHalfH);

        // malloc memory to store temp u v
        uint8_t *tempUVData = static_cast<uint8_t *>(malloc(NUM_2 * dstUSize));
        uint8_t *tempVData = tempUVData;
        uint8_t *tempUData = tempUVData + dstUSize;
        if (imageInfo.pixelFormat == PixelFormat::NV12) {
            uint8_t *tempSwap = tempVData;
            tempVData = tempUData;
            tempUData = tempSwap;
        }

        // resize u and v
        if (SUCCESS != libyuv::ScalePlane(uData, srcHalfW, srcHalfW, srcHalfH, tempUData, dstHalfW, dstHalfW, dstHalfH,
            filterMode)) {
            m->Release();
            IMAGE_LOGE("Scale YUV420SP, resize u or v failed");
            return;
        }
        if (SUCCESS != libyuv::ScalePlane(vData, srcHalfW, srcHalfW, srcHalfH, tempVData, dstHalfW, dstHalfW, dstHalfH,
            filterMode)) {
            m->Release();
            IMAGE_LOGE("Scale YUV420SP, resize u or v failed");
            return;
        }

        uint8_t *dst_uv = dst + getUOffset(dstW, dstH);
        libyuv::MergeUVPlane(tempVData, dstHalfW, tempUData, dstHalfW, dst_uv, NUM_2 * dstHalfW, dstHalfW, dstHalfH);

        free(uvData);
        uData = vData = uvData = nullptr;
        free(tempUVData);
        tempUData = tempVData = tempUVData = nullptr;

        SetPixelsAddr(reinterpret_cast<void *>(dst), m->extend.data, m->data.size, m->GetType(), nullptr);
        imageInfo.size.width = dstW;
        imageInfo.size.height = dstH;
        SetImageInfo(imageInfo, true);
    }
}

void PixelYUV::flip(bool xAxis, bool yAxis)
{
    ImageInfo imageInfo;
    GetImageInfo(imageInfo);
    int32_t width = imageInfo.size.width;
    int32_t height = imageInfo.size.height;
    const uint8_t *src = data_;
    uint8_t *dst = nullptr;
    uint32_t pictureSize = GetImageSize(width, height);
    MemoryData memoryData = {nullptr, pictureSize, "flip ImageData", {width, height}};
    auto m = MemoryManager::CreateMemory(allocatorType_, memoryData);
    if (m == nullptr) {
        IMAGE_LOGE("flip CreateMemory failed");
        return;
    }
#if !defined(IOS_PLATFORM) && !defined(A_PLATFORM)
    if (allocatorType_ == AllocatorType::DMA_ALLOC) {
        if (m->extend.data == nullptr) {
            IMAGE_LOGE("GendstTransInfo get surfacebuffer failed");
        }
    }
#endif
    dst = reinterpret_cast<uint8_t *>(m->data.data);
    if (xAxis) {
        if (imageInfo.pixelFormat == PixelFormat::YU12 || imageInfo.pixelFormat == PixelFormat::YV12) {
            if (SUCCESS != libyuv::I420Copy(src, width, src + getUOffset(width, height), getUStride(width),
                src + getVOffset(width, height), getUStride(width), dst, width,
                dst + getUOffset(width, height), getUStride(width),
                dst + getVOffset(width, height), getUStride(width), width, -height)) {
                m->Release();
                IMAGE_LOGE("Flip YUV420P, YUV420P Copy failed");
                return;
            }
        }
        if (imageInfo.pixelFormat == PixelFormat::NV21 || imageInfo.pixelFormat == PixelFormat::NV12) {
            libyuv::NV12ToI420(src, width, src + getUOffset(width, height), getUVStride(width),
                dst, width, dst + getUOffset(width, height), getUStride(width),
                dst + getVOffset(width, height), getUStride(width), width, height);
            if (SUCCESS != libyuv::I420Copy(dst, width, dst + getUOffset(width, height), getUStride(width),
                dst + getVOffset(width, height), getUStride(width), const_cast<uint8_t *>(src), width,
                const_cast<uint8_t *>(src) + getUOffset(width, height), getUStride(width),
                const_cast<uint8_t *>(src) + getVOffset(width, height), getUStride(width), width, -height)) {
                m->Release();
                IMAGE_LOGE("Flip YUV420SP, YUV420SP Copy failed");
                return;
            }
            libyuv::I420ToNV12(src, width, src + getUOffset(width, height), getUStride(width),
                src + getVOffset(width, height), getUStride(width), dst, width,
                dst + getUOffset(width, height), getUVStride(width), width, height);
        }
    }

    if (yAxis) {
        if (imageInfo.pixelFormat == PixelFormat::YU12 || imageInfo.pixelFormat == PixelFormat::YV12) {
            libyuv::I420Mirror(src, width, src + getUOffset(width, height), getUStride(width),
                src + getVOffset(width, height), getUStride(width), dst, width,
                dst + getUOffset(width, height), getUStride(width),
                dst + getVOffset(width, height), getUStride(width), width, height);
        }
        if (imageInfo.pixelFormat == PixelFormat::NV21 || imageInfo.pixelFormat == PixelFormat::NV12) {
            libyuv::NV12ToI420(src, width, src + getUOffset(width, height), getUVStride(width), dst, width,
                dst + getUOffset(width, height), getUStride(width), dst + getVOffset(width, height), getUStride(width),
                width, height);
            libyuv::I420Mirror(dst, width, dst + getUOffset(width, height), getUStride(width),
                dst + getVOffset(width, height), getUStride(width), const_cast<uint8_t *>(src),
                width, const_cast<uint8_t *>(src) + getUOffset(width, height), getUStride(width),
                const_cast<uint8_t *>(src) + getVOffset(width, height), getUStride(width), width, height);
            libyuv::I420ToNV12(src, width, src + getUOffset(width, height), getUStride(width),
                src + getVOffset(width, height), getUStride(width), dst, width,
                dst + getUOffset(width, height), getUVStride(width), width, height);
        }
    }
    SetPixelsAddr(dst, m->extend.data, m->data.size, m->GetType(), nullptr);
}
#endif

uint32_t PixelYUV::WritePixels(const uint8_t *source, const uint64_t &bufferSize, const uint32_t &offset,
    const uint32_t &stride, const Rect &region)
{
    if (!CheckPixelsInput(source, bufferSize, offset, region)) {
        IMAGE_LOGE("write pixel by rect input parameter fail.");
        return ERR_IMAGE_INVALID_PARAMETER;
    }
    if (!IsEditable()) {
        IMAGE_LOGE("write pixel by rect PixelYUV data is not editable.");
        return ERR_IMAGE_PIXELMAP_NOT_ALLOW_MODIFY;
    }
    if (!ImageUtils::IsValidImageInfo(imageInfo_)) {
        IMAGE_LOGE("write pixel by rect current PixelYUV image info is invalid.");
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
    if (!WritePixelsConvert(source + offset, srcInfo, data_, dstPosition, imageInfo_)) {
        IMAGE_LOGE("write pixel by rect call WritePixelsConvert fail.");
        return ERR_IMAGE_WRITE_PIXELMAP_FAILED;
    }
    return SUCCESS;
}

uint32_t PixelYUV::ReadPixels(const uint64_t &bufferSize, const uint32_t &offset, const uint32_t &stride,
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
    if (!ReadPixelsConvert((void *)data_, srcPosition, imageInfo_, dst + offset, dstImageInfo)) {
        IMAGE_LOGE("read pixels by rect call ReadPixelsConvert fail.");
        return ERR_IMAGE_READ_PIXELMAP_FAILED;
    }
    return SUCCESS;
}

void PixelYUV::translate(float xAxis, float yAxis)
{
    if (xAxis == 0 && yAxis == 0) {
        return;
    }
    int32_t width = imageInfo_.size.width;
    int32_t height = imageInfo_.size.height;

    int32_t dstSize = GetImageSize(width, height);
    MemoryData memoryData = {nullptr, dstSize, "translate ImageData", {width, height}};
    auto m = MemoryManager::CreateMemory(allocatorType_, memoryData);
    if (m == nullptr) {
        IMAGE_LOGE("translate CreateMemory failed");
        return;
    }
#if !defined(IOS_PLATFORM) && !defined(A_PLATFORM)
    if (allocatorType_ == AllocatorType::DMA_ALLOC) {
        if (m->extend.data == nullptr) {
            IMAGE_LOGE("GendstTransInfo get surfacebuffer failed");
        }
    }
#endif
    uint8_t *dst = reinterpret_cast<uint8_t *>(m->data.data);
    if (!YUVTransLate(data_, imageInfo_.size, dst, imageInfo_.pixelFormat, xAxis, yAxis)) {
        m->Release();
        return;
    }
    imageInfo_.size.width = width;
    imageInfo_.size.height = height;
    if (memcpy_s(data_, dstSize, dst, dstSize) != 0) {
        IMAGE_LOGE("translate memcpy failed");
        return;
    }
}

uint32_t PixelYUV::ReadPixel(const Position &pos, uint32_t &dst)
{
    if (pos.x < 0 || pos.y < 0) {
        IMAGE_LOGE("read pixel by pos input invalid exception. [x(%{public}d), y(%{public}d)]", pos.x, pos.y);
        return ERR_IMAGE_INVALID_PARAMETER;
    }
    if (data_ == nullptr) {
        IMAGE_LOGE("read pixel by pos source data is null.");
        return ERR_IMAGE_READ_PIXELMAP_FAILED;
    }
    YUVReadPixel(data_, imageInfo_.size, imageInfo_.pixelFormat, pos, dst);
    return SUCCESS;
}

bool PixelYUV::WritePixels(const uint32_t &color)
{
    if (data_ == nullptr) {
        IMAGE_LOGE("erase pixels by color current pixel map data is null.");
        return false;
    }
    YUV420WritePixels(imageInfo_.size, data_, imageInfo_.pixelFormat, color);
    return true;
}

uint32_t PixelYUV::WritePixel(const Position &pos, const uint32_t &color)
{
    if (pos.x < 0 || pos.y < 0) {
        IMAGE_LOGE("write pixel by pos but input position is invalid. [x(%{public}d), y(%{public}d)]", pos.x, pos.y);
        return ERR_IMAGE_INVALID_PARAMETER;
    }
    if (data_ == nullptr) {
        IMAGE_LOGE("write pixel by pos but current pixelmap data is nullptr.");
        return ERR_IMAGE_WRITE_PIXELMAP_FAILED;
    }
    YUVWritePixel(data_, imageInfo_.size, imageInfo_.pixelFormat, pos, color);
    return SUCCESS;
}

ColorYUV420 PixelYUV::GetYUV420Color(uint32_t x, uint32_t y)
{
    ColorYUV420 colorYUV;
    ImageInfo imageInfo;
    GetImageInfo(imageInfo);
    int width = imageInfo.size.width;
    int height = imageInfo.size.height;
    PixelFormat format = imageInfo.pixelFormat;
    colorYUV.colorY = GetYUV420Y(x, y, width, data_);
    colorYUV.colorU = GetYUV420U(x, y, width, height, format, data_);
    colorYUV.colorV = GetYUV420V(x, y, width, height, format, data_);
    return colorYUV;
}

void PixelYUV::SetPixelsAddr(void *addr, void *context, uint32_t size, AllocatorType type, CustomFreePixelMap func)
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

bool PixelYUV::YUV420WritePixels(Size &size, uint8_t *srcPixels, const PixelFormat &format, const uint32_t &color)
{
    switch (format) {
        case PixelFormat::YU12:
        case PixelFormat::YV12: {
            bool isI420 = (format == PixelFormat::YU12 ? true : false);
            YUV420PWritePixels(size, srcPixels, color, isI420);
            return true;
        }
        case PixelFormat::NV21:
        case PixelFormat::NV12: {
            bool isNV12 = (format == PixelFormat::NV12 ? true : false);
            YUV420SPWritePixels(size, srcPixels, color, isNV12);
            return true;
        }
        default:
            return false;
    }
}

bool PixelYUV::YUVReadPixel(const uint8_t *srcPixels, Size &size, const PixelFormat &format, const Position &pos,
    uint32_t &dst)
{
    switch (format) {
        case PixelFormat::YU12:
        case PixelFormat::YV12: {
            bool isI420 = (format == PixelFormat::YU12) ? true : false;
            YUV420PReadPixel(srcPixels, size, pos, dst, isI420);
            return true;
        }
        case PixelFormat::NV21:
        case PixelFormat::NV12: {
            bool isNV12 = (format == PixelFormat::NV12) ? true : false;
            YUV420SPReadPixel(srcPixels, size, pos, dst, isNV12);
            return true;
        }
        default:
            return false;
    }
}

bool PixelYUV::YUVWritePixel(uint8_t *srcPixels, Size &size, const PixelFormat &format, const Position &pos,
    const uint32_t &color)
{
    switch (format) {
        case PixelFormat::YU12:
        case PixelFormat::YV12: {
            bool isI420 = (format == PixelFormat::YU12) ? true : false;
            YUV420PWritePixel(srcPixels, size, pos, color, isI420);
            return true;
        }
        case PixelFormat::NV21:
        case PixelFormat::NV12: {
            bool isNV12 = (format == PixelFormat::NV12) ? true : false;
            YUV420SPWritePixel(srcPixels, size, pos, color, isNV12);
            return true;
        }
        default:
            return false;
    }
}

bool PixelYUV::YUVTransLate(const uint8_t *srcPixels, Size &size, uint8_t *dstPixels, const PixelFormat &format,
    float xAxis, float yAxis)
{
    switch (format) {
        case PixelFormat::YU12:
        case PixelFormat::YV12: {
            bool isI420 = (format == PixelFormat::YU12) ? true : false;
            YUV420PTranslate(srcPixels, size, dstPixels, xAxis, yAxis, isI420);
            return true;
        }
        case PixelFormat::NV21:
        case PixelFormat::NV12: {
            YUV420SPTranslate(srcPixels, size, dstPixels, xAxis, yAxis);
            return true;
        }
        default:
            return false;
    }
}

bool PixelYUV::WritePixelsConvert(const void *srcPixels, const ImageInfo &srcInfo, void *dstPixels,
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

bool PixelYUV::ReadPixelsConvert(const void *srcPixels, const Position &srcPos, const ImageInfo &srcInfo,
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
    if (!YUVCrop((uint8_t *)srcPixels, srcInfo.size, static_cast<uint8_t *>(dstPixels), rect, srcInfo.pixelFormat)) {
        return false;
    }
    return true;
}

bool PixelYUV::CheckPixelsInput(const uint8_t *dst, const uint64_t &bufferSize, const uint32_t &offset,
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
        IMAGE_LOGE("CheckPixelsInput left(%{public}d) + width(%{public}d) is > PixelYUV width(%{public}d).",
            region.left, region.width, GetWidth());
        return false;
    }
    if (region.top > GetHeight() - region.height) {
        IMAGE_LOGE("CheckPixelsInput top(%{public}d) + height(%{public}d) is > PixelYUV height(%{public}d).",
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

bool PixelYUV::YUVCrop(const uint8_t *srcPixels, const Size &size, uint8_t *dstPixels, const Rect &rect,
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

bool PixelYUV::CropYU12(const uint8_t *srcPixels, const Size &size, uint8_t *dstPixels, Rect rect)
{
    rect.top = getUVStride(rect.top);
    for (int i = 0; i < size.height; i++) {
        if (memcpy_s(dstPixels + i * rect.width, rect.width,
            srcPixels + (rect.top + i) * size.width + rect.left, rect.width) != 0) {
            IMAGE_LOGE("CropYU12 memcpy yplane failed");
            return false;
        }
    }
    for (int i = 0; i < getUVHeight(rect.height); i++) {
        if (memcpy_s(dstPixels + getUOffset(rect.width, rect.height) + i * getUStride(rect.width),
            getUStride(rect.width), srcPixels + getUOffset(size.width, size.height) + (rect.top / NUM_2 + i) *
            getUStride(size.width) + rect.left / NUM_2, getUStride(rect.width)) != 0 ||
            memcpy_s(dstPixels + getVOffset(rect.width, rect.height) +
            i * getUStride(rect.width), getUStride(rect.width), srcPixels + getVOffset(size.width, size.height) +
            ((rect.top / NUM_2) + i) * getUStride(size.width) + rect.left / NUM_2, getUStride(rect.width)) != 0) {
            IMAGE_LOGE("CropYU12 memcpy uplane or vplane failed");
            return false;
            }
    }
    return true;
}

bool PixelYUV::CropNV21(const uint8_t *srcPixels, const Size &size, uint8_t *dstPixels, Rect rect)
{
    rect.top = getUVStride(rect.top);
    for (int32_t i = 0; i < rect.height; i++) {
        if (memcpy_s(dstPixels + i * rect.width, rect.width,
            srcPixels + (rect.top + i) * size.width + rect.left, rect.width) != 0) {
            IMAGE_LOGE("CropNV21 memcpy yplane failed");
            return false;
        }
    }
    const uint8_t *line = srcPixels + getUOffset(size.width, size.height) + rect.left;
    for (int32_t i = 0; i < getUVHeight(rect.height); ++i) {
        if (memcpy_s(dstPixels + getUOffset(rect.width, rect.height) + i * getUVStride(rect.width),
            getUVStride(rect.width),
            line + ((rect.top / NUM_2) + i) * getUVStride(size.width),
            getUVStride(rect.width)) != 0) {
            IMAGE_LOGE("CropNV21 memcpy uvplane failed");
            return false;
        }
    }
    return true;
}

#ifdef LIBYUV_ENABLE

static bool YUVRotateConvert(Size &size, int32_t degrees, int32_t &dstWidth, int32_t &dstHeight,
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
}

bool PixelYUV::YUVRotate(const uint8_t *srcPixels, Size &size, uint8_t *dstPixels, int32_t degrees,
    const PixelFormat &format)
{
    libyuv::RotationMode rotateNum = libyuv::RotationMode::kRotate0;
    int32_t dstWidth = size.width;
    int32_t dstHeight = size.height;
    if (YUVRotateConvert(size, degrees, dstWidth, dstHeight, rotateNum)) {
        IMAGE_LOGI("Rotate degress is 0, don't need rotate");
        return true;
    }
    MemoryData memoryData = {nullptr, GetImageSize(dstWidth, dstHeight), "YUVRotate ImageData", {dstWidth, dstHeight}};
    auto m = MemoryManager::CreateMemory(allocatorType_, memoryData);
    if (m == nullptr) {
        IMAGE_LOGE("translate CreateMemory failed");
        return;
    }
#if !defined(IOS_PLATFORM) && !defined(A_PLATFORM)
    if (allocatorType_ == AllocatorType::DMA_ALLOC) {
        if (m->extend.data == nullptr) {
            IMAGE_LOGE("GendstTransInfo get surfacebuffer failed");
        }
    }
#endif
    uint8_t *dstData = reinterpret_cast<uint8_t *>(m->data.data);
    switch (format) {
        case PixelFormat::NV21:
            if (libyuv::NV12ToI420Rotate(srcPixels, size.width, srcPixels + getUOffset(size.width, size.height),
                    getUVStride(size.width), dstData, dstWidth,
                    dstData + getVOffset(dstWidth, dstHeight),
                    getUStride(dstWidth), dstData + getUOffset(dstWidth, dstHeight),
                    getUStride(dstWidth), size.width, size.height, rotateNum) == -1) {
                m->Release();
                return false;
            }
            if (libyuv::I420ToNV21(dstData, dstWidth, dstData + getUOffset(dstWidth, dstHeight), getUStride(dstWidth),
                    dstData + getVOffset(dstWidth, dstHeight), getUStride(dstWidth),
                    dstPixels, dstWidth, dstPixels + getUOffset(dstWidth, dstHeight),
                    getUStride(dstWidth), dstWidth, dstHeight) == -1) {
                m->Release();
                return false;
            }
            m->Release();
            break;
        case PixelFormat::NV12:
            if (libyuv::NV12ToI420Rotate(srcPixels, size.width, srcPixels + getUOffset(size.width, size.height),
                    getUVStride(size.width), dstData, dstWidth,
                    dstData + getUOffset(dstWidth, dstHeight), getUStride(dstWidth),
                    dstData + getVOffset(dstWidth, dstHeight), getUStride(dstWidth),
                    size.width, size.height, rotateNum) == -1) {
                m->Release();
                return false;
            }
            if (libyuv::I420ToNV12(dstData, dstWidth, dstData + getUOffset(dstWidth, dstHeight),
                    getUStride(dstWidth), dstData + getVOffset(dstWidth, dstHeight),
                    getUStride(dstWidth), dstPixels, dstWidth, dstPixels + getUOffset(dstWidth, dstHeight),
                    getUVStride(dstWidth), dstWidth, dstHeight) == -1) {
                m->Release();
                return false;
            }
            m->Release();
            break;
        case PixelFormat::YV12:
        case PixelFormat::YU12:
            if (libyuv::I420Rotate(srcPixels, size.width, srcPixels + getUOffset(size.width, size.height),
                    getUStride(size.width), srcPixels + getVOffset(size.width, size.height),
                    getUStride(size.width), dstPixels, dstWidth, dstPixels + getUOffset(dstWidth, dstHeight),
                    getUStride(dstWidth), dstPixels + getVOffset(dstWidth, dstHeight),
                    getUStride(dstWidth), size.width, size.height, rotateNum) == -1) {
                return false;
            }
            break;
        default:
            return false;
    }
    size.width = dstWidth;
    size.height = dstHeight;
    return true;
}

#else

bool PixelYUV::YUVRotate(const uint8_t *srcPixels, Size &size, uint8_t *dstPixels, int32_t degrees,
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
            NV12ToI420(srcPixels, size, dstPixels);
            break;
        case PixelFormat::NV12:
            NV12ToI420(srcPixels, size, dstPixels);
            break;
        default:
            return false;
    }
    size_t count = GetImageSize(size.width, size.height);
    if (count == 0 || count > PIXEL_YUV_MAX_RAM_SIZE) {
        IMAGE_LOGE("YUVRotate parameter count:[%{public}d] error.", count);
        return false;
    }
    uint8_t *src = static_cast<uint8_t *>(malloc(count));
    if (!YV12Rotate(dstPixels, size, src, degrees)) {
        free(src);
        return false;
    }
    switch (format) {
        case PixelFormat::NV21:
            I420ToNV21(src, dstPixels, size.width, size.height);
            free(src);
            return true;
        case PixelFormat::NV12:
            I420ToNV12(src, dstPixels, size.width, size.height);
            free(src);
            return true;
        case PixelFormat::YV12:
        case PixelFormat::YU12:
            return true;
        default:
            return false;
    }
}
#endif

bool PixelYUV::I420AndYV12Transfers(const uint8_t *src, uint8_t *dst, int32_t width, int32_t height)
{
    uint32_t ySize = getUOffset(width, height);
    uint32_t uvDataSize = getUStride(width) * getUVHeight(height);
    if (memcpy_s(dst, ySize, src, ySize) != EOK ||
        memcpy_s(dst + ySize, uvDataSize, src + ySize + uvDataSize, uvDataSize) != EOK ||
        memcpy_s(dst + ySize + uvDataSize, uvDataSize, src + ySize, uvDataSize) != EOK) {
        IMAGE_LOGE("I420AndYV12Transfers memcpy failed");
        return false;
    }
    return true;
}

uint8_t PixelYUV::GetYUV420Y(uint32_t x, uint32_t y, int32_t width, const uint8_t *in)
{
    return *(in + y * width + x);
}

uint8_t PixelYUV::GetYUV420U(uint32_t x, uint32_t y, int32_t width, int32_t height, PixelFormat format,
                             const uint8_t *in)
{
    switch (format) {
        case PixelFormat::NV21:
            if (width & 1) {
                return *(in + y / NUM_2 * NUM_2 + width * height + (y / NUM_2) * (width - 1) + (x & ~1) + 1);
            }
            return *(in + width * height + (y / NUM_2) * width + (x & ~1) + 1);
        case PixelFormat::NV12:
            if (width & 1) {
                return *(in + y / NUM_2 * NUM_2 + width * height + (y / NUM_2) * (width - 1) + (x & ~1));
            }
            return *(in + width * height + (y / NUM_2) * width + (x & ~1));
        case PixelFormat::YU12:
            if (width & 1) {
                return *(in + y / NUM_2 + (width * height) + (y / NUM_2) * (width / NUM_2) + (x / NUM_2));
            }
            return *(in + (width * height) + (y / NUM_2) * (width / NUM_2) + (x / NUM_2));
        case PixelFormat::YV12:
            if (width & 1) {
                return *(in + height / NUM_2 + y / NUM_2 + (width * height) + (width / NUM_2) * (height / NUM_2) +
                            (y / NUM_2) * (width / NUM_2) + (x / NUM_2));
            }
            return *(in + (width * height) + (width / NUM_2) * (height / NUM_2) +
                        (y / NUM_2) * (width / NUM_2) + (x / NUM_2));
        default:
            break;
    }
    return SUCCESS;
}

uint8_t PixelYUV::GetYUV420V(uint32_t x, uint32_t y, int32_t width, int32_t height, PixelFormat format,
    const uint8_t *in)
{
    switch (format) {
        case PixelFormat::NV21:
            if (width & 1) {
                return *(in + y / NUM_2 * NUM_2 + width * height + (y / NUM_2) * (width - 1) + (x & ~1));
            }
            return *(in + width * height + (y / NUM_2) * width + (x & ~1));
        case PixelFormat::NV12:
            if (width & 1) {
                return *(in + y / NUM_2 * NUM_2 + width * height + (y / NUM_2) * (width - 1) + (x & ~1) + 1);
            }
            return *(in + width * height + (y / NUM_2) * width + (x & ~1) + 1);
        case PixelFormat::YU12:
            if (width & 1) {
                return *(in + height / NUM_2 + y / NUM_2 + (width * height) + (width / NUM_2) * (height / NUM_2) +
                            (y / NUM_2) * (width / NUM_2) + (x / NUM_2));
            }
            return *(in + (width * height) + (width / NUM_2) * (height / NUM_2) +
                        (y / NUM_2) * (width / NUM_2) + (x / NUM_2));
            break;
        case PixelFormat::YV12:
            if (width & 1) {
                return *(in + y / NUM_2 + (width * height) + (y / NUM_2) * (width / NUM_2) + (x / NUM_2));
            }
            return *(in + (width * height) + (y / NUM_2) * (width / NUM_2) + (x / NUM_2));
        default:
            break;
    }
    return SUCCESS;
}

void PixelYUV::SetRowDataSizeForImageInfo(ImageInfo info)
{
    rowDataSize_ = info.size.width * NUM_3 / NUM_2;
    return;
}

void PixelYUV::YUV420PTranslate(const uint8_t *srcPixels, Size &size, uint8_t *dstPixels, float xAxis, float yAxis,
    bool isI420)
{
    int32_t dstWidth = size.width;
    int32_t dstHeight = size.height;
    const uint8_t *srcU;
    const uint8_t *srcV;
    uint8_t *dstU;
    uint8_t *dstV;
    if (isI420) {
        srcU = srcPixels + getUOffset(size.width, size.height);
        srcV = srcPixels + getVOffset(size.width, size.height);
        dstU = dstPixels + getUOffset(dstWidth, dstHeight);
        dstV = dstPixels + getVOffset(dstWidth, dstHeight);
    } else {
        srcV = srcPixels + getUOffset(size.width, size.height);
        srcU = srcPixels + getVOffset(size.width, size.height);
        dstV = dstPixels + getUOffset(dstWidth, dstHeight);
        dstU = dstPixels + getVOffset(dstWidth, dstHeight);
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

    for (int32_t y = 0; y < getUVHeight(size.height); y++) {
        for (int32_t x = 0; x < getUStride(size.width); x++) {
            int32_t newX = x + xAxis / NUM_2;
            int32_t newY = y + yAxis / NUM_2;
            if (newX >= 0 && newY >= 0 && newX < size.width / NUM_2 && newY < size.height / NUM_2) {
                *(dstU + newY * getUStride(dstWidth) + newX) = *(srcU + y * getUStride(size.width) + x);
                *(dstV + newY * getUStride(dstWidth) + newX) = *(srcV + y * getUStride(size.width) + x);
            }
        }
    }
}

void PixelYUV::YUV420SPTranslate(const uint8_t *srcPixels, Size &size, uint8_t *dstPixels, float xAxis, float yAxis)
{
    int32_t dstWidth = size.width;
    int32_t dstHeight = size.height;

    const uint8_t *srcUV = srcPixels + getUOffset(size.width, size.height);
    uint8_t *dstUV = dstPixels + getUOffset(dstWidth, dstHeight);

    for (int32_t y = 0; y < size.height; y++) {
        for (int32_t x = 0; x < size.width; x++) {
            int32_t newX = x + xAxis;
            int32_t newY = y + yAxis;
            if (newX >= 0 && newY >= 0 && newX < size.width && newY < size.height) {
                *(dstPixels + newY * dstWidth + newX) = *(srcPixels + y * size.width + x);
            }
        }
    }

    for (int32_t y = 0; y < getUVHeight(size.height); y++) {
        for (int32_t x = 0; x < getUStride(size.width); x++) {
            int32_t newX = x + xAxis / NUM_2;
            int32_t newY = y + yAxis / NUM_2;
            if (newX >= 0 && newX < size.width / NUM_2 && newY >= 0 && newY < size.height / NUM_2) {
                *(dstUV + (newY * dstWidth + newX * NUM_2)) = *(srcUV + (y * size.width + x * NUM_2));
                *(dstUV + (newY * dstWidth + newX * NUM_2) + 1) = *(srcUV + (y * size.width + x * NUM_2) + 1);
            }
        }
    }
}

void PixelYUV::YUV420PReadPixel(const uint8_t *srcPixels, Size &size, const Position &pos, uint32_t &dst, bool isI420)
{
    const uint8_t *srcU;
    const uint8_t *srcV;

    if (isI420) {
        srcU = srcPixels + getUOffset(size.width, size.height);
        srcV = srcPixels + getVOffset(size.width, size.height);
    } else {
        srcV = srcPixels + getUOffset(size.width, size.height);
        srcU = srcPixels + getVOffset(size.width, size.height);
    }

    uint8_t colorY = *(srcPixels + (pos.y * size.width + pos.x));
    uint8_t colorU = *(srcU + ((pos.y / NUM_2) * getUStride(size.width) + pos.x / NUM_2));
    uint8_t colorV = *(srcV + ((pos.y / NUM_2) * getUStride(size.width) + pos.x / NUM_2));

    uint32_t YUVcolor = (colorY << Y_SHIFT) | (colorU << U_SHIFT) | (colorV << V_SHIFT);
    dst = YUVcolor;
}

void PixelYUV::YUV420SPReadPixel(const uint8_t *srcPixels, Size &size, const Position &pos, uint32_t &dst, bool isNV12)
{
    const uint8_t *srcUV = srcPixels + getUOffset(size.width, size.height);
    uint8_t colorY = *(srcPixels + (pos.y * size.width + pos.x));
    uint8_t colorU = 0;
    uint8_t colorV = 0;

    if (isNV12) {
        colorU = *(srcUV + (pos.y / NUM_2) * getUStride(size.width) + pos.x / NUM_2);
        colorV = *(srcUV + (pos.y / NUM_2) * getUStride(size.width) + pos.x / NUM_2 + 1);
    } else {
        colorV = *(srcUV + (pos.y / NUM_2) * getUStride(size.width) + pos.x / NUM_2);
        colorU = *(srcUV + (pos.y / NUM_2) * getUStride(size.width) + pos.x / NUM_2 + 1);
    }

    uint32_t YUVcolor = (colorY << Y_SHIFT) | (colorU << U_SHIFT) | (colorV << V_SHIFT);
    dst = YUVcolor;
}

void PixelYUV::YUV420PWritePixel(uint8_t *srcPixels, Size &size, const Position &pos,
                                 const uint32_t &color, bool isI420)
{
    uint8_t *srcU;
    uint8_t *srcV;

    uint8_t colorY = (color >> Y_SHIFT) & YUV420_MASK;
    uint8_t colorU = (color >> U_SHIFT) & YUV420_MASK;
    uint8_t colorV = (color >> V_SHIFT) & YUV420_MASK;

    if (isI420) {
        srcU = srcPixels + getUOffset(size.width, size.height);
        srcV = srcPixels + getVOffset(size.width, size.height);
    } else {
        srcV = srcPixels + getUOffset(size.width, size.height);
        srcU = srcPixels + getVOffset(size.width, size.height);
    }

    *(srcPixels + (pos.y * size.width + pos.x)) = colorY;
    *(srcU + ((pos.y / NUM_2) * getUStride(size.width) + pos.x / NUM_2)) = colorU;
    *(srcV + ((pos.y / NUM_2) * getUStride(size.width) + pos.x / NUM_2)) = colorV;
}

void PixelYUV::YUV420SPWritePixel(uint8_t *srcPixels, Size &size, const Position &pos,
                                  const uint32_t &color, bool isNV12)
{
    uint8_t *srcUV = srcPixels + size.width * size.height;
    uint8_t colorY = (color >> Y_SHIFT) & YUV420_MASK;
    uint8_t colorU = (color >> U_SHIFT) & YUV420_MASK;
    uint8_t colorV = (color >> V_SHIFT) & YUV420_MASK;

    *(srcPixels + (pos.y * size.width + pos.x)) = colorY;

    if (isNV12) {
        *(srcUV + (pos.y / NUM_2) * getUStride(size.width) + pos.x / NUM_2) = colorU;
        *(srcUV + (pos.y / NUM_2) * getUStride(size.width) + pos.x / NUM_2 + 1) = colorV;
    } else {
        *(srcUV + (pos.y / NUM_2) * getUStride(size.width) + pos.x / NUM_2) = colorV;
        *(srcUV + (pos.y / NUM_2) * getUStride(size.width) + pos.x / NUM_2 + 1) = colorU;
    }
}

void PixelYUV::YUV420PWritePixels(Size &size, uint8_t *srcPixels, const uint32_t &color, bool isI420)
{
    uint8_t colorY = (color >> Y_SHIFT) & YUV420_MASK;
    uint8_t colorU = (color >> U_SHIFT) & YUV420_MASK;
    uint8_t colorV = (color >> V_SHIFT) & YUV420_MASK;

    uint8_t *srcU;
    uint8_t *srcV;
    if (isI420) {
        srcU = srcPixels + getUOffset(size.width, size.height);
        srcV = srcPixels + getVOffset(size.width, size.height);
    } else {
        srcV = srcPixels + getUOffset(size.width, size.height);
        srcU = srcPixels + getVOffset(size.width, size.height);
    }

    for (int32_t i = 0; i < size.height; i++) {
        for (int32_t j = 0; j < size.width; j++) {
            *(srcPixels + i * size.width + j) = colorY;
        }
    }
    for (int32_t i = 0; i < getUVHeight(size.height); i++) {
        for (int32_t j = 0; j < getUStride(size.width); j++) {
            *(srcU + i * getUStride(size.width) + j) = colorU;
            *(srcV + i * getUStride(size.width) + j) = colorV;
        }
    }
}

void PixelYUV::YUV420SPWritePixels(Size &size, uint8_t *srcPixels, const uint32_t &color, bool isNV12)
{
    uint8_t colorY = (color >> Y_SHIFT) & YUV420_MASK;
    uint8_t colorU = (color >> U_SHIFT) & YUV420_MASK;
    uint8_t colorV = (color >> V_SHIFT) & YUV420_MASK;

    uint8_t *srcUV = srcPixels + getUOffset(size.width, size.height);

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

void PixelYUV::WriteDataI420Convert(const uint8_t *srcPixels, const Size &size, uint8_t *dstPixels,
                                    Position dstPos, const Size &dstSize)
{
    dstPos.y = getUVStride(dstPos.y);
    const uint8_t *srcU = srcPixels + getUOffset(size.width, size.height);
    const uint8_t *srcV = srcPixels + getVOffset(size.width, size.height);
    uint8_t *dstU = dstPixels + getUOffset(dstSize.width, dstSize.height);
    uint8_t *dstV = dstPixels + getVOffset(dstSize.width, dstSize.height);
    for (int32_t i = 0; i < size.height; i++) {
        if (memcpy_s(dstPixels + (dstPos.y + i) * dstSize.width + dstPos.x, size.width,
            srcPixels + i * size.width, size.width) != EOK) {
            IMAGE_LOGE("WriteDataI420Convert memcpy yplane failed");
            return;
        }
    }
    for (int32_t i = 0; i < getUVHeight(size.height); i++) {
        if (memcpy_s(dstU + (dstPos.y / NUM_2 + i) * getUStride(dstSize.width) + dstPos.x / NUM_2,
            getUStride(size.width), srcU + i * getUStride(size.width), getUStride(size.width)) != 0 ||
            memcpy_s(dstV + (dstPos.y / NUM_2 + i) * getUStride(dstSize.width) + dstPos.x / NUM_2,
            getUStride(size.width), srcV + i * getUStride(size.width), getUStride(size.width)) != 0) {
            IMAGE_LOGE("WriteDataI420Convert memcpy uplane or vplane failed");
            return;
        }
    }
}

void PixelYUV::WriteDataNV12Convert(uint8_t *srcPixels, const Size &size, uint8_t *dstPixels,
                                    Position dstPos, const Size &dstSize)
{
    dstPos.y = getUVStride(dstPos.y);
    for (int i = 0; i < size.height; i++) {
        if (memcpy_s(dstPixels + (dstPos.y + i) * dstSize.width + dstPos.x, size.width,
            srcPixels + i * size.width, size.width) != 0) {
            IMAGE_LOGE("WriteDataNV12Convert memcpy yplane failed");
            return;
        }
    }
    for (int i = 0; i < getUVHeight(size.height); ++i) {
        if (memcpy_s(dstPixels + ((dstPos.y) / NUM_2 + i) * getUVStride(dstSize.width) + dstPos.x +
            getUOffset(dstSize.width, dstSize.height), getUVStride(size.width),
            srcPixels + getUOffset(size.width, size.height) + i * getUVStride(size.width),
            getUVStride(size.width)) != 0) {
            IMAGE_LOGE("WriteDataNV12Convert memcpy uplane or vplane failed");
            return;
        }
    }
}

uint32_t PixelYUV::GetImageSize(int32_t width, int32_t height)
{
    return width * height + ((width + 1) / NUM_2) * ((height + 1) / NUM_2) * NUM_2;
}

void PixelYUV::assignYUVDataOnType(PixelFormat format, int32_t width, int32_t height)
{
    if (format == PixelFormat::YU12 || format == PixelFormat::YV12) {
        yuvDataInfo_.y_width = width;
        yuvDataInfo_.y_height = height;
        yuvDataInfo_.y_stride = width;
        yuvDataInfo_.u_stride = getUStride(width);
        yuvDataInfo_.v_stride = getUStride(width);
    }
    if (format == PixelFormat::NV12 || format == PixelFormat::NV21) {
        yuvDataInfo_.y_width = width;
        yuvDataInfo_.y_height = height;
        yuvDataInfo_.y_stride = width;
        yuvDataInfo_.uv_width = getUVStride(width);
        yuvDataInfo_.uv_height = getUVHeight(height);
        yuvDataInfo_.uv_stride = getUVStride(width);
    }
}

bool PixelYUV::isYUVImage(PixelFormat format)
{
    if (format == PixelFormat::NV21 || format == PixelFormat::NV12 ||
        format == PixelFormat::YU12 || format == PixelFormat::YV12) {
        return true;
    }
    return false;
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

uint32_t PixelYUV::ApplyColorSpace(const OHOS::ColorManager::ColorSpace &grColorSpace)
{
    auto grName = grColorSpace.GetColorSpaceName();
    if (grColorSpace_ != nullptr && isSameColorSpace(*grColorSpace_, grColorSpace)) {
        if (grColorSpace_->GetColorSpaceName() != grName) {
            InnerSetColorSpace(grColorSpace);
        }
        return SUCCESS;
    }
    ImageInfo imageInfo;
    GetImageInfo(imageInfo);
    // Build sk source infomation
    PixelFormat format = imageInfo.pixelFormat;
    imageInfo.pixelFormat = PixelFormat::BGRA_8888;
    SkTransInfo src;
    src.info = ToSkImageInfo(imageInfo, ToSkColorSpace(this));
    uint64_t rowStride = src.info.minRowBytes();
    uint8_t *srcData = data_;
#if !defined(_WIN32) && !defined(_APPLE) && !defined(IOS_PLATFORM) && !defined(A_PLATFORM)
    if (GetAllocatorType() == AllocatorType::DMA_ALLOC && GetFd() != nullptr) {
        SurfaceBuffer *sbBuffer = reinterpret_cast<SurfaceBuffer *>(GetFd());
        rowStride = sbBuffer->GetStride();
    }
    srcData = static_cast<uint8_t *>(GetWritablePixels());
#endif

    int32_t width = imageInfo.size.width;
    int32_t height = imageInfo.size.height;
    uint8_t *RGBAdata = static_cast<uint8_t *>(malloc(width * height * NUM_4));
    YUV420ToBGRA(srcData, RGBAdata, width, height, format);
    src.bitmap.installPixels(src.info, RGBAdata, rowStride);
    // Build sk target infomation
    SkTransInfo dst;
    dst.info = ToSkImageInfo(imageInfo, grColorSpace.ToSkColorSpace());
    MemoryData memoryData = {nullptr, width * height * NUM_4, "ApplyColorSpace ImageData",
        {dst.info.width(), dst.info.height()}};
    auto m = MemoryManager::CreateMemory(allocatorType_, memoryData);
    if (m == nullptr) {
        IMAGE_LOGE("applyColorSpace CreateMemory failed");
        return ERR_IMAGE_COLOR_CONVERT;
    }
    // Transfor pixels by readPixels
    if (!src.bitmap.readPixels(dst.info, m->data.data, rowStride, 0, 0)) {
        m->Release();
        IMAGE_LOGE("ReadPixels failed");
        return ERR_IMAGE_COLOR_CONVERT;
    }

    int32_t dstWidth = dst.info.width();
    int32_t dstHeight = dst.info.height();
    uint32_t pictureSize = GetImageSize(dstWidth, dstHeight);
    MemoryData memoryYUVData = {nullptr, pictureSize, "Trans ImageData", {dstWidth, dstHeight}};
    auto YUVMemory = MemoryManager::CreateMemory(allocatorType_, memoryYUVData);
    if (YUVMemory == nullptr) {
        YUVMemory->Release();
        IMAGE_LOGE("applyColorSpace CreateYUVMemory failed");
        return ERR_IMAGE_COLOR_CONVERT;
    }
    BGRAToYUV420(reinterpret_cast<uint8_t *>(m->data.data), reinterpret_cast<uint8_t *>(YUVMemory->data.data),
        dstWidth, dstHeight, format);
    m->Release();
    imageInfo.pixelFormat = format;
    dst.info = ToSkImageInfo(imageInfo, grColorSpace.ToSkColorSpace());
    ToImageInfo(imageInfo, dst.info);
    grColorSpace_ = std::make_shared<OHOS::ColorManager::ColorSpace>(dst.info.refColorSpace(), grName);
    SetPixelsAddr(reinterpret_cast<void *>(YUVMemory->data.data), nullptr, pictureSize, YUVMemory->GetType(), nullptr);
    SetImageInfo(imageInfo, true);
    return SUCCESS;
}
#endif

#ifdef LIBYUV_ENABLE
bool PixelYUV::BGRAToYUV420(const uint8_t *src, uint8_t *dst, int srcW, int srcH, PixelFormat pixelFormat)
{
    uint32_t pictureSize = GetImageSize(srcW, srcH);
    uint8_t *temp = static_cast<uint8_t *>(malloc(pictureSize));
    libyuv::ARGBToI420(src, srcW * NUM_4, temp, srcW, temp + getUOffset(srcW, srcH), getUStride(srcW),
                      temp + getVOffset(srcW, srcH), getUStride(srcW), srcW, srcH);
    int32_t r = 0;
    switch (pixelFormat) {
        case PixelFormat::NV12:
            r = libyuv::I420ToNV12(temp, srcW, temp + getUOffset(srcW, srcH), getUStride(srcW),
                    temp + getVOffset(srcW, srcH), getUStride(srcW), dst, srcW,
                    dst + getUOffset(srcW, srcH), getUVStride(srcW), srcW, srcH);
            break;
        case PixelFormat::NV21:
            r = libyuv::I420ToNV21(temp, srcW, temp + getUOffset(srcW, srcH), getUStride(srcW),
                    temp + getVOffset(srcW, srcH), getUStride(srcW), dst, srcW,
                    dst + getUOffset(srcW, srcH), getUVStride(srcW), srcW, srcH);
            break;
        default:
            break;
    }
    free(temp);
    return r == 0;
}

bool PixelYUV::YUV420ToBGRA(const uint8_t *sample, uint8_t *dst_argb, int width, int height, PixelFormat pixelFormat)
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

bool PixelYUV::YUV420ToARGB(const uint8_t *sample, uint8_t *dst_argb, int width, int height, PixelFormat pixelFormat)
{
    uint8_t *temp = static_cast<uint8_t *>(malloc(width * height * NUM_4));
    int32_t r = YUV420ToBGRA(sample, temp, width, height, pixelFormat);
    if (r != 0) {
        return false;
    }
    return 0 == libyuv::ARGBToBGRA(temp, width * NUM_4, dst_argb, width * NUM_4, width, height);
}

#else

bool PixelYUV::I420ToNV12(const uint8_t *srcPixels, uint8_t *dstPixels, int32_t width, int32_t height)
{
    int32_t halfWidth = getUStride(width);
    int32_t halfHeight = getUVHeight(height);
    const uint8_t *srcV = srcPixels + getVOffset(width, height);
    const uint8_t *srcU = srcPixels + getUOffset(width, height);
    uint8_t *dstUV = dstPixels + getUOffset(width, height);
    int32_t dstStrideUV = getUVStride(width);
    // Negative height means invert the image.
    if (memcpy_s(dstPixels, width * height, srcPixels, width * height) != 0) {
        IMAGE_LOGE("I420ToNV12 memcpy failed");
        return false;
    }
    MergeUVPlane(srcU, halfWidth, srcV, halfWidth, dstUV, dstStrideUV, halfWidth, halfHeight);
    return true;
}

bool PixelYUV::NV12ToI420(const uint8_t *srcPixels, const Size &size, uint8_t *dstPixels)
{
    if (srcPixels == nullptr || dstPixels == nullptr || size.width <= 0 || size.height <= 0) {
        IMAGE_LOGE("NV12ToI420 param error!");
        return false;
    }
    const uint8_t *srcUV = srcPixels + getUOffset(size.width, size.height);
    uint8_t *destU = dstPixels + getUOffset(size.width, size.height);
    uint8_t *destV = dstPixels + getVOffset(size.width, size.height);
    if (memcpy_s(dstPixels, size.width * size.height, srcPixels, size.width * size.height) != 0) {
        IMAGE_LOGE("NV12ToI420 memcpy failed");
        return false;
    }
    for (int32_t i = 0; i < getUStride(size.width) * getUVHeight(size.height); i++) {
        destU[i] = srcUV[i * NUM_2];
        destV[i] = srcUV[i * NUM_2 + 1];
    }
    return true;
}

bool PixelYUV::I420ToNV21(const uint8_t *srcPixels, uint8_t *dstPixels, int32_t width, int32_t height)
{
    int32_t halfWidth = getUStride(width);
    int32_t halfHeight = getUVHeight(height);
    const uint8_t *srcU = srcPixels + getVOffset(width, height);
    const uint8_t *srcV = srcPixels + getUOffset(width, height);
    uint8_t *dstUV = dstPixels + getUOffset(width, height);
    int32_t dstStrideUV = getUVStride(width);
    if (memcpy_s(dstPixels, width * height, srcPixels, width * height) != 0) {
        IMAGE_LOGE("I420ToNV21 memcpy failed");
        return false;
    }
    MergeUVPlane(srcU, halfWidth, srcV, halfWidth, dstUV, dstStrideUV,
        halfWidth, halfHeight);
    return true;
}

static bool YUVRotateConvert(Size &size, int32_t degrees, int32_t &dstWidth, int32_t &dstHeight)
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

bool PixelYUV::YV12Rotate(const uint8_t *srcPixels, Size &size, uint8_t *dstPixels, int32_t degrees)
{
    int32_t dstWidth = size.width;
    int32_t dstHeight = size.height;
    if (degrees < 0) {
        degrees += degrees360;
    }
    if (YUVRotateConvert(size, degrees, dstWidth, dstHeight)) {
        IMAGE_LOGI("Rotate degress is 0, don't need rotate");
        return true;
    }
    const uint8_t *srcU = srcPixels + getUOffset(size.width, size.height);
    const uint8_t *srcV = srcPixels + getVOffset(size.width, size.height);
    int32_t srcStrideU = getUStride(size.width);
    int32_t srcStrideV = getUStride(size.width);
    uint8_t *dstU = dstPixels + getUOffset(dstWidth, dstHeight);
    uint8_t *dstV = dstPixels + getVOffset(dstWidth, dstHeight);
    int32_t dstStrideU = getUStride(dstWidth);
    int32_t dstStrideV = getUStride(dstWidth);
    Size halfSize = {getUStride(size.width), getUVHeight(size.height)};
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

void PixelYUV::MergeUVPlane(const uint8_t *srcU, int32_t srcStrideU, const uint8_t *srcV, int32_t srcStrideV,
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

void PixelYUV::MergeUV(const uint8_t *srcU, const uint8_t *srcV, uint8_t *dstUV, int32_t width)
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

void PixelYUV::RotatePlane270(const uint8_t *src, int32_t srcStride, uint8_t *dst, int32_t dstStride,
    const Size &half)
{
    dst += dstStride * (half.width - 1);
    dstStride = -dstStride;
    TransposePlane(src, srcStride, dst, dstStride, half);
}

void PixelYUV::RotatePlane180(const uint8_t *src, int32_t srcStride, uint8_t *dst, int32_t dstStride,
    const Size &half)
{
    // Swap top and bottom row and mirror the content. Uses a temporary row.
    uint8_t *row = static_cast<uint8_t *>(malloc(half.width));
    const uint8_t *src_bot = src + srcStride * (half.height - 1);
    uint8_t *dst_bot = dst + dstStride * (half.height - 1);
    int32_t half_height = getUVHeight(half.height);
    for (int32_t y = 0; y < half_height; ++y) {
        if (memcpy_s(row, half.width, src, half.width) != 0) {
            IMAGE_LOGE("RotatePlane180 memcpy failed.");
            return;
        }
        MirrorRow(src_bot, dst, half.width);
        MirrorRow(row, dst_bot, half.width);
        src += (y + 1) * srcStride;
        dst += (y + 1) * dstStride;
        src_bot -= (y + 1) * srcStride;
        dst_bot -= (y + 1) * dstStride;
    }
    free(row);
}

void PixelYUV::RotatePlane90(const uint8_t *src, int32_t srcStride, uint8_t *dst, int32_t dstStride, const Size &half)
{
    src += srcStride * (half.height - 1);
    srcStride = -srcStride;
    TransposePlane(src, srcStride, dst, dstStride, half);
}

void PixelYUV::TransposePlane(const uint8_t *src, int32_t srcStride, uint8_t *dst, int32_t dstStride,
    const Size &half)
{
    for (int32_t y = 0; y < half.height; ++y) {
        for (int32_t x = 0; x < half.width; ++x) {
            dst[x * dstStride + y] = src[y * srcStride + x];
        }
    }
}

void PixelYUV::MirrorRow(const uint8_t *src, uint8_t *dst, int32_t width)
{
    src += width - 1;
    for (int x = 0; x < width - 1; x += NUM_2) {
        dst[x] = src[-x];
        dst[x + 1] = src[-(x + 1)];
    }
    if (width & 1) {
        dst[width - 1] = src[0];
    }
}

bool PixelYUV::BGRAToYUV420(const uint8_t *src, uint8_t *dst, int32_t width, int32_t height, PixelFormat pixelFormat)
{
    uint32_t pictureSize = GetImageSize(width, height);
    if (pictureSize == 0 || pictureSize > PIXEL_YUV_MAX_RAM_SIZE) {
        IMAGE_LOGE("BGRAToYUV420 parameter pictureSize:[%{public}d] error.", pictureSize);
        return false;
    }
    uint8_t *temp = static_cast<uint8_t *>(malloc(pictureSize));
    bool result = BGRAToI420(src, temp, width, height);
    if (!result) {
        return false;
    }
    switch (pixelFormat) {
        case PixelFormat::NV12:
            I420ToNV12(temp, dst, width, height);
            break;
        case PixelFormat::NV21:
            I420ToNV21(temp, dst, width, height);
            break;
        case PixelFormat::YU12:
            if (memcpy_s(dst, pictureSize, temp, pictureSize) != 0) {
                IMAGE_LOGE("BGRAToYUV420 to YU12 memcpy failed.");
                return false;
            }
            free(temp);
            return true;
        case PixelFormat::YV12:
            I420AndYV12Transfers(temp, dst, width, height);
            break;
        default:
            break;
    }
    free(temp);
    return true;
}

bool PixelYUV::YUV420ToBGRA(const uint8_t *in, uint8_t *out, int32_t width, int32_t height, PixelFormat pixelFormat)
{
    if (!in || !out || width <= 0 || height == 0) {
        return false;
    }
    for (int32_t i = 0; i < height; i++) {
        for (int32_t j = 0; j < width; j++) {
            uint8_t Y = GetYUV420Y(j, i, width, in);
            uint8_t U = GetYUV420U(j, i, width, height, pixelFormat, in);
            uint8_t V = GetYUV420V(j, i, width, height, pixelFormat, in);

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

bool PixelYUV::YUV420ToARGB(const uint8_t *in, uint8_t *out, int32_t width, int32_t height, PixelFormat pixelFormat)
{
    if (!in || !out || width <= 0 || height == 0) {
        return false;
    }
    for (int32_t i = 0; i < height; i++) {
        for (int32_t j = 0; j < width; j++) {
            uint8_t Y = GetYUV420Y(j, i, width, in);
            uint8_t U = GetYUV420U(j, i, width, height, pixelFormat, in);
            uint8_t V = GetYUV420V(j, i, width, height, pixelFormat, in);

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

int32_t PixelYUV::I420Copy(const uint8_t *srcY, int32_t srcStrideY, const uint8_t *srcU, int32_t srcStrideU,
    const uint8_t *srcV, int32_t srcStrideV, uint8_t *dstY, int32_t dstStrideY, uint8_t *dstU,
    int32_t dstStrideU, uint8_t *dstV, int32_t dstStrideV, int32_t width, int32_t height)
{
    int32_t halfwidth = getUStride(width);
    int32_t halfheight = getUVHeight(height);
    if ((!srcY && dstY) || !srcU || !srcV || !dstU || !dstV || width <= 0 || height == 0) {
        return ERR_IMAGE_INVALID_PARAMETER;
    }
    // Negative height means invert the image.
    if (height < 0) {
        height = -height;
        halfheight = getUVHeight(height);
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

void PixelYUV::translateRGBToYUV(const uint8_t *src, uint8_t *dst, int32_t width, bool isUpos, size_t i, size_t pos)
{
    for (int x = 0; x < width; x += NUM_2) {
        uint8_t r = src[NUM_4 * i];
        uint8_t g = src[NUM_4 * i + 1];
        uint8_t b = src[NUM_4 * i + NUM_2];

        uint8_t y = dst[i++] = INT_16 + INT_219 * (RGB_TO_YUV_PARAM_1 * r + RGB_TO_YUV_PARAM_2 * g +
            RGB_TO_YUV_PARAM_3 * b) / INT_255;

        if (isUpos) {
            dst[pos++] = INT_128 + INT_224 * (RGB_TO_YUV_PARAM_4 * (b - y) / RGB_TO_YUV_PARAM_5) / INT_255;
        } else {
            dst[pos++] = INT_128 + INT_224 * (RGB_TO_YUV_PARAM_4 * (r - y) / RGB_TO_YUV_PARAM_6) / INT_255;
        }

        b = src[NUM_4 * i];
        g = src[NUM_4 * i + 1];
        r = src[NUM_4 * i + NUM_2];
        if ((width & 1) && x == width - 1) {
            // nothing to do
        } else {
            dst[i++] = INT_16 + INT_219 * (RGB_TO_YUV_PARAM_1 * r + RGB_TO_YUV_PARAM_2 * g +
                RGB_TO_YUV_PARAM_3 * b) / INT_255;
        }
    }
}

bool PixelYUV::BGRAToI420(const uint8_t *src, uint8_t *dst, int32_t width, int32_t height)
{
    if (!src || !dst || width <= 0 || height == 0) {
        return false;
    }
    size_t upos = getUOffset(width, height);
    size_t vpos = getVOffset(width, height);

    size_t i = 0;

    for (int32_t line = 0; line < height; ++line) {
        if (!(line % NUM_2)) {
            translateRGBToYUV(src, dst, width, true, i, upos);
        } else {
            translateRGBToYUV(src, dst, width, false, i, vpos);
        }
    }
    return true;
}

void PixelYUV::MirrorPlane(const uint8_t *srcY, int32_t srcStrideY, uint8_t *dstY, int32_t dstStrideY,
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
        srcY += (y + 1) * srcStrideY;
        dstY += (y + 1) * dstStrideY;
    }
}

void PixelYUV::MirrorUVRow(const uint8_t *srcUV, uint8_t *dstUV, int32_t width)
{
    srcUV += (width - 1) << 1;
    dstUV[0] = srcUV[0];
    dstUV[1] = srcUV[1];
    for (int32_t x = 1; x < width; ++x) {
        dstUV[NUM_2 * x] = srcUV[(-NUM_2) * x];
        dstUV[NUM_2 * x + 1] = srcUV[(-NUM_2) * x + 1];
    }
}

void PixelYUV::MirrorUVPlane(const uint8_t *srcUV, int32_t srcStrideUV, uint8_t *dstUV, int32_t dstStrideUV,
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
        srcUV += (y + 1) * srcStrideUV;
        dstUV += (y + 1) * dstStrideUV;
    }
}

int32_t PixelYUV::I420Mirror(const uint8_t *srcY, int32_t srcStrideY, const uint8_t *srcU, int32_t srcStrideU,
                             const uint8_t *srcV, int32_t srcStrideV, uint8_t *dstY, int32_t dstStrideY,
                             uint8_t *dstU, int32_t dstStrideU, uint8_t *dstV, int32_t dstStrideV,
                             int32_t width, int32_t height)
{
    int32_t halfwidth = getUStride(width);
    int32_t halfheight = getUVHeight(height);

    if (!srcY || !srcU || !srcV || !dstU || !dstV || width <= 0 || height == 0) {
        return ERR_IMAGE_INVALID_PARAMETER;
    }

    // Negative height means invert the image.
    if (height < 0) {
        height = -height;
        halfheight = getUVHeight(height);
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

int32_t PixelYUV::NV12Mirror(const uint8_t *srcY, int32_t srcStrideY, const uint8_t *srcUV, int32_t srcStrideUV,
    uint8_t *dstY, int32_t dstStrideY, uint8_t *dstUV, int32_t dstStrideUV, int32_t width, int32_t height)
{
    int32_t halfwidth = getUStride(width);
    int32_t halfheight = getUVHeight(height);

    if (!srcY || !srcUV || !dstUV || width <= 0 || height == 0) {
        return ERR_IMAGE_INVALID_PARAMETER;
    }

    // Negative height means invert the image.
    if (height < 0) {
        height = -height;
        halfheight = getUVHeight(height);
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

int32_t PixelYUV::NV12Copy(const uint8_t *srcY, int32_t srcStrideY, const uint8_t *srcUV, int32_t srcStrideUV,
    uint8_t *dstY, int32_t dstStrideY, uint8_t *dstUV, int32_t dstStrideUV, int32_t width, int32_t height)
{
    int32_t halfwidth = getUStride(width);
    int32_t halfheight = getUVHeight(height);

    if (!srcY || !dstY || !srcUV || !dstUV || width <= 0 || height == 0) {
        return ERR_IMAGE_INVALID_PARAMETER;
    }

    // Negative height means invert the image.
    if (height < 0) {
        height = -height;
        halfheight = getUVHeight(height);
        srcY = srcY + (height - 1) * srcStrideY;
        srcUV = srcUV + (halfheight - 1) * srcStrideUV;
        srcStrideY = -srcStrideY;
        srcStrideUV = -srcStrideUV;
    }
    CopyPlane(srcY, srcStrideY, dstY, dstStrideY, width, height);
    CopyPlane(srcUV, srcStrideUV, dstUV, dstStrideUV, halfwidth * NUM_2, halfheight);
    return SUCCESS;
}

void PixelYUV::CopyPlane(const uint8_t *srcY, int32_t srcStrideY, uint8_t *dstY, int32_t dstStrideY,
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

#endif
} // namespace Media
} // namespace OHOS