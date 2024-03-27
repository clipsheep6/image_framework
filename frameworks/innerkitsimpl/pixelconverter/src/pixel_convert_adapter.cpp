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

#include "pixel_convert_adapter.h"

#include <map>

#include "image_log.h"
#include "include/core/SkBitmap.h"
#include "include/core/SkCanvas.h"
#include "include/core/SkColor.h"
#include "include/core/SkColorSpace.h"
#include "include/core/SkImageInfo.h"
#include "include/core/SkPaint.h"
#include "include/core/SkPixmap.h"
#include "media_errors.h"
#ifdef _WIN32
#include <iomanip>
#endif

#undef LOG_DOMAIN
#define LOG_DOMAIN LOG_TAG_DOMAIN_ID_IMAGE

#undef LOG_TAG
#define LOG_TAG "PixelConvertAdapter"

namespace OHOS {
namespace Media {
using namespace std;

static const uint8_t NUM_0 = 0;
static const uint8_t NUM_1 = 1;
static const uint8_t NUM_2 = 2;
static const uint8_t NUM_3 = 3;
static const uint8_t NUM_4 = 4;

constexpr int32_t INT_128 = 128;
constexpr int32_t INT_255 = 255;
const float YUV_TO_RGB888_PARAM_1 = 1.402;
const float YUV_TO_RGB888_PARAM_2 = 0.344136;
const float YUV_TO_RGB888_PARAM_3 = 0.714136;
const float YUV_TO_RGB888_PARAM_4 = 1.772;

static const map<PixelFormat, SkColorType> PIXEL_FORMAT_MAP = {
    { PixelFormat::UNKNOWN, SkColorType::kUnknown_SkColorType},
    { PixelFormat::ARGB_8888, SkColorType::kRGBA_8888_SkColorType},
    { PixelFormat::ALPHA_8, SkColorType::kAlpha_8_SkColorType},
    { PixelFormat::RGB_565, SkColorType::kRGB_565_SkColorType},
    { PixelFormat::RGBA_F16, SkColorType::kRGBA_F16_SkColorType},
    { PixelFormat::RGBA_8888, SkColorType::kRGBA_8888_SkColorType},
    { PixelFormat::BGRA_8888, SkColorType::kBGRA_8888_SkColorType},
    { PixelFormat::RGB_888, SkColorType::kRGB_888x_SkColorType},
};

static SkColorType PixelFormatConvert(const PixelFormat &pixelFormat)
{
    auto formatSearch = PIXEL_FORMAT_MAP.find(pixelFormat);
    return (formatSearch != PIXEL_FORMAT_MAP.end()) ? formatSearch->second : SkColorType::kUnknown_SkColorType;
}

static void ARGBToRGBA(uint8_t* srcPixels, uint8_t* dstPixels, uint32_t byteCount)
{
    if (byteCount % NUM_4 != NUM_0) {
        IMAGE_LOGE("Pixel count must multiple of 4.");
        return;
    }
    uint8_t *src = srcPixels;
    uint8_t *dst = dstPixels;
    for (uint32_t i = NUM_0 ; i < byteCount; i += NUM_4) {
        // 0-R 1-G 2-B 3-A
        dst[NUM_0] = src[NUM_1];
        dst[NUM_1] = src[NUM_2];
        dst[NUM_2] = src[NUM_3];
        dst[NUM_3] = src[NUM_0];
        src += NUM_4;
        dst += NUM_4;
    }
}

static void RGBAToARGB(uint8_t* srcPixels, uint8_t* dstPixels, uint32_t byteCount)
{
    if (byteCount % NUM_4 != NUM_0) {
        IMAGE_LOGE("Pixel count must multiple of 4.");
        return;
    }
    uint8_t *src = srcPixels;
    uint8_t *dst = dstPixels;
    for (uint32_t i = NUM_0 ; i < byteCount; i += NUM_4) {
        // 0-A 1-R 2-G 3-B
        dst[NUM_0] = src[NUM_3];
        dst[NUM_1] = src[NUM_0];
        dst[NUM_2] = src[NUM_1];
        dst[NUM_3] = src[NUM_2];
        src += NUM_4;
        dst += NUM_4;
    }
}

static void RGBxToRGB(const uint8_t* srcPixels, uint8_t* dstPixels, uint32_t byteCount)
{
    if (byteCount % NUM_4 != NUM_0) {
        IMAGE_LOGE("Pixel count must multiple of 4.");
        return;
    }
    const uint8_t *src = srcPixels;
    uint8_t *dst = dstPixels;
    for (uint32_t i = NUM_0 ; i < byteCount; i += NUM_4) {
        // 0-R 1-G 2-B
        dst[NUM_0] = src[NUM_0];
        dst[NUM_1] = src[NUM_1];
        dst[NUM_2] = src[NUM_2];
        src += NUM_4;
        dst += NUM_3;
    }
}

static void RGBToRGBx(const uint8_t* srcPixels, uint8_t* dstPixels, uint32_t byteCount)
{
    if (byteCount % NUM_3 != NUM_0) {
        IMAGE_LOGE("Pixel count must multiple of 3.");
        return;
    }
    const uint8_t *src = srcPixels;
    uint8_t *dst = dstPixels;
    for (uint32_t i = NUM_0 ; i < byteCount; i += NUM_3) {
        // 0-R 1-G 2-B
        dst[NUM_0] = src[NUM_0];
        dst[NUM_1] = src[NUM_1];
        dst[NUM_2] = src[NUM_2];
        dst[NUM_3] = 0;
        src += NUM_3;
        dst += NUM_4;
    }
}

static int32_t GetRGBxRowBytes(const ImageInfo &imgInfo)
{
    return imgInfo.size.width * NUM_4;
}

static int32_t GetRGBxSize(const ImageInfo &imgInfo)
{
    return imgInfo.size.height * GetRGBxRowBytes(imgInfo);
}

bool PixelConvertAdapter::WritePixelsConvert(const void *srcPixels, uint32_t srcRowBytes, const ImageInfo &srcInfo,
                                             void *dstPixels, const Position &dstPos, uint32_t dstRowBytes,
                                             const ImageInfo &dstInfo)
{
    // basic valid check, other parameters valid check in writePixels method
    if (srcPixels == nullptr || dstPixels == nullptr) {
        IMAGE_LOGE("src or dst pixels invalid.");
        return false;
    }

    SkAlphaType srcAlphaType = static_cast<SkAlphaType>(srcInfo.alphaType);
    SkAlphaType dstAlphaType = static_cast<SkAlphaType>(dstInfo.alphaType);
    SkColorType srcColorType = PixelFormatConvert(srcInfo.pixelFormat);
    SkColorType dstColorType = PixelFormatConvert(dstInfo.pixelFormat);
    SkImageInfo srcImageInfo = SkImageInfo::Make(srcInfo.size.width, srcInfo.size.height, srcColorType, srcAlphaType);
    SkImageInfo dstImageInfo = SkImageInfo::Make(dstInfo.size.width, dstInfo.size.height, dstColorType, dstAlphaType);

    int32_t dstRGBxSize = (dstInfo.pixelFormat == PixelFormat::RGB_888) ? GetRGBxSize(dstInfo) : NUM_1;
    auto dstRGBxPixels = std::make_unique<uint8_t[]>(dstRGBxSize);
    auto keepDstPixels = dstPixels;
    dstPixels = (dstInfo.pixelFormat == PixelFormat::RGB_888) ? &dstRGBxPixels[0] : dstPixels;
    dstRowBytes = (dstInfo.pixelFormat == PixelFormat::RGB_888) ? GetRGBxRowBytes(dstInfo) : dstRowBytes;

    int32_t srcRGBxSize = (srcInfo.pixelFormat == PixelFormat::RGB_888) ? GetRGBxSize(srcInfo) : NUM_1;
    auto srcRGBxPixels = std::make_unique<uint8_t[]>(srcRGBxSize);
    if (srcInfo.pixelFormat == PixelFormat::RGB_888) {
        RGBToRGBx(static_cast<const uint8_t*>(srcPixels), &srcRGBxPixels[0], srcRowBytes * srcInfo.size.height);
        srcPixels = &srcRGBxPixels[0];
        srcRowBytes = GetRGBxRowBytes(srcInfo);
    }
    SkPixmap srcPixmap(srcImageInfo, srcPixels, srcRowBytes);
    if (srcInfo.pixelFormat == PixelFormat::ARGB_8888) {
        uint8_t* src = static_cast<uint8_t*>(srcPixmap.writable_addr());
        ARGBToRGBA(src, src, srcRowBytes * srcInfo.size.height);
    }

    SkBitmap dstBitmap;
    if (!dstBitmap.installPixels(dstImageInfo, dstPixels, dstRowBytes)) {
        IMAGE_LOGE("WritePixelsConvert dst bitmap install pixels failed.");
        return false;
    }
    if (!dstBitmap.writePixels(srcPixmap, dstPos.x, dstPos.y)) {
        IMAGE_LOGE("WritePixelsConvert dst bitmap write pixels by source failed.");
        return false;
    }

    if (dstInfo.pixelFormat == PixelFormat::ARGB_8888) {
        uint32_t dstSize = dstRowBytes * dstInfo.size.height;
        RGBAToARGB(static_cast<uint8_t*>(dstPixels), static_cast<uint8_t*>(dstPixels), dstSize);
    } else if (dstInfo.pixelFormat == PixelFormat::RGB_888) {
        RGBxToRGB(&dstRGBxPixels[0], static_cast<uint8_t*>(keepDstPixels), dstRGBxSize);
    }

    return true;
}

bool PixelConvertAdapter::ReadPixelsConvert(const void *srcPixels, const Position &srcPos, uint32_t srcRowBytes,
                                            const ImageInfo &srcInfo, void *dstPixels, uint32_t dstRowBytes,
                                            const ImageInfo &dstInfo)
{
    // basic valid check, other parameters valid check in readPixels method
    if (srcPixels == nullptr || dstPixels == nullptr) {
        IMAGE_LOGE("src or dst pixels invalid.");
        return false;
    }
    SkAlphaType srcAlphaType = static_cast<SkAlphaType>(srcInfo.alphaType);
    SkAlphaType dstAlphaType = static_cast<SkAlphaType>(dstInfo.alphaType);
    SkColorType srcColorType = PixelFormatConvert(srcInfo.pixelFormat);
    SkColorType dstColorType = PixelFormatConvert(dstInfo.pixelFormat);
    SkImageInfo srcImageInfo = SkImageInfo::Make(srcInfo.size.width, srcInfo.size.height, srcColorType, srcAlphaType);
    SkImageInfo dstImageInfo = SkImageInfo::Make(dstInfo.size.width, dstInfo.size.height, dstColorType, dstAlphaType);

    SkBitmap srcBitmap;
    if (!srcBitmap.installPixels(srcImageInfo, const_cast<void *>(srcPixels), srcRowBytes)) {
        IMAGE_LOGE("ReadPixelsConvert src bitmap install pixels failed.");
        return false;
    }
    if (!srcBitmap.readPixels(dstImageInfo, dstPixels, dstRowBytes, srcPos.x, srcPos.y)) {
        IMAGE_LOGE("ReadPixelsConvert read dst pixels from source failed.");
        return false;
    }
    return true;
}

bool PixelConvertAdapter::EraseBitmap(const void *srcPixels, uint32_t srcRowBytes, const ImageInfo &srcInfo,
                                      uint32_t color)
{
    if (srcPixels == nullptr) {
        IMAGE_LOGE("srcPixels is null.");
        return false;
    }
    SkAlphaType srcAlphaType = static_cast<SkAlphaType>(srcInfo.alphaType);
    SkColorType srcColorType = PixelFormatConvert(srcInfo.pixelFormat);
    SkImageInfo srcImageInfo = SkImageInfo::Make(srcInfo.size.width, srcInfo.size.height, srcColorType, srcAlphaType);
    SkBitmap srcBitmap;
    if (!srcBitmap.installPixels(srcImageInfo, const_cast<void *>(srcPixels), srcRowBytes)) {
        IMAGE_LOGE("ReadPixelsConvert src bitmap install pixels failed.");
        return false;
    }
    const SkColor4f skColor = SkColor4f::FromColor(color);
    SkPaint paint;
    paint.setColor4f(skColor, SkColorSpace::MakeSRGB().get());
    paint.setBlendMode(SkBlendMode::kSrc);
    SkCanvas canvas(srcBitmap);
    canvas.drawPaint(paint);
    return true;
}

static uint8_t GetYuv420Y(uint32_t x, uint32_t y, int32_t width, const uint8_t *in)
{
    return *(in + y * width + x);
}

static uint8_t GetYuv420U(uint32_t x, uint32_t y, Size &size, PixelFormat format,
    const uint8_t *in)
{
    int32_t width = size.width;
    int32_t height = size.height;
    switch (format) {
        case PixelFormat::NV21:
            if (width & 1) {
                return *(in + y / NUM_2 * NUM_2 + width * height + (y / NUM_2) * (width - 1) + (x & ~1) + 1);
            }
            return *(in + width * height + (y / NUM_2) * width + (x & ~1) + 1);
        case PixelFormat::NV12:
            if (width & 1) {
                return *(in + y / NUM_2 * NUM_2 + width * height + (y / NUM_2) * (width - 1) + (x & ~1));
            }
            return *(in + width * height + (y / NUM_2) * width + (x & ~1));
        case PixelFormat::YU12:
            if (width & 1) {
                return *(in + y / NUM_2 + (width * height) + (y / NUM_2) * (width / NUM_2) + (x / NUM_2));
            }
            return *(in + (width * height) + (y / NUM_2) * (width / NUM_2) + (x / NUM_2));
        case PixelFormat::YV12:
            if (width & 1) {
                return *(in + height / NUM_2 + y / NUM_2 + (width * height) + (width / NUM_2) * (height / NUM_2) +
                            (y / NUM_2) * (width / NUM_2) + (x / NUM_2));
            }
            return *(in + (width * height) + (width / NUM_2) * (height / NUM_2) +
                        (y / NUM_2) * (width / NUM_2) + (x / NUM_2));
        default:
            break;
    }
    return SUCCESS;
}

static uint8_t GetYuv420V(uint32_t x, uint32_t y, Size &size, PixelFormat format,
    const uint8_t *in)
{
    int32_t width = size.width;
    int32_t height = size.height;
    switch (format) {
        case PixelFormat::NV21:
            if (width & 1) {
                return *(in + y / NUM_2 * NUM_2 + width * height + (y / NUM_2) * (width - 1) + (x & ~1));
            }
            return *(in + width * height + (y / NUM_2) * width + (x & ~1));
        case PixelFormat::NV12:
            if (width & 1) {
                return *(in + y / NUM_2 * NUM_2 + width * height + (y / NUM_2) * (width - 1) + (x & ~1) + 1);
            }
            return *(in + width * height + (y / NUM_2) * width + (x & ~1) + 1);
        case PixelFormat::YU12:
            if (width & 1) {
                return *(in + height / NUM_2 + y / NUM_2 + (width * height) + (width / NUM_2) * (height / NUM_2) +
                            (y / NUM_2) * (width / NUM_2) + (x / NUM_2));
            }
            return *(in + (width * height) + (width / NUM_2) * (height / NUM_2) +
                        (y / NUM_2) * (width / NUM_2) + (x / NUM_2));
            break;
        case PixelFormat::YV12:
            if (width & 1) {
                return *(in + y / NUM_2 + (width * height) + (y / NUM_2) * (width / NUM_2) + (x / NUM_2));
            }
            return *(in + (width * height) + (y / NUM_2) * (width / NUM_2) + (x / NUM_2));
        default:
            break;
    }
    return SUCCESS;
}

bool PixelConvertAdapter::YUV420ToRGB888(const uint8_t *in, uint8_t *out, int32_t width, int32_t height,
    PixelFormat pixelFormat)
{
    if (!in || !out || width <= 0 || height == 0) {
        return false;
    }
    Size size = {width, height};
    for (int32_t i = 0; i < height; i++) {
        for (int32_t j = 0; j < width; j++) {
            uint8_t Y = GetYuv420Y(j, i, width, in);
            uint8_t U = GetYuv420U(j, i, size, pixelFormat, in);
            uint8_t V = GetYuv420V(j, i, size, pixelFormat, in);

            int32_t colorR = Y + YUV_TO_RGB888_PARAM_1 * (V - INT_128);
            int32_t colorG = Y - YUV_TO_RGB888_PARAM_2 * (U - INT_128) - YUV_TO_RGB888_PARAM_3 * (V - INT_128);
            int32_t colorB = Y + YUV_TO_RGB888_PARAM_4 * (U - INT_128);

            colorR = colorR > INT_255 ? INT_255 : (colorR < 0 ? 0 : colorR);
            colorG = colorG > INT_255 ? INT_255 : (colorG < 0 ? 0 : colorG);
            colorB = colorB > INT_255 ? INT_255 : (colorB < 0 ? 0 : colorB);

            *out++ = colorB;
            *out++ = colorG;
            *out++ = colorR;
        }
    }
    return true;
}

} // namespace Media
} // namespace OHOS
