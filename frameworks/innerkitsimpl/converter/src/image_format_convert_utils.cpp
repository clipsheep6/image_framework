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
#include <cstring>
#include <iostream>
#include <map>
#include "log_tags.h"
#include "image_log.h"
#include "hilog/log.h"
#include "securec.h"

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

    constexpr uint32_t SRCSLICEY = 0;
    constexpr uint32_t EVEN_ODD_DIVISOR = 2;
    constexpr uint32_t TWO_SLICES = 2;
    constexpr uint32_t BYTES_PER_PIXEL_RGB565 = 2;
    constexpr uint32_t BYTES_PER_PIXEL_RGB = 3;
    constexpr uint32_t BYTES_PER_PIXEL_RGBA = 4;
    constexpr uint32_t BYTES_PER_PIXEL_BGRA = 4;
    constexpr uint32_t STRIDES_PER_PLANE = 8;
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

const struct YuvConstants* MapColorSpaceToYuvConstants(ColorSpace colorSpace)
{
    auto it = colorSpaceMap.find(colorSpace);
    if (it != colorSpaceMap.end()) {
        return it->second;
    }
    return nullptr;
}
#endif

static std::map<PixelFormat, AVPixelFormat> PixelFormatMap = {
    {PixelFormat::UNKNOWN, AV_PIX_FMT_NONE},
    {PixelFormat::NV12, AV_PIX_FMT_NV12},
    {PixelFormat::NV21, AV_PIX_FMT_NV21},
    {PixelFormat::RGB_565, AV_PIX_FMT_RGB565BE},
    {PixelFormat::RGBA_8888, AV_PIX_FMT_RGBA},
    {PixelFormat::BGRA_8888, AV_PIX_FMT_BGRA},
    {PixelFormat::ARGB_8888, AV_PIX_FMT_ARGB},
    {PixelFormat::RGBA_F16, AV_PIX_FMT_RGBA64BE},
    {PixelFormat::RGB_888, AV_PIX_FMT_RGB24},
};

AVPixelFormat findPixelFormat(PixelFormat format)
{
    if (PixelFormatMap.find(format) != PixelFormatMap.end()) {
        return PixelFormatMap[format];
    } else {
        return AV_PIX_FMT_NONE;
    }
}

static bool NV12ToRGB565Manual(const uint8_t *srcBuffer, const YUVDataInfo &yDInfo, uint8_t **destBuffer)
{
    AVPixelFormat srcFormat = findPixelFormat(PixelFormat::NV12);
    AVPixelFormat dstFormat = findPixelFormat(PixelFormat::RGB_565);
    SwsContext *swsContext = sws_getContext(yDInfo.yWidth, yDInfo.yHeight, srcFormat, yDInfo.yWidth,
        yDInfo.yHeight, dstFormat, SWS_BILINEAR, nullptr, nullptr, nullptr);
    if (swsContext == nullptr) {
        IMAGE_LOGD("Error to create SwsContext.");
        return false;
    }

    const uint8_t *srcSlice[] = {srcBuffer + yDInfo.yOffset, srcBuffer + yDInfo.uvOffset};
    const int srcStride[] = {static_cast<int>(yDInfo.yStride), static_cast<int>(yDInfo.uvStride)};
    uint8_t *dstSlice[] = {*destBuffer};
    const int dstStride[] = {static_cast<int>(yDInfo.yWidth * NUM_2)};
    int pixelFormatConvert = sws_scale(swsContext, srcSlice, srcStride, NUM_0, yDInfo.yHeight, dstSlice, dstStride);
    sws_freeContext(swsContext);
    if (pixelFormatConvert == 0) {
        return false;
    }
    return true;
}

static bool NV21ToNV12Manual(const uint8_t *srcBuffer, const YUVDataInfo &yDInfo, uint8_t **destBuffer)
{
    AVPixelFormat srcFormat = findPixelFormat(PixelFormat::NV21);
    AVPixelFormat dstFormat = findPixelFormat(PixelFormat::NV12);
    SwsContext *swsContext = sws_getContext(yDInfo.yWidth, yDInfo.yHeight, srcFormat, yDInfo.yWidth,
        yDInfo.yHeight, dstFormat, SWS_BILINEAR, nullptr, nullptr, nullptr);
    if (swsContext == nullptr) {
        IMAGE_LOGD("Error to create SwsContext.");
        return false;
    }

    int widthEvent = (yDInfo.yWidth % EVEN_ODD_DIVISOR == 0) ? (yDInfo.yWidth) : (yDInfo.yWidth + 1);
    const uint8_t *srcSlice[] = {srcBuffer + yDInfo.yOffset, srcBuffer + yDInfo.uvOffset};
    const int srcStride[] = {static_cast<int>(yDInfo.yStride), static_cast<int>(yDInfo.uvStride)};
    uint8_t *dstSlice[] = {*destBuffer, *destBuffer + yDInfo.yWidth * yDInfo.yHeight};
    const int dstStride[] = {static_cast<int>(yDInfo.yWidth), static_cast<int>(widthEvent)};

    int height = sws_scale(swsContext, srcSlice, srcStride, 0, yDInfo.yHeight, dstSlice, dstStride);
    sws_freeContext(swsContext);
    if (height == 0) {
        return false;
    }
    return true;
}

static bool NV12ToNV21Manual(const uint8_t *srcBuffer, const YUVDataInfo &yDInfo, uint8_t **destBuffer)
{
    AVPixelFormat srcFormat = findPixelFormat(PixelFormat::NV12);
    AVPixelFormat dstFormat = findPixelFormat(PixelFormat::NV21);
    SwsContext *swsContext = sws_getContext(yDInfo.yWidth, yDInfo.yHeight, srcFormat, yDInfo.yWidth,
        yDInfo.yHeight, dstFormat, SWS_BILINEAR, nullptr, nullptr, nullptr);
    if (swsContext == nullptr) {
        IMAGE_LOGD("Error to create SwsContext.");
        return false;
    }

    int widthEvent = (yDInfo.yWidth % EVEN_ODD_DIVISOR == 0) ? (yDInfo.yWidth) : (yDInfo.yWidth + 1);
    const uint8_t *srcSlice[] = {srcBuffer + yDInfo.yOffset, srcBuffer + yDInfo.uvOffset};
    const int srcStride[] = {static_cast<int>(yDInfo.yStride), static_cast<int>(yDInfo.uvStride)};
    uint8_t *dstSlice[] = {*destBuffer, *destBuffer + yDInfo.yWidth * yDInfo.yHeight};
    const int dstStride[] = {static_cast<int>(yDInfo.yWidth), static_cast<int>(widthEvent)};

    int height = sws_scale(swsContext, srcSlice, srcStride, 0, yDInfo.yHeight, dstSlice, dstStride);
    sws_freeContext(swsContext);
    if (height == 0) {
        return false;
    }
    return true;
}

static void RGB565ToNV12Manual(const uint8_t *srcBuffer, const Size &imageSize, uint8_t **destBuffer)
{
    AVPixelFormat srcFormat = findPixelFormat(PixelFormat::RGB_565);
    AVPixelFormat dstFormat = findPixelFormat(PixelFormat::NV12);
    SwsContext *swsContext = sws_getContext(imageSize.width, imageSize.height, srcFormat, imageSize.width,
        imageSize.height, dstFormat,SWS_BILINEAR, nullptr, nullptr, nullptr);
    if (!swsContext) {
        IMAGE_LOGD("Error to create SwsContext.");
        sws_freeContext(swsContext);
    }

    int widthEvent = (imageSize.width % NUM_2 == NUM_0) ? (imageSize.width) : (imageSize.width + NUM_1);
    const uint8_t *srcSlice[NUM_1] = {srcBuffer};
    const int srcStride[NUM_1] = {static_cast<int>(imageSize.width * NUM_2)};
    uint8_t *dstSlice[NUM_2] = {*destBuffer, *destBuffer + imageSize.width * imageSize.height};
    const int dstStride[NUM_2] = {static_cast<int>(imageSize.width), static_cast<int>(widthEvent)};
    int pixelFormatConvert = sws_scale(swsContext, srcSlice, srcStride, 0, imageSize.height, dstSlice, dstStride);
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
    const int dstStride[NUM_2] = {static_cast<int>(imageSize.width), static_cast<int>(widthEvent)};
    int pixelFormatConvert = sws_scale(swsContext, srcSlice, srcStride, NUM_0, imageSize.height, dstSlice, dstStride);
    if (pixelFormatConvert == NUM_0) {
        delete[] destBuffer;
    }
    sws_freeContext(swsContext);
}

static bool NV12ToRGBAF16Manual(const uint8_t *srcBuffer, const YUVDataInfo &yDInfo, uint8_t **destBuffer)
{
    AVPixelFormat srcFormat = findPixelFormat(PixelFormat::NV12);
    AVPixelFormat dstFormat = findPixelFormat(PixelFormat::RGBA_F16);
    struct SwsContext *ctx = sws_getContext(yDInfo.yWidth, yDInfo.yHeight, srcFormat,
                                            yDInfo.yWidth, yDInfo.yHeight, dstFormat,
                                            SWS_BILINEAR, nullptr, nullptr, nullptr);
    if (ctx == nullptr) {
        IMAGE_LOGE("sws_getContext: result is null!");
        return false;
    }

    const uint8_t *srcSlice[] = { srcBuffer + yDInfo.yOffset, srcBuffer + yDInfo.uvOffset };
    const int srcStride[] = { static_cast<int>(yDInfo.yStride), static_cast<int>(yDInfo.uvStride) };
    const int dstStride[] = { static_cast<int>(yDInfo.yWidth * STRIDES_PER_PLANE) };
    int sliceHeight = sws_scale(ctx, srcSlice, srcStride, SRCSLICEY, yDInfo.yHeight, destBuffer, dstStride);
    sws_freeContext(ctx);

    if (sliceHeight == 0) {
        IMAGE_LOGE("sws_scale: the height of the output slice is 0!");
        return false;
    }
    return true;
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
    const int dstStride[NUM_2] = {static_cast<int>(imageSize.width), static_cast<int>(widthEvent)};
    int pixelFormatConvert = sws_scale(swsContext, srcSlice, srcStride, NUM_0, imageSize.height, dstSlice, dstStride);
    if (pixelFormatConvert == NUM_0) {
        delete[] destBuffer;
    }
    sws_freeContext(swsContext);
}

#ifdef DCAMERA_MMAP_RESERVE
static bool NV12ToRGBANoManual(const uint8_t *srcBuffer, const YUVDataInfo &yDInfo, uint8_t **destBuffer,
                               ColorSpace colorSpace)
{
    uint32_t yPlaneSize = yDInfo.yWidth * yDInfo.yHeight;
    const uint8_t *yPlane = srcBuffer;
    const uint8_t *uvPlane = yPlane + yPlaneSize;
    uint32_t uvPlaneWidth = (yDInfo.yWidth + 1) / EVEN_ODD_DIVISOR;
    uint32_t uvPlaneHeight =  (yDInfo.yHeight + 1) / EVEN_ODD_DIVISOR;
    uint32_t uPlaneSize = uvPlaneWidth * uvPlaneHeight;
    uint32_t vPlaneSize = uPlaneSize;
    uint32_t yStride = yDInfo.yWidth;
    uint32_t uvStride = uvPlaneWidth + uvPlaneWidth;
    uint32_t i420BufferSize = yPlaneSize + uPlaneSize + vPlaneSize;

    std::unique_ptr<uint8_t[]> i420Buffer = std::make_unique<uint8_t[]>(i420BufferSize);
    if (i420Buffer == nullptr) {
        IMAGE_LOGE("Dynamically allocating memory for i420 buffer failed!");
        return false;
    }

    const struct YuvConstants *yuvConstants = MapColorSpaceToYuvConstants(colorSpace);
    const ImageConverter &converter = ConverterHandle::GetInstance()->GetHandle();
    const uint8_t* ySrc = i420Buffer;
    const uint8_t* uSrc = i420Buffer + yPlaneSize;
    const uint8_t* vSrc = i420Buffer + yPlaneSize + uPlaneSize;
    uint32_t uStride = uvPlaneWidth;
    uint32_t vStride = uvPlaneWidth;

    int32_t ret = converter.NV12ToI420(srcBuffer + yDInfo.yOffset, yDInfo.yStride, srcBuffer + yDInfo.uvOffset,
        yDInfo.uvStride, ySrc, yStride, uSrc, uStride, vSrc, vStride, yDInfo.yWidth, yDInfo.yHeight);
    if (ret != 0) {
        IMAGE_LOGE("NV12ToI420 failed, ret = %{public}d!", ret);
        return false;
    }

    uint32_t rgbaStride = yDInfo.yWidth * BYTES_PER_PIXEL_RGBA;
    ret = converter.I420ToRGBAMatrix(ySrc, yStride, uSrc, uStride, vSrc, vStride, *destBuffer,
        rgbaStride, yuvConstants, yDInfo.yWidth, yDInfo.yHeight);
    if (ret != 0) {
        IMAGE_LOGE("I420ToRGBAMatrix failed, ret = %{public}d!", ret);
        return false;
    }

    return true;
}
#endif

static bool NV12ToRGBAManual(const uint8_t *srcBuffer, const YUVDataInfo &yDInfo, uint8_t **destBuffer)
{
    AVPixelFormat srcFormat = findPixelFormat(PixelFormat::NV12);
    AVPixelFormat dstFormat = findPixelFormat(PixelFormat::RGBA_8888);
    struct SwsContext *ctx = sws_getContext(yDInfo.yWidth, yDInfo.yHeight, srcFormat,
                                            yDInfo.yWidth, yDInfo.yHeight, dstFormat,
                                            SWS_BILINEAR, nullptr, nullptr, nullptr);
    if (ctx == nullptr) {
        IMAGE_LOGE("sws_getContext: result is null!");
        return false;
    }

    const uint8_t *srcSlice[] = { srcBuffer + yDInfo.yOffset, srcBuffer + yDInfo.uvOffset };
    const int srcStride[] = { static_cast<int>(yDInfo.yStride), static_cast<int>(yDInfo.uvStride) };
    const int dstStride[] = { static_cast<int>(yDInfo.yWidth * BYTES_PER_PIXEL_RGBA) };
    int sliceHeight = sws_scale(ctx, srcSlice, srcStride, SRCSLICEY, yDInfo.yHeight, destBuffer, dstStride);
    sws_freeContext(ctx);

    if (sliceHeight == 0) {
        IMAGE_LOGE("sws_scale: the height of the output slice is 0!");
        return false;
    }
    return true;
}

#ifdef DCAMERA_MMAP_RESERVE
static bool NV12ToBGRANoManual(const uint8_t *srcBuffer, const YUVDataInfo &yDInfo, uint8_t **destBuffer,
                               ColorSpace colorSpace)
{
    uint32_t yPlaneSize = yDInfo.yWidth * yDInfo.yHeight;
    const uint8_t *yPlane = srcBuffer;
    const uint8_t *uvPlane = yPlane + yPlaneSize;
    uint32_t uvPlaneWidth = (yDInfo.yWidth + 1) / EVEN_ODD_DIVISOR;
    uint32_t uvPlaneHeight =  (yDInfo.yHeight + 1) / EVEN_ODD_DIVISOR;
    uint32_t uPlaneSize = uvPlaneWidth * uvPlaneHeight;
    uint32_t vPlaneSize = uPlaneSize;
    uint32_t yStride = yDInfo.yWidth;
    uint32_t uvStride = uvPlaneWidth + uvPlaneWidth;
    uint32_t i420BufferSize = yPlaneSize + uPlaneSize + vPlaneSize;

    std::unique_ptr<uint8_t[]> i420Buffer = std::make_unique<uint8_t[]>(i420BufferSize);
    if (i420Buffer == nullptr) {
        IMAGE_LOGE("Dynamically allocating memory for i420 buffer failed!");
        return false;
    }

    const struct YuvConstants *yuvConstants = MapColorSpaceToYuvConstants(colorSpace);
    const ImageConverter &converter = ConverterHandle::GetInstance()->GetHandle();
    const uint8_t* ySrc = i420Buffer;
    const uint8_t* uSrc = i420Buffer + yPlaneSize;
    const uint8_t* vSrc = i420Buffer + yPlaneSize + uPlaneSize;
    uint32_t uStride = uvPlaneWidth;
    uint32_t vStride = uvPlaneWidth;

    int32_t ret = converter.NV12ToI420(srcBuffer + yDInfo.yOffset, yDInfo.yStride, srcBuffer + yDInfo.uvOffset,
        yDInfo.uvStride, ySrc, yStride, uSrc, uStride, vSrc, vStride, yDInfo.yWidth, yDInfo.yHeight);
    if (ret != 0) {
        IMAGE_LOGE("NV12ToI420 failed, ret = %{public}d!", ret);
        return false;
    }

    uint32_t bgraStride = yDInfo.yWidth * BYTES_PER_PIXEL_BGRA;
    ret = converter.I420ToBGRA(ySrc, yStride, uSrc, uStride, vSrc, vStride, *destBuffer, bgraStride,
        yDInfo.yWidth, yDInfo.yHeight);
    if (ret != 0) {
        IMAGE_LOGE("I420ToBGRA failed, ret = %{public}d!", ret);
        return false;
    }

    return true;
}
#endif

static bool NV12ToBGRAManual(const uint8_t *srcBuffer, const YUVDataInfo &yDInfo, uint8_t **destBuffer)
{
    AVPixelFormat srcFormat = findPixelFormat(PixelFormat::NV12);
    AVPixelFormat dstFormat = findPixelFormat(PixelFormat::BGRA_8888);
    struct SwsContext *ctx = sws_getContext(yDInfo.yWidth, yDInfo.yHeight, srcFormat,
                                            yDInfo.yWidth, yDInfo.yHeight, dstFormat,
                                            SWS_BILINEAR, nullptr, nullptr, nullptr);
    if (ctx == nullptr) {
        IMAGE_LOGE("sws_getContext: result is null!");
        return false;
    }

    const uint8_t *srcSlice[] = { srcBuffer + yDInfo.yOffset, srcBuffer + yDInfo.uvOffset };
    const int srcStride[] = {static_cast<int>(yDInfo.yStride), static_cast<int>(yDInfo.uvStride)};
    const int dstStride[] = { static_cast<int>(yDInfo.yWidth * BYTES_PER_PIXEL_BGRA) };
    int heightOfDstSlice = sws_scale(ctx, srcSlice, srcStride, SRCSLICEY, yDInfo.yHeight, destBuffer, dstStride);
    sws_freeContext(ctx);

    if (heightOfDstSlice == 0) {
        IMAGE_LOGE("sws_scale: the height of the output slice is 0!");
        return false;
    }
    return true;
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
    const int dstStride[NUM_2] = {static_cast<int>(imageSize.width), static_cast<int>(widthEvent)};
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
    const int dstStride[NUM_2] = {static_cast<int>(imageSize.width), static_cast<int>(widthEvent)};
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
    const int dstStride[NUM_2] = {static_cast<int>(imageSize.width), static_cast<int>(widthEvent)};
    int pixelFormatConvert = sws_scale(swsContext, srcSlice, srcStride, NUM_0, imageSize.height, dstSlice, dstStride);
    if (pixelFormatConvert == NUM_0) {
        delete[] destBuffer;
    }
    sws_freeContext(swsContext);
}

#ifdef DCAMERA_MMAP_RESERVE
static bool NV21ToRGBAMatrix(const uint8_t *srcBuffer, const YUVDataInfo &yDInfo, uint8_t **destBuffer)
{
    const struct YuvConstants* yuvConstants = mapColorSPaceToYuvConstants(colorSpace);
    uint32_t yu12BufferSize = yDInfo.yWidth * yDInfo.yHeight + yDInfo.uvWidth * yDInfo.uvHeight * EVEN_ODD_DIVISOR;
    std::unique_ptr<uint8_t[]> yu12Buffer(new(std::nothrow) uint8_t[yu12BufferSize]());
    if (*yu12Buffer == nullptr) {
        IMAGE_LOGE("apply space for I420 buffer failed!");
        return false;
    }

    const ImageConverter &converter = ConverterHandle.GetInstance()->GetHandle();
    uint8_t* src_y = srcBuffer + yDInfo.yOffset;
    uint8_t* src_vu = srcBuffer + yDInfo.uvOffset;
    int src_stride_vu = (yDInfo.uvStride + 1) / EVEN_ODD_DIVISOR * EVEN_ODD_DIVISOR;
    uint8_t* dst_u = yu12Buffer + yDInfo.yWidth * yDInfo.yHeight;
    int dst_stride_u = (yDInfo.yWidth + 1) / EVEN_ODD_DIVISOR;
    uint8_t* dst_v = yu12Buffer + yDInfo.yWidth * yDInfo.yHeight + ((yDInfo.yWidth + 1) / EVEN_ODD_DIVISOR *
        (yDInfo.yHeight + 1) / EVEN_ODD_DIVISOR);
    int dst_stride_v = (yDInfo.yWidth + 1) / EVEN_ODD_DIVISOR;

    int ret = converter.NV21ToI420(src_y, yDInfo.yStride, src_vu, src_stride_vu, yu12Buffer, yDInfo.yWidth, dst_u,
                                   dst_stride_u, dst_v, dst_stride_v, yDInfo.yWidth, yDInfo.yHeight);
    if (!ret) {
        IMAGE_LOGE("NV12ToI420 failed, ret = %{public}d!", ret);
        return false;
    }

    ret = converter.I420ToRGBAMatrix(yu12Buffer, yDInfo.yWidth, dst_u, dst_stride_u, dst_v, dst_stride_v, *destBuffer,
                                     yDInfo.yWidth, * NUM_4, yuvConstants, yDInfo.yWidth, yDInfo.yHeight);
    if (!ret) {
        IMAGE_LOGE("NV12ToI420 failed, ret = %{public}d!", ret);
        return false;
    }
    return true;
    }
}
#endif

static bool NV21ToRGBAManual(const uint8_t *srcBuffer, const YUVDataInfo &yDInfo, uint8_t **destBuffer)
{
    AVPixelFormat srcFormat = findPixelFormat(PixelFormat::NV21);
    AVPixelFormat dstFormat = findPixelFormat(PixelFormat::RGBA_8888);
    SwsContext *swsContext = sws_getContext(yDInfo.yWidth, yDInfo.yHeight, srcFormat, yDInfo.yWidth,
        yDInfo.yHeight, dstFormat, SWS_BILINEAR, nullptr, nullptr, nullptr);
    if (swsContext == nullptr) {
        IMAGE_LOGE("Error to create SwsContext.");
        return false;
    }

    const uint8_t *srcSlice[] = {srcBuffer + yDInfo.yOffset, srcBuffer + yDInfo.uvOffset};
    const int srcStride[] = {static_cast<int>(yDInfo.yStride),static_cast<int>(yDInfo.uvStride)};
    uint8_t *dstSlice[] = {*destBuffer};
    const int dstStride[] = {static_cast<int>(yDInfo.yWidth * NUM_4)};
    int hight = sws_scale(swsContext, srcSlice, srcStride, SRCSLICEY, yDInfo.yHeight, dstSlice, dstStride);
    sws_freeContext(swsContext);
    if (hight == 0) {
        IMAGE_LOGE("Image pixel format conversion failed");
        return false;
    }
    return true;
}

#ifdef DCAMERA_MMAP_RESERVE
static bool NV21ToRGB565Matrix(const uint8_t *srcBuffer, const YUVDataInfo &yDInfo, uint8_t **destBuffer)
{
    const struct YuvConstants* yuvConstants = mapColorSPaceToYuvConstants(colorSpace);
    std::unique_ptr<uint8_t[]> yu12Buffer(new(std::nothrow) uint8_t[(yDInfo.yWidth * yDInfo.yHeight +
        yDInfo.uvWidth * yDInfo.uvHeight * EVEN_ODD_DIVISOR)]());
    if (*yu12Buffer == nullptr) {
        IMAGE_LOGE("apply space for I420 buffer failed!");
        return false;
    }

    const ImageConverter &converter_ = ConverterHandle.GetInstance()->GetHandle();
    const uint8_t* src_y = srcBuffer + yDInfo.yOffset;
    const uint8_t* src_vu = srcBuffer + yDInfo.uvOffset;
    int src_stride_vu = (yDInfo.uvStride + 1) / EVEN_ODD_DIVISOR * EVEN_ODD_DIVISOR;
    uint8_t* dst_u = yu12Buffer + yDInfo.yWidth * yDInfo.yHeight;
    int dst_stride_u = yDInfo.uvStride / EVEN_ODD_DIVISOR;
    uint8_t* dst_v = yu12Buffer + yDInfo.yWidth * yDInfo.yHeight + yDInfo.uvWidth * yDInfo.uvHeight;
    int dst_stride_v = yDInfo.uvStride / EVEN_ODD_DIVISOR;


    int ret = converter_.NV21ToI420(src_y, yDInfo.yStride, src_vu, src_stride_vu, yu12Buffer, yDInfo.yWidth,
        dst_u, dst_stride_u, dst_v, dst_stride_v, yDInfo.yWidth, yDInfo.yHeight);
    if (!ret) {
        IMAGE_LOGE("NV21ToI420 failed, ret = %{public}d!", ret);
        return false;
    }

    ret = converter_.I420ToRGB565Matrix( yu12Buffer, yDInfo.yWidth, dst_u, dst_stride_u, dst_v, dst_stride_v,
        *destBuffer, yDInfo.yWidth * NUM_2, yuvConstants, yDInfo.yWidth, yDInfo.yWidth);
    if (!ret) {
        IMAGE_LOGE("NV12ToI420 failed, ret = %{public}d!", ret);
        return false;
    }
    return true;
}
#endif

static bool NV21ToRGB565Manual(const uint8_t *srcBuffer, const YUVDataInfo &yDInfo, uint8_t **destBuffer)
{
    AVPixelFormat srcFormat = findPixelFormat(PixelFormat::NV21);
    AVPixelFormat dstFormat = findPixelFormat(PixelFormat::RGB_565);
    SwsContext *swsContext = sws_getContext(yDInfo.yWidth, yDInfo.yHeight, srcFormat, yDInfo.yWidth,
        yDInfo.yHeight, dstFormat, SWS_BILINEAR, nullptr, nullptr, nullptr);
    if (swsContext == nullptr) {
        IMAGE_LOGE("Error to create SwsContext.");
        return false;
    }

    const uint8_t *srcSlice[] = {srcBuffer + yDInfo.yOffset, srcBuffer + yDInfo.uvOffset};
    const int srcStride[] = {static_cast<int>(yDInfo.yStride),static_cast<int>(yDInfo.uvStride)};
    uint8_t *dstSlice[] = {*destBuffer};
    const int dstStride[] = {static_cast<int>(yDInfo.yWidth * BYTES_PER_PIXEL_RGB565)};
    int hight = sws_scale(swsContext, srcSlice, srcStride, SRCSLICEY, yDInfo.yHeight, dstSlice,
                                       dstStride);

    sws_freeContext(swsContext);
    if (hight == 0) {
        IMAGE_LOGE("Image pixel format conversion failed");
        return false;
    }
    return true;
}

#ifdef DCAMERA_MMAP_RESERVE
static bool NV21ToI420ToBGRA(const uint8_t *srcBuffer, const YUVDataInfo &yDInfo, uint8_t **destBuffer)
{
    uint32_t yu12BufferSize =yDInfo.yWidth * yDInfo.yHeight + ((yDInfo.yWidth + 1) / EVEN_ODD_DIVISOR *
        (yDInfo.yHeight + 1) / EVEN_ODD_DIVISOR * EVEN_ODD_DIVISOR);
    std::unique_ptr<uint8_t[]> yu12Buffer(new(std::nothrow) uint8_t[yu12BufferSize]());
    if (*yu12Buffer == nullptr) {
        IMAGE_LOGE("apply space for I420 buffer failed!");
        return false;
    }

    const ImageConverter &converter = ConverterHandle.GetInstance()->GetHandle();
    uint8_t* src_y = srcBuffer + yDInfo.yOffset;
    uint8_t* src_vu = srcBuffer + yDInfo.uvOffset;
    int src_stride_vu = (yDInfo.uvStride + 1) / EVEN_ODD_DIVISOR * EVEN_ODD_DIVISOR;
    uint8_t* dst_u = yu12Buffer + yDInfo.yWidth * yDInfo.yHeight;
    int dst_stride_u = (yDInfo.yWidth + 1) / EVEN_ODD_DIVISOR;
    uint8_t* dst_v = yu12Buffer + yDInfo.yWidth * yDInfo.yHeight + ((yDInfo.yWidth + 1) / EVEN_ODD_DIVISOR *
        (yDInfo.yHeight + 1) / EVEN_ODD_DIVISOR);
    int dst_stride_v = (yDInfo.yWidth + 1) / EVEN_ODD_DIVISOR;

    int ret = converter.NV21ToI420(src_y, yDInfo.yStride, src_vu, src_stride_vu, yu12Buffer, yDInfo.yWidth, dst_u,
        dst_stride_u, dst_v, dst_stride_v, yDInfo.yWidth, yDInfo.yHeight);
    if (!ret) {
        IMAGE_LOGE("NV21ToI420 failed, ret = %{public}d!", ret);
        return false;
    }

    int ret = converter.I420ToBGRA(yu12Buffer, yDInfo.yWidth, dst_u, dst_stride_u, dst_v, dst_stride_v, *destBuffer,
        yDInfo.yWidth * NUM_4, yDInfo.yWidth, yDInfo.yHeight);
    if (!ret) {
        IMAGE_LOGE("I420ToBGRA failed, ret = %{public}d!", ret);
        return false;
    }
    return true;
}
#endif

static bool NV21ToBGRAManual(const uint8_t *srcBuffer, const YUVDataInfo &yDInfo, uint8_t **destBuffer)
{
    AVPixelFormat srcFormat = findPixelFormat(PixelFormat::NV21);
    AVPixelFormat dstFormat = findPixelFormat(PixelFormat::BGRA_8888);
    SwsContext *swsContext = sws_getContext(yDInfo.yWidth, yDInfo.yHeight, srcFormat, yDInfo.yWidth,
        yDInfo.yHeight, dstFormat, SWS_BILINEAR, nullptr, nullptr, nullptr);
    if (swsContext == nullptr) {
        IMAGE_LOGD("Error to create SwsContext.");
        return false;
    }

    const uint8_t *srcSlice[] = {srcBuffer + yDInfo.yOffset, srcBuffer + yDInfo.uvOffset};
    const int srcStride[] = {static_cast<int>(yDInfo.yStride), static_cast<int>(yDInfo.uvStride)};
    uint8_t *dstSlice[] = {*destBuffer};
    const int dstStride[] = {static_cast<int>(yDInfo.yWidth * BYTES_PER_PIXEL_RGBA)};
    int hight = sws_scale(swsContext, srcSlice, srcStride, SRCSLICEY, yDInfo.yHeight, dstSlice,
                                       dstStride);
    sws_freeContext(swsContext);
    if (hight == 0) {
        IMAGE_LOGE("Image pixel format conversion failed");
        return false;
    }
    return true;
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
    const int dstStride[NUM_2] = {static_cast<int>(imageSize.width), static_cast<int>(widthEvent)};
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
    const int dstStride[NUM_2] = {static_cast<int>(imageSize.width), static_cast<int>(widthEvent)};
    int pixelFormatConvert = sws_scale(swsContext, srcSlice, srcStride, NUM_0, imageSize.height, dstSlice, dstStride);
    if (pixelFormatConvert == NUM_0) {
        delete[] destBuffer;
    }
    sws_freeContext(swsContext);
}

bool NV12ToRGB565(const uint8_t *srcBuffer, const YUVDataInfo &yDInfo, uint8_t **destBuffer,
                  size_t &destBufferSize, [[maybe_unused]]ColorSpace colorSpace)
{
    if (srcBuffer == nullptr || destBuffer == nullptr || yDInfo.yWidth == 0 || yDInfo.yHeight == 0 ||
        yDInfo.uvWidth == 0 || yDInfo.uvHeight == 0) {
        return false;
    }

    destBufferSize = static_cast<size_t>(yDInfo.yWidth * yDInfo.yHeight * BYTES_PER_PIXEL_RGB565);
    *destBuffer = new(std::nothrow) uint8_t[destBufferSize]();
    if (*destBuffer == nullptr) {
        IMAGE_LOGD("apply space for dest buffer failed!");
        return false;
    }
#ifdef DCAMERA_MMAP_RESERVE
    const uint8_t *srcY = srcBuffer + yDInfo.yOffset;
    const uint8_t *srcUV = srcBuffer + yDInfo.uvOffset;
    const struct YuvConstants* yuvConstants = mapColorSPaceToYuvConstants(colorSpace);
    int dstStrideRGB565 = yDInfo.yWidth * BYTES_PER_PIXEL_RGB565;
    int widthEven = (yDInfo.yWidth + 1) / EVEN_ODD_DIVISOR * TWO_SLICES;
    const ImageConverter &converter = ConverterHandle.GetInstance()->GetHandle();
    int32_t result = converter.NV12ToRGB565(srcY, yDInfo.yWidth, srcUV, widthEven, *destBuffer, dstStrideRGB565,
        yuvConstants, yDInfo.yWidth, yDInfo.yWidth);
    if (result) {
        delete[] (*destBuffer);
        IMAGE_LOGD("NV12 auto conversion to RGB565 failed!");
        return false;
    }
#else
    if (!NV12ToRGB565Manual(srcBuffer, yDInfo, destBuffer)) {
        delete[] (*destBuffer);
        IMAGE_LOGD("NV12 manual conversion to RGB565 failed!");
        return false;
    }
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
        imageSize.height, dstFormat, SWS_BILINEAR, nullptr, nullptr, nullptr);
    if (!swsContext) {
        IMAGE_LOGD("Error to create SwsContext.");
        sws_freeContext(swsContext);
    }

    int widthEvent = (imageSize.width % NUM_2 == NUM_0) ? (imageSize.width) : (imageSize.width + NUM_1);
    const uint8_t *srcSlice[NUM_1] = {srcBuffer};
    const int srcStride[NUM_1] = {static_cast<int>(imageSize.width * NUM_8)};
    uint8_t *dstSlice[NUM_2] = {*destBuffer, *destBuffer + imageSize.width * imageSize.height};
    const int dstStride[NUM_2] = {static_cast<int>(imageSize.width), static_cast<int>(widthEvent)};
    int pixelFormatConvert = sws_scale(swsContext, srcSlice, srcStride, 0, imageSize.height, dstSlice, dstStride);
    if (pixelFormatConvert == NUM_0) {
        delete[] destBuffer;
    }
    sws_freeContext(swsContext);
    return true;
}

bool NV21ToNV12(const uint8_t *srcBuffer, const YUVDataInfo &yDInfo, uint8_t **destBuffer,
                size_t &destBufferSize, [[maybe_unused]]ColorSpace colorSpace)
{
    if (srcBuffer == nullptr || destBuffer == nullptr || yDInfo.yWidth == 0 || yDInfo.yHeight == 0 ||
        yDInfo.uvWidth == 0 || yDInfo.uvHeight == 0) {
        return false;
    }
    destBufferSize = yDInfo.yWidth * yDInfo.yHeight + yDInfo.uvWidth * TWO_SLICES * yDInfo.uvHeight;
    *destBuffer = new(std::nothrow) uint8_t[destBufferSize];
    if (*destBuffer == nullptr) {
        IMAGE_LOGD("apply space for dest buffer failed!");
        return false;
    }
    bool result = false;
#ifdef DCAMERA_MMAP_RESERVE
    result = NV21ToNV12Auto(srcBuffer, yDInfo, destBuffer, destBufferSize);
#else
    result = NV21ToNV12Manual(srcBuffer, yDInfo, destBuffer);
#endif
    if (!result) {
        delete[] (*destBuffer);
        IMAGE_LOGD("NV21 conversion to NV12 failed!");
    }
    return result;
}

#ifdef DCAMERA_MMAP_RESERVE
static bool NV21ToNV12Auto(const uint8_t *srcBuffer, const YUVDataInfo &yDInfo, uint8_t **destBuffer,
    const size_t &destBufferSize)
{
    int32_t width = yDInfo.yWidth;
    int32_t height = yDInfo.yHeight;
    uint8_t *tempBuffer = new(std::nothrow) uint8_t[destBufferSize];
    if (tempBuffer  == nullptr) {
        return false;
    }
    uint8_t *tempY = tempBuffer;
    uint8_t *tempU = tempBuffer + width * height;
    uint8_t *tempV = tempU + ((width + 1) / EVEN_ODD_DIVISOR) * ((height + 1) / EVEN_ODD_DIVISOR);
    const ImageConverter &converter = ConverterHandle.GetInstance()->GetHandle();
    int32_t tempRet = converter.NV21ToI420(srcBuffer + yDInfo.yOffset, yDInfo.yStride,
        srcBuffer + yDInfo.uvOffset, yDInfo.uvStride,
        tempY, width,
        tempU, (width + 1) / EVEN_ODD_DIVISOR,
        tempV, (width + 1) / EVEN_ODD_DIVISOR,
        width,
        height);
    if (tempRet != 0) {
        delete[] tempBuffer;
        return false;
    }
    tempRet = converter.I420ToNV12(tempY, width,
        tempU, (width + 1) / EVEN_ODD_DIVISOR,
        tempV, (width + 1) / EVEN_ODD_DIVISOR,
        *destBuffer, width,
        *destBuffer + width * height, ((width + 1) / EVEN_ODD_DIVISOR) * TWO_SLICES,
        width,
        height);
    delete[] tempBuffer;
    return tempRet == 0 ? true : false;
}

static bool NV12ToNV21Auto(const uint8_t *srcBuffer, const YUVDataInfo &yDInfo, uint8_t **destBuffer,
    const size_t &destBufferSize)
{
    uint8_t *tempBuffer = new(std::nothrow) uint8_t[destBufferSize];
    if (tempBuffer  == nullptr) {
        return false;
    }
    int32_t width = yDInfo.yWidth;
    int32_t height = yDInfo.yHeight
    uint8_t *tempY = tempBuffer;
    uint8_t *tempU = tempBuffer + width * height;
    uint8_t *tempV = tempU + ((width + 1) / EVEN_ODD_DIVISOR) * ((height + 1) / EVEN_ODD_DIVISOR);
    const ImageConverter &converter = ConverterHandle.GetInstance()->GetHandle();

    int32_t result = converter.NV12ToI420(srcBuffer + yDInfo.yOffset, yDInfo.yStride,
        srcBuffer + yDInfo.uvOffset, yDInfo.uvStride,
        tempY, width,
        tempU, (width + 1) / EVEN_ODD_DIVISOR,
        tempV,
        (width + 1) / EVEN_ODD_DIVISOR,
        width,
        height);
    if (result != 0) {
        delete[] tempBuffer;
        return false;
    }
    result = converter.I420ToNV12(tempY, width,
        tempU, (width + 1) / EVEN_ODD_DIVISOR,
        tempV, (width + 1) / EVEN_ODD_DIVISOR,
        *destBuffer, width,
        *destBuffer + width * height, ((width + 1) / EVEN_ODD_DIVISOR) * TWO_SLICES,
        width,
        height);
    delete[] tempBuffer;
    return result == 0 ? true : false;
}
#endif

bool NV21ToRGBAF16(const uint8_t *srcBuffer, const YUVDataInfo &yDInfo, uint8_t **destBuffer,
                   size_t &destBufferSize, [[maybe_unused]]ColorSpace colorSpace)
{
    if (srcBuffer == nullptr || destBuffer == nullptr || yDInfo.yWidth == 0 || yDInfo.yHeight == 0 ||
        yDInfo.uvWidth == 0 || yDInfo.uvHeight == 0) {
        return false;
    }
    destBufferSize = yDInfo.yWidth * yDInfo.yHeight * STRIDES_PER_PLANE;
    *destBuffer = new(std::nothrow) uint8_t[destBufferSize]();
    if (*destBuffer == nullptr) {
        IMAGE_LOGD("apply space for dest buffer failed!");
        return false;
    }
    AVPixelFormat srcFormat = findPixelFormat(PixelFormat::NV21);
    AVPixelFormat dstFormat = findPixelFormat(PixelFormat::RGBA_F16);
    SwsContext *swsContext = sws_getContext(yDInfo.yWidth, yDInfo.yHeight, srcFormat, yDInfo.yWidth,
        yDInfo.yHeight, dstFormat, SWS_BILINEAR, nullptr, nullptr, nullptr);
    if (swsContext == nullptr) {
        IMAGE_LOGD("Error to create SwsContext.");
        return false;
    }
    
    const uint8_t *srcSlice[] = {srcBuffer + yDInfo.yOffset, srcBuffer + yDInfo.uvOffset};
    const int srcStride[] = {static_cast<int>(yDInfo.yStride), static_cast<int>(yDInfo.uvStride)};
    uint8_t *dstSlice[] = {*destBuffer};
    const int dstStride[] = {static_cast<int>(yDInfo.yWidth * STRIDES_PER_PLANE)};
    int height = sws_scale(swsContext, srcSlice, srcStride, SRCSLICEY, yDInfo.yHeight, dstSlice, dstStride);
    sws_freeContext(swsContext);
    if (height == 0) {
        delete[] (*destBuffer);
        return false;
    }
    return true;
}

bool NV12ToNV21(const uint8_t *srcBuffer, const YUVDataInfo &yDInfo, uint8_t **destBuffer,
                size_t &destBufferSize, [[maybe_unused]]ColorSpace colorSpace)
{
    if (srcBuffer == nullptr || destBuffer == nullptr || yDInfo.yWidth == 0 || yDInfo.yHeight == 0 ||
        yDInfo.uvWidth == 0 || yDInfo.uvHeight == 0) {
        return false;
    }
    destBufferSize = yDInfo.yWidth * yDInfo.yHeight + yDInfo.uvWidth * EVEN_ODD_DIVISOR * yDInfo.uvHeight;
    *destBuffer = new(std::nothrow) uint8_t[destBufferSize]();
    if (*destBuffer == nullptr) {
        IMAGE_LOGD("apply space for dest buffer failed!");
        return false;
    }
    bool result = false;
#ifdef DCAMERA_MMAP_RESERVE
    result = NV12ToNV21Auto(srcBuffer, yDInfo, destBuffer, destBufferSize);
#else
    result = NV12ToNV21Manual(srcBuffer, yDInfo, destBuffer);
#endif
    if (!result) {
        delete[] (*destBuffer);
    }
    return result;
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
#ifdef DCAMERA_MMAP_RESERVE
    uint8_t *nv12Y = *destBuffer;
    uint8_t *nv12UV = *destBuffer + destPlaneSizeY;
    auto &convertHandle = ConverterHandle.GetInstance();
    if (&converterHandle) {
        const ImageConverter &converter = convertHandle->GetHandle();
        if (&converter) {
            converter.ARGBToNV12(srcBuffer, imageSize.width * NUM_4, nv12Y, imageSize.width, nv12UV,
                (imageSize.width + NUM_1) / NUM_2 * NUM_2, imageSize.width, imageSize.height);
        }
    }
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

#ifdef DCAMERA_MMAP_RESERVE
    std::unique_ptr<uint8_t[]> yu12Buffer(new(std::nothrow) uint8_t[imageSize.width * imageSize.height +
        ((imageSize.width + NUM_1) / NUM_2) * ((imageSize.height + NUM_1) / NUM_2) * NUM_2]());
    if (yu12Buffer == nullptr) {
        IMAGE_LOGD("apply space for dest buffer failed!");
        return false;
    }
    auto &convertHandle = ConverterHandle.GetInstance();
    if (&converterHandle) {
        const ImageConverter &converter = convertHandle->GetHandle();
        if (&converter) {
            converter.RGB565ToI420(srcBuffer, imageSize.width * NUM_2, yu12Buffer, imageSize.width,
                yu12Buffer + imageSize.width * imageSize.height, (imageSize.width + NUM_1)/ NUM_2, yu12Buffer +
                imageSize.width * imageSize.height + ((imageSize.width + NUM_1) / NUM_2) *
                ((imageSize.height + NUM_1) / NUM_2), (imageSize.width + NUM_1) / NUM_2,
                imageSize.width, imageSize.height);
            converter.I420ToNV12(yu12Buffer, imageSize.width, yu12Buffer + imageSize.width * imageSize.height,
                (imageSize.width + NUM_1) / NUM_2, yu12Buffer + imageSize.width * imageSize.height +
                ((imageSize.width + NUM_1) / NUM_2) * ((imageSize.height + NUM_1) / NUM_2),
                (imageSize.width + NUM_1) / NUM_2, *destBuffer, imageSize.width, *destBuffer + imageSize.width *
                imageSize.height, ((imageSize.width + NUM_1) / NUM_2) * NUM_2, imageSize.width, imageSize.height);
        }
    }
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
#ifdef DCAMERA_MMAP_RESERVE
    std::unique_ptr<uint8_t[]> yu12Buffer(new(std::nothrow) uint8_t[imageSize.width * imageSize.height +
        ((imageSize.width + NUM_1) / NUM_2) * ((imageSize.height + NUM_1) / NUM_2) * NUM_2]());
    if (yu12Buffer == nullptr) {
        IMAGE_LOGD("apply space for dest buffer failed!");
        return false;
    }
    auto &convertHandle = ConverterHandle.GetInstance();
    if (&converterHandle) {
        const ImageConverter &converter = convertHandle->GetHandle();
        if (&converter) {
            converter.RGB565ToI420(srcBuffer, imageSize.width * NUM_2, yu12Buffer, imageSize.width,
                yu12Buffer + imageSize.width * imageSize.height, (imageSize.width + NUM_1) / NUM_2, yu12Buffer +
                imageSize.width * imageSize.height + ((imageSize.width + NUM_1) / NUM_2) *
                ((imageSize.height + NUM_1) / NUM_2), (imageSize.width + NUM_1) / NUM_2,
                imageSize.width, imageSize.height);
            converter.I420ToNV21(yu12Buffer, imageSize.width, yu12Buffer + imageSize.width * imageSize.height,
                (imageSize.width + NUM_1) / NUM_2, yu12Buffer + imageSize.width * imageSize.height +
                ((imageSize.width + NUM_1) / NUM_2) * ((imageSize.height + NUM_1) / NUM_2),
                (imageSize.width + NUM_1) / NUM_2, *destBuffer, imageSize.width, *destBuffer + imageSize.width *
                imageSize.height, ((imageSize.width + NUM_1) / NUM_2) * NUM_2, imageSize.width, imageSize.height);
        }
    }
#else
    RGB565ToNV21Manual(srcBuffer, imageSize, destBuffer);
#endif
    return true;
}

bool NV12ToRGBAF16(const uint8_t *data, const YUVDataInfo &yDInfo, uint8_t **destBuffer,
                   size_t &destBufferSize, [[maybe_unused]]ColorSpace colorSpace)
{
    if (data == nullptr || destBuffer == nullptr) {
        IMAGE_LOGE("Input buffer pointer data or destBuffer is null!");
        return false;
    }

    uint32_t frameSize = yDInfo.yWidth * yDInfo.yHeight;
    destBufferSize = frameSize * sizeof(uint64_t);
    if (destBufferSize == 0) {
        IMAGE_LOGE("Invalid destination buffer size is 0!");
        return false;
    }

    *destBuffer = new(std::nothrow) uint8_t[destBufferSize]();
    if (*destBuffer == nullptr) {
        IMAGE_LOGE("Dynamically allocating memory for destination buffer failed!");
        return false;
    }

    if (!NV12ToRGBAF16Manual(data, yDInfo, destBuffer)) {
        IMAGE_LOGE("NV12ToRGBAF16 failed!");
        delete[] (*destBuffer);
        *destBuffer = nullptr;
        return false;
    }

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
#ifdef DCAMERA_MMAP_RESERVE
    uint8_t *nv21Y = *destBuffer;
    uint8_t *nv21VU = *destBuffer + destPlaneSizeY;
    auto &convertHandle = ConverterHandle.GetInstance();
    if (&converterHandle) {
        const ImageConverter &converter = convertHandle->GetHandle();
        if (&converter) {
            converter.ARGBToNV21(srcBuffer, imageSize.width * NUM_4, nv21Y, imageSize.width, nv21VU,
                (imageSize.width + NUM_1) / NUM_2 * NUM_2, imageSize.width, imageSize.height);
        }
    }
#else
    BGRAToNV21Manual(srcBuffer, imageSize, destBuffer);
#endif
    return true;
}

bool NV12ToRGBA(const uint8_t *data, const YUVDataInfo &yDInfo, uint8_t **destBuffer,
                size_t &destBufferSize, [[maybe_unused]]ColorSpace colorSpace)
{
    if (data == nullptr || destBuffer == nullptr) {
        IMAGE_LOGE("Input buffer pointer data or destBuffer is null!");
        return false;
    }

    destBufferSize = static_cast<size_t>(yDInfo.yWidth * yDInfo.yHeight * BYTES_PER_PIXEL_RGBA);
    if (destBufferSize == 0) {
        IMAGE_LOGE("Invalid destination buffer size is 0!");
        return false;
    }

    *destBuffer = new(std::nothrow) uint8_t[destBufferSize]();
    if (*destBuffer == nullptr) {
        IMAGE_LOGE("Dynamically allocating memory for destination buffer failed!");
        return false;
    }

    bool bRet = false;
#ifdef DCAMERA_MMAP_RESERVE
    bRet = NV12ToRGBANoManual(data, yDInfo, destBuffer, colorSpace);
#else
    bRet = NV12ToRGBAManual(data, yDInfo, destBuffer);
#endif
    if (!bRet) {
        IMAGE_LOGE("NV12ToRGBA failed!");
        delete[] (*destBuffer);
        *destBuffer = nullptr;
        return false;
    }

    return true;
}

bool NV12ToBGRA(const uint8_t *data, const YUVDataInfo &yDInfo, uint8_t **destBuffer,
                size_t &destBufferSize, [[maybe_unused]]ColorSpace colorSpace)
{
    if (data == nullptr || destBuffer == nullptr) {
        IMAGE_LOGE("Input buffer pointer data or destBuffer is null!");
        return false;
    }

    destBufferSize = static_cast<size_t>(yDInfo.yWidth * yDInfo.yHeight * BYTES_PER_PIXEL_BGRA);
    if (destBufferSize == 0) {
        IMAGE_LOGE("Invalid destination buffer size is 0!");
        return false;
    }

    *destBuffer = new(std::nothrow) uint8_t[destBufferSize]();
    if (*destBuffer == nullptr) {
        IMAGE_LOGE("Dynamically allocating memory for destination buffer failed!");
        return false;
    }

    bool bRet = false;
#ifdef DCAMERA_MMAP_RESERVE
    bRet = NV12ToBGRANoManual(data, yDInfo, destBuffer, colorSpace);
#else
    bRet = NV12ToBGRAManual(data, yDInfo, destBuffer);
#endif
    if (!bRet) {
        IMAGE_LOGE("NV12ToBGRA failed!");
        return false;
    }

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
        imageSize.height, dstFormat, SWS_BILINEAR, nullptr, nullptr, nullptr);
    if (!swsContext) {
        sws_freeContext(swsContext);
        return false;
    }

    int widthEvent = (imageSize.width % NUM_2 == NUM_0) ? (imageSize.width) : (imageSize.width + NUM_1);
    const uint8_t *srcSlice[NUM_1] = {srcBuffer};
    const int srcStride[NUM_1] = {static_cast<int>(imageSize.width * NUM_8)};
    uint8_t *dstSlice[NUM_2] = {*destBuffer, *destBuffer + imageSize.width * imageSize.height};
    const int dstStride[NUM_2] = {static_cast<int>(imageSize.width), static_cast<int>(widthEvent)};
    int pixelFormatConvert = sws_scale(swsContext, srcSlice, srcStride, 0, imageSize.height, dstSlice, dstStride);
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
#ifdef DCAMERA_MMAP_RESERVE
    const uint32_t i420BufferSize = static_cast<size_t>(imageSize.width * imageSize.height +
        ((imageSize.width + NUM_1) / NUM_2) * ((imageSize.height + NUM_1) / NUM_2) * NUM_2);
    std::unique_ptr<uint8[]> i420Buffer(new(std::nothrow) uint8_t[i420BufferSize]());
    if (i420Buffer == nullptr) {
        IMAGE_LOGD("apply space for dest buffer failed!");
        return false;
    }
    uint8_t *i420Y = i420Buffer;
    uint8_t *i420U = i420Buffer + imageSize.width * imageSize.height;
    uint8_t *i420V = i420Buffer + imageSize.width * imageSize.height +
        ((imageSize.width + NUM_1) / NUM_2) * ((imageSize.height + NUM_1) / NUM_2);
    uint8_t *nv12Y = *destBuffer;
    uint8_t *nv12UV = *destBuffer + imageSize.width * imageSize.height;
    auto &convertHandle = ConverterHandle.GetInstance();
    if (&converterHandle) {
        const ImageConverter &converter = convertHandle->GetHandle();
        if (&converter) {
            converter.ARGBToI420(srcBuffer, NUM_4 * imageSize.width, i420Y, imageSize.width, i420U,
            (imageSize.width + NUM_1) / NUM_2, i420V, (imageSize.width + NUM_1) / NUM_2,
            imageSize.width, imageSize.height);
                converter.I420ToNV12(i420Y, imageSize.width, i420V, (imageSize.width + NUM_1) / NUM_2, i420U,
            (imageSize.width + NUM_1) / NUM_2, nv12Y, imageSize.width, nv12UV,
            (imageSize.width + NUM_1) / NUM_2 * NUM_2, imageSize.width, imageSize.height);
        }
    }
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

#ifdef DCAMERA_MMAP_RESERVE
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
    uint8_t *nv21Y = *destBuffer;
    uint8_t *nv21VU = *destBuffer + imageSize.width * imageSize.height;
    auto &convertHandle = ConverterHandle.GetInstance();
    if (&converterHandle) {
        const ImageConverter &converter = convertHandle->GetHandle();
        if (&converter) {
            converter.ARGBToI420(srcBuffer, NUM_4 * imageSize.width, i420Y, imageSize.width, i420U,
                (imageSize.width + NUM_1) / NUM_2, i420V, (imageSize.width + NUM_1) / NUM_2,
                imageSize.width, imageSize.height);
            converter.I420ToNV21(i420Y, imageSize.width, i420V, (imageSize.width + NUM_1) / NUM_2, i420U,
                (imageSize.width + NUM_1) / NUM_2, nv21Y, imageSize.width, nv21VU,
                (imageSize.width + NUM_1) / NUM_2 * NUM_2, imageSize.width, imageSize.height);
        }
    }
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
#ifdef DCAMERA_MMAP_RESERVE
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
    auto &convertHandle = ConverterHandle.GetInstance();
    if (&converterHandle) {
        const ImageConverter &converter = convertHandle->GetHandle();
        if (&converter) {
            converter.RGB24ToI420(srcBuffer, imageSize.width * NUM_3, I420Y, imageSize.width, I420U,
                (imageSize.width + NUM_1) / NUM_2, I420V, (imageSize.width + NUM_1) / NUM_2,
                imageSize.width, imageSize.height);
            converter.I420ToNV21(I420Y, imageSize.width,I420U, (imageSize.width + NUM_1 )/ NUM_2,
                I420V, (imageSize.width + NUM_1) / NUM_2, *destBuffer, imageSize.width,
                *destBuffer + imageSize.width * imageSize.height + NUM_1,
                (imageSize.width + NUM_1) / NUM_2 * NUM_2, imageSize.width, imageSize.height);
        }
    }
#else
    RGBToNV21Manual(srcBuffer, imageSize, destBuffer);
#endif
    return true;
}

#ifdef DCAMERA_MMAP_RESERVE
static bool NV12ToRGBNoManual(const uint8_t *srcBuffer, const YUVDataInfo &yDInfo, uint8_t **destBuffer,
                              ColorSpace colorSpace)
{
    const struct YuvConstants* yuvConstants = MapColorSpaceToYuvConstants(colorSpace);
    const ImageConverter &converter = ConverterHandle::GetInstance()->GetHandle();
    int32_t ret = converter.NV12ToRGB24Matrix(srcBuffer + yDInfo.yOffset, yDInfo.yStride, srcBuffer + yDInfo.uvOffset,
        yDInfo.uvStride, *destBuffer, yDInfo.yStride * BYTES_PER_PIXEL_RGB, yuvConstants, yDInfo.yWidth,
        yDInfo.yHeight);
    if (ret != 0) {
        IMAGE_LOGE("NV12ToRGB24Matrix failed, ret = %{public}d!", ret);
        return false;
    }

    return true;
}
#endif

static bool NV12ToRGBManual(const uint8_t *srcBuffer, const YUVDataInfo &yDInfo, uint8_t **destBuffer)
{
    AVPixelFormat srcFormat = findPixelFormat(PixelFormat::NV12);
    AVPixelFormat dstFormat = findPixelFormat(PixelFormat::RGB_888);
    struct SwsContext *ctx = sws_getContext(yDInfo.yWidth, yDInfo.yHeight, srcFormat,
                                            yDInfo.yWidth, yDInfo.yHeight, dstFormat,
                                            SWS_BILINEAR, nullptr, nullptr, nullptr);
    if (ctx == nullptr) {
        IMAGE_LOGE("sws_getContext: result is null!");
        return false;
    }

    const uint8_t *srcSlice[] = { srcBuffer + yDInfo.yOffset, srcBuffer + yDInfo.uvOffset };
    const int srcStride[] = { static_cast<int>(yDInfo.yStride), static_cast<int>(yDInfo.uvStride) };
    const int dstStride[] = { static_cast<int>(yDInfo.yWidth * BYTES_PER_PIXEL_RGB) };
    int sliceHeight = sws_scale(ctx, srcSlice, srcStride, SRCSLICEY, yDInfo.yHeight, destBuffer, dstStride);
    sws_freeContext(ctx);

    if (sliceHeight == 0) {
        IMAGE_LOGE("sws_scale: the height of the output slice is 0!");
        return false;
    }
    return true;
}

bool NV12ToRGB(const uint8_t *data, const YUVDataInfo &yDInfo, uint8_t **destBuffer,
               size_t &destBufferSize, [[maybe_unused]]ColorSpace colorSpace)
{
    if (data == nullptr || destBuffer == nullptr) {
        IMAGE_LOGE("Input buffer pointer data or destBuffer is null!");
        return false;
    }

    destBufferSize = static_cast<size_t>(yDInfo.yWidth * yDInfo.yHeight * BYTES_PER_PIXEL_RGB);
    if (destBufferSize == 0) {
        IMAGE_LOGE("Invalid destination buffer size is 0!");
        return false;
    }

    (*destBuffer) = new(std::nothrow) uint8_t[destBufferSize]();
    if (*destBuffer == nullptr) {
        IMAGE_LOGE("Dynamically allocating memory for destination buffer failed!");
        return false;
    }

    bool bRet = false;
#ifdef DCAMERA_MMAP_RESERVE
    bRet = NV12ToRGBNoManual(data, yDInfo, destBuffer, colorSpace);
#else
    bRet = NV12ToRGBManual(data, yDInfo, destBuffer);
#endif
    if (!bRet) {
        IMAGE_LOGE("NV12ToRGB failed!");
        delete[] (*destBuffer);
        *destBuffer = nullptr;
        return false;
    }

    return true;
}

bool NV21ToRGB(const uint8_t *data, const YUVDataInfo &yDInfo, uint8_t **destBuffer,
               size_t &destBufferSize, [[maybe_unused]]ColorSpace colorSpace)
{
    if (data == nullptr || destBuffer == nullptr || yDInfo.yWidth == 0 || yDInfo.yHeight == 0 ||
        yDInfo.uvWidth == 0 || yDInfo.uvHeight == 0) {
        return false;
    }

    destBufferSize = static_cast<size_t>(yDInfo.yWidth * yDInfo.yHeight * BYTES_PER_PIXEL_RGB);
    *destBuffer = new(std::nothrow) uint8_t[destBufferSize]();
    if (*destBuffer == nullptr) {
        IMAGE_LOGE("apply space for dest buffer failed!");
        return false;
    }

#ifdef DCAMERA_MMAP_RESERVE
    const struct YuvConstants* yuvConstants = mapColorSPaceToYuvConstants(colorSpace);
    const ImageConverter &converter = ConverterHandle.GetInstance()->GetHandle();
    converter.NV21ToRGB24Matrix(data + yDInfo.yOffset, yDInfo.yStride, data + yDInfo.uvOffset,
        (yDInfo.uvStride + 1) / EVEN_ODD_DIVISOR * EVEN_ODD_DIVISOR, *destBuffer, yDInfo.yWidth * BYTES_PER_PIXEL_RGB,
        yuvConstants, yDInfo.yWidth, yDInfo.yHeight);
#else
    AVPixelFormat srcFormat = findPixelFormat(PixelFormat::NV21);
    AVPixelFormat dstFormat = findPixelFormat(PixelFormat::RGB_888);
    SwsContext *swsContext = sws_getContext(yDInfo.yWidth, yDInfo.yHeight, srcFormat, yDInfo.yWidth,
                                             yDInfo.yHeight, dstFormat, SWS_BILINEAR, nullptr, nullptr, nullptr);
    if (swsContext == nullptr) {
        IMAGE_LOGE("Error to create SwsContext.");
        return false;
    }

    const uint8_t *srcSlice[] = { data + yDInfo.yOffset, data + yDInfo.uvOffset };
    const int srcStride[] = { static_cast<int>(yDInfo.yStride), static_cast<int>(yDInfo.uvStride) };
    const int dstStride[] = {static_cast<int>(yDInfo.yWidth * BYTES_PER_PIXEL_RGB)};
    int hight = sws_scale(swsContext, srcSlice, srcStride, SRCSLICEY, yDInfo.yHeight,
        destBuffer, dstStride);
    sws_freeContext(swsContext);
    if (hight == 0) {
        IMAGE_LOGE("Image pixel format conversion failed");
        delete[] destBuffer;
        return false;
    }
#endif
    return true;
}

bool NV21ToRGBA(const uint8_t *srcBuffer, const YUVDataInfo &yDInfo, uint8_t **destBuffer,
                size_t &destBufferSize, ColorSpace colorSpace)
{
    if (srcBuffer == nullptr || destBuffer == nullptr || yDInfo.yWidth == 0 || yDInfo.yHeight == 0 ||
        yDInfo.uvWidth == 0 || yDInfo.uvHeight == 0) {
        return false;
    }

    destBufferSize = static_cast<size_t>(yDInfo.yWidth * yDInfo.yHeight * BYTES_PER_PIXEL_RGBA);
    *destBuffer = new(std::nothrow) uint8_t[destBufferSize]();
    if (*destBuffer == nullptr) {
        IMAGE_LOGE("Apply space for dest buffer failed!");
        return false;
    }
    bool bRet = false;
#ifdef DCAMERA_MMAP_RESERVE
    bRet = NV21ToRGBAMatrix(srcBuffer, yDInfo, destBuffer)
#else
    bRet = NV21ToRGBAManual(srcBuffer, yDInfo, destBuffer);
#endif
    if (!bRet) {
        IMAGE_LOGE("NV21ToRGBA failed!");
        delete[] destBuffer;
        return bRet;
    }
    return true;
}

bool NV21ToBGRA(const uint8_t *srcBuffer, const YUVDataInfo &yDInfo, uint8_t **destBuffer,
                size_t &destBufferSize, [[maybe_unused]]ColorSpace colorSpace)
{
    if (srcBuffer == nullptr || destBuffer == nullptr || yDInfo.yWidth == 0 || yDInfo.yHeight == 0 ||
        yDInfo.uvWidth == 0 || yDInfo.uvHeight == 0) {
        return false;
    }

    destBufferSize = static_cast<size_t>(yDInfo.yWidth * yDInfo.yHeight * BYTES_PER_PIXEL_BGRA);
    *destBuffer = new(std::nothrow) uint8_t[destBufferSize]();
    if (*destBuffer == nullptr) {
        IMAGE_LOGE("apply space for dest buffer failed!");
        return false;
    }

    bool bRet = false;
#ifdef DCAMERA_MMAP_RESERVE
    bRet = NV21ToI420ToBGRA(srcBuffer, yDInfo, destBuffer);
#else
    bRet = NV21ToBGRAManual(srcBuffer, yDInfo, destBuffer);
#endif
    if (!bRet) {
        IMAGE_LOGE("NV21ToBGRA failed!");
        delete[] destBuffer;
        return bRet;
    }
    return true;
}

bool NV21ToRGB565(const uint8_t *srcBuffer, const YUVDataInfo &yDInfo, uint8_t **destBuffer,
                  size_t &destBufferSize, ColorSpace colorSpace)
{
    if (srcBuffer == nullptr || destBuffer == nullptr || yDInfo.yWidth == 0 || yDInfo.yHeight == 0 ||
        yDInfo.uvWidth == 0 || yDInfo.uvHeight == 0) {
        return false;
    }

    destBufferSize = static_cast<size_t>(yDInfo.yWidth * yDInfo.yHeight * BYTES_PER_PIXEL_RGB565);
    *destBuffer = new(std::nothrow) uint8_t[destBufferSize]();
    if (*destBuffer == nullptr) {
        IMAGE_LOGD("apply space for dest buffer failed!");
        return false;
    }

    bool bRet = false;
#ifdef DCAMERA_MMAP_RESERVE
    bRet = NV21ToRGB565Matrix(srcBuffer, yDInfo, destBuffer);
#else
    bRet = NV21ToRGB565Manual(srcBuffer, yDInfo, destBuffer);
#endif
    if (!bRet) {
        IMAGE_LOGE("NV21ToRGB565 failed!");
        delete[] destBuffer;
        return bRet;
    }
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
#ifdef DCAMERA_MMAP_RESERVE
    uint8_t *yu12Buffer (new(std::nothrow) uint8_t[imageSize.width * imageSize.height +
        (imageSize.width + NUM_1) / NUM_2 * (imageSize.height + NUM_1) / NUM_2 * NUM_2]());
    if (*yu12Buffer == nullptr) {
        IMAGE_LOGD("apply space for I420 buffer failed!");
        return false;
    }
    auto &convertHandle = ConverterHandle.GetInstance();
    if (&converterHandle) {
        const ImageConverter &converter = convertHandle->GetHandle();
        if (&converter) {
            converter.RGB24ToI420(srcBuffer, imageSize.width * NUM_3, yu12Buffer, imageSize.width,
                yu12Buffer + imageSize.width * imageSize.height, (imageSize.width + NUM_1) / NUM_2,
                yu12Buffer + imageSize.width * imageSize.height + (imageSize.width + NUM_1) / NUM_2 *
                (imageSize.height + NUM_1) / NUM_2, (imageSize.width + NUM_1) / NUM_2,
                imageSize.width, imageSize.height);
            converter.I420ToNV12(yu12Buffer, imageSize.width, yu12Buffer + imageSize.width * imageSize.height,
                (imageSize.width + NUM_1) / NUM_2, yu12Buffer + imageSize.width * imageSize.height +
                (imageSize.width + NUM_1) / NUM_2 * (imageSize.height + NUM_1) / NUM_2, (imageSize.width + NUM_1) /
                NUM_2, *destBuffer, imageSize.width, *destBuffer + imageSize.width * imageSize.height + NUM_1,
                (imageSize.width + NUM_1) / NUM_2 * NUM_2, imageSize.width, imageSize.height);
        }
    }
#else
    RGBToNV12Manual(srcBuffer, imageSize, destBuffer);
#endif
    return true;
}
} // namespace Media
} // namespace OHOS