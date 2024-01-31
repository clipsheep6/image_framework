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
#include "hilog/log.h"
#include "media_errors.h"
#include "log_tags.h"

namespace {
    constexpr uint8_t NUM_0 = 0;
    constexpr uint8_t NUM_1 = 1;
    constexpr uint8_t NUM_2 = 2;
    constexpr uint8_t NUM_3 = 3;
    constexpr uint8_t NUM_4 = 4;
    // constexpr uint8_t NUM_5 = 5;
    // constexpr uint8_t NUM_6 = 6;
    constexpr uint8_t NUM_8 = 8;
}

namespace OHOS {
namespace Media {
using namespace OHOS::HiviewDFX;
static constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, LOG_TAG_DOMAIN_ID_IMAGE, "ImageFormatConvert" };

const size_t MAX_CONVERT_FUNC_NUM = 16;

static const ConvertFunction g_convertFuncMap[MAX_CONVERT_FUNC_NUM][MAX_CONVERT_FUNC_NUM] = {
    {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
     nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr},
    {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
     nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr},
    {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
     RGB565ToNV21, RGB565ToNV12, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr},
    {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
     RGBAToNV21, RGBAToNV12, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr},
    {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
     BGRAToNV21, BGRAToNV12, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr},
    {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
     RGBToNV21, RGBToNV12, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr},
    {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
     nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr},
    {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
     RGBAF16ToNV21, RGBAF16ToNV12, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr},
    {nullptr, nullptr, NV21ToRGB565, NV21ToRGBA, NV21ToBGRA, NV21ToRGB, nullptr, NV21ToRGBAF16,
     nullptr, NV21ToNV12, nullptr, nullptr, nullptr, nullptr, NV21ToYU12, NV21ToYV12},
    {nullptr, nullptr, NV12ToRGB565, NV12ToRGBA, NV12ToBGRA, NV12ToRGB, nullptr, NV12ToRGBAF16,
     NV12ToNV21, nullptr, nullptr, nullptr, nullptr, nullptr, NV12ToYU12, NV12ToYV12},
    {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
     nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr},
    {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
     nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr},
    {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
     nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr},
    {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
     nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr},
    {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
     YU12ToNV21, YU12ToNV12, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr},
    {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
     YV12ToNV21, YV12ToNV12, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}
};

ImageFormatConvert::ImageFormatConvert() {
    srcBuffer_ = nullptr;
    destBuffer_ = nullptr;
    destBufferRefCnt_ = 0;
    imageSize_ = {0, 0};
    srcFormat_ = PixelFormat::UNKNOWN;
    destFormat_ = PixelFormat::UNKNOWN;
    colorSpace_ = ColorSpace::UNKNOWN;
    srcPixelMap_ = nullptr;
    cvtFunc_ = nullptr;
}

ImageFormatConvert::ImageFormatConvert(const_uint8_buffer_type srcBuffer, const size_t srcBufferSize,
                                       const int32_t width, const int32_t height, PixelFormat srcFormat)
{
    srcBuffer_ = srcBuffer;
    srcBufferSize_ = srcBufferSize;
    destBuffer_ = nullptr;
    destBufferRefCnt_ = 0;
    imageSize_ = {width, height};
    srcFormat_ = srcFormat;
    destFormat_ = PixelFormat::RGBA_8888;
    colorSpace_ = ColorSpace::SRGB;
    srcPixelMap_ = nullptr;
    cvtFunc_ = nullptr;
}

ImageFormatConvert::ImageFormatConvert(const_uint8_buffer_type srcBuffer, const size_t srcBufferSize, const Size &size,
                                       PixelFormat srcFormat, PixelFormat destFormat)
{
    srcBuffer_ = srcBuffer;
    srcBufferSize_ = srcBufferSize;
    destBuffer_ = nullptr;
    destBufferRefCnt_ = 0;
    imageSize_ = size;
    srcFormat_ = srcFormat;
    destFormat_ = destFormat;
    colorSpace_ = ColorSpace::SRGB;
    srcPixelMap_ = nullptr;
    cvtFunc_ = nullptr;
}

ImageFormatConvert::ImageFormatConvert(const ConvertDataInfo &srcDataInfo, const Size &size,
                                       PixelFormat destPixelFormat, ColorSpace colorSpace)
{
    srcBuffer_ = srcDataInfo.buffer;
    srcBufferSize_ = srcDataInfo.bufferSize;
    destBuffer_ = nullptr;
    destBufferSize_ = 0;
    destBufferRefCnt_ = 0;
    imageSize_ = size;
    srcFormat_ = srcDataInfo.pixelFormat;
    destFormat_ = destPixelFormat;
    colorSpace_ = colorSpace;
    srcPixelMap_ = nullptr;
    cvtFunc_ = nullptr;
}



ImageFormatConvert::ImageFormatConvert(const_uint8_buffer_type srcBuffer, const size_t srcBufferSize,
                                       const Size &size, ColorSpace colorSpace, ConvertFunction cvtFunc)
{
    srcBuffer_ = srcBuffer;
    srcBufferSize_ = srcBufferSize;
    destBuffer_ = nullptr;
    destBufferRefCnt_ = 0;
    imageSize_ = size;
    srcFormat_ = PixelFormat::UNKNOWN;
    destFormat_ = PixelFormat::UNKNOWN;
    colorSpace_ = colorSpace;
    srcPixelMap_ = nullptr;
    cvtFunc_ = cvtFunc;
}

ImageFormatConvert::ImageFormatConvert(const std::shared_ptr<PixelMap> srcPiexlMap, PixelFormat destFormat)
{
    srcBuffer_ = nullptr;
    destBuffer_ = nullptr;
    destBufferRefCnt_ = 0;
    imageSize_ = {0, 0};
    srcFormat_ = PixelFormat::UNKNOWN;
    destFormat_ = destFormat;
    colorSpace_ = ColorSpace::SRGB;
    srcPixelMap_ = srcPiexlMap;
    cvtFunc_ = nullptr;
    ReadPixelMap();
}

ImageFormatConvert::~ImageFormatConvert()
{
    srcBuffer_ = nullptr;
    cvtFunc_ = nullptr;
    if (destBuffer_ != nullptr && destBufferRefCnt_ <= NUM_0) {
        delete[] destBuffer_;
        destBuffer_ = nullptr;
    }
}

bool ImageFormatConvert::SetSourceBuffer(const_uint8_buffer_type srcBuffer, const size_t bufferSize,
                                         const int32_t width, const int32_t height, PixelFormat format)
{
    if (!IsValidSize(width, height) || !CheckBufferInfo(srcBuffer, bufferSize, format, width, height)) {
        HiLog::Error(LABEL, "parameter is invalid");
        return false;
    }
    srcBuffer_ = srcBuffer;
    srcBufferSize_ = bufferSize;
    imageSize_ = {width, height};
    srcFormat_ = format;
    return true;
}

bool ImageFormatConvert::SetSourceBuffer(const_uint8_buffer_type srcBuffer, const size_t bufferSize,
                                         const Size &size, PixelFormat format)
{
    if (!IsValidSize(size) || !CheckBufferInfo(srcBuffer, bufferSize, format, size.width, size.height)) {
        HiLog::Error(LABEL, "parameter is invalid");
        return false;
    }
    srcBuffer_ = srcBuffer;
    srcBufferSize_ = bufferSize;
    imageSize_ = size;
    srcFormat_ = format;
    return true;
}

bool ImageFormatConvert::SetSourceBuffer(const ConvertDataInfo &srcDataInfo, const Size &size)
{
    if (!IsValidSize(size) || !CheckBufferInfo(srcDataInfo.buffer, srcDataInfo.bufferSize,
        srcDataInfo.pixelFormat, size.width, size.height)) {
        HiLog::Error(LABEL, "parameter is invalid");
        return false;
    }
    srcBuffer_ = srcDataInfo.buffer;
    srcBufferSize_ = srcDataInfo.bufferSize;
    srcFormat_ = srcDataInfo.pixelFormat;
    imageSize_ = size;
    return true;
}

bool ImageFormatConvert::SetSourcePixelMap(const std::shared_ptr<PixelMap> srcPiexlMap)
{
    if (srcPiexlMap == nullptr) {
        HiLog::Error(LABEL, "parameter is invalid");
        return false;
    }
    srcPixelMap_ = srcPiexlMap;
    ReadPixelMap();
    return true;
}

bool ImageFormatConvert::SetSize(const int32_t width, const int32_t height)
{
    if (!IsValidSize(width, height)) {
        HiLog::Error(LABEL, "parameter is invalid");
        return false;
    }
    imageSize_.width = width;
    imageSize_.height= height;
    return true;
}

bool ImageFormatConvert::SetSize(const Size &size)
{
    if (!IsValidSize(size)) {
        HiLog::Error(LABEL, "parameter is invalid");
        return false;
    }
    imageSize_ = size;
    return true;
}

bool ImageFormatConvert::SetSourceFormat(PixelFormat srcFormat)
{
    if(!IsSupport(srcFormat)) {
        HiLog::Error(LABEL, "source format is unsupported");
        return false;
    }
    srcFormat_ = srcFormat;
    return true;
}

bool ImageFormatConvert::SetDestinationFormat(PixelFormat destFormat)
{
    if(!IsSupport(destFormat)) {
        HiLog::Error(LABEL, "destination format is unsupported");
        return false;
    }
    destFormat_ = destFormat;
    return true;
}

bool ImageFormatConvert::SetColorSapace(ColorSpace colorSpace)
{
    if (!IsValidColorSpace(colorSpace)) {
        HiLog::Error(LABEL, "color space is invalid");
        return false;
    }
    colorSpace_ = colorSpace;
    return true;
}

bool ImageFormatConvert::SetConvertFunction(ConvertFunction cvtFunc)
{
    if (cvtFunc == nullptr) {
        HiLog::Error(LABEL, "convert function pointer is null!");
        return false;
    }
    cvtFunc_ = cvtFunc;
    return true;
}

bool ImageFormatConvert::SetConvertFunction(PixelFormat srcFormat, PixelFormat destFormat)
{
    if (!(IsSupport(srcFormat) && IsSupport(destFormat))) {
        HiLog::Error(LABEL, "Image format is invalid!");
        return false;
    }

    srcFormat_ = srcFormat;
    destFormat_ = destFormat;
    uint32_t row = static_cast<uint32_t>(srcFormat);
    uint32_t column = static_cast<uint32_t>(destFormat);
    cvtFunc_ = g_convertFuncMap[row][column];

    if (cvtFunc_ == nullptr) {
        HiLog::Error(LABEL, "current format is the same or not support");
        return false;
    }
    return true;
}

uint8_buffer_type ImageFormatConvert::GetDestinationBuffer()
{
    ++destBufferRefCnt_;
    return destBuffer_;
}

uint32_t ImageFormatConvert::ConvertImageFormat(uint8_buffer_type &destBuffer, size_t &destBufferSize)
{
    if (!IsValidSize(imageSize_)) {
        HiLog::Error(LABEL, "image size is invalid");
        return ERR_IMAGE_INVALID_PARAMETER;
    }
    if (!IsValidColorSpace(colorSpace_)) {
        HiLog::Error(LABEL, "color space is invalid");
        return ERR_IMAGE_INVALID_PARAMETER;
    }
    if (!CheckBufferInfo(srcBuffer_, srcBufferSize_, srcFormat_, imageSize_.width, imageSize_.height)) {
        HiLog::Error(LABEL, "source data is invalid");
        return ERR_IMAGE_SOURCE_DATA_INCOMPLETE;
    }
    if (cvtFunc_ == nullptr) {
        if (!GetConvertFuncByFormat()) {
            HiLog::Error(LABEL, "get convert function by format failed!");
            return ERR_IMAGE_INVALID_PARAMETER;
        }
    }

    if (!cvtFunc_(srcBuffer_, imageSize_, &destBuffer_, destBufferSize_, colorSpace_)) {
        if (destBuffer_) {
            ReleaseBuffer(destBuffer_);
        }
        HiLog::Error(LABEL, "format convert failed!");
        return IMAGE_RESULT_FORMAT_CONVERT_FAILED;
    }
    destBufferSize = destBufferSize_;
    destBuffer = destBuffer_;
    ++destBufferRefCnt_;

    return SUCCESS;
}

uint32_t ImageFormatConvert::ConvertImageFormat(std::unique_ptr<PixelMap> &destPixelMap)
{
    if (!IsSupportPixelMap(destFormat_)) {
        HiLog::Error(LABEL, "destination format not support PixelMap");
        return ERR_MEDIA_FORMAT_UNSUPPORT;
    }

    uint8_buffer_type destBuffer = nullptr;
    size_t bufferSize = 0;

    if (!ConvertImageFormat(destBuffer, bufferSize)) {
        return IMAGE_RESULT_FORMAT_CONVERT_FAILED;
    }

    if (!MakeDestPixelMap(destPixelMap, destBuffer, bufferSize)) {
        --destBufferRefCnt_;
        HiLog::Error(LABEL, "create pixel map failed");
        return ERR_IMAGE_PIXELMAP_CREATE_FAILED;
    }
    --destBufferRefCnt_;
    return SUCCESS;
}

bool ImageFormatConvert::IsValidSize(int32_t width, int32_t height)
{
    return width > 0 && height > 0;
}

bool ImageFormatConvert::IsValidSize(const Size &size)
{
    return IsValidSize(size.width, size.height);
}

bool ImageFormatConvert::IsValidColorSpace(ColorSpace colorSpace)
{
    return colorSpace >= ColorSpace::DISPLAY_P3 && colorSpace <= ColorSpace::SMPTE_C;
}

bool ImageFormatConvert::CheckBufferInfo(const_uint8_buffer_type buffer, const size_t bufferSize,
                                         PixelFormat format, uint32_t width, uint32_t height)
{
    if (buffer == nullptr) {
        HiLog::Error(LABEL, "source buffer is null");
        return false;
    }

    if (!IsSupport(format)) {
        HiLog::Error(LABEL, "format is not support or invalid");
        return false;
    }

    if (GetBufferSizeByFormat(format, width, height) != bufferSize) {
        HiLog::Error(LABEL, "buffer size is wrong");
        return false;
    }

    return true;
}

size_t ImageFormatConvert::GetBufferSizeByFormat(PixelFormat format, int32_t width, int32_t height)
{
    switch (format) {
        
        case PixelFormat::RGB_565:{
            return width * height * NUM_2;
        }
        case PixelFormat::RGB_888:{
            return width * height * NUM_3;
        }
        case PixelFormat::ARGB_8888:
        case PixelFormat::RGBA_8888:
        case PixelFormat::BGRA_8888:{
            return width * height * NUM_4;
        }
        case PixelFormat::RGBA_F16:{
            return width * height * NUM_8;
        }
        case PixelFormat::NV21:
        case PixelFormat::NV12:
        case PixelFormat::YU12:
        case PixelFormat::YV12:{
            return width * height + ((width + NUM_1) / NUM_2) *
                ((height + NUM_1) / NUM_2) * NUM_2;
        }
        default:{
            return NUM_0;
        }
    }
}

bool ImageFormatConvert::GetConvertFuncByFormat()
{
    if (!(IsSupport(srcFormat_) && IsSupport(destFormat_))) {
        HiLog::Error(LABEL, "Image format is invalid!");
        return false;
    }

    uint32_t row = static_cast<uint32_t>(srcFormat_);
    uint32_t column = static_cast<uint32_t>(destFormat_);
    cvtFunc_ = g_convertFuncMap[row][column];
    if (!cvtFunc_) {
        HiLog::Error(LABEL, "current format is not supported or format is wrong");
        return false;
    }
    return true;
}

void ImageFormatConvert::ReleaseBuffer(uint8_buffer_type buffer)
{
    if (buffer != nullptr) {
        delete[] buffer;
        buffer = nullptr;
    }
}

void ImageFormatConvert::ReadPixelMap()
{
    if (srcPixelMap_ == nullptr) {
        HiLog::Error(LABEL, "source pixel map is null");
        return;
    }

    int32_t width = srcPixelMap_->GetWidth();
    int32_t height = srcPixelMap_->GetHeight();
    imageSize_ = {width, height};
    colorSpace_ = srcPixelMap_->GetColorSpace();
    srcBuffer_ = srcPixelMap_->GetPixels();
    srcFormat_ = srcPixelMap_->GetPixelFormat();
}

bool ImageFormatConvert::MakeDestPixelMap(std::unique_ptr<PixelMap> &destPixelMap, uint8_buffer_type destBuffer,
                                          size_t destBufferSize)
{
    InitializationOptions opts;
    opts.srcPixelFormat = destFormat_;
    opts.pixelFormat = destFormat_;
    opts.size = imageSize_;
    if (srcPixelMap_ != nullptr) {
        opts.alphaType = srcPixelMap_->GetAlphaType();
        opts.editable = srcPixelMap_->IsEditable();
    }

    auto destPixelMapUnique = PixelMap::Create(reinterpret_cast<uint32_t*>(destBuffer),
                                               static_cast<uint32_t>(destBufferSize), opts);
    if (destPixelMapUnique == nullptr) {
        HiLog::Error(LABEL, "create pixel map failed");
        return false;
    }

    ImageInfo srcInfo;
    ImageInfo destInfo;
    if (srcPixelMap_ != nullptr) {
        srcPixelMap_->GetImageInfo(srcInfo);
        destInfo.alphaType = srcInfo.alphaType;
        destInfo.stride = srcInfo.stride;
        destInfo.baseDensity = srcInfo.baseDensity;
    }
    destInfo.colorSpace = colorSpace_;
    destInfo.pixelFormat = destFormat_;
    destInfo.size = imageSize_;
    if (destPixelMapUnique->SetImageInfo(destInfo) != SUCCESS) {
        HiLog::Error(LABEL, "set imageInfo failed");
        return false;
    }
    destPixelMap = std::move(destPixelMapUnique);
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
        case PixelFormat::NV12:
        case PixelFormat::YU12:
        case PixelFormat::YV12:{
            return true;
        }
        default:{
            return false;
        }
    }
}

bool ImageFormatConvert::IsSupportPixelMap(PixelFormat format)
{
    switch (format) {
        case PixelFormat::ARGB_8888:
        case PixelFormat::RGB_565:
        case PixelFormat::RGBA_8888:
        case PixelFormat::BGRA_8888:
        case PixelFormat::RGB_888:
        case PixelFormat::RGBA_F16:
        case PixelFormat::NV21:
        case PixelFormat::NV12:
        case PixelFormat::YU12:
        case PixelFormat::YV12:{
            return true;
        }
        default:{
            return false;
        }
    }
}

} // namespace Media
} // namespace OHOS