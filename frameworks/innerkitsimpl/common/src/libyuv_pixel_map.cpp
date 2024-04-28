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

#include "libyuv_pixel_map.h"

#include "image_utils.h"
#include "image_trace.h"
#include "image_type_converter.h"
#include "memory_manager.h"
#include "hitrace_meter.h"
#include "libyuv_pixel_map_utils.h"
#include "media_errors.h"
#include "pubdef.h"
#include "pixel_yuv_utils.h"
#include "securec.h"
#include "image_log.h"
#include "image_mdk_common.h"
#include "image_system_properties.h"
#if !defined(IOS_PLATFORM) && !defined(ANDROID_PLATFORM)
#include "surface_buffer.h"
#endif

#undef LOG_DOMAIN
#define LOG_DOMAIN LOG_TAG_DOMAIN_ID_IMAGE

#undef LOG_TAG
#define LOG_TAG "LibYuvPixelMap"

namespace OHOS {
namespace Media {
using namespace std;

static const uint8_t NUM_2 = 2;
constexpr int32_t MAX_DIMENSION = INT32_MAX >> NUM_2;
constexpr int32_t ANTIALIASING_SIZE = 350;
struct TransInfos{
    SkMatrix matrix;
};

struct TransMemoryInfo {
    AllocatorType allocType;
    std::unique_ptr<AbsMemory> memory = nullptr;
};

static SkImageInfo ToSkImageInfo(ImageInfo &info, sk sp<SkColorSpace> colorSpace)
{
    SkColorType colorType = ImageTypeConverter::ToSkColorType(info.pixelFormat);
    SkAlphaType alphaType = ImageTypeconverter::ToSkAlphaType(info.alphaType);
    IMAGE_LOGE("ToSkImageInfo w %{public}d,h %{public}d", info.size.width, info.size.height);
    IMAGE_LOGE(
        "ToskImageInfo pf %{public}s, at %{public}s, skpf %{public}s, skat %{public}s",
        ImageTypeConverter::ToName(info.pixelFormat).c_str(),
        ImageTypeConverter::ToName(info.alphaType).c_str(),
        ImageTypeConverter::ToName(colorType).c_str(),
        ImageTypeConverter::ToName(alphaType).c str());
    return SkImageInfo::Make(info.size.width, info.size.height, colorType, alphaType, colorSpace);
}

static sk_sp<SkColorSpace> ToSkColorSpace(PixelMap *pixelmap)
{
#ifdef IMAGE_COLORSPACE_FLAG
     if (pixelmap->InnerGetGrColorspacePtr() == nullptr){
         return nullptr;
     }
     return pixelmap->InnerGetGrColorspacePtr()->ToSkColorSpace();
#else
     return nullptr;
#endif
}

static bool isSameColorspace(const 0HOS::ColorManager::ColorSpace &src,
    const 0HOS::ColorManager::ColorSpace &dst)
{
    auto skSrc=src.ToSkColorSpace();
    auto skDst= dst.ToSkColorSpace();
    return SkColorSpace::Equals(skSrc.get(),skDst.get());
}

static int32_t GetVOffset(int32_t width, int32_t height)
{
    return width * height + ((width + 1) / NUM_2) * ((height + 1) / NUM_2);
}

void LibYuvPixelMap::scale(float xAxis, float yAxis)
{
    ImageInfo imageInfo;
    GetImageInfo(imageInfo);
    AntiAliasingoption operation = AntiAliasingOption::NONE;
    AntiAliasingoption option = AntiAliasingOption::NONE;
    if (ImageSystemProperties::GetAntiAliasingEnabled() && IsSupportAntiAliasing(imageInfo, option)) {
        operation = AntiAliasingOption::MEDIUM;
    } else {
        operation = option;
    }
    scale(xAxis, yAxis, AntiAliasingOption::NONE);
}

void LibYuvPixelMap::PrintYuvDataInfo()
{
    IMAGE LOGE("[lmg]PixelYuv::PrintYuvDataInfo"
               "yWidth %{public}d,yHeight %{public}d,uvWidth %{public}d,"
               "uvHeight %{public}d,yStride %{public}d, uvStride %{public}d,"
               "yOffset  %{public}d,uvOffset.%{public}d)"
               "imageInfo_.width %{public}d, imageInfo_.height %{public}d" yuvDataInfo_.yWidth,
               yuvDataInfo_.yHeight, yuvDataInfo_.uvWidth,
               yuvDataInfo_.uvHeight, yuvDataInfo_.yStride, yuvDataInfo_.uvStride,
               yuvDataInfo_.yOffset, yuvDataInfo_.uvOffset,
               imaeInfo_.size.width, imageInfo_.size.height);
}

static bool IsSupportAntiAliasing(const ImageInfo& imageInfo, const AntiAliasingOption &option)
{
    return option != AntiAliasingOption::NONE && imageInfo.size.width <= ANTIALIASING_SIZE &&
            imageInfo.size.height <= ANTIALIASING_SIZE;
}

void LibYuvPixelMap::scale(float xAxis, float yAxis, const AntiAliasingOption &option)
{
    IMAGE_LOGE("[lmg]PixelYuv::scale enter");
    ImageTrace imageTrace("PixelMap scale");
    ImageInfo imageInfo;
    GetImageInfo(imageInfo);
    int32_t dstW = imageInfo.size.width  * xAxis;
    int32_t dstH = imageInfo.size.height * yAxis;
    Size desiredSize ={dstW, dstH};
    MemoryData memoryData = {nullptr, GetImageSize(dstW, dstH), "Trans ImageData", desiredSize};
    auto m = MemoryManager::CreateMemory(allocatorType_, memoryData);
    if (m == nullptr){
        IMAGE_LOGE("scale CreateMemory failed");
        return;
#if !defined(IOS PLATFORM)&& !defined(A PLATFORM)
    if(allocatorType ==AllocatorType::DMA ALLOC){
        if(m->extend.data == nullptr){
            IMAGE_LOGE("GendstTransInfo get surfacebuffer failed");
        }
    }
#endif
    uint8_t *dst = reinterpret_cast<uint8_t *>(m->data.data);
    YUVDataInfo yuvDataInfo;
    GetImageYUVInfo(yuvDataInfo);
    YuvImageInfo yuvInfo = { PixelYuvUtils::ConvertFormat(imageInfo.pixelFormat),imageInfo_.size.width,
        imageInfo.size.height, imageInfo_.pixelFormat, yuvDataInfo};
    PrintYuvDataInfo();
    PixelYuvUtils::ScaleYuv420(xAxis, yAxis, option, yuvInfo, data_, dst);
    SetPixelsAddr(reinterpret_cast<void *>(dst), m->extend.data, m->data.size, m->GetType(), nullptr);
    imageInfo.size.width = dstW;
    imageInfo.size.height = dstH;
    SetImageInfo(imageInfo, true);
}

void LibYuvPixelMap::rotate(float degrees)
{
    IMAGE_LOGE("[lmq]PixelYuv::rotate(degrees%{public}d) enter", (int)degrees);
    if(degrees == 0){
        return;
    }
    YUVDataInfo yuvDataInfo;
    GetImageYUVInfo(yuvDataInfo);
    if (!PixelYuvUtils: :YuvRotate(data_, imageInfo_.size, degrees, imageInfo_.pixelFormat, yuvDataInfo)) {
        return;
}
    AssignYuvDataOnType(imageInfo_.pixelFormat, imageInfo_.size.width, imageInfo_.size.height);
    return;
}

bool LibYuvPixelMap::resize(float xAxis, float yAxis)
{
    scale(xAxis,yAxis);
    return true;
}

void LibYuvPixelMap::ScaleYuv420(float xAxis, float yAxis, const AntiAliasingOption &option)
{
    libyuv::FilterMode filterMode = libyuv::FilterMode::kFilterNone;
    LibYuvPixelMapUtils::ConvertYuvMode(filterMode, option);
    ImageInfo imageInfo;
    GetImageInfo(imageInfo);
    int32_t srcW = imageInfo.size.width;
    int32_t srcH = imageInfo.size.height;
    int32_t dstW = srcW * xAxis;
    int32_t dstH = srcH * yAxis;
    const uint8_t *src = data_;
    uint32_t pictureSize = GetImageSize(dstW, dstH);
    Size desiredSize = {dstW, dstH};
    MemoryData memoryData = {nullptr, pictureSize, "ScaleYuv420 ImageData", desiredSize};
    auto dstMemory = MemoryManager::CreateMemory(allocatorType_, memoryData);
    if (dstMemory == nullptr) {
        IMAGE_LOGE("ScaleYuv420 CreateMemory failed");
        return;
    }
    uint8_t *dst = reinterpret_cast<uint8_t *>(dstMemory->data.data);
    if (imageInfo.pixelFormat == PixelFormat::NV12 || imageInfo.pixelFormat == PixelFormat::NV21) {
        uint32_t srcHalfW = GetUStride(srcW);
        uint32_t srcHalfH = GetUVHeight(srcH);
        uint32_t dstHalfW = GetUStride(dstW);
        uint32_t dstHalfH = GetUVHeight(dstH);

        // resize y_plane
        libyuv::ScalePlane(src, srcW, srcW, srcH, dst, dstW, dstW, dstH, filterMode);
        //Whether the row width is odd or even, U and Z are equal in size
        uint32_t srcUSize = GetUStride(srcW) * GetUVHeight(srcH);
        uint32_t dstUSize = GetUStride(dstW) * GetUVHeight(dstH);
        // Split VUplane
        std::unique_ptr<uint8_t[]> uvData = std::make_unique<uint8_t[]>(NUM_2 * srcUSize);

        // NV21
        uint8_t *vData = uvData.get();
        uint8_t *uData = uvData.get() + srcUSize;
        // If it's in NV12 format，swap u and v
        if (imageInfo.pixelFormat == PixelFormat::NV12) {
            uint8_t *tempSwap = vData;
            vData = uData;
            uData = tempSwap;
        }

        const uint8_t *src_uv = src + GetYSize(srcW, srcH);
        libyuv::SplitUVPlane(src_uv, NUM_2 * srcHalfW, vData, srcHalfW, uData, srcHalfW, srcHalfW, srcHalfH);

        // malloc memory to store temp u v
        std::unique_ptr<uint8_t[]> tempUVData = std::make_unique<uint8_t[]>(NUM_2 * srcUSize);
        uint8_t *tempVData = tempUVData.get();
        uint8_t *tempUData = tempUVData.get() + dstUSize;
        if (imageInfo.pixelFormat == PixelFormat::NV12) {
            uint8_t *tempSwap = tempVData;
            tempVData = tempUData;
            tempUData = tempSwap;
        }

        // resize u and v
        libyuv::ScalePlane(uData, srcHalfW, srcHalfW, srcHalfH, tempUData, dstHalfW, dstHalfW, dstHalfH, filterMode);
        libyuv::ScalePlane(vData, srcHalfW, srcHalfW, srcHalfH, tempVData, dstHalfW, dstHalfW, dstHalfH, filterMode);

        uint8_t *dst_uv = dst + GetYSize(dstW, dstH);
        libyuv::MergeUVPlane(tempVData, dstHalfW, tempUData, dstHalfW, dst_uv, NUM_2 * dstHalfW, dstHalfW, dstHalfH);

        uData = vData = nullptr;
        tempUData = tempVData = nullptr;

        SetPixelsAddr(reinterpret_cast<void *>(dst), dstMemory->extend.data,
            dstMemory->data.size, dstMemory->GetType(), nullptr);
        imageInfo.size.width = dstW;
        imageInfo.size.height = dstH;
        SetImageInfo(imageInfo, true);
    }
}

bool LibYuvPixelMap::FlipXaxis(const uint8_t *src, uint8_t *dst, int32_t width, int32_t height, PixelFormat format)
{
    if (format == PixelFormat::NV21 || format == PixelFormat::NV12) {
        libyuv::NV12ToI420(src, width, src + GetYSize(width, height), GetUVStride(width),
            dst, width, dst + GetYSize(width, height), GetUStride(width),
            dst + GetVOffset(width, height), GetUStride(width), width, height);
        if (SUCCESS != libyuv::I420Copy(dst, width, dst + GetYSize(width, height), GetUStride(width),
            dst + GetVOffset(width, height), GetUStride(width), const_cast<uint8_t *>(src), width,
            const_cast<uint8_t *>(src) + GetYSize(width, height), GetUStride(width),
            const_cast<uint8_t *>(src) + GetVOffset(width, height), GetUStride(width), width, -height)) {
            IMAGE_LOGE("Flip Yuv420SP Copy failed");
            return false;
        }
        libyuv::I420ToNV12(src, width, src + GetYSize(width, height), GetUStride(width),
            src + GetVOffset(width, height), GetUStride(width), dst, width,
            dst + GetYSize(width, height), GetUVStride(width), width, height);
    }
    return true;
}

void LibYuvPixelMap::FlipYaxis(const uint8_t *src, uint8_t *dst, int32_t width, int32_t height, PixelFormat format)
{
    if (format == PixelFormat::NV21 || format == PixelFormat::NV12) {
        libyuv::NV12ToI420(src, width, src + GetYSize(width, height), GetUVStride(width), dst, width,
            dst + GetYSize(width, height), GetUStride(width), dst + GetVOffset(width, height), GetUStride(width),
            width, height);
        libyuv::I420Mirror(dst, width, dst + GetYSize(width, height), GetUStride(width),
            dst + GetVOffset(width, height), GetUStride(width), const_cast<uint8_t *>(src),
            width, const_cast<uint8_t *>(src) + GetYSize(width, height), GetUStride(width),
            const_cast<uint8_t *>(src) + GetVOffset(width, height), GetUStride(width), width, height);
        libyuv::I420ToNV12(src, width, src + GetYSize(width, height), GetUStride(width),
            src + GetVOffset(width, height), GetUStride(width), dst, width,
            dst + GetYSize(width, height), GetUVStride(width), width, height);
    }
}

void LibYuvPixelMap::flip(bool xAxis, bool yAxis)
{
    IMAGE_LOGE("[lmq]PixelYuv::flip(%{public}d, %{public}d) enter", xAxis, yAxis);
    IMAGE_LOGE("[lmg]PixelYuv::flip("
               "yWidth %{public}d,yHeight %{public}d, uvWidth %{public}d,"
               "uvHeight %{public}d,yStride %{public}d, uvStride %{public}d,"
               "yOffset %{public}d,uvOffset %{public}d)",
               yuvDataInfo_.yWidth, yuvDataInfo_.yHeight, yuvDataInfo_.uvwidth,
               yuvDataInfo_.uvHeight, yuvDataInfo_.yStride, yuvDataInfo_.uvstride,
               yuvDataInfo_.yOffset, yuvDataInfo_.uvOffset);

    if (xAxis == false && yAxis == false) {
        return;
    }
    ImageInfo imageInfo;
    GetImageInfo(imageInfo);
    uint8_t *dst = nullptr;
    int32_t width = imageInfo.size.width;
    int32_t height = imageInfo.size.height;
    uint32_t pictureSize = GetImagesize(width, height);
    MemoryData memoryData = {nullptr, pictureSize, "flip ImageData", {width, height}};
    auto m = MemoryManager::CreateMemory(allocatorType_, memoryData);
    if (m == nullptr) {
        IMAGE_LOGE("flip CreateMemory failed");
        return;
    }
#if !defined(IOS_PLATFORM) && !defined(ANDROID_PLATFORM)
    if (allocatorType_ == AllocatorType::DMA_ALLOC) {
        if (m->extend.data == nullptr) {
            IMAGE_LOGE("GendstTransInfo get surfacebuffer failed");
#endif
            dst = reinterpret cast<uint8 t *>(m->data.data);
            YUVDataInfo yuvDataInfo;
            GetImageYUVInfo(yuvDataInfo);
            if (xAxis && yAxis) {
                if (!PixelYuvUtils::ReversalYuv(data_, dst, imageInfo.size, imageInfo.pixelformat, yuvDataInfo)) {
                    IMAGE_LOGE("ReversalYuv failed");
                    m->Release();
                    return;
                }
            } else {
                bool isXaxis = ((xAxis | yAxis) && xAxis) ? true : false;
                if (!PixelYuvUtils::FlipYuv(data_, dst, imageInfo, isXaxis, yuvDataInfo))
                {
                    IMAGE_LOGE("FlipYuv failed");
                    m->Release();
                    return;
                }
                SetPixelsAddr(dst, m->extend.data, m->data.size, m->GetType(), nullptr);
                AssignYuvDataOnType(imageInfo.pixelFormat, width, height);
            }
        }
    }
}

#ifdef IMAGE_COLORSPACE_FLAG
static void ToImageInfo(ImageInfo &info, SkImageInfo &skInfo, bool sizeOnly = true)
{
    info.size.width = skInfo.width();
    info.size.height = skInfo.height();
    if (!sizeonly) {
        info.alphaType = ImageTypeConverter::ToAlphaType(skInfo.alphaType());
        info.pixelFormat = ImageTypeConverter::ToPixelFormat(skInfo.colorType());
    }
}

bool LibYuvPixelMap::CheckColorSpace(const 0HOS::ColorManager::ColorSpace &grColorSpace)
{
    auto grName = grColorSpace.GetColorSpaceName();
    if (grColorSpace != nullptr && isSameColorSpace(*grColorSpace_,grColorSpace)){
        if (grColorSpace_->GetColorSpaceName()!=grName) {
            InnerSetColorSpace(grColorSpace);
            IMAGE_LOGE("applyColorSpace inner set");
        }
        IMAGE_LOGE("[lmg]CheckColorSpace true");
        return true;
    }
    return false;
}

int32_t LibYuvPixelMap::ColorSpaceBGRAToYuv(uint8_t * bgraData, SkTransYuvInfo & dst, ImageInfo & imageInfo,
                                            PixelFormat & format, const 0HOS ::ColorManager::ColorSpace &grColorSpace)
    IMAGE_LOGE("[lmg]ColorSpaceBGRAToYuv enter");
    int32_t dstWidth = dst.info.width();
    int32_t dstHeight = dst.info.height();
    uint32_t pictureSize = GetImagesize(dstWidth, dstHeight);
    std::unique_ptr<uint8_t[]> yuvData = std::make_unique<uint8_t[]>(pictureSize),
    if (!PixelYuvUtils::BGRAToYuv420(bgraData, yuvData.get(),
        dstWidth, dstHeight, format)) {
        IMAGE_LOGE("[lmg]BGRAToYuv420 failed");
        return ERR_IMAGE_COLOR_CONVERT;
    }
    dst.info = ToSkImageInfo(imageInfo, grColorSpace.ToSkColorSpace());
    ToImageInfo(imageInfo, dst.info);
    auto *grName = grColorSpace.GetColorSpaceName();
    grColorSpace_ = std : : make_shared<0HOS ::ColorManager::ColorSpace>(dst.info.refColorSpace(), grName);
    SetPixelsAddr(yuvData.get(), nullptr, pictureSize, allocatorType_, nullptr);
    return SUCCESS;
}

bool LibYuvPixelMap::CheckColorSpace(const 0HOS ::ColorManager::ColorSpace &grColorSpace)
{
    auto grName = grColorSpace.GetColorSpaceName();
    if (grColorSpace_ != nullptr && isSameColorSpace(*grColorSpace_, grColorSpace)) {
        if (grColorSpace_->GetColorSpaceName() != grName) {
            InnerSetColorSpace(grColorSpace);
            IMAGE_LOGI("applyColorSpace inner set");
        }
        return true;
    }
    return false;
}
uint32_t LibYuvPixelMap::ApplyColorSpace(const 0HOS::colorManager::ColorSpace &grColorSpace)
{
    if (CheckColorSpace(grColorSpace)) {
        return SUCCESS:
        /*convert yuV420 to·BRGA */
        PixelFormat format= imageInfo_.pixelFormat;
        YUVDataInfo yuvDataInfo;
        GetImageYUVInfo(yuvDataInfo);

        int32_t width= imageInfo_.size.width;
        int32_t height= imageInfo_.size.height;
        std::unique_ptr<uint8_t[]> RGBAdata = std::make_unique<uint8_t[]>(width * height * NuM_4);
        if(!PixelYuvUtils::Yuv420ToBGRA(srcData, RGBAdata.get(),tmpInfo.size, format, yuvDataInfo)){
            IMAGE_LOGE("[lmg]Yuv420ToBGRA failed");
            return ERR_IMAGE_COLOR_CONVERT;
        }
        IMAGE_LOGE("applyColorSpace Yuv420ToBGRA sucess");
        ImageInfo bgraImageInfo=imageInfo ;
        bgraImageInfo.pixelFormat=PixelFormat::BGRA_8888;

        SkTransYuvInfo src;
        src.info = ToSkImageInfo(bgraImageInfo, ToskColorspace(this));
        uint64_t rowStride= src.info.minRowBytes();
        IMAGE_LOGE("applyColorSpace rowStride:%{public}d sucess",rowStride);

        auto bret =src.bitmap.installPixels(src.info, RGBAdata.get(), rowStride);
        if (bret == false)
            IMAGE_LOGE("src.bitmap.installPixels failed");
        return -1;
    }
    //Build sk target infomation
    SkTransYuvInfo dst;

    dst.info =ToSkImageInfo(bgraImageInfo,grColorSpace.ToskColorSpace());
    std::unique_ptr<uint8_t[]> RGBAdatac = std::make_unique<uint8_t[]>(width * height * NuM_4);
    //Transfor pixels*by readPixels
    if (!src.bitmap.readPixels(dst.info, RGBAdatac.get(),rowStride, 0, 0)){
        IMAGE_LOGE("ReadPixels failed")
        return ERR IMAGE COLOR CONVERT;
    }
    //convert bgra back to·yuv
    if (ColorSpaceBGRAToYuv(RGBAdatac.get(),dst, imageInfo_, format,grColorSpace)!= SUCCESS){
        IMAGE_LOGE("ColorSpaceBGRAToYuv failed");
        return ERR_IMAGE_COLOR_CONVERT;
    }
    SetImageInfo(imageInfo_,true);
    return SUCCESS;
}
#endif
} // namespace Media
} // namespace OHOS