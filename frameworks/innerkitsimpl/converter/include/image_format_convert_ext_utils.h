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

#ifndef FRAMEWORKS_INNERKITSIMPL_COMMON_INCLUDE_IMAGE_FORMAT_CONVERT_EXT_UTILS_H
#define FRAMEWORKS_INNERKITSIMPL_COMMON_INCLUDE_IMAGE_FORMAT_CONVERT_EXT_UTILS_H

#include <cinttypes>
#include <image_type.h>

namespace OHOS {
namespace Media {

struct I420Info {
    uint32_t width;
    uint32_t height;
    uint8_t *I420Y;
    uint32_t yStride;
    uint8_t *I420U;
    uint32_t uStride;
    uint8_t *I420V;
    uint32_t vStride;
    uint32_t uvHeight;
};

class ImageFormatConvertExtUtils {
public:
    static bool RGB565ToNV12(const uint8_t *srcBuffer, const RGBDataInfo &rgbInfo, uint8_t **destBuffer,
                             size_t &destBufferSize, [[maybe_unused]]ColorSpace colorSpace);
    static bool RGB565ToNV21(const uint8_t *srcBuffer, const RGBDataInfo &rgbInfo, uint8_t **destBuffer,
                             size_t &destBufferSize, [[maybe_unused]]ColorSpace colorSpace);
    static bool BGRAToNV21(const uint8_t *srcBuffer, const RGBDataInfo &rgbInfo, uint8_t **destBuffer,
                           size_t &destBufferSize,  [[maybe_unused]]ColorSpace colorSpace);
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
    static bool NV12ToNV21(const uint8_t *srcBuffer, const YUVDataInfo &yDInfo, uint8_t **destBuffer,
                           size_t &destBufferSize, [[maybe_unused]]ColorSpace colorSpace);
    static bool NV12ToRGBA(const uint8_t *data, const YUVDataInfo &yDInfo, uint8_t **destBuffer,
                           size_t &destBufferSize, [[maybe_unused]]ColorSpace colorSpace);
    static bool NV12ToBGRA(const uint8_t *data, const YUVDataInfo &yDInfo, uint8_t **destBuffer,
                           size_t &destBufferSize, [[maybe_unused]]ColorSpace colorSpace);
    static bool NV12ToRGB(const uint8_t *data, const YUVDataInfo &yDInfo, uint8_t **destBuffer,
                          size_t &destBufferSize, [[maybe_unused]]ColorSpace colorSpace);

    static bool RGB565ToNV12New(const uint8_t *srcBuffer, const RGBDataInfo &rgbInfo, DstConvertDataInfo &dstInfo,
                                [[maybe_unused]]ColorSpace colorSpace);
    static bool RGB565ToNV21New(const uint8_t *srcBuffer, const RGBDataInfo &rgbInfo, DstConvertDataInfo &dstInfo,
                                [[maybe_unused]]ColorSpace colorSpace);
    static bool BGRAToNV21New(const uint8_t *srcBuffer, const RGBDataInfo &rgbInfo, DstConvertDataInfo &dstInfo,
                              [[maybe_unused]]ColorSpace colorSpace);
    static bool RGBAToNV21New(const uint8_t *srcBuffer, const RGBDataInfo &rgbInfo, DstConvertDataInfo &dstInfo,
                              [[maybe_unused]]ColorSpace colorSpace);
    static bool RGBAToNV12New(const uint8_t *srcBuffer, const RGBDataInfo &rgbInfo, DstConvertDataInfo &dstInfo,
                              [[maybe_unused]]ColorSpace colorSpace);
    static bool RGBToNV21New(const uint8_t *srcBuffer, const RGBDataInfo &rgbInfo, DstConvertDataInfo &dstInfo,
                             [[maybe_unused]]ColorSpace colorSpace);
    static bool RGBToNV12New(const uint8_t *srcBuffer, const RGBDataInfo &rgbInfo, DstConvertDataInfo &dstInfo,
                             [[maybe_unused]]ColorSpace colorSpace);
    static bool BGRAToNV12New(const uint8_t *srcBuffer, const RGBDataInfo &rgbInfo, DstConvertDataInfo &dstInfo,
                              [[maybe_unused]]ColorSpace colorSpace);

    static bool NV21ToRGBNew(const uint8_t *srcBuffer, const YUVDataInfo &yDInfo, DstConvertDataInfo &dstInfo,
                             [[maybe_unused]]ColorSpace colorSpace);
    static bool NV21ToRGBANew(const uint8_t *srcBuffer, const YUVDataInfo &yDInfo, DstConvertDataInfo &dstInfo,
                              [[maybe_unused]]ColorSpace colorSpace);
    static bool NV21ToBGRANew(const uint8_t *srcBuffer, const YUVDataInfo &yDInfo, DstConvertDataInfo &dstInfo,
                              [[maybe_unused]]ColorSpace colorSpace);
    static bool NV21ToRGB565New(const uint8_t *srcBuffer, const YUVDataInfo &yDInfo, DstConvertDataInfo &dstInfo,
                                [[maybe_unused]]ColorSpace colorSpace);
    static bool NV12ToRGB565New(const uint8_t *srcBuffer, const YUVDataInfo &yDInfo, DstConvertDataInfo &dstInfo,
                                [[maybe_unused]]ColorSpace colorSpace);
    static bool NV21ToNV12New(const uint8_t *srcBuffer, const YUVDataInfo &yDInfo, DstConvertDataInfo &dstInfo,
                              [[maybe_unused]]ColorSpace colorSpace);
    static bool NV12ToNV21New(const uint8_t *srcBuffer, const YUVDataInfo &yDInfo, DstConvertDataInfo &dstInfo,
                              [[maybe_unused]]ColorSpace colorSpace);
    static bool NV12ToRGBANew(const uint8_t *srcBuffer, const YUVDataInfo &yDInfo, DstConvertDataInfo &dstInfo,
                              [[maybe_unused]]ColorSpace colorSpace);
    static bool NV12ToBGRANew(const uint8_t *srcBuffer, const YUVDataInfo &yDInfo, DstConvertDataInfo &dstInfo,
                              [[maybe_unused]]ColorSpace colorSpace);
    static bool NV12ToRGBNew(const uint8_t *srcBuffer, const YUVDataInfo &yDInfo, DstConvertDataInfo &dstInfo,
                             [[maybe_unused]]ColorSpace colorSpace);
};
} // namespace Media
} // namespace OHOS

#endif // FRAMEWORKS_INNERKITSIMPL_COMMON_INCLUDE_IMAGE_FORMAT_CONVERT_EXT_UTILS_H