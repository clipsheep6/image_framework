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

#ifndef FRAMEWORKS_INNERKITSIMPL_UTILS_INCLUDE_PIXEL_YUV_UTILS_H_
#define FRAMEWORKS_INNERKITSIMPL_UTILS_INCLUDE_PIXEL_YUV_UTILS_H_

#include <cstdlib>
#include <cstdio>
#include <string>
#include "image_type.h"
#include "iosfwd"

#ifdef LIBYUV_ENABLE
#include "libyuv.h"
#endif

namespace OHOS {
namespace Media {

struct UVPos {
    size_t upos = 0;
    size_t vpos = 0;
};

class PixelYuvUtils {
public:
    static bool WriteYuvConvert(const void *srcPixels, const ImageInfo &srcInfo, void *dstPixels,
        const Position &dstPos, const ImageInfo &dstInfo);
    static bool ReadYuvConvert(const void *srcPixels, const Position &srcPos, const ImageInfo &srcInfo,
        void *dstPixels, const ImageInfo &dstInfo);
    static void SetTranslateDataDefault(uint8_t *srcPixels, int32_t width, int32_t height);
    static uint8_t GetYuv420Y(uint32_t x, uint32_t y, int32_t width, const uint8_t *in);
    static uint8_t GetYuv420U(uint32_t x, uint32_t y, int32_t width, int32_t height, PixelFormat format,
        const uint8_t *in);
    static uint8_t GetYuv420V(uint32_t x, uint32_t y, int32_t width, int32_t height, PixelFormat format,
        const uint8_t *in);
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
    static int32_t I420Copy(const uint8_t *srcY, int32_t srcStrideY, const uint8_t *srcU, int32_t srcStrideU,
        const uint8_t *srcV, int32_t srcStrideV, uint8_t *dstY, int32_t dstStrideY, uint8_t *dstU,
        int32_t dstStrideU, uint8_t *dstV, int32_t dstStrideV, int32_t width, int32_t height);
    static int32_t I420Mirror(const uint8_t *srcY, int32_t srcStrideY, const uint8_t *srcU, int32_t srcStrideU,
        const uint8_t *srcV, int32_t srcStrideV, uint8_t *dstY, int32_t dstStrideY,
        uint8_t *dstU, int32_t dstStrideU, uint8_t *dstV, int32_t dstStrideV,
        int32_t width, int32_t height);
    static int32_t NV12Mirror(const uint8_t *srcY, int32_t srcStrideY, const uint8_t *srcUV, int32_t srcStrideUV,
        uint8_t *dstY, int32_t dstStrideY, uint8_t *dstUV, int32_t dstStrideUV, int32_t width, int32_t height);
    static int32_t NV12Copy(const uint8_t *srcY, int32_t srcStrideY, const uint8_t *srcUV, int32_t srcStrideUV,
        uint8_t *dstY, int32_t dstStrideY, uint8_t *dstUV, int32_t dstStrideUV, int32_t width, int32_t height);
    static bool Yuv420WritePixels(Size &size, uint8_t *srcPixels, const PixelFormat &format, const uint32_t &color);
    static bool YuvReadPixel(const uint8_t *srcPixels, Size &size, const PixelFormat &format, const Position &pos,
        uint32_t &dst);
    static bool YuvWritePixel(uint8_t *srcPixels, Size &size, const PixelFormat &format, const Position &pos,
        const uint32_t &color);
    static bool YuvTranslate(const uint8_t *srcPixels, Size &size, uint8_t *dstPixels, const PixelFormat &format,
        float xAxis, float yAxis);
    static bool YuvCrop(const uint8_t *srcPixels, const Size &size, uint8_t *dstPixels, const Rect &rect,
        const PixelFormat &srcFormat);
    static bool YuvRotate(const uint8_t *srcPixels, Size &size, uint8_t *dstPixels, int32_t degrees,
        const PixelFormat &format);
    #endif
};
} // namespace Media
} // namespace OHOS

#endif // FRAMEWORKS_INNERKITSIMPL_UTILS_INCLUDE_PIXEL_YUV_UTILS_H_
