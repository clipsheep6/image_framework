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

#ifndef FRAMEWORKS_INNERKITSIMPL_UTILS_INCLUDE_PIXEL_YUV_UTILS_H
#define FRAMEWORKS_INNERKITSIMPL_UTILS_INCLUDE_PIXEL_YUV_UTILS_H

#include <cstdlib>
#include <cstdio>
#include <string>
#include "image_type.h"
#include "iosfwd"

#ifdef __cplusplus
extern "C" {
#endif
#include "libavfilter/avfilter.h"
#include "libavutil/avstring.h"
#include "libavfilter/buffersrc.h"
#include "libavfilter/buffersink.h"
#include "libavutil/frame.h"
#include "libavutil/imgutils.h"
#include "libavutil/pixfmt.h"
#include "libswscale/swscale.h"
#ifdef __cplusplus
}
#endif

#ifdef LIBYUV_ENABLE
#include "libyuv.h"
#endif

namespace OHOS {
namespace Media {

struct UVPos {
    size_t upos = 0;
    size_t vpos = 0;
};

struct XYaxis {
    float xAxis = 0;
    float yAxis = 0;
};

struct Yuv420UV {
    int32_t srcStrideU = 0;
    int32_t srcStrideV = 0;
    int32_t dstStrideUV = 0;
};

struct YuvImageInfo {
    AVPixelFormat format = AVPixelFormat::AV_PIX_FMT_NONE;
    int32_t width;
    int32_t height;
};

class PixelYuvUtils {
public:
    static int32_t YuvConvertOption(const AntiAliasingOption &option);
    static bool WriteYuvConvert(const void *srcPixels, const ImageInfo &srcInfo, void *dstPixels,
        const Position &dstPos, const ImageInfo &dstInfo);
    static bool ReadYuvConvert(const void *srcPixels, const Position &srcPos, const ImageInfo &srcInfo,
        void *dstPixels, const ImageInfo &dstInfo);
    static void SetTranslateDataDefault(uint8_t *srcPixels, int32_t width, int32_t height);
    static uint8_t GetYuv420Y(uint32_t x, uint32_t y, int32_t width, const uint8_t *in);
    static uint8_t GetYuv420U(uint32_t x, uint32_t y, Size &size, PixelFormat format,
        const uint8_t *in);
    static uint8_t GetYuv420V(uint32_t x, uint32_t y, Size &size, PixelFormat format,
        const uint8_t *in);
    static AVPixelFormat ConvertFormat(const PixelFormat &pixelFormat);
    #ifdef LIBYUV_ENABLE
    static bool BGRAToYuv420(const uint8_t *src, uint8_t *dst, int srcW, int srcH, PixelFormat pixelFormat);
    static bool Yuv420ToBGRA(const uint8_t *sample, uint8_t *dst_argb,
        int32_t width, int32_t height, PixelFormat pixelFormat);
    static bool Yuv420ToARGB(const uint8_t *sample, uint8_t *dst_argb,
        int32_t width, int32_t height, PixelFormat pixelFormat);
    static bool YuvRotate(const uint8_t *srcPixels, Size &size, uint8_t *dstPixels, int32_t degrees,
        const PixelFormat &format);
    static void ConvertYuvMode(libyuv::FilterMode &filterMode, const AntiAliasingOption &option);
    #else
    static bool BGRAToYuv420(const uint8_t *src, uint8_t *dst, int32_t width, int32_t height,
        PixelFormat pixelFormat);
    static bool Yuv420ToBGRA(const uint8_t *in, uint8_t *out, int32_t width, int32_t height,
        PixelFormat pixelFormat);
    static bool Yuv420ToARGB(const uint8_t *in, uint8_t *out, int32_t width, int32_t height,
        PixelFormat pixelFormat);
    static bool Yuv420WritePixels(Size &size, uint8_t *srcPixels, const PixelFormat &format, const uint32_t &color);
    static bool YuvReadPixel(const uint8_t *srcPixels, Size &size, const PixelFormat &format, const Position &pos,
        uint32_t &dst);
    static bool YuvWritePixel(uint8_t *srcPixels, Size &size, const PixelFormat &format, const Position &pos,
        const uint32_t &color);
    static bool YuvTranslate(const uint8_t *srcPixels, ImageInfo &info, uint8_t *dstPixels, XYaxis &xyAxis);
    static bool YuvCrop(uint8_t *srcData, YuvImageInfo &srcInfo, uint8_t *dstData, const Rect &rect);
    static bool YuvRotate(uint8_t *srcData, YuvImageInfo &srcInfo,
        uint8_t *dstData, YuvImageInfo &dstInfo, int32_t degrees);
    static bool YuvFlip(const uint8_t *srcData, YuvImageInfo &srcInfo, uint8_t *dstData, bool xAxis);
    static bool YuvReversal(uint8_t *srcData, YuvImageInfo &srcInfo, uint8_t *dstData, YuvImageInfo &dstInfo);
    static int32_t YuvScale(const uint8_t *srcPixels, YuvImageInfo &srcInfo,
        uint8_t *dstPixels, YuvImageInfo &dstInfo, int32_t module);
    #endif
};
} // namespace Media
} // namespace OHOS

#endif // FRAMEWORKS_INNERKITSIMPL_UTILS_INCLUDE_PIXEL_YUV_UTILS_H
