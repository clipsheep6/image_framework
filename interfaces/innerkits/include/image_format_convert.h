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
#include "image_plugin_type.h"
#include "image_type.h"
#include <memory>

namespace OHOS {
namespace Media {
using uint8_buffer_type = uint8_t *;
using const_uint8_buffer_type = const uint8_t *;

struct ConvertDataInfo {
    uint8_buffer_type buffer = nullptr;
    size_t bufferSize;
    Size imageSize;
    PixelFormat pixelFormat = PixelFormat::UNKNOWN;
    ColorSpace colorSpace = ColorSpace::SRGB;
};

class ImageFormatConvert {
public:
    static uint32_t ConvertImageFormat(const ConvertDataInfo &srcDataInfo, ConvertDataInfo &destDataInfo);
    static uint32_t ConvertImageFormat(const std::shared_ptr<PixelMap> srcPiexlMap,
                                       std::shared_ptr<PixelMap> &destPiexlMap, PixelFormat destFormat);
private:
    static bool IsValidSize(const Size &size);
    static bool CheckConvertDataInfo(const ConvertDataInfo &convertDataInfo);
    static size_t GetBufferSizeByFormat(PixelFormat format, const Size &size);
    static ConvertFunction GetConvertFuncByFormat(PixelFormat srcFormat, PixelFormat destFormat);
    static bool MakeDestPixelMap(std::shared_ptr<PixelMap> &destPixelMap, uint8_buffer_type destBuffer,
                                 const size_t destBufferSize, ImageInfo &info, AllocatorType allcatorType);
    static bool IsSupport(PixelFormat format);
};

} //OHOS
} //Media
#endif // INTERFACES_INNERKITS_INCLUDE_IMAGE_FORMAT_CONVERT_H_