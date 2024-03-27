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

#include "pixel_yuv_utils.h"

#include "image_log.h"
#include "ios"
#include "istream"
#include "image_trace.h"
#include "image_system_properties.h"
#include "media_errors.h"
#include "memory_manager.h"
#include "securec.h"
#if !defined(IOS_PLATFORM) && !defined(A_PLATFORM)
#include "surface_buffer.h"
#endif

#undef LOG_DOMAIN
#define LOG_DOMAIN LOG_TAG_DOMAIN_ID_IMAGE

#undef LOG_TAG
#define LOG_TAG "PixelYuvUtils"

namespace OHOS {
namespace Media {

static const uint8_t NUM_2 = 2;
static const uint8_t NUM_3 = 3;
static const uint8_t NUM_4 = 4;
#ifndef LIBYUV_ENABLE
static const int32_t degrees360 = 360;
constexpr int32_t INT_16 = 16;
constexpr int32_t INT_128 = 128;
constexpr int32_t INT_219 = 219;
constexpr int32_t INT_224 = 224;
constexpr int32_t INT_255 = 255;
const float YUV_TO_RGB_PARAM_1 = 1.402;
const float YUV_TO_RGB_PARAM_2 = 0.344136;
const float YUV_TO_RGB_PARAM_3 = 0.714136;
const float YUV_TO_RGB_PARAM_4 = 1.772;
const float RGB_TO_YUV_PARAM_1 = 0.2627;
const float RGB_TO_YUV_PARAM_2 = 0.678;
const float RGB_TO_YUV_PARAM_3 = 0.0593;
const float RGB_TO_YUV_PARAM_4 = 0.5;
const float RGB_TO_YUV_PARAM_5 = 0.9407;
const float RGB_TO_YUV_PARAM_6 = 0.7373;
// Define pixel yuv malloc max size 600MB
constexpr int32_t PIXEL_YUV_MAX_RAM_SIZE = 600 * 1024 * 1024;
#endif
static const int32_t degrees90 = 90;
static const int32_t degrees180 = 180;
static const int32_t degrees270 = 270;
constexpr uint8_t Y_SHIFT = 16;
constexpr uint8_t U_SHIFT = 8;
constexpr uint8_t V_SHIFT = 0;
constexpr uint8_t YUV_MASK = 0xFF;
constexpr uint8_t Y_DEFAULT = 0x10;
constexpr uint8_t UV_DEFAULT = 0x80;
constexpr uint8_t TRANSPOSE_CLOCK = 1;
constexpr uint8_t TRANSPOSE_CCLOCK = 2;


static const std::map<PixelFormat, AVPixelFormat> FFMPEG_PIXEL_FORMAT_MAP = {
    { PixelFormat::UNKNOWN, AVPixelFormat::AV_PIX_FMT_NONE },
    { PixelFormat::NV21, AVPixelFormat::AV_PIX_FMT_NV21 },
    { PixelFormat::NV12, AVPixelFormat::AV_PIX_FMT_NV12 },
};

int32_t PixelYuvUtils::YuvConvertOption(const AntiAliasingOption &option)
{
    switch (option) {
        case AntiAliasingOption::NONE:
            return SWS_POINT;
        case AntiAliasingOption::LOW:
            return SWS_BILINEAR;
        case AntiAliasingOption::MEDIUM:
            return SWS_BICUBIC;
        case AntiAliasingOption::HIGH:
            return SWS_AREA;
        case AntiAliasingOption::FAST_BILINEAER:
            return SWS_FAST_BILINEAR;
        case AntiAliasingOption::BICUBLIN:
            return SWS_BICUBLIN;
        case AntiAliasingOption::GAUSS:
            return SWS_GAUSS;
        case AntiAliasingOption::SINC:
            return SWS_SINC;
        case AntiAliasingOption::LANCZOS:
            return SWS_LANCZOS;
        case AntiAliasingOption::SPLINE:
            return SWS_SPLINE;
        default:
            return SWS_POINT;
    }
}

static int32_t GetYSize(int32_t width, int32_t height)
{
    return width * height;
}

static int32_t GetVOffset(int32_t width, int32_t height)
{
    return width * height + ((width + 1) / NUM_2) * ((height + 1) / NUM_2);
}

// The stride of u and v are the same, Yuv420P u, v single planer
static int32_t GetUStride(int32_t width)
{
    return (width + 1) / NUM_2;
}

static int32_t GetUVHeight(int32_t height)
{
    return (height + 1) / NUM_2;
}

// Yuv420SP, u、 v blend planer
static int32_t GetUVStride(int32_t width)
{
    return (width + 1) / NUM_2 * NUM_2;
}

static uint32_t GetImageSize(int32_t width, int32_t height)
{
    return width * height + ((width + 1) / NUM_2) * ((height + 1) / NUM_2) * NUM_2;
}

static void WriteDataI420Convert(const uint8_t *srcPixels, const Size &size, uint8_t *dstPixels,
    Position dstPos, const Size &dstSize)
{
    dstPos.y = GetUVStride(dstPos.y);
    const uint8_t *srcU = srcPixels + GetYSize(size.width, size.height);
    const uint8_t *srcV = srcPixels + GetVOffset(size.width, size.height);
    uint8_t *dstU = dstPixels + GetYSize(dstSize.width, dstSize.height);
    uint8_t *dstV = dstPixels + GetVOffset(dstSize.width, dstSize.height);
    for (int32_t i = 0; i < size.height; i++) {
        if (memcpy_s(dstPixels + (dstPos.y + i) * dstSize.width + dstPos.x, size.width,
            srcPixels + i * size.width, size.width) != EOK) {
            IMAGE_LOGE("WriteDataI420Convert memcpy yplane failed");
            return;
        }
    }
    for (int32_t i = 0; i < GetUVHeight(size.height); i++) {
        if (memcpy_s(dstU + (dstPos.y / NUM_2 + i) * GetUStride(dstSize.width) + dstPos.x / NUM_2,
            GetUStride(size.width), srcU + i * GetUStride(size.width), GetUStride(size.width)) != 0 ||
            memcpy_s(dstV + (dstPos.y / NUM_2 + i) * GetUStride(dstSize.width) + dstPos.x / NUM_2,
            GetUStride(size.width), srcV + i * GetUStride(size.width), GetUStride(size.width)) != 0) {
            IMAGE_LOGE("WriteDataI420Convert memcpy uplane or vplane failed");
            return;
        }
    }
}

static void SwapUV(uint8_t *dstPixels, int32_t width, int32_t heigth)
{
    uint8_t *uvData = dstPixels + GetYSize(width, heigth);
    for (int32_t i = 0; i < GetUVStride(width) * GetUVHeight(heigth); i += NUM_2) {
        std::swap(uvData[i], uvData[i + 1]);
    }
}

static void WriteDataNV12Convert(uint8_t *srcPixels, const Size &size, uint8_t *dstPixels,
    Position dstPos, const Size &dstSize)
{
    if (size.width == size.height) {
        SwapUV(srcPixels, size.width, size.height);
    }
    dstPos.y = GetUVStride(dstPos.y);
    for (int i = 0; i < size.height; i++) {
        if (memcpy_s(dstPixels + (dstPos.y + i) * dstSize.width + dstPos.x, size.width,
            srcPixels + i * size.width, size.width) != 0) {
            IMAGE_LOGE("WriteDataNV12Convert memcpy yplane failed");
            return;
        }
    }
    for (int i = 0; i < GetUVHeight(size.height); ++i) {
        if (memcpy_s(dstPixels + ((dstPos.y) / NUM_2 + i) * GetUVStride(dstSize.width) + dstPos.x +
            GetYSize(dstSize.width, dstSize.height), GetUVStride(size.width),
            srcPixels + GetYSize(size.width, size.height) + i * GetUVStride(size.width),
            GetUVStride(size.width)) != 0) {
            IMAGE_LOGE("WriteDataNV12Convert memcpy uplane or vplane failed");
            return;
        }
    }
}

bool PixelYuvUtils::WriteYuvConvert(const void *srcPixels, const ImageInfo &srcInfo, void *dstPixels,
    const Position &dstPos, const ImageInfo &dstInfo)
{
    if (srcPixels == nullptr || dstPixels == nullptr) {
        IMAGE_LOGE("src or dst pixels invalid.");
        return false;
    }
    switch (dstInfo.pixelFormat) {
        case PixelFormat::YU12:
            WriteDataI420Convert((uint8_t *)srcPixels, srcInfo.size, static_cast<uint8_t *>(dstPixels), dstPos,
                dstInfo.size);
            return true;
        case PixelFormat::YV12:
            WriteDataI420Convert((uint8_t *)srcPixels, srcInfo.size, static_cast<uint8_t *>(dstPixels), dstPos,
                dstInfo.size);
            return true;
        case PixelFormat::NV21:
            WriteDataNV12Convert((uint8_t *)srcPixels, srcInfo.size, static_cast<uint8_t *>(dstPixels), dstPos,
                dstInfo.size);
            return true;
        case PixelFormat::NV12:
            WriteDataNV12Convert((uint8_t *)srcPixels, srcInfo.size, static_cast<uint8_t *>(dstPixels), dstPos,
                dstInfo.size);
            return true;
        default:
            return false;
    }
}

AVPixelFormat PixelYuvUtils::ConvertFormat(const PixelFormat &pixelFormat)
{
    auto formatSearch = FFMPEG_PIXEL_FORMAT_MAP.find(pixelFormat);
    return (formatSearch != FFMPEG_PIXEL_FORMAT_MAP.end()) ? formatSearch->second : AVPixelFormat::AV_PIX_FMT_NONE;
}

static void SetAVFrameInfo(AVFrame* frame, YuvImageInfo &info)
{
    frame->width = info.width;
    frame->height = info.height;
    frame->format = info.format;
}

static void CleanUpFilterGraph(AVFilterGraph **filterGraph, AVFrame **srcFrame, AVFrame **dstFrame)
{
    if (dstFrame && *dstFrame) {
        av_frame_free(dstFrame);
        *dstFrame = NULL;
    }

    // Free the filter graph
    if (filterGraph && *filterGraph) {
        avfilter_graph_free(filterGraph);
        *filterGraph = NULL;
    }

    // Free the source frame
    if (srcFrame && *srcFrame) {
        av_frame_free(srcFrame);
        *srcFrame = NULL;
    }
}

static bool ProcessFilterGraph(AVFilterGraph *filterGraph, AVFilterContext *bufferSrcCtx,
    AVFilterContext *bufferSinkCtx, AVFrame *srcFrame, AVFrame *dstFrame)
{
    // Configure the filtergraph with the previously set options
    if (avfilter_graph_config(filterGraph, nullptr) < 0) {
        IMAGE_LOGE("avfilter_graph_config failed");
        return false;
    }

    // Send the source frame to the filtergraph
    if (av_buffersrc_add_frame_flags(bufferSrcCtx, srcFrame, AV_BUFFERSRC_FLAG_KEEP_REF) < 0) {
        IMAGE_LOGE("av_buffersrc_add_frame_flags failed");
        return false;
    }

    // Fetch the filtered frame from the buffersink
    if (av_buffersink_get_frame(bufferSinkCtx, dstFrame) < 0) {
        IMAGE_LOGE("av_buffersink_get_frame failed");
        return false;
    }
    return true;
}

static bool CreateBufferSource(AVFilterGraph **filterGraph, AVFilterContext **bufferSrcCtx,
    YuvImageInfo &srcInfo)
{
    const char *bufferSrcArgs = av_asprintf("video_size=%dx%d:pix_fmt=%d:time_base=1/1:pixel_aspect=1/1",
        srcInfo.width, srcInfo.height, srcInfo.format);
    if (!bufferSrcArgs) {
        IMAGE_LOGE("bufferSrcArgs is null");
        return false;
    }

    if (avfilter_graph_create_filter(bufferSrcCtx, avfilter_get_by_name("buffer"), "in",
        bufferSrcArgs, nullptr, *filterGraph) < 0) {
        IMAGE_LOGE("create bufferSrcCtx filter falied");
        av_free(reinterpret_cast<void *>(const_cast<char *>(bufferSrcArgs)));
        return false;
    }

    av_free(reinterpret_cast<void *>(const_cast<char *>(bufferSrcArgs)));
    return true;
}

static bool CreateBufferSinkFilter(AVFilterGraph **filterGraph, AVFilterContext **bufferSinkCtx)
{
    int32_t ret = avfilter_graph_create_filter(bufferSinkCtx, avfilter_get_by_name("buffersink"), "out",
        nullptr, nullptr, *filterGraph);
    if (ret < 0) {
        IMAGE_LOGE("create bufferSinkCtx filter falied");
        return false;
    }
    return true;
}

static bool CreateCropFilter(AVFilterGraph **filterGraph, AVFilterContext **cropCtx,
    const Rect &rect)
{
    const char *cropArgs = av_asprintf("x=%d:y=%d:out_w=%d:out_h=%d", rect.left, rect.top, rect.width, rect.height);
    if (!cropArgs) {
        IMAGE_LOGE("YuvCrop cropArgs is null");
        return false;
    }

    int32_t ret = avfilter_graph_create_filter(cropCtx, avfilter_get_by_name("crop"), "crop",
        cropArgs, nullptr, *filterGraph);
    if (ret < 0) {
        IMAGE_LOGE("create crop filter failed, ret = %{public}d", ret);
        av_free(reinterpret_cast<void *>(const_cast<char *>(cropArgs)));
        return false;
    }
    av_free(reinterpret_cast<void *>(const_cast<char *>(cropArgs)));
    return true;
}

static bool CropUpDataDstdata(uint8_t *dstData, AVFrame *dstFrame, const Rect &rect)
{
    dstFrame->width = rect.width;
    dstFrame->height = rect.height;

    int32_t dstSize = av_image_get_buffer_size(static_cast<AVPixelFormat>(dstFrame->format),
        dstFrame->width, dstFrame->height, 1);
    if (dstSize < 0) {
        IMAGE_LOGE("YuvCrop get size failed");
        return false;
    }

    // Copy the output frame data to the destination buffer
    if (av_image_copy_to_buffer(dstData, dstSize, dstFrame->data, dstFrame->linesize,
        static_cast<AVPixelFormat>(dstFrame->format), dstFrame->width, dstFrame->height, 1) < 0) {
        return false;
    }

    return true;
}

bool PixelYuvUtils::YuvCrop(uint8_t *srcData, YuvImageInfo &srcInfo, uint8_t *dstData, const Rect &rect)
{
    AVFrame *srcFrame = av_frame_alloc();
    AVFrame *dstFrame = av_frame_alloc();
    if (srcFrame == nullptr || dstFrame == nullptr) {
        IMAGE_LOGE("YuvCrop av_frame_alloc failed!");
        return false;
    }
    SetAVFrameInfo(srcFrame, srcInfo);
    av_image_fill_arrays(srcFrame->data, srcFrame->linesize, srcData, srcInfo.format, srcInfo.width, srcInfo.height, 1);
    AVFilterGraph *filterGraph = avfilter_graph_alloc();
    if (!filterGraph) {
        CleanUpFilterGraph(&filterGraph, &srcFrame, &dstFrame);
        return false;
    }
    // Create buffer source filter
    AVFilterContext *bufferSrcCtx = nullptr;
    if (!CreateBufferSource(&filterGraph, &bufferSrcCtx, srcInfo)) {
        CleanUpFilterGraph(&filterGraph, &srcFrame, &dstFrame);
        return false;
    }
    // Create crop filter
    AVFilterContext *cropCtx = nullptr;
    if (!CreateCropFilter(&filterGraph, &cropCtx, rect)) {
        CleanUpFilterGraph(&filterGraph, &srcFrame, &dstFrame);
        return false;
    }
    // Create buffer sink filter
    AVFilterContext *bufferSinkCtx = nullptr;
    if (!CreateBufferSinkFilter(&filterGraph, &bufferSinkCtx)) {
        CleanUpFilterGraph(&filterGraph, &srcFrame, &dstFrame);
        return false;
    }
    // Link filters
    if (avfilter_link(bufferSrcCtx, 0, cropCtx, 0) < 0 || avfilter_link(cropCtx, 0, bufferSinkCtx, 0) < 0) {
        CleanUpFilterGraph(&filterGraph, &srcFrame, &dstFrame);
        return false;
    }
    if (!ProcessFilterGraph(filterGraph, bufferSrcCtx, bufferSinkCtx, srcFrame, dstFrame)) {
        CleanUpFilterGraph(&filterGraph, &srcFrame, &dstFrame);
        return false;
    }
    if (!CropUpDataDstdata(dstData, dstFrame, rect)) {
        CleanUpFilterGraph(&filterGraph, &srcFrame, &dstFrame);
        return false;
    }
    // Clean up
    CleanUpFilterGraph(&filterGraph, &srcFrame, &dstFrame);
    return true;
}

int32_t PixelYuvUtils::YuvScale(const uint8_t *srcPixels, YuvImageInfo &srcInfo,
    uint8_t *dstPixels, YuvImageInfo &dstInfo, int32_t module)
{
    int ret = 0;
    AVFrame *inputFrame = nullptr;
    AVFrame *outputFrame = nullptr;
    struct SwsContext *ctx = nullptr;

    if (srcInfo.format == AVPixelFormat::AV_PIX_FMT_NONE || dstInfo.format == AVPixelFormat::AV_PIX_FMT_NONE) {
        IMAGE_LOGE("unsupport src/dst pixel format!");
        return -1;
    }
    if (srcInfo.width <= 0 || srcInfo.height <= 0 || dstInfo.width <= 0 || dstInfo.height <= 0) {
        IMAGE_LOGE("src/dst width/height error!");
        return -1;
    }

    inputFrame = av_frame_alloc();
    outputFrame = av_frame_alloc();
    if (inputFrame != nullptr && outputFrame != nullptr) {
        ctx = sws_getContext(srcInfo.width, srcInfo.height, srcInfo.format,
                             dstInfo.width, dstInfo.height, dstInfo.format,
                             module, nullptr, nullptr, nullptr);
        if (ctx != nullptr) {
            av_image_fill_arrays(inputFrame->data, inputFrame->linesize, srcPixels,
                srcInfo.format, srcInfo.width, srcInfo.height, 1);
            av_image_fill_arrays(outputFrame->data, outputFrame->linesize, dstPixels,
                dstInfo.format, dstInfo.width, dstInfo.height, 1);

            sws_scale(ctx, (uint8_t const **)inputFrame->data, inputFrame->linesize, 0, srcInfo.height,
                outputFrame->data, outputFrame->linesize);
        } else {
            IMAGE_LOGE("FFMpeg: sws_getContext failed!");
            ret = -1;
        }
    } else {
        IMAGE_LOGE("FFMpeg: av_frame_alloc failed!");
        ret = -1;
    }

    av_frame_free(&inputFrame);
    av_frame_free(&outputFrame);
    sws_freeContext(ctx);

    return ret;
}

static bool CreateRotateFilter(AVFilterGraph **filterGraph, AVFilterContext **transposeCtx,
    int32_t rotateNum)
{
    const char *rotateArgs = av_asprintf("%d", rotateNum);
    if (!rotateArgs) {
        IMAGE_LOGE("rotateArgs is null");
        return false;
    }

    int ret = avfilter_graph_create_filter(transposeCtx, avfilter_get_by_name("transpose"),
        "rotate", rotateArgs, nullptr, *filterGraph);
    if (ret < 0) {
        IMAGE_LOGE("create transpose filter failed, ret = %{public}d", ret);
        av_free(reinterpret_cast<void *>(const_cast<char *>(rotateArgs)));
        return false;
    }
    av_free(reinterpret_cast<void *>(const_cast<char *>(rotateArgs)));
    return true;
}

static bool RoatateUpDataDstdata(YuvImageInfo &srcInfo, YuvImageInfo &dstInfo, uint8_t *dstData,
    AVFrame *srcFrame, AVFrame *dstFrame)
{
    dstFrame->width = srcFrame->height;
    dstFrame->height = srcFrame->width;
    dstInfo.width = srcInfo.height;
    dstInfo.height = srcInfo.width;

    int32_t dstSize = av_image_get_buffer_size(static_cast<AVPixelFormat>(dstFrame->format),
        dstFrame->width, dstFrame->height, 1);
    if (dstSize < 0) {
        IMAGE_LOGE("RoatateUpDataDstdata get size failed");
        return false;
    }

    // Copy the output frame data to the destination buffer
    if (av_image_copy_to_buffer(dstData, dstSize, dstFrame->data, dstFrame->linesize,
        static_cast<AVPixelFormat>(dstFrame->format), dstFrame->width, dstFrame->height, 1) < 0) {
        IMAGE_LOGE("RoatateUpDataDstdata copy data failed");
        return false;
    }

    return true;
}

static bool Rotate(uint8_t *srcData, YuvImageInfo &srcInfo, uint8_t *dstData,
    YuvImageInfo &dstInfo, int32_t rotateNum)
{
    AVFrame *srcFrame = av_frame_alloc();
    AVFrame *dstFrame = av_frame_alloc();
    if (srcFrame == nullptr || dstFrame == nullptr) {
        IMAGE_LOGE("Rotate av_frame_alloc failed");
        return false;
    }

    SetAVFrameInfo(srcFrame, srcInfo);
    av_image_fill_arrays(srcFrame->data, srcFrame->linesize, srcData, srcInfo.format, srcInfo.width, srcInfo.height, 1);

    AVFilterGraph *filterGraph = avfilter_graph_alloc();
    if (!filterGraph) {
        CleanUpFilterGraph(&filterGraph, &srcFrame, &dstFrame);
        return false;
    }
    // Create buffer source filter
    AVFilterContext *bufferSrcCtx = nullptr;
    if (!CreateBufferSource(&filterGraph, &bufferSrcCtx, srcInfo)) {
        CleanUpFilterGraph(&filterGraph, &srcFrame, &dstFrame);
        return false;
    }
    // Create transpose filter
    AVFilterContext *transposeCtx = nullptr;
    if (!CreateRotateFilter(&filterGraph, &transposeCtx, rotateNum)) {
        CleanUpFilterGraph(&filterGraph, &srcFrame, &dstFrame);
        return false;
    }
    // Create buffer sink filter
    AVFilterContext *bufferSinkCtx = nullptr;
    if (!CreateBufferSinkFilter(&filterGraph, &bufferSinkCtx)) {
        CleanUpFilterGraph(&filterGraph, &srcFrame, &dstFrame);
        return false;
    }
    // Link filters together
    if (avfilter_link(bufferSrcCtx, 0, transposeCtx, 0) < 0 || avfilter_link(transposeCtx, 0, bufferSinkCtx, 0) < 0) {
        CleanUpFilterGraph(&filterGraph, &srcFrame, &dstFrame);
        return false;
    }
    if (!ProcessFilterGraph(filterGraph, bufferSrcCtx, bufferSinkCtx, srcFrame, dstFrame)) {
        CleanUpFilterGraph(&filterGraph, &srcFrame, &dstFrame);
        return false;
    }
    if (!RoatateUpDataDstdata(srcInfo, dstInfo, dstData, srcFrame, dstFrame)) {
        CleanUpFilterGraph(&filterGraph, &srcFrame, &dstFrame);
        return false;
    }
    // Clean up
    CleanUpFilterGraph(&filterGraph, &srcFrame, &dstFrame);
    return true;
}

static bool CreateFilpFilter(AVFilterGraph **filterGraph, AVFilterContext **flipCtx, bool xAxis)
{
    const char *flipType = xAxis ? "hflip" : "vflip";
    int32_t ret = avfilter_graph_create_filter(flipCtx, avfilter_get_by_name(flipType),
        "flip", NULL, NULL, *filterGraph);
    if (ret < 0) {
        IMAGE_LOGE("create flip filter failed, ret = %{public}d", ret);
        return false;
    }
    return true;
}

static bool FlipUpDataDstdata(YuvImageInfo &srcInfo, uint8_t *dstData, AVFrame *srcFrame, AVFrame *dstFrame)
{
    dstFrame->width = srcFrame->width;
    dstFrame->height = srcFrame->height;

    int32_t dstSize = av_image_get_buffer_size(static_cast<AVPixelFormat>(dstFrame->format),
        dstFrame->width, dstFrame->height, 1);
    if (dstSize < 0) {
        IMAGE_LOGE("FlipUpDataDstdata get size failed");
        return false;
    }

    // Copy the output frame data to the destination buffer
    if (av_image_copy_to_buffer(dstData, dstSize, dstFrame->data, dstFrame->linesize,
        static_cast<AVPixelFormat>(dstFrame->format), dstFrame->width, dstFrame->height, 1) < 0) {
        IMAGE_LOGE("FlipUpDataDstdata copy data failed");
        return false;
    }

    return true;
}

bool PixelYuvUtils::YuvFlip(const uint8_t *srcData, YuvImageInfo &srcInfo, uint8_t *dstData, bool xAxis)
{
    AVFrame *srcFrame = av_frame_alloc();
    AVFrame *dstFrame = av_frame_alloc();
    if (srcFrame == nullptr || dstFrame == nullptr) {
        IMAGE_LOGE("FlipYuv av_frame_alloc failed");
        return false;
    }
    SetAVFrameInfo(srcFrame, srcInfo);
    av_image_fill_arrays(srcFrame->data, srcFrame->linesize, srcData, srcInfo.format, srcInfo.width, srcInfo.height, 1);
    AVFilterGraph *filterGraph = avfilter_graph_alloc();
    if (!filterGraph) {
        CleanUpFilterGraph(&filterGraph, &srcFrame, &dstFrame);
        return false;
    }
    // Create buffer source filter
    AVFilterContext *bufferSrcCtx = nullptr;
    if (!CreateBufferSource(&filterGraph, &bufferSrcCtx, srcInfo)) {
        CleanUpFilterGraph(&filterGraph, &srcFrame, &dstFrame);
        return false;
    }
    // Create crop filter
    AVFilterContext *flipCtx = nullptr;
    if (!CreateFilpFilter(&filterGraph, &flipCtx, xAxis)) {
        CleanUpFilterGraph(&filterGraph, &srcFrame, &dstFrame);
        return false;
    }
    // Create buffer sink filter
    AVFilterContext *bufferSinkCtx = nullptr;
    if (!CreateBufferSinkFilter(&filterGraph, &bufferSinkCtx)) {
        CleanUpFilterGraph(&filterGraph, &srcFrame, &dstFrame);
        return false;
    }
    // Link filters
    if (avfilter_link(bufferSrcCtx, 0, flipCtx, 0) < 0 || avfilter_link(flipCtx, 0, bufferSinkCtx, 0) < 0) {
        CleanUpFilterGraph(&filterGraph, &srcFrame, &dstFrame);
        return false;
    }
    if (!ProcessFilterGraph(filterGraph, bufferSrcCtx, bufferSinkCtx, srcFrame, dstFrame)) {
        CleanUpFilterGraph(&filterGraph, &srcFrame, &dstFrame);
        return false;
    }
    if (!FlipUpDataDstdata(srcInfo, dstData, srcFrame, dstFrame)) {
        CleanUpFilterGraph(&filterGraph, &srcFrame, &dstFrame);
        return false;
    }
    // Clean up
    CleanUpFilterGraph(&filterGraph, &srcFrame, &dstFrame);
    return true;
}

bool PixelYuvUtils::YuvReversal(uint8_t *srcData, YuvImageInfo &srcInfo, uint8_t *dstData, YuvImageInfo &dstInfo)
{
    uint32_t dataSize = GetImageSize(srcInfo.width, srcInfo.height);
    std::unique_ptr<uint8_t[]> tmpData = std::make_unique<uint8_t[]>(dataSize);
    if (!YuvFlip(srcData, srcInfo, tmpData.get(), true)) {
        IMAGE_LOGE("YuvRotate failed");
        return false;
    }
    if (!YuvFlip(tmpData.get(), srcInfo, dstData, false)) {
        IMAGE_LOGE("YuvRotate failed");
        return false;
    }
    dstInfo.width = srcInfo.width;
    dstInfo.height = dstInfo.height;
    return true;
}


bool PixelYuvUtils::YuvRotate(uint8_t *srcData, YuvImageInfo &srcInfo,
    uint8_t *dstData, YuvImageInfo &dstInfo, int32_t degrees)
{
    if (degrees < 0) {
        degrees += degrees360;
    }
    switch (degrees) {
        case 0:
            return true;
        case degrees90:
            if (!Rotate(srcData, srcInfo, dstData, dstInfo, TRANSPOSE_CLOCK)) {
                IMAGE_LOGE("YuvRotate 90 failed");
                return false;
            }
            return true;
        case degrees180: {
            if (!YuvReversal(srcData, srcInfo, dstData, dstInfo)) {
                IMAGE_LOGE("YuvRotate 180 failed");
                return false;
            }
            return true;
        }
        case degrees270:
            if (!Rotate(srcData, srcInfo, dstData, dstInfo, TRANSPOSE_CCLOCK)) {
                IMAGE_LOGE("YuvRotate 270 failed");
                return false;
            }
            return true;
        default:
            return false;
    }
}

bool PixelYuvUtils::ReadYuvConvert(const void *srcPixels, const Position &srcPos, const ImageInfo &srcInfo,
    void *dstPixels, const ImageInfo &dstInfo)
{
    if (srcPixels == nullptr || dstPixels == nullptr) {
        IMAGE_LOGE("src or dst pixels invalid.");
        return false;
    }
    Rect rect;
    rect.left = srcPos.x;
    rect.top = srcPos.y;
    rect.width = dstInfo.size.width;
    rect.height = dstInfo.size.height;
    YuvImageInfo info = {ConvertFormat(srcInfo.pixelFormat), srcInfo.size.width, srcInfo.size.height};
    if (!YuvCrop((uint8_t *)srcPixels, info, static_cast<uint8_t *>(dstPixels), rect)) {
        return false;
    }
    return true;
}

static bool I420AndYV12Transfers(const uint8_t *src, uint8_t *dst, int32_t width, int32_t height)
{
    uint32_t ySize = GetYSize(width, height);
    uint32_t uvDataSize = GetUStride(width) * GetUVHeight(height);
    if (memcpy_s(dst, ySize, src, ySize) != EOK ||
        memcpy_s(dst + ySize, uvDataSize, src + ySize + uvDataSize, uvDataSize) != EOK ||
        memcpy_s(dst + ySize + uvDataSize, uvDataSize, src + ySize, uvDataSize) != EOK) {
        IMAGE_LOGE("I420AndYV12Transfers memcpy failed");
        return false;
    }
    return true;
}

void PixelYuvUtils::SetTranslateDataDefault(uint8_t *srcPixels, int32_t width, int32_t height)
{
    int32_t ySize = GetYSize(width, height);
    int32_t uvSize = GetUStride(width) * GetUVHeight(height) * NUM_2;
    if (memset_s(srcPixels, ySize, Y_DEFAULT, ySize) != EOK ||
        memset_s(srcPixels + ySize, uvSize, UV_DEFAULT, uvSize) != EOK) {
        IMAGE_LOGW("set translate default color failed");
    }
}

uint8_t PixelYuvUtils::GetYuv420Y(uint32_t x, uint32_t y, int32_t width, const uint8_t *in)
{
    return *(in + y * width + x);
}

uint8_t PixelYuvUtils::GetYuv420U(uint32_t x, uint32_t y, Size &size, PixelFormat format,
    const uint8_t *in)
{
    int32_t width = size.width;
    int32_t height = size.height;
    switch (format) {
        case PixelFormat::NV21:
            if (width & 1) {
                return *(in + y / NUM_2 * NUM_2 + GetYSize(width, height) + (y / NUM_2) * (width - 1) + (x & ~1) + 1);
            }
            return *(in + GetYSize(width, height) + (y / NUM_2) * width + (x & ~1) + 1);
        case PixelFormat::NV12:
            if (width & 1) {
                return *(in + y / NUM_2 * NUM_2 + GetYSize(width, height) + (y / NUM_2) * (width - 1) + (x & ~1));
            }
            return *(in + GetYSize(width, height) + (y / NUM_2) * width + (x & ~1));
        case PixelFormat::YU12:
            if (width & 1) {
                return *(in + y / NUM_2 + GetYSize(width, height) + (y / NUM_2) * (width / NUM_2) + (x / NUM_2));
            }
            return *(in + GetYSize(width, height) + (y / NUM_2) * (width / NUM_2) + (x / NUM_2));
        case PixelFormat::YV12:
            if (width & 1) {
                return *(in + height / NUM_2 + y / NUM_2 + GetYSize(width, height) +
                            (width / NUM_2) * (height / NUM_2) + (y / NUM_2) * (width / NUM_2) + (x / NUM_2));
            }
            return *(in + GetYSize(width, height) + (width / NUM_2) * (height / NUM_2) +
                        (y / NUM_2) * (width / NUM_2) + (x / NUM_2));
        default:
            break;
    }
    return SUCCESS;
}

uint8_t PixelYuvUtils::GetYuv420V(uint32_t x, uint32_t y, Size &size, PixelFormat format,
    const uint8_t *in)
{
    int32_t width = size.width;
    int32_t height = size.height;
    switch (format) {
        case PixelFormat::NV21:
            if (width & 1) {
                return *(in + y / NUM_2 * NUM_2 + GetYSize(width, height) + (y / NUM_2) * (width - 1) + (x & ~1));
            }
            return *(in + GetYSize(width, height) + (y / NUM_2) * width + (x & ~1));
        case PixelFormat::NV12:
            if (width & 1) {
                return *(in + y / NUM_2 * NUM_2 + GetYSize(width, height) + (y / NUM_2) * (width - 1) + (x & ~1) + 1);
            }
            return *(in + GetYSize(width, height) + (y / NUM_2) * width + (x & ~1) + 1);
        case PixelFormat::YU12:
            if (width & 1) {
                return *(in + height / NUM_2 + y / NUM_2 + GetYSize(width, height) +
                            (width / NUM_2) * (height / NUM_2) + (y / NUM_2) * (width / NUM_2) + (x / NUM_2));
            }
            return *(in + GetYSize(width, height) + (width / NUM_2) * (height / NUM_2) +
                        (y / NUM_2) * (width / NUM_2) + (x / NUM_2));
            break;
        case PixelFormat::YV12:
            if (width & 1) {
                return *(in + y / NUM_2 + GetYSize(width, height) + (y / NUM_2) * (width / NUM_2) + (x / NUM_2));
            }
            return *(in + GetYSize(width, height) + (y / NUM_2) * (width / NUM_2) + (x / NUM_2));
        default:
            break;
    }
    return SUCCESS;
}

static void MergeUV(const uint8_t *srcU, const uint8_t *srcV, uint8_t *dstUV, int32_t width)
{
    for (int32_t x = 0; x < width - 1; x += NUM_2) {
        dstUV[0] = srcU[x];
        dstUV[1] = srcV[x];
        dstUV[NUM_2] = srcU[x + 1];
        dstUV[NUM_3] = srcV[x + 1];
        dstUV += NUM_4;
    }
    if (width & 1) {
        dstUV[0] = srcU[width - 1];
        dstUV[1] = srcV[width - 1];
    }
}

static void MergeUVPlane(const uint8_t *srcU, const uint8_t *srcV, uint8_t *dstUV, Yuv420UV &info, Size &size)
{
    if (size.width <= 0 || size.height == 0) {
        return;
    }
    // Coalesce rows.
    if (info.srcStrideU == size.width && info.srcStrideV == size.width && info.dstStrideUV == size.width * NUM_2) {
        size.width *= size.height;
        size.height = 1;
        info.srcStrideU = info.srcStrideV = info.dstStrideUV = 0;
    }

    for (int32_t y = 0; y < size.height; ++y) {
        // Merge a row of U and V into a row of UV.
        MergeUV(srcU, srcV, dstUV, size.width);
        srcU += info.srcStrideU;
        srcV += info.srcStrideV;
        dstUV += info.dstStrideUV;
    }
}


#ifdef LIBYUV_ENABLE
bool PixelYuvUtils::BGRAToYuv420(const uint8_t *src, uint8_t *dst, int srcW, int srcH, PixelFormat pixelFormat)
{
    uint32_t pictureSize = GetImageSize(srcW, srcH);
    std::unique_ptr<uint8_t[]> temp = std::make_unique<uint8_t[]>(pictureSize);
    libyuv::ARGBToI420(src, srcW * NUM_4, temp.get(), srcW, temp.get() + GetYSize(srcW, srcH), GetUStride(srcW),
                      temp.get() + GetVOffset(srcW, srcH), GetUStride(srcW), srcW, srcH);
    int32_t r = 0;
    switch (pixelFormat) {
        case PixelFormat::NV12:
            r = libyuv::I420ToNV12(temp.get(), srcW, temp.get() + GetYSize(srcW, srcH), GetUStride(srcW),
                    temp.get() + GetVOffset(srcW, srcH), GetUStride(srcW), dst, srcW,
                    dst + GetYSize(srcW, srcH), GetUVStride(srcW), srcW, srcH);
            break;
        case PixelFormat::NV21:
            r = libyuv::I420ToNV21(temp.get(), srcW, temp.get() + GetYSize(srcW, srcH), GetUStride(srcW),
                    temp.get() + GetVOffset(srcW, srcH), GetUStride(srcW), dst, srcW,
                    dst + GetYSize(srcW, srcH), GetUVStride(srcW), srcW, srcH);
            break;
        default:
            break;
    }
    return r == 0;
}

bool PixelYuvUtils::Yuv420ToBGRA(const uint8_t *sample, uint8_t *dst_argb,
    int32_t width, int32_t height, PixelFormat pixelFormat)
{
    int alignedSrcW = (width + 1) & ~1;
    const uint8_t *src = nullptr;
    const uint8_t *src_uv = nullptr;
    const uint32_t dstStrideARGB = width * NUM_4;
    switch (pixelFormat) {
        case PixelFormat::NV12:
            src = sample;
            src_uv = sample + alignedSrcW * height;
            return 0 == libyuv::NV12ToARGB(src, width, src_uv, alignedSrcW, dst_argb, dstStrideARGB, width, height);
        case PixelFormat::NV21:
            src = sample;
            src_uv = sample + alignedSrcW * height;
            return 0 == libyuv::NV21ToARGB(src, width, src_uv, alignedSrcW, dst_argb, dstStrideARGB, width, height);
        default:
            break;
    }
    return false;
}

bool PixelYuvUtils::Yuv420ToARGB(const uint8_t *sample, uint8_t *dst_argb,
    int32_t width, int32_t height, PixelFormat pixelFormat)
{
    std::unique_ptr<uint8_t[]> temp = std::make_unique<uint8_t[]>(width * height * NUM_4);
    int32_t r = Yuv420ToBGRA(sample, temp.get(), width, height, pixelFormat);
    if (r != 0) {
        return false;
    }
    return 0 == libyuv::ARGBToBGRA(temp.get(), width * NUM_4, dst_argb, width * NUM_4, width, height);
}
#else

static bool I420ToNV12(const uint8_t *srcPixels, uint8_t *dstPixels, int32_t width, int32_t height)
{
    int32_t halfWidth = GetUStride(width);
    int32_t halfHeight = GetUVHeight(height);
    const uint8_t *srcV = srcPixels + GetVOffset(width, height);
    const uint8_t *srcU = srcPixels + GetYSize(width, height);
    uint8_t *dstUV = dstPixels + GetYSize(width, height);
    int32_t dstStrideUV = GetUVStride(width);
    // Negative height means invert the image.
    if (memcpy_s(dstPixels, GetYSize(width, height), srcPixels, GetYSize(width, height)) != 0) {
        IMAGE_LOGE("I420ToNV12 memcpy failed");
        return false;
    }
    Yuv420UV info = {halfWidth, halfWidth, dstStrideUV};
    Size size = {halfWidth, halfHeight};
    MergeUVPlane(srcU, srcV, dstUV, info, size);
    return true;
}

static bool I420ToNV21(const uint8_t *srcPixels, uint8_t *dstPixels, int32_t width, int32_t height)
{
    int32_t halfWidth = GetUStride(width);
    int32_t halfHeight = GetUVHeight(height);
    const uint8_t *srcU = srcPixels + GetVOffset(width, height);
    const uint8_t *srcV = srcPixels + GetYSize(width, height);
    uint8_t *dstUV = dstPixels + GetYSize(width, height);
    int32_t dstStrideUV = GetUVStride(width);
    if (memcpy_s(dstPixels, GetYSize(width, height), srcPixels, GetYSize(width, height)) != 0) {
        IMAGE_LOGE("I420ToNV21 memcpy failed");
        return false;
    }
    Yuv420UV info = {halfWidth, halfWidth, dstStrideUV};
    Size size = {halfWidth, halfHeight};
    MergeUVPlane(srcU, srcV, dstUV, info, size);
    return true;
}

static void TranslateRGBToYuv(const uint8_t *src, uint8_t *dst, Size &size, size_t &i, UVPos &uvPos)
{
    for (int x = 0; x < size.width; x += NUM_2) {
        uint8_t b = src[NUM_4 * i];
        uint8_t g = src[NUM_4 * i + 1];
        uint8_t r = src[NUM_4 * i + NUM_2];

        uint8_t y = dst[i++] = INT_16 + INT_219 * (RGB_TO_YUV_PARAM_1 * r + RGB_TO_YUV_PARAM_2 * g +
            RGB_TO_YUV_PARAM_3 * b) / INT_255;

        dst[uvPos.upos++] = INT_128 + INT_224 * (RGB_TO_YUV_PARAM_4 * (b - y) / RGB_TO_YUV_PARAM_5) / INT_255;
        dst[uvPos.vpos++] = INT_128 + INT_224 * (RGB_TO_YUV_PARAM_4 * (r - y) / RGB_TO_YUV_PARAM_6) / INT_255;

        if ((size.width & 1) && x == size.width - 1) {
            // nothing to do
        } else {
            dst[i++] = INT_16 + INT_219 * (RGB_TO_YUV_PARAM_1 * r + RGB_TO_YUV_PARAM_2 * g +
                RGB_TO_YUV_PARAM_3 * b) / INT_255;
        }
    }
}

static bool BGRAToI420(const uint8_t *src, uint8_t *dst, int32_t width, int32_t height)
{
    if (!src || !dst || width <= 0 || height == 0) {
        return false;
    }

    size_t i = 0;
    Size size = {width, height};
    UVPos uvPos;
    uvPos.upos = GetYSize(size.width, size.height);
    uvPos.vpos = GetVOffset(size.width, size.height);

    for (int32_t line = 0; line < height; ++line) {
        if (!(line % NUM_2)) {
            TranslateRGBToYuv(src, dst, size, i, uvPos);
        } else {
            for (int32_t x = 0; x < width; x += 1) {
                uint8_t b = src[NUM_4 * i];
                uint8_t g = src[NUM_4 * i + 1];
                uint8_t r = src[NUM_4 * i + NUM_2];

                dst[i++] = INT_16 + INT_219 * (RGB_TO_YUV_PARAM_1 * r + RGB_TO_YUV_PARAM_2 * g +
                    RGB_TO_YUV_PARAM_3 * b) / INT_255;
            }
        }
    }
    return true;
}

bool PixelYuvUtils::BGRAToYuv420(const uint8_t *src, uint8_t *dst, int32_t width, int32_t height,
    PixelFormat pixelFormat)
{
    uint32_t pictureSize = GetImageSize(width, height);
    if (pictureSize == 0 || pictureSize > PIXEL_YUV_MAX_RAM_SIZE) {
        IMAGE_LOGE("BGRAToYuv420 parameter pictureSize:[%{public}u] error.", pictureSize);
        return false;
    }
    std::unique_ptr<uint8_t[]> temp = std::make_unique<uint8_t[]>(pictureSize);
    bool result = BGRAToI420(src, temp.get(), width, height);
    if (!result) {
        return false;
    }
    switch (pixelFormat) {
        case PixelFormat::NV12:
            I420ToNV12(temp.get(), dst, width, height);
            break;
        case PixelFormat::NV21:
            I420ToNV21(temp.get(), dst, width, height);
            break;
        case PixelFormat::YU12:
            if (memcpy_s(dst, pictureSize, temp.get(), pictureSize) != 0) {
                IMAGE_LOGE("BGRAToYuv420 to YU12 memcpy failed.");
                return false;
            }
            return true;
        case PixelFormat::YV12:
            I420AndYV12Transfers(temp.get(), dst, width, height);
            break;
        default:
            break;
    }
    return true;
}

bool PixelYuvUtils::Yuv420ToBGRA(const uint8_t *in, uint8_t *out, int32_t width, int32_t height,
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

            int32_t colorR = Y + YUV_TO_RGB_PARAM_1 * (V - INT_128);
            int32_t colorG = Y - YUV_TO_RGB_PARAM_2 * (U - INT_128) - YUV_TO_RGB_PARAM_3 * (V - INT_128);
            int32_t colorB = Y + YUV_TO_RGB_PARAM_4 * (U - INT_128);

            colorR = colorR > INT_255 ? INT_255 : (colorR < 0 ? 0 : colorR);
            colorG = colorG > INT_255 ? INT_255 : (colorG < 0 ? 0 : colorG);
            colorB = colorB > INT_255 ? INT_255 : (colorB < 0 ? 0 : colorB);

            *out++ = colorB;
            *out++ = colorG;
            *out++ = colorR;
            *out++ = 0xFF;
        }
    }
    return true;
}

bool PixelYuvUtils::Yuv420ToARGB(const uint8_t *in, uint8_t *out, int32_t width, int32_t height,
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

            int32_t colorR = Y + YUV_TO_RGB_PARAM_1 * (V - INT_128);
            int32_t colorG = Y - YUV_TO_RGB_PARAM_2 * (U - INT_128) - YUV_TO_RGB_PARAM_3 * (V - INT_128);
            int32_t colorB = Y + YUV_TO_RGB_PARAM_4 * (U - INT_128);

            colorR = colorR > INT_255 ? INT_255 : (colorR < 0 ? 0 : colorR);
            colorG = colorG > INT_255 ? INT_255 : (colorG < 0 ? 0 : colorG);
            colorB = colorB > INT_255 ? INT_255 : (colorB < 0 ? 0 : colorB);

            *out++ = 0xFF;
            *out++ = colorR;
            *out++ = colorG;
            *out++ = colorB;
        }
    }
    return true;
}
#endif

#ifdef LIBYUV_ENABLE
static bool YuvRotateConvert(Size &size, int32_t degrees, int32_t &dstWidth, int32_t &dstHeight,
    libyuv::RotationMode &rotateNum)
{
    switch (degrees) {
        case 0:
            return true;
        case degrees90:
            dstWidth = size.height;
            dstHeight = size.width;
            rotateNum = libyuv::RotationMode::kRotate90;
            break;
        case degrees180:
            rotateNum = libyuv::RotationMode::kRotate180;
            break;
        case degrees270:
            dstWidth = size.height;
            dstHeight = size.width;
            rotateNum = libyuv::RotationMode::kRotate270;
            break;
        default:
            return false;
    }
    return true;
}

static bool NV21Rotate(const uint8_t *src, uint8_t *dstData, PixelSize &size,
    uint8_t *dstPixels, libyuv::RotationMode &rotateNum)
{
    if (libyuv::NV12ToI420Rotate(src, size.srcW, src + GetYSize(size.srcW, size.srcH),
            GetUVStride(size.srcW), dstData, size.dstW,
            dstData + GetVOffset(size.dstW, size.dstH),
            GetUStride(size.dstW), dstData + GetYSize(size.dstW, size.dstH),
            GetUStride(size.dstW), size.srcW, size.srcH, rotateNum) == -1) {
        IMAGE_LOGE("NV21 to I420 rotate failed");
        return false;
    }
    if (libyuv::I420ToNV21(dstData, size.dstW, dstData + GetYSize(size.dstW, size.dstH), GetUStride(size.dstW),
            dstData + GetVOffset(size.dstW, size.dstH), GetUStride(size.dstW),
            dstPixels, size.dstW, dstPixels + GetYSize(size.dstW, size.dstH),
            GetUVStride(size.dstW), size.dstW, size.dstH) == -1) {
        IMAGE_LOGE("I420 to NV21 failed");
        return false;
    }
    return true;
}

static bool NV12Rotate(const uint8_t *src, uint8_t *dstData, PixelSize &size,
    uint8_t *dstPixels, libyuv::RotationMode &rotateNum)
{
    if (libyuv::NV12ToI420Rotate(src, size.srcW, src + GetYSize(size.srcW, size.srcH),
            GetUVStride(size.srcW), dstData, size.dstW,
            dstData + GetYSize(size.dstW, size.dstH), GetUStride(size.dstW),
            dstData + GetVOffset(size.dstW, size.dstH), GetUStride(size.dstW),
            size.srcW, size.srcH, rotateNum) == -1) {
        IMAGE_LOGE("NV12 to I420 rotate failed");
        return false;
    }
    if (libyuv::I420ToNV12(dstData, size.dstW, dstData + GetYSize(size.dstW, size.dstH),
            GetUStride(size.dstW), dstData + GetVOffset(size.dstW, size.dstH),
            GetUStride(size.dstW), dstPixels, size.dstW, dstPixels + GetYSize(size.dstW, size.dstH),
            GetUVStride(size.dstW), size.dstW, size.dstH) == -1) {
        IMAGE_LOGE("I420 to NV21 failed");
        return false;
    }
    return true;
}

static bool I420Rotate(const uint8_t *src, PixelSize &size, uint8_t *dstPixels, libyuv::RotationMode &rotateNum)
{
    if (libyuv::I420Rotate(src, size.srcW, src + GetYSize(size.srcW, size.srcH),
            GetUStride(size.srcW), src + GetVOffset(size.srcW, size.srcH),
            GetUStride(size.srcW), dstPixels, size.dstW, dstPixels + GetYSize(size.dstW, size.dstH),
            GetUStride(size.dstW), dstPixels + GetVOffset(size.dstW, size.dstH),
            GetUStride(size.dstW), size.srcW, size.srcH, rotateNum) == -1) {
            IMAGE_LOGE("I420 rotate failed");
        return false;
    }
    return true;
}

bool PixelYuvUtils::YuvRotate(const uint8_t *srcPixels, Size &size, uint8_t *dstPixels, int32_t degrees,
    const PixelFormat &format)
{
    libyuv::RotationMode rotateNum = libyuv::RotationMode::kRotate0;
    int32_t dstWidth = size.width;
    int32_t dstHeight = size.height;
    if (YuvRotateConvert(size, degrees, dstWidth, dstHeight, rotateNum)) {
        IMAGE_LOGI("Rotate degress is 0, don't need rotate");
        return true;
    }
    MemoryData memoryData = {nullptr, GetImageSize(dstWidth, dstHeight), "YuvRotate ImageData", {dstWidth, dstHeight}};
    auto m = MemoryManager::CreateMemory(allocatorType_, memoryData);
    if (m == nullptr) {
        IMAGE_LOGE("translate CreateMemory failed");
        return false;
    }
#if !defined(IOS_PLATFORM) && !defined(A_PLATFORM)
    if (allocatorType_ == AllocatorType::DMA_ALLOC) {
        if (m->extend.data == nullptr) {
            IMAGE_LOGE("GendstTransInfo get surfacebuffer failed");
        }
    }
#endif
    uint8_t *dstData = reinterpret_cast<uint8_t *>(m->data.data);
    PixelSize pixelSize = {size.width, size.height, dstWidth, dstHeight};
    switch (format) {
        case PixelFormat::NV21:
            if (!NV21Rotate(srcPixels, dstData, pixelSize, dstPixels, rotateNum)) {
                m->Release();
                return false;
            }
        case PixelFormat::NV12:
            if (!NV12Rotate(srcPixels, dstData, pixelSize, dstPixels, rotateNum)) {
                m->Release();
                return false;
            }
        case PixelFormat::YV12:
        case PixelFormat::YU12:
            if (!I420Rotate(srcPixels, pixelSize, dstPixels, rotateNum)) {
                m->Release();
                return false;
            }
        default:
            return false;
    }
    size.width = dstWidth;
    size.height = dstHeight;
    return true;
}

void PixelYuvUtils::ConvertYuvMode(libyuv::FilterMode &filterMode, const AntiAliasingOption &option)
{
    switch (option) {
        case AntiAliasingOption::NONE:
            filterMode = libyuv::FilterMode::kFilterNone;
            break;
        case AntiAliasingOption::LOW:
            filterMode = libyuv::FilterMode::kFilterLinear;
            break;
        case AntiAliasingOption::MEDIUM:
            filterMode = libyuv::FilterMode::kFilterBilinear;
            break;
        case AntiAliasingOption::HIGH:
            filterMode = libyuv::FilterMode::kFilterBox;
            break;
        default:
            break;
    }
}
#else
#endif

static void Yuv420PWritePixels(Size &size, uint8_t *srcPixels, const uint32_t &color, bool isI420)
{
    uint8_t colorY = (color >> Y_SHIFT) & YUV_MASK;
    uint8_t colorU = (color >> U_SHIFT) & YUV_MASK;
    uint8_t colorV = (color >> V_SHIFT) & YUV_MASK;

    uint8_t *srcU;
    uint8_t *srcV;
    if (isI420) {
        srcU = srcPixels + GetYSize(size.width, size.height);
        srcV = srcPixels + GetVOffset(size.width, size.height);
    } else {
        srcV = srcPixels + GetYSize(size.width, size.height);
        srcU = srcPixels + GetVOffset(size.width, size.height);
    }

    for (int32_t i = 0; i < size.height; i++) {
        for (int32_t j = 0; j < size.width; j++) {
            *(srcPixels + i * size.width + j) = colorY;
        }
    }
    for (int32_t i = 0; i < GetUVHeight(size.height); i++) {
        for (int32_t j = 0; j < GetUStride(size.width); j++) {
            *(srcU + i * GetUStride(size.width) + j) = colorU;
            *(srcV + i * GetUStride(size.width) + j) = colorV;
        }
    }
}

static void Yuv420SPWritePixels(Size &size, uint8_t *srcPixels, const uint32_t &color, bool isNV12)
{
    uint8_t colorY = (color >> Y_SHIFT) & YUV_MASK;
    uint8_t colorU = (color >> U_SHIFT) & YUV_MASK;
    uint8_t colorV = (color >> V_SHIFT) & YUV_MASK;

    uint8_t *srcUV = srcPixels + GetYSize(size.width, size.height);

    for (int32_t y = 0; y < size.height; y++) {
        for (int32_t x = 0; x < size.width; x++) {
            *(srcPixels + y * size.width + x) = colorY;
        }
    }

    for (int32_t y = 0; y < size.height / NUM_2; y++) {
        for (int32_t x = 0; x < size.width; x++) {
            if (isNV12) {
                *(srcUV + (y * size.width + x)) = colorU;
                *(srcUV + (y * size.width + x) + 1) = colorV;
            } else {
                *(srcUV + (y * size.width + x)) = colorV;
                *(srcUV + (y * size.width + x) + 1) = colorU;
            }
        }
    }
}

bool PixelYuvUtils::Yuv420WritePixels(Size &size, uint8_t *srcPixels, const PixelFormat &format, const uint32_t &color)
{
    switch (format) {
        case PixelFormat::YU12:
        case PixelFormat::YV12: {
            bool isI420 = (format == PixelFormat::YU12 ? true : false);
            Yuv420PWritePixels(size, srcPixels, color, isI420);
            return true;
        }
        case PixelFormat::NV21:
        case PixelFormat::NV12: {
            bool isNV12 = (format == PixelFormat::NV12 ? true : false);
            Yuv420SPWritePixels(size, srcPixels, color, isNV12);
            return true;
        }
        default:
            return false;
    }
}

static void Yuv420PReadPixel(const uint8_t *srcPixels, Size &size, const Position &pos, uint32_t &dst, bool isI420)
{
    const uint8_t *srcU;
    const uint8_t *srcV;

    if (isI420) {
        srcU = srcPixels + GetYSize(size.width, size.height);
        srcV = srcPixels + GetVOffset(size.width, size.height);
    } else {
        srcV = srcPixels + GetYSize(size.width, size.height);
        srcU = srcPixels + GetVOffset(size.width, size.height);
    }

    uint8_t colorY = *(srcPixels + (pos.y * size.width + pos.x));
    uint8_t colorU = *(srcU + ((pos.y / NUM_2) * GetUStride(size.width) + pos.x / NUM_2));
    uint8_t colorV = *(srcV + ((pos.y / NUM_2) * GetUStride(size.width) + pos.x / NUM_2));

    uint32_t yuvColor = (colorY << Y_SHIFT) | (colorU << U_SHIFT) | (colorV << V_SHIFT);
    dst = yuvColor;
}

static void Yuv420SPReadPixel(const uint8_t *srcPixels, Size &size, const Position &pos, uint32_t &dst, bool isNV12)
{
    const uint8_t *srcUV = srcPixels + GetYSize(size.width, size.height);
    uint8_t colorY = *(srcPixels + (pos.y * size.width + pos.x));
    uint8_t colorU = 0;
    uint8_t colorV = 0;

    if (isNV12) {
        colorU = *(srcUV + (pos.y / NUM_2) * GetUStride(size.width) + pos.x / NUM_2);
        colorV = *(srcUV + (pos.y / NUM_2) * GetUStride(size.width) + pos.x / NUM_2 + 1);
    } else {
        colorV = *(srcUV + (pos.y / NUM_2) * GetUStride(size.width) + pos.x / NUM_2);
        colorU = *(srcUV + (pos.y / NUM_2) * GetUStride(size.width) + pos.x / NUM_2 + 1);
    }

    uint32_t yuvColor = (colorY << Y_SHIFT) | (colorU << U_SHIFT) | (colorV << V_SHIFT);
    dst = yuvColor;
}


bool PixelYuvUtils::YuvReadPixel(const uint8_t *srcPixels, Size &size, const PixelFormat &format, const Position &pos,
    uint32_t &dst)
{
    switch (format) {
        case PixelFormat::YU12:
        case PixelFormat::YV12: {
            bool isI420 = (format == PixelFormat::YU12) ? true : false;
            Yuv420PReadPixel(srcPixels, size, pos, dst, isI420);
            return true;
        }
        case PixelFormat::NV21:
        case PixelFormat::NV12: {
            bool isNV12 = (format == PixelFormat::NV12) ? true : false;
            Yuv420SPReadPixel(srcPixels, size, pos, dst, isNV12);
            return true;
        }
        default:
            return false;
    }
}

static void Yuv420PWritePixel(uint8_t *srcPixels, Size &size, const Position &pos,
    const uint32_t &color, bool isI420)
{
    uint8_t *srcU;
    uint8_t *srcV;

    uint8_t colorY = (color >> Y_SHIFT) & YUV_MASK;
    uint8_t colorU = (color >> U_SHIFT) & YUV_MASK;
    uint8_t colorV = (color >> V_SHIFT) & YUV_MASK;

    if (isI420) {
        srcU = srcPixels + GetYSize(size.width, size.height);
        srcV = srcPixels + GetVOffset(size.width, size.height);
    } else {
        srcV = srcPixels + GetYSize(size.width, size.height);
        srcU = srcPixels + GetVOffset(size.width, size.height);
    }

    *(srcPixels + (pos.y * size.width + pos.x)) = colorY;
    *(srcU + ((pos.y / NUM_2) * GetUStride(size.width) + pos.x / NUM_2)) = colorU;
    *(srcV + ((pos.y / NUM_2) * GetUStride(size.width) + pos.x / NUM_2)) = colorV;
}

static void Yuv420SPWritePixel(uint8_t *srcPixels, Size &size, const Position &pos,
    const uint32_t &color, bool isNV12)
{
    uint8_t *srcUV = srcPixels + GetYSize(size.width, size.height);
    uint8_t colorY = (color >> Y_SHIFT) & YUV_MASK;
    uint8_t colorU = (color >> U_SHIFT) & YUV_MASK;
    uint8_t colorV = (color >> V_SHIFT) & YUV_MASK;

    *(srcPixels + (pos.y * size.width + pos.x)) = colorY;

    if (isNV12) {
        *(srcUV + (pos.y / NUM_2) * GetUStride(size.width) + pos.x / NUM_2) = colorU;
        *(srcUV + (pos.y / NUM_2) * GetUStride(size.width) + pos.x / NUM_2 + 1) = colorV;
    } else {
        *(srcUV + (pos.y / NUM_2) * GetUStride(size.width) + pos.x / NUM_2) = colorV;
        *(srcUV + (pos.y / NUM_2) * GetUStride(size.width) + pos.x / NUM_2 + 1) = colorU;
    }
}

bool PixelYuvUtils::YuvWritePixel(uint8_t *srcPixels, Size &size, const PixelFormat &format, const Position &pos,
    const uint32_t &color)
{
    switch (format) {
        case PixelFormat::YU12:
        case PixelFormat::YV12: {
            bool isI420 = (format == PixelFormat::YU12) ? true : false;
            Yuv420PWritePixel(srcPixels, size, pos, color, isI420);
            return true;
        }
        case PixelFormat::NV21:
        case PixelFormat::NV12: {
            bool isNV12 = (format == PixelFormat::NV12) ? true : false;
            Yuv420SPWritePixel(srcPixels, size, pos, color, isNV12);
            return true;
        }
        default:
            return false;
    }
}

static void Yuv420PTranslate(const uint8_t *srcPixels, Size &size, uint8_t *dstPixels, XYaxis &xyAxis,
    bool isI420)
{
    int32_t dstWidth = size.width;
    int32_t dstHeight = size.height;
    const uint8_t *srcU;
    const uint8_t *srcV;
    uint8_t *dstU;
    uint8_t *dstV;
    if (isI420) {
        srcU = srcPixels + GetYSize(size.width, size.height);
        srcV = srcPixels + GetVOffset(size.width, size.height);
        dstU = dstPixels + GetYSize(dstWidth, dstHeight);
        dstV = dstPixels + GetVOffset(dstWidth, dstHeight);
    } else {
        srcV = srcPixels + GetYSize(size.width, size.height);
        srcU = srcPixels + GetVOffset(size.width, size.height);
        dstV = dstPixels + GetYSize(dstWidth, dstHeight);
        dstU = dstPixels + GetVOffset(dstWidth, dstHeight);
    }

    for (int32_t y = 0; y < size.height; y++) {
        for (int32_t x = 0; x < size.width; x++) {
            int32_t newX = x + xyAxis.xAxis;
            int32_t newY = y + xyAxis.yAxis;
            if (newX >= 0 && newY >= 0 && newX < size.width && newY < size.height) {
                *(dstPixels + newY * dstWidth + newX) = *(srcPixels + y * size.width + x);
            }
        }
    }

    for (int32_t y = 0; y < GetUVHeight(size.height); y++) {
        for (int32_t x = 0; x < GetUStride(size.width); x++) {
            int32_t newX = x + xyAxis.xAxis / NUM_2;
            int32_t newY = y + xyAxis.yAxis / NUM_2;
            if (newX >= 0 && newY >= 0 && newX < size.width / NUM_2 && newY < size.height / NUM_2) {
                *(dstU + newY * GetUStride(dstWidth) + newX) = *(srcU + y * GetUStride(size.width) + x);
                *(dstV + newY * GetUStride(dstWidth) + newX) = *(srcV + y * GetUStride(size.width) + x);
            }
        }
    }
}

static void Yuv420SPTranslate(const uint8_t *srcPixels, Size &size, uint8_t *dstPixels, XYaxis &xyAxis)
{
    int32_t dstWidth = size.width;
    int32_t dstHeight = size.height;

    const uint8_t *srcUV = srcPixels + GetYSize(size.width, size.height);
    uint8_t *dstUV = dstPixels + GetYSize(dstWidth, dstHeight);

    for (int32_t y = 0; y < size.height; y++) {
        for (int32_t x = 0; x < size.width; x++) {
            int32_t newX = x + xyAxis.xAxis;
            int32_t newY = y + xyAxis.yAxis;
            if (newX >= 0 && newY >= 0 && newX < size.width && newY < size.height) {
                *(dstPixels + newY * dstWidth + newX) = *(srcPixels + y * size.width + x);
            }
        }
    }

    for (int32_t y = 0; y < GetUVHeight(size.height); y++) {
        for (int32_t x = 0; x < GetUStride(size.width); x++) {
            int32_t newX = x + xyAxis.xAxis / NUM_2;
            int32_t newY = y + xyAxis.yAxis / NUM_2;
            if (newX >= 0 && newX < size.width / NUM_2 && newY >= 0 && newY < size.height / NUM_2) {
                *(dstUV + (newY * dstWidth + newX * NUM_2)) = *(srcUV + (y * size.width + x * NUM_2));
                *(dstUV + (newY * dstWidth + newX * NUM_2) + 1) = *(srcUV + (y * size.width + x * NUM_2) + 1);
            }
        }
    }
}

bool PixelYuvUtils::YuvTranslate(const uint8_t *srcPixels, ImageInfo &info, uint8_t *dstPixels, XYaxis &xyAxis)
{
    switch (info.pixelFormat) {
        case PixelFormat::YU12:
        case PixelFormat::YV12: {
            bool isI420 = (info.pixelFormat == PixelFormat::YU12) ? true : false;
            Yuv420PTranslate(srcPixels, info.size, dstPixels, xyAxis, isI420);
            return true;
        }
        case PixelFormat::NV21:
        case PixelFormat::NV12: {
            Yuv420SPTranslate(srcPixels, info.size, dstPixels, xyAxis);
            return true;
        }
        default:
            return false;
    }
}

} // namespace Media
} // namespace OHOS