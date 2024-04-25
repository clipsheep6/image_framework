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

#ifndef FRAMEWORKS_INNERKITSIMPL_UTILS_INCLUDE_LIBYUV_PIXEL_MAP_UTILS_H
#define FRAMEWORKS_INNERKITSIMPL_UTILS_INCLUDE_LIBYUV_PIXEL_MAP_UTILS_H

#include <cstdlib>
#include <cstdio>
#include <string>
#include "image_type.h"
#include "iosfwd"

#ifdef __cplusplus
extern "C" {
#endif
#include "libavfilter/avfilter.h"
#include "libavfilter/buffersrc.h"
#include "libavfilter/buffersink.h"
#include "libavutil/avstring.h"
#include "libavutil/frame.h"
#include "libavutil/imgutils.h"
#include "libavutil/pixfmt.h"
#include "libswscale/swscale.h"
#ifdef __cplusplus
}
#endif

#include "libyuv.h"

namespace OHOS {
namespace Media {

class LibYuvPixelMapUtils {
public:
    static bool BGRAToYuv420(const uint8_t *src, uint8_t *dst, int srcW, int srcH, PixelFormat pixelFormat);
    static bool Yuv420ToBGRA(const uint8_t *sample, uint8_t *dst_argb,
        int32_t width, int32_t height, PixelFormat pixelFormat);
    static bool Yuv420ToARGB(const uint8_t *sample, uint8_t *dst_argb,
        int32_t width, int32_t height, PixelFormat pixelFormat);
    static bool YuvRotate(const uint8_t *srcPixels, Size &size, uint8_t *dstPixels, int32_t degrees,
        const PixelFormat &format);
    static void ConvertYuvMode(libyuv::FilterMode &filterMode, const AntiAliasingOption &option);

};
} // namespace Media
} // namespace OHOS
#endif // RAMEWORKS_INNERKITSIMPL_UTILS_INCLUDE_LIBYUV_PIXEL_MAP_UTILS_H
