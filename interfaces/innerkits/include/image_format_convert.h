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

#ifndef INTERFACES_INNERKITS_INCLUDE_IMAGE_FORMAT_CONVERT_H
#define INTERFACES_INNERKITS_INCLUDE_IMAGE_FORMAT_CONVERT_H

#include <memory>

#include "image_format_convert_utils.h"
#include "image_plugin_type.h"
#include "image_type.h"
#include "pixel_map.h"
#include "memory_manager.h"

namespace OHOS {
namespace Media {
using uint8_buffer_type = uint8_t *;
using const_uint8_buffer_type = const uint8_t *;
using ConvertFunction = bool(*)(const uint8_t*, const RGBDataInfo&, uint8_t**, size_t&, [[maybe_unused]]ColorSpace);
using YUVConvertFunction = bool(*)(const uint8_t*, const YUVDataInfo&, uint8_t**, size_t&,
    [[maybe_unused]]ColorSpace);

using ConvertFunctionNew = bool(*)(const uint8_t*, const RGBDataInfo&, DstConvertDataInfo &dstInfo,
    [[maybe_unused]]ColorSpace);
using YUVConvertFunctionNew = bool(*)(const uint8_t*, const YUVDataInfo&, DstConvertDataInfo &dstInfo,
    [[maybe_unused]]ColorSpace);

struct ConvertDataInfo {
    uint8_buffer_type buffer = nullptr;
    size_t bufferSize;
    Size imageSize;
    PixelFormat pixelFormat = PixelFormat::UNKNOWN;
    ColorSpace colorSpace = ColorSpace::SRGB;
};

class ImageFormatConvert {
    friend class ImageFormatConvertTest;
public:
    static uint32_t ConvertImageFormat(const ConvertDataInfo &srcDataInfo, ConvertDataInfo &destDataInfo);
    static uint32_t ConvertImageFormat(std::shared_ptr<PixelMap> &srcPiexlMap, PixelFormat destFormat);
private:
    static bool IsValidSize(const Size &size);
    static bool CheckConvertDataInfo(const ConvertDataInfo &convertDataInfo);
    static uint32_t YUVConvertImageFormatOption(std::shared_ptr<PixelMap> &srcPiexlMap, const PixelFormat &srcFormat,
                                                PixelFormat destFormat);
    static size_t GetBufferSizeByFormat(PixelFormat format, const Size &size);
    static ConvertFunction GetConvertFuncByFormat(PixelFormat srcFormat, PixelFormat destFormat);
    static ConvertFunctionNew GetConvertFuncByFormatNew(PixelFormat srcFormat, PixelFormat destFormat);
    static YUVConvertFunction YUVGetConvertFuncByFormat(PixelFormat srcFormat, PixelFormat destFormat);
    static YUVConvertFunctionNew YUVGetConvertFuncByFormatNew(PixelFormat srcFormat, PixelFormat destFormat);
    static bool MakeDestPixelMap(std::shared_ptr<PixelMap> &destPixelMap, uint8_buffer_type destBuffer,
                                 const size_t destBufferSize, ImageInfo &info, AllocatorType allcatorType,
                                 void *context);
    static bool IsSupport(PixelFormat format);
    static std::unique_ptr<AbsMemory> CreateMemory(PixelFormat pixelFormat, uint32_t pictureSize,
                                                   AllocatorType allocatorType, int32_t width, int32_t height,
                                                   YUVStrideInfo &strides);
};
} //OHOS
} //Media
#endif // INTERFACES_INNERKITS_INCLUDE_IMAGE_FORMAT_CONVERT_H