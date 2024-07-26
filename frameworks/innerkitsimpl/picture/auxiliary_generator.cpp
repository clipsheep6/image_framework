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

static const std::string IMAGE_EXTENDED_CODEC = "image/extended";
static const uint32_t FIRST_FRAME = 0;

// Interface for Jpeg/Heif
ImageInfo AuxiliaryGenerator::MakeImageInfo(int width, int height, PixelFormat pf, AlphaType at, ColorSpace cs)
{
    ImageInfo info;
    info.size.width = width;
    info.size.height = height;
    info.pixelFormat = pf;
    info.alphaType = at;
    info.colorSpace = cs;
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
uint32_t AuxiliaryGenerator::DecodeHdrMetadata(AbsImageDecoder *extDecoder, std::unique_ptr<AuxiliaryPicture> &auxPicture)
{
    ImageHdrType hdrType = extDecoder->CheckHdrType();
    std::shared_ptr<HdrMetadata> hdrMetadata = std::make_shared<HdrMetadata>(extDecoder->GetHdrMetadata(hdrType));
    std::shared_ptr<PixelMap> pixelMap = auxPicture->GetContentPixel();
    pixelMap->SetHdrMetadata(hdrMetadata);
    pixelMap->SetHdrType(hdrType);
    return SUCCESS;
}

// Interface for Jpeg/Heif
uint32_t AuxiliaryGenerator::DecodeFragmentMetadata(AbsImageDecoder *extDecoder, std::unique_ptr<AuxiliaryPicture> &auxPicture)
{
    AuxiliaryPictureType type = auxPicture->GetType();
    if (type != AuxiliaryPictureType::FRAGMENT_MAP) {
        return ERR_MEDIA_DATA_UNSUPPORT;
    }

    std::shared_ptr<ImageMetadata> fragmentMetadata = nullptr;  // TODO: 1.水印metadata的解析依赖实际数据格式（外部）；2.FragmentMetadata PR尚未合入（内部）
    if (fragmentMetadata == nullptr) {
        IMAGE_LOGE("Decode fragment metadata failed! Auxiliary picture type: %{public}d", type);
        return ERR_IMAGE_DECODE_ABNORMAL;
    }
    auxPicture->SetMetadata(MetadataType::FRAGMENT, fragmentMetadata);
    return SUCCESS;
}

uint32_t AuxiliaryGenerator::DecodeHeifMetadata(AbsImageDecoder *extDecoder, AuxiliaryPictureType type,
                                                std::unique_ptr<AuxiliaryPicture> &auxPicture)
{
    uint32_t errorCode;
    switch (type) {
        case AuxiliaryPictureType::GAINMAP: {
            errorCode = DecodeHdrMetadata(extDecoder, auxPicture);
            if (errorCode != SUCCESS) {
                IMAGE_LOGE("DecodeHdrMetadata() failed! errorCode: %{public}d", errorCode);
                return errorCode;
            }
            break;
        }
        case AuxiliaryPictureType::FRAGMENT_MAP: {
            errorCode = DecodeFragmentMetadata(extDecoder, auxPicture);
            if (errorCode != SUCCESS) {
                IMAGE_LOGE("DecodeFragmentMetadata() failed! errorCode: %{public}d", errorCode);
                return errorCode;
            }
            break;
        }
        default: {
            errorCode = ERR_MEDIA_DATA_UNSUPPORT;
            IMAGE_LOGE("Get metadata failed! AuxiliaryPictureType is %{public}d", type);
            break;
        }
    }
    return errorCode;
}

// Interface for Jpeg
AbsImageDecoder* AuxiliaryGenerator::DoCreateDecoder(std::string codecFormat, PluginServer &pluginServer, InputDataStream &sourceData,
    uint32_t &errorCode) __attribute__((no_sanitize("cfi")))
{
    std::map<std::string, AttrData> capabilities = {{IMAGE_ENCODE_FORMAT, AttrData(codecFormat)}};
    for (const auto &capability : capabilities) {
        std::string x = "undefined";
        capability.second.GetValue(x);
        IMAGE_LOGD("[AuxiliaryGenerator] capabilities [%{public}s],[%{public}s]", capability.first.c_str(), x.c_str());
    }
    auto decoder = pluginServer.CreateObject<AbsImageDecoder>(AbsImageDecoder::SERVICE_DEFAULT, capabilities);
    if (decoder == nullptr) {
        IMAGE_LOGE("[AuxiliaryGenerator] failed to create decoder object!");
        errorCode = ERR_IMAGE_PLUGIN_CREATE_FAILED;
        return nullptr;
    }
    errorCode = SUCCESS;
    decoder->SetSource(sourceData);
    return decoder;
}

// Interface for Jpeg
void AuxiliaryGenerator::FreeContextBuffer(const Media::CustomFreePixelMap &func, AllocatorType allocType, PlImageBuffer &buffer)
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

shared_ptr<AuxiliaryPicture> AuxiliaryGenerator::GenerateHeifAuxiliaryPicture(
    AbsImageDecoder *extDecoder, AuxiliaryPictureType type, uint32_t &errorCode)
{
#ifdef HEIF_HW_DECODE_ENABLE
    if (extDecoder == nullptr) {
        IMAGE_LOGE("Invalid parameter");
        errorCode = ERR_IMAGE_INVALID_PARAMETER;
        return nullptr;
    }

    // Decode heif auxiliary map.
    DecodeContext context;
    context.info.pixelFormat = PixelFormat::NV21;
    if (!extDecoder->DecodeHeifAuxiliaryMap(context, type)) {
        IMAGE_LOGE("Decode heif auxiliary map failure");
        errorCode = ERR_IMAGE_DECODE_FAILED;
        return nullptr;
    }

    // Create pixelMap and set image information.
    shared_ptr<PixelMap> pixelMap;
    if (ImageSource::IsYuvFormat(context.outInfo.pixelFormat)) {
#ifdef EXT_PIXEL
        pixelMap = make_unique<PixelYuvExt>();
#else
        pixelMap = make_unique<PixelYuv>();
#endif
    } else {
        pixelMap = make_unique<PixelMap>();
    }
    PixelMapAddrInfos addrInfos;
    ImageSource::ContextToAddrInfos(context, addrInfos);
    pixelMap->SetPixelsAddr(addrInfos.addr, addrInfos.context, addrInfos.size, addrInfos.type, addrInfos.func);
    ImageInfo info = MakeImageInfo(context.outInfo.size.width, context.outInfo.size.height,
        context.outInfo.pixelFormat, context.outInfo.alphaType, context.outInfo.colorSpace);
    pixelMap->SetImageInfo(info);
    OHOS::ColorManager::ColorSpace grColorSpace = extDecoder->getGrColorSpace();
    pixelMap->InnerSetColorSpace(grColorSpace);

    // Create auxiliary picture object, and set auxiliary picture info.
    unique_ptr<AuxiliaryPicture> auxPicture = AuxiliaryPicture::Create(pixelMap, type);
    AuxiliaryPictureInfo auxInfo = MakeAuxiliaryPictureInfo(type, context.outInfo.size, pixelMap->GetRowStride(),
        context.outInfo.pixelFormat, context.outInfo.colorSpace);
    auxPicture->SetAuxiliaryPictureInfo(auxInfo);

    errorCode = DecodeHeifMetadata(extDecoder, type, auxPicture);
    if (errorCode != SUCCESS) {
        IMAGE_LOGE("Decode heif metadata failed.");
        return nullptr;
    }
    return std::move(auxPicture);
#endif
    errorCode = ERR_IMAGE_HW_DECODE_UNSUPPORT;
    return nullptr;
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
    PluginServer &pluginServer = ImageUtils::GetPluginServer();
    auto auxDecoder = DoCreateDecoder(IMAGE_EXTENDED_CODEC, pluginServer, *auxStream, errorCode);
    if (supportStatus->second == SupportCodec::SUPPORT) {
        if (auxDecoder == nullptr) {
            errorCode = IMAGE_RESULT_CREATE_DECODER_FAILED;
            return nullptr;
        }

        DecodeContext auxCtx;
        auxCtx.allocatorType = AllocatorType::DMA_ALLOC;
        errorCode = auxDecoder->Decode(FIRST_FRAME, auxCtx);
        if (errorCode != SUCCESS) {
            IMAGE_LOGE("Jpeg Decode auxiliary picture failed! errorCode: %{public}d", errorCode);
            FreeContextBuffer(auxCtx.freeFunc, auxCtx.allocatorType, auxCtx.pixelsBuffer);
            return nullptr;
        }

        if (ImageSource::IsYuvFormat(auxCtx.outInfo.pixelFormat)) {
#ifdef EXT_PIXEL
            auxPixelMap = std::make_shared<PixelYuvExt>();
#else
            auxPixelMap = std::make_shared<PixelYuv>();
#endif
        } else {
            auxPixelMap = std::make_shared<PixelMap>();
        }
        if (auxPixelMap == nullptr) {
            errorCode = ERR_IMAGE_ADD_PIXEL_MAP_FAILED;
            return nullptr;
        }
        ImageInfo imageinfo = MakeImageInfo(auxCtx.outInfo.size.width, auxCtx.outInfo.size.height,
                                            auxCtx.pixelFormat, auxCtx.outInfo.alphaType,
                                            auxCtx.outInfo.colorSpace);
        auxPixelMap->SetImageInfo(imageinfo, true);
        PixelMapAddrInfos addrInfos;
        ImageSource::ContextToAddrInfos(auxCtx, addrInfos);
        auxPixelMap->SetPixelsAddr(addrInfos.addr, addrInfos.context, addrInfos.size, addrInfos.type, addrInfos.func);
#ifdef IMAGE_COLORSPACE_FLAG
        if (auxCtx.hdrType > ImageHdrType::SDR) {
            auxPixelMap->InnerSetColorSpace(OHOS::ColorManager::ColorSpace(auxCtx.grColorSpaceName));
        } else if (auxDecoder->IsSupportICCProfile()) {
            OHOS::ColorManager::ColorSpace grColorSpace = auxDecoder->getGrColorSpace();
            auxPixelMap->InnerSetColorSpace(grColorSpace);
        }
#endif
        auxPicture = AuxiliaryPicture::Create(auxPixelMap, type, imageinfo.size);
        AuxiliaryPictureInfo auxInfo = MakeAuxiliaryPictureInfo(type, imageinfo.size, auxPixelMap->GetRowStride(),
                                                                imageinfo.pixelFormat, imageinfo.colorSpace);
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
        if (errorCode != SUCCESS) {
            IMAGE_LOGE("DecodeHdrMetadata() failed! errorCode: %{public}d", errorCode);
            return nullptr;
        }
    }

    if (type == AuxiliaryPictureType::FRAGMENT_MAP) {
        errorCode = DecodeFragmentMetadata(auxDecoder, auxPicture);
        if (errorCode != SUCCESS) {
            IMAGE_LOGE("DecodeFragmentMetadata() failed! errorCode: %{public}d", errorCode);
            return nullptr;
        }
    }

    std::shared_ptr<AuxiliaryPicture> finalAuxPicture = std::move(auxPicture);
    return finalAuxPicture;
}

} // Media
} // OHOS
