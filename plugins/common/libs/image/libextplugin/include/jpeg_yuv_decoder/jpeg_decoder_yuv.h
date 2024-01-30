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

#ifndef PLUGINS_COMMON_LIBS_IMAGE_LIBEXTPLUGIN_INCLUDE_JPEG_DECODER_YUV_H
#define PLUGINS_COMMON_LIBS_IMAGE_LIBEXTPLUGIN_INCLUDE_JPEG_DECODER_YUV_H

#include <string>
#include "jpeg_yuvdata_converter.h"
#include "abs_image_decoder.h"
#include "turbojpeg.h"
#include "jpeglib.h"

namespace OHOS {
namespace ImagePlugin {

typedef int (*FUNC_I4xxToI420)(YUVPlaneInfo& src, YUVPlaneInfo& dest);
typedef int (*FUNC_I4xxToNV21)(YUVPlaneInfo& src, YUVPlaneInfo& dest);

struct ConverterPair {
    FUNC_I4xxToI420 to420Func = nullptr;
    FUNC_I4xxToNV21 toNV21Func = nullptr;
};

enum class JpegYUVFmt {
    OutFmt_YU12 = 1,
    OutFmt_YV12,
    OutFmt_NV12,
    OutFmt_NV21,
};

enum JpegYUVDecodeError {
    JpegYUVDecodeError_Unknown = -1,
    JpegYUVDecodeError_Success = 0,
    JpegYUVDecodeError_InvalidParameter,
    JpegYUVDecodeError_DecodeFailed,
    JpegYUVDecodeError_BadImage,
    JpegYUVDecodeError_SubSampleNotSupport,
    JpegYUVDecodeError_MemoryMallocFailed,
    JpegYUVDecodeError_MemoryNotEnoughToSaveResult,
    JpegYUVDecodeError_ConvertError,
};

struct JpegDecoderYUVParameter {
    uint32_t jpgwidth_ = 0;
    uint32_t jpgheight_ = 0;
    const uint8_t *jpegBuffer_ = nullptr;
    uint32_t jpegBufferSize_ = 0;
    uint8_t *yuvBuffer_ = nullptr;
    uint32_t yuvBufferSize_ = 0;
    JpegYUVFmt outfmt_ = JpegYUVFmt::OutFmt_YU12;
    uint32_t outwidth_ = 0;
    uint32_t outheight_ = 0;
};

struct JpegScaleFactor {
    uint32_t num;
    uint32_t denom;
};

class JpegDecoderYUV {
public:
    JpegDecoderYUV() = default;
    int DoDecode(DecodeContext &context, JpegDecoderYUVParameter &decodeParameter);
    static bool GetScaledSize(uint32_t jpgwidth, uint32_t jpgheight, uint32_t &width, uint32_t &height);
    static uint32_t GetYUVOutSize(uint32_t width, uint32_t height);

protected:
    static void InitPlaneOutInfoTo420(uint32_t width, uint32_t height, YUVPlaneInfo &info);
    static void InitPlaneOutInfoTo420NV(uint32_t width, uint32_t height, YUVPlaneInfo &info);
    static void FillJpgOutYUVInfo(YUVPlaneInfo& info, uint32_t width, uint32_t height, uint8_t* data, int samp);
    static uint32_t Get420OutPlaneWidth(YUVComponentIndex com, int imageWidth);
    static uint32_t Get420OutPlaneHeight(YUVComponentIndex com, int imageHeight);
    static uint32_t Get420OutPlaneSize(YUVComponentIndex com, int imageWidth, int imageHeight);
    static uint32_t GetJpegDecompressedYUVSize(uint32_t width, uint32_t height, int subsample);
    static void InitYUVDataOutInfoTo420(uint32_t width, uint32_t height, PlYUVDataInfo &info);
    static void InitYUVDataOutInfoTo420NV(uint32_t width, uint32_t height, PlYUVDataInfo &info);
    static void InitYUVDataOutInfo(uint32_t width, uint32_t height, PlYUVDataInfo &info);
    static bool IsYU12YV12Format(JpegYUVFmt fmt);
    static tjscalingfactor GetScaledFactor(uint32_t jpgwidth, uint32_t jpgheight, uint32_t width, uint32_t height);
    static void JpegCalculateOutputSize(uint32_t jpgwidth, uint32_t jpgheight, uint32_t& width, uint32_t& height);
    bool IsSupportedSubSample(int jpegSubsamp);
    bool IsOutSizeValid(uint32_t outwidth, uint32_t outheight);
    bool CanFastDecodeFrom420to420(uint32_t width, uint32_t height, uint32_t jpgYUVSizeOut, int subsamp);
    int DecodeHeader(tjhandle dehandle, int& retSubsamp);
    int DoDecodeToYUVPlane(DecodeContext &context, tjhandle dehandle, uint32_t outw, uint32_t outh);
    int DecodeFrom420To420(DecodeContext &context, tjhandle dehandle, uint32_t width, uint32_t height);
    bool ValidateParameter(YUVPlaneInfo &srcPlaneInfo, ConverterPair &converter);
    int ConvertFrom4xx(YUVPlaneInfo &srcPlaneInfo, ConverterPair &converter);
    int ConvertFromGray(YUVPlaneInfo &srcPlaneInfo);

protected:
    JpegDecoderYUVParameter decodeParameter_;
};

}
}
#endif