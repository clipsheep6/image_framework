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

#include "auxiliary_generator.h"
#include "abs_image_decoder.h"
#include "image_log.h"
#include "image_utils.h"
#include "securec.h"
#include "surface_buffer.h"
#ifdef EXT_PIXEL
#include "pixel_yuv_ext.h"
#endif
#include "pixel_map.h"
#include "pixel_yuv.h"
#include "hdr_type.h"
#include "metadata.h"
#include "metadata_accessor.h"
#include "metadata_accessor_factory.h"
#include "image_source.h"

namespace OHOS {
namespace Media {
using namespace ImagePlugin;

static const std::string IMAGE_ENCODE_FORMAT_UNDEFINED = "undefined";
static const std::string IMAGE_EXTENDED_CODEC = "image/extended";
static const uint32_t FIRST_FRAME = 0;

static inline bool IsSizeVailed(const Size &size)
{
    return (size.width != 0 && size.height != 0);
}

// Interface for Jpeg/Heif
ImageInfo AuxiliaryGenerator::MakeImageInfo(
    int width, int height, PixelFormat format, AlphaType alphaType, ColorSpace colorSpace)
{
    ImageInfo info;
    info.size.width = width;
    info.size.height = height;
    info.pixelFormat = format;
    info.alphaType = alphaType;
    info.colorSpace = colorSpace;
    return info;
}

// Interface for Jpeg/Heif
AuxiliaryPictureInfo AuxiliaryGenerator::MakeAuxiliaryPictureInfo(
    AuxiliaryPictureType type, const Size &size, int32_t rowStride, PixelFormat format, ColorSpace colorSpace)
{
    AuxiliaryPictureInfo info;
    info.auxiliaryPictureType = type;
    info.size.width = size.width;
    info.size.height = size.height;
    info.rowStride = rowStride;
    info.pixelFormat = format;
    info.colorSpace = colorSpace;
    return info;
}

// Interface for Jpeg/Heif
std::shared_ptr<PixelMap> AuxiliaryGenerator::CreatePixelMapByContext(DecodeContext &context,
    std::unique_ptr<AbsImageDecoder> &decoder, uint32_t &errorCode)
{
    std::shared_ptr<PixelMap> pixelMap;
    if (ImageSource::IsYuvFormat(context.pixelFormat)) {
#ifdef EXT_PIXEL
        pixelMap = std::make_shared<PixelYuvExt>();
#else
        pixelMap = std::make_shared<PixelYuv>();
#endif
    } else {
        pixelMap = std::make_shared<PixelMap>();
    }
    if (pixelMap == nullptr) {
        errorCode = ERR_IMAGE_ADD_PIXEL_MAP_FAILED;
        return nullptr;
    }
    ImageInfo imageinfo = MakeImageInfo(context.outInfo.size.width, context.outInfo.size.height,
                                        context.pixelFormat, context.outInfo.alphaType, context.colorSpace);
    pixelMap->SetImageInfo(imageinfo, true);
    PixelMapAddrInfos addrInfos;
    ImageSource::ContextToAddrInfos(context, addrInfos);
    pixelMap->SetPixelsAddr(addrInfos.addr, addrInfos.context, addrInfos.size, addrInfos.type, addrInfos.func);
#ifdef IMAGE_COLORSPACE_FLAG
    if (context.hdrType > ImageHdrType::SDR) {
        pixelMap->InnerSetColorSpace(ColorManager::ColorSpace(context.grColorSpaceName));
    } else if (decoder->IsSupportICCProfile()) {
        pixelMap->InnerSetColorSpace(decoder->getGrColorSpace());
    }
#endif
    return pixelMap;
}

// Interface for Jpeg/Heif
uint32_t AuxiliaryGenerator::DecodeHdrMetadata(std::unique_ptr<AbsImageDecoder> &extDecoder,
    std::unique_ptr<AuxiliaryPicture> &auxPicture)
{
    ImageHdrType hdrType = extDecoder->CheckHdrType();
    std::shared_ptr<HdrMetadata> hdrMetadata = std::make_shared<HdrMetadata>(extDecoder->GetHdrMetadata(hdrType));
    std::shared_ptr<PixelMap> pixelMap = auxPicture->GetContentPixel();
    if (pixelMap == nullptr) {
        IMAGE_LOGE("Get invalid content pixel map for hdr metadata");
        return ERR_IMAGE_GET_DATA_ABNORMAL;
    }
    pixelMap->SetHdrMetadata(hdrMetadata);
    pixelMap->SetHdrType(hdrType);
    return SUCCESS;
}

// Interface for Jpeg/Heif
uint32_t AuxiliaryGenerator::DecodeFragmentMetadata(std::unique_ptr<AbsImageDecoder> &extDecoder,
    std::unique_ptr<AuxiliaryPicture> &auxPicture)
{
    // TODO: 1.水印metadata的解析依赖实际数据格式（外部）；2.FragmentMetadata PR尚未合入（内部）
    std::shared_ptr<ImageMetadata> fragmentMetadata = nullptr;
    if (fragmentMetadata == nullptr) {
        IMAGE_LOGE("Decode fragment metadata failed!");
        return ERR_IMAGE_DECODE_ABNORMAL;
    }
    auxPicture->SetMetadata(MetadataType::FRAGMENT, fragmentMetadata);
    return SUCCESS;
}

uint32_t AuxiliaryGenerator::DecodeHeifMetadata(std::unique_ptr<AbsImageDecoder> &extDecoder,
    AuxiliaryPictureType type, std::unique_ptr<AuxiliaryPicture> &auxPicture)
{
    uint32_t errorCode;
    switch (type) {
        case AuxiliaryPictureType::GAINMAP: {
            errorCode = DecodeHdrMetadata(extDecoder, auxPicture);
            if (errorCode != SUCCESS) {
                IMAGE_LOGE("Decode heif hdr metadata failed! errorCode: %{public}d", errorCode);
            }
            break;
        }
        case AuxiliaryPictureType::FRAGMENT_MAP: {
            errorCode = DecodeFragmentMetadata(extDecoder, auxPicture);
            if (errorCode != SUCCESS) {
                IMAGE_LOGE("Decode heif fragment metadata failed! errorCode: %{public}d", errorCode);
            }
            break;
        }
        default: {
            errorCode = ERR_MEDIA_DATA_UNSUPPORT;
            IMAGE_LOGE("Unsupport heif auxiliary picture type is %{public}d", type);
            break;
        }
    }
    return errorCode;
}

// Interface for Jpeg
AbsImageDecoder* AuxiliaryGenerator::DoCreateDecoder(std::string codecFormat, PluginServer &pluginServer,
    InputDataStream &sourceData, uint32_t &errorCode) __attribute__((no_sanitize("cfi")))
{
    std::map<std::string, AttrData> capabilities = {{IMAGE_ENCODE_FORMAT, AttrData(codecFormat)}};
    for (const auto &capability : capabilities) {
        std::string x(IMAGE_ENCODE_FORMAT_UNDEFINED);
        capability.second.GetValue(x);
        IMAGE_LOGD("Capabilities [%{public}s],[%{public}s]", capability.first.c_str(), x.c_str());
    }
    auto decoder = pluginServer.CreateObject<AbsImageDecoder>(AbsImageDecoder::SERVICE_DEFAULT, capabilities);
    if (decoder == nullptr) {
        IMAGE_LOGE("Failed to create decoder object!");
        errorCode = ERR_IMAGE_PLUGIN_CREATE_FAILED;
        return nullptr;
    }
    errorCode = SUCCESS;
    decoder->SetSource(sourceData);
    return decoder;
}

uint32_t AuxiliaryGenerator::SetJpegAuxiliaryDecodeOption(std::unique_ptr<AbsImageDecoder> &decoder,
    PlImageInfo &plInfo)
{
    Size size;
    uint32_t errorCode = decoder->GetImageSize(FIRST_FRAME, size);
    if (errorCode != SUCCESS || !IsSizeVailed(size)) {
        return ERR_IMAGE_DATA_ABNORMAL;
    }
    PixelDecodeOptions plOptions;
    plOptions.desiredSize = size;
    plOptions.desiredPixelFormat = PixelFormat::RGBA_8888;
    errorCode = decoder->SetDecodeOptions(FIRST_FRAME, plOptions, plInfo);
    return errorCode;
}

uint32_t AuxiliaryGenerator::DoJpegDecode(std::unique_ptr<AbsImageDecoder> &decoder, DecodeContext &context)
{
    if (decoder == nullptr) {
        return IMAGE_RESULT_CREATE_DECODER_FAILED;
    }
    if (SetJpegAuxiliaryDecodeOption(decoder, context.info) != SUCCESS) {
        return ERR_IMAGE_DATA_ABNORMAL;
    }
    context.allocatorType = AllocatorType::DMA_ALLOC;
    if (decoder->Decode(FIRST_FRAME, context) != SUCCESS) {
        FreeContextBuffer(context.freeFunc, context.allocatorType, context.pixelsBuffer);
        return ERR_IMAGE_DECODE_ABNORMAL;
    }
    return SUCCESS;
}

// Interface for Jpeg
void AuxiliaryGenerator::FreeContextBuffer(const Media::CustomFreePixelMap &func, AllocatorType allocType,
    PlImageBuffer &buffer)
{
    if (func != nullptr) {
        func(buffer.buffer, buffer.context, buffer.bufferSize);
        return;
    }

#if !defined(_WIN32) && !defined(_APPLE) && !defined(IOS_PLATFORM) && !defined(ANDROID_PLATFORM)
    if (allocType == AllocatorType::SHARE_MEM_ALLOC) {
        int *fd = static_cast<int *>(buffer.context);
        if (buffer.buffer != nullptr) {
            ::munmap(buffer.buffer, buffer.bufferSize);
        }
        if (fd != nullptr) {
            ::close(*fd);
        }
        return;
    } else if (allocType == AllocatorType::DMA_ALLOC) {
        if (buffer.buffer != nullptr) {
            ImageUtils::SurfaceBuffer_Unreference(static_cast<SurfaceBuffer *>(buffer.context));
            buffer.context = nullptr;
        }
    } else if (allocType == AllocatorType::HEAP_ALLOC) {
        if (buffer.buffer != nullptr) {
            free(buffer.buffer);
            buffer.buffer = nullptr;
        }
    }
#else
    if (buffer.buffer != nullptr) {
        free(buffer.buffer);
        buffer.buffer = nullptr;
    }
#endif
}

std::shared_ptr<AuxiliaryPicture> AuxiliaryGenerator::GenerateHeifAuxiliaryPicture(
    std::unique_ptr<AbsImageDecoder> &extDecoder, AuxiliaryPictureType type, uint32_t &errorCode)
{
#ifdef HEIF_HW_DECODE_ENABLE
    if (extDecoder == nullptr) {
        IMAGE_LOGE("Invalid parameter");
        errorCode = ERR_IMAGE_INVALID_PARAMETER;
        return nullptr;
    }

    DecodeContext context;
    context.allocatorType = AllocatorType::DMA_ALLOC;
    if (!extDecoder->DecodeHeifAuxiliaryMap(context, type)) {
        IMAGE_LOGE("Decode heif auxiliary map failure");
        errorCode = ERR_IMAGE_DECODE_FAILED;
        return nullptr;
    }

    std::shared_ptr<PixelMap> pixelMap = CreatePixelMapByContext(context, extDecoder, errorCode);
    unique_ptr<AuxiliaryPicture> auxPicture = AuxiliaryPicture::Create(pixelMap, type, context.outInfo.size);
    AuxiliaryPictureInfo auxInfo = MakeAuxiliaryPictureInfo(type, context.outInfo.size, pixelMap->GetRowStride(),
        context.outInfo.pixelFormat, context.outInfo.colorSpace);
    auxPicture->SetAuxiliaryPictureInfo(auxInfo);

    errorCode = DecodeHeifMetadata(extDecoder, type, auxPicture);
    if (errorCode != SUCCESS) {
        IMAGE_LOGE("Decode heif metadata failed.");
        return nullptr;
    }
    return std::move(auxPicture);
#else
    errorCode = ERR_IMAGE_HW_DECODE_UNSUPPORT;
    return nullptr;
#endif
}

std::shared_ptr<AuxiliaryPicture> AuxiliaryGenerator::GenerateJpegAuxiliaryPicture(
    std::unique_ptr<InputDataStream> &auxStream, AuxiliaryPictureType type, uint32_t &errorCode)
{
    IMAGE_LOGI("AuxiliaryPictureType: %{public}d", type);
    auto supportStatus = SUPPORT_CODEC_AUXILIARY_MAP.find(type);
    if (supportStatus == SUPPORT_CODEC_AUXILIARY_MAP.end()) {
        return nullptr;
    }

    std::shared_ptr<PixelMap> auxPixelMap;
    std::unique_ptr<AuxiliaryPicture> auxPicture;
    auto auxDecoder = std::unique_ptr<AbsImageDecoder>(
        DoCreateDecoder(IMAGE_EXTENDED_CODEC, ImageUtils::GetPluginServer(), *auxStream, errorCode));
    if (supportStatus->second == SupportCodec::SUPPORT) {
        DecodeContext auxCtx;
        errorCode = DoJpegDecode(auxDecoder, auxCtx);
        if (errorCode != SUCCESS) {
            IMAGE_LOGE("DoJpegDecode() failed! errorCode: %{public}d", errorCode);
            return nullptr;
        }
        auxPixelMap = CreatePixelMapByContext(auxCtx, auxDecoder, errorCode);
        auxPicture = AuxiliaryPicture::Create(auxPixelMap, type, auxCtx.outInfo.size);
        AuxiliaryPictureInfo auxInfo = MakeAuxiliaryPictureInfo(type, auxCtx.outInfo.size,
            auxPixelMap->GetRowStride(), auxCtx.pixelFormat, auxCtx.outInfo.colorSpace);
        auxPicture->SetAuxiliaryPictureInfo(auxInfo);
    } else {
        InitializationOptions opts;
        std::unique_ptr<PixelMap> pixelMap = PixelMap::Create(
            reinterpret_cast<uint32_t*>(auxStream->GetDataPtr()), auxStream->GetStreamSize(), opts);
        auxPixelMap = std::move(pixelMap);
        auxPicture = AuxiliaryPicture::Create(auxPixelMap, type);
    }

    if (type == AuxiliaryPictureType::GAINMAP) {
        errorCode = DecodeHdrMetadata(auxDecoder, auxPicture);
    } else if (type == AuxiliaryPictureType::FRAGMENT_MAP) {
        errorCode = DecodeFragmentMetadata(auxDecoder, auxPicture);
    }
    if (errorCode != SUCCESS) {
        IMAGE_LOGE("Ipeg decode metadata failed! errorCode: %{public}d", errorCode);
        return nullptr;
    }

    std::shared_ptr<AuxiliaryPicture> finalAuxPicture = std::move(auxPicture);
    return finalAuxPicture;
}

} // Media
} // OHOS
