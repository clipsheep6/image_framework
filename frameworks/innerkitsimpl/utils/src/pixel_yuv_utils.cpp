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
#if !defined(IOS_PLATFORM) && !defined(ANDROID_PLATFORM)
#include "surface_buffer.h"
#endif

#undef LOG_DOMAIN
#define LOG_DOMAIN LOG_TAG_DOMAIN_ID_IMAGE

#undef LOG_TAG
#define LOG_TAG "PixelYuvUtils"

namespace OHOS {
namespace Media {

static const uint8_t NUM_2 = 2;

static const int32_t degrees360 = 360;
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
    { PixelFormat::ARGB_8888, AVPixelFormat::AV_PIX_FMT_ARGB },
    { PixelFormat::BGRA_8888, AVPixelFormat::AV_PIX_FMT_BGRA },
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

static void WriteDataNV12Convert(uint8_t *srcPixels, const Size &size, uint8_t *dstPixels,
    Position dstPos, const YUVDataInfo &yuvDataInfo)
{
    uint8_t *dstY = dstPixels + yuvDataInfo.yOffset;
    uint8_t *dstUV = dstPixels + yuvDataInfo.uvOffset;
    dstPos.y = GetUVStride(dstPos.y);
    for (int i = 0; i < size.height; i++) {
        if (memcpy_s(dstY + (dstPos.y + i) * yuvDataInfo.yStride + dstPos.x,
            size.width,
            srcPixels + i * size.width,
            size.width) != 0) {
            IMAGE_LOGE("WriteDataNV12Convert memcpy yplane failed");
            return;
        }
    }
    for (int i = 0; i < GetUVHeight(size.height); ++i) {
        if (memcpy_s(dstUV + ((dstPos.y) / NUM_2 + i) * GetUVStride(yuvDataInfo.uvStride) + dstPos.x,
            GetUVStride(size.width),
            srcPixels + GetYSize(size.width, size.height) + i * GetUVStride(size.width),
            GetUVStride(size.width)) != 0) {
            IMAGE_LOGE("WriteDataNV12Convert memcpy uplane or vplane failed");
            return;
        }
    }
}

bool PixelYuvUtils::WriteYuvConvert(const void *srcPixels, const ImageInfo &srcInfo, void *dstPixels,
    const Position &dstPos, const YUVDataInfo &yuvDataInfo)
{
    if (srcPixels == nullptr || dstPixels == nullptr) {
        IMAGE_LOGE("src or dst pixels invalid.");
        return false;
    }
    switch (srcInfo.pixelFormat) {
        case PixelFormat::NV21:
            WriteDataNV12Convert((uint8_t *)srcPixels, srcInfo.size, static_cast<uint8_t *>(dstPixels), dstPos,
                yuvDataInfo);
            return true;
        case PixelFormat::NV12:
            WriteDataNV12Convert((uint8_t *)srcPixels, srcInfo.size, static_cast<uint8_t *>(dstPixels), dstPos,
                yuvDataInfo);
            return true;
        default:
            return false;
    }
}

static void FillSrcFrameInfo(AVFrame *frame, uint8_t *pixels, YuvImageInfo &info)
{
    if (info.format == AVPixelFormat::AV_PIX_FMT_NV21 || info.format == AVPixelFormat::AV_PIX_FMT_NV12) {
        frame->data[0] = pixels + info.yuvDataInfo.yOffset;
        frame->data[1] = pixels + info.yuvDataInfo.uvOffset;
        frame->linesize[0] = info.yuvDataInfo.yStride;
        frame->linesize[1] = info.yuvDataInfo.uvStride;
    } else {
        av_image_fill_arrays(frame->data, frame->linesize, pixels,
            info.format, info.width, info.height, 1);
    }
}

static void FillRectFrameInfo(AVFrame *frame, uint8_t *pixels, const Rect &rect)
{
    frame->data[0] = pixels;
    frame->data[1] = pixels + GetYSize(rect.width, rect.height);
    frame->linesize[0] = rect.width;
    frame->linesize[1] = GetUVStride(rect.width);
}

static void FillDstFrameInfo(AVFrame *frame, uint8_t *pixels, YuvImageInfo &info)
{
    if (info.format == AVPixelFormat::AV_PIX_FMT_NV21 || info.format == AVPixelFormat::AV_PIX_FMT_NV12) {
        frame->data[0] = pixels;
        frame->data[1] = pixels + GetYSize(info.width, info.height);
        frame->linesize[0] = info.width;
        frame->linesize[1] = GetUVStride(info.width);
    } else {
        av_image_fill_arrays(frame->data, frame->linesize, pixels,
            info.format, info.width, info.height, 1);
    }
}

static void FillRotateFrameInfo(AVFrame *frame, uint8_t *pixels, YuvImageInfo &info)
{
    frame->data[0] = pixels;
    frame->data[1] = pixels + GetYSize(info.width, info.height);
    frame->linesize[0] = info.height;
    frame->linesize[1] = GetUVStride(info.height);
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
        srcInfo.yuvDataInfo.yStride, srcInfo.yuvDataInfo.yHeight, srcInfo.format);
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
    FillSrcFrameInfo(srcFrame, srcData, srcInfo);
    FillRectFrameInfo(dstFrame, dstData, rect);
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

int32_t PixelYuvUtils::YuvScale(uint8_t *srcPixels, YuvImageInfo &srcInfo,
    uint8_t *dstPixels, YuvImageInfo &dstInfo, int32_t module)
{
    int ret = 0;
    AVFrame *srcFrame = nullptr;
    AVFrame *dstFrame = nullptr;
    struct SwsContext *ctx = nullptr;

    if (srcInfo.format == AVPixelFormat::AV_PIX_FMT_NONE || dstInfo.format == AVPixelFormat::AV_PIX_FMT_NONE) {
        IMAGE_LOGE("unsupport src/dst pixel format!");
        return -1;
    }
    if (srcInfo.width <= 0 || srcInfo.height <= 0 || dstInfo.width <= 0 || dstInfo.height <= 0) {
        IMAGE_LOGE("src/dst width/height error!");
        return -1;
    }

    srcFrame = av_frame_alloc();
    dstFrame = av_frame_alloc();
    if (srcFrame != nullptr && dstFrame != nullptr) {
        ctx = sws_getContext(srcInfo.width, srcInfo.height, srcInfo.format,
                             dstInfo.width, dstInfo.height, dstInfo.format,
                             module, nullptr, nullptr, nullptr);
        if (ctx != nullptr) {
            FillSrcFrameInfo(srcFrame, srcPixels, srcInfo);
            FillDstFrameInfo(dstFrame, dstPixels, dstInfo);
            sws_scale(ctx, srcFrame->data, srcFrame->linesize, 0, srcInfo.height,
                dstFrame->data, dstFrame->linesize);
        } else {
            IMAGE_LOGE("FFMpeg: sws_getContext failed!");
            ret = -1;
        }
    } else {
        IMAGE_LOGE("FFMpeg: av_frame_alloc failed!");
        ret = -1;
    }

    av_frame_free(&srcFrame);
    av_frame_free(&dstFrame);
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
    FillSrcFrameInfo(srcFrame, srcData, srcInfo);
    FillRotateFrameInfo(dstFrame, dstData, srcInfo);

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
        flipType, NULL, NULL, *filterGraph);
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

bool PixelYuvUtils::YuvFlip(uint8_t *srcData, YuvImageInfo &srcInfo, uint8_t *dstData, bool xAxis)
{
    AVFrame *srcFrame = av_frame_alloc();
    AVFrame *dstFrame = av_frame_alloc();
    if (srcFrame == nullptr || dstFrame == nullptr) {
        IMAGE_LOGE("FlipYuv av_frame_alloc failed");
        return false;
    }
    SetAVFrameInfo(srcFrame, srcInfo);
    FillSrcFrameInfo(srcFrame, srcData, srcInfo);
    FillDstFrameInfo(dstFrame, dstData, srcInfo);
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
        IMAGE_LOGE("YuvFlip xAxis failed");
        return false;
    }
    if (!YuvFlip(tmpData.get(), srcInfo, dstData, false)) {
        IMAGE_LOGE("YuvFlip yAxis failed");
        return false;
    }
    dstInfo.width = srcInfo.width;
    dstInfo.height = srcInfo.height;
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

bool PixelYuvUtils::ReadYuvConvert(const void *srcPixels, const Position &srcPos, YuvImageInfo &info,
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
    if (!YuvCrop((uint8_t *)srcPixels, info, static_cast<uint8_t *>(dstPixels), rect)) {
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

uint8_t PixelYuvUtils::GetYuv420Y(uint32_t x, uint32_t y, YUVDataInfo &info, const uint8_t *in)
{
    return *(in + y * info.yStride + x);
}

uint8_t PixelYuvUtils::GetYuv420U(uint32_t x, uint32_t y, YUVDataInfo &info, PixelFormat format,
    const uint8_t *in)
{
    int32_t width = info.yStride;
    switch (format) {
        case PixelFormat::NV21:
            if (width & 1) {
                return *(in + y / NUM_2 * NUM_2 + info.uvOffset + (y / NUM_2) * (width - 1) + (x & ~1) + 1);
            }
            return *(in + info.uvOffset + (y / NUM_2) * width + (x & ~1) + 1);
        case PixelFormat::NV12:
            if (width & 1) {
                return *(in + y / NUM_2 * NUM_2 + info.uvOffset + (y / NUM_2) * (width - 1) + (x & ~1));
            }
            return *(in + info.uvOffset + (y / NUM_2) * width + (x & ~1));
        default:
            break;
    }
    return SUCCESS;
}

uint8_t PixelYuvUtils::GetYuv420V(uint32_t x, uint32_t y, YUVDataInfo &info, PixelFormat format,
    const uint8_t *in)
{
    int32_t width = info.yStride;
    switch (format) {
        case PixelFormat::NV21:
            if (width & 1) {
                return *(in + y / NUM_2 * NUM_2 + info.uvOffset + (y / NUM_2) * (width - 1) + (x & ~1));
            }
            return *(in + info.uvOffset + (y / NUM_2) * width + (x & ~1));
        case PixelFormat::NV12:
            if (width & 1) {
                return *(in + y / NUM_2 * NUM_2 + info.uvOffset + (y / NUM_2) * (width - 1) + (x & ~1) + 1);
            }
            return *(in + info.uvOffset + (y / NUM_2) * width + (x & ~1) + 1);
        default:
            break;
    }
    return SUCCESS;
}

bool PixelYuvUtils::BGRAToYuv420(const uint8_t *src, YuvImageInfo &srcInfo, uint8_t *dst, YuvImageInfo &dstInfo)
{
    if (YuvScale(const_cast<uint8_t *>(src), srcInfo, dst, dstInfo, SWS_BICUBIC) != SUCCESS) {
        IMAGE_LOGE("BGRAToYuv420 failed");
        return false;
    }
    return true;
}

bool PixelYuvUtils::Yuv420ToBGRA(const uint8_t *in, YuvImageInfo &srcInfo, uint8_t *out, YuvImageInfo &dstInfo)
{
    if (YuvScale(const_cast<uint8_t *>(in), srcInfo, out, dstInfo, SWS_BICUBIC) != SUCCESS) {
        IMAGE_LOGE("Yuv420ToBGRA failed");
        return false;
    }
    return true;
}

bool PixelYuvUtils::Yuv420ToARGB(const uint8_t *in, YuvImageInfo &srcInfo, uint8_t *out, YuvImageInfo &dstInfo)
{
    if (YuvScale(const_cast<uint8_t *>(in), srcInfo, out, dstInfo, SWS_BICUBIC) != SUCCESS) {
        IMAGE_LOGE("Yuv420ToBGRA failed");
        return false;
    }
    return true;
}

static void Yuv420SPWritePixels(const YUVDataInfo &yuvDataInfo, uint8_t *srcPixels, const uint32_t &color, bool isNV12)
{
    uint8_t colorY = (color >> Y_SHIFT) & YUV_MASK;
    uint8_t colorU = (color >> U_SHIFT) & YUV_MASK;
    uint8_t colorV = (color >> V_SHIFT) & YUV_MASK;

    uint8_t *srcY = srcPixels + yuvDataInfo.yOffset;
    uint8_t *srcUV = srcPixels + yuvDataInfo.uvOffset;

    for (int32_t y = 0; y < yuvDataInfo.yHeight; y++) {
        for (int32_t x = 0; x < yuvDataInfo.yWidth; x++) {
            *(srcY + y * yuvDataInfo.yStride + x) = colorY;
        }
    }

    for (int32_t y = 0; y < yuvDataInfo.uvHeight; y++) {
        for (int32_t x = 0; x < yuvDataInfo.yWidth; x += NUM_2) {
            if (isNV12) {
                *(srcUV + (y * yuvDataInfo.uvStride + x)) = colorU;
                *(srcUV + (y * yuvDataInfo.uvStride + x) + 1) = colorV;
            } else {
                *(srcUV + (y * yuvDataInfo.uvStride + x)) = colorV;
                *(srcUV + (y * yuvDataInfo.uvStride + x) + 1) = colorU;
            }
        }
    }
}

bool PixelYuvUtils::Yuv420WritePixels(const YUVDataInfo &yuvDataInfo, uint8_t *srcPixels, const PixelFormat &format,
    const uint32_t &color)
{
    switch (format) {
        case PixelFormat::NV21:
        case PixelFormat::NV12: {
            bool isNV12 = (format == PixelFormat::NV12 ? true : false);
            Yuv420SPWritePixels(yuvDataInfo, srcPixels, color, isNV12);
            return true;
        }
        default:
            return false;
    }
}

static void Yuv420SPReadPixel(const uint8_t *srcPixels, const YUVDataInfo &yuvDataInfo, const Position &pos,
    uint32_t &dst, bool isNV12)
{
    const uint8_t *srcY = srcPixels + yuvDataInfo.yOffset;
    const uint8_t *srcUV = srcPixels + yuvDataInfo.uvOffset;
    uint8_t colorY = *(srcY + (pos.y * yuvDataInfo.yStride + pos.x));
    uint8_t colorU = 0;
    uint8_t colorV = 0;

    if (isNV12) {
        colorU = *(srcUV + (pos.y / NUM_2) * yuvDataInfo.uvStride + pos.x / NUM_2);
        colorV = *(srcUV + (pos.y / NUM_2) * yuvDataInfo.uvStride + pos.x / NUM_2 + 1);
    } else {
        colorV = *(srcUV + (pos.y / NUM_2) * yuvDataInfo.uvStride + pos.x / NUM_2);
        colorU = *(srcUV + (pos.y / NUM_2) * yuvDataInfo.uvStride + pos.x / NUM_2 + 1);
    }

    uint32_t yuvColor = (colorY << Y_SHIFT) | (colorU << U_SHIFT) | (colorV << V_SHIFT);
    dst = yuvColor;
}


bool PixelYuvUtils::YuvReadPixel(const uint8_t *srcPixels, const YUVDataInfo &yuvDataInfo, const PixelFormat &format,
    const Position &pos, uint32_t &dst)
{
    switch (format) {
        case PixelFormat::NV21:
        case PixelFormat::NV12: {
            bool isNV12 = (format == PixelFormat::NV12) ? true : false;
            Yuv420SPReadPixel(srcPixels, yuvDataInfo, pos, dst, isNV12);
            return true;
        }
        default:
            return false;
    }
}

static void Yuv420SPWritePixel(uint8_t *srcPixels, const YUVDataInfo &yuvDataInfo, const Position &pos,
    const uint32_t &color, bool isNV12)
{
    uint8_t *srcY = srcPixels + yuvDataInfo.yOffset;
    uint8_t *srcUV = srcPixels + yuvDataInfo.uvOffset;
    uint8_t colorY = (color >> Y_SHIFT) & YUV_MASK;
    uint8_t colorU = (color >> U_SHIFT) & YUV_MASK;
    uint8_t colorV = (color >> V_SHIFT) & YUV_MASK;

    *(srcY + (pos.y * yuvDataInfo.yStride + pos.x)) = colorY;

    if (isNV12) {
        *(srcUV + (pos.y / NUM_2) * yuvDataInfo.uvStride + pos.x / NUM_2) = colorU;
        *(srcUV + (pos.y / NUM_2) * yuvDataInfo.uvStride + pos.x / NUM_2 + 1) = colorV;
    } else {
        *(srcUV + (pos.y / NUM_2) * yuvDataInfo.uvStride + pos.x / NUM_2) = colorV;
        *(srcUV + (pos.y / NUM_2) * yuvDataInfo.uvStride + pos.x / NUM_2 + 1) = colorU;
    }
}

bool PixelYuvUtils::YuvWritePixel(uint8_t *srcPixels, const YUVDataInfo &yuvDataInfo, const PixelFormat &format,
    const Position &pos, const uint32_t &color)
{
    switch (format) {
        case PixelFormat::NV21:
        case PixelFormat::NV12: {
            bool isNV12 = (format == PixelFormat::NV12) ? true : false;
            Yuv420SPWritePixel(srcPixels, yuvDataInfo, pos, color, isNV12);
            return true;
        }
        default:
            return false;
    }
}

static void Yuv420SPTranslate(const uint8_t *srcPixels, YUVDataInfo &info, uint8_t *dstPixels, XYaxis &xyAxis)
{
    const uint8_t *srcY = srcPixels + info.yOffset;
    const uint8_t *srcUV = srcPixels + info.uvOffset;
    uint8_t *dstY = dstPixels + info.yOffset;
    uint8_t *dstUV = dstPixels + info.uvOffset;

    for (int32_t y = 0; y < info.yHeight; y++) {
        for (int32_t x = 0; x < info.yWidth; x++) {
            int32_t newX = x + xyAxis.xAxis;
            int32_t newY = y + xyAxis.yAxis;
            if (newX >= 0 && newY >= 0 && newX < info.yStride && newY < info.yHeight) {
                *(dstY + newY * info.yStride + newX) = *(srcY + y * info.yStride + x);
            }
        }
    }

    for (int32_t y = 0; y < GetUVHeight(info.yHeight); y++) {
        for (int32_t x = 0; x < GetUVStride(info.yWidth); x += NUM_2) {
            int32_t newX = x + xyAxis.xAxis;
            int32_t newY = y + xyAxis.yAxis / NUM_2;
            if (newX >= 0 && newX < info.uvStride && newY >= 0 && newY < GetUVHeight(info.yHeight)) {
                *(dstUV + newY * info.uvStride + newX) = *(srcUV + y * info.uvStride + x);
                *(dstUV + newY * info.uvStride + newX + 1) = *(srcUV + y * info.uvStride + x + 1);
            }
        }
    }
}

bool PixelYuvUtils::YuvTranslate(const uint8_t *srcPixels, YUVDataInfo &info, uint8_t *dstPixels, XYaxis &xyAxis,
    const PixelFormat &format)
{
    switch (format) {
        case PixelFormat::NV21:
        case PixelFormat::NV12: {
            Yuv420SPTranslate(srcPixels, info, dstPixels, xyAxis);
            return true;
        }
        default:
            return false;
    }
}

} // namespace Media
} // namespace OHOS