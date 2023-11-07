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
#include "ext_pixel_convert.h"
#include "ext_wstream.h"
#include "image_type_converter.h"
#include "image_utils.h"
#include "media_errors.h"
#include "string_ex.h"
#if !defined(IOS_PLATFORM) && !defined(A_PLATFORM)
#include "surface_buffer.h"
#endif
#include "astcenc.h"
#include <iostream>
#include <string>
#include <sstream>
#include <chrono>

namespace OHOS {
namespace ImagePlugin {
using namespace OHOS::HiviewDFX;
using namespace Media;
namespace {
    constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {LOG_CORE, LOG_TAG_DOMAIN_ID_PLUGIN, "ExtEncoder"};
}

#define TEXTURE_HEAD_BYTES (16)
static const uint32_t ASTC_MAGIC_ID = 0x5CA1AB13;

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
            HiLog::Error(LABEL, "ExtEncoder::BuildSkBitmap pixel convert failed %{public}d", res);
            return res;
        }
        pixels = holder.buf.get();
        skInfo = skInfo.makeColorType(SkColorType::kRGBA_8888_SkColorType);
    }

    uint64_t rowStride = skInfo.minRowBytes64();
    if (pixelMap->GetAllocatorType() == Media::AllocatorType::DMA_ALLOC) {
        SurfaceBuffer* sbBuffer = reinterpret_cast<SurfaceBuffer*> (pixelMap->GetFd());
        rowStride = sbBuffer->GetStride();
    }
    if (!bitmap.installPixels(skInfo, pixels, rowStride)) {
        HiLog::Error(LABEL, "ExtEncoder::BuildSkBitmap to skbitmap failed");
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
    if (IsAstc(opts_.format)) {
        return ASTCEncoder();
    }
    auto iter = std::find_if(FORMAT_NAME.begin(), FORMAT_NAME.end(),
        [this](const std::map<SkEncodedImageFormat, std::string>::value_type item) {
            return IsSameTextStr(item.second, opts_.format);
    });
    if (iter == FORMAT_NAME.end()) {
        HiLog::Error(LABEL, "ExtEncoder::FinalizeEncode unsupported format %{public}s", opts_.format.c_str());
        return ERR_IMAGE_INVALID_PARAMETER;
    }
    auto encodeFormat = iter->first;
    SkBitmap bitmap;
    TmpBufferHolder holder;
    auto errorCode = BuildSkBitmap(pixelmap_, bitmap, encodeFormat, holder);
    if (errorCode != SUCCESS) {
        HiLog::Error(LABEL, "ExtEncoder::FinalizeEncode BuildSkBitmap failed");
        return errorCode;
    }
    ExtWStream wStream(output_);
    if (!SkEncodeImage(&wStream, bitmap, iter->first, opts_.quality)) {
        HiLog::Error(LABEL, "ExtEncoder::FinalizeEncode encode failed");
        return ERR_IMAGE_ENCODE_FAILED;
    }
    return SUCCESS;
}

// test ASTCEncoder
void GenAstcHeader(astc_header &hdr, astcenc_image img, TextureEncodeOptions *encodeParams)
{
    hdr.magic[0] = ASTC_MAGIC_ID & 0xFF;
    hdr.magic[1] = (ASTC_MAGIC_ID >> 8) & 0xFF;
    hdr.magic[2] = (ASTC_MAGIC_ID >> 16) & 0xFF;
    hdr.magic[3] = (ASTC_MAGIC_ID >> 24) & 0xFF;

    hdr.block_x = static_cast<uint8_t>(encodeParams->blockX_);
    hdr.block_y = static_cast<uint8_t>(encodeParams->blockY_);
    hdr.block_z = 1;

    hdr.dim_x[0] = img.dim_x & 0xFF;
    hdr.dim_x[1] = (img.dim_x >> 8) & 0xFF;
    hdr.dim_x[2] = (img.dim_x >> 16) & 0xFF;

    hdr.dim_y[0] = img.dim_x & 0xFF;
    hdr.dim_y[1] = (img.dim_x >> 8) & 0xFF;
    hdr.dim_y[2] = (img.dim_x >> 16) & 0xFF;

    hdr.dim_z[0] = img.dim_x & 0xFF;
    hdr.dim_z[1] = (img.dim_x >> 8) & 0xFF;
    hdr.dim_z[2] = (img.dim_x >> 16) & 0xFF;
}

int InitAstcencConfig(astcenc_profile profile, TextureEncodeOptions *option, astcenc_config& config)
{
    unsigned int block_x = option->blockX_;
    unsigned int block_y = option->blockY_;
    unsigned int block_z = 1;

    float quality = ASTCENC_PRE_FAST;
    unsigned int flags = 0;
    astcenc_error status = astcenc_config_init(profile, block_x, block_y,
        block_z, quality, flags, &config);
    if (status == ASTCENC_ERR_BAD_BLOCK_SIZE)
    {
        HiLog::Debug(LABEL_TEST, "ERROR: block size is invalid");
        return 1;
    }
    else if (status == ASTCENC_ERR_BAD_CPU_FLOAT)
    {
        HiLog::Debug(LABEL_TEST, "ERROR: astcenc must not be compiled with fast-math");
        return 1;
    }
    else if (status != ASTCENC_SUCCESS)
    {
        HiLog::Debug(LABEL_TEST, "ERROR: config failed");
        return 1;
    }
    return 0;
}

void extractDimensions(std::string &format, TextureEncodeOptions &param)
{
    std::size_t slashPos = format.find('/');
    if (slashPos != std::string::npos) {
        std::string dimensions = format.substr(slashPos + 1);
        std::size_t starPos = dimensions.find('*');
        if (starPos != std::string::npos) {
            std::string widthStr = dimensions.substr(0, starPos);
            std::string heightStr = dimensions.substr(starPos + 1);

            param.blockX_ = static_cast<uint8_t>(std::stoi(widthStr));
            param.blockY_ = static_cast<uint8_t>(std::stoi(heightStr));
        }
    }
}

uint32_t ExtEncoder::ASTCEncoder()
{
    ImageInfo imageInfo;
    pixelmap_->GetImageInfo(imageInfo);
    TextureEncodeOptions param;
    param.width_ = imageInfo.size.width;
    param.height_ = imageInfo.size.height;
    extractDimensions(opts_.format, param);

    size_t requestSize;
    output_->GetCapicity(requestSize);
    uint8_t *inRGBA = (uint8_t*)malloc(requestSize);
    pixelmap_->readPixels(requestSize, inRGBA);


    // int fileSize = pixelmap_->GetByteCount();
    // uint8_t *inRGBA = (uint8_t*)malloc(fileSize);
    // pixelmap_->readPixels(fileSize, inRGBA);

    AstcEncoder work;
    InitAstcencConfig(work.profile, &param, work.config);
    astcenc_context_alloc(&work.config, 1, &work.codec_context);
    work.swizzle_ = {ASTCENC_SWZ_R, ASTCENC_SWZ_G, ASTCENC_SWZ_B, ASTCENC_SWZ_A};
    work.image_.dim_x = param.width_;
    work.image_.dim_y = param.height_;
    work.image_.dim_z = 1;
    work.image_.data_type = ASTCENC_TYPE_U8;
    work.image_.data = (void **)malloc(sizeof(void*) * work.image_.dim_z);
    GenAstcHeader(work.head, work.image_, &param);

    work.image_.data[0] = inRGBA;
    int outSize = ((param.width_ + param.blockX_ - 1) / param.blockX_) *
        ((param.height_ + param.blockY_ -1) / param.blockY_) * TEXTURE_HEAD_BYTES + TEXTURE_HEAD_BYTES;

    memcpy_s(output_->GetAddr(), sizeof(astc_header), &work.head, sizeof(astc_header));
    work.data_out_ = output_->GetAddr() + TEXTURE_HEAD_BYTES;
    work.data_len_ = outSize - TEXTURE_HEAD_BYTES;
    work.error_ = astcenc_compress_image(work.codec_context, &work.image_, &work.swizzle_, work.data_out_, work.data_len_, 0);
    if (ASTCENC_SUCCESS != work.error_) {
        HiLog::Error(LABEL_TEST, "astc compress failed");
        return ERROR;
    }
    output_->SetOffSet(outSize);
    return SUCCESS;
}
} // namespace ImagePlugin
} // namespace OHOS
