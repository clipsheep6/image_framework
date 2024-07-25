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
#include <map>
#include "hilog/log.h"
#include "image_log.h"
#include "log_tags.h"
#include "securec.h"
#include "pixel_convert_adapter.h"

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
    constexpr uint32_t SRCSLICEY = 0;
    constexpr uint32_t EVEN_ODD_DIVISOR = 2;
    constexpr uint32_t TWO_SLICES = 2;
    constexpr uint32_t BYTES_PER_PIXEL_RGB565 = 2;
    constexpr uint32_t BYTES_PER_PIXEL_RGB = 3;
    constexpr uint32_t BYTES_PER_PIXEL_RGBA = 4;
    constexpr uint32_t BYTES_PER_PIXEL_BGRA = 4;
    constexpr uint32_t STRIDES_PER_PLANE = 8;
    constexpr int32_t PIXEL_MAP_MAX_RAM_SIZE = 600 * 1024 * 1024;
}

#undef LOG_TAG
#define LOG_TAG "ImageFormatConvert"
namespace OHOS {
namespace Media {
static AVPixelFormat findPixelFormat(PixelFormat format)
{
    auto formatSearch = PixelConvertAdapter::FFMPEG_PIXEL_FORMAT_MAP.find(format);
    if (formatSearch != PixelConvertAdapter::FFMPEG_PIXEL_FORMAT_MAP.end()) {
        return formatSearch->second;
    } else {
        return AV_PIX_FMT_NONE;
    }
}

static bool CalcRGBStride(PixelFormat format, uint32_t width, int &stride)
{
    auto avFormat = findPixelFormat(format);
    switch (avFormat) {
        case AV_PIX_FMT_RGB565:
            stride = static_cast<int>(width * BYTES_PER_PIXEL_RGB565);
            break;
        case AV_PIX_FMT_RGBA:
            IMAGE_LOGE("yyytest CalcRGBStride AV_PIX_FMT_RGBA");
            stride = static_cast<int>(width * BYTES_PER_PIXEL_RGBA);
            break;
        case AV_PIX_FMT_RGBA64:
            stride = static_cast<int>(width * STRIDES_PER_PLANE);
            break;
        case AV_PIX_FMT_BGRA:
            stride = static_cast<int>(width * BYTES_PER_PIXEL_BGRA);
            break;
        case AV_PIX_FMT_RGB24:
            stride = static_cast<int>(width * BYTES_PER_PIXEL_RGB);
            break;
        default:
            return false;
    }
    return true;
}

static bool YuvToRGBParam(const YUVDataInfo &yDInfo, SrcConvertParam &srcParam, DstConvertParam &dstParam,
                          DstConvertDataInfo &dstInfo)
{
    srcParam.slice[0] = srcParam.buffer + yDInfo.yOffset;
    srcParam.slice[1] = srcParam.buffer + yDInfo.uvOffset;
    srcParam.stride[0] = static_cast<int>(yDInfo.yStride);
    srcParam.stride[1] = static_cast<int>(yDInfo.uvStride);
    IMAGE_LOGE("yyytest YuvToRGBParam yOffset=%{public}d, uvOffset=%{public}d",yDInfo.yOffset, yDInfo.uvOffset);
    IMAGE_LOGE("yyytest YuvToRGBParam yStride=%{public}d, uvStride=%{public}d",yDInfo.yStride, yDInfo.uvStride);
    int dstStride = 0;
    if (dstInfo.allocType == AllocatorType::DMA_ALLOC) {
        dstStride = dstInfo.yStride;
        dstParam.slice[0] = dstInfo.buffer + dstInfo.yOffset;
    } else {
        IMAGE_LOGE("yyytest YuvToRGBParam CalcRGBStride start");
        auto bRet = CalcRGBStride(dstParam.format, dstParam.width, dstStride);
        if (!bRet) {
            return false;
        }
        dstParam.slice[0] = dstInfo.buffer;
    }
    dstParam.stride[0] = dstStride;
    IMAGE_LOGE("yyytest YuvToRGBParam dstStride=%{public}d", dstStride);
    return true;
}

static bool YuvToYuvParam(const YUVDataInfo &yDInfo, SrcConvertParam &srcParam, DstConvertParam &dstParam,
                          DstConvertDataInfo &dstInfo)
{
    srcParam.slice[0] = srcParam.buffer + yDInfo.yOffset;
    srcParam.slice[1] = srcParam.buffer + yDInfo.uvOffset;

    srcParam.stride[0] = static_cast<int>(yDInfo.yStride);
    srcParam.stride[1] = static_cast<int>(yDInfo.uvStride);

    int dstyStride = 0;
    int dstuvStride = 0;
    if (dstInfo.allocType == AllocatorType::DMA_ALLOC) {
        dstyStride = dstInfo.yStride;
        dstuvStride = dstInfo.uvStride;
        dstParam.slice[0] = dstInfo.buffer + dstInfo.yOffset;
        dstParam.slice[1] = dstInfo.buffer + dstInfo.uvOffset;
    } else {
        IMAGE_LOGE("yyytest YuvToRGBParam CalcRGBStride start");
        dstyStride = static_cast<int>(dstParam.width);
        dstuvStride = (dstParam.width % EVEN_ODD_DIVISOR == 0) ? (dstParam.width) : (dstParam.width + 1);
        dstParam.slice[0] = dstInfo.buffer;
        dstParam.slice[1] = dstInfo.buffer + dstyStride * dstParam.height;
    }

    dstParam.stride[0] = dstyStride;
    dstParam.stride[1] = dstuvStride;
    return true;
}

static bool RGBToYuvParam(const RGBDataInfo &rgbInfo, SrcConvertParam &srcParam, DstConvertParam &dstParam,
                          DstConvertDataInfo &dstInfo)
{
    srcParam.slice[0] = srcParam.buffer;
    srcParam.stride[0] = static_cast<int>(rgbInfo.stride);

    if (dstInfo.allocType == AllocatorType::DMA_ALLOC) {
        dstParam.stride[0] = static_cast<int>(dstInfo.yStride);
        dstParam.stride[1] = static_cast<int>(dstInfo.uvStride);
        dstParam.slice[0] = dstInfo.buffer + dstInfo.yOffset;
        dstParam.slice[1] = dstInfo.buffer + dstInfo.uvOffset;
    } else {
        IMAGE_LOGE("yyytest YuvToRGBParam CalcRGBStride start");
        int uvStride = (dstParam.width % EVEN_ODD_DIVISOR == 0) ? (dstParam.width) : (dstParam.width + 1);
        dstParam.stride[0] = static_cast<int>(dstParam.width);
        dstParam.stride[1] = static_cast<int>(uvStride);
        dstParam.slice[0] = dstInfo.buffer;
        dstParam.slice[1] = dstInfo.buffer + dstParam.width * dstParam.height;
    }
    return true;
}

static bool SoftDecode(const SrcConvertParam &srcParam, const DstConvertParam &dstParam)
{
    auto srcformat = findPixelFormat(srcParam.format);
    auto dstformat = findPixelFormat(dstParam.format);
    SwsContext *swsContext = sws_getContext(srcParam.width, srcParam.height, srcformat, dstParam.width,
        dstParam.height, dstformat, SWS_BILINEAR, nullptr, nullptr, nullptr);
    if (swsContext == nullptr) {
        IMAGE_LOGE("yyytest Error to create SwsContext.");
        return false;
    }
    int height = 0;
    const uint8_t *srcSlice[] = {srcParam.slice[0], srcParam.slice[1]};
    int srcStride[] = {static_cast<int>(srcParam.stride[0]), static_cast<int>(srcParam.stride[1])};
    uint8_t *dstSlice[] = {dstParam.slice[0], dstParam.slice[1]};
    int dstStride[] = {static_cast<int>(dstParam.stride[0]), static_cast<int>(dstParam.stride[1])};
    height = sws_scale(swsContext, srcSlice, srcStride, SRCSLICEY, dstParam.height, dstSlice, dstStride);

    sws_freeContext(swsContext);
    if (height == 0) {
        IMAGE_LOGE("yyytest Image pixel format conversion failed");
        return false;
    }
    return true;
}

static bool YuvToYuv(const uint8_t *srcBuffer, const YUVDataInfo &yDInfo,PixelFormat srcFormat,
    DstConvertDataInfo &dstInfo, PixelFormat dstFormat)
{
    IMAGE_LOGE("yyytest yuv conversion to yuv start!");
    if (srcBuffer == nullptr || dstInfo.buffer == nullptr || yDInfo.yWidth == 0 || yDInfo.yHeight == 0 ||
        yDInfo.uvWidth == 0 || yDInfo.uvHeight == 0 || dstInfo.bufferSize == 0) {
        return false;
    }
    SrcConvertParam srcParam;
    srcParam.format = srcFormat;
    srcParam.buffer = srcBuffer;

    DstConvertParam dstParam;
    dstParam.format = dstFormat;

    if (!YuvToYuvParam(yDInfo, srcParam, dstParam, dstInfo)) {
        IMAGE_LOGE("yuv conversion to yuv failed!");
        return false;
    }
    if (!SoftDecode(srcParam, dstParam)) {
        IMAGE_LOGE("yuv manual conversion to yuv failed!");
        return false;
    }
    IMAGE_LOGE("yyytest YuvToYuv succ!");
    return true;
}

static bool YuvToRGB(const uint8_t *srcBuffer, const YUVDataInfo &yDInfo, PixelFormat srcFormat,
    DstConvertDataInfo &dstInfo, PixelFormat dstFormat)
{
    IMAGE_LOGE("yyytest yuv conversion to RGB start!");
    if (srcBuffer == nullptr || dstInfo.buffer == nullptr || yDInfo.yWidth == 0 || yDInfo.yHeight == 0 ||
        yDInfo.uvWidth == 0 || yDInfo.uvHeight == 0 || dstInfo.bufferSize == 0) {
        return false;
    }
    SrcConvertParam srcParam = {yDInfo.yWidth, yDInfo.yHeight};
    srcParam.format = srcFormat;
    srcParam.buffer = srcBuffer;

    DstConvertParam dstParam = {yDInfo.yWidth, yDInfo.yHeight};
    dstParam.format = dstFormat;

    if (!YuvToRGBParam(yDInfo, srcParam, dstParam, dstInfo)) {
        IMAGE_LOGE("yyytest yuv conversion to RGB failed!");
        return false;
    }
    if (!SoftDecode(srcParam, dstParam)) {
        IMAGE_LOGE("yyytest yuv manual conversion to RGB failed!");
        return false;
    }
    IMAGE_LOGE("yyytest YuvToRGB succ!");
    return true;
}

static bool RGBToYuv(const uint8_t *srcBuffer, const RGBDataInfo &rgbInfo, PixelFormat srcFormat,
                     DstConvertDataInfo &dstInfo, PixelFormat dstFormat)
{
    IMAGE_LOGE("yyytest RGB conversion to YUV start!");
    if (srcBuffer == nullptr || dstInfo.buffer == nullptr || rgbInfo.width == 0 || rgbInfo.height == 0 ||
        dstInfo.bufferSize == 0) {
        return false;
    }
    SrcConvertParam srcParam = {rgbInfo.width, rgbInfo.height};
    srcParam.format = srcFormat;
    srcParam.buffer = srcBuffer;

    DstConvertParam dstParam = {rgbInfo.width, rgbInfo.height};
    dstParam.format = dstFormat;

    if (!RGBToYuvParam(rgbInfo, srcParam, dstParam, dstInfo)) {
        IMAGE_LOGE("yyytest RGB conversion to YUV failed!");
        return false;
    }
    if (!SoftDecode(srcParam, dstParam)) {
        IMAGE_LOGE("yyytest RGB manual conversion to YUV failed!");
        return false;
    }
    IMAGE_LOGE("yyytest RGBToYuv succ!");
    return true;
}

bool ImageFormatConvertUtils::NV12ToRGB565New(const uint8_t *srcBuffer, const YUVDataInfo &yDInfo,
                                              DstConvertDataInfo &dstInfo,
                                              [[maybe_unused]]ColorSpace colorSpace)
{

    return YuvToRGB(srcBuffer, yDInfo, PixelFormat::NV12, dstInfo, PixelFormat::RGB_565);
}

bool ImageFormatConvertUtils::NV21ToRGB565New(const uint8_t *srcBuffer, const YUVDataInfo &yDInfo,
                                              DstConvertDataInfo &dstInfo,
                                              [[maybe_unused]]ColorSpace colorSpace)
{
    return YuvToRGB(srcBuffer, yDInfo, PixelFormat::NV21, dstInfo, PixelFormat::RGB_565);
}

bool ImageFormatConvertUtils::NV21ToRGBNew(const uint8_t *srcBuffer, const YUVDataInfo &yDInfo,
                                           DstConvertDataInfo &dstInfo,
                                           [[maybe_unused]]ColorSpace colorSpace)
{
    return YuvToRGB(srcBuffer, yDInfo, PixelFormat::NV21, dstInfo, PixelFormat::RGB_888);
}

bool ImageFormatConvertUtils::NV12ToRGBNew(const uint8_t *srcBuffer, const YUVDataInfo &yDInfo,
                                           DstConvertDataInfo &dstInfo,
                                           [[maybe_unused]]ColorSpace colorSpace)
{
    return YuvToRGB(srcBuffer, yDInfo, PixelFormat::NV12, dstInfo, PixelFormat::RGB_888);
}

bool ImageFormatConvertUtils::NV21ToRGBANew(const uint8_t *srcBuffer, const YUVDataInfo &yDInfo,
                                            DstConvertDataInfo &dstInfo,
                                            [[maybe_unused]]ColorSpace colorSpace)
{
    IMAGE_LOGE("yyytest NV21 conversion to RGBA_8888 start!");
    return YuvToRGB(srcBuffer, yDInfo, PixelFormat::NV21, dstInfo, PixelFormat::RGBA_8888);
}

bool ImageFormatConvertUtils::NV12ToRGBANew(const uint8_t *srcBuffer, const YUVDataInfo &yDInfo,
                                            DstConvertDataInfo &dstInfo,
                                            [[maybe_unused]]ColorSpace colorSpace)
{
    IMAGE_LOGE("yyytest NV12 conversion to RGBA_8888 start!");
    return YuvToRGB(srcBuffer, yDInfo, PixelFormat::NV12, dstInfo, PixelFormat::RGBA_8888);
}

bool ImageFormatConvertUtils::NV21ToBGRANew(const uint8_t *srcBuffer, const YUVDataInfo &yDInfo,
                                            DstConvertDataInfo &dstInfo,
                                            [[maybe_unused]]ColorSpace colorSpace)
{
    return YuvToRGB(srcBuffer, yDInfo, PixelFormat::NV21, dstInfo, PixelFormat::BGRA_8888);
}

bool ImageFormatConvertUtils::NV12ToBGRANew(const uint8_t *srcBuffer, const YUVDataInfo &yDInfo,
                                            DstConvertDataInfo &dstInfo,
                                            [[maybe_unused]]ColorSpace colorSpace)
{
    return YuvToRGB(srcBuffer, yDInfo, PixelFormat::NV12, dstInfo, PixelFormat::BGRA_8888);
}

bool ImageFormatConvertUtils::NV21ToRGBAF16New(const uint8_t *srcBuffer, const YUVDataInfo &yDInfo,
                                               DstConvertDataInfo &dstInfo,
                                               [[maybe_unused]]ColorSpace colorSpace)
{
    return YuvToRGB(srcBuffer, yDInfo, PixelFormat::NV12, dstInfo, PixelFormat::RGBA_F16);
}

bool ImageFormatConvertUtils::NV12ToRGBAF16New(const uint8_t *srcBuffer, const YUVDataInfo &yDInfo,
                                               DstConvertDataInfo &dstInfo,
                                               [[maybe_unused]]ColorSpace colorSpace)
{
    return YuvToRGB(srcBuffer, yDInfo, PixelFormat::NV12, dstInfo, PixelFormat::RGBA_F16);
}

bool ImageFormatConvertUtils::NV12ToNV21New(const uint8_t *srcBuffer, const YUVDataInfo &yDInfo,
                                            DstConvertDataInfo &dstInfo,
                                            [[maybe_unused]]ColorSpace colorSpace)
{
    IMAGE_LOGE("yyytest NV12 conversion to N21 start!");
    return YuvToYuv(srcBuffer, yDInfo, PixelFormat::NV12, dstInfo, PixelFormat::NV21);
}

bool ImageFormatConvertUtils::NV21ToNV12New(const uint8_t *srcBuffer, const YUVDataInfo &yDInfo,
                                            DstConvertDataInfo &dstInfo,
                                            [[maybe_unused]]ColorSpace colorSpace)
{
    IMAGE_LOGE("yyytest NV21 conversion to N12 start!");
    return YuvToYuv(srcBuffer, yDInfo, PixelFormat::NV21, dstInfo, PixelFormat::NV12);
}

bool ImageFormatConvertUtils::RGBToNV21New(const uint8_t *srcBuffer, const RGBDataInfo &rgbInfo,
                                           DstConvertDataInfo &dstInfo,
                                           [[maybe_unused]]ColorSpace colorSpace)
{
    IMAGE_LOGE("yyytest RGB_888 conversion to N21 start!");
    return RGBToYuv(srcBuffer, rgbInfo, PixelFormat::RGB_888, dstInfo, PixelFormat::NV21);
}

bool ImageFormatConvertUtils::RGBToNV12New(const uint8_t *srcBuffer, const RGBDataInfo &rgbInfo,
                                           DstConvertDataInfo &dstInfo,
                                           [[maybe_unused]]ColorSpace colorSpace)
{
    IMAGE_LOGE("yyytest RGB_888 conversion to N12 start!");
    return RGBToYuv(srcBuffer, rgbInfo, PixelFormat::RGB_888, dstInfo, PixelFormat::NV12);
}

bool ImageFormatConvertUtils::RGB565ToNV21New(const uint8_t *srcBuffer, const RGBDataInfo &rgbInfo,
                                              DstConvertDataInfo &dstInfo,
                                              [[maybe_unused]]ColorSpace colorSpace)
{
    IMAGE_LOGE("yyytest RGB_565 conversion to N21 start!");
    return RGBToYuv(srcBuffer, rgbInfo, PixelFormat::RGB_565, dstInfo, PixelFormat::NV21);
}

bool ImageFormatConvertUtils::RGB565ToNV12New(const uint8_t *srcBuffer, const RGBDataInfo &rgbInfo,
                                              DstConvertDataInfo &dstInfo,
                                              [[maybe_unused]]ColorSpace colorSpace)
{
    IMAGE_LOGE("yyytest RGB_565 conversion to N12 start!");
    return RGBToYuv(srcBuffer, rgbInfo, PixelFormat::RGB_565, dstInfo, PixelFormat::NV12);
}

bool ImageFormatConvertUtils::RGBAToNV21New(const uint8_t *srcBuffer, const RGBDataInfo &rgbInfo,
                                            DstConvertDataInfo &dstInfo,
                                            [[maybe_unused]]ColorSpace colorSpace)
{
    IMAGE_LOGE("yyytest RGBA_8888 conversion to N21 start!");
    return RGBToYuv(srcBuffer, rgbInfo, PixelFormat::RGBA_8888, dstInfo, PixelFormat::NV21);
}

bool ImageFormatConvertUtils::RGBAToNV12New(const uint8_t *srcBuffer, const RGBDataInfo &rgbInfo,
                                            DstConvertDataInfo &dstInfo,
                                            [[maybe_unused]]ColorSpace colorSpace)
{
    IMAGE_LOGE("yyytest RGBA_8888 conversion to N12 start!");
    return RGBToYuv(srcBuffer, rgbInfo, PixelFormat::RGBA_8888, dstInfo, PixelFormat::NV12);
}

bool ImageFormatConvertUtils::BGRAToNV21New(const uint8_t *srcBuffer, const RGBDataInfo &rgbInfo,
                                            DstConvertDataInfo &dstInfo,
                                            [[maybe_unused]]ColorSpace colorSpace)
{
    IMAGE_LOGE("yyytest BGRA_8888 conversion to N21 start!");
    return RGBToYuv(srcBuffer, rgbInfo, PixelFormat::BGRA_8888, dstInfo, PixelFormat::NV21);
}

bool ImageFormatConvertUtils::BGRAToNV12New(const uint8_t *srcBuffer, const RGBDataInfo &rgbInfo,
                                            DstConvertDataInfo &dstInfo,
                                            [[maybe_unused]]ColorSpace colorSpace)
{
    IMAGE_LOGE("yyytest BGRA_8888 conversion to N12 start!");
    return RGBToYuv(srcBuffer, rgbInfo, PixelFormat::BGRA_8888, dstInfo, PixelFormat::NV12);
}

bool ImageFormatConvertUtils::RGBAF16ToNV21New(const uint8_t *srcBuffer, const RGBDataInfo &rgbInfo,
                                               DstConvertDataInfo &dstInfo,
                                               [[maybe_unused]]ColorSpace colorSpace)
{
    IMAGE_LOGE("yyytest BGRA_8888 conversion to N12 start!");
    return RGBToYuv(srcBuffer, rgbInfo, PixelFormat::RGBA_F16, dstInfo, PixelFormat::NV21);
}

bool ImageFormatConvertUtils::RGBAF16ToNV12New(const uint8_t *srcBuffer, const RGBDataInfo &rgbInfo,
                                               DstConvertDataInfo &dstInfo,
                                               [[maybe_unused]]ColorSpace colorSpace)
{
    IMAGE_LOGE("yyytest BGRA_8888 conversion to N12 start!");
    return RGBToYuv(srcBuffer, rgbInfo, PixelFormat::RGBA_F16, dstInfo, PixelFormat::NV12);
}

static bool NV12ToRGB565SoftDecode(const uint8_t *srcBuffer, const YUVDataInfo &yDInfo, uint8_t **destBuffer)
{
    AVPixelFormat srcFormat = findPixelFormat(PixelFormat::NV12);
    AVPixelFormat dstFormat = findPixelFormat(PixelFormat::RGB_565);
    SwsContext *swsContext = sws_getContext(yDInfo.yWidth, yDInfo.yHeight, srcFormat, yDInfo.yWidth,
        yDInfo.yHeight, dstFormat, SWS_BILINEAR, nullptr, nullptr, nullptr);
    if (swsContext == nullptr) {
        IMAGE_LOGE("Error to create SwsContext.");
        return false;
    }

    const uint8_t *srcSlice[] = {srcBuffer + yDInfo.yOffset, srcBuffer + yDInfo.uvOffset};
    const int srcStride[] = {static_cast<int>(yDInfo.yStride), static_cast<int>(yDInfo.uvStride)};
    uint8_t *dstSlice[] = {*destBuffer};
    const int dstStride[] = {static_cast<int>(yDInfo.yWidth * BYTES_PER_PIXEL_RGB565)};
    int height = sws_scale(swsContext, srcSlice, srcStride, SRCSLICEY, yDInfo.yHeight, dstSlice, dstStride);
    sws_freeContext(swsContext);
    if (height == 0) {
        IMAGE_LOGE("Image pixel format conversion failed");
        return false;
    }
    return true;
}

static bool NV21ToNV12SoftDecode(const uint8_t *srcBuffer, const YUVDataInfo &yDInfo, uint8_t **destBuffer)
{
    AVPixelFormat srcFormat = findPixelFormat(PixelFormat::NV21);
    AVPixelFormat dstFormat = findPixelFormat(PixelFormat::NV12);
    SwsContext *swsContext = sws_getContext(yDInfo.yWidth, yDInfo.yHeight, srcFormat, yDInfo.yWidth,
        yDInfo.yHeight, dstFormat, SWS_BILINEAR, nullptr, nullptr, nullptr);
    if (swsContext == nullptr) {
        IMAGE_LOGE("Error to create SwsContext.");
        return false;
    }

    int widthEvent = (yDInfo.yWidth % EVEN_ODD_DIVISOR == 0) ? (yDInfo.yWidth) : (yDInfo.yWidth + 1);
    const uint8_t *srcSlice[] = {srcBuffer + yDInfo.yOffset, srcBuffer + yDInfo.uvOffset};
    const int srcStride[] = {static_cast<int>(yDInfo.yStride), static_cast<int>(yDInfo.uvStride)};
    uint8_t *dstSlice[] = {*destBuffer, *destBuffer + yDInfo.yWidth * yDInfo.yHeight};
    const int dstStride[] = {static_cast<int>(yDInfo.yWidth), static_cast<int>(widthEvent)};

    int height = sws_scale(swsContext, srcSlice, srcStride, SRCSLICEY, yDInfo.yHeight, dstSlice, dstStride);
    sws_freeContext(swsContext);
    if (height == 0) {
        IMAGE_LOGE("Image pixel format conversion failed");
        return false;
    }
    return true;
}

static bool NV12ToNV21SoftDecode(const uint8_t *srcBuffer, const YUVDataInfo &yDInfo, uint8_t **destBuffer)
{
    AVPixelFormat srcFormat = findPixelFormat(PixelFormat::NV12);
    AVPixelFormat dstFormat = findPixelFormat(PixelFormat::NV21);
    SwsContext *swsContext = sws_getContext(yDInfo.yWidth, yDInfo.yHeight, srcFormat, yDInfo.yWidth,
        yDInfo.yHeight, dstFormat, SWS_BILINEAR, nullptr, nullptr, nullptr);
    if (swsContext == nullptr) {
        IMAGE_LOGE("Error to create SwsContext.");
        return false;
    }

    int widthEvent = (yDInfo.yWidth % EVEN_ODD_DIVISOR == 0) ? (yDInfo.yWidth) : (yDInfo.yWidth + 1);
    const uint8_t *srcSlice[] = {srcBuffer + yDInfo.yOffset, srcBuffer + yDInfo.uvOffset};
    const int srcStride[] = {static_cast<int>(yDInfo.yStride), static_cast<int>(yDInfo.uvStride)};
    uint8_t *dstSlice[] = {*destBuffer, *destBuffer + yDInfo.yWidth * yDInfo.yHeight};
    const int dstStride[] = {static_cast<int>(yDInfo.yWidth), static_cast<int>(widthEvent)};

    int height = sws_scale(swsContext, srcSlice, srcStride, SRCSLICEY, yDInfo.yHeight, dstSlice, dstStride);
    sws_freeContext(swsContext);
    if (height == 0) {
        IMAGE_LOGE("Image pixel format conversion failed");
        return false;
    }
    return true;
}

static bool RGB565ToNV12SoftDecode(const uint8_t *srcBuffer, const RGBDataInfo &rgbInfo, uint8_t **destBuffer)
{
    AVPixelFormat srcFormat = findPixelFormat(PixelFormat::RGB_565);
    AVPixelFormat dstFormat = findPixelFormat(PixelFormat::NV12);
    SwsContext *swsContext = sws_getContext(rgbInfo.width, rgbInfo.height, srcFormat, rgbInfo.width,
        rgbInfo.height, dstFormat, SWS_BILINEAR, nullptr, nullptr, nullptr);
    if (swsContext == nullptr) {
        IMAGE_LOGE("Error to create SwsContext.");
        return false;
    }

    int widthEvent = (rgbInfo.width % EVEN_ODD_DIVISOR == 0) ? (rgbInfo.width) : (rgbInfo.width + 1);
    const uint8_t *srcSlice[] = {srcBuffer};
    const int srcStride[] = {static_cast<int>(rgbInfo.width * BYTES_PER_PIXEL_RGB565)};
    uint8_t *dstSlice[] = {*destBuffer, *destBuffer + rgbInfo.width * rgbInfo.height};
    const int dstStride[] = {static_cast<int>(rgbInfo.width), static_cast<int>(widthEvent)};
    int height = sws_scale(swsContext, srcSlice, srcStride, SRCSLICEY, rgbInfo.height, dstSlice, dstStride);
    sws_freeContext(swsContext);
    if (height == 0) {
        IMAGE_LOGE("Image pixel format conversion failed");
        return false;
    }
    return true;
}

static bool RGB565ToNV21SoftDecode(const uint8_t *srcBuffer, const RGBDataInfo &rgbInfo, uint8_t **destBuffer)
{
    AVPixelFormat srcFormat = findPixelFormat(PixelFormat::RGB_565);
    AVPixelFormat dstFormat = findPixelFormat(PixelFormat::NV21);
    SwsContext *swsContext = sws_getContext(rgbInfo.width, rgbInfo.height, srcFormat, rgbInfo.width,
        rgbInfo.height, dstFormat, SWS_BILINEAR, nullptr, nullptr, nullptr);
    if (swsContext == nullptr) {
        IMAGE_LOGE("Error to create SwsContext.");
        return false;
    }

    int widthEvent = (rgbInfo.width % EVEN_ODD_DIVISOR == 0) ? (rgbInfo.width) : (rgbInfo.width + 1);
    const uint8_t *srcSlice[] = {srcBuffer};
    const int srcStride[] = {static_cast<int>(rgbInfo.width * BYTES_PER_PIXEL_RGB565)};
    uint8_t *dstSlice[] = {*destBuffer, *destBuffer + rgbInfo.width * rgbInfo.height};
    const int dstStride[] = {static_cast<int>(rgbInfo.width), static_cast<int>(widthEvent)};
    int height = sws_scale(swsContext, srcSlice, srcStride, SRCSLICEY, rgbInfo.height, dstSlice, dstStride);
    sws_freeContext(swsContext);
    if (height == 0) {
        IMAGE_LOGE("Image pixel format conversion failed");
        return false;
    }
    return true;
}

static bool NV12ToRGBAF16SoftDecode(const uint8_t *srcBuffer, const YUVDataInfo &yDInfo, uint8_t **destBuffer)
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
    int height = sws_scale(ctx, srcSlice, srcStride, SRCSLICEY, yDInfo.yHeight, destBuffer, dstStride);
    sws_freeContext(ctx);
    if (height == 0) {
        IMAGE_LOGE("Image pixel format conversion failed");
        return false;
    }
    return true;
}

static bool BGRAToNV21SoftDecode(const uint8_t *srcBuffer, const RGBDataInfo &rgbInfo, uint8_t **destBuffer)
{
    AVPixelFormat srcFormat = findPixelFormat(PixelFormat::BGRA_8888);
    AVPixelFormat dstFormat = findPixelFormat(PixelFormat::NV21);
    SwsContext *swsContext = sws_getContext(rgbInfo.width, rgbInfo.height, srcFormat, rgbInfo.width,
        rgbInfo.height, dstFormat, SWS_BILINEAR, nullptr, nullptr, nullptr);
    if (swsContext == nullptr) {
        IMAGE_LOGE("Error to create SwsContext.");
        return false;
    }

    int widthEvent = (rgbInfo.width % EVEN_ODD_DIVISOR == 0) ? (rgbInfo.width) : (rgbInfo.width + 1);
    const uint8_t *srcSlice[] = {srcBuffer};
    const int srcStride[] = {static_cast<int>(rgbInfo.width * BYTES_PER_PIXEL_BGRA)};
    uint8_t *dstSlice[] = {*destBuffer, *destBuffer + rgbInfo.width * rgbInfo.height};
    const int dstStride[] = {static_cast<int>(rgbInfo.width), static_cast<int>(widthEvent)};
    int height = sws_scale(swsContext, srcSlice, srcStride, SRCSLICEY, rgbInfo.height, dstSlice, dstStride);
    sws_freeContext(swsContext);
    if (height == 0) {
        IMAGE_LOGE("Image pixel format conversion failed");
        return false;
    }
    return true;
}

static bool NV12ToRGBASoftDecode(const uint8_t *srcBuffer, const YUVDataInfo &yDInfo, uint8_t **destBuffer)
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
    int height = sws_scale(ctx, srcSlice, srcStride, SRCSLICEY, yDInfo.yHeight, destBuffer, dstStride);
    sws_freeContext(ctx);

    if (height == 0) {
        IMAGE_LOGE("Image pixel format conversion failed");
        return false;
    }
    return true;
}

static bool NV12ToBGRASoftDecode(const uint8_t *srcBuffer, const YUVDataInfo &yDInfo, uint8_t **destBuffer)
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
    int height = sws_scale(ctx, srcSlice, srcStride, SRCSLICEY, yDInfo.yHeight, destBuffer, dstStride);
    sws_freeContext(ctx);

    if (height == 0) {
        IMAGE_LOGE("Image pixel format conversion failed");
        return false;
    }
    return true;
}

static bool RGBAToNV12SoftDecode(const uint8_t *srcBuffer, const RGBDataInfo &rgbInfo, uint8_t **destBuffer)
{
    AVPixelFormat srcFormat = findPixelFormat(PixelFormat::RGBA_8888);
    AVPixelFormat dstFormat = findPixelFormat(PixelFormat::NV12);
    SwsContext *swsContext = sws_getContext(rgbInfo.width, rgbInfo.height, srcFormat, rgbInfo.width,
        rgbInfo.height, dstFormat, SWS_BILINEAR, nullptr, nullptr, nullptr);
    if (swsContext == nullptr) {
        IMAGE_LOGE("Error to create SwsContext.");
        return false;
    }

    int widthEvent = (rgbInfo.width % EVEN_ODD_DIVISOR == 0) ? (rgbInfo.width) : (rgbInfo.width + 1);
    const uint8_t *srcSlice[] = {srcBuffer};
    const int srcStride[] = {static_cast<int>(rgbInfo.width * BYTES_PER_PIXEL_RGBA)};
    uint8_t *dstSlice[] = {*destBuffer, *destBuffer + rgbInfo.width * rgbInfo.height};
    const int dstStride[] = {static_cast<int>(rgbInfo.width), static_cast<int>(widthEvent)};
    int height = sws_scale(swsContext, srcSlice, srcStride, SRCSLICEY, rgbInfo.height, dstSlice, dstStride);
    sws_freeContext(swsContext);
    if (height == 0) {
        IMAGE_LOGE("Image pixel format conversion failed");
        return false;
    }
    return true;
}

static bool RGBAToNV21SoftDecode(const uint8_t *srcBuffer, const RGBDataInfo &rgbInfo, uint8_t **destBuffer)
{
    AVPixelFormat srcFormat = findPixelFormat(PixelFormat::RGBA_8888);
    AVPixelFormat dstFormat = findPixelFormat(PixelFormat::NV21);
    SwsContext *swsContext = sws_getContext(rgbInfo.width, rgbInfo.height, srcFormat, rgbInfo.width,
        rgbInfo.height, dstFormat, SWS_BILINEAR, nullptr, nullptr, nullptr);
    if (swsContext == nullptr) {
        IMAGE_LOGE("Error to create SwsContext.");
        return false;
    }

    int widthEvent = (rgbInfo.width % EVEN_ODD_DIVISOR == 0) ? (rgbInfo.width) : (rgbInfo.width + 1);
    const uint8_t *srcSlice[] = {srcBuffer};
    const int srcStride[] = {static_cast<int>(rgbInfo.width * BYTES_PER_PIXEL_RGBA)};
    uint8_t *dstSlice[] = {*destBuffer, *destBuffer + rgbInfo.width * rgbInfo.height};
    const int dstStride[] = {static_cast<int>(rgbInfo.width), static_cast<int>(widthEvent)};
    int height = sws_scale(swsContext, srcSlice, srcStride, SRCSLICEY, rgbInfo.height, dstSlice, dstStride);
    sws_freeContext(swsContext);
    if (height == 0) {
        IMAGE_LOGE("Image pixel format conversion failed");
        return false;
    }
    return true;
}

static bool RGBToNV21SoftDecode(const uint8_t *srcBuffer, const RGBDataInfo &rgbInfo, uint8_t **destBuffer)
{
    AVPixelFormat srcFormat = findPixelFormat(PixelFormat::RGB_888);
    AVPixelFormat dstFormat = findPixelFormat(PixelFormat::NV21);
    SwsContext *swsContext = sws_getContext(rgbInfo.width, rgbInfo.height, srcFormat, rgbInfo.width,
        rgbInfo.height, dstFormat, SWS_BILINEAR, nullptr, nullptr, nullptr);
    if (swsContext == nullptr) {
        IMAGE_LOGE("Error to create SwsContext.");
        return false;
    }

    int widthEvent = (rgbInfo.width % EVEN_ODD_DIVISOR == 0) ? (rgbInfo.width) : (rgbInfo.width + 1);
    const uint8_t *srcSlice[] = {srcBuffer};
    const int srcStride[] = {static_cast<int>(rgbInfo.width * BYTES_PER_PIXEL_RGB)};
    uint8_t *dstSlice[] = {*destBuffer, *destBuffer + rgbInfo.width * rgbInfo.height};
    const int dstStride[] = {static_cast<int>(rgbInfo.width), static_cast<int>(widthEvent)};
    int height = sws_scale(swsContext, srcSlice, srcStride, SRCSLICEY, rgbInfo.height, dstSlice, dstStride);
    sws_freeContext(swsContext);
    if (height == 0) {
        IMAGE_LOGE("Image pixel format conversion failed");
        return false;
    }
    return true;
}

static bool NV21ToRGBASoftDecode(const uint8_t *srcBuffer, const YUVDataInfo &yDInfo, uint8_t **destBuffer)
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
    const int srcStride[] = {static_cast<int>(yDInfo.yStride), static_cast<int>(yDInfo.uvStride)};
    uint8_t *dstSlice[] = {*destBuffer};
    const int dstStride[] = {static_cast<int>(yDInfo.yWidth * BYTES_PER_PIXEL_RGBA)};
    int height = sws_scale(swsContext, srcSlice, srcStride, SRCSLICEY, yDInfo.yHeight, dstSlice, dstStride);
    sws_freeContext(swsContext);
    if (height == 0) {
        IMAGE_LOGE("Image pixel format conversion failed");
        return false;
    }
    return true;
}

static bool NV21ToRGB565SoftDecode(const uint8_t *srcBuffer, const YUVDataInfo &yDInfo, uint8_t **destBuffer)
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
    const int srcStride[] = {static_cast<int>(yDInfo.yStride), static_cast<int>(yDInfo.uvStride)};
    uint8_t *dstSlice[] = {*destBuffer};
    const int dstStride[] = {static_cast<int>(yDInfo.yWidth * BYTES_PER_PIXEL_RGB565)};
    int height = sws_scale(swsContext, srcSlice, srcStride, SRCSLICEY, yDInfo.yHeight, dstSlice, dstStride);

    sws_freeContext(swsContext);
    if (height == 0) {
        IMAGE_LOGE("Image pixel format conversion failed");
        return false;
    }
    return true;
}

static bool NV21ToBGRASoftDecode(const uint8_t *srcBuffer, const YUVDataInfo &yDInfo, uint8_t **destBuffer)
{
    AVPixelFormat srcFormat = findPixelFormat(PixelFormat::NV21);
    AVPixelFormat dstFormat = findPixelFormat(PixelFormat::BGRA_8888);
    SwsContext *swsContext = sws_getContext(yDInfo.yWidth, yDInfo.yHeight, srcFormat, yDInfo.yWidth,
        yDInfo.yHeight, dstFormat, SWS_BILINEAR, nullptr, nullptr, nullptr);
    if (swsContext == nullptr) {
        IMAGE_LOGE("Error to create SwsContext.");
        return false;
    }

    const uint8_t *srcSlice[] = {srcBuffer + yDInfo.yOffset, srcBuffer + yDInfo.uvOffset};
    const int srcStride[] = {static_cast<int>(yDInfo.yStride), static_cast<int>(yDInfo.uvStride)};
    uint8_t *dstSlice[] = {*destBuffer};
    const int dstStride[] = {static_cast<int>(yDInfo.yWidth * BYTES_PER_PIXEL_RGBA)};
    int height = sws_scale(swsContext, srcSlice, srcStride, SRCSLICEY, yDInfo.yHeight, dstSlice, dstStride);
    sws_freeContext(swsContext);
    if (height == 0) {
        IMAGE_LOGE("Image pixel format conversion failed");
        return false;
    }
    return true;
}

static bool RGBToNV12SoftDecode(const uint8_t *srcBuffer, const RGBDataInfo &rgbInfo, uint8_t **destBuffer)
{
    AVPixelFormat srcFormat = findPixelFormat(PixelFormat::RGB_888);
    AVPixelFormat dstFormat = findPixelFormat(PixelFormat::NV12);
    SwsContext *swsContext = sws_getContext(rgbInfo.width, rgbInfo.height, srcFormat, rgbInfo.width,
        rgbInfo.height, dstFormat, SWS_BILINEAR, nullptr, nullptr, nullptr);
    if (swsContext == nullptr) {
        IMAGE_LOGE("Error to create SwsContext.");
        return false;
    }

    int widthEvent = (rgbInfo.width % EVEN_ODD_DIVISOR == 0) ? (rgbInfo.width) : (rgbInfo.width + 1);
    const uint8_t *srcSlice[] = {srcBuffer};
    const int srcStride[] = {static_cast<int>(rgbInfo.width * BYTES_PER_PIXEL_RGB)};
    uint8_t *dstSlice[] = {*destBuffer, *destBuffer + rgbInfo.width * rgbInfo.height};
    const int dstStride[] = {static_cast<int>(rgbInfo.width), static_cast<int>(widthEvent)};
    int height = sws_scale(swsContext, srcSlice, srcStride, SRCSLICEY, rgbInfo.height, dstSlice, dstStride);
    sws_freeContext(swsContext);
    if (height == 0) {
        IMAGE_LOGE("Image pixel format conversion failed");
        return false;
    }
    return true;
}

static bool BGRAToNV12SoftDecode(const uint8_t *srcBuffer, const RGBDataInfo &rgbInfo, uint8_t **destBuffer)
{
    AVPixelFormat srcFormat = findPixelFormat(PixelFormat::BGRA_8888);
    AVPixelFormat dstFormat = findPixelFormat(PixelFormat::NV12);
    SwsContext *swsContext = sws_getContext(rgbInfo.width, rgbInfo.height, srcFormat, rgbInfo.width,
        rgbInfo.height, dstFormat, SWS_BILINEAR, nullptr, nullptr, nullptr);
    if (swsContext == nullptr) {
        IMAGE_LOGE("Error to create SwsContext.");
        return false;
    }

    int widthEvent = (rgbInfo.width % EVEN_ODD_DIVISOR == 0) ? (rgbInfo.width) : (rgbInfo.width + 1);
    const uint8_t *srcSlice[] = {srcBuffer};
    const int srcStride[] = {static_cast<int>(rgbInfo.width * BYTES_PER_PIXEL_BGRA)};
    uint8_t *dstSlice[] = {*destBuffer, *destBuffer + rgbInfo.width * rgbInfo.height};
    const int dstStride[] = {static_cast<int>(rgbInfo.width), static_cast<int>(widthEvent)};
    int height = sws_scale(swsContext, srcSlice, srcStride, SRCSLICEY, rgbInfo.height, dstSlice, dstStride);
    sws_freeContext(swsContext);
    if (height == 0) {
        IMAGE_LOGE("Image pixel format conversion failed");
        return false;
    }
    return true;
}

bool ImageFormatConvertUtils::NV12ToRGB565(const uint8_t *srcBuffer, const YUVDataInfo &yDInfo, uint8_t **destBuffer,
                                           size_t &destBufferSize, [[maybe_unused]]ColorSpace colorSpace)
{
    if (srcBuffer == nullptr || destBuffer == nullptr || yDInfo.yWidth == 0 || yDInfo.yHeight == 0 ||
        yDInfo.uvWidth == 0 || yDInfo.uvHeight == 0) {
        return false;
    }

    destBufferSize = static_cast<size_t>(yDInfo.yWidth * yDInfo.yHeight * BYTES_PER_PIXEL_RGB565);
    if (destBufferSize == 0 || destBufferSize > PIXEL_MAP_MAX_RAM_SIZE) {
        IMAGE_LOGE("Invalid destination buffer size calculation!");
        return false;
    }
    *destBuffer = new(std::nothrow) uint8_t[destBufferSize]();
    if (*destBuffer == nullptr) {
        IMAGE_LOGE("apply space for dest buffer failed!");
        return false;
    }
    if (!NV12ToRGB565SoftDecode(srcBuffer, yDInfo, destBuffer)) {
        delete[] (*destBuffer);
        IMAGE_LOGE("NV12 manual conversion to RGB565 failed!");
        return false;
    }
    return true;
}

bool ImageFormatConvertUtils::RGBAF16ToNV21(const uint8_t *srcBuffer, const RGBDataInfo &rgbInfo,
    uint8_t **destBuffer, size_t &destBufferSize, [[maybe_unused]]ColorSpace colorSpace)
{
    if (srcBuffer == nullptr || destBuffer == nullptr || rgbInfo.width < 0 || rgbInfo.height < 0) {
        return false;
    }
    uint32_t frameSize = rgbInfo.width * rgbInfo.height;
    destBufferSize = frameSize +
        (((rgbInfo.width + 1) / TWO_SLICES * (rgbInfo.height + 1) / TWO_SLICES) * TWO_SLICES);
    if (destBufferSize == 0 || destBufferSize > PIXEL_MAP_MAX_RAM_SIZE) {
        IMAGE_LOGE("Invalid destination buffer size calculation!");
        return false;
    }
    *destBuffer = new(std::nothrow) uint8_t[destBufferSize]();
    if (*destBuffer == nullptr) {
        IMAGE_LOGE("apply space for dest buffer failed!");
        return false;
    }

    AVPixelFormat srcFormat = findPixelFormat(PixelFormat::RGBA_F16);
    AVPixelFormat dstFormat = findPixelFormat(PixelFormat::NV21);
    SwsContext *swsContext = sws_getContext(rgbInfo.width, rgbInfo.height, srcFormat, rgbInfo.width,
        rgbInfo.height, dstFormat, SWS_BILINEAR, nullptr, nullptr, nullptr);
    if (swsContext == nullptr) {
        IMAGE_LOGE("Error to create SwsContext.");
        delete[] destBuffer;
        *destBuffer = nullptr;
        return false;
    }

    int widthEvent = (rgbInfo.width % EVEN_ODD_DIVISOR == 0) ? (rgbInfo.width) : (rgbInfo.width + 1);
    const uint8_t *srcSlice[] = {srcBuffer};
    const int srcStride[] = {static_cast<int>(rgbInfo.width * STRIDES_PER_PLANE)};
    uint8_t *dstSlice[] = {*destBuffer, *destBuffer + rgbInfo.width * rgbInfo.height};
    const int dstStride[] = {static_cast<int>(rgbInfo.width), static_cast<int>(widthEvent)};
    int height = sws_scale(swsContext, srcSlice, srcStride, SRCSLICEY, rgbInfo.height, dstSlice, dstStride);
    sws_freeContext(swsContext);
    if (height == 0) {
        IMAGE_LOGE("RGBAF16ToNV21 format conversion failed");
        delete[] destBuffer;
        *destBuffer = nullptr;
        return false;
    }
    return true;
}

bool ImageFormatConvertUtils::NV21ToNV12(const uint8_t *srcBuffer, const YUVDataInfo &yDInfo, uint8_t **destBuffer,
                                         size_t &destBufferSize, [[maybe_unused]]ColorSpace colorSpace)
{
    if (srcBuffer == nullptr || destBuffer == nullptr || yDInfo.yWidth == 0 || yDInfo.yHeight == 0 ||
        yDInfo.uvWidth == 0 || yDInfo.uvHeight == 0) {
        return false;
    }
    destBufferSize = yDInfo.yWidth * yDInfo.yHeight + yDInfo.uvWidth * TWO_SLICES * yDInfo.uvHeight;
    if (destBufferSize == 0 || destBufferSize > PIXEL_MAP_MAX_RAM_SIZE) {
        IMAGE_LOGE("Invalid destination buffer size is 0!");
        return false;
    }
    *destBuffer = new(std::nothrow) uint8_t[destBufferSize];
    if (*destBuffer == nullptr) {
        IMAGE_LOGE("apply space for dest buffer failed!");
        return false;
    }
    bool result = NV21ToNV12SoftDecode(srcBuffer, yDInfo, destBuffer);
    if (!result) {
        delete[] (*destBuffer);
        IMAGE_LOGE("NV21 conversion to NV12 failed!");
    }
    return result;
}

bool ImageFormatConvertUtils::NV21ToRGBAF16(const uint8_t *srcBuffer, const YUVDataInfo &yDInfo, uint8_t **destBuffer,
                                            size_t &destBufferSize, [[maybe_unused]]ColorSpace colorSpace)
{
    if (srcBuffer == nullptr || destBuffer == nullptr || yDInfo.yWidth == 0 || yDInfo.yHeight == 0 ||
        yDInfo.uvWidth == 0 || yDInfo.uvHeight == 0) {
        return false;
    }
    destBufferSize = yDInfo.yWidth * yDInfo.yHeight * STRIDES_PER_PLANE;
    if (destBufferSize == 0 || destBufferSize > PIXEL_MAP_MAX_RAM_SIZE) {
        IMAGE_LOGE("Invalid destination buffer size is 0!");
        return false;
    }
    *destBuffer = new(std::nothrow) uint8_t[destBufferSize]();
    if (*destBuffer == nullptr) {
        IMAGE_LOGE("apply space for dest buffer failed!");
        return false;
    }
    AVPixelFormat srcFormat = findPixelFormat(PixelFormat::NV21);
    AVPixelFormat dstFormat = findPixelFormat(PixelFormat::RGBA_F16);
    SwsContext *swsContext = sws_getContext(yDInfo.yWidth, yDInfo.yHeight, srcFormat, yDInfo.yWidth,
        yDInfo.yHeight, dstFormat, SWS_BILINEAR, nullptr, nullptr, nullptr);
    if (swsContext == nullptr) {
        IMAGE_LOGE("Error to create SwsContext.");
        return false;
    }
    
    const uint8_t *srcSlice[] = {srcBuffer + yDInfo.yOffset, srcBuffer + yDInfo.uvOffset};
    const int srcStride[] = {static_cast<int>(yDInfo.yStride), static_cast<int>(yDInfo.uvStride)};
    uint8_t *dstSlice[] = {*destBuffer};
    const int dstStride[] = {static_cast<int>(yDInfo.yWidth * STRIDES_PER_PLANE)};
    int height = sws_scale(swsContext, srcSlice, srcStride, SRCSLICEY, yDInfo.yHeight, dstSlice, dstStride);
    sws_freeContext(swsContext);
    if (height == 0) {
        IMAGE_LOGE("Image pixel format conversion failed");
        delete[] (*destBuffer);
        return false;
    }
    return true;
}

bool ImageFormatConvertUtils::NV12ToNV21(const uint8_t *srcBuffer, const YUVDataInfo &yDInfo, uint8_t **destBuffer,
                                         size_t &destBufferSize, [[maybe_unused]]ColorSpace colorSpace)
{
    if (srcBuffer == nullptr || destBuffer == nullptr || yDInfo.yWidth == 0 || yDInfo.yHeight == 0 ||
        yDInfo.uvWidth == 0 || yDInfo.uvHeight == 0) {
        return false;
    }
    destBufferSize = yDInfo.yWidth * yDInfo.yHeight + yDInfo.uvWidth * EVEN_ODD_DIVISOR * yDInfo.uvHeight;
    if (destBufferSize == 0 || destBufferSize > PIXEL_MAP_MAX_RAM_SIZE) {
        IMAGE_LOGE("Invalid destination buffer size calculation!");
        return false;
    }

    *destBuffer = new(std::nothrow) uint8_t[destBufferSize]();
    if (*destBuffer == nullptr) {
        IMAGE_LOGE("apply space for dest buffer failed!");
        return false;
    }
    bool result = NV12ToNV21SoftDecode(srcBuffer, yDInfo, destBuffer);
    if (!result) {
        delete[] (*destBuffer);
        *destBuffer = nullptr;
        IMAGE_LOGE("NV12 conversion to NV21 failed!");
    }
    return result;
}

bool ImageFormatConvertUtils::BGRAToNV12(const uint8_t *srcBuffer, const RGBDataInfo &rgbInfo, uint8_t **destBuffer,
                                         size_t &destBufferSize, [[maybe_unused]]ColorSpace colorSpace)
{
    if (srcBuffer == nullptr || destBuffer == nullptr || rgbInfo.width < 0 || rgbInfo.height < 0) {
        return false;
    }
    size_t destPlaneSizeY = rgbInfo.width * rgbInfo.height;
    size_t srcPlaneSizeUV = ((rgbInfo.width + 1) / TWO_SLICES) * ((rgbInfo.height + 1) / TWO_SLICES);
    destBufferSize = static_cast<size_t>(destPlaneSizeY + srcPlaneSizeUV * TWO_SLICES);
    if (destBufferSize == 0 || destBufferSize > PIXEL_MAP_MAX_RAM_SIZE) {
        IMAGE_LOGE("Invalid destination buffer size calculation!");
        return false;
    }
    *destBuffer = new(std::nothrow) uint8_t[destBufferSize]();
    if (*destBuffer == nullptr) {
        IMAGE_LOGE("apply space for dest buffer failed!");
        return false;
    }
    bool bRet = BGRAToNV12SoftDecode(srcBuffer, rgbInfo, destBuffer);
    if (!bRet) {
        IMAGE_LOGE("BGRAToNV12 failed!");
        delete[] (*destBuffer);
        *destBuffer = nullptr;
        return false;
    }
    return true;
}

bool ImageFormatConvertUtils::RGB565ToNV12(const uint8_t *srcBuffer, const RGBDataInfo &rgbInfo,
                                           uint8_t **destBuffer, size_t &destBufferSize,
                                           [[maybe_unused]]ColorSpace colorSpace)
{
    if (srcBuffer == nullptr || destBuffer == nullptr || rgbInfo.width < 0 || rgbInfo.height < 0) {
        return false;
    }
    destBufferSize = static_cast<size_t>(rgbInfo.width * rgbInfo.height +
        ((rgbInfo.width + 1) / TWO_SLICES) * ((rgbInfo.height + 1) / TWO_SLICES) * TWO_SLICES);
    if (destBufferSize == 0 || destBufferSize > PIXEL_MAP_MAX_RAM_SIZE) {
        IMAGE_LOGE("Invalid destination buffer size calculation!");
        return false;
    }

    *destBuffer = new(std::nothrow) uint8_t[destBufferSize]();
    if (*destBuffer == nullptr) {
        IMAGE_LOGE("apply space for dest buffer failed!");
        return false;
    }
    bool bRet = RGB565ToNV12SoftDecode(srcBuffer, rgbInfo, destBuffer);
    if (!bRet) {
        IMAGE_LOGE("RGB565ToNV12 failed!");
        delete[] (*destBuffer);
        *destBuffer = nullptr;
        return false;
    }
    return true;
}

bool ImageFormatConvertUtils::RGB565ToNV21(const uint8_t *srcBuffer, const RGBDataInfo &rgbInfo,
    uint8_t **destBuffer, size_t &destBufferSize, [[maybe_unused]]ColorSpace colorSpace)
{
    if (srcBuffer == nullptr || destBuffer == nullptr || rgbInfo.width < 0 || rgbInfo.height < 0) {
        return false;
    }
    destBufferSize = static_cast<size_t>(rgbInfo.width * rgbInfo.height + ((rgbInfo.width + 1) / TWO_SLICES) *
        ((rgbInfo.height + 1) / TWO_SLICES) * TWO_SLICES);
    if (destBufferSize == 0 || destBufferSize > PIXEL_MAP_MAX_RAM_SIZE) {
        IMAGE_LOGE("Invalid destination buffer size calculation!");
        return false;
    }
    *destBuffer = new(std::nothrow) uint8_t[destBufferSize]();
    if (*destBuffer == nullptr) {
        IMAGE_LOGE("apply space for dest buffer failed!");
        return false;
    }
    bool bRet = RGB565ToNV21SoftDecode(srcBuffer, rgbInfo, destBuffer);
    if (!bRet) {
        IMAGE_LOGE("RGB565ToNV21 failed!");
        delete[] (*destBuffer);
        *destBuffer = nullptr;
        return false;
    }
    return true;
}

bool ImageFormatConvertUtils::NV12ToRGBAF16(const uint8_t *data, const YUVDataInfo &yDInfo, uint8_t **destBuffer,
                                            size_t &destBufferSize, [[maybe_unused]]ColorSpace colorSpace)
{
    if (data == nullptr || destBuffer == nullptr) {
        IMAGE_LOGE("Input buffer pointer data or destBuffer is null!");
        return false;
    }

    uint32_t frameSize = yDInfo.yWidth * yDInfo.yHeight;
    destBufferSize = frameSize * sizeof(uint64_t);
    if (destBufferSize == 0 || destBufferSize > PIXEL_MAP_MAX_RAM_SIZE) {
        IMAGE_LOGE("Invalid destination buffer size is 0!");
        return false;
    }
    *destBuffer = new(std::nothrow) uint8_t[destBufferSize]();
    if (*destBuffer == nullptr) {
        IMAGE_LOGE("Dynamically allocating memory for destination buffer failed!");
        return false;
    }

    if (!NV12ToRGBAF16SoftDecode(data, yDInfo, destBuffer)) {
        IMAGE_LOGE("NV12ToRGBAF16 failed!");
        delete[] (*destBuffer);
        *destBuffer = nullptr;
        return false;
    }

    return true;
}

bool ImageFormatConvertUtils::BGRAToNV21(const uint8_t *srcBuffer, const RGBDataInfo &rgbInfo, uint8_t **destBuffer,
                                         size_t &destBufferSize, [[maybe_unused]]ColorSpace colorSpace)
{
    if (srcBuffer == nullptr || destBuffer == nullptr || rgbInfo.width < 0 || rgbInfo.height < 0) {
        return false;
    }
    const int32_t destPlaneSizeY = rgbInfo.width * rgbInfo.height;
    const int32_t destPlaneSizeVU = ((rgbInfo.width + 1) / TWO_SLICES) * ((rgbInfo.height + 1) / TWO_SLICES);
    destBufferSize = static_cast<size_t>(destPlaneSizeY + destPlaneSizeVU * TWO_SLICES);
    if (destBufferSize == 0 || destBufferSize > PIXEL_MAP_MAX_RAM_SIZE) {
        IMAGE_LOGE("Invalid destination buffer size calculation!");
        return false;
    }
    *destBuffer = new(std::nothrow) uint8_t[destBufferSize]();
    if (*destBuffer == nullptr) {
        IMAGE_LOGE("apply space for dest buffer failed!");
        return false;
    }
    bool bRet = BGRAToNV21SoftDecode(srcBuffer, rgbInfo, destBuffer);
    if ((!bRet)) {
        IMAGE_LOGE("BGRAToNV21 failed!");
        delete[] (*destBuffer);
        *destBuffer = nullptr;
        return false;
    }
    return true;
}

bool ImageFormatConvertUtils::NV12ToRGBA(const uint8_t *data, const YUVDataInfo &yDInfo, uint8_t **destBuffer,
                                         size_t &destBufferSize, [[maybe_unused]]ColorSpace colorSpace)
{
    if (data == nullptr || destBuffer == nullptr) {
        IMAGE_LOGE("Input buffer pointer data or destBuffer is null!");
        return false;
    }

    destBufferSize = static_cast<size_t>(yDInfo.yWidth * yDInfo.yHeight * BYTES_PER_PIXEL_RGBA);
    if (destBufferSize == 0 || destBufferSize > PIXEL_MAP_MAX_RAM_SIZE) {
        IMAGE_LOGE("Invalid destination buffer size is 0!");
        return false;
    }

    *destBuffer = new(std::nothrow) uint8_t[destBufferSize]();
    if (*destBuffer == nullptr) {
        IMAGE_LOGE("Dynamically allocating memory for destination buffer failed!");
        return false;
    }

    bool bRet = NV12ToRGBASoftDecode(data, yDInfo, destBuffer);
    if (!bRet) {
        IMAGE_LOGE("NV12ToRGBA failed!");
        delete[] (*destBuffer);
        *destBuffer = nullptr;
        return false;
    }
    return true;
}

bool ImageFormatConvertUtils::NV12ToBGRA(const uint8_t *data, const YUVDataInfo &yDInfo, uint8_t **destBuffer,
                                         size_t &destBufferSize, [[maybe_unused]]ColorSpace colorSpace)
{
    if (data == nullptr || destBuffer == nullptr) {
        IMAGE_LOGE("Input buffer pointer data or destBuffer is null!");
        return false;
    }

    destBufferSize = static_cast<size_t>(yDInfo.yWidth * yDInfo.yHeight * BYTES_PER_PIXEL_BGRA);
    if (destBufferSize == 0 || destBufferSize > PIXEL_MAP_MAX_RAM_SIZE) {
        IMAGE_LOGE("Invalid destination buffer size is 0!");
        return false;
    }

    *destBuffer = new(std::nothrow) uint8_t[destBufferSize]();
    if (*destBuffer == nullptr) {
        IMAGE_LOGE("Dynamically allocating memory for destination buffer failed!");
        return false;
    }

    bool bRet = NV12ToBGRASoftDecode(data, yDInfo, destBuffer);
    if (!bRet) {
        IMAGE_LOGE("NV12ToBGRA failed!");
        delete[] (*destBuffer);
        *destBuffer = nullptr;
        return false;
    }
    return true;
}

bool ImageFormatConvertUtils::RGBAF16ToNV12(const uint8_t *srcBuffer, const RGBDataInfo &rgbInfo,
    uint8_t **destBuffer, size_t &destBufferSize, [[maybe_unused]]ColorSpace colorSpace)
{
    if (srcBuffer == nullptr || destBuffer == nullptr || rgbInfo.width < 0 || rgbInfo.height < 0) {
        return false;
    }
    uint32_t frameSize = rgbInfo.width * rgbInfo.height;
    destBufferSize = frameSize +
        (((rgbInfo.width + 1) / TWO_SLICES * (rgbInfo.height + 1) / TWO_SLICES) * TWO_SLICES);
    if (destBufferSize == 0 || destBufferSize > PIXEL_MAP_MAX_RAM_SIZE) {
        IMAGE_LOGE("Invalid destination buffer size calculation!");
        return false;
    }
    *destBuffer = new(std::nothrow) uint8_t[destBufferSize]();
    if (*destBuffer == nullptr) {
        IMAGE_LOGE("apply space for dest buffer failed!");
        return false;
    }

    AVPixelFormat srcFormat = findPixelFormat(PixelFormat::RGBA_F16);
    AVPixelFormat dstFormat = findPixelFormat(PixelFormat::NV12);
    SwsContext *swsContext = sws_getContext(rgbInfo.width, rgbInfo.height, srcFormat, rgbInfo.width,
        rgbInfo.height, dstFormat, SWS_BILINEAR, nullptr, nullptr, nullptr);
    if (swsContext == nullptr) {
        IMAGE_LOGE("Error to create SwsContext.");
        delete[] destBuffer;
        *destBuffer = nullptr;
        return false;
    }

    int widthEvent = (rgbInfo.width % EVEN_ODD_DIVISOR == 0) ? (rgbInfo.width) : (rgbInfo.width + 1);
    const uint8_t *srcSlice[] = {srcBuffer};
    const int srcStride[] = {static_cast<int>(rgbInfo.width * STRIDES_PER_PLANE)};
    uint8_t *dstSlice[] = {*destBuffer, *destBuffer + rgbInfo.width * rgbInfo.height};
    const int dstStride[] = {static_cast<int>(rgbInfo.width), static_cast<int>(widthEvent)};
    int height = sws_scale(swsContext, srcSlice, srcStride, SRCSLICEY, rgbInfo.height, dstSlice, dstStride);
    sws_freeContext(swsContext);
    if (height == 0) {
        IMAGE_LOGE("RGBAF16ToNV12 format conversion failed");
        delete[] destBuffer;
        *destBuffer = nullptr;
        return false;
    }
    return true;
}

bool ImageFormatConvertUtils::RGBAToNV12(const uint8_t *srcBuffer, const RGBDataInfo &rgbInfo, uint8_t **destBuffer,
                                         size_t &destBufferSize, [[maybe_unused]]ColorSpace colorSpace)
{
    if (srcBuffer == nullptr || destBuffer == nullptr || rgbInfo.width < 0 || rgbInfo.height < 0) {
        return false;
    }
    size_t destPlaneSizeY = rgbInfo.width * rgbInfo.height;
    size_t destPlaneSizeUV = ((rgbInfo.width + 1) / TWO_SLICES) * ((rgbInfo.height + 1) / TWO_SLICES);
    destBufferSize = destPlaneSizeY + destPlaneSizeUV * TWO_SLICES;
    if (destBufferSize == 0 || destBufferSize > PIXEL_MAP_MAX_RAM_SIZE) {
        IMAGE_LOGE("Invalid destination buffer size calculation!");
        return false;
    }
    *destBuffer = new(std::nothrow) uint8_t[destBufferSize]();
    if (*destBuffer == nullptr) {
        IMAGE_LOGE("apply space for dest buffer failed!");
        return false;
    }
    bool bRet = RGBAToNV12SoftDecode(srcBuffer, rgbInfo, destBuffer);
    if (!bRet) {
        IMAGE_LOGE("RGBAToNV12 failed!");
        delete[] (*destBuffer);
        *destBuffer = nullptr;
        return false;
    }
    return true;
}

bool ImageFormatConvertUtils::RGBAToNV21(const uint8_t *srcBuffer, const RGBDataInfo &rgbInfo, uint8_t **destBuffer,
                                         size_t &destBufferSize, [[maybe_unused]]ColorSpace colorSpace)
{
    if (srcBuffer == nullptr || destBuffer == nullptr || rgbInfo.width < 0 || rgbInfo.height < 0) {
        return false;
    }
    size_t destPlaneSizeY = rgbInfo.width * rgbInfo.height;
    size_t destPlaneSizeVU = ((rgbInfo.width + 1) / TWO_SLICES) * ((rgbInfo.height + 1) / TWO_SLICES);
    destBufferSize = static_cast<size_t>(destPlaneSizeY + destPlaneSizeVU * TWO_SLICES);
    if (destBufferSize == 0 || destBufferSize > PIXEL_MAP_MAX_RAM_SIZE) {
        IMAGE_LOGE("Invalid destination buffer size calculation!");
        return false;
    }
    *destBuffer = new(std::nothrow) uint8_t[destBufferSize]();
    if (*destBuffer == nullptr) {
        IMAGE_LOGE("apply space for dest buffer failed!");
        return false;
    }

    bool bRet = RGBAToNV21SoftDecode(srcBuffer, rgbInfo, destBuffer);
    if (!bRet) {
        IMAGE_LOGE("RGBAToNV21 failed!");
        delete[] (*destBuffer);
        *destBuffer = nullptr;
        return false;
    }
    return true;
}

bool ImageFormatConvertUtils::RGBToNV21(const uint8_t *srcBuffer, const RGBDataInfo &rgbInfo, uint8_t **destBuffer,
                                        size_t &destBufferSize, [[maybe_unused]]ColorSpace colorSpace)
{
    if (srcBuffer == nullptr || destBuffer == nullptr || rgbInfo.width < 0 || rgbInfo.height < 0) {
        return false;
    }
    destBufferSize = static_cast<size_t>
        (rgbInfo.width * rgbInfo.height + ((rgbInfo.width + 1) / TWO_SLICES *
        ((rgbInfo.height + 1) / TWO_SLICES) * TWO_SLICES));
    if (destBufferSize == 0 || destBufferSize > PIXEL_MAP_MAX_RAM_SIZE) {
        IMAGE_LOGE("Invalid destination buffer size calculation!");
        return false;
    }
    *destBuffer = new(std::nothrow) uint8_t[destBufferSize]();
    if (*destBuffer == nullptr) {
        IMAGE_LOGE("apply space for dest buffer failed!");
        return false;
    }
    bool bRet = RGBToNV21SoftDecode(srcBuffer, rgbInfo, destBuffer);
    if (!bRet) {
        IMAGE_LOGE("RGBToNV21 failed!");
        delete[] (*destBuffer);
        *destBuffer = nullptr;
        return false;
    }
    return true;
}

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
    int height = sws_scale(ctx, srcSlice, srcStride, SRCSLICEY, yDInfo.yHeight, destBuffer, dstStride);
    sws_freeContext(ctx);

    if (height == 0) {
        IMAGE_LOGE("Image pixel format conversion failed");
        return false;
    }
    return true;
}

bool ImageFormatConvertUtils::NV12ToRGB(const uint8_t *data, const YUVDataInfo &yDInfo, uint8_t **destBuffer,
                                        size_t &destBufferSize, [[maybe_unused]]ColorSpace colorSpace)
{
    if (data == nullptr || destBuffer == nullptr) {
        IMAGE_LOGE("Input buffer pointer data or destBuffer is null!");
        return false;
    }

    destBufferSize = static_cast<size_t>(yDInfo.yWidth * yDInfo.yHeight * BYTES_PER_PIXEL_RGB);
    if (destBufferSize == 0 || destBufferSize > PIXEL_MAP_MAX_RAM_SIZE) {
        IMAGE_LOGE("Invalid destination buffer size is 0!");
        return false;
    }

    (*destBuffer) = new(std::nothrow) uint8_t[destBufferSize]();
    if (*destBuffer == nullptr) {
        IMAGE_LOGE("Dynamically allocating memory for destination buffer failed!");
        return false;
    }

    bool bRet = NV12ToRGBManual(data, yDInfo, destBuffer);
    if (!bRet) {
        IMAGE_LOGE("NV12ToRGB failed!");
        delete[] (*destBuffer);
        *destBuffer = nullptr;
        return false;
    }

    return true;
}

bool ImageFormatConvertUtils::NV21ToRGB(const uint8_t *data, const YUVDataInfo &yDInfo, uint8_t **destBuffer,
                                        size_t &destBufferSize, [[maybe_unused]]ColorSpace colorSpace)
{
    if (data == nullptr || destBuffer == nullptr || yDInfo.yWidth == 0 || yDInfo.yHeight == 0 ||
        yDInfo.uvWidth == 0 || yDInfo.uvHeight == 0) {
        return false;
    }

    destBufferSize = static_cast<size_t>(yDInfo.yWidth * yDInfo.yHeight * BYTES_PER_PIXEL_RGB);
    if (destBufferSize == 0 || destBufferSize > PIXEL_MAP_MAX_RAM_SIZE) {
        IMAGE_LOGE("Invalid destination buffer size is 0!");
        return false;
    }
    *destBuffer = new(std::nothrow) uint8_t[destBufferSize]();
    if (*destBuffer == nullptr) {
        IMAGE_LOGE("apply space for dest buffer failed!");
        return false;
    }

    AVPixelFormat srcFormat = findPixelFormat(PixelFormat::NV21);
    AVPixelFormat dstFormat = findPixelFormat(PixelFormat::RGB_888);
    SwsContext *swsContext = sws_getContext(yDInfo.yWidth, yDInfo.yHeight, srcFormat, yDInfo.yWidth,
                                             yDInfo.yHeight, dstFormat, SWS_BILINEAR, nullptr, nullptr, nullptr);
    if (swsContext == nullptr) {
        delete[] destBuffer;
        *destBuffer = nullptr;
        IMAGE_LOGE("Error to create SwsContext.");
        return false;
    }

    const uint8_t *srcSlice[] = { data + yDInfo.yOffset, data + yDInfo.uvOffset };
    const int srcStride[] = { static_cast<int>(yDInfo.yStride), static_cast<int>(yDInfo.uvStride) };
    const int dstStride[] = {static_cast<int>(yDInfo.yWidth * BYTES_PER_PIXEL_RGB)};
    int height = sws_scale(swsContext, srcSlice, srcStride, SRCSLICEY, yDInfo.yHeight,
        destBuffer, dstStride);
    sws_freeContext(swsContext);
    if (height == 0) {
        IMAGE_LOGE("Image pixel format conversion failed");
        delete[] destBuffer;
        *destBuffer = nullptr;
        return false;
    }
    return true;
}

bool ImageFormatConvertUtils::NV21ToRGBA(const uint8_t *srcBuffer, const YUVDataInfo &yDInfo, uint8_t **destBuffer,
                                         size_t &destBufferSize, ColorSpace colorSpace)
{
    if (srcBuffer == nullptr || destBuffer == nullptr || yDInfo.yWidth == 0 || yDInfo.yHeight == 0 ||
        yDInfo.uvWidth == 0 || yDInfo.uvHeight == 0) {
        return false;
    }

    destBufferSize = static_cast<size_t>(yDInfo.yWidth * yDInfo.yHeight * BYTES_PER_PIXEL_RGBA);
    if (destBufferSize == 0 || destBufferSize > PIXEL_MAP_MAX_RAM_SIZE) {
        IMAGE_LOGE("Invalid destination buffer size is 0!");
        return false;
    }
    *destBuffer = new(std::nothrow) uint8_t[destBufferSize]();
    if (*destBuffer == nullptr) {
        IMAGE_LOGE("Apply space for dest buffer failed!");
        return false;
    }
    bool bRet = NV21ToRGBASoftDecode(srcBuffer, yDInfo, destBuffer);
    if (!bRet) {
        IMAGE_LOGE("NV21ToRGBA failed!");
        delete[] destBuffer;
        *destBuffer = nullptr;
        return bRet;
    }
    return true;
}

bool ImageFormatConvertUtils::NV21ToBGRA(const uint8_t *srcBuffer, const YUVDataInfo &yDInfo, uint8_t **destBuffer,
                                         size_t &destBufferSize, [[maybe_unused]]ColorSpace colorSpace)
{
    if (srcBuffer == nullptr || destBuffer == nullptr || yDInfo.yWidth == 0 || yDInfo.yHeight == 0 ||
        yDInfo.uvWidth == 0 || yDInfo.uvHeight == 0) {
        return false;
    }

    destBufferSize = static_cast<size_t>(yDInfo.yWidth * yDInfo.yHeight * BYTES_PER_PIXEL_BGRA);
    if (destBufferSize == 0 || destBufferSize > PIXEL_MAP_MAX_RAM_SIZE) {
        IMAGE_LOGE("Invalid destination buffer size is 0!");
        return false;
    }
    *destBuffer = new(std::nothrow) uint8_t[destBufferSize]();
    if (*destBuffer == nullptr) {
        IMAGE_LOGE("apply space for dest buffer failed!");
        return false;
    }

    bool bRet = NV21ToBGRASoftDecode(srcBuffer, yDInfo, destBuffer);
    if (!bRet) {
        IMAGE_LOGE("NV21ToBGRA failed!");
        delete[] destBuffer;
        *destBuffer = nullptr;
        return bRet;
    }
    return true;
}

bool ImageFormatConvertUtils::NV21ToRGB565(const uint8_t *srcBuffer, const YUVDataInfo &yDInfo, uint8_t **destBuffer,
                                           size_t &destBufferSize, ColorSpace colorSpace)
{
    if (srcBuffer == nullptr || destBuffer == nullptr || yDInfo.yWidth == 0 || yDInfo.yHeight == 0 ||
        yDInfo.uvWidth == 0 || yDInfo.uvHeight == 0) {
        return false;
    }

    destBufferSize = static_cast<size_t>(yDInfo.yWidth * yDInfo.yHeight * BYTES_PER_PIXEL_RGB565);
    if (destBufferSize == 0 || destBufferSize > PIXEL_MAP_MAX_RAM_SIZE) {
        IMAGE_LOGE("Invalid destination buffer size is 0!");
        return false;
    }
    *destBuffer = new(std::nothrow) uint8_t[destBufferSize]();
    if (*destBuffer == nullptr) {
        IMAGE_LOGE("apply space for dest buffer failed!");
        return false;
    }

    bool bRet = NV21ToRGB565SoftDecode(srcBuffer, yDInfo, destBuffer);
    if (!bRet) {
        IMAGE_LOGE("NV21ToRGB565 failed!");
        delete[] destBuffer;
        *destBuffer = nullptr;
        return bRet;
    }
    return true;
}

bool ImageFormatConvertUtils::RGBToNV12(const uint8_t *srcBuffer, const RGBDataInfo &rgbInfo, uint8_t **destBuffer,
                                        size_t &destBufferSize, [[maybe_unused]]ColorSpace colorSpace)
{
    if (srcBuffer == nullptr || destBuffer == nullptr || rgbInfo.width < 0 || rgbInfo.height < 0) {
        return false;
    }
    destBufferSize = static_cast<size_t>(rgbInfo.width * rgbInfo.height + (rgbInfo.width + 1) / TWO_SLICES *
        (rgbInfo.height + 1) / TWO_SLICES * TWO_SLICES);
    if (destBufferSize == 0 || destBufferSize > PIXEL_MAP_MAX_RAM_SIZE) {
        IMAGE_LOGE("Invalid destination buffer size calculation!");
        return false;
    }
    (*destBuffer) = new(std::nothrow) uint8_t[destBufferSize]();
    if (*destBuffer == nullptr) {
        IMAGE_LOGE("apply space for dest buffer failed!");
        return false;
    }
    bool bRet = RGBToNV12SoftDecode(srcBuffer, rgbInfo, destBuffer);
    if (!bRet) {
        IMAGE_LOGE("RGBToNV12 failed!");
        delete[] destBuffer;
        *destBuffer = nullptr;
        return bRet;
    }
    return true;
}
} // namespace Media
} // namespace OHOS