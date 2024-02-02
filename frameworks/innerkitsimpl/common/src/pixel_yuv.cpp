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

namespace OHOS {
namespace Media {
using namespace OHOS::HiviewDFX;
using namespace std;

constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, LOG_TAG_DOMAIN_ID_IMAGE, "pixelYUV" };

static const uint8_t NUM_2 = 2;
static const uint8_t NUM_3 = 3;
static const uint8_t NUM_4 = 4;
static const uint8_t NUM_5 = 5;
static const uint8_t NUM_6 = 6;
static const uint8_t NUM_7 = 7;
static const uint8_t NUM_8 = 8;
static const int32_t degrees90 = 90; // 90度角
static const int32_t degrees180 = 180; // 180度角
static const int32_t degrees270 = 270; // 270度角
constexpr int32_t MAX_DIMENSION = INT32_MAX >> NUM_2;
constexpr uint8_t Y_SHIFT = 16;
constexpr uint8_t U_SHIFT = 8;
constexpr uint8_t V_SHIFT = 0;
constexpr uint8_t YUV420_MASK = 0xFF;

pixelYUV::~pixelYUV()
{
    FreePixelMap();
}

void pixelYUV::WriteDataNV12Convert(uint8_t *srcPixels, const Size &size, uint8_t *dstPixels,
                                                const Position &dstPos, const Size &dstSize)
{
    for (int32_t i = 0; i < size.height; i++) {
        memcpy_s(dstPixels + (dstPos.y + i) * dstSize.width + dstPos.x, size.width,
                 srcPixels + i * size.width, size.width);
    }
    for (int32_t i = 0; i < (size.height + 1) / NUM_2; ++i)
    {
        memcpy_s(dstPixels + (dstPos.y + i) * (((dstSize.width + 1) / NUM_2) * NUM_2) + dstPos.x +
                 dstSize.height *dstSize.width, ((size.width + 1) / NUM_2) * NUM_2,
                 srcPixels + size.width * size.height + i * (((size.width + 1) / NUM_2) * NUM_2),
                 ((size.width + 1) / NUM_2) * NUM_2);
    }
}

uint32_t pixelYUV::getPixelBytesNumber()
{
    return imageInfo_.size.width * imageInfo_.size.height +
           ((imageInfo_.size.width + 1) / NUM_2)  * ((imageInfo_.size.height + 1) / NUM_2) * NUM_2;
}

uint32_t pixelYUV::SetAlpha(const float percent)
{
    HiLog::Error(LABEL, "SetAlpha is not support on pixelYUV");
    return ERR_IMAGE_DATA_UNSUPPORT;
}

const uint8_t *pixelYUV::GetPixel8(int32_t x, int32_t y)
{
    HiLog::Error(LABEL, "GetPixel8 is not support on pixelYUV");
    return nullptr;
}

const uint16_t *pixelYUV::GetPixel16(int32_t x, int32_t y)
{
    HiLog::Error(LABEL, "GetPixel16 is not support on pixelYUV");
    return nullptr;
}

const uint32_t *pixelYUV::GetPixel32(int32_t x, int32_t y)
{
    HiLog::Error(LABEL, "GetPixel32 is not support on pixelYUV");
    return nullptr;
}

bool pixelYUV::GetARGB32Color(int32_t x, int32_t y, uint32_t &color)
{
    HiLog::Error(LABEL, "GetARGB32Color is not support on pixelYUV");
    return false;
}

uint8_t pixelYUV::GetARGB32ColorA(uint32_t color)
{
    HiLog::Error(LABEL, "GetARGB32ColorA is not support on pixelYUV");
    return 0;
}

uint8_t pixelYUV::GetARGB32ColorR(uint32_t color)
{
    HiLog::Error(LABEL, "GetARGB32ColorR is not support on pixelYUV");
    return 0;
}

uint8_t pixelYUV::GetARGB32ColorG(uint32_t color)
{
    HiLog::Error(LABEL, "GetARGB32ColorG is not support on pixelYUV");
    return 0;
}

uint8_t pixelYUV::GetARGB32ColorB(uint32_t color)
{
    HiLog::Error(LABEL, "GetARGB32ColorB is not support on pixelYUV");
    return 0;
}


uint32_t pixelYUV::SetRowDataSizeForImageInfo(ImageInfo info)
{
    rowDataSize_ = info.size.width * NUM_3 / NUM_2;
    return SUCCESS;
}

void pixelYUV::rotate(float degrees)
{
    int count = imageInfo_.size.width * imageInfo_.size.height +
                ((imageInfo_.size.width + 1) / NUM_2) * ((imageInfo_.size.height + 1) / NUM_2) * NUM_2;
    uint8_t *dst = (uint8_t *)malloc(count);
    YUVRotate(data_, imageInfo_.size, dst, degrees, imageInfo_.pixelFormat);
    memcpy_s(data_, count, dst, count);
    free(dst);
    return;
}

uint32_t pixelYUV::crop(const Rect &rect)
{
    ImageInfo imageInfo;
    GetImageInfo(imageInfo);
    Size desiredSize = {rect.width, rect.height};
    MemoryData memoryData = {nullptr, rect.width * rect.height +
                             ((rect.width + 1) / NUM_2) * ((rect.height + 1) / NUM_2) * NUM_2,
                             "Trans ImageData", desiredSize};
    auto m = MemoryManager::CreateMemory(allocatorType_, memoryData);
    if (m == nullptr) {
        HiLog::Error(LABEL, "crop CreateMemory failed");
        return ERR_IMAGE_CROP;
    }
#if !defined(IOS_PLATFORM) && !defined(A_PLATFORM)
    if (allocatorType_ == AllocatorType::DMA_ALLOC) {
        if (m->extend.data == nullptr) {
            HiLog::Error(LABEL, "GendstTransInfo get surfacebuffer failed");
        }
    }
#endif
    size_t count = rect.width * rect.height + ((rect.width + 1) / NUM_2) * ((rect.height + 1) / NUM_2) * NUM_2;
    if (!YUVCrop(data_, imageInfo_.size, (uint8_t*)m->data.data, rect, imageInfo.pixelFormat, imageInfo.pixelFormat)) {
        return ERR_IMAGE_CROP;
    }
    imageInfo.size.height = rect.height;
    imageInfo.size.width = rect.width;

    SetPixelsAddr(m->data.data, m->extend.data, m->data.size, m->GetType(), nullptr);
    SetImageInfo(imageInfo, true);
    return SUCCESS;
}

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

static SkImageInfo ToSkImageInfo(ImageInfo &info, sk_sp<SkColorSpace> colorSpace)
{
    SkColorType colorType = ImageTypeConverter::ToSkColorType(info.pixelFormat);
    SkAlphaType alphaType = ImageTypeConverter::ToSkAlphaType(info.alphaType);
    HiLog::Debug(LABEL, "ToSkImageInfo w %{public}d, h %{public}d", info.size.width, info.size.height);
    HiLog::Debug(LABEL,
        "ToSkImageInfo pf %{public}s, at %{public}s, skpf %{public}s, skat %{public}s",
        ImageTypeConverter::ToName(info.pixelFormat).c_str(),
        ImageTypeConverter::ToName(info.alphaType).c_str(),
        ImageTypeConverter::ToName(colorType).c_str(),
        ImageTypeConverter::ToName(alphaType).c_str()
    );
    return SkImageInfo::Make(info.size.width, info.size.height, colorType, alphaType, colorSpace);
}

static void ToImageInfo(ImageInfo &info, SkImageInfo &skInfo, bool sizeOnly = true)
{
    info.size.width = skInfo.width();
    info.size.height = skInfo.height();
    if (!sizeOnly) {
        info.alphaType = ImageTypeConverter::ToAlphaType(skInfo.alphaType());
        info.pixelFormat = ImageTypeConverter::ToPixelFormat(skInfo.colorType());
    }
}

void pixelYUV::scale(float xAxis, float yAxis)
{
    ImageTrace imageTrace("PixelMap scale");
    TransInfos infos;
    infos.matrix.setScale(xAxis, yAxis);
    PixelFormat format = PixelFormat::UNKNOWN;
    if (imageInfo_.pixelFormat == PixelFormat::NV21 || imageInfo_.pixelFormat == PixelFormat::NV12 ||
        imageInfo_.pixelFormat == PixelFormat::YU12 || imageInfo_.pixelFormat == PixelFormat::YV12) {
        uint32_t pictureSize = imageInfo_.size.width * imageInfo_.size.height * NUM_4;
        uint8_t *argbData = (uint8_t *)malloc(pictureSize);
        YUV420ToRGBA(data_, argbData, imageInfo_.size.width, imageInfo_.size.height, imageInfo_.pixelFormat);
        format = imageInfo_.pixelFormat;
        imageInfo_.pixelFormat = PixelFormat::RGBA_8888;
        int32_t allocType = static_cast<int32_t>(AllocatorType::DEFAULT);
        SetPixelsAddr(argbData, nullptr, pictureSize, static_cast<AllocatorType>(allocType), nullptr);
    }
    if (!DoTranslation(infos)) {
        HiLog::Error(LABEL, "scale falied");
    }
     if (format != PixelFormat::UNKNOWN) {
        int width = imageInfo_.size.width;
        int height = imageInfo_.size.height;
        uint32_t pictureSize = width * height + 2 *((width + 1) / NUM_2) * ((height + 1) / NUM_2);
        uint8_t *YUVData = (uint8_t *)malloc(pictureSize);
        RGBAToYUV420(data_, YUVData, width, height, format);
        int32_t allocType = static_cast<int32_t>(AllocatorType::DEFAULT);
        SetPixelsAddr(YUVData, nullptr, pictureSize, static_cast<AllocatorType>(allocType), nullptr);
        imageInfo_.pixelFormat= format;
    }
}

void pixelYUV::scale(float xAxis, float yAxis, const AntiAliasingOption &option)
{
    ImageTrace imageTrace("PixelMap scale");
    TransInfos infos;
    infos.matrix.setScale(xAxis, yAxis);
    PixelFormat format = PixelFormat::UNKNOWN;
    if (imageInfo_.pixelFormat == PixelFormat::NV21 || imageInfo_.pixelFormat == PixelFormat::NV12 ||
        imageInfo_.pixelFormat == PixelFormat::YU12 || imageInfo_.pixelFormat == PixelFormat::YV12) {
        uint32_t pictureSize = imageInfo_.size.width * imageInfo_.size.height * NUM_4;
        uint8_t *argbData = (uint8_t *)malloc(pictureSize);
        YUV420ToRGBA(data_, argbData, imageInfo_.size.width, imageInfo_.size.height, imageInfo_.pixelFormat);
        format = imageInfo_.pixelFormat;
        imageInfo_.pixelFormat = PixelFormat::RGBA_8888;
        int32_t allocType = static_cast<int32_t>(AllocatorType::DEFAULT);
        SetPixelsAddr(argbData, nullptr, pictureSize, static_cast<AllocatorType>(allocType), nullptr);
    }
    if (!DoTranslation(infos, option)) {
        HiLog::Error(LABEL, "scale falied");
    }
    if (format != PixelFormat::UNKNOWN) {
        int width = imageInfo_.size.width;
        int height = imageInfo_.size.height;
        uint32_t pictureSize = width * height + 2 *((width + 1) / NUM_2) * ((height + 1) / NUM_2);
        uint8_t *YUVData = (uint8_t *)malloc(pictureSize);
        RGBAToYUV420(data_, YUVData, width, height, format);
        int32_t allocType = static_cast<int32_t>(AllocatorType::DEFAULT);
        SetPixelsAddr(YUVData, nullptr, pictureSize, static_cast<AllocatorType>(allocType), nullptr);
        imageInfo_.pixelFormat= format;
    }
}

bool pixelYUV::resize(float xAxis, float yAxis)
{
    ImageTrace imageTrace("PixelMap resize");
    TransInfos infos;
    infos.matrix.setScale(xAxis, yAxis);
    PixelFormat format = PixelFormat::UNKNOWN;
    if (imageInfo_.pixelFormat == PixelFormat::NV21 || imageInfo_.pixelFormat == PixelFormat::NV12 ||
        imageInfo_.pixelFormat == PixelFormat::YU12 || imageInfo_.pixelFormat == PixelFormat::YV12) {
        uint32_t pictureSize = imageInfo_.size.width * imageInfo_.size.height * NUM_4;
        uint8_t *argbData = (uint8_t *)malloc(pictureSize);
        YUV420ToRGBA(data_, argbData, imageInfo_.size.width, imageInfo_.size.height, imageInfo_.pixelFormat);
        format = imageInfo_.pixelFormat;
        imageInfo_.pixelFormat = PixelFormat::RGBA_8888;
        int32_t allocType = static_cast<int32_t>(AllocatorType::DEFAULT);
        SetPixelsAddr(argbData, nullptr, pictureSize, static_cast<AllocatorType>(allocType), nullptr);
    }
    if (!DoTranslation(infos)) {
        HiLog::Error(LABEL, "resize falied");
        return false;
    }
    if (format != PixelFormat::UNKNOWN) {
        if (data_ == nullptr)
        {
            return false;
        }
        int width = imageInfo_.size.width;
        int height = imageInfo_.size.height;
        uint32_t pictureSize = width * height + 2 *((width + 1) / NUM_2) * ((height + 1) / NUM_2);
        uint8_t *YUVData = (uint8_t *)malloc(pictureSize);
        RGBAToYUV420(data_, YUVData, width, height, format);
        int32_t allocType = static_cast<int32_t>(AllocatorType::DEFAULT);
        SetPixelsAddr(YUVData, nullptr, pictureSize, static_cast<AllocatorType>(allocType), nullptr);
        imageInfo_.pixelFormat= format;
    }
    return true;
}

void pixelYUV::flip(bool xAxis, bool yAxis)
{
    ImageInfo imageInfo;
    GetImageInfo(imageInfo);

    int32_t srcW = imageInfo_.size.width;
    int32_t srcH = imageInfo_.size.height;
    PixelFormat format = imageInfo_.pixelFormat;
    const uint8_t *src = data_;
    if (xAxis) {
        uint32_t pictureSize = srcW * srcH + NUM_2 *((srcW + 1) / NUM_2) * ((srcH + 1) / NUM_2);
        uint8_t *dst = (uint8_t *)malloc(pictureSize);
        switch (format) {
        case PixelFormat::YU12:
        case PixelFormat::YV12:
            I420Copy(src, srcW, src + srcW * srcH, (srcW + 1) / NUM_2,
                src + srcW * srcH + ((srcW + 1) / NUM_2) * ((srcH + 1) / NUM_2),
                (srcW + 1) / NUM_2, dst, srcW, dst + srcW * srcH, (srcW + 1) / NUM_2,
                dst + srcW * srcH + ((srcW + 1) / NUM_2) * ((srcH + 1) / NUM_2),
                (srcW + 1) / NUM_2, srcW, -srcH);
            break;
        case PixelFormat::NV12:
        case PixelFormat::NV21:
            NV12Copy(src, srcW, src + srcW * srcH, (srcW + 1) / NUM_2 * NUM_2,
                        dst, srcW, dst + srcW * srcH, (srcW + 1) / NUM_2 * NUM_2, srcW, -srcH);
            break;
        default:
            break;
        }
        memcpy_s(data_, pictureSize, dst, pictureSize);
        free(dst);
    }
    if (yAxis) {
        uint32_t pictureSize = srcW * srcH + 2 *((srcW + 1) / NUM_2) * ((srcH + 1) / NUM_2);
        uint8_t *dst = (uint8_t *)malloc(pictureSize);
        switch (format) {
        case PixelFormat::YU12:
        case PixelFormat::YV12:
            I420Mirror(
                src, srcW,
                src + srcW * srcH, (srcW + 1) / NUM_2,
                src + srcW * srcH + ((srcW + 1) / NUM_2) * ((srcH + 1) / NUM_2), (srcW + 1) / NUM_2,
                dst, srcW,
                dst + srcW * srcH, (srcW + 1) / NUM_2,
                dst + srcW * srcH + ((srcW + 1) / NUM_2) * ((srcH + 1) / NUM_2), (srcW + 1) / NUM_2,
                srcW, srcH);
            break;
        case PixelFormat::NV12:
        case PixelFormat::NV21:
            NV12Mirror(src, srcW, src + srcW * srcH, (srcW + 1) / NUM_2 * NUM_2, dst, srcW,
                        dst + srcW * srcH, (srcW + 1) / NUM_2 * NUM_2, srcW, srcH);
            break;
        default:
            break;
        }
        memcpy_s(data_, pictureSize, dst, pictureSize);
        free(dst);
    }
}

void pixelYUV::SetPixelsAddr(void *addr, void *context, uint32_t size, AllocatorType type, CustomFreePixelMap func)
{
    if (data_ != nullptr) {
        HiLog::Error(LABEL,"SetPixelsAddr release the existed data first");
        FreePixelMap();
    }
    if (type == AllocatorType::SHARE_MEM_ALLOC && context == nullptr) {
        HiLog::Error(LABEL,"SetPixelsAddr error type %{public}d ", type);
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

uint32_t pixelYUV::ReadPixels(const uint64_t &bufferSize, const uint32_t &offset, const uint32_t &stride,
                              const Rect &region, uint8_t *dst)
{
    if (!CheckPixelsInput(dst, bufferSize, offset,  region)) {
        HiLog::Error(LABEL, "read pixels by rect input parameter fail.");
        return ERR_IMAGE_INVALID_PARAMETER;
    }
    
    if (data_ == nullptr) {
        HiLog::Error(LABEL, "read pixels by rect this pixel data is null.");
        return ERR_IMAGE_READ_PIXELMAP_FAILED;
    }
    ImageInfo dstImageInfo;
    dstImageInfo.size.width = region.width;
    dstImageInfo.size.height = region.height;
    dstImageInfo.pixelFormat = imageInfo_.pixelFormat;

    Position srcPosition { region.left, region.top };
    if (!ReadPixelsConvert((void*)data_, srcPosition,  imageInfo_, dst + offset, dstImageInfo)) {
        HiLog::Error(LABEL, "read pixels by rect call ReadPixelsConvert fail.");
        return ERR_IMAGE_READ_PIXELMAP_FAILED;
    }
    return SUCCESS;
}

uint32_t pixelYUV::WritePixels(const uint8_t *source, const uint64_t &bufferSize, const uint32_t &offset,
                               const uint32_t &stride, const Rect &region)
{
    if (!CheckPixelsInput(source, bufferSize, offset, region)) {
        HiLog::Error(LABEL, "write pixel by rect input parameter fail.");
        return ERR_IMAGE_INVALID_PARAMETER;
    }
    if (!IsEditable()) {
        HiLog::Error(LABEL, "write pixel by rect pixelYUV data is not editable.");
        return ERR_IMAGE_PIXELMAP_NOT_ALLOW_MODIFY;
    }
    if (!ImageUtils::IsValidImageInfo(imageInfo_)) {
        HiLog::Error(LABEL, "write pixel by rect current pixelYUV image info is invalid.");
        return ERR_IMAGE_WRITE_PIXELMAP_FAILED;
    }
    if (data_ == nullptr) {
        HiLog::Error(LABEL, "write pixel by rect current pixel map data is null.");
        return ERR_IMAGE_WRITE_PIXELMAP_FAILED;
    }
    uint32_t bytesPerPixel = ImageUtils::GetPixelBytes(imageInfo_.pixelFormat);
    if (bytesPerPixel == 0) {
        HiLog::Error(LABEL, "write pixel by rect get bytes by per pixel fail.");
        return ERR_IMAGE_WRITE_PIXELMAP_FAILED;
    }
    Position dstPosition { region.left, region.top };
    ImageInfo srcInfo;
    srcInfo.size.height = region.height;
    srcInfo.size.width = region.width;
    srcInfo.pixelFormat = imageInfo_.pixelFormat;
    if (!WritePixelsConvert(source + offset,  srcInfo, data_, dstPosition, 
        imageInfo_)) {
        HiLog::Error(LABEL, "write pixel by rect call WritePixelsConvert fail.");
        return ERR_IMAGE_WRITE_PIXELMAP_FAILED;
    }
    return SUCCESS;
}

bool pixelYUV::WritePixelsConvert(const void *srcPixels, const ImageInfo &srcInfo,
                                  void *dstPixels, const Position &dstPos, const ImageInfo &dstInfo)
{
        // basic valid check, other parameters valid check in writePixels method
    if (srcPixels == nullptr || dstPixels == nullptr) {
        HiLog::Error(LABEL, "src or dst pixels invalid.");
        return false;
    }
    uint8_t *src = nullptr;
    uint8_t *dst = nullptr;
    switch (dstInfo.pixelFormat) {
    case PixelFormat::YU12:
        WriteDataI420Convert(src, srcInfo.size, static_cast<uint8_t*>(dstPixels), dstPos, dstInfo.size);
        return true;
    case PixelFormat::YV12:
        WriteDataI420Convert(src, srcInfo.size, static_cast<uint8_t*>(dstPixels), dstPos, dstInfo.size);
        return true;
    case PixelFormat::NV21:
        WriteDataNV12Convert(src, srcInfo.size, static_cast<uint8_t*>(dstPixels), dstPos, dstInfo.size);
        return true;
    case PixelFormat::NV12:
        WriteDataNV12Convert(src, srcInfo.size, static_cast<uint8_t*>(dstPixels), dstPos, dstInfo.size);
        return true;
    default:
        return false;
    }
}

bool pixelYUV::ToI420(const uint8_t *src, uint8_t **dst, const int &width, const int &height, const PixelFormat &format)
{
    size_t count = 0;
    switch (format) {
    case PixelFormat::BGRA_8888:
        count = width * height * NUM_4;
        *dst = nullptr;
        //BGRA8888ToI420(src, width, height, dst);
        return true;
    case PixelFormat::YV12:
        count = width * height + ((width + 1) / NUM_2) * ((height + 1) / NUM_2) * NUM_2;
        *dst = (uint8_t *)malloc(count);
        I420AndYV12Transfers(src, *dst, width, height);
        return true;
    case PixelFormat::YU12:
        count = width * height + ((width + 1) / NUM_2) * ((height + 1) / NUM_2) * NUM_2;
        *dst = (uint8_t *)malloc(count);
        memcpy_s(*dst, count, src, count);
        return true;
    case PixelFormat::NV21:
        count = width * height + ((width + 1) / NUM_2) * ((height + 1) / NUM_2) * NUM_2;
        *dst = (uint8_t *)malloc(count);
        NV21ToI420(src, *dst, width, height);
        return true;
    case PixelFormat::NV12:
        count = width * height + ((width + 1) / NUM_2) * ((height + 1) / NUM_2) * NUM_2;
        *dst = (uint8_t *)malloc(count);
        NV12ToI420(src, *dst, width, height);
        return true;
    default:
        return false;
    }
}

bool pixelYUV::ReadPixelsConvert(const void *srcPixels, const Position &srcPos,
                                            const ImageInfo &srcInfo, void *dstPixels,
                                            const ImageInfo &dstInfo)
{
    // basic valid check, other parameters valid check in readPixels method
    if (srcPixels == nullptr || dstPixels == nullptr) {
        HiLog::Error(LABEL, "src or dst pixels invalid.");
        return false;
    }
    Rect rect;
    rect.left = srcPos.x;
    rect.top = srcPos.y;
    rect.width = dstInfo.size.width;
    rect.height = dstInfo.size.height;
    if (!YUVCrop((uint8_t*)srcPixels, srcInfo.size, static_cast<uint8_t*>(dstPixels),
                                        rect, srcInfo.pixelFormat, dstInfo.pixelFormat)) {
        return false;
    }
    return true;
}

ColorYUV420 pixelYUV::GetYUV420Color(uint32_t x, uint32_t y)
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

#ifdef LIBYUV
bool pixelYUV::YUVCrop(const uint8_t *srcPixels, const Size &size, uint8_t *dstPixels,
                       const Rect &rect, const PixelFormat &srcFormat, const PixelFormat &dstFormat)
{
    libyuv::FourCC formatNum;
    switch (srcFormat) {
    case PixelFormat::NV21:
        formatNum = libyuv::FourCC::FOURCC_NV21;
        break;
    case PixelFormat::NV12:
        formatNum = libyuv::FourCC::FOURCC_NV12;
        break;
    case PixelFormat::YV12:
        formatNum = libyuv::FourCC::FOURCC_YV12;
        break;
    case PixelFormat::YU12:
        formatNum = libyuv::FourCC::FOURCC_YU12;
        break;
    default:
        return false;
    }
    if (libyuv::ConvertToI420(srcPixels, size.width * size.height +
                              ((size.width + 1) / NUM_2) * ((size.height + 1) / NUM_2) * NUM_2,
                              dstPixels, rect.width, dstPixels + rect.width * rect.height, (rect.width + 1) / NUM_2,
                              dstPixels + rect.width * rect.height +
                              ((rect.width + 1) / NUM_2) * ((rect.height + 1) / NUM_2),
                              (rect.width + 1) / NUM_2, rect.top, rect.left, size.width, size.height, rect.width,
                              rect.height, libyuv::RotationMode::kRotate0, formatNum) == -1) {
        return false;
    }
    size_t count = rect.width * rect.height + ((rect.width + 1) / NUM_2) * ((rect.height + 1) / NUM_2) * NUM_2;
    uint8_t* data = nullptr;
    switch (dstFormat) {
    case PixelFormat::NV21:
        data = (uint8_t *)malloc(count);
        if (libyuv::I420ToNV21(dstPixels, rect.width, dstPixels + rect.width * rect.height,
                               (rect.width + 1) / NUM_2, dstPixels + rect.width * rect.height +
                               ((rect.width + 1) / NUM_2) * ((rect.height + 1) / NUM_2),
                               (rect.width + 1) / NUM_2, data, rect.width,
                               data + rect.width * rect.height, ((rect.width + 1) / NUM_2) * NUM_2,
                               rect.width, rect.height) == -1) {
            free(data);
            return false;
        }
        memcpy_s(dstPixels, count, data, count);
        free(data);
        return true;
    case PixelFormat::NV12:
        data = (uint8_t *)malloc(count);
        if (libyuv::I420ToNV12(dstPixels, rect.width, dstPixels + rect.width * rect.height,
                               (rect.width + 1) / NUM_2, dstPixels + rect.width * rect.height +
                               ((rect.width + 1) / NUM_2) * ((rect.height + 1) / NUM_2),
                               (rect.width + 1) / NUM_2, data, rect.width,
                               data + rect.width * rect.height, ((rect.width + 1) / NUM_2) * NUM_2,
                               rect.width, rect.height) == -1) {
            free(data);
            return false;
        }
        memcpy_s(dstPixels, count, data, count);
        free(data);
        return true;
    case PixelFormat::YV12:
        data = (uint8_t *)malloc(count);
        I420AndYV12Transfers(dstPixels, data, rect.width, rect.height);
        memcpy_s(dstPixels, count, data, count);
        free(data);
        return true;
    case PixelFormat::YU12:
        return true;
    default:
        return false;
    }
    return true;
}

bool pixelYUV::YUVRotate(const uint8_t *srcPixels, Size &size, uint8_t *dstPixels,
                         int32_t degrees, const PixelFormat &format)
{
    libyuv::RotationMode rotateNum = libyuv::RotationMode::kRotate0;
    int dstWidth = size.width;
    int dstHeight = size.height;
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
    uint8_t* dstData = nullptr;
    switch (format) {
    case PixelFormat::NV21:
        dstData = (uint8_t *)malloc(dstWidth * dstHeight + ((dstWidth + 1) / NUM_2) *
                                                                ((dstHeight + 1) / NUM_2) * NUM_2);
        if (libyuv::NV12ToI420Rotate(srcPixels, size.width, srcPixels + size.height * size.width,
                ((size.width + 1) / NUM_2) * NUM_2, dstData, dstWidth,
                (dstData + dstWidth * dstHeight +
                ((dstWidth + 1) / NUM_2) * ((dstHeight + 1) / NUM_2)),
                (dstWidth + 1) / NUM_2, dstData + dstWidth * dstHeight,
                (dstWidth + 1) / NUM_2, size.width, size.height, rotateNum) == -1) {
            free(dstData);
            return false;
        }
        if (libyuv::I420ToNV21(dstData, dstWidth, dstData + dstWidth * dstHeight,
                               (dstWidth + 1) / NUM_2,
                               dstData + dstWidth * dstHeight + ((dstWidth + 1) / NUM_2) * ((dstHeight + 1) / NUM_2),
                               (dstWidth + 1) / NUM_2, dstPixels, dstWidth, dstPixels + dstWidth * dstHeight,
                               ((dstWidth + 1) / NUM_2) * NUM_2, dstWidth, dstHeight) == -1) {
            free(dstData);
            return false;
        }
        free(dstData);
        break;
    case PixelFormat::NV12:
        dstData = (uint8_t *)malloc(dstWidth * dstHeight + ((dstWidth + 1) / NUM_2) *
                                                                ((dstHeight + 1) / NUM_2) * NUM_2);
        if (libyuv::NV12ToI420Rotate(srcPixels, size.width, srcPixels + size.height * size.width,
                                     ((size.width + 1) / NUM_2) * NUM_2, dstData, dstWidth,
                                     dstData + dstWidth * dstHeight, (dstWidth + 1) / NUM_2,
                                     (dstData + dstWidth * dstHeight + ((dstWidth + 1) / NUM_2) *
                                     ((dstHeight + 1) / NUM_2)), (dstWidth + 1) / NUM_2,
                                     size.width, size.height, rotateNum) == -1) {
            free(dstData);
            return false;
        }
        if (libyuv::I420ToNV12(dstData, dstWidth, dstData + dstWidth * dstHeight,
                               (dstWidth + 1) / NUM_2,
                               dstData + dstWidth * dstHeight + ((dstWidth + 1) / NUM_2) * ((dstHeight + 1) / NUM_2),
                               (dstWidth + 1) / NUM_2, dstPixels, dstWidth, dstPixels + dstWidth * dstHeight,
                               ((dstWidth + 1) / NUM_2) * NUM_2, dstWidth, dstHeight) == -1) {
            free(dstData);
            return false;
        }
        free(dstData);
        break;
    case PixelFormat::YV12:
    case PixelFormat::YU12:
        if (libyuv::I420Rotate(srcPixels, size.width, srcPixels + size.width * size.height, (size.width + 1) / NUM_2,
                               srcPixels + size.width * size.height +
                               ((size.width + 1) / NUM_2) * ((size.height + 1) / NUM_2), (size.width + 1) / NUM_2,
                               dstPixels, dstWidth, dstPixels + dstWidth * dstHeight, (dstWidth + 1) / NUM_2,
                               dstPixels + dstWidth * dstHeight + ((dstWidth + 1) / NUM_2) * ((dstHeight + 1) / NUM_2),
                               (dstWidth + 1) / NUM_2, size.width, size.height, rotateNum) == -1) {
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

bool pixelYUV::YUVCrop(const uint8_t *srcPixels, const Size &size, uint8_t *dstPixels, const Rect &rect,
                                 const PixelFormat &srcFormat, const PixelFormat &dstFormat)
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

bool pixelYUV::YUVRotate(const uint8_t *srcPixels, Size &size, uint8_t *dstPixels,
                         int32_t degrees, const PixelFormat &format)
{
    switch (format) {
    case PixelFormat::YU12:
    case PixelFormat::YV12:
        if (!YV12Rotate(srcPixels, size, dstPixels, degrees)) {
            return false;
        }
        return true;
    case PixelFormat::NV21:
        NV21ToI420(srcPixels, dstPixels, size.width, size.height);
        break;
    case PixelFormat::NV12:
        NV12ToI420(srcPixels, dstPixels, size.width, size.height);
        break;
    default:
        return false;
    }
    size_t count = size.width * size.height + ((size.width + 1) / NUM_2) * ((size.height + 1) / NUM_2) * NUM_2;
    uint8_t *src = (uint8_t *)malloc(count);
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



void pixelYUV::TransposeW8(const uint8_t *src, int32_t srcStride, uint8_t *dst, int32_t dstStride, int32_t width)
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

void pixelYUV::TransposePlane(const uint8_t *src, int32_t srcStride, uint8_t *dst, int32_t dstStride,
                              const Size &half)
{
    int32_t i = half.height;
    while (i >= NUM_8) {
        TransposeW8(src, srcStride, dst, dstStride, half.width);
        src += NUM_8 * srcStride; // Go down 8 rows.
        dst += NUM_8;              // Move over 8 columns.
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

void pixelYUV::MirrorRow(const uint8_t *src, uint8_t *dst, int32_t width)
{
    src += width - 1;
    for (int x = 0; x < width - 1; x += NUM_2) {
        dst[x] = src[0];
        dst[x + 1] = src[-1];
        src -= 2;
    }
    if (width & 1) {
        dst[width - 1] = src[0];
    }
}

void pixelYUV::MirrorPlane(const uint8_t* srcY, int32_t srcStrideY, uint8_t* dstY, int32_t dstStrideY,
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

void pixelYUV::MirrorUVRow(const uint8_t* srcUV, uint8_t* dstUV, int32_t width)
{
    srcUV += (width - 1) << 1;
    for (int32_t x = 0; x < width; ++x) {
    dstUV[0] = srcUV[0];
    dstUV[1] = srcUV[1];
    srcUV -= 2;
    dstUV += 2;
    }
}

void pixelYUV::MirrorUVPlane(const uint8_t* srcUV, int32_t srcStrideUV, uint8_t* dstUV, int32_t dstStrideUV,
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

int32_t pixelYUV::I420Mirror(const uint8_t* srcY, int32_t srcStrideY, const uint8_t* srcU, int32_t srcStrideU,
                             const uint8_t* srcV, int32_t srcStrideV, uint8_t* dstY, int32_t dstStrideY,
                             uint8_t* dstU, int32_t dstStrideU, uint8_t* dstV, int32_t dstStrideV,
                             int32_t width, int32_t height)
{
    int32_t halfwidth = (width + 1) >> 1;
    int32_t halfheight = (height + 1) >> 1;

    if (!srcY || !srcU || !srcV || !dstU || !dstV || width <= 0 || height == 0) {
        return ERR_IMAGE_INVALID_PARAMETER;
    }

  // Negative height means invert the image.
    if (height < 0) {
        height = -height;
        halfheight = (height + 1) >> 1;
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

int32_t pixelYUV::NV12Mirror(const uint8_t* srcY, int32_t srcStrideY, const uint8_t* srcUV, int32_t srcStrideUV,
                             uint8_t* dstY, int32_t dstStrideY, uint8_t* dstUV, int32_t dstStrideUV,
                             int32_t width, int32_t height)
{
    int32_t halfwidth = (width + 1) >> 1;
    int32_t halfheight = (height + 1) >> 1;

    if (!srcY || !srcUV || !dstUV || width <= 0 || height == 0) {
        return ERR_IMAGE_INVALID_PARAMETER;
    }

    // Negative height means invert the image.
    if (height < 0) {
        height = -height;
        halfheight = (height + 1) >> 1;
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

int32_t pixelYUV::I420Copy(const uint8_t* srcY, int32_t srcStrideY, const uint8_t* srcU, int32_t srcStrideU,
                           const uint8_t* srcV, int32_t srcStrideV, uint8_t* dstY, int32_t dstStrideY,
                           uint8_t* dstU, int32_t dstStrideU, uint8_t* dstV, int32_t dstStrideV,
                           int32_t width, int32_t height)
{
    int32_t halfwidth = (width + 1) >> 1;
    int32_t halfheight = (height + 1) >> 1;
    if ((!srcY && dstY) || !srcU || !srcV || !dstU || !dstV || width <= 0 || height == 0) {
        return ERR_IMAGE_INVALID_PARAMETER;
    }
    // Negative height means invert the image.
    if (height < 0) {
        height = -height;
        halfheight = (height + 1) >> 1;
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

int32_t pixelYUV::NV12Copy(const uint8_t* srcY, int32_t srcStrideY, const uint8_t* srcUV, int32_t srcStrideUV,
                           uint8_t* dstY, int32_t dstStrideY, uint8_t* dstUV, int32_t dstStrideUV,
                           int32_t width, int32_t height)
{
    int32_t halfwidth = (width + 1) >> 1;
    int32_t halfheight = (height + 1) >> 1;

    if (!srcY || !dstY || !srcUV || !dstUV || width <= 0 || height == 0) {
        return ERR_IMAGE_INVALID_PARAMETER;
    }

    // Negative height means invert the image.
    if (height < 0) {
        height = -height;
        halfheight = (height + 1) >> 1;
        srcY = srcY + (height - 1) * srcStrideY;
        srcUV = srcUV + (halfheight - 1) * srcStrideUV;
        srcStrideY = -srcStrideY;
        srcStrideUV = -srcStrideUV;
    }
    CopyPlane(srcY, srcStrideY, dstY, dstStrideY, width, height);
    CopyPlane(srcUV, srcStrideUV, dstUV, dstStrideUV, halfwidth * 2, halfheight);
    return SUCCESS;
}

void pixelYUV::RotatePlane90(const uint8_t *src, int32_t srcStride, uint8_t *dst, int32_t dstStride, const Size &half)
{
    src += srcStride * (half.height - 1);
    srcStride = -srcStride;
    TransposePlane(src, srcStride, dst, dstStride, half);
}

void pixelYUV::RotatePlane180(const uint8_t *src, int32_t srcStride, uint8_t *dst, int32_t dstStride,
                              const Size &half)
{
    // Swap top and bottom row and mirror the content. Uses a temporary row.
    uint8_t *row = (uint8_t *)malloc(half.width);
    const uint8_t *src_bot = src + srcStride * (half.height - 1);
    uint8_t *dst_bot = dst + dstStride * (half.height - 1);
    int32_t half_height = (half.height + 1) >> 1;
    for (int32_t y = 0; y < half_height; ++y) {
        memcpy_s(row, half.width, src, half.width);
        MirrorRow(src_bot, dst, half.width);
        MirrorRow(row, dst_bot, half.width);
        src += srcStride;
        dst += dstStride;
        src_bot -= srcStride;
        dst_bot -= dstStride;
    }
    free(row);
}

void pixelYUV::RotatePlane270(const uint8_t *src, int32_t srcStride, uint8_t *dst, int32_t dstStride,
                              const Size &half)
{
    dst += dstStride * (half.width - 1);
    dstStride = -dstStride;
    TransposePlane(src, srcStride, dst, dstStride, half);
}

bool pixelYUV::YV12Rotate(const uint8_t *srcPixels, Size &size, uint8_t *dstPixels, int32_t degrees)
{
    int32_t dstWidth = size.width;
    int32_t dstHeight = size.height;
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
    }
    const uint8_t *srcU = srcPixels + size.width * size.height;
    const uint8_t *srcV = srcPixels + size.width * size.height +
                           ((size.width + 1) / NUM_2) * ((size.height + 1) / NUM_2);
    int32_t srcStrideU = (size.width + 1) / NUM_2;
    int32_t srcStrideV = (size.width + 1) / NUM_2;
    uint8_t *dstU = dstPixels + dstWidth * dstHeight;
    uint8_t *dstV = dstPixels + dstWidth * dstHeight + ((dstWidth + 1) / NUM_2) * ((dstHeight + 1) / NUM_2);
    int32_t dstStrideU = (dstWidth + 1) / NUM_2;
    int32_t dstStrideV = (dstWidth + 1) / NUM_2;
    Size halfSize = {(size.width + 1) >> 1, (size.height + 1) >> 1};
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

bool pixelYUV::CropYU12(const uint8_t *srcPixels, const Size &size, uint8_t *dstPixels, const Rect &rect)
{
    for (int i = 0; i < size.height; i++) {
        memcpy_s(dstPixels + i * rect.width, rect.width,
                 srcPixels + (rect.top + i) * size.width + rect.left, rect.width);
    }
    for (int i = 0; i < (rect.height + 1) / NUM_2; i++) {
        memcpy_s(dstPixels + rect.width * rect.height + i * ((rect.width + 1) / NUM_2), ((rect.width + 1) / NUM_2),
                 srcPixels + size.width * size.height + (rect.top / NUM_2 + i) * ((size.width + 1) / NUM_2) +
                 rect.left / NUM_2, ((rect.width + 1) / NUM_2));
        memcpy_s(dstPixels + rect.width * rect.height + ((rect.width + 1) / NUM_2) * ((rect.height + 1) / NUM_2) +
                 i * ((rect.width + 1) / NUM_2), ((rect.width + 1) / NUM_2),
                 srcPixels + size.width * size.height + ((size.width + 1) / NUM_2) * ((size.height + 1) / NUM_2) +
                 ((rect.top / NUM_2) + i) * ((size.width + 1) / NUM_2) + rect.left / NUM_2, ((rect.width + 1) / NUM_2));
    }
    return true;
}

uint8_t pixelYUV::GetYUV420Y(uint32_t x, uint32_t y, int32_t width, const uint8_t *in)
{
    return *(in + y * width + x);
}

uint8_t pixelYUV::GetYUV420U(uint32_t x, uint32_t y, int32_t width,
                             int32_t height, PixelFormat format, const uint8_t *in)
{
    switch (format) {
        case PixelFormat::NV21: {
            if (width & 1) {
                return *(in + y / NUM_2 * NUM_2 + width * height + (y / NUM_2) * (width - 1) + (x & ~1) + 1);
            }
            return *(in + width * height + (y / NUM_2) * width + (x & ~1) + 1);
        }
        case PixelFormat::NV12: {
            if (width & 1) {
                return *(in + y / NUM_2 * NUM_2 + width * height + (y / NUM_2) * (width - 1) + (x & ~1));
            }
            return *(in + width * height + (y / NUM_2) * width + (x & ~1));
        }
        case PixelFormat::YU12: {
            if (width & 1) {
              return *(in + y / NUM_2 +(width * height) + (y / NUM_2) * (width / NUM_2) + (x / NUM_2));
            }
            return *(in +(width * height) + (y / NUM_2) * (width / NUM_2) + (x / NUM_2));
        }
        case PixelFormat::YV12: {
            if (width & 1) {
                return *(in + height / NUM_2 + y / NUM_2 + (width * height) + (width / NUM_2) * (height / NUM_2) +
                        (y / NUM_2) * (width / NUM_2) + (x / NUM_2));
            }
            return *(in + (width * height) + (width / NUM_2) * (height / NUM_2) +
                    (y / NUM_2) * (width / NUM_2) + (x / NUM_2));
        }
        default:
            break;
    }
    return SUCCESS;
}

uint8_t pixelYUV::GetYUV420V(uint32_t x, uint32_t y, int32_t width,
                             int32_t height, PixelFormat format, const uint8_t *in)
{
    switch (format) {
        case PixelFormat::NV21: {
            if (width & 1) {
                return *(in + y/2 * 2 + width * height + (y / NUM_2) * (width - 1) + (x & ~1));
            }
            return *(in + width * height + (y / NUM_2) * width + (x & ~1));
        }
        case PixelFormat::NV12: {
            if (width & 1) {
                return *(in + y/2 * 2 + width * height + (y / NUM_2) * (width - 1) + (x & ~1) + 1);
            }
            return *(in + width * height + (y / NUM_2) * width + (x & ~1) + 1);
        }
        case PixelFormat::YU12: {
            if (width & 1) {
              return *(in + height / NUM_2 + y / NUM_2 + (width * height) + (width / NUM_2) * (height / NUM_2) +
                      (y / NUM_2) * (width / NUM_2) + (x / NUM_2));
            }
              return *(in + (width * height) + (width / NUM_2) * (height / NUM_2) +
                      (y / NUM_2) * (width / NUM_2) + (x / NUM_2));
            break;
        }
        case PixelFormat::YV12: {
            if (width & 1) {
              return *(in + y / NUM_2 +(width * height) + (y / NUM_2) * (width / NUM_2) + (x / NUM_2));
            }
            return *(in + (width * height) + (y / NUM_2) * (width / NUM_2) + (x / NUM_2));
        }
        default:
            break;
    }
    return SUCCESS;
}

bool pixelYUV::RGBAToI420(const uint8_t *src, uint8_t *dst, int32_t width, int32_t height)
{
    if (!src || !dst || width <= 0 || height == 0) {
        return false;
    }
    size_t image_size = width * height;

    size_t upos = image_size;
    size_t vpos = upos + ((width + 1) / NUM_2) * ((height + 1) / NUM_2);

    size_t i = 0;

    for(int32_t line = 0; line < height; ++line) {
        if (!(line % NUM_2)) {
            for (int x = 0; x < width; x += 2) {
                uint8_t r = src[4 * i];
                uint8_t g = src[4 * i + 1];
                uint8_t b = src[4 * i + 2];

                uint8_t y = dst[i++] = 16 + 219 * (0.2627 * r + 0.678 * g + 0.0593 * b) / 255;

                dst[upos++] = 128 + 224 * (0.5 * (b - y) / 0.9407) / 255;
                dst[vpos++] = 128 + 224 * (0.5 * (r - y) / 0.7373) / 255;

                r = src[4 * i];
                g = src[4 * i + 1];
                b = src[4 * i + 2];
                if ((width & 1) && x == width - 1) {
                  //nothing to do
                } else {
                  dst[i++] = 16 + 219 * (0.2627 * r + 0.678 * g + 0.0593 * b) / 255;
                }
            }
        } else {
            for(int32_t x = 0; x < width; x += 1) {
                uint8_t r = src[4 * i];
                uint8_t g = src[4 * i + 1];
                uint8_t b = src[4 * i + 2];

                dst[i++] = 16 + 219 * (0.2627 * r + 0.678 * g + 0.0593 * b) / 255;
            }
        }
    }
    return true;
}


bool pixelYUV::YUV420ToARGB(const uint8_t *in, uint8_t *out, int32_t width, int32_t height, PixelFormat pixelFormat)
{
    if (!in || !out || width <=0 || height == 0) {
       return false;
    }
    for (int32_t i = 0; i < height; i++) {
        for (int32_t j = 0; j < width; j++) {
            uint8_t Y = GetYUV420Y(j, i, width, in);
            uint8_t U = GetYUV420U(j, i, width, height, pixelFormat, in);
            uint8_t V = GetYUV420V(j, i, width, height, pixelFormat, in);

            int32_t colorR = Y + 1.402 * (V - 128);
            int32_t colorG = Y - 0.344136 * (U - 128) - 0.714136 * (V - 128);
            int32_t colorB = Y + 1.772 * (U - 128);

            colorR = colorR > 255 ? 255 : (colorR < 0 ? 0 : colorR);
            colorG = colorG > 255 ? 255 : (colorG < 0 ? 0 : colorG);
            colorB = colorB > 255 ? 255 : (colorB < 0 ? 0 : colorB);

            *out++ = 0xFF;
            *out++ = colorR;
            *out++ = colorG;
            *out++ = colorB;
        }
    }
    return true;
}

bool pixelYUV::RGBAToYUV420(const uint8_t *src, uint8_t *dst, int32_t width, int32_t height, PixelFormat pixelFormat)
{
    uint32_t pictureSize = width * height + NUM_2 *((width + 1) / NUM_2) * ((height + 1) / NUM_2);
    uint8_t *temp = (uint8_t*)malloc(pictureSize);
    bool result = RGBAToI420(src, temp, width, height);
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
        memcpy_s(dst, pictureSize, temp, pictureSize);
        break;
    case PixelFormat::YV12:
        I420AndYV12Transfers(temp, dst, width, height);
        break;
    default:
        break;
    }
    free(temp);
    return true;
}

bool pixelYUV::YUV420ToRGBA(const uint8_t *in, uint8_t *out, int32_t width, int32_t height, PixelFormat pixelFormat)
{
    if (!in || !out || width <=0 || height == 0) {
       return false;
    }
    for (int32_t i = 0; i < height; i++) {
        for (int32_t j = 0; j < width; j++) {
            uint8_t Y = GetYUV420Y(j, i, width, in);
            uint8_t U = GetYUV420U(j, i, width, height, pixelFormat, in);
            uint8_t V = GetYUV420V(j, i, width, height, pixelFormat, in);

            int32_t colorR = Y + 1.402 * (V - 128);
            int32_t colorG = Y - 0.344136 * (U - 128) - 0.714136 * (V - 128);
            int32_t colorB = Y + 1.772 * (U - 128);

            colorR = colorR > 255 ? 255 : (colorR < 0 ? 0 : colorR);
            colorG = colorG > 255 ? 255 : (colorG < 0 ? 0 : colorG);
            colorB = colorB > 255 ? 255 : (colorB < 0 ? 0 : colorB);

            *out++ = colorR;
            *out++ = colorG;
            *out++ = colorB;
            *out++ = 0xFF;
        }
    }
    return true;
}

void pixelYUV::WriteDataI420Convert(uint8_t *srcPixels, const Size &size, uint8_t *dstPixels,
                                    const Position &dstPos, const Size &dstSize)
{
    const uint8_t *srcU = srcPixels + size.width * size.height;
    const uint8_t *srcV = srcU + ((size.width + 1) / NUM_2) * ((size.height + 1) / NUM_2);
    uint8_t *dstU = dstPixels + dstSize.width * dstSize.height;
    uint8_t *dstV = dstU + ((dstSize.width + 1) / NUM_2) * ((dstSize.height + 1) / NUM_2);
    for (int32_t i = 0; i < size.height; i++) {
        memcpy_s(dstPixels + (dstPos.y + i) * dstSize.width + dstPos.x, size.width,
                 srcPixels + i * size.width, size.width);
    }
    for (int32_t i = 0; i < (size.height + 1) / NUM_2; i++) {
        memcpy_s(dstU + (dstPos.y / NUM_2 + i) * ((dstSize.width + 1) / NUM_2) + dstPos.x / NUM_2,
                 ((size.width + 1) / NUM_2), srcU + i * ((size.width + 1) / NUM_2), ((size.width + 1) / NUM_2));
        memcpy_s(dstV + (dstPos.y / NUM_2 + i) * ((dstSize.width + 1) / NUM_2) + dstPos.x / NUM_2,
                 ((size.width + 1) / NUM_2), srcV + i * ((size.width + 1) / NUM_2), ((size.width + 1) / NUM_2));
    }
}

bool pixelYUV::I420AndYV12Transfers(const uint8_t *src, uint8_t *dst, int32_t width, int32_t height)
{
    uint32_t ySize = width * height;
    memcpy_s(dst, ySize, src, ySize);
    uint32_t uvDataSize = ((width + 1) / NUM_2) * ((height + 1) / NUM_2);
    memcpy_s(dst + ySize, uvDataSize, src + ySize + uvDataSize, uvDataSize);
    memcpy_s(dst + ySize + uvDataSize, uvDataSize, src + ySize, uvDataSize);
    return true;
}

void pixelYUV::SplitUV(const uint8_t *srcUV, uint8_t *dstU, uint8_t *dstV, int32_t width)
{
    for (int32_t x = 0; x < width - 1; x += NUM_2) {
        dstU[x] = srcUV[0];
        dstU[x + 1] = srcUV[NUM_2];
        dstV[x] = srcUV[1];
        dstV[x + 1] = srcUV[NUM_3];
        srcUV += NUM_4;
    }
    if (width & 1) {
        dstU[width - 1] = srcUV[0];
        dstV[width - 1] = srcUV[1];
    }
}

bool pixelYUV::NV12ToI420(const uint8_t *src, uint8_t *dst, int32_t width, int32_t height)
{
    if (!src || !dst || width <= 0 || height == 0) {
        return false;
    }
    uint32_t ySize = width * height;
    uint32_t uvDataSize = ((width + 1) / NUM_2) * ((height + 1) / NUM_2);
    uint8_t uvData[uvDataSize * NUM_2];
    uint8_t uData[uvDataSize];
    uint8_t vData[uvDataSize];
    memcpy_s(uvData, uvDataSize * NUM_2, src + ySize, uvDataSize * NUM_2);
    uint8_t *srcUV = uvData;
    uint8_t *srcU = uData;
    uint8_t *srcV = vData;
    for (int32_t y = 0; y < (height + 1) / NUM_2; ++y) {
        SplitUV(srcUV, srcU, srcV, (width + 1) / NUM_2);
        srcU += ((width + 1) >> 1);
        srcV += ((width + 1) >> 1);
        srcUV += (width + 1) / NUM_2 * NUM_2;
    }
    memcpy_s(dst, ySize, src, ySize);
    memcpy_s(dst + ySize, uvDataSize, uData, uvDataSize);
    memcpy_s(dst + ySize + uvDataSize, uvDataSize, vData, uvDataSize);
    return true;
}

void pixelYUV::CopyPlane(const uint8_t* srcY, int32_t srcStrideY, uint8_t* dstY, int32_t dstStrideY,
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
        memcpy_s(dstY, width, srcY, width);
        srcY += srcStrideY;
        dstY += dstStrideY;
    }
}

bool pixelYUV::I420ToNV21(const uint8_t *srcPixels, uint8_t *dstPixels, int32_t width, int32_t height)
{
    int32_t halfWidth = (width + 1) / NUM_2;
    int32_t halfHeight = (height + 1) / NUM_2;
    const uint8_t* srcU = srcPixels + width * height + halfHeight * halfWidth;
    const uint8_t* srcV = srcPixels + width * height;
    uint8_t* dstUV = dstPixels + width * height;
    int32_t dstStrideUV = halfWidth * NUM_2;
    memcpy_s(dstPixels, width * height, srcPixels, width * height);
    MergeUVPlane(srcU, halfWidth, srcV, halfWidth, dstUV, dstStrideUV,
                 halfWidth, halfHeight);
    return true;
}

bool pixelYUV::I420ToNV12(const uint8_t *srcPixels, uint8_t *dstPixels, int32_t width, int32_t height)
{
    int32_t halfWidth = (width + 1) / NUM_2;
    int32_t halfHeight = (height + 1) / NUM_2;
    const uint8_t* srcV = srcPixels + width * height + halfHeight * halfWidth;
    const uint8_t* srcU = srcPixels + width * height;
    uint8_t* dstUV = dstPixels + width * height;
    int32_t dstStrideUV = halfWidth * NUM_2;
    // Negative height means invert the image.
    memcpy_s(dstPixels, width * height, srcPixels, width * height);
    MergeUVPlane(srcU, halfWidth, srcV, halfWidth, dstUV, dstStrideUV,
                 halfWidth, halfHeight);
    return true;
}


void pixelYUV::MergeUV(const uint8_t *srcU, const uint8_t *srcV, uint8_t *dstUV, int32_t width)
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

void pixelYUV::MergeUVPlane(const uint8_t* srcU, int32_t srcStrideU, const uint8_t* srcV, int32_t srcStrideV,
                            uint8_t* dstUV, int32_t dstStrideUV, int32_t width, int32_t height)
{
    if (width <= 0 || height == 0) {
        return;
    }
    // Coalesce rows.
    if (srcStrideU == width && srcStrideV == width && dstStrideUV == width * 2) {
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

bool pixelYUV::NV21ToI420(const uint8_t *src, uint8_t *dst, int32_t width, int32_t height)
{
    if (!src || !dst || width <= 0 || height == 0) {
        return false;
    }
    uint32_t ySize = width * height;
    uint32_t uvDataSize = ((width + 1) / NUM_2) * ((height + 1) / NUM_2);
    uint8_t uvData[uvDataSize * NUM_2];
    uint8_t uData[uvDataSize];
    uint8_t vData[uvDataSize];
    memcpy_s(uvData, uvDataSize * NUM_2, src + ySize, uvDataSize * NUM_2);
    uint8_t *srcUV = uvData;
    uint8_t *srcU = uData;
    uint8_t *srcV = vData;
    for (int32_t y = 0; y < (height + 1) / NUM_2; ++y) {
        SplitUV(srcUV, srcV, srcU, (width + 1) / NUM_2);
        srcU += ((width + 1) >> 1);
        srcV += ((width + 1) >> 1);
        srcUV += (width + 1) / NUM_2 * NUM_2;
    }
    memcpy_s(dst, ySize, src, ySize);
    memcpy_s(dst + ySize, uvDataSize, uData, uvDataSize);
    memcpy_s(dst + ySize + uvDataSize, uvDataSize, vData, uvDataSize);
    return true;
}

bool pixelYUV::CheckPixelsInput(const uint8_t *dst, const uint64_t &bufferSize, const uint32_t &offset,
                                 const Rect &region)
{
    if (dst == nullptr) {
        HiLog::Error(LABEL, "CheckPixelsInput input dst address is null.");
        return false;
    }

    if (bufferSize == 0) {
        HiLog::Error(LABEL, "CheckPixelsInput input buffer size is 0.");
        return false;
    }

    if (region.left < 0 || region.top < 0 || static_cast<uint64_t>(offset) > bufferSize) {
        HiLog::Error(
            LABEL,
            "CheckPixelsInput left(%{public}d) or top(%{public}d)  or offset(%{public}u) < 0.",
            region.left, region.top,  offset);
        return false;
    }
    if (region.width <= 0 || region.height <= 0 || region.width > MAX_DIMENSION || region.height > MAX_DIMENSION) {
        HiLog::Error(LABEL, "CheckPixelsInput width(%{public}d) or height(%{public}d) is < 0.", region.width,
                     region.height);
        return false;
    }
    if (region.left > GetWidth() - region.width) {
        HiLog::Error(LABEL, "CheckPixelsInput left(%{public}d) + width(%{public}d) is > pixelYUV width(%{public}d).",
                     region.left, region.width, GetWidth());
        return false;
    }
    if (region.top > GetHeight() - region.height) {
        HiLog::Error(LABEL, "CheckPixelsInput top(%{public}d) + height(%{public}d) is > pixelYUV height(%{public}d).",
                     region.top, region.height, GetHeight());
        return false;
    }

    if (static_cast<uint64_t>(offset) > (bufferSize - NUM_3)) {
        HiLog::Error(LABEL,
                     "CheckPixelsInput fail, height(%{public}d), width(%{public}d) "
                     "offset(%{public}u), bufferSize:%{public}llu.",
                     region.height, region.width,  offset,
                     static_cast<unsigned long long>(bufferSize));
        return false;
    }
    return true;
}

void pixelYUV::YUV420PTranslate(const uint8_t *srcPixels, Size &size, uint8_t *dstPixels,
                                float xAxis, float yAxis, bool isI420)
{
    int32_t dstWidth = size.width;
    int32_t dstHeight = size.height;

    const uint8_t *srcU;
    const uint8_t *srcV;
    uint8_t *dstU;
    uint8_t *dstV;

    if (isI420) {
        srcU = srcPixels + size.width * size.height;
        srcV = srcU + ((size.width + 1) / NUM_2) * ((size.height + 1) / NUM_2);
        dstU = dstPixels + dstWidth * dstHeight;
        dstV = dstU + ((dstWidth + 1) / NUM_2) * ((dstHeight + 1) / NUM_2);
    } else {
        srcV = srcPixels + size.width * size.height;
        srcU = srcV + ((size.width+ 1) / NUM_2) * ((size.height + 1) / NUM_2);
        dstV = dstPixels + dstWidth * dstHeight;
        dstU = dstV + ((dstWidth + 1) / NUM_2) * ((dstHeight + 1) / NUM_2);
    }

    for (int32_t y = 0; y < size.height; y++) {
        for (int32_t x = 0; x < size.width; x++) {
            int32_t newX = x + xAxis;
            int32_t newY = y + yAxis;
            if (newX >= 0 && newY >= 0 && newX < size.width  && newY < size.height) {
                *(dstPixels + newY * dstWidth + newX) = *(srcPixels + y * size.width + x);
            }
        }
    }

    for (int32_t y = 0; y < size.height / NUM_2; y++) {
        for (int32_t x = 0; x < size.width / NUM_2; x++) {
            int32_t newX = x + xAxis / NUM_2;
            int32_t newY = y + yAxis / NUM_2;
            if (newX >= 0 && newY >= 0 && newX < size.width / NUM_2  && newY < size.height / NUM_2) {
                *(dstU + newY * (dstWidth / NUM_2) + newX) = *(srcU + y * (size.width / NUM_2) + x);
                *(dstV + newY * (dstWidth / NUM_2) + newX) = *(srcV + y * (size.width / NUM_2) + x);
            }
        }
    }
}

void pixelYUV::YUV420SPTranslate(const uint8_t *srcPixels, Size &size, uint8_t *dstPixels, float xAxis, float yAxis)
{
    int32_t dstWidth = size.width;
    int32_t dstHeight = size.height;

    const uint8_t *srcUV = srcPixels + size.width * size.height;
    uint8_t *dstUV = dstPixels + dstWidth * dstHeight;

    for (int32_t y = 0; y < size.height; y++) {
        for (int32_t x = 0; x < size.width; x++) {
            int32_t newX = x + xAxis;
            int32_t newY = y + yAxis;
            if (newX >= 0 && newY >= 0 && newX < size.width && newY < size.height) {
                *(dstPixels + newY * dstWidth + newX) = *(srcPixels + y * size.width + x);
            }
        }
    }

    for (int32_t y = 0; y < size.height / NUM_2; y++) {
        for (int32_t x = 0; x < size.width / NUM_2; x++) {
            int32_t newX = x + xAxis / NUM_2;
            int32_t newY = y + yAxis / NUM_2;
            if (newX >= 0 && newX < size.width / NUM_2 && newY >= 0 && newY < size.height / NUM_2) {
                *(dstUV + (newY * dstWidth + newX * NUM_2)) = *(srcUV + (y * size.width + x * NUM_2));
                *(dstUV + (newY * dstWidth + newX * NUM_2) + 1) = *(srcUV + (y * size.width + x * NUM_2) + 1);
            }
        }
    }
}

bool pixelYUV::YUVTransLate(const uint8_t *srcPixels, Size &size, uint8_t *dstPixels,
                            const PixelFormat &format, float xAxis, float yAxis)
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

void pixelYUV::translate(float xAxis, float yAxis)
{
    if (xAxis == 0 && yAxis == 0) {
        return;
    }
    int32_t width = imageInfo_.size.width;
    int32_t height = imageInfo_.size.height;

    int32_t dstSize = width * height + ((width + 1) / NUM_2 * (height + 1) / NUM_2) * NUM_2;
    uint8_t *dst = (uint8_t *)malloc(dstSize);
    YUVTransLate(data_, imageInfo_.size, dst, imageInfo_.pixelFormat, xAxis, yAxis);

    imageInfo_.size.width = width;
    imageInfo_.size.height = height;
    memcpy_s(data_, dstSize, dst, dstSize);
    free(dst);
}

void pixelYUV::YUV420PReadPixel(const uint8_t *srcPixels, Size &size, const Position &pos, uint32_t &dst, bool isI420)
{
    const uint8_t *srcU;
    const uint8_t *srcV;

    if (isI420) {
        srcU = srcPixels + size.width * size.height;
        srcV = srcU + (size.width / NUM_2) * (size.height / NUM_2);
    } else {
        srcV = srcPixels + size.width * size.height;
        srcU = srcV + (size.width / NUM_2) * (size.height / NUM_2);
    }

    uint8_t colorY = *(srcPixels + (pos.y * size.width + pos.x));
    uint8_t colorU = *(srcU + ((pos.y / NUM_2) * (size.width / NUM_2) + pos.x / NUM_2));
    uint8_t colorV = *(srcV + ((pos.y / NUM_2) * (size.width / NUM_2) + pos.x / NUM_2));

    uint32_t YUVcolor = (colorY << Y_SHIFT) | (colorU << U_SHIFT) | (colorV << V_SHIFT);
    dst = YUVcolor;
}

void pixelYUV::YUV420SPReadPixel(const uint8_t *srcPixels, Size &size, const Position &pos, uint32_t &dst, bool isNV12)
{
    const uint8_t *srcUV = srcPixels + size.width * size.height;
    uint8_t colorY = *(srcPixels + (pos.y * size.width + pos.x));
    uint8_t colorU = 0;
    uint8_t colorV = 0;

    if (isNV12) {
        colorU = *(srcUV + (pos.y / NUM_2) * size.width + pos.x / NUM_2);
        colorV = *(srcUV + (pos.y / NUM_2) * size.width + pos.x / NUM_2+ 1);
    } else {
        colorV = *(srcUV + (pos.y / NUM_2) * size.width + pos.x / NUM_2);
        colorU = *(srcUV + (pos.y / NUM_2) * size.width + pos.x / NUM_2+ 1);
    }

    uint32_t YUVcolor = (colorY << Y_SHIFT) | (colorU << U_SHIFT) | (colorV << V_SHIFT);
    dst = YUVcolor;
}


bool pixelYUV::YUVReadPixel(const uint8_t *srcPixels, Size &size, const PixelFormat &format,
                            const Position &pos, uint32_t &dst)
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
        YUV420SPReadPixel(srcPixels, size, pos, dst,  isNV12);
        return true;
    }
    default:
        return false;
    }
}

uint32_t pixelYUV::ReadPixel(const Position &pos, uint32_t &dst)
{
    if (pos.x < 0 || pos.y < 0) {
        HiLog::Error(LABEL, "read pixel by pos input invalid exception. [x(%{public}d), y(%{public}d)]", pos.x, pos.y);
        return ERR_IMAGE_INVALID_PARAMETER;
    }
    if (data_ == nullptr) {
        HiLog::Error(LABEL, "read pixel by pos source data is null.");
        return ERR_IMAGE_READ_PIXELMAP_FAILED;
    }
    YUVReadPixel(data_, imageInfo_.size, imageInfo_.pixelFormat, pos, dst);
    return SUCCESS;
}

void pixelYUV::YUV420PWritePixel(uint8_t *srcPixels, Size &size, const Position &pos,
                                 const uint32_t &color, bool isI420)
{
    uint8_t *srcU;
    uint8_t *srcV;

    uint8_t colorY = color >> Y_SHIFT & YUV420_MASK;
    uint8_t colorU = color >> U_SHIFT & YUV420_MASK;
    uint8_t colorV = color >> V_SHIFT & YUV420_MASK;

    if (isI420) {
        srcU = srcPixels + size.width * size.height;
        srcV = srcU + (size.width / NUM_2) * (size.height / NUM_2);
    } else {
        srcV = srcPixels + size.width * size.height;
        srcU = srcV + (size.width / NUM_2) * (size.height / NUM_2);
    }

    *(srcPixels + (pos.y * size.width + pos.x)) = colorY;
    *(srcU + ((pos.y / NUM_2) * (size.width / NUM_2) + pos.x / NUM_2)) = colorU;
    *(srcV + ((pos.y / NUM_2) * (size.width / NUM_2) + pos.x / NUM_2)) = colorV;
}

void pixelYUV::YUV420SPWritePixel(uint8_t *srcPixels, Size &size, const Position &pos,
                                  const uint32_t &color, bool isNV12)
{
    uint8_t *srcUV = srcPixels + size.width * size.height;
    uint8_t colorY = color >> Y_SHIFT & YUV420_MASK;
    uint8_t colorU = color >> U_SHIFT & YUV420_MASK;
    uint8_t colorV = color >> V_SHIFT & YUV420_MASK;

    *(srcPixels + (pos.y * size.width + pos.x)) = colorY;

    if (isNV12) {
        *(srcUV + (pos.y / NUM_2) * size.width + pos.x / NUM_2) = colorU;
        *(srcUV + (pos.y / NUM_2) * size.width + pos.x / NUM_2+ 1) = colorV;
    } else {
        *(srcUV + (pos.y / NUM_2) * size.width + pos.x / NUM_2) = colorV;
        *(srcUV + (pos.y / NUM_2) * size.width + pos.x / NUM_2 + 1) = colorU;
    }
}

bool pixelYUV::YUVWritePixel(uint8_t *srcPixels, Size &size, const PixelFormat &format,
                             const Position &pos, const uint32_t &color)
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

uint32_t pixelYUV::WritePixel(const Position &pos, const uint32_t &color)
{
    if (pos.x < 0 || pos.y < 0) {
        HiLog::Error(LABEL,
                     "write pixel by pos but input position is invalid. [x(%{public}d), y(%{public}d)]",
                     pos.x, pos.y);
        return ERR_IMAGE_INVALID_PARAMETER;
    }
    if (data_ == nullptr) {
        HiLog::Error(LABEL, "write pixel by pos but current pixelmap data is nullptr.");
        return ERR_IMAGE_WRITE_PIXELMAP_FAILED;
    }
    YUVWritePixel(data_, imageInfo_.size, imageInfo_.pixelFormat, pos, color);
    return SUCCESS;
}


void pixelYUV::YUV420PWritePixels(Size &size, uint8_t *srcPixels, const uint32_t &color, bool isI420)
{
    uint8_t colorY = color >> Y_SHIFT & YUV420_MASK;
    uint8_t colorU = color >> U_SHIFT & YUV420_MASK;
    uint8_t colorV = color >> V_SHIFT & YUV420_MASK;
     
    uint8_t *srcU;
    uint8_t *srcV;
    if (isI420) {
        srcU = srcPixels + size.width * size.height;
        srcV = srcU + (size.width + 1) / NUM_2 * (size.height + 1) / NUM_2;
    } else {
        srcV = srcPixels + size.width * size.height;
        srcU = srcV + (size.width + 1) / NUM_2 * (size.height + 1) / NUM_2;
    }

    for (int32_t i = 0; i < size.height; i++) {
        for (int32_t j = 0; j < size.width; j++) {
            *(srcPixels + i * size.width + j) = colorY;
        }
    }
    for (int32_t i = 0; i < (size.height + 1) / NUM_2; i++) {
        for (int32_t j = 0; j < (size.width + 1) / NUM_2; j++) {
            *(srcU + i * (size.width + 1) / NUM_2 + j) = colorU;
            *(srcV + i * (size.width + 1) / NUM_2 + j) = colorV;
        }
    }
}

void pixelYUV::YUV420SPWritePixels(Size &size, uint8_t *srcPixels, const uint32_t &color, bool isNV12)
{
    uint8_t colorY = color >> Y_SHIFT & YUV420_MASK;
    uint8_t colorU = color >> U_SHIFT & YUV420_MASK;
    uint8_t colorV = color >> V_SHIFT & YUV420_MASK;
  
    uint8_t *srcUV = srcPixels + size.width * size.height;

    for (int32_t y = 0; y < size.height; y++) {
        for (int32_t x = 0; x < size.width; x++) {
            *(srcPixels + y* size.width + x) = colorY;
        }
    }

    for (int32_t y = 0; y < size.height / NUM_2; y++) {
        for (int32_t x = 0; x < size.width; x++) {
            if (isNV12) {
                *(srcUV + (y * size.width + x )) = colorU;
                *(srcUV + (y * size.width + x ) + 1) = colorV;
            } else {
                *(srcUV + (y * size.width + x)) = colorV;
                *(srcUV + (y * size.width + x) + 1) = colorU;
            }
        }
    }
}

bool pixelYUV::YUV420WritePixels(Size &size, uint8_t *srcPixels, const PixelFormat &format, const uint32_t &color)
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

bool pixelYUV::WritePixels(const uint32_t &color)
{
    if (data_ == nullptr) {
        HiLog::Error(LABEL, "erase pixels by color current pixel map data is null.");
        return false;
    }
    YUV420WritePixels(imageInfo_.size, data_, imageInfo_.pixelFormat, color);
    return true;
}

static bool isSameColorSpace(const OHOS::ColorManager::ColorSpace &src,
    const OHOS::ColorManager::ColorSpace &dst)
{
    auto skSrc = src.ToSkColorSpace();
    auto skDst = dst.ToSkColorSpace();
    return SkColorSpace::Equals(skSrc.get(), skDst.get());
}

#ifdef IMAGE_COLORSPACE_FLAG
uint32_t pixelYUV::ApplyColorSpace(const OHOS::ColorManager::ColorSpace &grColorSpace)
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
    imageInfo.pixelFormat = PixelFormat::RGBA_8888;
    SkTransInfo src;
    src.info = ToSkImageInfo(imageInfo, ToSkColorSpace(this));
    uint64_t rowStride = src.info.minRowBytes();
    uint8_t* srcData = data_;
#if !defined(_WIN32) && !defined(_APPLE) && !defined(IOS_PLATFORM) && !defined(A_PLATFORM)
    if (GetAllocatorType() == AllocatorType::DMA_ALLOC && GetFd() != nullptr) {
        SurfaceBuffer* sbBuffer = reinterpret_cast<SurfaceBuffer*>(GetFd());
        rowStride = sbBuffer->GetStride();
    }
    srcData = static_cast<uint8_t *>(GetWritablePixels());
#endif

    int32_t width = imageInfo.size.width;
    int32_t height = imageInfo.size.height;
    uint8_t *RGBAdata = (uint8_t *)malloc(width * height * NUM_4);
    YUV420ToRGBA(srcData, RGBAdata, width, height, format);
    src.bitmap.installPixels(src.info, RGBAdata, rowStride);
    // Build sk target infomation
    SkTransInfo dst;
    dst.info = ToSkImageInfo(imageInfo, grColorSpace.ToSkColorSpace());
    // uint32_t pictureSize = width * height + 2 *((width + 1) / NUM_2) * ((height + 1) / NUM_2);
    MemoryData memoryData = {nullptr, width * height * NUM_4, "Trans ImageData", {dst.info.width(), dst.info.height()}};
    // memoryData = {nullptr, dst.info.computeMinByteSize(), "Trans ImageData", {dst.info.width(), dst.info.height()}};
    auto m = MemoryManager::CreateMemory(allocatorType_, memoryData);
    if (m == nullptr) {
        HiLog::Error(LABEL, "applyColorSpace CreateMemory failed");
        return ERR_IMAGE_COLOR_CONVERT;
    }
    // Transfor pixels by readPixels
    if (!src.bitmap.readPixels(dst.info, m->data.data, rowStride, 0, 0)) {
        m->Release();
        HiLog::Error(LABEL, "ReadPixels failed");
        return ERR_IMAGE_COLOR_CONVERT;
    }

    int32_t dstWidth = dst.info.width();
    int32_t dstHeight = dst.info.height();
    uint32_t pictureSize = dstWidth * dstHeight + 2 *((dstWidth + 1) / NUM_2) * ((dstHeight + 1) / NUM_2);
    MemoryData memoryYUVData = {nullptr, pictureSize, "Trans ImageData", {dstWidth, dstHeight}};
    auto YUVMemory = MemoryManager::CreateMemory(allocatorType_, memoryYUVData);
    if (YUVMemory == nullptr) {
        HiLog::Error(LABEL, "applyColorSpace CreateYUVMemory failed");
        return ERR_IMAGE_COLOR_CONVERT;
    }
    RGBAToYUV420(reinterpret_cast<uint8_t*>(m->data.data), reinterpret_cast<uint8_t*>(YUVMemory->data.data),
                 dstWidth, dstHeight, format);
    m->Release();
    imageInfo.pixelFormat = format;
    dst.info = ToSkImageInfo(imageInfo, grColorSpace.ToSkColorSpace());
    ToImageInfo(imageInfo, dst.info);
    grColorSpace_ = std::make_shared<OHOS::ColorManager::ColorSpace>(dst.info.refColorSpace(), grName);
    SetPixelsAddr(reinterpret_cast<void*>(YUVMemory->data.data), nullptr, pictureSize, YUVMemory->GetType(), nullptr);
    SetImageInfo(imageInfo, true);
    return SUCCESS;
}
#endif

bool pixelYUV::CropNV21(const uint8_t *srcPixels, const Size &size, uint8_t *dstPixels, const Rect &rect)
{
    for (int32_t i = 0; i < rect.height; i++) {
        memcpy_s(dstPixels + i * rect.width, rect.width,
                 srcPixels + (rect.top + i) * size.width + rect.left, rect.width);
    }
    const uint8_t *line = srcPixels + size.height * size.width + rect.left;
    for (int32_t i = 0; i < (rect.height + 1) / NUM_2; ++i) {
        memcpy_s(dstPixels + rect.width * rect.height + i * ((((rect.width + 1) / NUM_2) * NUM_2)),
                 ((rect.width + 1) / NUM_2) * NUM_2,
                 line + (rect.top + i) * ((size.width + 1) / NUM_2) * NUM_2,
                 ((rect.width + 1) / NUM_2) * NUM_2);
    }
    return true;
}

} // namespace Media
} // namespace OHOS