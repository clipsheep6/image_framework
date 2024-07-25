/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

#include "image_format_convert.h"

#include <map>
#include <memory>
#include "hilog/log.h"
#include "image_format_convert_ext_utils.h"
#include "image_log.h"
#include "image_source.h"
#include "log_tags.h"
#include "media_errors.h"
#include "pixel_yuv.h"
#if !defined(_WIN32) && !defined(_APPLE) && !defined(IOS_PLATFORM) && !defined(ANDROID_PLATFORM)
#include "v1_0/buffer_handle_meta_key_type.h"
#include "v1_0/cm_color_space.h"
#include "v1_0/hdr_static_metadata.h"
#include "surface_buffer.h"
#endif

namespace {
    constexpr uint8_t NUM_0 = 0;
    constexpr uint8_t NUM_1 = 1;
    constexpr uint8_t NUM_2 = 2;
    constexpr uint8_t NUM_3 = 3;
    constexpr uint8_t NUM_4 = 4;
    constexpr uint8_t NUM_8 = 8;
}

#undef LOG_DOMAIN
#define LOG_DOMAIN LOG_TAG_DOMAIN_ID_IMAGE

#undef LOG_TAG
#define LOG_TAG "ImageFormatConvert"

namespace OHOS {
namespace Media {

#if !defined(_WIN32) && !defined(_APPLE) && !defined(IOS_PLATFORM) && !defined(ANDROID_PLATFORM)
using namespace HDI::Display::Graphic::Common::V1_0;
#endif

static const std::map<std::pair<PixelFormat, PixelFormat>, ConvertFunctionNew> g_cvtFuncMapNew = []() {
#ifndef EXT_PIXEL
    static const std::map<std::pair<PixelFormat, PixelFormat>, ConvertFunctionNew> g_cvtFuncMapNew = {
        {std::make_pair(PixelFormat::RGB_565, PixelFormat::NV21), ImageFormatConvertUtils::RGB565ToNV21New},
        {std::make_pair(PixelFormat::RGB_565, PixelFormat::NV12), ImageFormatConvertUtils::RGB565ToNV12New},
        {std::make_pair(PixelFormat::RGBA_8888, PixelFormat::NV21), ImageFormatConvertUtils::RGBAToNV21New},
        {std::make_pair(PixelFormat::RGBA_8888, PixelFormat::NV12), ImageFormatConvertUtils::RGBAToNV12New},
        {std::make_pair(PixelFormat::BGRA_8888, PixelFormat::NV21), ImageFormatConvertUtils::BGRAToNV21New},
        {std::make_pair(PixelFormat::BGRA_8888, PixelFormat::NV12), ImageFormatConvertUtils::BGRAToNV12New},
        {std::make_pair(PixelFormat::RGB_888, PixelFormat::NV21), ImageFormatConvertUtils::RGBToNV21New},
        {std::make_pair(PixelFormat::RGB_888, PixelFormat::NV12), ImageFormatConvertUtils::RGBToNV12New},
        {std::make_pair(PixelFormat::RGBA_F16, PixelFormat::NV21), ImageFormatConvertUtils::RGBAF16ToNV21New},
        {std::make_pair(PixelFormat::RGBA_F16, PixelFormat::NV12), ImageFormatConvertUtils::RGBAF16ToNV12New},
    };
#else
    static const std::map<std::pair<PixelFormat, PixelFormat>, ConvertFunction> g_cvtFuncMapNew = {
        {std::make_pair(PixelFormat::RGB_565, PixelFormat::NV21), ImageFormatConvertExtUtils::RGB565ToNV21New},
        {std::make_pair(PixelFormat::RGB_565, PixelFormat::NV12), ImageFormatConvertExtUtils::RGB565ToNV12New},
        {std::make_pair(PixelFormat::RGBA_8888, PixelFormat::NV21), ImageFormatConvertExtUtils::RGBAToNV21New},
        {std::make_pair(PixelFormat::RGBA_8888, PixelFormat::NV12), ImageFormatConvertExtUtils::RGBAToNV12New},
        {std::make_pair(PixelFormat::BGRA_8888, PixelFormat::NV21), ImageFormatConvertExtUtils::BGRAToNV21New},
        {std::make_pair(PixelFormat::BGRA_8888, PixelFormat::NV12), ImageFormatConvertExtUtils::BGRAToNV12New},
        {std::make_pair(PixelFormat::RGB_888, PixelFormat::NV21), ImageFormatConvertExtUtils::RGBToNV21New},
        {std::make_pair(PixelFormat::RGB_888, PixelFormat::NV12), ImageFormatConvertExtUtils::RGBToNV12New},
        {std::make_pair(PixelFormat::RGBA_F16, PixelFormat::NV21), ImageFormatConvertUtils::RGBAF16ToNV21New},
        {std::make_pair(PixelFormat::RGBA_F16, PixelFormat::NV12), ImageFormatConvertUtils::RGBAF16ToNV12New},
    };
#endif
    return g_cvtFuncMapNew;
}();

static const std::map<std::pair<PixelFormat, PixelFormat>, ConvertFunction> g_cvtFuncMap = []() {
#ifndef EXT_PIXEL
    static const std::map<std::pair<PixelFormat, PixelFormat>, ConvertFunction> cvtFuncMap = {
        {std::make_pair(PixelFormat::RGB_565, PixelFormat::NV21), ImageFormatConvertUtils::RGB565ToNV21},
        {std::make_pair(PixelFormat::RGB_565, PixelFormat::NV12), ImageFormatConvertUtils::RGB565ToNV12},
        {std::make_pair(PixelFormat::RGBA_8888, PixelFormat::NV21), ImageFormatConvertUtils::RGBAToNV21},
        {std::make_pair(PixelFormat::RGBA_8888, PixelFormat::NV12), ImageFormatConvertUtils::RGBAToNV12},
        {std::make_pair(PixelFormat::BGRA_8888, PixelFormat::NV21), ImageFormatConvertUtils::BGRAToNV21},
        {std::make_pair(PixelFormat::BGRA_8888, PixelFormat::NV12), ImageFormatConvertUtils::BGRAToNV12},
        {std::make_pair(PixelFormat::RGB_888, PixelFormat::NV21), ImageFormatConvertUtils::RGBToNV21},
        {std::make_pair(PixelFormat::RGB_888, PixelFormat::NV12), ImageFormatConvertUtils::RGBToNV12},
        {std::make_pair(PixelFormat::RGBA_F16, PixelFormat::NV21), ImageFormatConvertUtils::RGBAF16ToNV21},
        {std::make_pair(PixelFormat::RGBA_F16, PixelFormat::NV12), ImageFormatConvertUtils::RGBAF16ToNV12},
    };
#else
    static const std::map<std::pair<PixelFormat, PixelFormat>, ConvertFunction> cvtFuncMap = {
        {std::make_pair(PixelFormat::RGB_565, PixelFormat::NV21), ImageFormatConvertExtUtils::RGB565ToNV21},
        {std::make_pair(PixelFormat::RGB_565, PixelFormat::NV12), ImageFormatConvertExtUtils::RGB565ToNV12},
        {std::make_pair(PixelFormat::RGBA_8888, PixelFormat::NV21), ImageFormatConvertExtUtils::RGBAToNV21},
        {std::make_pair(PixelFormat::RGBA_8888, PixelFormat::NV12), ImageFormatConvertExtUtils::RGBAToNV12},
        {std::make_pair(PixelFormat::BGRA_8888, PixelFormat::NV21), ImageFormatConvertExtUtils::BGRAToNV21},
        {std::make_pair(PixelFormat::BGRA_8888, PixelFormat::NV12), ImageFormatConvertExtUtils::BGRAToNV12},
        {std::make_pair(PixelFormat::RGB_888, PixelFormat::NV21), ImageFormatConvertExtUtils::RGBToNV21},
        {std::make_pair(PixelFormat::RGB_888, PixelFormat::NV12), ImageFormatConvertExtUtils::RGBToNV12},
        {std::make_pair(PixelFormat::RGBA_F16, PixelFormat::NV21), ImageFormatConvertUtils::RGBAF16ToNV21},
        {std::make_pair(PixelFormat::RGBA_F16, PixelFormat::NV12), ImageFormatConvertUtils::RGBAF16ToNV12},
    };
#endif
    return cvtFuncMap;
}();

static const std::map<std::pair<PixelFormat, PixelFormat>, YUVConvertFunctionNew> g_yuvCvtFuncMapNew = []() {
#ifndef EXT_PIXEL
    std::map<std::pair<PixelFormat, PixelFormat>, YUVConvertFunctionNew> g_yuvCvtFuncMapNew = {
        {std::make_pair(PixelFormat::NV12, PixelFormat::NV21), ImageFormatConvertUtils::NV12ToNV21New},
        {std::make_pair(PixelFormat::NV12, PixelFormat::RGB_888), ImageFormatConvertUtils::NV12ToRGBNew},
        {std::make_pair(PixelFormat::NV12, PixelFormat::RGB_565), ImageFormatConvertUtils::NV12ToRGB565New},
        {std::make_pair(PixelFormat::NV12, PixelFormat::RGBA_F16), ImageFormatConvertUtils::NV12ToRGBAF16New},
        {std::make_pair(PixelFormat::NV12, PixelFormat::RGBA_8888), ImageFormatConvertUtils::NV12ToRGBANew},
        {std::make_pair(PixelFormat::NV12, PixelFormat::BGRA_8888), ImageFormatConvertUtils::NV12ToBGRANew},
        {std::make_pair(PixelFormat::NV21, PixelFormat::NV12), ImageFormatConvertUtils::NV21ToNV12New},
        {std::make_pair(PixelFormat::NV21, PixelFormat::RGB_888), ImageFormatConvertUtils::NV21ToRGBNew},
        {std::make_pair(PixelFormat::NV21, PixelFormat::RGB_565), ImageFormatConvertUtils::NV21ToRGB565New},
        {std::make_pair(PixelFormat::NV21, PixelFormat::RGBA_F16), ImageFormatConvertUtils::NV21ToRGBAF16New},
        {std::make_pair(PixelFormat::NV21, PixelFormat::RGBA_8888), ImageFormatConvertUtils::NV21ToRGBANew},
        {std::make_pair(PixelFormat::NV21, PixelFormat::BGRA_8888), ImageFormatConvertUtils::NV21ToBGRANew},
    };
#else
    std::map<std::pair<PixelFormat, PixelFormat>, YUVConvertFunctionNew> g_yuvCvtFuncMapNew = {
        {std::make_pair(PixelFormat::NV21, PixelFormat::RGB_888), ImageFormatConvertExtUtils::NV21ToRGBNew},
        {std::make_pair(PixelFormat::NV21, PixelFormat::RGBA_8888), ImageFormatConvertExtUtils::NV21ToRGBANew},
        {std::make_pair(PixelFormat::NV21, PixelFormat::BGRA_8888), ImageFormatConvertExtUtils::NV21ToBGRANew},
        {std::make_pair(PixelFormat::NV21, PixelFormat::RGB_565), ImageFormatConvertExtUtils::NV21ToRGB565New},
        {std::make_pair(PixelFormat::NV12, PixelFormat::RGB_565), ImageFormatConvertExtUtils::NV12ToRGB565New},
        {std::make_pair(PixelFormat::NV21, PixelFormat::NV12), ImageFormatConvertExtUtils::NV21ToNV12New},
        {std::make_pair(PixelFormat::NV21, PixelFormat::RGBA_F16), ImageFormatConvertUtils::NV21ToRGBAF16New},
        {std::make_pair(PixelFormat::NV12, PixelFormat::NV21), ImageFormatConvertExtUtils::NV12ToNV21New},
        {std::make_pair(PixelFormat::NV12, PixelFormat::RGBA_F16), ImageFormatConvertUtils::NV12ToRGBAF16New},
        {std::make_pair(PixelFormat::NV12, PixelFormat::RGBA_8888), ImageFormatConvertExtUtils::NV12ToRGBANew},
        {std::make_pair(PixelFormat::NV12, PixelFormat::BGRA_8888), ImageFormatConvertExtUtils::NV12ToBGRANew},
        {std::make_pair(PixelFormat::NV12, PixelFormat::RGB_888), ImageFormatConvertExtUtils::NV12ToRGBNew},
    };
#endif
    return g_yuvCvtFuncMapNew;
}();

static const std::map<std::pair<PixelFormat, PixelFormat>, YUVConvertFunction> g_yuvCvtFuncMap = []() {
#ifndef EXT_PIXEL
    std::map<std::pair<PixelFormat, PixelFormat>, YUVConvertFunction> yuvCvtFuncMap = {
        {std::make_pair(PixelFormat::NV12, PixelFormat::NV21), ImageFormatConvertUtils::NV12ToNV21},
        {std::make_pair(PixelFormat::NV12, PixelFormat::RGB_888), ImageFormatConvertUtils::NV12ToRGB},
        {std::make_pair(PixelFormat::NV12, PixelFormat::RGB_565), ImageFormatConvertUtils::NV12ToRGB565},
        {std::make_pair(PixelFormat::NV12, PixelFormat::RGBA_F16), ImageFormatConvertUtils::NV12ToRGBAF16},
        {std::make_pair(PixelFormat::NV12, PixelFormat::RGBA_8888), ImageFormatConvertUtils::NV12ToRGBA},
        {std::make_pair(PixelFormat::NV12, PixelFormat::BGRA_8888), ImageFormatConvertUtils::NV12ToBGRA},
        {std::make_pair(PixelFormat::NV21, PixelFormat::NV12), ImageFormatConvertUtils::NV21ToNV12},
        {std::make_pair(PixelFormat::NV21, PixelFormat::RGB_888), ImageFormatConvertUtils::NV21ToRGB},
        {std::make_pair(PixelFormat::NV21, PixelFormat::RGB_565), ImageFormatConvertUtils::NV21ToRGB565},
        {std::make_pair(PixelFormat::NV21, PixelFormat::RGBA_F16), ImageFormatConvertUtils::NV21ToRGBAF16},
        {std::make_pair(PixelFormat::NV21, PixelFormat::RGBA_8888), ImageFormatConvertUtils::NV21ToRGBA},
        {std::make_pair(PixelFormat::NV21, PixelFormat::BGRA_8888), ImageFormatConvertUtils::NV21ToBGRA},
    };
#else
    std::map<std::pair<PixelFormat, PixelFormat>, YUVConvertFunction> yuvCvtFuncMap = {
        {std::make_pair(PixelFormat::NV21, PixelFormat::RGB_888), ImageFormatConvertExtUtils::NV21ToRGB},
        {std::make_pair(PixelFormat::NV21, PixelFormat::RGBA_8888), ImageFormatConvertExtUtils::NV21ToRGBA},
        {std::make_pair(PixelFormat::NV21, PixelFormat::BGRA_8888), ImageFormatConvertExtUtils::NV21ToBGRA},
        {std::make_pair(PixelFormat::NV21, PixelFormat::RGB_565), ImageFormatConvertExtUtils::NV21ToRGB565},
        {std::make_pair(PixelFormat::NV12, PixelFormat::RGB_565), ImageFormatConvertExtUtils::NV12ToRGB565},
        {std::make_pair(PixelFormat::NV21, PixelFormat::NV12), ImageFormatConvertExtUtils::NV21ToNV12},
        {std::make_pair(PixelFormat::NV21, PixelFormat::RGBA_F16), ImageFormatConvertUtils::NV21ToRGBAF16},
        {std::make_pair(PixelFormat::NV12, PixelFormat::NV21), ImageFormatConvertExtUtils::NV12ToNV21},
        {std::make_pair(PixelFormat::NV12, PixelFormat::RGBA_F16), ImageFormatConvertUtils::NV12ToRGBAF16},
        {std::make_pair(PixelFormat::NV12, PixelFormat::RGBA_8888), ImageFormatConvertExtUtils::NV12ToRGBA},
        {std::make_pair(PixelFormat::NV12, PixelFormat::BGRA_8888), ImageFormatConvertExtUtils::NV12ToBGRA},
        {std::make_pair(PixelFormat::NV12, PixelFormat::RGB_888), ImageFormatConvertExtUtils::NV12ToRGB},
    };
#endif
    return yuvCvtFuncMap;
}();

uint32_t ImageFormatConvert::ConvertImageFormat(const ConvertDataInfo &srcDataInfo, ConvertDataInfo &destDataInfo)
{
    if (!CheckConvertDataInfo(srcDataInfo)) {
        IMAGE_LOGE("source convert data info is invalid");
        return ERR_IMAGE_INVALID_PARAMETER;
    }

    if (!IsSupport(destDataInfo.pixelFormat)) {
        IMAGE_LOGE("destination format is not support or invalid");
        return ERR_IMAGE_INVALID_PARAMETER;
    }

    ConvertFunction cvtFunc = GetConvertFuncByFormat(srcDataInfo.pixelFormat, destDataInfo.pixelFormat);
    if (cvtFunc == nullptr) {
        IMAGE_LOGE("get convert function by format failed!");
        return ERR_IMAGE_INVALID_PARAMETER;
    }

    uint32_t stride = NUM_4 * srcDataInfo.imageSize.width;
    RGBDataInfo RgbDataInfo = {srcDataInfo.imageSize.width, srcDataInfo.imageSize.height, stride};

    if (!cvtFunc(srcDataInfo.buffer, RgbDataInfo, &destDataInfo.buffer,
                 destDataInfo.bufferSize, srcDataInfo.colorSpace)) {
        IMAGE_LOGE("format convert failed!");
        return IMAGE_RESULT_FORMAT_CONVERT_FAILED;
    }
    destDataInfo.imageSize = srcDataInfo.imageSize;
    destDataInfo.colorSpace = srcDataInfo.colorSpace;

    return SUCCESS;
}

uint32_t ImageFormatConvert::ConvertImageFormat(std::shared_ptr<PixelMap> &srcPiexlMap, PixelFormat destFormat)
{
    IMAGE_LOGE("yyytest ConvertImageFormat start");
    if (srcPiexlMap == nullptr) {
        IMAGE_LOGE("source pixel map is null");
        return ERR_IMAGE_INVALID_PARAMETER;
    }

    if (!IsSupport(destFormat)) {
        IMAGE_LOGE("destination format not support");
        return ERR_MEDIA_FORMAT_UNSUPPORT;
    }

    PixelFormat srcFormat = srcPiexlMap->GetPixelFormat();
    if ((srcFormat == PixelFormat::NV21) || (srcFormat == PixelFormat::NV12)) {
        IMAGE_LOGE("yyytest ConvertImageFormat 1111");
        uint32_t ret = YUVConvertImageFormatOption(srcPiexlMap, srcFormat, destFormat);
        IMAGE_LOGD("convert yuv format ret = %{public}d!", ret);
        return ret;
    }

    IMAGE_LOGE("yyytest ConvertImageFormat 222");
    ConvertFunctionNew cvtFunc = GetConvertFuncByFormatNew(srcFormat, destFormat);
    if (cvtFunc == nullptr) {
        IMAGE_LOGE("get convert function by format failed!");
        return ERR_IMAGE_INVALID_PARAMETER;
    }

    const_uint8_buffer_type srcBuffer = srcPiexlMap->GetPixels();
    // uint32_t stride = NUM_4 * srcPiexlMap->GetWidth();
    // #if !defined(_WIN32) && !defined(_APPLE) && !defined(IOS_PLATFORM) && !defined(ANDROID_PLATFORM)
    // if (srcPiexlMap->GetAllocatorType() == AllocatorType::DMA_ALLOC) {
    //     auto sb = reinterpret_cast<SurfaceBuffer*>(srcPiexlMap->GetFd());
    //     stride = sb->GetStride();
    // }
    // #endif
    // RGBDataInfo srcDataInfo = {srcPiexlMap->GetWidth(), srcPiexlMap->GetHeight(), stride};
    //uint8_buffer_type destBuffer = nullptr;
    //size_t destBufferSize = 0;
    // if (!cvtFunc(srcBuffer, srcDataInfo, &destBuffer, destBufferSize, srcPiexlMap->GetColorSpace())) {
    //     IMAGE_LOGE("format convert failed!");
    //     return IMAGE_RESULT_FORMAT_CONVERT_FAILED;
    // }

    ImageInfo imageInfo;
    srcPiexlMap->GetImageInfo(imageInfo);

    int32_t width = imageInfo.size.width;
    int32_t height = imageInfo.size.height;
    YUVStrideInfo dstStrides;
    auto allocType = srcPiexlMap->GetAllocatorType();
    uint32_t pictureSize = GetBufferSizeByFormat(destFormat, imageInfo.size);

    auto m = CreateMemory(destFormat, pictureSize, allocType, width, height, dstStrides);
    if (m == nullptr) {
        IMAGE_LOGE("yyytest CreateMemory failed");
        return ERR_IMAGE_INVALID_PARAMETER;
    }
    uint32_t stride = srcPiexlMap->GetRowStride();
    #if !defined(_WIN32) && !defined(_APPLE) && !defined(IOS_PLATFORM) && !defined(ANDROID_PLATFORM)
    if (allocType == AllocatorType::DMA_ALLOC) {
        auto sb = reinterpret_cast<SurfaceBuffer*>(srcPiexlMap->GetFd());
        stride = sb->GetStride();
    }
    #endif
    RGBDataInfo rgbDataInfo = {width, height, stride};

    DstConvertDataInfo dstInfo;
    dstInfo.allocType = allocType;
    dstInfo.buffer = reinterpret_cast<uint8_t *>(m->data.data);
    dstInfo.bufferSize = pictureSize;
    dstInfo.yStride = dstStrides.yStride;
    dstInfo.uvStride = dstStrides.uvStride;
    dstInfo.yOffset = dstStrides.yOffset;
    dstInfo.uvOffset = dstStrides.uvOffset;

    if (!cvtFunc(srcBuffer, rgbDataInfo, dstInfo, srcPiexlMap->GetColorSpace())) {
       IMAGE_LOGE("yyytest format convert failed!");
       return IMAGE_RESULT_FORMAT_CONVERT_FAILED;
    }

    ImageInfo destInfo;
    destInfo.alphaType = imageInfo.alphaType;
    destInfo.baseDensity = imageInfo.baseDensity;
    destInfo.colorSpace = imageInfo.colorSpace;
    destInfo.pixelFormat = destFormat;
    destInfo.size = imageInfo.size;
    if (!MakeDestPixelMap(srcPiexlMap, dstInfo.buffer, dstInfo.bufferSize, destInfo, allocType, m->extend.data)) {
        IMAGE_LOGE("yyytest create pixel map failed");
        return ERR_IMAGE_PIXELMAP_CREATE_FAILED;
    }
    return SUCCESS;
}

bool ImageFormatConvert::IsValidSize(const Size &size)
{
    return size.width > 0 && size.height > 0;
}

bool ImageFormatConvert::CheckConvertDataInfo(const ConvertDataInfo &convertDataInfo)
{
    if (convertDataInfo.buffer == nullptr) {
        IMAGE_LOGE("buffer is null");
        return false;
    }

    if (!IsSupport(convertDataInfo.pixelFormat)) {
        IMAGE_LOGE("format is not support or invalid");
        return false;
    }

    if (!IsValidSize(convertDataInfo.imageSize)) {
        IMAGE_LOGE("image size is invalid");
        return false;
    }

    if (GetBufferSizeByFormat(convertDataInfo.pixelFormat, convertDataInfo.imageSize) != convertDataInfo.bufferSize) {
        IMAGE_LOGE("buffer size is wrong");
        return false;
    }

    return true;
}

std::unique_ptr<AbsMemory> ImageFormatConvert::CreateMemory(PixelFormat pixelFormat, uint32_t pictureSize,
    AllocatorType allocatorType, int32_t width, int32_t height, YUVStrideInfo &strides)
{
    MemoryData memoryData = {nullptr, pictureSize, "PixelConvert", {width, height}, pixelFormat};
    auto m = MemoryManager::CreateMemory(allocatorType, memoryData);
    if (m == nullptr) {
        IMAGE_LOGE("CreateMemory failed");
        return m;
    }
    IMAGE_LOGE("yyytest CreateMemory allocatorType: %{public}d", allocatorType);
    #if !defined(IOS_PLATFORM)&& !defined(ANDROID_PLATFORM)
    if (allocatorType == AllocatorType::DMA_ALLOC) {
        if (m->extend.data == nullptr) {
            IMAGE_LOGE("CreateMemory get surfacebuffer failed");
        } else {
            auto sb = reinterpret_cast<SurfaceBuffer*>(m->extend.data);
            OH_NativeBuffer_Planes *planes = nullptr;
            GSError retVal = sb->GetPlanesInfo(reinterpret_cast<void**>(&planes));
            if (retVal != OHOS::GSERROR_OK || planes == nullptr) {
                IMAGE_LOGE("CreateMemory Get planesInfo failed, retVal:%{public}d", retVal);
            } else if (planes->planeCount >= NUM_2) {
                if (pixelFormat == PixelFormat::NV21 || pixelFormat == PixelFormat::NV12) {
                    auto yStride = planes->planes[0].columnStride;
                    auto uvStride = planes->planes[1].columnStride;
                    auto yOffset = planes->planes[0].offset;
                    auto uvOffset = planes->planes[1].offset - 1;
                    strides = {yStride, uvStride, yOffset, uvOffset};
                } else {
                    auto yStride = planes->planes[0].columnStride;
                    auto yOffset = planes->planes[0].offset;
                    strides = {yStride, 0, yOffset, 0};
                }

            }
        }
    }
    #endif
    return m;
}

uint32_t ImageFormatConvert::YUVConvertImageFormatOption(std::shared_ptr<PixelMap> &srcPiexlMap,
                                                         const PixelFormat &srcFormat, PixelFormat destFormat)
{
    IMAGE_LOGE("yyytest YUVConvertImageFormatOption start 111");
    YUVConvertFunctionNew yuvCvtFunc = YUVGetConvertFuncByFormatNew(srcFormat, destFormat);
    if (yuvCvtFunc == nullptr) {
        IMAGE_LOGE("get convert function by format failed!");
        return ERR_IMAGE_INVALID_PARAMETER;
    }

    const_uint8_buffer_type data = srcPiexlMap->GetPixels();
    YUVDataInfo yDInfo;
    srcPiexlMap->GetImageYUVInfo(yDInfo);
    ImageInfo imageInfo;
    srcPiexlMap->GetImageInfo(imageInfo);
    if (srcFormat == PixelFormat::NV21 &&
        (yDInfo.yWidth == 0 || yDInfo.yHeight == 0 || yDInfo.uvWidth == 0 || yDInfo.uvHeight == 0)) {
        IMAGE_LOGE("info is invalid");
        yDInfo.yWidth = static_cast<uint32_t>(imageInfo.size.width);
        yDInfo.yHeight = static_cast<uint32_t>(imageInfo.size.height);
        yDInfo.uvWidth = static_cast<uint32_t>((imageInfo.size.width + 1) / NUM_2);
        yDInfo.uvHeight = static_cast<uint32_t>((imageInfo.size.height + 1) / NUM_2);
    }

    int32_t width = imageInfo.size.width;
    int32_t height = imageInfo.size.height;
    YUVStrideInfo dstStrides;
    auto allocType = srcPiexlMap->GetAllocatorType();
    uint32_t pictureSize = GetBufferSizeByFormat(destFormat, imageInfo.size);
    IMAGE_LOGE("yyytest YUVConvertImageFormatOption %{public}d", pictureSize);
    auto m = CreateMemory(destFormat, pictureSize, allocType, width, height, dstStrides);
    if (m == nullptr) {
        IMAGE_LOGE("yyytest CreateMemory failed");
        return ERR_IMAGE_INVALID_PARAMETER;
    }
    DstConvertDataInfo dstInfo;
    dstInfo.allocType = allocType;
    dstInfo.buffer = reinterpret_cast<uint8_t *>(m->data.data);
    dstInfo.bufferSize = pictureSize;
    dstInfo.yStride = dstStrides.yStride;
    dstInfo.uvStride = dstStrides.uvStride;
    dstInfo.yOffset = dstStrides.yOffset;
    dstInfo.uvOffset = dstStrides.uvOffset;

    // if (!yuvCvtFunc(data, yDInfo, &destBuffer, destBufferSize, srcPiexlMap->GetColorSpace())) {
    //     IMAGE_LOGE("format convert failed!");
    //     return IMAGE_RESULT_FORMAT_CONVERT_FAILED;
    // }

    if (!yuvCvtFunc(data, yDInfo, dstInfo, srcPiexlMap->GetColorSpace())) {
       IMAGE_LOGE("yyytest format convert failed!");
       return IMAGE_RESULT_FORMAT_CONVERT_FAILED;
    }

    ImageInfo dstImageInfo;
    dstImageInfo.alphaType = imageInfo.alphaType;
    dstImageInfo.baseDensity = imageInfo.baseDensity;
    dstImageInfo.colorSpace = imageInfo.colorSpace;
    dstImageInfo.pixelFormat = destFormat;
    dstImageInfo.size = imageInfo.size;
    if (!MakeDestPixelMap(srcPiexlMap, dstInfo.buffer, dstInfo.bufferSize, dstImageInfo, allocType, m->extend.data)) {
        IMAGE_LOGE("yyytest create pixel map failed");
        return ERR_IMAGE_PIXELMAP_CREATE_FAILED;
    }
    IMAGE_LOGE("yyytest create pixel map succ");
    return SUCCESS;
}

size_t ImageFormatConvert::GetBufferSizeByFormat(PixelFormat format, const Size &size)
{
    switch (format) {
        case PixelFormat::RGB_565:{
            return size.width * size.height * NUM_2;
        }
        case PixelFormat::RGB_888:{
            return size.width * size.height * NUM_3;
        }
        case PixelFormat::ARGB_8888:
        case PixelFormat::RGBA_8888:
        case PixelFormat::BGRA_8888:{
            return size.width * size.height * NUM_4;
        }
        case PixelFormat::RGBA_F16:{
            return size.width * size.height * NUM_8;
        }
        case PixelFormat::NV21:
        case PixelFormat::NV12:{
            return size.width * size.height + ((size.width + NUM_1) / NUM_2) *
                ((size.height + NUM_1) / NUM_2) * NUM_2;
        }
        default:{
            return NUM_0;
        }
    }
}

ConvertFunction ImageFormatConvert::GetConvertFuncByFormat(PixelFormat srcFormat, PixelFormat destFormat)
{
    auto iter = g_cvtFuncMap.find(std::make_pair(srcFormat, destFormat));
    if (iter == g_cvtFuncMap.end()) {
        IMAGE_LOGE("current format is not supported or format is wrong");
        return nullptr;
    }
    return iter->second;
}

ConvertFunctionNew ImageFormatConvert::GetConvertFuncByFormatNew(PixelFormat srcFormat, PixelFormat destFormat)
{
    auto iter = g_cvtFuncMapNew.find(std::make_pair(srcFormat, destFormat));
    if (iter == g_cvtFuncMapNew.end()) {
        IMAGE_LOGE("current format is not supported or format is wrong");
        return nullptr;
    }
    return iter->second;
}


YUVConvertFunction ImageFormatConvert::YUVGetConvertFuncByFormat(PixelFormat srcFormat, PixelFormat destFormat)
{
    auto iter = g_yuvCvtFuncMap.find(std::make_pair(srcFormat, destFormat));
    if (iter == g_yuvCvtFuncMap.end()) {
        IMAGE_LOGE("current format is not supported or format is wrong");
        return nullptr;
    }
    return iter->second;
}

YUVConvertFunctionNew ImageFormatConvert::YUVGetConvertFuncByFormatNew(PixelFormat srcFormat, PixelFormat destFormat)
{
    auto iter = g_yuvCvtFuncMapNew.find(std::make_pair(srcFormat, destFormat));
    if (iter == g_yuvCvtFuncMapNew.end()) {
        IMAGE_LOGE("current format is not supported or format is wrong");
        return nullptr;
    }
    return iter->second;
}

bool ImageFormatConvert::MakeDestPixelMap(std::shared_ptr<PixelMap> &destPixelMap, uint8_buffer_type destBuffer,
                                          const size_t destBufferSize, ImageInfo &info,
                                          AllocatorType allcatorType, void *context)
{
    std::unique_ptr<PixelMap> pixelMap;
    if (info.pixelFormat == PixelFormat::NV21 || info.pixelFormat == PixelFormat::NV12 ||
        info.pixelFormat == PixelFormat::YCBCR_P010 || info.pixelFormat == PixelFormat::YCRCB_P010) {
        pixelMap = std::make_unique<PixelYuv>();
        if (pixelMap == nullptr) {
            return false;
        }
        pixelMap->AssignYuvDataOnType(info.pixelFormat, info.size.width, info.size.height);
    } else {
        pixelMap = std::make_unique<PixelMap>();
    }
    if (pixelMap->SetImageInfo(info) != SUCCESS) {
        IMAGE_LOGE("set imageInfo failed");
        return false;
    }
    pixelMap->SetPixelsAddr(destBuffer, context, destBufferSize, allcatorType, nullptr);
    destPixelMap = std::move(pixelMap);
    return true;
}

bool ImageFormatConvert::IsSupport(PixelFormat format)
{
    switch (format) {
        case PixelFormat::ARGB_8888:
        case PixelFormat::RGB_565:
        case PixelFormat::RGBA_8888:
        case PixelFormat::BGRA_8888:
        case PixelFormat::RGB_888:
        case PixelFormat::RGBA_F16:
        case PixelFormat::NV21:
        case PixelFormat::NV12:{
            return true;
        }
        default:{
            return false;
        }
    }
}
} // namespace Media
} // namespace OHOS