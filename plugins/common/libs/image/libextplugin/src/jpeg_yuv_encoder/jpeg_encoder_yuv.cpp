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

#include <dlfcn.h>
#include <iostream>
#include <memory>
#include "turbojpeg.h"
#include "securec.h"
#include "image_log.h"
#include "jpeg_encoder_yuv.h"

namespace OHOS {
namespace ImagePlugin {

constexpr int32_t COMP_PAD = 4;
constexpr int32_t COMP_SUBSAMP = TJSAMP_420;
constexpr int32_t COMP_JPEGQUAL = 100;
constexpr int32_t COMP_FLAGS = 0;
static const char* YUV_LIB_PATH = "libyuv.z.so";

typedef int32_t(*CONVERT_FUNC)(const uint8_t*, int32_t, const uint8_t*, int32_t,
    uint8_t*, int32_t,  uint8_t*, int32_t, uint8_t*, int32_t, int32_t, int32_t);

JpegEncoderYuv::JpegEncoderYuv(uint32_t yuvWidth, uint32_t yuvHeight, Media::PixelFormat yuvPixFormat,
    const uint8_t* yuvBuffer, uint32_t yuvBufferSize, uint8_t** jpegBuffer, unsigned long* jpegBufferSize,
    JpegYuvEncodeError& err) : dlHandler_(nullptr), tjHandler_(nullptr), jpegBuffer_(nullptr)
{
    dlHandler_ = dlopen(YUV_LIB_PATH, RTLD_LAZY | RTLD_NODELETE);

    if (yuvWidth <= 0 || yuvHeight <= 0 || yuvBuffer == nullptr || yuvBufferSize <= 0) {
        IMAGE_LOGE("Bad yuv parameter.");
        err = JpegYuvEncodeError_BadParameter;
        return;
    }

    std::unique_ptr<uint8_t[]> i420Buf = std::make_unique<uint8_t[]>(yuvBufferSize);
    if (!i420Buf) {
        IMAGE_LOGE("Alloc I420 buffer failed.");
        err = JpegYuvEncodeError_MemoryAllocFailed;
        return;
    }

    /*
     * jpeg-turbo compresses a YUV planar image into a JPEG image，
     * so we need to convert NV12/NV21 to I420.
     */
    err = DoConvert(yuvWidth, yuvHeight, yuvPixFormat, yuvBuffer, i420Buf.get());
    if (err != JpegYuvEncodeError_Success) {
        return;
    }

    tjHandler_ = tjInitCompress();
    if (tjHandler_ == nullptr) {
        IMAGE_LOGE("tjInitCompress failed.");
        err = JpegYuvDecodeError_CompressFailed;
        return;
    }

    int32_t ret = tjCompressFromYUV(tjHandler_, i420Buf.get(), yuvWidth, COMP_PAD, yuvHeight,
        COMP_SUBSAMP, &jpegBuffer_, jpegBufferSize, COMP_JPEGQUAL, COMP_FLAGS);
    if (ret != 0) {
        IMAGE_LOGE("tjCompressFromYUV failed with error: %{public}s", tjGetErrorStr2(tjHandler_));
        err = JpegYuvDecodeError_CompressFailed;
        return;
    }

    *jpegBuffer = jpegBuffer_;
    err = JpegYuvEncodeError_Success;
}

JpegEncoderYuv::~JpegEncoderYuv()
{
    if (jpegBuffer_ != nullptr) {
        tjFree(jpegBuffer_);
    }
    if (tjHandler_ != nullptr) {
        tjDestroy(tjHandler_);
    }
    if (dlHandler_ != nullptr) {
        dlclose(dlHandler_);
    }
}

JpegYuvEncodeError JpegEncoderYuv::DoConvert(uint32_t nvWidth, uint32_t nvHeight,
    Media::PixelFormat nvPixFormat, const uint8_t* nvBuffer, uint8_t* i420Buffer)
{
    int32_t halfWidth = (nvWidth + 1) >> 1;
    int32_t halfHeight = (nvHeight + 1) >> 1;
    int32_t y_stride = nvWidth;
    int32_t u_stride = halfWidth;
    int32_t v_stride = halfWidth;
    int32_t uv_stride = u_stride + v_stride;
    const uint8_t* y_src = nvBuffer;
    const uint8_t* uv_src = nvBuffer + nvWidth * nvHeight;
    uint8_t* y_dst = i420Buffer;
    uint8_t* u_dst = y_dst + nvWidth * nvHeight;
    uint8_t* v_dst = u_dst + halfWidth * halfHeight;
    CONVERT_FUNC funcConvert = nullptr;

    switch (nvPixFormat) {
        case Media::PixelFormat::NV12:
            if (dlHandler_ != nullptr) {
                funcConvert = (CONVERT_FUNC)dlsym(dlHandler_, "NV12ToI420");
            } else {
                funcConvert = JpegEncoderYuv::NV12ToI420;
            }
            break;
        case Media::PixelFormat::NV21:
            if (dlHandler_ != nullptr) {
                funcConvert = (CONVERT_FUNC)dlsym(dlHandler_, "NV21ToI420");
            } else {
                funcConvert = JpegEncoderYuv::NV21ToI420;
            }
            break;
        default:
            IMAGE_LOGE("Unsupport pixel format. format=%{public}d", nvPixFormat);
            return JpegYuvEncodeError_UnsupportPixelFormat;
    }

    if (funcConvert == nullptr) {
        IMAGE_LOGE("Can not load libyuv function.");
        return JpegYuvDecodeError_LoadLibYUVFailed;
    }

    int32_t ret = funcConvert(y_src, y_stride, uv_src, uv_stride,
        y_dst, y_stride, u_dst, u_stride, v_dst, v_stride, nvWidth, nvHeight);
    if (ret != JpegYuvEncodeError_Success) {
        IMAGE_LOGE("Convert from nv to i420 failed.");
        return JpegYuvDecodeError_CompressFailed;
    }
    return JpegYuvEncodeError_Success;
}

int32_t JpegEncoderYuv::NV12ToI420(
    const uint8_t* src_y, int32_t src_stride_y,
    const uint8_t* src_uv, int32_t src_stride_uv,
    uint8_t* dst_y, int32_t dst_stride_y,
    uint8_t* dst_u, int32_t dst_stride_u,
    uint8_t* dst_v, int32_t dst_stride_v,
    int32_t width, int32_t height)
{
    int32_t halfWidth = (width + 1) >> 1;
    int32_t halfHeight = (height + 1) >> 1;

    if (dst_y) {
        for (int32_t i = 0; i < height; ++i) {
            memcpy_s(dst_y, width, src_y, width);
            src_y += src_stride_y;
            dst_y += dst_stride_y;
        }
    }

    for (int32_t y = 0; y < halfHeight; ++y) {
        SplitUVRow(src_uv, dst_u, dst_v, halfWidth);
        dst_u += dst_stride_u;
        dst_v += dst_stride_v;
        src_uv += src_stride_uv;
    }

    return JpegYuvEncodeError_Success;
}

void JpegEncoderYuv::SplitUVRow(const uint8_t* src_uv, uint8_t* dst_u, uint8_t* dst_v, int32_t width)
{
    int32_t halfWidth = (width + 1) >> 1;
    for (int32_t x = 0; x < halfWidth; x++) {
        *dst_u++ = *src_uv++;
        *dst_v++ = *src_uv++;
        *dst_u++ = *src_uv++;
        *dst_v++ = *src_uv++;
    }

    if (width & 1) {
        dst_u[width - 1] = src_uv[0];
        dst_v[width - 1] = src_uv[1];
    }
}
}
}