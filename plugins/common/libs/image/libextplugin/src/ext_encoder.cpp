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

#include "SkBitmap.h"
#include "SkImageEncoder.h"
#ifdef IMAGE_COLORSPACE_FLAG
#include "color_space.h"
#endif
#if !defined(IOS_PLATFORM) && !defined(A_PLATFORM)
#include "astc_codec.h"
#endif

#include "data_buf.h"
#include "ext_pixel_convert.h"
#include "ext_wstream.h"
#include "image_accessor_factory.h"
#include "image_accessor_interface.h"
#include "image_log.h"
#include "image_type_converter.h"
#include "image_utils.h"
#include "media_errors.h"
#include "string_ex.h"
#if !defined(IOS_PLATFORM) && !defined(A_PLATFORM)
#include "surface_buffer.h"
#endif
#include "tiff_parser.h"

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

static uint32_t BuildSkBitmap(Media::PixelMap *pixelMap, SkBitmap &bitmap,
    SkEncodedImageFormat format, TmpBufferHolder &holder)
{
    uint32_t res = SUCCESS;
    SkImageInfo skInfo = ToSkInfo(pixelMap);
    auto pixels = pixelMap->GetWritablePixels();
    if (format == SkEncodedImageFormat::kJPEG &&
        skInfo.colorType() == SkColorType::kRGB_888x_SkColorType &&
        pixelMap->GetCapacity() < skInfo.computeMinByteSize()) {
        res = RGBToRGBx(pixelMap, skInfo, holder);
        if (res != SUCCESS) {
            IMAGE_LOGE("ExtEncoder::BuildSkBitmap pixel convert failed %{public}d", res);
            return res;
        }
        pixels = holder.buf.get();
        skInfo = skInfo.makeColorType(SkColorType::kRGBA_8888_SkColorType);
    }

    uint64_t rowStride = skInfo.minRowBytes64();

#if !defined(IOS_PLATFORM) && !defined(A_PLATFORM)
    if (pixelMap->GetAllocatorType() == Media::AllocatorType::DMA_ALLOC) {
        SurfaceBuffer* sbBuffer = reinterpret_cast<SurfaceBuffer*> (pixelMap->GetFd());
        rowStride = sbBuffer->GetStride();
    }
#endif

    if (!bitmap.installPixels(skInfo, pixels, rowStride)) {
        IMAGE_LOGE("ExtEncoder::BuildSkBitmap to skbitmap failed");
        return ERR_IMAGE_INVALID_PARAMETER;
    }
    return res;
}

bool IsAstc(const std::string &format)
{
    return format.find("image/astc") == 0;
}

uint32_t ExtEncoder::DoFinalizeEncode()
{
    auto iter = std::find_if(FORMAT_NAME.begin(), FORMAT_NAME.end(),
        [this](const std::map<SkEncodedImageFormat, std::string>::value_type item) {
            return IsSameTextStr(item.second, opts_.format);
    });
    if (iter == FORMAT_NAME.end()) {
        IMAGE_LOGE("ExtEncoder::FinalizeEncode unsupported format %{public}s", opts_.format.c_str());
        return ERR_IMAGE_INVALID_PARAMETER;
    }

    SkBitmap bitmap;
    TmpBufferHolder holder;
    auto errorCode = BuildSkBitmap(pixelmap_, bitmap, iter->first, holder);
    if (errorCode != SUCCESS) {
        IMAGE_LOGE("ExtEncoder::FinalizeEncode BuildSkBitmap failed");
        return errorCode;
    }

    if (pixelmap_->GetExifMetadata() == nullptr ||
        pixelmap_->GetExifMetadata()->GetExifData() == null) {
        ExtWStream wStream(output_);
        if (!SkEncodeImage(&wStream, bitmap, iter->first, opts_.quality)) {
            IMAGE_LOGE("ExtEncoder::FinalizeEncode encode failed");
            return ERR_IMAGE_ENCODE_FAILED;
        }
        return SUCCESS;
    }

    unsigned char *dataPtr;
    uint32_t datSize = 0;
    auto exifData = pixelmap_->GetExifMetadata()->GetExifData();
    TiffParser::Encode(&dataPtr, datSize, exifData);
    DataBuf exifBlob(dataPtr, datSize);
    TempStream tStream;
    if (!SkEncodeImage(&tStream, bitmap, iter->first, opts_.quality)) {
        IMAGE_LOGE("ExtEncoder::FinalizeEncode encode failed");
        return ERR_IMAGE_ENCODE_FAILED;
    }

    auto destImageAccessor = ImageAccessorFactory::Create(tStream.GetAddr(), tStream.bytesWritten());
    if (destImageAccessor != nullptr) {
        if (destImageAccessor->WriteExifBlob(exifBlob) == SUCCESS) {
            if (destImageAccessor->WriteToOutput(*output_)){
                return SUCCESS;
            }
        }
    }
    if (!output_.Write(tStream.GetAddr(), tStream.bytesWritten())) {
        return ERR_IMAGE_ENCODE_FAILED;
    }

    return SUCCESS;
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

    return DoFinalizeEncode();
}
} // namespace ImagePlugin
} // namespace OHOS
