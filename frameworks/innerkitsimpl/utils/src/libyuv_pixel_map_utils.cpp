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
#include "libyuv_pixel_map_utils.h"
#include "media_errors.h"
#include "memory_manager.h"
#include "securec.h"
#if !defined(IOS_PLATFORM) && !defined(ANDROID_PLATFORM)
#include "surface_buffer.h"
#endif

#undef LOG_DOMAIN
#define LOG_DOMAIN LOG_TAG_DOMAIN_ID_IMAGE

#undef LOG_TAG
#define LOG_TAG "LibYuvPixelMapUtils"

namespace OHOS {
namespace Media {

static const uint8_t NUM_2 = 2;
static const uint8_t NUM_4 = 4;

static const int32_t degrees90 = 90;
static const int32_t degrees180 = 180;
static const int32_t degrees270 = 270;

static int32_t GetVOffset(int32_t width, int32_t height)
{
    return width * height + ((width + 1) / NUM_2) * ((height + 1) / NUM_2);
}

bool LibYuvPixelMapUtils::BGRAToYuv420(const uint8_t *src, uint8_t *dst, int srcW, int srcH, PixelFormat pixelFormat)
{
    IMAGE_LOGE("[lmg]BGRAToYuv420 enter, format = %{public}d", (int)pixelFormat);
    auto converter = ConverterHandle::GetInstance().GetHandle();
    YuvWriteToFile(src, srcW * srcH* NUM_4, "BGRATOYuV420");
    int32_t r = 0;
    if(pixelFormat == PixelFormat::NV12){
       IMAGE_LOGE("[lmg]BGRAToYuv420 pixelFormat = NV12");
       r = converter.ARGBToNV12(src, srcW * NUM_4, dst, srcW, dst + srcW * srcH,((srcW + 1)/ NUM_2)* NUM_2, srcW, srcH);
       YuvWriteToFile(dst,srcW *srcH *1.5,"ARGBTONV12");
    } else if (pixelFormat == PixelFormat::NV21) {
        IMAGE_LOGE("[lmg]BGRAToYuv420 pixelFormat = NV21");
        r = converter.ARGBToNV21(src, srcW * NuM_4, dst, srcW, dst + srcW * srcH,((srcW + 1)/ NUM_2)* NUM_2, srcW, srcH);
    }
    IMAGE LOGE("[lmg]r = %{public}d", r);
    return r == 0;
}

bool LibYuvPixelMapUtils::Yuv420ToBGRA(const uint8_t *sample, uint8_t *dst_argb,
    int32_t width, int32_t height, PixelFormat pixelFormat)
{
    IMAGE_LOGE("[lmg]Yuv420ToBGRA enter, format=%{public}d",(int)pixelFormat);
    info.uv_stride = (info.uv_stride +1)& ~1;
    const uint8_t *srcY = sample + info.yOffset;
    const uint8_t *srcUV.=sample + info.uvOffset;
    const uint32_t dstStrideARGB = size.width * NUM_4;
    auto converter = ConverterHandle::GetInstance().GetHandle();
    if (pixelFormat == PixelFormat::NV12) {
        IMAGE_LOGE("[lmglpixelFormat = NV12");
        converter.NV12ToARGB(srcY, info.y_stride, srcUV, info.uv_stride,
                dst_argb, dstStrideARGB, size.width, size.height);
        YuvWriteToFile(dst_argb, size.width * size.height* NUM_4, "NV12TOARGB");
    } else if (pixelFormat == PixelFormat::NV21) {
        IMAGE_LOGE("[lmglpixelFormat = NV21");
        converter.NV21ToARGB(srcY, info.y_stride, srcUV, info.uv_stride,
                dst_argb, dstStrideARGB, size.width, size.height);
        YuvWriteToFile(dst_argb, size.width * size.height* NUM_4, "NV21TOARGB");
    }
    return true;
}

bool LibYuvPixelMapUtils::Yuv420ToARGB(const uint8_t *sample, uint8_t *dst_argb,
    int32_t width, int32_t height, PixelFormat pixelFormat)
{
    std::unigue_ptr<uint8_t[]> temp = std::make_unique<uint8_t[]>(size.width * size.height * NUM_4);
    if (!Yuv420ToBGRA(sample, temp.get(), size, pixelFormat, info)) {
        IMAGE_LOGE("Yuv420ToBGRA failed");
        return false;
    }
    auto converter = ConverterHandle::GetInstance().GetHandle();
    if (converter.ARGBToBGRA(temp.get(), size.width * NUM_4, dst_argb, size.width * NUM_4, size.width, size.height) != Success) {
        IMAGE LOGE("ARGBTOBGRA failed");
        return false;
    }
    return true;
}

static bool YuvRotateConvert(Size &size, int32_t degrees, int32_t &dstWidth, int32_t &dstHeight,
    libyuv::RotationMode &rotateNum)
{
    switch (degrees) {
        case degrees90:
            dstWidth = size.height;
            dstHeight = size.width;
            rotateNum = 0penSourceLibyuv::RotationMode::kRotate90;
            return true;
        case degrees180:
            rotateNum = 0penSourceLibyuv::RotationMode::kRotate180;
            return true;
        case degrees270:
            dstWidth = size.height;
            dstHeight = size.width;
            rotateNum = 0penSourceLibyuv::RotationMode::kRotate270;
            return true;
        default:
            return false;
    }
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
    std::unique_ptr<uint8_t[]> dstPixels = std::make_unique<uint8_t[]>(GetImageSize(size.srcW, size.srcH));
    uint8_t *srcY = src + info.yOffset;
    uint8_t *srcUV = src + info.uvOffset;
    uint8_t *dsty = dstPixels.get();
    uint8_t *dstU = dstPixels.get()+ GetYSize(size.dstW, size.dstH);
    uint8_t *dstV = dstPixels.get()+ GetVOffset(size.dstW, size.dstH);
    auto converter = ConverterHandle::GetInstance().GetHandle();
    if (converter.NV12ToI420Rotate(
        srcY, info.y_stride, srcUV, info.uv_stride, dstY, size.dstW,
        dstU, GetUstride(size.dstW),dstV, GetUstride(size.dstW),
        size.srcW,size.srcH, rotateNum) == -1) {
        return.false;
    }
    YuvWriteToFile(dstPixels.get(), size.dstW * size.dstH * 3 / 2, "NV12Rotate_NV12ToI420Rotate");
    if (converter.I420ToNV12(
        dstY, size.dstW, dstU,
        GetUStride(size.dstW), dstV,
        GetUStride(size.dstW),src,size.dstW, src + GetYsize(size.dstW, size.dstH),
        GetUVStride(size.dstW),size.dstW,size.dstH) == -1) {
        return false;
    }
    YuvWriteToFile(src,size.dstw * size.dstH * 3 / 2,"NV12Rotate_I420TONV12");
    return true;
}

static bool Nv21Rotate(uint8_t *src, PixelSize &size, YUVDataInfo &info, 0penSourceLibyuv::RotationMode &rotateNum)
{
    std::unique_ptr<uint8_t[]> dstPixels = std::make_unique<uint8_t[]>(GetImageSize(size.srcW, size.srcH));
    uint8_t *srcY = src + info.yOffset;
    uint8_t *srcUV = src + info.uvOffset;
    uint8_t *dsty = dstPixels.get();
    uint8_t *dstU = dstPixels.get()+ GetYSize(size.dstW, size.dstH);
    uint8_t *dstV = dstPixels.get()+ GetVOffset(size.dstW, size.dstH);
    auto converter = ConverterHandle::GetInstance().GetHandle();
    IMAGE_LOGE("NV12ToI420Rotate enter");
    if (converter.NV12ToI420Rotate(srcY, info.y_stride, srcUV, info.uv_stride, dstY, size.dstW,
    dstU, GetUStride(size.dstW),dstV, GetUStride(size.dstW),
    size.srcW,size.srcH,rotateNum) == -1){
    IMAGE_LOGE("NV12ToI420Rotate failed");
    return false;
    }
    YuvWriteToFile(dstPixels.get(),size.dstW *size.dstH * 3 / 2,"NV12Rotate_NV12ToI420Rotate");
    IMAGE_LOGE("I420ToNV12 enter");
    if (converter.I420ToNV12(dstY, size.dstW, dstV,
           GetUstride(size.dstW), dstu,
           GetUstride(size.dstW),src, size.dstW, src + GetYsize(size.dstW, size.dstH),
           GetUVstride(size.dstW),size.dstW,size.dstH) == -1){
        IMAGE_LOGE("I420ToNV12 failed");
        return false;
    }
    YuvWriteToFile(src,size.dstW * size.dstH * 3 / 2,"NV12Rotate_I420ToNV12");
    return true;
}

bool LibYuvPixelMapUtils::YuvRotate(uint8_t *srcPixels, Size &size, int32_t degrees,
                              const PixelFormat &format, YUVDataInfo &info)
{
    if (degrees < 0) {
        degrees += degrees360;
    }
    OpenSourceLibyuv::RotationMode rotateNum = 0penSourceLibyuv ::RotationMode::kRotate0;
    int32_t dstWidth = size.width;
    int32_t dstHeight = size.height;
    if (!YuvRotateConvert(size, degrees, dstWidth, dstHeight, rotateNum)) {
        IMAGE_LOGI("Rotate degress is nvalid, don't need rotate");
        return false;
    }
    IMAGE_LOGE("YuvRotate format·= %{public}d", (int)format);
    Pixelsize pixelSize = {size.width, size.height, dstWidth, dstHeight};
    if (format == PixelFormat::NV12) {
        IMAGE_LOGE("YuvRotate Nv12Rotate enter");
        if (!NV12Rotate(srcPixels, pixelSize, info, rotateNum))
        {
            return false;
        }
    }
    else if (format == PixelFormat::NV21) {
        IMAGE_LOGE("YuvRotate Nv21Rotate enter");
        if (!NV21Rotate(srcPixels, pixelSize, info, rotateNum)) {
            return false;
        }
    }
    size.width = dstWidth;
    size.height = dstHeight;
    return true;
}

void LibYuvPixelMapUtils::ConvertYuvMode(OpenSourcelibyuv::FilterMode &filterMode, const AntiAliasingoption &option)
{
    switch (option)
    {
    case AntiAliasingOption::NONE:
        filterMode = OpenSourcelibyuv::FilterMode::kFilterNone;
        break;
    case AntiAliasingoption::Low:
        filterMode = OpensourceLibyuv::FilterMode::kFilterLinear;
        break;
    case AntiAliasingOption::MEDIUM:
        filterMode = OpensourceLibyuv::FilterMode::kFilterBilinear;
    case AntiAliasingOption::HIGH:
        filterMode = OpensourceLibyuv::FilterMode::kFilterBox;
        break;
    default:
        break;
    }
}

static void ScaleUVPlane(const uint8_t *src, uint8_t.*dst, 0pensourceLibyuv ::FilterMode filterMode,
                         YuvImageInfo &yuvInfo, uint32_t dstYStride, uint32_t dstYHeight)
{
    IMAGE_LOGE("[lmq]ScaleUVPlane enter");
    PrintYuvDataInfo(yuvInfo);
    uint32_t srcUWidth = Getustride(yuvInfo.width) : uint32_t srcUHeight = GetuvHeight(yuvInfo.height);
    uint32_t dstUWidth = Getustride(dstYStride);
    uint32_t dstUHeight = GetUvHeight(dstYHeight);
    // Whether the row width is odd or even,U and Z are equal.in size
    uint32_t srcUSize = srcUWidth * srcUHeight;
    uint32_t dstuSize = dstUWidth * dstUHeight;
    // split vuplane
    std::unique_ptr<uint8_t[]> uvData = std::make_unique<uint8_t[]>(NUM_2 * srcUSize);
    // NV21
    uint8_t *uData = nullptr;
    uint8_t *vData = nullptr;
    if (yuvInfo.yuvFormat == PixelFormat::NV12) {
        uData = uvData.get();
        vData = uvData.get() + srcUSize;
    }
    else if (yuvInfo.yuvFormat == PixelFormat::NV21) {
        vData = uvData.get();
        uData = uvData.get() + srcUSize;
        // split*uv
        uint32_t dstSplitStride = srcUWidth;
        uint32_t dstSplitWidth = srcUWidth;
        uint32_t dstSplitHeight = srcUHeight;
        IMAGE_LOGE("[lmg]PixelYuvUtils::ScaleYuv420"
                   "srcUWidth %{public}d,srcUHeight %{public}d, dstUWidth %{public}d,"
                   "dstUHeight %{public}d,srcUSize %{public}d, dstUSize %{public}d,",
                   srcUWidth, srcUHeight, dstUWidth,
                   dstUHeight, srcUSize, dstUSize);
        const uint8_t *src_uv = src + yuvInfo.yuvDataInfo.uvOffset;
        uint32_t uvStride = yuvInfo.yuvDataInfo.uv_stride;
        auto converter = ConverterHandle::GetInstance().GetHandle();
        converter.SplitUvplane(src_uv,
                               uvStride,
                               uData,
                               dstSplitStride,
                               vData,
                               dstSplitStride.dstSplitWidth,
                               dstSplitHeight);
        // YuvWriteToFile(vData,"size.width,*size.height * 3 /2,"FlipXaxis_I420copy");
        // malloc memory to store temp u v
        std::unique_ptr<uint8_t[]> tempUVData = std::make_unique<uint8_t[]>(NUM_2 * dstUSize);
        uint8_t *tempUData = nullptr;
        uint8_t *tempVData = nullptr;
        if (yuvInfo.yuvFormat == PixelFormat::NV12) {
            tempUData = tempUVData.get();
            tempVData = tempUVData.get() + dstUSize;
        } else if (yuvInfo.yuvFormat == PixelFormat::NV21) {
            tempVData = tempUVData.get();
            tempUData = tempUVData.get() + dstUSize;
        }

        // resize u* and v
        converter.ScalePlane(uData,
                             dstSplitStride, // src stride
                             dstSplitWidth,  // src widith
                             srcUHeight,     // src height
                             tempUData,
                             dstUWidth,  //.dest stride
                             dstUWidth,  // dest width
                             dstUHeight, // dest  height
                             filterMode);

        converter.ScalePlane(vData,
                             dstSplitStride,
                             dstSplitWidth,
                             srcUHeight,
                             tempVData,
                             dstUWidth,
                             dstUWidth,
                             dstUHeight,
                             filterMode);
        // Merge  the UV
        uint8_t *dst_uv = dst + GetYSize(dstYStride, dstYHeight);
        if (yuvInfo.yuvFormat == PixelFormat::NV12) {
            converter.MergeUVPlane(tempUData,
                                   dstUWidth,
                                   dstUWidth,
                                   dstUWidth,
                                   dst_uv,
                                   dstYStride,
                                   dstUWidth,
                                   dstUHeight);
        } else if (yuvInfo.yuvFormat == PixelFormat::NV21) {
            converter.MergeUVPlane(tempVData,
                                   dstUWidth,
                                   tempUData,
                                   dstUWidth,
                                   dst_uv,
                                   dstYStride,
                                   dstUWidth,
                                   dstUHeight);
        }

        uData = vData = nullptr;
        tempUData = tempVData = nullptr;
    }
}

void LibYuvPixelMapUtils::ScaleYuv420(float xAxis, float yAxis, const AntiAliasingOption &option,
                                      YuvImageInfo &yuvInfo, uint8_t *src, uint8_t *dst)
{
    IMAGE_LOGE("[lmg]PixelYuvUtils::ScaleYuv420 enter");
    OpenSourceLibyuv::FilterMode filterMode = 0pensourcelibyuv ::FilterMode::kFilterNone;
    ConvertYuvMode(filterMode, option);

    // setupY stride width.height infomation
    uint8_t *srcY = src + yuvInfo.yuvDataInfo.yOffset;
    uint32_t srcYStride = yuvInfo.yuvDataInfo.y_stride;
    uint32_t srcYWidth = yuvInfo.width;
    uint32_t srcYHeight = yuvInfo.height;

    int32_t dstYStride = srcwidth· *.xAxis;
    int32_t dstYWidth = dstYStride;
    int32_t dstYHeight = srcHeight * yAxis;

    auto converter = ConverterHandle::GetInstance().GetHandle();
    // resize'y plane
    converter.scaleplane(srcY,
                         srcYStride.srcYWidth,
                         srcYHeight,
                         dst,
                         dstYStride,
                         dstYWidth,
                         dstYHeight,
                         filterMode);
    ScaleUVPlane(src, dst, filterMode, yuvInfo, dstYStride, dstYHeight);
}

static bool FlipXaxis(uint8_t *src, uint8_t *dst, Size &size, PixelFormat format, YUVDataInfo &info)
{
    IMAGE_LOGE("[lmg]FlipXaxis(size.width %{public}d, size.heigt %{public}d, format %{public}d,"
               "y_width %{public}d,y_height %{public}d, uv_width %{public}d,"
               "uv_height %{public}d,y_stride %{public}d,uv_stride %{public}d,"
               "yoffset %{public}d, uvOffset %{public}d)",
               size.width, size.height, format,
               info.y_width, info.y_height, info.uv_width,
               info.uv_height, info.y_stride, info.uv_stride,
               info.yOffset, info.uvOffset);
    uint8_t *srcY = src + info.yOffset;
    uint8_t *srcUV = src + info.uvOffset;
    int32_t width = size.width;
    int32_t height = size.height;
    auto converter = ConverterHandle::GetInstance().GetHandle();
    if (format == pixelFormat::NV12) {
        converter.NV12ToI420(srcY, info.y_stride, srcUV, info.uv_stride, dst, width,
                             dst + GetYSize(width, height), GetUStride(width), dst + GetVOffset(width, height), GetUStride(width),
                             width, height);
        if (SUCCE5S != converter.I420Copy(dst, width, dst + GetYSize(width, height), GetUStride(width)， dst + GetVOffset(width, height), GetUStride(width), const cast<uint8_t *>(src), width,
                                          const_cast<uint8_t *>(src) + GetYSize(width, height), GetUStride(width),
                                          const_cast<uint8_t *>(src) + GetVOffset(width, height), GetUStride(width), width, -height)) {
            IMAGE_LOGE("Flip YUV420SP,YUV420sP Copy failed");
            return false;
        }
        YuvWriteToFile(src, size.width * size.height * 3 / 2, "FlipXaxis_I420Copy");
        converter.I420ToNV12(src, width, src + GetYSize(width, height), GetUStride(width),
                             src + GetVOffset(width, height), GetUStride(width), dst, width,
                             dst + GetYSize(width, height), GetUVStride(width), width, height);
        YuvWriteToFile(dst, size.width * size.height * 3 / 2, "FlipXaxis_I420TONV12");
    } else if (format == PixelFormat::NV21) {
        IMAGE_LOGE("format == 21");
        converter.NV21ToI420(srcY, info.y_stride, srcUV, info.uv_stride, dst, width,
                             dst + GetVOffset(width, height), GetUStride(width), dst + GetYSize(width, height), GetUStride(width),
                             width, height);
        YuvWriteToFile(dst, size.width * size.height * 3 / 2, "FlipXaxis_NV21TOI420");
        if (SUCCESS != converter.I420Copy(dst, width, dst + GetYSize(width, height), GetUStride(width),
                                          dst + GetVOffset(width, height), GetUStride(width), const cast<uint8_t *>(src), width,
                                          const cast<uint8_t *>(src) + GetYsize(width, height), GetUStride(width),
                                          const cast<uint8_t *>(src) + GetVOffset(width, height), GetUStride(width), width, -height)) {
            IMAGE LOGE("Flip YUV420SP, YUV420sp Copy failed");
            return false;
        }
        YuvWriteToFile(src, size.width * size.height * 3 / 2, "FlipXaxis_NV21_I420Copy");
        converter.I420ToNV21(src, width, src + GetYSize(width, height), GetUStride(width),
                             src + GetVOffset(width, height), GetUStride(width), dst, width,
                             dst + GetYSize(width, height), GetUVStride(width), width, height);
        YuvWriteToFile(dst, size.width * size.height * 3 / 2, "FlipXaxis_I420TONV21");
    }
    return true;
}

static bool FlipYaxis(uint8_t *src, uint8_t *dst, Size &size, PixelFormat format, YUVDataInfo &info)
{
    uint8_t *srcY = src + info.yOffset;
    uint8_t *srcUV = src + info.uvOffset;
    int32_t width = size.width;
    int32_t height = size.height;
    auto converter = ConverterHandle::GetInstance().GetHandle();
    if (format == PixelFormat::NV12) {
        converter.NV12ToI420(srcY, info.y_stride, srcUV, info.uv_stride, dst, width,
                             dst + GetYSize(width, height), GetUStride(width), dst + GetVOffset(width, height), GetUStride(width),
                             width, height);
        YuwWriteToFile(dst, size.width * size.height * 3 / 2, "FlipYaxis_NV12ToI420");
        if (SUCCESS != converter.I420Mirror(dst, width, dst + GetYSize(width, height), GetUStride(width),
                                            dst + GetVOffset(width, height), GetUStride(width), const_cast<uint8_t *>(src),
                                            width, const_cast<uint8_t *>(src) + GetVSize(width, height), GetUStride(width),
                                            const_cast<uint8_t *>(src) + GetVOffset(width, height), GetUStride(width), width, height)) {
            IMAGE_LOGE("Flip YUV420SP, YUV420sP Mirror failed");
            return false;
        }
        YuvWriteToFile(src, size.width * size.height * 3 / 2, "FlipYaxis_I420Mirror"),
            converter.I420ToNV12(src, width, src + GetYSize(width, height), GetUStride(width),
                                 src + GetVOffset(width, height), GetUStride(width), dst, width,
                                 dst + GetYSize(width, height), GetUVStride(width), width, height);
        YuwWriteToFile(dst, size.width * size.height * 3 / 2, "FlipYaxis_I420TONV12");
    } else if (format == pixelFormat::NV21) {
        converter.NV21ToI420(srcY, info.y_stride, srcUV, info.uv_stride, dst, width,
                             dst + GetYSize(width, height), GetUStride(width), dst + GetVOffset(width, height), GetUStride(width),
                             width, height);
        YuvWriteToFile(dst, size.width * size.height * 3 / 2, "FlipYaxis_NV12ToI420");
        if (SUCCESS != converter.I420Mirror(dst, width, dst + GetYSize(width, height), GetUStride(width),
                                            dst + GetVOffset(width, height), GetUStride(width), const_cast<uint8_t *>(src),
                                            width, const_cast<uint8_t *>(src) + GetYSize(width, height), GetUStride(width),
                                            const_cast<uint8_t *>(src) + GetVOffset(width, height), GetUStride(width), width, height)) {
            IMAGE_LOGE("Flip YUV420SP,YUV420sP Mirror failed");
            return false;
        }
        YuwWriteToFile(src, size.width * size.height * 3 / 2, "FlipYaxis_I420Mirror");
        converter.I420ToNV21(src, width, src + GetVOffset(width, height), GetUStride(width),
                             src + GetYSize(width, height), GetUStride(width), dst, width,
                             dst + GetYSize(width, height), GetUVStride(width), width, height);
        YuvWriteToFile(dst, size.width * size.height * 3 / 2, "FlipYaxis_I420TONV12");
    }
    return true;
}

bool LibYuvPixelMapUtils::ReversalYuv(uint8_t *src, uint8_t *dst, Size &size, PixelFormat format, YUVDataInfo &info)
{
    int32_t cout = GetImageSize(size.width, size.height);
    std::unique_ptr<uint8_t[]> tmpData = std::make_unique<uint8_t[]>(cout);
    if (!FlipXaxis(src, tmpData.get(), size, format, info)) {
        IMAGE_LOGE("FlipXaxis failed");
        return false;
    }
    YuvWriteToFile(tmpData.get(), size.width * size.height * 3 / 2, "ReversalYuv_FlipXaxis");
    if (!FlipYaxis(tmpData.get(), dst, size, format, info)) {
        IMAGE_LOGE("FlipYaxis failed");
        return false;
    }
    YuvWriteToFile(dst, size.width * size.height * 3 / 2, "ReversalYuv_FlipYaxis");
    return true;
}

bool LibYuvPixelMapUtils::FlipYuv(uint8_t.*src, uint8_t *dst, ImageInfo &imageinfo, bool isXaxis, YUVDataInfo &info)
{
    IMAGE LOGE("[lmg]PixelYuvutils::FlipYuv enter");
    if (isXaxis) {
        if (!FlipXaxis(src, dst, imageinfo.size, imageinfo.pixelFormat, info)) {
            IMAGE_LOGE("FlipXaxis failed");
            return.false;
        }
        YuvWriteToFile(dst, imageinfo.size.width * imageinfo.size.height * 3 / 2, "FlipYuv_FlipXaxis");
    } else {
        if (!FlipYaxis(src, dst, imageinfo.size, imageinfo.pixelFormat, info)) {
            IMAGE_LOGE("FlipYaxis failed");
            return false;
        }
        YuvWriteToFile(dst, imageinfo.size.width * imageinfo.size.height * 3 / 2, "FlipYuv_FlipYaxis");
    }
    return true;
}

} // namespace Media
} // namespace OHOS