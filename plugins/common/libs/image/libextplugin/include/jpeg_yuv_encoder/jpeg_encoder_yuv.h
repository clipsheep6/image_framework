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

#ifndef PLUGINS_COMMON_LIBS_IMAGE_LIBEXTPLUGIN_INCLUDE_JPEG_ENCODER_YUV_H
#define PLUGINS_COMMON_LIBS_IMAGE_LIBEXTPLUGIN_INCLUDE_JPEG_ENCODER_YUV_H

#include "image_type.h"

namespace OHOS {
namespace ImagePlugin {

enum JpegYuvEncodeError {
    JpegYuvEncodeError_Success = 0,
    JpegYuvEncodeError_BadParameter,
    JpegYuvEncodeError_UnsupportPixelFormat,
    JpegYuvEncodeError_MemoryAllocFailed,
    JpegYuvDecodeError_CompressFailed,
    JpegYuvDecodeError_LoadLibYUVFailed,
};

class JpegEncoderYuv {
public:
    explicit JpegEncoderYuv(uint32_t yuvWidth, uint32_t yuvHeight,
        Media::PixelFormat yuvPixFormat,
        const uint8_t* yuvBuffer, uint32_t yuvBufferSize,
        uint8_t** jpegBuffer, unsigned long* jpegBufferSize,
        JpegYuvEncodeError& err);
    virtual ~JpegEncoderYuv();

protected:
    JpegYuvEncodeError DoConvert(uint32_t nvWidth, uint32_t nvHeight,
        Media::PixelFormat nvPixFormat, const uint8_t* nvBuffer, uint8_t* i420Buffer);
    static int32_t NV12ToI420(
        const uint8_t* src_y, int32_t src_stride_y,
        const uint8_t* src_uv, int32_t src_stride_uv,
        uint8_t* dst_y, int32_t dst_stride_y,
        uint8_t* dst_u, int32_t dst_stride_u,
        uint8_t* dst_v, int32_t dst_stride_v,
        int32_t width, int32_t height);
    static int32_t NV21ToI420(
        const uint8_t* src_y, int32_t src_stride_y,
        const uint8_t* src_uv, int32_t src_stride_uv,
        uint8_t* dst_y, int32_t dst_stride_y,
        uint8_t* dst_u, int32_t dst_stride_u,
        uint8_t* dst_v, int32_t dst_stride_v,
        int32_t width, int32_t height)
    {
        return NV12ToI420(src_y, src_stride_y, src_uv, src_stride_uv,
            dst_y, dst_stride_y, dst_v, dst_stride_v, dst_u, dst_stride_u,
            width, height);
    }
    static void SplitUVRow(const uint8_t* src_uv, uint8_t* dst_u, uint8_t* dst_v, int32_t width);

protected:
    void* dlHandler_;
    void* tjHandler_;
    uint8_t* jpegBuffer_;
};

}
}
#endif // PLUGINS_COMMON_LIBS_IMAGE_LIBEXTPLUGIN_INCLUDE_JPEG_ENCODER_YUV_H