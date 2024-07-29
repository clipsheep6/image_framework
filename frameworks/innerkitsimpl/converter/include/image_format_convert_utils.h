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

#ifndef FRAMEWORKS_INNERKITSIMPL_COMMON_INCLUDE_IMAGE_FORMAT_CONVERT_UTILS_H
#define FRAMEWORKS_INNERKITSIMPL_COMMON_INCLUDE_IMAGE_FORMAT_CONVERT_UTILS_H

#include <cinttypes>
#include <image_type.h>

namespace OHOS {
namespace Media {
class ImageFormatConvertUtils {
public:
    static bool RGBAF16ToNV21(const uint8_t *srcBuffer, const RGBDataInfo &rgbInfo, uint8_t **destBuffer,
                              size_t &destBufferSize, [[maybe_unused]]ColorSpace colorSpace);
    static bool RGB565ToNV12(const uint8_t *srcBuffer, const RGBDataInfo &rgbInfo, uint8_t **destBuffer,
                             size_t &destBufferSize, [[maybe_unused]]ColorSpace colorSpace);
    static bool RGB565ToNV21(const uint8_t *srcBuffer, const RGBDataInfo &rgbInfo, uint8_t **destBuffer,
                             size_t &destBufferSize, [[maybe_unused]]ColorSpace colorSpace);
    static bool BGRAToNV21(const uint8_t *srcBuffer, const RGBDataInfo &rgbInfo, uint8_t **destBuffer,
                           size_t &destBufferSize,  [[maybe_unused]]ColorSpace colorSpace);
    static bool RGBAF16ToNV12(const uint8_t *srcBuffer, const RGBDataInfo &rgbInfo, uint8_t **destBuffer,
                              size_t &destBufferSize, [[maybe_unused]]ColorSpace colorSpace);
    static bool RGBAToNV21(const uint8_t *srcBuffer, const RGBDataInfo &rgbInfo, uint8_t **destBuffer,
                           size_t &destBufferSize,  [[maybe_unused]]ColorSpace colorSpace);
    static bool RGBAToNV12(const uint8_t *srcBuffer, const RGBDataInfo &rgbInfo, uint8_t **destBuffer,
                           size_t &destBufferSize,  [[maybe_unused]]ColorSpace colorSpace);
    static bool RGBToNV21(const uint8_t *srcBuffer, const RGBDataInfo &rgbInfo, uint8_t **destBuffer,
                          size_t &destBufferSize,  [[maybe_unused]]ColorSpace colorSpace);
    static bool RGBToNV12(const uint8_t *srcBuffer, const RGBDataInfo &rgbInfo, uint8_t **destBuffer,
                          size_t &destBufferSize, [[maybe_unused]]ColorSpace colorSpace);
    static bool BGRAToNV12(const uint8_t *srcBuffer, const RGBDataInfo &rgbInfo, uint8_t **destBuffer,
                           size_t &destBufferSize, [[maybe_unused]]ColorSpace colorSpace);
    static bool RGB565ToNV12P010(const uint8_t *srcBuffer, const RGBDataInfo &imageSize, uint8_t **destBuffer,
                                 size_t &destBufferSize, [[maybe_unused]]ColorSpace colorSpace);
    static bool RGB565ToNV21P010(const uint8_t *srcBuffer, const RGBDataInfo &imageSize, uint8_t **destBuffer,
                                 size_t &destBufferSize, [[maybe_unused]]ColorSpace colorSpace);
    static bool RGBAToNV12P010(const uint8_t *srcBuffer, const RGBDataInfo &imageSize, uint8_t **destBuffer,
                               size_t &destBufferSize, [[maybe_unused]]ColorSpace colorSpace);
    static bool RGBAToNV21P010(const uint8_t *srcBuffer, const RGBDataInfo &imageSize, uint8_t **destBuffer,
                               size_t &destBufferSize, [[maybe_unused]]ColorSpace colorSpace);
    static bool BGRAToNV12P010(const uint8_t *srcBuffer, const RGBDataInfo &imageSize, uint8_t **destBuffer,
                               size_t &destBufferSize, [[maybe_unused]]ColorSpace colorSpace);
    static bool BGRAToNV21P010(const uint8_t *srcBuffer, const RGBDataInfo &imageSize, uint8_t **destBuffer,
                               size_t &destBufferSize, [[maybe_unused]]ColorSpace colorSpace);
    static bool RGBToNV12P010(const uint8_t *srcBuffer, const RGBDataInfo &imageSize, uint8_t **destBuffer,
                              size_t &destBufferSize, [[maybe_unused]]ColorSpace colorSpace);
    static bool RGBToNV21P010(const uint8_t *srcBuffer, const RGBDataInfo &imageSize, uint8_t **destBuffer,
                              size_t &destBufferSize, [[maybe_unused]]ColorSpace colorSpace);
    static bool RGBAF16ToNV12P010(const uint8_t *srcBuffer, const RGBDataInfo &imageSize, uint8_t **destBuffer,
                                  size_t &destBufferSize, [[maybe_unused]]ColorSpace colorSpace);
    static bool RGBAF16ToNV21P010(const uint8_t *srcBuffer, const RGBDataInfo &imageSize, uint8_t **destBuffer,
                                  size_t &destBufferSize, [[maybe_unused]]ColorSpace colorSpace);
    static bool RGBA1010102ToNV12(const uint8_t *srcBuffer, const RGBDataInfo &imageSize, uint8_t **destBuffer,
                                  size_t &destBufferSize, [[maybe_unused]]ColorSpace colorSpace);
    static bool RGBA1010102ToNV12P010(const uint8_t *srcBuffer, const RGBDataInfo &imageSize, uint8_t **destBuffer,
                                      size_t &destBufferSize, [[maybe_unused]]ColorSpace colorSpace);
    static bool RGBA1010102ToNV21P010(const uint8_t *srcBuffer, const RGBDataInfo &imageSize, uint8_t **destBuffer,
                                      size_t &destBufferSize, [[maybe_unused]]ColorSpace colorSpace);
    static bool RGBA1010102ToNV21(const uint8_t *srcBuffer, const RGBDataInfo &imageSize, uint8_t **destBuffer,
                                  size_t &destBufferSize, [[maybe_unused]]ColorSpace colorSpace);
    static bool NV21ToRGB(const uint8_t *data, const YUVDataInfo &yDInfo, uint8_t **destBuffer,
                          size_t &destBufferSize, [[maybe_unused]]ColorSpace colorSpace);
    static bool NV21ToRGBA(const uint8_t *srcBuffer, const YUVDataInfo &yDInfo, uint8_t **destBuffer,
                           size_t &destBufferSize, [[maybe_unused]]ColorSpace colorSpace);
    static bool NV21ToBGRA(const uint8_t *srcBuffer, const YUVDataInfo &yDInfo, uint8_t **destBuffer,
                           size_t &destBufferSize, [[maybe_unused]]ColorSpace colorSpace);
    static bool NV21ToRGB565(const uint8_t *srcBuffer, const YUVDataInfo &yDInfo, uint8_t **destBuffer,
                             size_t &destBufferSize, [[maybe_unused]]ColorSpace colorSpace);
    static bool NV12ToRGB565(const uint8_t *srcBuffer, const YUVDataInfo &yDInfo, uint8_t **destBuffer,
                             size_t &destBufferSize, [[maybe_unused]]ColorSpace colorSpace);
    static bool NV21ToNV12(const uint8_t *srcBuffer, const YUVDataInfo &yDInfo, uint8_t **destBuffer,
                           size_t &destBufferSize, [[maybe_unused]]ColorSpace colorSpace);
    static bool NV21ToRGBAF16(const uint8_t *srcBuffer, const YUVDataInfo &yDInfo, uint8_t **destBuffer,
                              size_t &destBufferSize, [[maybe_unused]]ColorSpace colorSpace);
    static bool NV12ToNV21(const uint8_t *srcBuffer, const YUVDataInfo &yDInfo, uint8_t **destBuffer,
                           size_t &destBufferSize, [[maybe_unused]]ColorSpace colorSpace);
    static bool NV12ToRGBAF16(const uint8_t *data, const YUVDataInfo &yDInfo, uint8_t **destBuffer,
                              size_t &destBufferSize, [[maybe_unused]]ColorSpace colorSpace);
    static bool NV12ToRGBA1010102(const uint8_t *data, const YUVDataInfo &yDInfo, uint8_t **destBuffer,
                                  size_t &destBufferSize, [[maybe_unused]]ColorSpace colorSpace);
    static bool NV21ToRGBA1010102(const uint8_t *data, const YUVDataInfo &yDInfo, uint8_t **destBuffer,
                                  size_t &destBufferSize, [[maybe_unused]]ColorSpace colorSpace);
    static bool NV12P010ToRGBA1010102(const uint8_t *data, const YUVDataInfo &yDInfo, uint8_t **destBuffer,
                                      size_t &destBufferSize, [[maybe_unused]]ColorSpace colorSpace);
    static bool NV21P010ToRGBA1010102(const uint8_t *data, const YUVDataInfo &yDInfo, uint8_t **destBuffer,
                                      size_t &destBufferSize, [[maybe_unused]]ColorSpace colorSpace);
    static bool NV12ToRGBA(const uint8_t *data, const YUVDataInfo &yDInfo, uint8_t **destBuffer,
                           size_t &destBufferSize, [[maybe_unused]]ColorSpace colorSpace);
    static bool NV12ToBGRA(const uint8_t *data, const YUVDataInfo &yDInfo, uint8_t **destBuffer,
                           size_t &destBufferSize, [[maybe_unused]]ColorSpace colorSpace);
    static bool NV12ToRGB(const uint8_t *data, const YUVDataInfo &yDInfo, uint8_t **destBuffer,
                          size_t &destBufferSize, [[maybe_unused]]ColorSpace colorSpace);
    
    static bool NV12ToNV12P010(const uint8_t *data, const YUVDataInfo &yDInfo, uint8_t **destBuffer,
                               size_t &destBufferSize, [[maybe_unused]]ColorSpace colorSpace);
    static bool NV12ToNV21P010(const uint8_t *data, const YUVDataInfo &yDInfo, uint8_t **destBuffer,
                               size_t &destBufferSize, [[maybe_unused]]ColorSpace colorSpace);
    static bool NV21ToNV12P010(const uint8_t *data, const YUVDataInfo &yDInfo, uint8_t **destBuffer,
                               size_t &destBufferSize, [[maybe_unused]]ColorSpace colorSpace);
    static bool NV21ToNV21P010(const uint8_t *data, const YUVDataInfo &yDInfo, uint8_t **destBuffer,
                               size_t &destBufferSize, [[maybe_unused]]ColorSpace colorSpace);

    static bool NV12P010ToNV12(const uint8_t *data, const YUVDataInfo &yDInfo, uint8_t **destBuffer,
                               size_t &destBufferSize, [[maybe_unused]]ColorSpace colorSpace);
    static bool NV12P010ToNV21(const uint8_t *data, const YUVDataInfo &yDInfo, uint8_t **destBuffer,
                               size_t &destBufferSize, [[maybe_unused]]ColorSpace colorSpace);
    static bool NV12P010ToNV21P010(const uint8_t *data, const YUVDataInfo &yDInfo, uint8_t **destBuffer,
                                   size_t &destBufferSize, [[maybe_unused]]ColorSpace colorSpace);
    static bool NV12P010ToRGB565(const uint8_t *data, const YUVDataInfo &yDInfo, uint8_t **destBuffer,
                                 size_t &destBufferSize, [[maybe_unused]]ColorSpace colorSpace);
    static bool NV12P010ToRGBA8888(const uint8_t *data, const YUVDataInfo &yDInfo, uint8_t **destBuffer,
                                   size_t &destBufferSize, [[maybe_unused]]ColorSpace colorSpace);
    static bool NV12P010ToBGRA8888(const uint8_t *data, const YUVDataInfo &yDInfo, uint8_t **destBuffer,
                                   size_t &destBufferSize, [[maybe_unused]]ColorSpace colorSpace);
    static bool NV12P010ToRGB888(const uint8_t *data, const YUVDataInfo &yDInfo, uint8_t **destBuffer,
                                 size_t &destBufferSize, [[maybe_unused]]ColorSpace colorSpace);
    static bool NV12P010ToRGBAF16(const uint8_t *data, const YUVDataInfo &yDInfo, uint8_t **destBuffer,
                                  size_t &destBufferSize, [[maybe_unused]]ColorSpace colorSpace);
    static bool NV21P010ToNV12(const uint8_t *data, const YUVDataInfo &yDInfo, uint8_t **destBuffer,
                               size_t &destBufferSize, [[maybe_unused]]ColorSpace colorSpace);
    static bool NV21P010ToNV21(const uint8_t *data, const YUVDataInfo &yDInfo, uint8_t **destBuffer,
                               size_t &destBufferSize, [[maybe_unused]]ColorSpace colorSpace);
    static bool NV21P010ToNV12P010(const uint8_t *data, const YUVDataInfo &yDInfo, uint8_t **destBuffer,
                                   size_t &destBufferSize, [[maybe_unused]]ColorSpace colorSpace);
    static bool NV21P010ToRGB565(const uint8_t *data, const YUVDataInfo &yDInfo, uint8_t **destBuffer,
                                  size_t &destBufferSize, [[maybe_unused]]ColorSpace colorSpace);
    static bool NV21P010ToRGBA8888(const uint8_t *data, const YUVDataInfo &yDInfo, uint8_t **destBuffer,
                                   size_t &destBufferSize, [[maybe_unused]]ColorSpace colorSpace);
    static bool NV21P010ToBGRA8888(const uint8_t *data, const YUVDataInfo &yDInfo, uint8_t **destBuffer,
                                   size_t &destBufferSize, [[maybe_unused]]ColorSpace colorSpace);
    static bool NV21P010ToRGB888(const uint8_t *data, const YUVDataInfo &yDInfo, uint8_t **destBuffer,
                                 size_t &destBufferSize, [[maybe_unused]]ColorSpace colorSpace);
    static bool NV21P010ToRGBAF16(const uint8_t *data, const YUVDataInfo &yDInfo, uint8_t **destBuffer,
                                  size_t &destBufferSize, [[maybe_unused]]ColorSpace colorSpace);
};
} // namespace Media
} // namespace OHOS

#endif //FRAMEWORKS_INNERKITSIMPL_COMMON_INCLUDE_IMAGE_FORMAT_CONVERT_UTILS_H