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

#ifndef INTERFACES_INNERKITS_INCLUDE_IMAGE_FORMAT_CONVERT_H_
#define INTERFACES_INNERKITS_INCLUDE_IMAGE_FORMAT_CONVERT_H_

#include "pixel_map.h"
#include "image_format_convert_utils.h"
#include "image_source.h"
#include "image_plugin_type.h"
#include "image_type.h"
#include  <memory>

namespace OHOS {
namespace Media {
using uint8_buffer_type = uint8_t *;
using const_uint8_buffer_type = const uint8_t *;

struct ConvertDataInfo {
    uint8_buffer_type buffer;
    size_t bufferSize;
    PixelFormat pixelFormat = PixelFormat::UNKNOWN;
};

struct ImagePackerError {
    bool hasErrorCode;
    uint32_t errorCode;
    std::string msg;
};

class ImageFormatConvert
{
public:
    ImageFormatConvert();
    ImageFormatConvert(const_uint8_buffer_type srcBuffer, const size_t srcBufferSize,
                       const int32_t width, const int32_t height, PixelFormat srcFormat);
    ImageFormatConvert(const_uint8_buffer_type srcBuffer, const size_t srcBufferSize, const Size &size,
                       PixelFormat srcFormat, PixelFormat destFormat);
    ImageFormatConvert(const ConvertDataInfo &srcDataInfo, const Size &size, PixelFormat destPixelFormat,
                       ColorSpace colorSpace);
    ImageFormatConvert(const_uint8_buffer_type srcBuffer, const size_t srcBufferSize, const Size &size,
                       ColorSpace colorSpace, ConvertFunction cvtFunc = nullptr);
    ImageFormatConvert(const std::shared_ptr<PixelMap> srcPiexlMap, PixelFormat destFormat);
    ~ImageFormatConvert();

    bool SetSourceBuffer(const_uint8_buffer_type srcBuffer, const size_t bufferSize,
                         const int32_t width, const int32_t height, PixelFormat format);
    bool SetSourceBuffer(const_uint8_buffer_type srcBuffer, const size_t bufferSize,
                         const Size &size, PixelFormat format);
    bool SetSourceBuffer(const ConvertDataInfo &srcDataInfo, const Size &size);
    bool SetSourcePixelMap(const std::shared_ptr<PixelMap> srcPiexlMap);
    bool SetSize(const int32_t width, const int32_t height);
    bool SetSize(const Size &size);
    bool SetSourceFormat(PixelFormat srcFormat);
    bool SetDestinationFormat(PixelFormat destFormat);
    bool SetColorSapace(ColorSpace colorSpace);
    bool SetConvertFunction(ConvertFunction cvtFunc);
    bool SetConvertFunction(PixelFormat srcFormat, PixelFormat destFormat);
    bool SetPlInfo(const Size&size);
    bool SetAddr(uint8_buffer_type destBuffer, size_t destBufferSize);
    bool CreateSource(PixelFormat &destFormat, const Size &size);
    uint8_buffer_type GetDestinationBuffer();

    uint32_t ConvertImageFormat(uint8_buffer_type &destBuffer, size_t &destBufferSize);
    uint32_t ConvertImageFormat(std::unique_ptr<PixelMap> &destPixelMap);
    ImagePackerError error;
private:
    bool IsValidSize(int32_t width, int32_t height);
    bool IsValidSize(const Size &size);
    bool IsValidColorSpace(ColorSpace colorSpace);
    bool CheckBufferInfo(const_uint8_buffer_type buffer, const size_t bufferSize,
                         PixelFormat format, uint32_t width, uint32_t height);
    size_t GetBufferSizeByFormat(PixelFormat format, int32_t width, int32_t height);
    bool GetConvertFuncByFormat();
    void ReleaseBuffer(uint8_buffer_type buffer);
    void ReadPixelMap();
    bool MakeDestPixelMap(std::unique_ptr<PixelMap> &destPixelMap, uint8_buffer_type destBuffer, size_t destBufferSize);
    bool IsSupport(PixelFormat format);
    bool IsSupportPixelMap(PixelFormat format);


    const_uint8_buffer_type srcBuffer_;
    size_t srcBufferSize_;
    uint8_buffer_type destBuffer_;
    size_t destBufferSize_;
    uint8_t destBufferRefCnt_;
    std::shared_ptr<PixelMap> srcPixelMap_;
    ColorSpace colorSpace_;
    PixelFormat srcFormat_;
    PixelFormat destFormat_;
    Size imageSize_;
    ConvertFunction cvtFunc_;
    ImagePlugin::PlImageInfo plInfo;
    PixelMapAddrInfos addrInfos;
    std::unique_ptr<ImageSource> imageSource;
    std::unique_ptr<PixelMap> destPixelMapUnique;
};

} //OHOS
} //Media
#endif // INTERFACES_INNERKITS_INCLUDE_IMAGE_FORMAT_CONVERT_H_