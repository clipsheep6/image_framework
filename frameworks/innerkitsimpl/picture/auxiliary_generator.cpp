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

enum AuxMetadataType { EXIF, FRAGMENT, HDR };
std::map<AuxMetadataType, std::string> AuxMetadataTypeMap = {
    { AuxMetadataType::EXIF, "Exif\0\0" },
    { AuxMetadataType::FRAGMENT, "Fragment\0\0" },
    { AuxMetadataType::HDR, "Hdr\0\0" }
};

// class AuxiliaryPicture;
// namespace InnerFormat {
// static const std::string RAW_FORMAT = "image/x-raw";
// static const std::string ASTC_FORMAT = "image/astc";
// static const std::string EXTENDED_FORMAT = "image/x-skia";
static const std::string IMAGE_EXTENDED_CODEC = "image/extended";
// static const std::string SVG_FORMAT = "image/svg+xml";
// static const std::string RAW_EXTENDED_FORMATS[] = {
//     "image/x-sony-arw",
//     "image/x-canon-cr2",
//     "image/x-adobe-dng",
//     "image/x-nikon-nef",
//     "image/x-nikon-nrw",
//     "image/x-olympus-orf",
//     "image/x-fuji-raf",
//     "image/x-panasonic-rw2",
//     "image/x-pentax-pef",
//     "image/x-samsung-srw",
// };
static const uint32_t FIRST_FRAME = 0;
// static const int INT_ZERO = 0;

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

AuxiliaryPictureInfo AuxiliaryGenerator::MakeAuxiliaryPictureInfo(
    AuxiliaryPictureType type, Size size, int32_t rowStride, PixelFormat format, ColorSpace colorSpace)
{
    AuxiliaryPictureInfo info;
    info.auxiliaryPictureType = type;
    info.size = size;
    info.rowStride = rowStride;
    info.pixelFormat = format;
    info.colorSpace = colorSpace;
    return info;
}

std::shared_ptr<ImageMetadata> AuxiliaryGenerator::CreateExifMetadata(
    uint8_t *buffer, const uint32_t size, uint32_t &errorCode)
{
    auto metadataAccessor = MetadataAccessorFactory::Create(buffer, size);
    if (metadataAccessor == nullptr) {
        IMAGE_LOGD("metadataAccessor nullptr return ERR");
        errorCode = ERR_IMAGE_SOURCE_DATA;  // TODO
        return nullptr;
    }

    uint32_t ret = metadataAccessor->Read();
    if (ret != SUCCESS) {
        IMAGE_LOGD("get metadataAccessor ret %{public}d", ret);
        errorCode = ERR_IMAGE_DECODE_EXIF_UNSUPPORT;  // TODO
        return nullptr;
    }

    if (metadataAccessor->Get() == nullptr) {
        if (!metadataAccessor->Create()) {
            IMAGE_LOGD("metadataAccessor create failed.");
            errorCode = ERR_IMAGE_SOURCE_DATA;  // TODO
            return nullptr;
        }
    }

    std::shared_ptr<ExifMetadata> exifMetadata = metadataAccessor->Get();
    return exifMetadata;
}

bool AuxiliaryGenerator::DecodeHeifMetaData(AbsImageDecoder* extDecoder, AuxiliaryPicture* auxPicture,
                                            AuxiliaryPictureType type, uint32_t &errorCode)
{
    for (const auto& metadataType : AuxMetadataTypeMap) {
        switch (metadataType.first) {
            case AuxMetadataType::EXIF: {
                std::vector<uint8_t> buffer = extDecoder->GetHeifMetadata(metadataType.second);
                if (buffer.size() == 0) {
                    IMAGE_LOGE("Get Metadata failed,Auxiliary picture type: %{public}d", type);
                    continue;
                }
                std::shared_ptr<ImageMetadata> exifMetadata = CreateExifMetadata(buffer.data(), buffer.size(), errorCode);
                if (exifMetadata == nullptr) {
                    IMAGE_LOGE("HEIF decode EXIF meta data failed! Auxiliary picture type: %{public}d", type);
                    continue;
                } else {
                    auxPicture->SetMetadata(MetadataType::EXIF, exifMetadata);
                }
                break;
            }
            case AuxMetadataType::HDR: {
                if (type != AuxiliaryPictureType::NONE && type != AuxiliaryPictureType::GAINMAP) {
                    continue;
                }
                ImageHdrType hdrType = extDecoder->CheckHdrType();
                HdrMetadata hdrMetadata = extDecoder->GetHdrMetadata(hdrType);
                std::shared_ptr<PixelMap> pixelMap = auxPicture->GetContentPixel();
                pixelMap->SetHdrMetadata(hdrMetadata);
                break;
            }
            case AuxMetadataType::FRAGMENT: {
                if (type != AuxiliaryPictureType::NONE && type != AuxiliaryPictureType::FRAGMENT_MAP) {
                    continue;
                }
                // TODO
                break;
            }
            default: {
                IMAGE_LOGE("Get Metadata failed, AuxMetadataType is %{public}s", metadataType.second.c_str());
                break;
            }
        }
    }
    return true;
}

AbsImageDecoder* AuxiliaryGenerator::DoCreateDecoder(std::string codecFormat, PluginServer &pluginServer, InputDataStream &sourceData,
    uint32_t &errorCode) __attribute__((no_sanitize("cfi")))
{
    std::map<std::string, AttrData> capabilities = { { IMAGE_ENCODE_FORMAT, AttrData(codecFormat) } };
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

// static bool IsSizeVailed(const Size &size)
// {
//     return (size.width != INT_ZERO && size.height != INT_ZERO);
// }

// static void CopySize(const Size &src, Size &dst)
// {
//     dst.width = src.width;
//     dst.height = src.height;
// }

// static bool IsDensityChange(int32_t srcDensity, int32_t wantDensity)
// {
//     return (srcDensity != 0 && wantDensity != 0 && srcDensity != wantDensity);
// }

// static int32_t GetScalePropByDensity(int32_t prop, int32_t srcDensity, int32_t wantDensity)
// {
//     if (srcDensity != 0) {
//         return (prop * wantDensity + (srcDensity >> 1)) / srcDensity;
//     }
//     return prop;
// }

// static void TransformSizeWithDensity(const Size &srcSize, int32_t srcDensity, const Size &wantSize,
//     int32_t wantDensity, Size &dstSize)
// {
//     if (IsSizeVailed(wantSize)) {
//         CopySize(wantSize, dstSize);
//     } else {
//         CopySize(srcSize, dstSize);
//     }

//     if (IsDensityChange(srcDensity, wantDensity)) {
//         dstSize.width = GetScalePropByDensity(dstSize.width, srcDensity, wantDensity);
//         dstSize.height = GetScalePropByDensity(dstSize.height, srcDensity, wantDensity);
//     }
// }

// static void CopyOptionsToPlugin(const DecodeOptions &opts, PixelDecodeOptions &plOpts)
// {
//     plOpts.CropRect.left = opts.CropRect.left;
//     plOpts.CropRect.top = opts.CropRect.top;
//     plOpts.CropRect.width = opts.CropRect.width;
//     plOpts.CropRect.height = opts.CropRect.height;
//     plOpts.desiredSize.width = opts.desiredSize.width;
//     plOpts.desiredSize.height = opts.desiredSize.height;
//     plOpts.rotateDegrees = opts.rotateDegrees;
//     plOpts.sampleSize = opts.sampleSize;
//     plOpts.desiredPixelFormat = opts.desiredPixelFormat;
//     plOpts.desiredColorSpace = opts.desiredColorSpace;
//     plOpts.allowPartialImage = opts.allowPartialImage;
//     plOpts.editable = opts.editable;
//     if (opts.SVGOpts.fillColor.isValidColor) {
//         plOpts.plFillColor.isValidColor = opts.SVGOpts.fillColor.isValidColor;
//         plOpts.plFillColor.color = opts.SVGOpts.fillColor.color;
//     }
//     if (opts.SVGOpts.strokeColor.isValidColor) {
//         plOpts.plStrokeColor.isValidColor = opts.SVGOpts.strokeColor.isValidColor;
//         plOpts.plStrokeColor.color = opts.SVGOpts.strokeColor.color;
//     }
//     if (opts.SVGOpts.SVGResize.isValidPercentage) {
//         plOpts.plSVGResize.isValidPercentage = opts.SVGOpts.SVGResize.isValidPercentage;
//         plOpts.plSVGResize.resizePercentage = opts.SVGOpts.SVGResize.resizePercentage;
//     }
//     plOpts.plDesiredColorSpace = opts.desiredColorSpaceInfo;
// }

// static uint32_t SetAuxiliaryPictureDecodeOption(std::unique_ptr<AbsImageDecoder>& decoder,
//                                                              PlImageInfo& plInfo, float scale)
// {
//     ImageInfo info;
//     Size size;
//     uint32_t errorCode = decoder->GetImageSize(FIRST_FRAME, size);
//     info.size.width = size.width;
//     info.size.height = size.height;
//     if (errorCode != SUCCESS || !IsSizeVailed({size.width, size.height})) {
//         errorCode = ERR_IMAGE_DATA_ABNORMAL;
//         return errorCode;
//     }
//     Size wantSize = info.size;
//     if (scale > 0 && scale < 1.0) {
//         wantSize.width = info.size.width * scale;
//         wantSize.height = info.size.height * scale;
//     }
//     DecodeOptions opts;
//     // TransformSizeWithDensity(info.size, sourceInfo_.baseDensity, wantSize, opts_.fitDensity, opts.desiredSize);
//     PixelDecodeOptions plOptions;
//     CopyOptionsToPlugin(opts, plOptions);
//     plOptions.desiredPixelFormat = PixelFormat::RGBA_8888;
//     errorCode = decoder->SetDecodeOptions(FIRST_FRAME, plOptions, plInfo);
//     return errorCode;
// }

shared_ptr<AuxiliaryPicture> AuxiliaryGenerator::GenerateHeifAuxiliaryPicture(
    AbsImageDecoder* extDecoder, AuxiliaryPictureType type, uint32_t &errorCode)
{
#ifdef HEIF_HW_DECODE_ENABLE
    if (extDecoder == nullptr || type == AuxiliaryPictureType::NONE) {
        IMAGE_LOGE("Invalid parameter");
        errorCode = 211;    // TODO
        return nullptr;
    }

    // Decode heif auxiliary map.
    DecodeContext context;
    if (!extDecoder->DecodeHeifAuxiliaryMap(context, type)) {
        IMAGE_LOGE("Decode heif auxiliary map failure");
        errorCode = 211;    // TODO
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
    ImageInfo info = MakeImageInfo(context.outInfo.size.width, context.outInfo.size.height, context.outInfo.pixelFormat,
        context.outInfo.alphaType, context.outInfo.colorSpace);
    pixelMap->SetImageInfo(info, true);
    OHOS::ColorManager::ColorSpace grColorSpace = extDecoder->getGrColorSpace();
    pixelMap->InnerSetColorSpace(grColorSpace);

    // Create auxiliary picture object, and set auxiliary picture info.
    unique_ptr<AuxiliaryPicture> auxPicture = AuxiliaryPicture::Create(pixelMap, type);
    auto* sbBuffer = reinterpret_cast<SurfaceBuffer*>(context.pixelsBuffer.context);
    AuxiliaryPictureInfo auxInfo = MakeAuxiliaryPictureInfo(
        type, {context.outInfo.size.width, context.outInfo.size.height}, sbBuffer->GetStride(),
        context.outInfo.pixelFormat, context.outInfo.colorSpace);
    auxiliaryPicture->SetAuxiliaryPictureInfo(auxInfo);

    if (!DecodeHeifMetaData(extDecoder, auxPicture.get(), type, errorCode)) {
        IMAGE_LOGE("Decode heif metadata failure");
        return nullptr;
    }
    return auxPicture;
#endif
    errorCode = 211;    // TODO: 待修改，等待最后通统一商定errorcode
    return nullptr;
}

std::shared_ptr<AuxiliaryPicture> AuxiliaryGenerator::GenerateJpegAuxiliaryPicture(
    std::unique_ptr<InputDataStream> &auxiliaryStream, AuxiliaryPictureType type, uint32_t &errorCode)
{
    auto supportStatus = SUPPORT_CODEC_AUXILIARY_MAP.find(type);
    if (supportStatus == SUPPORT_CODEC_AUXILIARY_MAP.end()) {
        return nullptr;
    }

    std::shared_ptr<PixelMap> auxPixelMap;
    if (supportStatus->second == SupportCodec::SUPPORT) {
        PluginServer &pluginServer = ImageUtils::GetPluginServer();
        auto auxDecoder =
            DoCreateDecoder(IMAGE_EXTENDED_CODEC, pluginServer, *auxiliaryStream, errorCode);
        if (auxDecoder == nullptr) {
            errorCode = 211;    // TODO: 待修改，等待最后通统一商定errorcode
            return nullptr;
        }

        // TODO: SetAuxiliaryPictureDecodeOption

        DecodeContext auxCtx;
        auxCtx.allocatorType = AllocatorType::DMA_ALLOC;
        errorCode = auxDecoder->Decode(FIRST_FRAME, auxCtx);

        // if (auxiliaryPictureInfo.size...)

        if (errorCode != SUCCESS) {
            IMAGE_LOGE("Jpeg Decode auxiliary picture failed! Auxiliary picture type: %{public}d", type);
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
            return nullptr;
        }

        ImageInfo imageinfo = MakeImageInfo(auxCtx.outInfo.size.width, auxCtx.outInfo.size.height,
                                            auxCtx.outInfo.pixelFormat, auxCtx.outInfo.alphaType,
                                            auxCtx.outInfo.colorSpace);
        auxPixelMap->SetImageInfo(imageinfo, true);
        PixelMapAddrInfos addrInfos;
        ImageSource::ContextToAddrInfos(auxCtx, addrInfos);
        auxPixelMap->SetPixelsAddr(addrInfos.addr, addrInfos.context, addrInfos.size, addrInfos.type, addrInfos.func);
        // auxPixelMap->SetPixelsAddr(auxCtx.pixelsBuffer.buffer, auxCtx.pixelsBuffer.context,
        //                            auxCtx.pixelsBuffer.bufferSize, auxCtx.allocatorType, auxCtx.freeFunc);
#ifdef IMAGE_COLORSPACE_FLAG
        bool isSupportICCProfile = auxDecoder->IsSupportICCProfile();
        if (isSupportICCProfile) {
            OHOS::ColorManager::ColorSpace grColorSpace = auxDecoder->getGrColorSpace();
            auxPixelMap->InnerSetColorSpace(grColorSpace);
        }
#endif
        // std::unique_ptr<PixelMap> pixelMap = PixelMap::Create(
        //     reinterpret_cast<uint32_t*>(auxCtx.pixelsBuffer.buffer), auxCtx.pixelsBuffer.bufferSize, opts);
        // auxPixelMap = std::move(pixelMap);
    } else {
        // TODO: 如果不解码，opts如何设置
        InitializationOptions opts;
        std::unique_ptr<PixelMap> pixelMap = PixelMap::Create(
            reinterpret_cast<uint32_t*>(auxiliaryStream->GetDataPtr()), auxiliaryStream->GetStreamSize(), opts);
        auxPixelMap = std::move(pixelMap);
        // auxPixelMap->SetPixelsAddr();
    }
    std::shared_ptr<AuxiliaryPicture> auxiliaryPicture = AuxiliaryPicture::Create(auxPixelMap, type);

    // TODO: 完善AuxiliaryPictureInfo
    AuxiliaryPictureInfo auxInfo = MakeAuxiliaryPictureInfo(type, {0, 0}, 0, PixelFormat::UNKNOWN, ColorSpace::UNKNOWN);
    auxiliaryPicture->SetAuxiliaryPictureInfo(auxInfo);

    // TODO: Metadata 处理
    if (!DecodeJpegMetaData(auxiliaryStream, auxiliaryPicture, errorCode)) {
        IMAGE_LOGE("Jpeg Decode Meta Data failed! Auxiliary picture type: %{public}d", type);
        return nullptr;
    }
    
    return auxiliaryPicture;
}

bool AuxiliaryGenerator::DecodeJpegMetaData(std::unique_ptr<ImagePlugin::InputDataStream> &auxStream,
    std::shared_ptr<AuxiliaryPicture> &auxPicture, uint32_t &errorCode)
{
    AuxiliaryPictureType type = auxPicture->GetType();
    if (type != AuxiliaryPictureType::NONE) {
        return false;
    }

    // Decode EXIF Meta Data
    // TODO: 需要确认哪些类型的辅助图需要exif metadata
    std::shared_ptr<ImageMetadata> exifMetadata = CreateExifMetadata(auxStream->GetDataPtr(),
                                                                     auxStream->GetStreamSize(),
                                                                     errorCode);
    if (exifMetadata == nullptr) {
        IMAGE_LOGE("Jpeg Decode EXIF Meta Data failed! Auxiliary picture type: %{public}d", type);
        return false;
    } else {
        auxPicture->SetMetadata(MetadataType::EXIF, exifMetadata);
    }

    // Decode FRAGMENT Meta Data
    if (type == AuxiliaryPictureType::FRAGMENT_MAP) {
        std::shared_ptr<ImageMetadata> fragmentMetadata = nullptr;  // TODO: 1.不知道水印metadata如何解析；2.FragmentMetadata PR尚未合入
        if (fragmentMetadata == nullptr) {
            IMAGE_LOGE("Jpeg Decode FRAGMENT Meta Data failed! Auxiliary picture type: %{public}d", type);
            return false;
        } else {
            auxPicture->SetMetadata(MetadataType::FRAGMENT, fragmentMetadata);
        }
    }

    return true;
}

} // Media
} // OHOS
