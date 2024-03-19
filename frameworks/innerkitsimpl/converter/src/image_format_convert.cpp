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
#include "pixel_yuv.h"
#include "image_source.h"
#include "image_log.h"
#include "hilog/log.h"
#include "media_errors.h"
#include "log_tags.h"
#include <memory>
#include <map>

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

static const std::map<std::pair<PixelFormat, PixelFormat>, ConvertFunction> g_cvtFuncMap = {
    {std::make_pair(PixelFormat::RGB_565, PixelFormat::NV21), RGB565ToNV21},
    {std::make_pair(PixelFormat::RGB_565, PixelFormat::NV12), RGB565ToNV12},
    {std::make_pair(PixelFormat::RGBA_8888, PixelFormat::NV21), RGBAToNV21},
    {std::make_pair(PixelFormat::RGBA_8888, PixelFormat::NV12), RGBAToNV12},
    {std::make_pair(PixelFormat::BGRA_8888, PixelFormat::NV21), BGRAToNV21},
    {std::make_pair(PixelFormat::BGRA_8888, PixelFormat::NV12), BGRAToNV12},
    {std::make_pair(PixelFormat::RGB_888, PixelFormat::NV21), RGBToNV21},
    {std::make_pair(PixelFormat::RGB_888, PixelFormat::NV12), RGBToNV12},
    {std::make_pair(PixelFormat::RGBA_F16, PixelFormat::NV21), RGBAF16ToNV21},
    {std::make_pair(PixelFormat::RGBA_F16, PixelFormat::NV12), RGBAF16ToNV12},
    {std::make_pair(PixelFormat::NV21, PixelFormat::RGB_565), NV21ToRGB565},
    {std::make_pair(PixelFormat::NV21, PixelFormat::RGBA_8888), NV21ToRGBA},
    {std::make_pair(PixelFormat::NV21, PixelFormat::BGRA_8888), NV21ToBGRA},
    {std::make_pair(PixelFormat::NV21, PixelFormat::RGB_888), NV21ToRGB},
    {std::make_pair(PixelFormat::NV21, PixelFormat::RGBA_F16), NV21ToRGBAF16},
    {std::make_pair(PixelFormat::NV21, PixelFormat::NV12), NV21ToNV12},
    {std::make_pair(PixelFormat::NV12, PixelFormat::RGB_565), NV12ToRGB565},
    {std::make_pair(PixelFormat::NV12, PixelFormat::RGBA_8888), NV12ToRGBA},
    {std::make_pair(PixelFormat::NV12, PixelFormat::BGRA_8888), NV12ToBGRA},
    {std::make_pair(PixelFormat::NV12, PixelFormat::RGB_888), NV12ToRGB},
    {std::make_pair(PixelFormat::NV12, PixelFormat::RGBA_F16), NV12ToRGBAF16},
    {std::make_pair(PixelFormat::NV12, PixelFormat::NV21), NV12ToNV21}
};

uint32_t ImageFormatConvert::ConvertImageFormat(const ConvertDataInfo &srcDataInfo, ConvertDataInfo &destDataInfo)
{
    if (!CheckConvertDataInfo(srcDataInfo)) {
        IMAGE_LOGD("source convert data info is invalid");
        return ERR_IMAGE_INVALID_PARAMETER;
    }

    if (!IsSupport(destDataInfo.pixelFormat)) {
        IMAGE_LOGD("destination format is not support or invalid");
        return ERR_IMAGE_INVALID_PARAMETER;
    }

    ConvertFunction cvtFunc = GetConvertFuncByFormat(srcDataInfo.pixelFormat, destDataInfo.pixelFormat);
    if (cvtFunc == nullptr) {
        IMAGE_LOGD("get convert function by format failed!");
        return ERR_IMAGE_INVALID_PARAMETER;
    }

    if (!cvtFunc(srcDataInfo.buffer, srcDataInfo.imageSize, &destDataInfo.buffer,
                 destDataInfo.bufferSize, srcDataInfo.colorSpace)) {
        IMAGE_LOGD("format convert failed!");
        return IMAGE_RESULT_FORMAT_CONVERT_FAILED;
    }
    destDataInfo.imageSize = srcDataInfo.imageSize;
    destDataInfo.colorSpace = srcDataInfo.colorSpace;

    return SUCCESS;
}

uint32_t ImageFormatConvert::ConvertImageFormat(const std::shared_ptr<PixelMap> srcPiexlMap,
    std::shared_ptr<PixelMap> &destPiexlMap, PixelFormat destFormat)
{
    if (srcPiexlMap == nullptr) {
        IMAGE_LOGD("source pixel map is null");
        return ERR_IMAGE_INVALID_PARAMETER;
    }

    if (!IsSupport(destFormat)) {
        IMAGE_LOGD("destination format not support");
        return ERR_MEDIA_FORMAT_UNSUPPORT;
    }

    PixelFormat srcFormat = srcPiexlMap->GetPixelFormat();
    ConvertFunction cvtFunc = GetConvertFuncByFormat(srcFormat, destFormat);
    if (cvtFunc == nullptr) {
        IMAGE_LOGD("get convert function by format failed!");
        return ERR_IMAGE_INVALID_PARAMETER;
    }

    const_uint8_buffer_type srcBuffer = srcPiexlMap->GetPixels();
    Size size = {srcPiexlMap->GetWidth(), srcPiexlMap->GetHeight()};
    uint8_buffer_type destBuffer = nullptr;
    size_t destBufferSize = 0;
    if (!cvtFunc(srcBuffer, size, &destBuffer, destBufferSize, srcPiexlMap->GetColorSpace())) {
        IMAGE_LOGD("format convert failed!");
        return IMAGE_RESULT_FORMAT_CONVERT_FAILED;
    }

    ImageInfo srcInfo;
    srcPiexlMap->GetImageInfo(srcInfo);
    ImageInfo destInfo;
    destInfo.alphaType = srcInfo.alphaType;
    destInfo.baseDensity = srcInfo.baseDensity;
    destInfo.colorSpace = srcInfo.colorSpace;
    destInfo.pixelFormat = destFormat;
    destInfo.size = srcInfo.size;
    if (!MakeDestPixelMap(destPiexlMap, destBuffer, destBufferSize, destInfo, srcPiexlMap->GetAllocatorType())) {
        IMAGE_LOGD("create pixel map failed");
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
        IMAGE_LOGD("buffer is null");
        return false;
    }

    if (!IsSupport(convertDataInfo.pixelFormat)) {
        IMAGE_LOGD("format is not support or invalid");
        return false;
    }

    if (!IsValidSize(convertDataInfo.imageSize)) {
        IMAGE_LOGD("image size is invalid");
        return false;
    }

    if (GetBufferSizeByFormat(convertDataInfo.pixelFormat, convertDataInfo.imageSize) != convertDataInfo.bufferSize) {
        IMAGE_LOGD("buffer size is wrong");
        return false;
    }

    return true;
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
        IMAGE_LOGD("current format is not supported or format is wrong");
        return nullptr;
    }
    return iter->second;
}

bool ImageFormatConvert::MakeDestPixelMap(std::shared_ptr<PixelMap> &destPixelMap, uint8_buffer_type destBuffer,
                                          const size_t destBufferSize, ImageInfo &info, AllocatorType allcatorType)
{
    std::unique_ptr<PixelMap> pixelMap;
    if (info.pixelFormat == PixelFormat::NV21 || info.pixelFormat == PixelFormat::NV12) {
        pixelMap = std::make_unique<PixelYUV>();
    } else {
        pixelMap = std::make_unique<PixelMap>();
    }
    pixelMap->SetPixelsAddr(destBuffer, nullptr, destBufferSize, allcatorType, nullptr);
    if (pixelMap->SetImageInfo(info) != SUCCESS) {
        IMAGE_LOGD("set imageInfo failed");
        return false;
    }
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