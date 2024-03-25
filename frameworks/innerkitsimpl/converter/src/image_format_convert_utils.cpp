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

#include "image_format_convert_utils.h"

#include <cmath>
#include <iostream>
#include <cstring>
#include "securec.h"
#include <map>
#ifdef LIBYUV
#include "libyuv/convert.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif
#include "libswscale/swscale.h"
#include "libavutil/opt.h"
#include "libavutil/imgutils.h"
#include "libavcodec/avcodec.h"
#ifdef __cplusplus
}
#endif

namespace {
    constexpr uint32_t NUM_0 = 0;
    constexpr uint32_t NUM_1 = 1;
    constexpr uint32_t NUM_2 = 2;
    constexpr uint32_t NUM_3 = 3;
    constexpr uint32_t NUM_4 = 4;
    constexpr uint32_t NUM_8 = 8;
}

#undef LOG_TAG
#define LOG_TAG "ImageFormatConvert"
namespace OHOS {
namespace Media {


#ifdef LIBYUV
using namespace libyuv;
std::map<ColorSpace, const struct YuvConstants*> colorSpaceMap = {
    {ColorSpace::UNKNOWN, nullptr},
    {ColorSpace::DISPLAY_P3, &kYuvJPEGConstants},
    {ColorSpace::SRGB, &kYuvJPEGConstants},
    {ColorSpace::LINEAR_SRGB, &kYuvJPEGConstants},
    {ColorSpace::EXTENDED_SRGB, &kYuvJPEGConstants},
    {ColorSpace::LINEAR_EXTENDED_SRGB, &kYuvJPEGConstants},
    {ColorSpace::GENERIC_XYZ, &kYuvJPEGConstants},
    {ColorSpace::GENERIC_LAB, &kYuvJPEGConstants},
    {ColorSpace::ACES, &kYuvJPEGConstants},
    {ColorSpace::ACES_CG, &kYuvJPEGConstants},
    {ColorSpace::ADOBE_RGB_1998, &kYuvJPEGConstants},
    {ColorSpace::DCI_P3, &kYuvJPEGConstants},
    {ColorSpace::ITU_709, &kYuvF709Constants},
    {ColorSpace::ITU_2020, &kYuvV2020Constants},
    {ColorSpace::ROMM_RGB, &kYuvJPEGConstants},
    {ColorSpace::NTSC_1953, &kYuvJPEGConstants},
    {ColorSpace::SMPTE_C, &kYuvJPEGConstants},
};

const struct YuvConstants* mapColorSPaceToYuvConstants(ColorSpace colorSpace)
{
    auto it = colorSpaceMap.find(colorSpace);
    if (it != colorSpaceMap.end()) {
        return it->second;
    }
    return nullptr;
}
#endif

static std::map<PixelFormat,AVPixelFormat> PixelFormatMap = {
    {PixelFormat::UNKNOWN,AV_PIX_FMT_NONE},
    {PixelFormat::NV12,AV_PIX_FMT_NV12},
    {PixelFormat::NV21,AV_PIX_FMT_NV21},
    {PixelFormat::RGB_565,AV_PIX_FMT_RGB565BE},
    {PixelFormat::RGBA_8888,AV_PIX_FMT_RGBA},
    {PixelFormat::BGRA_8888,AV_PIX_FMT_BGRA},
    {PixelFormat::ARGB_8888,AV_PIX_FMT_ARGB},
    {PixelFormat::RGBA_F16,AV_PIX_FMT_RGBA64BE},
    {PixelFormat::RGB_888,AV_PIX_FMT_RGB24},
};

AVPixelFormat findPixelFormat(PixelFormat format) {
    if (PixelFormatMap.find(format) != PixelFormatMap.end()) {
        return PixelFormatMap[format];
    }
    else {
        return AV_PIX_FMT_NONE;
    }
}

static void NV12ToRGB565Manual(const uint8_t *srcBuffer, const Size &imageSize, uint8_t **destBuffer)
{
    AVPixelFormat srcFormat = findPixelFormat(PixelFormat::NV12);
    AVPixelFormat dstFormat = findPixelFormat(PixelFormat::RGB_565);
    SwsContext *swsContext = sws_getContext(imageSize.width, imageSize.height, srcFormat, imageSize.width,
        imageSize.height, dstFormat, SWS_BILINEAR, nullptr, nullptr, nullptr);
    if (!swsContext) {
        IMAGE_LOGD("Error to create SwsContext.");
        sws_freeContext(swsContext);
        return false;
    }

    int widthEvent = (imageSize.width % NUM_2 == NUM_0) ? (imageSize.width) : (imageSize.width + NUM_1);
    const uint8_t *srcSlice[NUM_2] = {srcBuffer, srcBuffer + imageSize.width * imageSize.height};
    const int srcStride[NUM_2] = {static_cast<int>(imageSize.width),static_cast<int>(widthEvent)};
    uint8_t *dstSlice[NUM_1] = {*destBuffer};
    const int dstStride[NUM_1] = {static_cast<int>(imageSize.width * NUM_2)};
    int pixelFormatConvert = sws_scale(swsContext, srcSlice, srcStride, NUM_0, imageSize.height, dstSlice, dstStride);
    if (pixelFormatConvert == NUM_0) {
        delete[] destBuffer;
    }
    sws_freeContext(swsContext);
}

static void NV21ToNV12Manual(const uint8_t *srcBuffer, const Size &imageSize, uint8_t **destBuffer)
{
    AVPixelFormat srcFormat = findPixelFormat(PixelFormat::NV21);
    AVPixelFormat dstFormat = findPixelFormat(PixelFormat::NV12);
    SwsContext *swsContext = sws_getContext(imageSize.width, imageSize.height, srcFormat, imageSize.width,
        imageSize.height,dstFormat,SWS_BILINEAR,nullptr,nullptr,nullptr);
    if (!swsContext) {
        IMAGE_LOGD("Error to create SwsContext.");
        sws_freeContext(swsContext);
    }

    int widthEvent = (imageSize.width % NUM_2 == NUM_0) ? (imageSize.width) : (imageSize.width + NUM_1);
    const uint8_t *srcSlice[NUM_2] = {srcBuffer, srcBuffer + imageSize.width * imageSize.height};
    const int srcStride[NUM_2] = {static_cast<int>(imageSize.width),static_cast<int>(widthEvent)};
    uint8_t *dstSlice[NUM_2] = {*destBuffer, *destBuffer + imageSize.width * imageSize.height};
    const int dstStride[NUM_2] = {static_cast<int>(imageSize.width),static_cast<int>(widthEvent)};
    int pixelFormatConvert = sws_scale(swsContext,srcSlice,srcStride,0,imageSize.height,dstSlice,dstStride);
    if (pixelFormatConvert == NUM_0) {
        delete[] destBuffer;
    }
    sws_freeContext(swsContext);
}

static void NV12ToNV21Manual(const uint8_t *srcBuffer, const Size &imageSize, uint8_t **destBuffer)
{
    AVPixelFormat srcFormat = findPixelFormat(PixelFormat::NV12);
    AVPixelFormat dstFormat = findPixelFormat(PixelFormat::NV21);
    SwsContext *swsContext = sws_getContext(imageSize.width, imageSize.height, srcFormat, imageSize.width,
        imageSize.height,dstFormat,SWS_BILINEAR,nullptr,nullptr,nullptr);
    if (!swsContext) {
        IMAGE_LOGD("Error to create SwsContext.");
        sws_freeContext(swsContext);
    }

    int widthEvent = (imageSize.width % NUM_2 == NUM_0) ? (imageSize.width) : (imageSize.width + NUM_1);
    const uint8_t *srcSlice[NUM_2] = {srcBuffer, srcBuffer + imageSize.width * imageSize.height};
    const int srcStride[NUM_2] = {static_cast<int>(imageSize.width),static_cast<int>(widthEvent)};
    uint8_t *dstSlice[NUM_2] = {*destBuffer, *destBuffer + imageSize.width * imageSize.height};
    const int dstStride[NUM_2] = {static_cast<int>(imageSize.width),static_cast<int>(widthEvent)};
    int pixelFormatConvert = sws_scale(swsContext,srcSlice,srcStride,0,imageSize.height,dstSlice,dstStride);
    if (pixelFormatConvert == NUM_0) {
        delete[] destBuffer;
    }
    sws_freeContext(swsContext);
}

static void RGB565ToNV12Manual(const uint8_t *srcBuffer, const Size &imageSize, uint8_t **destBuffer)
{
    AVPixelFormat srcFormat = findPixelFormat(PixelFormat::RGB_565);
    AVPixelFormat dstFormat = findPixelFormat(PixelFormat::NV12);
    SwsContext *swsContext = sws_getContext(imageSize.width, imageSize.height, srcFormat, imageSize.width,
        imageSize.height,dstFormat,SWS_BILINEAR,nullptr,nullptr,nullptr);
    if (!swsContext) {
        IMAGE_LOGD("Error to create SwsContext.");
        sws_freeContext(swsContext);
    }

    int widthEvent = (imageSize.width % NUM_2 == NUM_0) ? (imageSize.width) : (imageSize.width + NUM_1);
    const uint8_t *srcSlice[NUM_1] = {srcBuffer};
    const int srcStride[NUM_1] = {static_cast<int>(imageSize.width * NUM_2)};
    uint8_t *dstSlice[NUM_2] = {*destBuffer, *destBuffer + imageSize.width * imageSize.height};
    const int dstStride[NUM_2] = {static_cast<int>(imageSize.width),static_cast<int>(widthEvent)};
    int pixelFormatConvert = sws_scale(swsContext,srcSlice,srcStride,0,imageSize.height,dstSlice,dstStride);
    if (pixelFormatConvert == NUM_0) {
        delete[] destBuffer;
    }
    sws_freeContext(swsContext);
}

static void RGB565ToNV21Manual(const uint8_t *srcBuffer, const Size &imageSize, uint8_t **destBuffer)
{
    AVPixelFormat srcFormat = findPixelFormat(PixelFormat::RGB_565);
    AVPixelFormat dstFormat = findPixelFormat(PixelFormat::NV21);
    SwsContext *swsContext = sws_getContext(imageSize.width, imageSize.height, srcFormat, imageSize.width,
        imageSize.height, dstFormat, SWS_BILINEAR, nullptr, nullptr, nullptr);
    if (!swsContext) {
        IMAGE_LOGD("Error to create SwsContext.");
        sws_freeContext(swsContext);
    }

    int widthEvent = (imageSize.width % NUM_2 == NUM_0) ? (imageSize.width) : (imageSize.width + NUM_1);
    const uint8_t *srcSlice[NUM_1] = {srcBuffer};
    const int srcStride[NUM_1] = {static_cast<int>(imageSize.width * NUM_2)};
    uint8_t *dstSlice[NUM_2] = {*destBuffer, *destBuffer + imageSize.width * imageSize.height};
    const int dstStride[NUM_2] = {static_cast<int>(imageSize.width),static_cast<int>(widthEvent)};
    int pixelFormatConvert = sws_scale(swsContext, srcSlice, srcStride, NUM_0, imageSize.height, dstSlice, dstStride);
    if (pixelFormatConvert == NUM_0) {
        delete[] destBuffer;
    }
    sws_freeContext(swsContext);
}

static void BGRAToNV21Manual(const uint8_t *srcBuffer, const Size &imageSize, uint8_t **destBuffer)
{
    AVPixelFormat srcFormat = findPixelFormat(PixelFormat::BGRA_8888);
    AVPixelFormat dstFormat = findPixelFormat(PixelFormat::NV21);
    SwsContext *swsContext = sws_getContext(imageSize.width, imageSize.height, srcFormat, imageSize.width,
        imageSize.height, dstFormat, SWS_BILINEAR, nullptr, nullptr, nullptr);
    if (!swsContext) {
        IMAGE_LOGD("Error to create SwsContext.");
        sws_freeContext(swsContext);
    }

    int widthEvent = (imageSize.width % NUM_2 == NUM_0) ? (imageSize.width) : (imageSize.width + NUM_1);
    const uint8_t *srcSlice[NUM_1] = {srcBuffer};
    const int srcStride[NUM_1] = {static_cast<int>(imageSize.width * NUM_4)};
    uint8_t *dstSlice[NUM_2] = {*destBuffer, *destBuffer + imageSize.width * imageSize.height};
    const int dstStride[NUM_2] = {static_cast<int>(imageSize.width),static_cast<int>(widthEvent)};
    int pixelFormatConvert = sws_scale(swsContext, srcSlice, srcStride, NUM_0, imageSize.height, dstSlice, dstStride);
    if (pixelFormatConvert == NUM_0) {
        delete[] destBuffer;
    }
    sws_freeContext(swsContext);
}

static void NV12ToRGBAManual(const uint8_t *srcBuffer, const Size &imageSize, uint8_t **destBuffer)
{
    AVPixelFormat srcFormat = findPixelFormat(PixelFormat::NV12);
    AVPixelFormat dstFormat = findPixelFormat(PixelFormat::RGBA_8888);
    SwsContext *swsContext = sws_getContext(imageSize.width, imageSize.height, srcFormat, imageSize.width,
        imageSize.height, dstFormat, SWS_BILINEAR, nullptr, nullptr, nullptr);
    if (!swsContext) {
        IMAGE_LOGD("Error to create SwsContext.");
        sws_freeContext(swsContext);
    }

    int widthEvent = (imageSize.width % NUM_2 == NUM_0) ? (imageSize.width) : (imageSize.width + NUM_1);
    const uint8_t *srcSlice[NUM_2] = {srcBuffer, srcBuffer + imageSize.width * imageSize.height} ;
    const int srcStride[NUM_2] = {static_cast<int>(imageSize.width),static_cast<int>(widthEvent)};
    uint8_t *dstSlice[NUM_1] = {*destBuffer};
    const int dstStride[NUM_1] = {static_cast<int>(imageSize.width * NUM_4)};
    int pixelFormatConvert = sws_scale(swsContext, srcSlice, srcStride, NUM_0,imageSize.height, dstSlice, dstStride);
    if (pixelFormatConvert == NUM_0) {
        delete[] destBuffer;
    }
    sws_freeContext(swsContext);
}

static void NV12ToBGRAManual(const uint8_t *srcBuffer, const Size &imageSize, uint8_t **destBuffer)
{
    AVPixelFormat srcFormat = findPixelFormat(PixelFormat::NV12);
    AVPixelFormat dstFormat = findPixelFormat(PixelFormat::BGRA_8888);
    SwsContext *swsContext = sws_getContext(imageSize.width, imageSize.height, srcFormat, imageSize.width,
        imageSize.height, dstFormat, SWS_BILINEAR, nullptr, nullptr, nullptr);
    if (!swsContext) {
        IMAGE_LOGD("Error to create SwsContext.");
        sws_freeContext(swsContext);
    }

    int widthEvent = (imageSize.width % NUM_2 == NUM_0) ? (imageSize.width) : (imageSize.width + NUM_1);
    const uint8_t *srcSlice[NUM_2] = {srcBuffer, srcBuffer + imageSize.width * imageSize.height} ;
    const int srcStride[NUM_2] = {static_cast<int>(imageSize.width),static_cast<int>(widthEvent)};
    uint8_t *dstSlice[NUM_1] = {*destBuffer};
    const int dstStride[NUM_1] = {static_cast<int>(imageSize.width * NUM_4)};
    int pixelFormatConvert = sws_scale(swsContext, srcSlice, srcStride, NUM_0, imageSize.height, dstSlice, dstStride);
    if (pixelFormatConvert == NUM_0) {
        delete[] destBuffer;
    }
    sws_freeContext(swsContext);
}

static void RGBAToNV12Manual(const uint8_t *srcBuffer, const Size &imageSize, uint8_t **destBuffer)
{
    AVPixelFormat srcFormat = findPixelFormat(PixelFormat::RGBA_8888);
    AVPixelFormat dstFormat = findPixelFormat(PixelFormat::NV12);
    SwsContext *swsContext = sws_getContext(imageSize.width, imageSize.height, srcFormat, imageSize.width,
        imageSize.height, dstFormat, SWS_BILINEAR, nullptr, nullptr, nullptr);
    if (!swsContext) {
        IMAGE_LOGD("Error to create SwsContext.");
        sws_freeContext(swsContext);
    }

    int widthEvent = (imageSize.width % NUM_2 == NUM_0) ? (imageSize.width) : (imageSize.width + NUM_1);
    const uint8_t *srcSlice[NUM_1] = {srcBuffer};
    const int srcStride[NUM_1] = {static_cast<int>(imageSize.width * NUM_4)};
    uint8_t *dstSlice[NUM_2] = {*destBuffer, *destBuffer + imageSize.width * imageSize.height};
    const int dstStride[NUM_2] = {static_cast<int>(imageSize.width),static_cast<int>(widthEvent)};
    int pixelFormatConvert = sws_scale(swsContext, srcSlice, srcStride, NUM_0, imageSize.height, dstSlice, dstStride);
    if (pixelFormatConvert == NUM_0) {
        delete[] destBuffer;
    }
    sws_freeContext(swsContext);
}

static void RGBAToNV21Manual(const uint8_t *srcBuffer, const Size &imageSize, uint8_t **destBuffer)
{
    AVPixelFormat srcFormat = findPixelFormat(PixelFormat::RGBA_8888);
    AVPixelFormat dstFormat = findPixelFormat(PixelFormat::NV21);
    SwsContext *swsContext = sws_getContext(imageSize.width, imageSize.height, srcFormat, imageSize.width,
        imageSize.height, dstFormat, SWS_BILINEAR, nullptr, nullptr, nullptr);
    if (!swsContext) {
        IMAGE_LOGD("Error to create SwsContext.");
        sws_freeContext(swsContext);
    }

    int widthEvent = (imageSize.width % NUM_2 == NUM_0) ? (imageSize.width) : (imageSize.width + NUM_1);
    const uint8_t *srcSlice[NUM_1] = {srcBuffer};
    const int srcStride[NUM_1] = {static_cast<int>(imageSize.width * NUM_4)};
    uint8_t *dstSlice[NUM_2] = {*destBuffer, *destBuffer + imageSize.width * imageSize.height};
    const int dstStride[NUM_2] = {static_cast<int>(imageSize.width),static_cast<int>(widthEvent)};
    int pixelFormatConvert = sws_scale(swsContext, srcSlice, srcStride, NUM_0, imageSize.height, dstSlice, dstStride);
    if (pixelFormatConvert == NUM_0) {
        delete[] destBuffer;
    }
    sws_freeContext(swsContext);
}

static void RGBToNV21Manual(const uint8_t *srcBuffer, const Size &imageSize, uint8_t **destBuffer)
{
    AVPixelFormat srcFormat = findPixelFormat(PixelFormat::RGB_888);
    AVPixelFormat dstFormat = findPixelFormat(PixelFormat::NV21);
    SwsContext *swsContext = sws_getContext(imageSize.width, imageSize.height, srcFormat, imageSize.width,
        imageSize.height, dstFormat, SWS_BILINEAR, nullptr, nullptr, nullptr);
    if (!swsContext) {
        IMAGE_LOGD("Error to create SwsContext.");
        sws_freeContext(swsContext);
    }

    int widthEvent = (imageSize.width % NUM_2 == NUM_0) ? (imageSize.width) : (imageSize.width + NUM_1);
    const uint8_t *srcSlice[NUM_1] = {srcBuffer};
    const int srcStride[NUM_1] = {static_cast<int>(imageSize.width * NUM_3)};
    uint8_t *dstSlice[NUM_2] = {*destBuffer, *destBuffer + imageSize.width * imageSize.height};
    const int dstStride[NUM_2] = {static_cast<int>(imageSize.width),static_cast<int>(widthEvent)};
    int pixelFormatConvert = sws_scale(swsContext, srcSlice, srcStride, NUM_0, imageSize.height, dstSlice, dstStride);
    if (pixelFormatConvert == NUM_0) {
        delete[] destBuffer;
    }
    sws_freeContext(swsContext);
}

static void NV21ToRGBAManual(const uint8_t *srcBuffer, const Size &imageSize, uint8_t **destBuffer)
{
    AVPixelFormat srcFormat = findPixelFormat(PixelFormat::NV21);
    AVPixelFormat dstFormat = findPixelFormat(PixelFormat::RGBA_8888);
    SwsContext *swsContext = sws_getContext(imageSize.width, imageSize.height, srcFormat, imageSize.width,
        imageSize.height, dstFormat, SWS_BILINEAR, nullptr, nullptr, nullptr);
    if (!swsContext) {
        IMAGE_LOGD("Error to create SwsContext.");
        sws_freeContext(swsContext);
    }

    int widthEvent = (imageSize.width % NUM_2 == NUM_0) ? (imageSize.width) : (imageSize.width + NUM_1);
    const uint8_t *srcSlice[NUM_2] = {srcBuffer, srcBuffer + imageSize.width * imageSize.height} ;
    const int srcStride[NUM_2] = {static_cast<int>(imageSize.width),static_cast<int>(widthEvent)};
    uint8_t *dstSlice[NUM_1] = {*destBuffer};
    const int dstStride[NUM_1] = {static_cast<int>(imageSize.width * NUM_4)};
    int pixelFormatConvert = sws_scale(swsContext, srcSlice, srcStride, NUM_0, imageSize.height, dstSlice, dstStride);
    if (pixelFormatConvert == NUM_0) {
        delete[] destBuffer;
    }
    sws_freeContext(swsContext);
}

static void NV21ToRGB565Manual(const uint8_t *srcBuffer, const Size &imageSize, uint8_t **destBuffer)
{
    AVPixelFormat srcFormat = findPixelFormat(PixelFormat::NV21);
    AVPixelFormat dstFormat = findPixelFormat(PixelFormat::RGB_565);
    SwsContext *swsContext = sws_getContext(imageSize.width, imageSize.height, srcFormat, imageSize.width,
        imageSize.height, dstFormat, SWS_BILINEAR, nullptr, nullptr, nullptr);
    if (!swsContext) {
        IMAGE_LOGD("Error to create SwsContext.");
        sws_freeContext(swsContext);
    }

    int widthEvent = (imageSize.width % NUM_2 == NUM_0) ? (imageSize.width) : (imageSize.width + NUM_1);
    const uint8_t *srcSlice[NUM_2] = {srcBuffer, srcBuffer + imageSize.width * imageSize.height} ;
    const int srcStride[NUM_2] = {static_cast<int>(imageSize.width),static_cast<int>(widthEvent)};
    uint8_t *dstSlice[NUM_1] = {*destBuffer};
    const int dstStride[NUM_1] = {static_cast<int>(imageSize.width * NUM_2)};
    int pixelFormatConvert = sws_scale(swsContext, srcSlice, srcStride, NUM_0, imageSize.height, dstSlice, dstStride);
    if (pixelFormatConvert == NUM_0) {
        delete[] destBuffer;
    }
    sws_freeContext(swsContext);
}

static void NV21ToBGRAManual(const uint8_t *srcBuffer, const Size &imageSize, uint8_t **destBuffer)
{
    AVPixelFormat srcFormat = findPixelFormat(PixelFormat::NV21);
    AVPixelFormat dstFormat = findPixelFormat(PixelFormat::BGRA_8888);
    SwsContext *swsContext = sws_getContext(imageSize.width, imageSize.height, srcFormat, imageSize.width,
        imageSize.height, dstFormat, SWS_BILINEAR, nullptr, nullptr, nullptr);
    if (!swsContext) {
        IMAGE_LOGD("Error to create SwsContext.");
        sws_freeContext(swsContext);
    }

    int widthEvent = (imageSize.width % NUM_2 == NUM_0) ? (imageSize.width) : (imageSize.width + NUM_1);
    const uint8_t *srcSlice[NUM_2] = {srcBuffer, srcBuffer + imageSize.width * imageSize.height} ;
    const int srcStride[NUM_2] = {static_cast<int>(imageSize.width),static_cast<int>(widthEvent)};
    uint8_t *dstSlice[NUM_1] = {*destBuffer};
    const int dstStride[NUM_1] = {static_cast<int>(imageSize.width * NUM_4)};
    int pixelFormatConvert = sws_scale(swsContext, srcSlice, srcStride, NUM_0, imageSize.height, dstSlice, dstStride);
    if (pixelFormatConvert == NUM_0) {
        delete[] destBuffer;
    }
    sws_freeContext(swsContext);
}

static void RGBToNV12Manual(const uint8_t *srcBuffer, const Size &imageSize, uint8_t **destBuffer)
{
    AVPixelFormat srcFormat = findPixelFormat(PixelFormat::RGB_888);
    AVPixelFormat dstFormat = findPixelFormat(PixelFormat::NV12);
    SwsContext *swsContext = sws_getContext(imageSize.width, imageSize.height, srcFormat, imageSize.width,
        imageSize.height, dstFormat, SWS_BILINEAR, nullptr, nullptr, nullptr);
    if (!swsContext) {
        IMAGE_LOGD("Error to create SwsContext.");
        sws_freeContext(swsContext);
    }

    int widthEvent = (imageSize.width % NUM_2 == NUM_0) ? (imageSize.width) : (imageSize.width + NUM_1);
    const uint8_t *srcSlice[NUM_1] = {srcBuffer};
    const int srcStride[NUM_1] = {static_cast<int>(imageSize.width * NUM_3)};
    uint8_t *dstSlice[NUM_2] = {*destBuffer, *destBuffer + imageSize.width * imageSize.height};
    const int dstStride[NUM_2] = {static_cast<int>(imageSize.width),static_cast<int>(widthEvent)};
    int pixelFormatConvert = sws_scale(swsContext, srcSlice, srcStride, NUM_0, imageSize.height, dstSlice, dstStride);
    if (pixelFormatConvert == NUM_0) {
        delete[] destBuffer;
    }
    sws_freeContext(swsContext);
}

static void BGRAToNV12Manual(const uint8_t *srcBuffer, const Size &imageSize, uint8_t **destBuffer)
{
    AVPixelFormat srcFormat = findPixelFormat(PixelFormat::BGRA_8888);
    AVPixelFormat dstFormat = findPixelFormat(PixelFormat::NV12);
    SwsContext *swsContext = sws_getContext(imageSize.width, imageSize.height, srcFormat, imageSize.width,
        imageSize.height, dstFormat, SWS_BILINEAR, nullptr, nullptr, nullptr);
    if (!swsContext) {
        IMAGE_LOGD("Error to create SwsContext.");
        sws_freeContext(swsContext);
    }

    int widthEvent = (imageSize.width % NUM_2 == NUM_0) ? (imageSize.width) : (imageSize.width + NUM_1);
    const uint8_t *srcSlice[NUM_1] = {srcBuffer};
    const int srcStride[NUM_1] = {static_cast<int>(imageSize.width * NUM_4)};
    uint8_t *dstSlice[NUM_2] = {*destBuffer, *destBuffer + imageSize.width * imageSize.height};
    const int dstStride[NUM_2] = {static_cast<int>(imageSize.width),static_cast<int>(widthEvent)};
    int pixelFormatConvert = sws_scale(swsContext, srcSlice, srcStride, NUM_0, imageSize.height, dstSlice, dstStride);
    if (pixelFormatConvert == NUM_0) {
        delete[] destBuffer;
    }
    sws_freeContext(swsContext);
}

bool NV12ToRGB565(const uint8_t *srcBuffer, const Size &imageSize, uint8_t **destBuffer,
                  size_t &destBufferSize, [[maybe_unused]]ColorSpace colorSpace)
{
    if (srcBuffer == nullptr || destBuffer == nullptr || imageSize.width < NUM_0 || imageSize.height < NUM_0) {
        return false;
    }
    destBufferSize = static_cast<size_t>(imageSize.width * imageSize.height * NUM_2);
    if (destBufferSize <= NUM_0) {
        IMAGE_LOGD("Invalid destination buffer size calculation!");
        return false;
    }
    *destBuffer = new(std::nothrow) uint8_t[destBufferSize]();
    if (*destBuffer == nullptr) {
        IMAGE_LOGD("apply space for dest buffer failed!");
        return false;
    }
#ifdef LIBYUV
    const struct YuvConstants* yuvConstants = mapColorSPaceToYuvConstants(colorSpace);
    int dstStrideRGB565 = imageSize.width * NUM_2;
    int widthEven = (imageSize.width % NUM_2 == NUM_0) ? (imageSize.width) : (imageSize.width + NUM_1) / NUM_2 * NUM_2;
    int srcStrideUV = widthEven;
    libyuv::NV12ToRGB565Matrix(srcY, imageSize.width, srcBuffer + imageSize.width * imageSize.height ,widthEven,
        *destBuffer, dstStrideRGB565, yuvConstants, imageSize.width, imageSize.height);
#else
    NV12ToRGB565Manual(srcBuffer, imageSize, destBuffer);
#endif
    return true;
}

bool RGBAF16ToNV21(const uint8_t *srcBuffer, const Size &imageSize, uint8_t **destBuffer,
                   size_t &destBufferSize, [[maybe_unused]]ColorSpace colorSpace)
{
    if (srcBuffer == nullptr || destBuffer == nullptr || imageSize.width < NUM_0 || imageSize.height < NUM_0) {
        return false;
    }
    uint32_t frameSize = imageSize.width * imageSize.height;
    destBufferSize = frameSize + (((imageSize.width + NUM_1) / NUM_2 * (imageSize.height + NUM_1) / NUM_2) * NUM_2);
    if (destBufferSize <= NUM_0) {
        IMAGE_LOGD("Invalid destination buffer size calculation!");
        return false;
    }
    *destBuffer = new(std::nothrow) uint8_t[destBufferSize]();
    if (*destBuffer == nullptr) {
        IMAGE_LOGD("apply space for dest buffer failed!");
        return false;
    }

    AVPixelFormat srcFormat = findPixelFormat(PixelFormat::RGBA_F16);
    AVPixelFormat dstFormat = findPixelFormat(PixelFormat::NV21);
    SwsContext *swsContext = sws_getContext(imageSize.width, imageSize.height, srcFormat, imageSize.width,
        imageSize.height,dstFormat,SWS_BILINEAR,nullptr,nullptr,nullptr);
    if (!swsContext) {
        IMAGE_LOGD("Error to create SwsContext.");
        sws_freeContext(swsContext);
    }

    int widthEvent = (imageSize.width % NUM_2 == NUM_0) ? (imageSize.width) : (imageSize.width + NUM_1);
    const uint8_t *srcSlice[NUM_1] = {srcBuffer};
    const int srcStride[NUM_1] = {static_cast<int>(imageSize.width * NUM_8)};
    uint8_t *dstSlice[NUM_2] = {*destBuffer, *destBuffer + imageSize.width * imageSize.height};
    const int dstStride[NUM_2] = {static_cast<int>(imageSize.width),static_cast<int>(widthEvent)};
    int pixelFormatConvert = sws_scale(swsContext,srcSlice,srcStride,0,imageSize.height,dstSlice,dstStride);
    if (pixelFormatConvert == NUM_0) {
        delete[] destBuffer;
    }
    sws_freeContext(swsContext);
    return true;
}

bool NV21ToNV12(const uint8_t *srcBuffer, const Size &imageSize, uint8_t **destBuffer,
                size_t &destBufferSize, [[maybe_unused]]ColorSpace colorSpace)
{
    if (srcBuffer == nullptr || destBuffer == nullptr || imageSize.width < NUM_0 || imageSize.height < NUM_0) {
        return false;
    }
    int32_t width = imageSize.width;
    int32_t height = imageSize.height;
    destBufferSize = width * height + ((width + NUM_1) / NUM_2) * ((height + NUM_1) / NUM_2) * NUM_2;
    if (destBufferSize <= NUM_0) {
        IMAGE_LOGD("Invalid destination buffer size calculation!");
        return false;
    }
    *destBuffer = new uint8_t[destBufferSize];
#ifdef LIBYUV
    int32_t i420_buffer_size = destBufferSize;
    if (i420_buffer_size <= NUM_0) {
        IMAGE_LOGD("Invalid destination buffer size calculation!");
        return false;
    }
    std::unique_ptr<uint8_t[]> i420_buffer(new uint8_t[i420_buffer_size]);
    libyuv::NV21ToI420(srcBuffer, width, srcBuffer + width * height, ((width + NUM_1) / NUM_2) * NUM_2, i420_buffer,
        width, i420_buffer + width * height, (width + NUM_1) / NUM_2, i420_buffer + width * height +
        ((width + NUM_1) / NUM_2) * ((height + NUM_1) / NUM_2), (width + NUM_1) / NUM_2, width, height);
    libyuv::I420ToNV12(i420_buffer, width, i420_buffer + width * height, (width + NUM_1) / NUM_2,
        i420_buffer + width * height + ((width + NUM_1) / NUM_2) * ((height + NUM_1) / NUM_2), (width + NUM_1) / NUM_2,
        *destBuffer, width, *destBuffer + width * height, ((width + NUM_1) / NUM_2) * NUM_2, width, height);
#else
    NV21ToNV12Manual(srcBuffer, imageSize, destBuffer);
#endif
    return true;
}

bool NV21ToRGBAF16(const uint8_t *srcBuffer, const Size &imageSize, uint8_t **destBuffer,
                   size_t &destBufferSize, [[maybe_unused]]ColorSpace colorSpace)
{
    if (srcBuffer == nullptr || destBuffer == nullptr || imageSize.width < NUM_0 || imageSize.height < NUM_0) {
        return false;
    }
    uint32_t frameSize = imageSize.width * imageSize.height;
    destBufferSize = frameSize * sizeof(uint64_t);
    if (destBufferSize <= NUM_0) {
        IMAGE_LOGD("Invalid destination buffer size calculation!");
        return false;
    }
    *destBuffer = new(std::nothrow) uint8_t[destBufferSize]();
    if (*destBuffer == nullptr) {
        IMAGE_LOGD("apply space for dest buffer failed!");
        return false;
    }
    AVPixelFormat srcFormat = findPixelFormat(PixelFormat::NV21);
    AVPixelFormat dstFormat = findPixelFormat(PixelFormat::RGBA_F16);
    SwsContext *swsContext = sws_getContext(imageSize.width, imageSize.height, srcFormat, imageSize.width,
        imageSize.height, dstFormat, SWS_BILINEAR, nullptr, nullptr, nullptr);
    if (!swsContext) {
        IMAGE_LOGD("Error to create SwsContext.");
        sws_freeContext(swsContext);
    }
    
    int widthEvent = (imageSize.width % NUM_2 == NUM_0) ? (imageSize.width) : (imageSize.width + NUM_1);
    const uint8_t *srcSlice[NUM_2] = {srcBuffer, srcBuffer + imageSize.width * imageSize.height};
    const int srcStride[NUM_2] = {static_cast<int>(imageSize.width),static_cast<int>(widthEvent)};
    uint8_t *dstSlice[NUM_1] = {*destBuffer};
    const int dstStride[NUM_1] = {static_cast<int>(imageSize.width * NUM_8)};
    int pixelFormatConvert = sws_scale(swsContext, srcSlice, srcStride, NUM_0, imageSize.height, dstSlice, dstStride);
    if (pixelFormatConvert == NUM_0) {
        delete[] destBuffer;
    }
    sws_freeContext(swsContext);
    return true;
}

bool NV12ToNV21(const uint8_t *srcBuffer, const Size &imageSize, uint8_t **destBuffer,
                size_t &destBufferSize, [[maybe_unused]]ColorSpace colorSpace)
{
    if (srcBuffer == nullptr || destBuffer == nullptr || imageSize.width < 0 || imageSize.height < 0) {
        IMAGE_LOGD("apply space for dest buffer failed!");
        return false;
    }
    destBufferSize = static_cast<size_t>(imageSize.width * imageSize.height +
        ((imageSize.width + NUM_1) / NUM_2) *((imageSize.height + NUM_1) / NUM_2) * NUM_2);
    if (destBufferSize <= NUM_0) {
        IMAGE_LOGD("Invalid destination buffer size calculation!");
        return false;
    }
    *destBuffer = new(std::nothrow) uint8_t[destBufferSize]();
    if (*destBuffer == nullptr) {
        IMAGE_LOGD("apply space for dest buffer failed!");
        return false;
    }
#ifdef LIBYUV
    std::unique_ptr<uint8_t[]>yu12Buffer(new(std::nothrow) uint8_t[imageSize.width * imageSize.height +
        ((imageSize.width + NUM_1) / NUM_2) * ((imageSize.height + NUM_1) / NUM_2) * NUM_2]());
    if (yu12Buffer == nullptr) {
        IMAGE_LOGD("apply space for dest buffer failed!");
        return false;
    }
    libyuv::NV12ToI420(srcBuffer, imageSize.width, srcBuffer + imageSize.width * imageSize.height,
        ((imageSize.width + NUM_1) / NUM_2) * NUM_2, yu12Buffer, imageSize.width, yu12Buffer +
        imageSize.width * imageSize.height, (imageSize.width + NUM_1) / NUM_2,
        yu12Buffer + imageSize.width * imageSize.height + ((imageSize.width + NUM_1) / NUM_2) *
        ((imageSize.height + NUM_1) / NUM_2), (imageSize.width + NUM_1) / NUM_2, imageSize.width, imageSize.height);
    libyuv::I420ToNV21(yu12Buffer, imageSize.width, yu12Buffer + imageSize.width * imageSize.height,
        (imageSize.width + NUM_1) / NUM_2, yu12Buffer + imageSize.width * imageSize.height +
        ((imageSize.width + NUM_1) / NUM_2) * ((imageSize.height + NUM_1) / NUM_2), (imageSize.width + NUM_1) / NUM_2,
        *destBuffer, imageSize.width, *destBuffer + imageSize.width * imageSize.height,
        ((imageSize.width + NUM_1) / NUM_2) * NUM_2, imageSize.width, imageSize.height);
#else
    NV12ToNV21Manual(srcBuffer, imageSize, destBuffer);
#endif
    return true;
}

bool BGRAToNV12(const uint8_t *srcBuffer, const Size &imageSize, uint8_t **destBuffer,
                size_t &destBufferSize, [[maybe_unused]]ColorSpace colorSpace)
{
    if (srcBuffer == nullptr || destBuffer == nullptr || imageSize.width < 0 || imageSize.height < 0) {
        return false;
    }
    size_t destPlaneSizeY = imageSize.width * imageSize.height;
    size_t srcPlaneSizeUV = ((imageSize.width + NUM_1) / NUM_2) * ((imageSize.height + NUM_1) / NUM_2);
    destBufferSize = static_cast<size_t>(destPlaneSizeY + srcPlaneSizeUV * NUM_2);
    if (destBufferSize <= NUM_0) {
        IMAGE_LOGD("Invalid destination buffer size calculation!");
        return false;
    }
    *destBuffer = new(std::nothrow) uint8_t[destBufferSize]();
    if (*destBuffer == nullptr) {
        std::cerr << "apply space for dest buffer failed!" << std::endl;
        return false;
    }
#ifdef LIBYUV
    uint8_t *nv12Y = *destBuffer;
    uint8_t *nv12UV = *destBuffer + destPlaneSizeY;
    libyuv::ARGBToNV12(srcBuffer, imageSize.width * NUM_4, nv12Y, imageSize.width, nv12UV,
        (imageSize.width + NUM_1) / NUM_2 * NUM_2, imageSize.width, imageSize.height);
#else
    BGRAToNV12Manual(srcBuffer, imageSize, destBuffer);
#endif
    return true;
}

bool RGB565ToNV12(const uint8_t *srcBuffer, const Size &imageSize, uint8_t **destBuffer,
                  size_t &destBufferSize, [[maybe_unused]]ColorSpace colorSpace)
{
    if (srcBuffer == nullptr || destBuffer == nullptr || imageSize.width < 0 || imageSize.height < 0) {
        IMAGE_LOGD("apply space for dest buffer failed!");
        return false;
    }
    destBufferSize = static_cast<size_t>(imageSize.width * imageSize.height +
        ((imageSize.width + NUM_1) / NUM_2) * ((imageSize.height + NUM_1) / NUM_2) * NUM_2);
    if (destBufferSize <= NUM_0) {
        IMAGE_LOGD("Invalid destination buffer size calculation!");
        return false;
    }
    *destBuffer = new(std::nothrow) uint8_t[destBufferSize]();
    if (*destBuffer == nullptr) {
        IMAGE_LOGD("apply space for dest buffer failed!");
        return false;
    }
#ifdef LIBYUV
    std::unique_ptr<uint8_t[]> yu12Buffer(new(std::nothrow) uint8_t[imageSize.width * imageSize.height +
        ((imageSize.width + NUM_1) / NUM_2) * ((imageSize.height + NUM_1) / NUM_2) * NUM_2]());
    if (yu12Buffer == nullptr) {
        IMAGE_LOGD("apply space for dest buffer failed!");
        return false;
    }
    libyuv::RGB565ToI420(srcBuffer, imageSize.width * NUM_2, yu12Buffer, imageSize.width,
        yu12Buffer + imageSize.width * imageSize.height, (imageSize.width + NUM_1)/ NUM_2, yu12Buffer +
        imageSize.width * imageSize.height + ((imageSize.width + NUM_1) / NUM_2) *
        ((imageSize.height + NUM_1) / NUM_2), (imageSize.width + NUM_1) / NUM_2, imageSize.width, imageSize.height);
    libyuv::I420ToNV12(yu12Buffer, imageSize.width, yu12Buffer + imageSize.width * imageSize.height,
        (imageSize.width + NUM_1) / NUM_2, yu12Buffer + imageSize.width * imageSize.height +
        ((imageSize.width + NUM_1) / NUM_2) * ((imageSize.height + NUM_1) / NUM_2),
        (imageSize.width + NUM_1) / NUM_2, *destBuffer, imageSize.width, *destBuffer + imageSize.width *
        imageSize.height, ((imageSize.width + NUM_1) / NUM_2) * NUM_2, imageSize.width, imageSize.height);
#else
    RGB565ToNV12Manual(srcBuffer, imageSize, destBuffer);
#endif
    return true;
}

bool RGB565ToNV21(const uint8_t *srcBuffer, const Size &imageSize, uint8_t **destBuffer,
                  size_t &destBufferSize, [[maybe_unused]]ColorSpace colorSpace)
{
    if (srcBuffer == nullptr || destBuffer == nullptr || imageSize.width < 0 || imageSize.height < 0) {
        IMAGE_LOGD("apply space for dest buffer failed!");
        return false;
    }
    destBufferSize = static_cast<size_t>(imageSize.width * imageSize.height + ((imageSize.width + NUM_1) / NUM_2) *
        ((imageSize.height + NUM_1) / NUM_2) * NUM_2);
    if (destBufferSize <= NUM_0) {
        IMAGE_LOGD("Invalid destination buffer size calculation!");
        return false;
    }
    *destBuffer = new(std::nothrow) uint8_t[destBufferSize]();
    if (*destBuffer == nullptr) {
        IMAGE_LOGD("apply space for dest buffer failed!");
        return false;
    }
#ifdef LIBYUV
    std::unique_ptr<uint8_t[]> yu12Buffer(new(std::nothrow) uint8_t[imageSize.width * imageSize.height +
        ((imageSize.width + NUM_1) / NUM_2) * ((imageSize.height + NUM_1) / NUM_2) * NUM_2]());
    if (yu12Buffer == nullptr) {
        IMAGE_LOGD("apply space for dest buffer failed!");
        return false;
    }
    libyuv::RGB565ToI420(srcBuffer, imageSize.width * NUM_2, yu12Buffer, imageSize.width,
        yu12Buffer + imageSize.width * imageSize.height, (imageSize.width + NUM_1) / NUM_2, yu12Buffer +
        imageSize.width * imageSize.height + ((imageSize.width + NUM_1) / NUM_2) *
        ((imageSize.height + NUM_1) / NUM_2), (imageSize.width + NUM_1) / NUM_2, imageSize.width, imageSize.height);
    libyuv::I420ToNV21(yu12Buffer, imageSize.width, yu12Buffer + imageSize.width * imageSize.height,
        (imageSize.width + NUM_1) / NUM_2, yu12Buffer + imageSize.width * imageSize.height +
        ((imageSize.width + NUM_1) / NUM_2) * ((imageSize.height + NUM_1) / NUM_2),
        (imageSize.width + NUM_1) / NUM_2, *destBuffer, imageSize.width, *destBuffer + imageSize.width *
        imageSize.height, ((imageSize.width + NUM_1) / NUM_2) * NUM_2, imageSize.width, imageSize.height);
#else
    RGB565ToNV21Manual(srcBuffer, imageSize, destBuffer);
#endif
    return true;
}

bool NV12ToRGBAF16(const uint8_t *srcBuffer, const Size &imageSize, uint8_t **destBuffer,
                   size_t &destBufferSize, [[maybe_unused]]ColorSpace colorSpace)
{
    if (srcBuffer == nullptr || destBuffer == nullptr || imageSize.width < 0 || imageSize.height < 0) {
        return false;
    }
    uint32_t frameSize = imageSize.width * imageSize.height;
    destBufferSize = frameSize * sizeof(uint64_t);
    if (destBufferSize <= NUM_0) {
        IMAGE_LOGD("Invalid destination buffer size calculation!");
        return false;
    }
    *destBuffer = new(std::nothrow) uint8_t[destBufferSize]();
    if (*destBuffer == nullptr) {
        IMAGE_LOGD("apply space for dest buffer failed!");
        return false;
    }
    AVPixelFormat srcFormat = findPixelFormat(PixelFormat::NV12);
    AVPixelFormat dstFormat = findPixelFormat(PixelFormat::RGBA_F16);
    SwsContext *swsContext = sws_getContext(imageSize.width, imageSize.height, srcFormat, imageSize.width,
        imageSize.height, dstFormat, SWS_BILINEAR, nullptr, nullptr, nullptr);
    if (!swsContext) {
        sws_freeContext(swsContext);
        return false;
    }

    int widthEvent = (imageSize.width % NUM_2 == NUM_0) ? (imageSize.width) : (imageSize.width + NUM_1);
    const uint8_t *srcSlice[NUM_2] = {srcBuffer, srcBuffer + imageSize.width * imageSize.height} ;
    const int srcStride[NUM_2] = {static_cast<int>(imageSize.width),static_cast<int>(widthEvent)};
    uint8_t *dstSlice[NUM_1] = {*destBuffer};
    const int dstStride[NUM_1] = {static_cast<int>(imageSize.width * NUM_8)};
    int pixelFormatConvert = sws_scale(swsContext, srcSlice, srcStride, NUM_0, imageSize.height, dstSlice, dstStride);
    if (pixelFormatConvert == NUM_0) {
        delete[] destBuffer;
    }
    sws_freeContext(swsContext);
    return true;
}

bool BGRAToNV21(const uint8_t *srcBuffer, const Size &imageSize, uint8_t **destBuffer, size_t &destBufferSize,
                [[maybe_unused]]ColorSpace colorSpace)
{
    if (srcBuffer == nullptr || destBuffer == nullptr || imageSize.width < NUM_0 || imageSize.height < NUM_0) {
        return false;
    }
    const int32_t destPlaneSizeY = imageSize.width * imageSize.height;
    const int32_t destPlaneSizeVU = ((imageSize.width + NUM_1) / NUM_2) * ((imageSize.height + NUM_1) / NUM_2);
    destBufferSize = static_cast<size_t>(destPlaneSizeY + destPlaneSizeVU * NUM_2);
    if (destBufferSize <= NUM_0) {
        IMAGE_LOGD("Invalid destination buffer size calculation!");
        return false;
    }
    *destBuffer = new(std::nothrow) uint8_t[destBufferSize]();
    if (*destBuffer == nullptr) {
        IMAGE_LOGD("apply space for dest buffer failed!");
        return false;
    }
#ifdef LIBYUV
    uint8_t *nv21Y = *destBuffer;
    uint8_t *nv21VU = *destBuffer + destPlaneSizeY;
    libyuv::ARGBToNV21(srcBuffer, imageSize.width * NUM_4, nv21Y, imageSize.width, nv21VU,
        (imageSize.width + NUM_1) / NUM_2 * NUM_2, imageSize.width, imageSize.height);
#else
    BGRAToNV21Manual(srcBuffer, imageSize, destBuffer);
#endif
    return true;
}

bool NV12ToRGBA(const uint8_t *srcBuffer, const Size &imageSize, uint8_t **destBuffer,
                size_t &destBufferSize, [[maybe_unused]]ColorSpace colorSpace)
{
    if (srcBuffer == nullptr || destBuffer == nullptr || imageSize.width < NUM_0 || imageSize.height < NUM_0) {
        return false;
    }
    destBufferSize = static_cast<size_t>(imageSize.width * imageSize.height * NUM_4);
    if (destBufferSize <= NUM_0) {
        IMAGE_LOGD("Invalid destination buffer size calculation!");
        return false;
    }
    *destBuffer = new(std::nothrow) uint8_t[destBufferSize]();
    if (*destBuffer == nullptr) {
        IMAGE_LOGD("apply space for dest buffer failed!");
        return false;
    }

#ifdef LIBYUV
    const int32_t yStride = imageSize.width;
    const uint8_t *uvPlane = srcBuffer + yStride* imageSize.height;
    const int32_t uvStride = imageSize.width;
    int32_t i420Buffer_size = imageSize.width * imageSize.height +
        ((imageSize.width + NUM_1) / NUM_2 * (imageSize.height + NUM_1) / NUM_2) * NUM_2;
    std::unique_ptr<uint8_t[]> i420Buffer(new(std::nothrow) uint8_t[i420Buffer_size]());
    if (i420Buffer == nullptr) {
        IMAGE_LOGD("apply space for dest buffer failed!");
        return false;
    }
    const struct YuvConstants *YuvConstants = mapColorSPaceToYuvConstants(colorSpace);
    if (imageSize.width % NUM_2 ==NUM_0) {
        libyuv::NV12ToI420(srcBuffer, yStride, uvPlane, uvStride, i420Buffer, uvStride, i420Buffer + imageSize.width *
            imageSize.height, yStride / NUM_2, i420Buffer + imageSize.width * imageSize.height + imageSize.width *
            imageSize.height / NUM_4, (yStride + NUM_1) / NUM_2, imageSize.width, imageSize.height);
        libyuv::I420ToRGBAMatrix(i420Buffer, yStride, i420Buffer + imageSize.width * imageSize.height, yStride / NUM_2,
            i420Buffer + imageSize.width * imageSize.height + imageSize.width * imageSize.height / NUM_4,
            yStride / NUM_2, *destBuffer, imageSize.width * NUM_4, YuvConstants, imageSize.width, imageSize.height);
    } else {
        libyuv::NV12ToI420(srcBuffer, yStride, uvPlane, (uvStride + NUM_1) / NUM_2 * NUM_2, i420Buffer, yStride,
            i420Buffer + imageSize.width * imageSize.height, (yStride + NUM_1) / NUM_2, i420Buffer + imageSize.width *
            imageSize.height + ((imageSize.width+ NUM_1) / NUM_2)* ((imageSize.height+ NUM_1) / NUM_2),
            (yStride + NUM_1) / NUM_2, imageSize.width, imageSize.height);
        libyuv::I420ToRGBAMatrix(i420Buffer, yStride, i420Buffer + yStride * imageSize.height,
            (yStride + NUM_1) / NUM_2, i420Buffer + imageSize.height * yStride +
            (imageSize.height + NUM_1) / NUM_2 * (yStride + NUM_1) / NUM_2,
            (yStride + NUM_1) / NUM_2, *destBuffer, yStride * NUM_4, YuvConstants, imageSize.width, imageSize.height);
    }
#else
    NV12ToRGBAManual(srcBuffer, imageSize, destBuffer);
#endif
    return true;
}

bool NV12ToBGRA(const uint8_t *srcBuffer, const Size &imageSize, uint8_t **destBuffer,
                size_t &destBufferSize, [[maybe_unused]]ColorSpace colorSpace)
{
    if (srcBuffer == nullptr || destBuffer == nullptr || imageSize.width < NUM_0 || imageSize.height < NUM_0) {
        return false;
    }
    destBufferSize = static_cast<size_t>(imageSize.width * imageSize.height * NUM_4);
    if (destBufferSize <= NUM_0) {
        IMAGE_LOGD("Invalid destination buffer size calculation!");
        return false;
    }
    *destBuffer = new(std::nothrow) uint8_t[destBufferSize]();
    if (*destBuffer == nullptr) {
        IMAGE_LOGD("apply space for dest buffer failed!");
        return false;
    }
#ifdef LIBYUV
    const int32_t yStride = imageSize.width;
    const int32_t uvStride = imageSize.width;
    const uint8_t *uvPlane = srcBuffer + imageSize.width * imageSize.height;
    int32_t i420Buffer_size = ((imageSize.width + NUM_1) / NUM_2 * (imageSize.height + NUM_1) / NUM_2) * NUM_2;
    std::unique_ptr<uint8_t[]> i420Buffer(new(std::nothrow) uint8_t[i420Buffer_size]);
    if (i420Buffer == nullptr) {
        IMAGE_LOGD("apply space for dest buffer failed!");
        return false;
    } 
    if (imageSize.width % NUM_2 == NUM_0) {
        libyuv::NV12ToI420(srcBuffer, yStride, uvPlane, uvStride, i420Buffer, yStride, i420Buffer + imageSize.width *
            imageSize.height, yStride / NUM_2, i420Buffer + imageSize.width * imageSize.height + imageSize.width *
            imageSize.height / NUM_4, yStride/ NUM_2, imageSize.width, imageSize.height);
        libyuv::I420ToBGRA(i420Buffer, yStride, i420Buffer + imageSize.width * imageSize.height, yStride / NUM_2,
            i420Buffer + imageSize.width * imageSize.height + imageSize.width * imageSize.height / NUM_4,
            yStride / NUM_2, *destBuffer, imageSize.width * NUM_4, imageSize.width, imageSize.height);
    } else {
        libyuv::NV12ToI420(srcBuffer, yStride, uvPlane, (uvStride + NUM_1) / NUM_2 * NUM_2, i420Buffer, yStride,
            i420Buffer + imageSize.width * imageSize.height, (yStride + NUM_1) / NUM_2, i420Buffer + imageSize.width *
            imageSize.height + ((imageSize.width + NUM_1) / NUM_2) * ((imageSize.height + NUM_1) / NUM_2),
            (yStride + NUM_1) / NUM_2, imageSize.width, imageSize.height);
        destBufferSize = static_cast<size_t>(imageSize.width * imageSize.height * NUM_4);
        *destBuffer = new uint8_t[destBufferSize]();
        libyuv::I420ToBGRA(i420Buffer, yStride, i420Buffer + yStride * imageSize.height, (yStride + NUM_1) / NUM_2,
            i420Buffer + imageSize.height * yStride + (imageSize.height + NUM_1) / NUM_2 * (yStride + NUM_1) / NUM_2,
            (yStride + NUM_1) / NUM_2, *destBuffer, yStride * NUM_4, mageSize.width, imageSize.height);
    }
#else
    NV12ToBGRAManual(srcBuffer, imageSize, destBuffer);
#endif
    return true;
}

bool RGBAF16ToNV12(const uint8_t *srcBuffer, const Size &imageSize, uint8_t **destBuffer,
                   size_t &destBufferSize, [[maybe_unused]]ColorSpace colorSpace)
{
    if (srcBuffer == nullptr || destBuffer == nullptr || imageSize.width < NUM_0 || imageSize.height < NUM_0) {
        return false;
    }
    uint32_t frameSize = imageSize.width * imageSize.height;
    destBufferSize = frameSize + (((imageSize.width + NUM_1) / NUM_2 * (imageSize.height + NUM_1) / NUM_2) * NUM_2);
    if (destBufferSize <= NUM_0) {
        IMAGE_LOGD("Invalid destination buffer size calculation!");
        return false;
    }
    *destBuffer = new(std::nothrow) uint8_t[destBufferSize]();
    if (*destBuffer == nullptr) {
        IMAGE_LOGD("apply space for dest buffer failed!");
        return false;
    }

    AVPixelFormat srcFormat = findPixelFormat(PixelFormat::RGBA_F16);
    AVPixelFormat dstFormat = findPixelFormat(PixelFormat::NV12);
    SwsContext *swsContext = sws_getContext(imageSize.width, imageSize.height, srcFormat, imageSize.width,
        imageSize.height,dstFormat,SWS_BILINEAR,nullptr,nullptr,nullptr);
    if (!swsContext) {
        sws_freeContext(swsContext);
        return false;
    }

    int widthEvent = (imageSize.width % NUM_2 == NUM_0) ? (imageSize.width) : (imageSize.width + NUM_1);
    const uint8_t *srcSlice[NUM_1] = {srcBuffer};
    const int srcStride[NUM_1] = {static_cast<int>(imageSize.width * NUM_8)};
    uint8_t *dstSlice[NUM_2] = {*destBuffer, *destBuffer + imageSize.width * imageSize.height};
    const int dstStride[NUM_2] = {static_cast<int>(imageSize.width),static_cast<int>(widthEvent)};
    int pixelFormatConvert = sws_scale(swsContext,srcSlice,srcStride,0,imageSize.height,dstSlice,dstStride);
    if (pixelFormatConvert == NUM_0) {
        delete[] destBuffer;
    }
    sws_freeContext(swsContext);
    return true;
}

bool RGBAToNV12(const uint8_t *srcBuffer, const Size &imageSize, uint8_t **destBuffer,
				size_t &destBufferSize, [[maybe_unused]]ColorSpace colorSpace)
{
    if (srcBuffer == nullptr || destBuffer == nullptr || imageSize.width < NUM_0 || imageSize.height < NUM_0) {
        return false;
    }
    size_t destPlaneSizeY = imageSize.width * imageSize.height;
    size_t destPlaneSizeUV = ((imageSize.width + NUM_1) / NUM_2) * ((imageSize.height + NUM_1) / NUM_2);
    destBufferSize = destPlaneSizeY + destPlaneSizeUV * NUM_2;
    if (destBufferSize <= NUM_0) {
        IMAGE_LOGD("Invalid destination buffer size calculation!");
        return false;
    }
    *destBuffer = new(std::nothrow) uint8_t[destBufferSize]();
    if (*destBuffer == nullptr) {
        IMAGE_LOGD("apply space for dest buffer failed!");
        return false;
    }
#ifdef LIBYUV
    const uint32_t i420BufferSize = static_cast<size_t>(imageSize.width * imageSize.height + ((imageSize.width + NUM_1)
        / NUM_2) * ((imageSize.height + NUM_1) / NUM_2) * NUM_2);
    std::unique_ptr<uint8_t[]> i420Buffer(new(std::nothrow) uint8_t[i420BufferSize]());
    if (i420Buffer == nullptr) {
        IMAGE_LOGD("apply space for dest buffer failed!");
        return false;
    }
    uint8_t *i420Y = i420Buffer;
    uint8_t *i420U = i420Buffer + imageSize.width * imageSize.height;
    uint8_t *i420V = i420Buffer + imageSize.width * imageSize.height +
        ((imageSize.width + NUM_1) / NUM_2) * ((imageSize.height + NUM_1) / NUM_2);
    libyuv::ARGBToI420(srcBuffer, NUM_4 * imageSize.width, i420Y, imageSize.width, i420U,
    	(imageSize.width + NUM_1) / NUM_2, i420V, (imageSize.width + NUM_1) / NUM_2,
        imageSize.width, imageSize.height);
    uint8_t *nv12Y = *destBuffer;
    uint8_t *nv12UV = *destBuffer + imageSize.width * imageSize.height;
    libyuv::I420ToNV12(i420Y, imageSize.width, i420V, (imageSize.width + NUM_1) / NUM_2, i420U,
	    (imageSize.width + NUM_1) / NUM_2, nv12Y, imageSize.width, nv12UV,
        (imageSize.width + NUM_1) / NUM_2 * NUM_2, imageSize.width, imageSize.height);
#else
    RGBAToNV12Manual(srcBuffer, imageSize, destBuffer);
#endif
    return true;
}

bool RGBAToNV21(const uint8_t *srcBuffer, const Size &imageSize, uint8_t **destBuffer,
				size_t &destBufferSize, [[maybe_unused]]ColorSpace colorSpace)
{
    if (srcBuffer == nullptr || destBuffer == nullptr || imageSize.width < NUM_0 || imageSize.height < NUM_0) {
        return false;
    }
    size_t destPlaneSizeY = imageSize.width * imageSize.height;
    size_t destPlaneSizeVU = ((imageSize.width + NUM_1) / NUM_2) * ((imageSize.height + NUM_1) / NUM_2);
    destBufferSize = static_cast<size_t>(destPlaneSizeY + destPlaneSizeVU * NUM_2);
    if (destBufferSize <= NUM_0) {
        IMAGE_LOGD("Invalid destination buffer size calculation!");
        return false;
    }
    *destBuffer = new(std::nothrow) uint8_t[destBufferSize]();
    if (*destBuffer == nullptr) {
        IMAGE_LOGD("apply space for dest buffer failed!");
        return false;
    }

#ifdef LIBYUV
    const uint32_t i420BufferSize = static_cast<size_t>(imageSize.width * imageSize.height + 
        ((imageSize.width + NUM_1) / NUM_2) * ((imageSize.height + NUM_1) / NUM_2) * NUM_2);
    std::unique_ptr<uint8_t[]> i420Buffer(new(std::nothrow) uint8_t[i420BufferSize]());
    if (i420Buffer == nullptr) {
        IMAGE_LOGD("apply space for dest buffer failed!");
        return false;
    }
    uint8_t *i420Y = i420Buffer;
    uint8_t *i420U = i420Buffer + imageSize.width * imageSize.height;
    uint8_t *i420V = i420Buffer + imageSize.width * imageSize.height +
        ((imageSize.width + NUM_1) / NUM_2) * ((imageSize.height + NUM_1) / NUM_2);
    libyuv::ARGBToI420(srcBuffer, NUM_4 * imageSize.width, i420Y, imageSize.width, i420U,
        (imageSize.width + NUM_1) / NUM_2, i420V, (imageSize.width + NUM_1) / NUM_2,
        imageSize.width, imageSize.height);
    uint8_t *nv21Y = *destBuffer;
    uint8_t *nv21VU = *destBuffer + imageSize.width * imageSize.height;
    libyuv::I420ToNV21(i420Y, imageSize.width, i420V, (imageSize.width + NUM_1) / NUM_2, i420U,
        (imageSize.width + NUM_1) / NUM_2, nv21Y, imageSize.width, nv21VU,
        (imageSize.width + NUM_1) / NUM_2 * NUM_2, imageSize.width, imageSize.height);
#else
    RGBAToNV21Manual(srcBuffer, imageSize, destBuffer);
#endif
    return true;
}

bool RGBToNV21(const uint8_t *srcBuffer, const Size &imageSize, uint8_t **destBuffer,
               size_t &destBufferSize, [[maybe_unused]]ColorSpace colorSpace)
{
    if (srcBuffer == nullptr || destBuffer == nullptr ||imageSize.width < NUM_0 || imageSize.height < NUM_0) {
        return false;
    }
    destBufferSize = static_cast<size_t>
        (imageSize.width * imageSize.height + ((imageSize.width + NUM_1) / NUM_2 *
        ((imageSize.height + NUM_1) / NUM_2) * NUM_2));
    if (destBufferSize <= NUM_0) {
        IMAGE_LOGD("Invalid destination buffer size calculation!");
        return false;
    }
    *destBuffer = new(std::nothrow) uint8_t[destBufferSize]();
    if (*destBuffer == nullptr) {
        IMAGE_LOGD("apply space for dest buffer failed!");
        return false;
    }
#ifdef LIBYUV
    const uint32_t yu12BufferSize = static_cast<size_t>(imageSize.width * imageSize.height +
        ((imageSize.width + NUM_1) / NUM_2) * ((imageSize.height + NUM_1) / NUM_2) * NUM_2);
    std::unique_ptr<uint8_t[]> yu12Buffer(new(std::nothrow) uint8_t[yu12BufferSize]());
    if (yu12Buffer == nullptr) {
        IMAGE_LOGD("apply space for I420 buffer failed!");
        return false;
    }
    uint8_t *I420Y = yu12Buffer;
    uint8_t *I420U = yu12Buffer + imageSize.height * imageSize.width;
    uint8_t *I420V = yu12Buffer + imageSize.height * imageSize.width +
        ((imageSize.width + NUM_1) / NUM_2) * ((imageSize.height + NUM_1) / NUM_2);
    libyuv::RGB24ToI420(srcBuffer, imageSize.width * NUM_3, I420Y, imageSize.width, I420U,
        (imageSize.width + NUM_1) / NUM_2, I420V, (imageSize.width + NUM_1) / NUM_2,
        imageSize.width, imageSize.height);
    libyuv::I420ToNV21(I420Y, imageSize.width, I420U, (imageSize.width + NUM_1) / NUM_2,
        I420V, (imageSize.width + NUM_1) / NUM_2, *destBuffer, imageSize.width,
        *destBuffer + imageSize.width * imageSize.height + NUM_1,
        (imageSize.width + NUM_1) / NUM_2 * NUM_2, imageSize.width, imageSize.height);
#else
    RGBToNV21Manual(srcBuffer, imageSize, destBuffer);
#endif
    return true;
}

bool NV12ToRGB(const uint8_t *srcBuffer, const Size &imageSize, uint8_t **destBuffer,
               size_t &destBufferSize, [[maybe_unused]]ColorSpace colorSpace)
{
    if (srcBuffer == nullptr || destBuffer == nullptr || imageSize.width < NUM_0 || imageSize.height < NUM_0) {
        return false;
    }
    destBufferSize = static_cast<size_t>(imageSize.width * imageSize.height * NUM_3);
    if (destBufferSize <= NUM_0) {
        IMAGE_LOGD("Invalid destination buffer size calculation!");
        return false;
    }
    (*destBuffer) = new(std::nothrow)uint8_t[destBufferSize]();
    if (*destBuffer == nullptr) {
        IMAGE_LOGD("apply space for dest buffer failed!");
        return false;
    }
#ifdef LIBYUV
    libyuv::NV12ToRGB24Matrix(srcBuffer, imageSize.width, srcUV + NUM_1, (imageSize.width + NUM_1) / NUM_2 * NUM_2,
        *destBuffer, imageSize.width * NUM_3, &kYuvV2020Constants, imageSize.width, imageSize.height);
#else
    AVPixelFormat srcFormat = findPixelFormat(PixelFormat::NV12);
    AVPixelFormat dstFormat = findPixelFormat(PixelFormat::RGB_888);
    SwsContext *swsContext = sws_getContext(imageSize.width, imageSize.height, srcFormat, imageSize.width,
        imageSize.height, dstFormat, SWS_BILINEAR, nullptr, nullptr, nullptr);
    if (!swsContext) {
        sws_freeContext(swsContext);
        return false;
    }

    int widthEvent = (imageSize.width % NUM_2 == NUM_0) ? (imageSize.width) : (imageSize.width + NUM_1);
    const uint8_t *srcSlice[NUM_2] = {srcBuffer, srcBuffer + imageSize.width * imageSize.height} ;
    const int srcStride[NUM_2] = {static_cast<int>(imageSize.width),static_cast<int>(widthEvent)};
    uint8_t *dstSlice[NUM_1] = {*destBuffer};
    const int dstStride[NUM_1] = {static_cast<int>(imageSize.width * NUM_3)};
    int pixelFormatConvert = sws_scale(swsContext, srcSlice, srcStride, NUM_0, imageSize.height, dstSlice, dstStride);
    if (pixelFormatConvert == NUM_0) {
        delete[] destBuffer;
    }
    sws_freeContext(swsContext);
#endif
    return true;
}

bool NV21ToRGB(const uint8_t *srcBuffer, const Size &imageSize, uint8_t **destBuffer,
               size_t &destBufferSize, [[maybe_unused]]ColorSpace colorSpace)
{
    if (srcBuffer == nullptr || destBuffer == nullptr || imageSize.width < NUM_0 || imageSize.height < NUM_0) {
        return false;
    }

    destBufferSize = static_cast<size_t>(imageSize.width * imageSize.height * NUM_3);
    if (destBufferSize <= NUM_0) {
        IMAGE_LOGD("Invalid destination buffer size calculation!");
        return false;
    }
    (*destBuffer) = new(std::nothrow) uint8_t[destBufferSize]();
    if (*destBuffer == nullptr) {
        IMAGE_LOGD("apply space for dest buffer failed!");
        return false;
    }
#ifdef LIBYUV
    const struct YuvConstants* yuvConstants = mapColorSPaceToYuvConstants(colorSpace);
    libyuv::NV21ToRGB24Matrix(
        srcBuffer, imageSize.width,
        srcBuffer + imageSize.width * imageSize.height + NUM_1, (imageSize.width + NUM_1) / NUM_2 * NUM_2,
        *destBuffer, imageSize.width * NUM_3, yuvConstants, imageSize.width, imageSize.height);
#else
    AVPixelFormat srcFormat = findPixelFormat(PixelFormat::NV21);
    AVPixelFormat dstFormat = findPixelFormat(PixelFormat::RGB_888);
    SwsContext *swsContext = sws_getContext(imageSize.width, imageSize.height, srcFormat, imageSize.width,
        imageSize.height, dstFormat, SWS_BILINEAR, nullptr, nullptr, nullptr);
    if (!swsContext) {
        sws_freeContext(swsContext);
        return false;
    }

    int widthEvent = (imageSize.width % NUM_2 == NUM_0) ? (imageSize.width) : (imageSize.width + NUM_1);
    const uint8_t *srcSlice[NUM_2] = {srcBuffer, srcBuffer + imageSize.width * imageSize.height} ;
    const int srcStride[NUM_2] = {static_cast<int>(imageSize.width),static_cast<int>(widthEvent)};
    uint8_t *dstSlice[NUM_1] = {*destBuffer};
    const int dstStride[NUM_1] = {static_cast<int>(imageSize.width * NUM_3)};
    int pixelFormatConvert = sws_scale(swsContext, srcSlice, srcStride, NUM_0, imageSize.height, dstSlice, dstStride);
    if (pixelFormatConvert == NUM_0) {
        delete[] destBuffer;
    }
    sws_freeContext(swsContext);
#endif
    return true;
}

bool NV21ToRGBA(const uint8_t *srcBuffer, const Size &imageSize, uint8_t **destBuffer,
                size_t &destBufferSize, [[maybe_unused]]ColorSpace colorSpace)
{
    if (srcBuffer == nullptr || destBuffer == nullptr || imageSize.width < NUM_0 || imageSize.height < NUM_0) {
        return false;
    }

    destBufferSize = static_cast<size_t>(imageSize.width * imageSize.height * NUM_4);
    if (destBufferSize <= NUM_0) {
        IMAGE_LOGD("Invalid destination buffer size calculation!");
        return false;
    }
    (*destBuffer) = new(std::nothrow) uint8_t[destBufferSize]();
    if (*destBuffer == nullptr) {
        IMAGE_LOGD("apply space for dest buffer failed!");
        return false;
    }
#ifdef LIBYUV
    const struct YuvConstants* yuvConstants = mapColorSPaceToYuvConstants(colorSpace);
    std::unique_ptr<uint8_t[]> yu12Buffer(new(std::nothrow) uint8_t[imageSize.width * imageSize.height +
        ((imageSize.width + NUM_1) / NUM_2 * (imageSize.height + NUM_1) / NUM_2 * NUM_2)]());
    if (*yu12Buffer == nullptr) {
        IMAGE_LOGD("apply space for I420 buffer failed!");
        return false;
    }
    libyuv::NV21ToI420(
        srcBuffer, imageSize.width, srcBuffer + imageSize.width * imageSize.height,
        (imageSize.width + NUM_1) / NUM_2 * NUM_2, yu12Buffer, imageSize.width, yu12Buffer +
        imageSize.width * imageSize.height, (imageSize.width + NUM_1) / NUM_2,
        yu12Buffer + imageSize.width * imageSize.height + ((imageSize.width + NUM_1) / NUM_2 *
        (imageSize.height + NUM_1) / NUM_2), (imageSize.width+NUM_1) / NUM_2, imageSize.width, imageSize.height);
    libyuv::I420ToRGBAMatrix(
        yu12Buffer, imageSize.width, yu12Buffer + imageSize.width * imageSize.height, (imageSize.width+NUM_1) / NUM_2,
        yu12Buffer + imageSize.width * imageSize.height + (imageSize.width + NUM_1) / NUM_2 *
        (imageSize.height + NUM_1) / NUM_2, (imageSize.width+NUM_1) / NUM_2, *destBuffer, imageSize.width * NUM_4,
        yuvConstants, imageSize.width, imageSize.height);
#else
    NV21ToRGBAManual(srcBuffer, imageSize, destBuffer);
#endif
    return true;
}

bool NV21ToBGRA(const uint8_t *srcBuffer, const Size &imageSize, uint8_t **destBuffer,
                size_t &destBufferSize, [[maybe_unused]]ColorSpace colorSpace)
{
    if (srcBuffer == nullptr || destBuffer == nullptr || imageSize.width < NUM_0 || imageSize.height < NUM_0) {
        return false;
    }
    destBufferSize = static_cast<size_t>(imageSize.width * imageSize.height * NUM_4);
    if (destBufferSize <= NUM_0) {
        IMAGE_LOGD("Invalid destination buffer size calculation!");
        return false;
    }
    (*destBuffer) = new(std::nothrow) uint8_t[destBufferSize]();
    if (*destBuffer == nullptr) {
        IMAGE_LOGD("apply space for dest buffer failed!");
        return false;
    }
#ifdef LIBYUV
    std::unique_ptr<uint8_t[]> yu12Buffer(new(std::nothrow) uint8_t[imageSize.width * imageSize.height +
        ((imageSize.width + NUM_1) / NUM_2 * (imageSize.height + NUM_1) / NUM_2 * NUM_2)]());
    if (*yu12Buffer == nullptr) {
        IMAGE_LOGD("apply space for I420 buffer failed!");
        return false;
    }
    libyuv::NV21ToI420(
        srcBuffer, imageSize.width, srcBuffer + imageSize.width * imageSize.height,
        (imageSize.width + NUM_1) / NUM_2 * NUM_2, yu12Buffer, imageSize.width,
        yu12Buffer + imageSize.width * imageSize.height, (imageSize.width + NUM_1) / NUM_2,
        yu12Buffer + imageSize.width * imageSize.height + (imageSize.width + NUM_1) / NUM_2 *
        (imageSize.height + NUM_1) / NUM_2,
        (imageSize.width + NUM_1) / NUM_2, imageSize.width, imageSize.height);

    libyuv::I420ToBGRA(
        yu12Buffer, imageSize.width, yu12Buffer + imageSize.width * imageSize.height,
        (imageSize.width + NUM_1) / NUM_2,
        yu12Buffer + imageSize.width * imageSize.height + (imageSize.width + NUM_1) / NUM_2 *
        (imageSize.height + NUM_1) / NUM_2, (imageSize.width + NUM_1) / NUM_2,
        *destBuffer, imageSize.width * NUM_4, imageSize.width, imageSize.height);
#else
    NV21ToBGRAManual(srcBuffer, imageSize, destBuffer);
#endif
    return true;
}

bool NV21ToRGB565(const uint8_t *srcBuffer, const Size &imageSize, uint8_t **destBuffer,
                  size_t &destBufferSize, [[maybe_unused]]ColorSpace colorSpace)
{
    if (srcBuffer == nullptr || destBuffer == nullptr || imageSize.width < NUM_0 || imageSize.height < NUM_0) {
        return false;
    }
    destBufferSize = static_cast<size_t>(imageSize.width * imageSize.height * NUM_2);
    if (destBufferSize <= NUM_0) {
        IMAGE_LOGD("Invalid destination buffer size calculation!");
        return false;
    }
    (*destBuffer) = new(std::nothrow) uint8_t[destBufferSize]();
    if (*destBuffer == nullptr) {
        IMAGE_LOGD("apply space for dest buffer failed!");
        return false;
    }
#ifdef LIBYUV
    const struct YuvConstants* yuvConstants = mapColorSPaceToYuvConstants(colorSpace);
    std::unique_ptr<uint8_t[]> yu12Buffer(new(std::nothrow) uint8_t[imageSize.width * imageSize.height +
        ((imageSize.width + NUM_1) / NUM_2 * (imageSize.height + NUM_1) / NUM_2 * NUM_2)]());
    if (*yu12Buffer == nullptr) {
        IMAGE_LOGD("apply space for I420 buffer failed!");
        return false;
    }
    libyuv::NV21ToI420(
        srcBuffer, imageSize.width, srcBuffer + imageSize.width * imageSize.height + NUM_1,
        (imageSize.width + NUM_1) / NUM_2 * NUM_2, yu12Buffer, imageSize.width,
        yu12Buffer + imageSize.width * imageSize.height, (imageSize.width + NUM_1) / NUM_2,
        yu12Buffer + imageSize.width * imageSize.height + (imageSize.width + NUM_1) / NUM_2 *
        (imageSize.height + NUM_1) / NUM_2 * NUM_2, (imageSize.width + NUM_1) / NUM_2,
        imageSize.width, imageSize.height);

    libyuv::I420ToRGB565Matrix(
        yu12Buffer, imageSize.width, yu12Buffer + imageSize.width * imageSize.height,
        (imageSize.width + NUM_1) / NUM_2,
        yu12Buffer + imageSize.width * imageSize.height + (imageSize.width + NUM_1) / NUM_2 *
        (imageSize.height + NUM_1) / NUM_2 * NUM_2, (imageSize.width + NUM_1) / NUM_2,
        *destBuffer, imageSize.width * NUM_2, yuvConstants, imageSize.width, imageSize.height);
#else
    NV21ToRGB565Manual(srcBuffer, imageSize, destBuffer);
#endif
    return true;
}

bool RGBToNV12(const uint8_t *srcBuffer, const Size &imageSize, uint8_t **destBuffer,
               size_t &destBufferSize, [[maybe_unused]]ColorSpace colorSpace)
{
    if (srcBuffer == nullptr || destBuffer == nullptr || imageSize.width < NUM_0 || imageSize.height < NUM_0) {
        return false;
    }
    destBufferSize = static_cast<size_t>(imageSize.width * imageSize.height + (imageSize.width + NUM_1) / NUM_2 *
        (imageSize.height + NUM_1) / NUM_2 * NUM_2);
    if (destBufferSize <= NUM_0) {
        IMAGE_LOGD("Invalid destination buffer size calculation!");
        return false;
    }
    (*destBuffer) = new(std::nothrow) uint8_t[destBufferSize]();
    if (*destBuffer == nullptr) {
        IMAGE_LOGD("apply space for dest buffer failed!");
        return false;
    }
#ifdef LIBYUV
    uint8_t *yu12Buffer (new(std::nothrow) uint8_t[imageSize.width * imageSize.height +
        (imageSize.width + NUM_1) / NUM_2 * (imageSize.height + NUM_1) / NUM_2 * NUM_2]());
    if (*yu12Buffer == nullptr) {
        IMAGE_LOGD("apply space for I420 buffer failed!");
        return false;
    }
    libyuv::RGB24ToI420(
        srcBuffer, imageSize.width * NUM_3, yu12Buffer, imageSize.width,
        yu12Buffer + imageSize.width * imageSize.height, (imageSize.width + NUM_1) / NUM_2,
        yu12Buffer + imageSize.width * imageSize.height + (imageSize.width + NUM_1) / NUM_2 *
        (imageSize.height + NUM_1) / NUM_2, (imageSize.width + NUM_1) / NUM_2, imageSize.width, imageSize.height);
        
    libyuv::I420ToNV12(
        yu12Buffer, imageSize.width, yu12Buffer + imageSize.width * imageSize.height,
        (imageSize.width + NUM_1) / NUM_2,
        yu12Buffer + imageSize.width * imageSize.height + (imageSize.width + NUM_1) / NUM_2 *
        (imageSize.height + NUM_1) / NUM_2, (imageSize.width + NUM_1) / NUM_2,
        *destBuffer, imageSize.width, *destBuffer + imageSize.width * imageSize.height + NUM_1,
        (imageSize.width + NUM_1) / NUM_2 * NUM_2, imageSize.width, imageSize.height);
#else
    RGBToNV12Manual(srcBuffer, imageSize, destBuffer);
#endif
    return true;
}
} // namespace Media
} // namespace OHOS