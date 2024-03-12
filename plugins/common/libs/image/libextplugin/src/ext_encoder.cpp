/*
 * Copyright (C) 2023 Huawei Device Co., Ltd.
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

#include "ext_encoder.h"
#include <algorithm>
#include <map>
#include "pixel_map.h"

#include "SkBitmap.h"
#include "SkImageEncoder.h"
#ifdef IMAGE_COLORSPACE_FLAG
#include "color_space.h"
#endif
#if !defined(IOS_PLATFORM) && !defined(A_PLATFORM)
#include "astc_codec.h"
#endif
#include "ext_pixel_convert.h"
#include "ext_wstream.h"
#include "image_log.h"
#include "image_type_converter.h"
#include "image_utils.h"
#include "media_errors.h"
#include "string_ex.h"
#if !defined(IOS_PLATFORM) && !defined(A_PLATFORM)
#include "surface_buffer.h"
#endif

#undef LOG_DOMAIN
#define LOG_DOMAIN LOG_TAG_DOMAIN_ID_PLUGIN

#undef LOG_TAG
#define LOG_TAG "ExtEncoder"

namespace OHOS {
namespace ImagePlugin {
using namespace Media;

static const std::map<SkEncodedImageFormat, std::string> FORMAT_NAME = {
    {SkEncodedImageFormat::kBMP, "image/bmp"},
    {SkEncodedImageFormat::kGIF, "image/gif"},
    {SkEncodedImageFormat::kICO, "image/ico"},
    {SkEncodedImageFormat::kJPEG, "image/jpeg"},
    {SkEncodedImageFormat::kPNG, "image/png"},
    {SkEncodedImageFormat::kWBMP, "image/bmp"},
    {SkEncodedImageFormat::kWEBP, "image/webp"},
    {SkEncodedImageFormat::kPKM, ""},
    {SkEncodedImageFormat::kKTX, ""},
    {SkEncodedImageFormat::kASTC, ""},
    {SkEncodedImageFormat::kDNG, ""},
    {SkEncodedImageFormat::kHEIF, "image/heif"},
};

static const uint8_t NUM_3 = 3;
static const uint8_t NUM_4 = 4;

ExtEncoder::ExtEncoder()
{
}

ExtEncoder::~ExtEncoder()
{
}

uint32_t ExtEncoder::StartEncode(OutputDataStream &outputStream, PlEncodeOptions &option)
{
    output_ = &outputStream;
    opts_ = option;
    return SUCCESS;
}

uint32_t ExtEncoder::AddImage(Media::PixelMap &pixelMap)
{
    pixelmap_ = &pixelMap;
    return SUCCESS;
}

struct TmpBufferHolder {
    std::unique_ptr<uint8_t[]> buf = nullptr;
};

struct ImageData {
    uint8_t *dst;
    uint8_t *pixels;
    ImageInfo info;
};

static SkImageInfo ToSkInfo(Media::PixelMap *pixelMap)
{
    ImageInfo info;
    pixelMap->GetImageInfo(info);
    SkColorType colorType = ImageTypeConverter::ToSkColorType(info.pixelFormat);
    SkAlphaType alphaType = ImageTypeConverter::ToSkAlphaType(info.alphaType);
    sk_sp<SkColorSpace> colorSpace = nullptr;
#ifdef IMAGE_COLORSPACE_FLAG
    if (pixelMap->InnerGetGrColorSpacePtr() != nullptr) {
        colorSpace = pixelMap->InnerGetGrColorSpacePtr()->ToSkColorSpace();
    }
#endif
    return SkImageInfo::Make(info.size.width, info.size.height, colorType, alphaType, colorSpace);
}

static uint32_t RGBToRGBx(Media::PixelMap *pixelMap, SkImageInfo &skInfo, TmpBufferHolder &holder)
{
    holder.buf = std::make_unique<uint8_t[]>(skInfo.computeMinByteSize());
    ExtPixels src = {
        static_cast<uint8_t*>(pixelMap->GetWritablePixels()),
        pixelMap->GetCapacity(), pixelMap->GetWidth()*pixelMap->GetHeight(),
    };
    ExtPixels dst = {
        holder.buf.get(), skInfo.computeMinByteSize(), skInfo.width()*skInfo.height(),
    };
    return ExtPixelConvert::RGBToRGBx(src, dst);
}

static bool IsYUVImage(PixelFormat format)
{
    if (format == PixelFormat::NV21 || format == PixelFormat::NV12) {
        return true;
    }
    return false;
}

static uint32_t pixelToSkInfo(ImageData &image, SkImageInfo &skInfo, Media::PixelMap *pixelMap,
    TmpBufferHolder &holder, SkEncodedImageFormat format)
{
    uint32_t res = SUCCESS;
    uint32_t width  = image.info.size.width;
    uint32_t height = image.info.size.height;
    uint8_t *srcData = static_cast<uint8_t*>(pixelMap->GetWritablePixels());
    if (IsYUVImage(image.info.pixelFormat)) {
        if (!pixelMap->YUV420ToRGB888(srcData, image.dst, width, height, image.info.pixelFormat)) {
            IMAGE_LOGE("ExtEncoder::BuildSkBitmap Support YUV format RGB convert failed ");
            return ERR_IMAGE_ENCODE_FAILED;
        }
        holder.buf = std::make_unique<uint8_t[]>(width * height * NUM_4);
        SkAlphaType alphaType = ImageTypeConverter::ToSkAlphaType(AlphaType::IMAGE_ALPHA_TYPE_UNKNOWN);
        skInfo = SkImageInfo::Make(width, height, SkColorType::kRGBA_8888_SkColorType, alphaType, nullptr);
        ExtPixels src = {
            image.dst, width * height * NUM_3, width * height * NUM_3,
        };
        ExtPixels dst = {
            holder.buf.get(), width * height * NUM_4, width * height * NUM_4,
        };
        res = ExtPixelConvert::RGBToRGBx(src, dst);
        if (res != SUCCESS) {
            IMAGE_LOGE("ExtEncoder::BuildSkBitmap Support YUV format RGB convert failed %{public}d", res);
            return res;
        }
        image.pixels = holder.buf.get();
    } else {
        skInfo = ToSkInfo(pixelMap);
        image.pixels = static_cast<uint8_t*>(pixelMap->GetWritablePixels());
        if (format == SkEncodedImageFormat::kJPEG &&
            skInfo.colorType() == SkColorType::kRGB_888x_SkColorType &&
            pixelMap->GetCapacity() < skInfo.computeMinByteSize()) {
            res = RGBToRGBx(pixelMap, skInfo, holder);
            if (res != SUCCESS) {
                IMAGE_LOGE("ExtEncoder::BuildSkBitmap RGB convert failed %{public}d", res);
                return res;
            }
            image.pixels = holder.buf.get();
            skInfo = skInfo.makeColorType(SkColorType::kRGBA_8888_SkColorType);
        }
    }
    return SUCCESS;
}

static uint32_t BuildSkBitmap(Media::PixelMap *pixelMap, SkBitmap &bitmap,
    SkEncodedImageFormat format, TmpBufferHolder &holder)
{
    uint32_t res = SUCCESS;
    SkImageInfo skInfo;
    ImageData imageData;
    pixelMap->GetImageInfo(imageData.info);
    uint32_t width  = imageData.info.size.width;
    uint32_t height = imageData.info.size.height;
    std::unique_ptr<uint8_t[]> dstData = std::make_unique<uint8_t[]>(width * height * NUM_3);
    imageData.dst = dstData.get();
    if (pixelToSkInfo(imageData, skInfo, pixelMap, holder, format) != SUCCESS) {
        IMAGE_LOGE("ExtEncoder::BuildSkBitmap pixel convert failed");
        return ERR_IMAGE_ENCODE_FAILED;
    }
    uint64_t rowStride = skInfo.minRowBytes64();

#if !defined(IOS_PLATFORM) && !defined(A_PLATFORM)
    if (pixelMap->GetAllocatorType() == Media::AllocatorType::DMA_ALLOC) {
        SurfaceBuffer* sbBuffer = reinterpret_cast<SurfaceBuffer*> (pixelMap->GetFd());
        rowStride = sbBuffer->GetStride();
    }
#endif
    if (!bitmap.installPixels(skInfo, imageData.pixels, rowStride)) {
        IMAGE_LOGE("ExtEncoder::BuildSkBitmap to skbitmap failed");
        return ERR_IMAGE_INVALID_PARAMETER;
    }
    return res;
}

bool IsAstc(const std::string &format)
{
    return format.find("image/astc") == 0;
}

uint32_t ExtEncoder::FinalizeEncode()
{
    if (pixelmap_ == nullptr || output_ == nullptr) {
        return ERR_IMAGE_INVALID_PARAMETER;
    }
#if !defined(IOS_PLATFORM) && !defined(A_PLATFORM)
    if (IsAstc(opts_.format)) {
        AstcCodec astcEncoder;
        astcEncoder.SetAstcEncode(output_, opts_, pixelmap_);
        return astcEncoder.ASTCEncode();
    }
#endif
    auto iter = std::find_if(FORMAT_NAME.begin(), FORMAT_NAME.end(),
        [this](const std::map<SkEncodedImageFormat, std::string>::value_type item) {
            return IsSameTextStr(item.second, opts_.format);
    });
    if (iter == FORMAT_NAME.end()) {
        IMAGE_LOGE("ExtEncoder::FinalizeEncode unsupported format %{public}s", opts_.format.c_str());
        return ERR_IMAGE_INVALID_PARAMETER;
    }
    auto encodeFormat = iter->first;
    SkBitmap bitmap;
    TmpBufferHolder holder;
    auto errorCode = BuildSkBitmap(pixelmap_, bitmap, encodeFormat, holder);
    if (errorCode != SUCCESS) {
        IMAGE_LOGE("ExtEncoder::FinalizeEncode BuildSkBitmap failed");
        return errorCode;
    }
    ExtWStream wStream(output_);
    if (!SkEncodeImage(&wStream, bitmap, iter->first, opts_.quality)) {
        IMAGE_LOGE("ExtEncoder::FinalizeEncode encode failed");
        return ERR_IMAGE_ENCODE_FAILED;
    }
    return SUCCESS;
}
} // namespace ImagePlugin
} // namespace OHOS
