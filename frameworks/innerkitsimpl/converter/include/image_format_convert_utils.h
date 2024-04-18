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

#ifndef FRAMEWORKS_INNERKITSIMPL_COMMON_INCLUDE_IMAGE_FORMAT_CONVERT_UTILS_H_
#define FRAMEWORKS_INNERKITSIMPL_COMMON_INCLUDE_IMAGE_FORMAT_CONVERT_UTILS_H_

#include <cinttypes>
#include <image_type.h>

namespace OHOS {
namespace Media {

using ConvertFunction = bool(*)(const uint8_t*, const Size&, uint8_t**, size_t&, [[maybe_unused]]ColorSpace);

bool NV12ToRGB565(const uint8_t *srcBuffer, const Size &imageSize, uint8_t **destBuffer,
                  size_t &destBufferSize, [[maybe_unused]]ColorSpace colorSpace);
bool RGBAF16ToNV21(const uint8_t *srcBuffer, const Size &imageSize, uint8_t **destBuffer,
                   size_t &destBufferSize, [[maybe_unused]]ColorSpace colorSpace);
bool YU12ToNV12(const uint8_t *srcBuffer, const Size &imageSize, uint8_t **destBuffer,
                size_t &destBufferSize, [[maybe_unused]]ColorSpace colorSpace);
bool NV21ToNV12(const uint8_t *srcBuffer, const Size &imageSize, uint8_t **destBuffer,
                size_t &destBufferSize, [[maybe_unused]]ColorSpace colorSpace);
bool NV21ToRGBAF16(const uint8_t *srcBuffer, const Size &imageSize, uint8_t **destBuffer,
                   size_t &destBufferSize, [[maybe_unused]]ColorSpace colorSpace);
bool NV12ToNV21(const uint8_t *srcBuffer, const Size &imageSize, uint8_t **destBuffer,
                size_t &destBufferSize, [[maybe_unused]]ColorSpace colorSpace);
bool RGB565ToNV12(const uint8_t *srcBuffer, const Size &imageSize, uint8_t **destBuffer,
                  size_t &destBufferSize, [[maybe_unused]]ColorSpace colorSpace);
bool RGB565ToNV21(const uint8_t *srcBuffer, const Size &imageSize, uint8_t **destBuffer,
                  size_t &destBufferSize, [[maybe_unused]]ColorSpace colorSpace);
bool NV12ToRGBAF16(const uint8_t *srcBuffer, const Size &imageSize, uint8_t **destBuffer,
                   size_t &destBufferSize, [[maybe_unused]]ColorSpace colorSpace);
bool NV21ToYU12(const uint8_t *srcBuffer, const Size &imageSize, uint8_t **destBuffer,
                size_t &destBufferSize,  [[maybe_unused]]ColorSpace colorSpace);
bool YU12ToNV21(const uint8_t *srcBuffer, const Size &imageSize, uint8_t **destBuffer,
                size_t &destBufferSize,  [[maybe_unused]]ColorSpace colorSpace);
bool BGRAToNV21(const uint8_t *srcBuffer, const Size &imageSize, uint8_t **destBuffer,
                size_t &destBufferSize,  [[maybe_unused]]ColorSpace colorSpace);
bool NV12ToYV12(const uint8_t *srcBuffer, const Size &imageSize, uint8_t **destBuffer,
                size_t &destBufferSize,  [[maybe_unused]]ColorSpace colorSpace);
bool NV21ToYV12(const uint8_t *srcBuffer, const Size &imageSize, uint8_t **destBuffer,
                size_t &destBufferSize,  [[maybe_unused]]ColorSpace colorSpace);
bool NV12ToRGBA(const uint8_t *srcBuffer, const Size &imageSize, uint8_t **destBuffer,
                size_t &destBufferSize, [[maybe_unused]]ColorSpace colorSpace);
bool NV12ToBGRA(const uint8_t *srcBuffer, const Size &imageSize, uint8_t **destBuffer,
                size_t &destBufferSize, [[maybe_unused]]ColorSpace colorSpace);
bool RGBAF16ToNV12(const uint8_t *srcBuffer, const Size &imageSize, uint8_t **destBuffer,
                   size_t &destBufferSize, [[maybe_unused]]ColorSpace colorSpace);
bool RGBAToNV21(const uint8_t *srcBuffer, const Size &imageSize, uint8_t **destBuffer,
                size_t &destBufferSize,  [[maybe_unused]]ColorSpace colorSpace);
bool RGBAToNV12(const uint8_t *srcBuffer, const Size &imageSize, uint8_t **destBuffer,
                size_t &destBufferSize,  [[maybe_unused]]ColorSpace colorSpace);
bool YV12ToNV21(const uint8_t *srcBuffer, const Size &imageSize, uint8_t **destBuffer,
                size_t &destBufferSize,  [[maybe_unused]]ColorSpace colorSpace);
bool RGBToNV21(const uint8_t *srcBuffer, const Size &imageSize, uint8_t **destBuffer,
               size_t &destBufferSize,  [[maybe_unused]]ColorSpace colorSpace);
bool NV12ToRGB(const uint8_t *srcBuffer, const Size &imageSize, uint8_t **destBuffer,
               size_t &destBufferSize,  [[maybe_unused]]ColorSpace colorSpace);
bool NV12ToYU12(const uint8_t *srcBuffer, const Size &imageSize, uint8_t **destBuffer,
                size_t &destBufferSize, [[maybe_unused]]ColorSpace colorSpace);
bool NV21ToRGB(const uint8_t *srcBuffer, const Size &imageSize, uint8_t **destBuffer,
               size_t &destBufferSize, [[maybe_unused]]ColorSpace colorSpace);
bool NV21ToRGBA(const uint8_t *srcBuffer, const Size &imageSize, uint8_t **destBuffer,
                size_t &destBufferSize, [[maybe_unused]]ColorSpace colorSpace);
bool NV21ToBGRA(const uint8_t *srcBuffer, const Size &imageSize, uint8_t **destBuffer,
                size_t &destBufferSize, [[maybe_unused]]ColorSpace colorSpace);
bool NV21ToRGB565(const uint8_t *srcBuffer, const Size &imageSize, uint8_t **destBuffer,
                  size_t &destBufferSize, [[maybe_unused]]ColorSpace colorSpace);
bool RGBToNV12(const uint8_t *srcBuffer, const Size &imageSize, uint8_t **destBuffer,
               size_t &destBufferSize, [[maybe_unused]]ColorSpace colorSpace);
bool YV12ToNV12(const uint8_t *srcBuffer, const Size &imageSize, uint8_t **destBuffer,
                size_t &destBufferSize, [[maybe_unused]]ColorSpace colorSpace);
bool BGRAToNV12(const uint8_t *srcBuffer, const Size &imageSize, uint8_t **destBuffer,
                size_t &destBufferSize, [[maybe_unused]]ColorSpace colorSpace);
} // namespace Media
} // namespace OHOS

#endif //FRAMEWORKS_INNERKITSIMPL_COMMON_INCLUDE_IMAGE_FORMAT_CONVERT_UTILS_H_