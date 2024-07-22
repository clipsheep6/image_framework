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

#include <memory>
#include "exif_metadata.h"
#include "picture.h"
#include "pixel_yuv.h"
#include "pixel_yuv_ext.h"
#include "image_utils.h"
#include "image_log.h"
#include "media_errors.h"                                                // Operation success
#ifdef IMAGE_COLORSPACE_FLAG
#include "color_space.h"
#endif
#include "surface_buffer.h"
#include "securec.h"
#include "tiff_parser.h"
#include "image_source.h"
#include "v1_0/cm_color_space.h"
#include "vpe_utils.h"

namespace OHOS {
namespace Media {
using namespace HDI::Display::Graphic::Common::V1_0;
namespace {
    static const std::map<int32_t, PixelFormat> PIXEL_FORMAT_MAP = {
        { GRAPHIC_PIXEL_FMT_RGBA_8888, PixelFormat::RGBA_8888 },
        { GRAPHIC_PIXEL_FMT_YCBCR_420_SP, PixelFormat::NV12 },
        { GRAPHIC_PIXEL_FMT_YCRCB_420_SP, PixelFormat::NV21 },
        { GRAPHIC_PIXEL_FMT_RGBA_1010102, PixelFormat::RGBA_1010102 },
        { GRAPHIC_PIXEL_FMT_BGRA_8888, PixelFormat::BGRA_8888 },
        { GRAPHIC_PIXEL_FMT_RGB_888, PixelFormat::RGB_888 },
        { GRAPHIC_PIXEL_FMT_RGB_565, PixelFormat::RGB_565 },
    };

    static const std::map<int32_t, ColorSpace> COLORSPACE_MAP = {
        { GRAPHIC_COLOR_GAMUT_INVALID, ColorSpace::UNKNOWN },
        { GRAPHIC_COLOR_GAMUT_NATIVE, ColorSpace::SRGB },
        { GRAPHIC_COLOR_GAMUT_STANDARD_BT709, ColorSpace::ITU_709 },
        { GRAPHIC_COLOR_GAMUT_DCI_P3, ColorSpace::DCI_P3 },
        { GRAPHIC_COLOR_GAMUT_SRGB, ColorSpace::SRGB },
        { GRAPHIC_COLOR_GAMUT_ADOBE_RGB, ColorSpace::ADOBE_RGB_1998 },
        { GRAPHIC_COLOR_GAMUT_DISPLAY_P3, ColorSpace::DISPLAY_P3 },
        { GRAPHIC_COLOR_GAMUT_BT2020, ColorSpace::ITU_2020 },
    };

#ifdef IMAGE_COLORSPACE_FLAG
    static const std::map<int32_t, ColorManager::ColorSpaceName> COLORSPACE_NAME_MAP = {
        { GRAPHIC_COLOR_GAMUT_INVALID, ColorManager::NONE },
        { GRAPHIC_COLOR_GAMUT_NATIVE, ColorManager::SRGB },
        { GRAPHIC_COLOR_GAMUT_STANDARD_BT601, ColorManager::BT601_EBU },
        { GRAPHIC_COLOR_GAMUT_STANDARD_BT709, ColorManager::BT709 },
        { GRAPHIC_COLOR_GAMUT_DCI_P3, ColorManager::DCI_P3 },
        { GRAPHIC_COLOR_GAMUT_SRGB, ColorManager::SRGB },
        { GRAPHIC_COLOR_GAMUT_ADOBE_RGB, ColorManager::ADOBE_RGB },
        { GRAPHIC_COLOR_GAMUT_DISPLAY_P3, ColorManager::DISPLAY_P3 },
        { GRAPHIC_COLOR_GAMUT_BT2020, ColorManager::BT2020 },
        { GRAPHIC_COLOR_GAMUT_BT2100_PQ, ColorManager::BT2020_PQ},
        { GRAPHIC_COLOR_GAMUT_BT2100_HLG, ColorManager::BT2020_HLG},
        { GRAPHIC_COLOR_GAMUT_DISPLAY_BT2020, ColorManager::BT2020},
    };
#endif
}

static const uint8_t NUM_0 = 0;
static const uint8_t NUM_1 = 1;
static const uint8_t NUM_2 = 2;

static bool IsYuvFormat(PixelFormat format)
{
    return format == PixelFormat::NV21 || format == PixelFormat::NV12;
}

static bool IsAlphaFormat(PixelFormat format)
{
    return format == PixelFormat::RGBA_8888 || format == PixelFormat::BGRA_8888 || format == PixelFormat::RGBA_1010102;
}

static PixelFormat SbFormat2PixelFormat(int32_t sbFormat)
{
    auto iter = PIXEL_FORMAT_MAP.find(sbFormat);
    if (iter == PIXEL_FORMAT_MAP.end()) {
        return PixelFormat::UNKNOWN;
    }
    return iter->second;
}

static ColorSpace SbGamut2ColorSpace(OHOS::GraphicColorGamut sbGamut)
{
    auto iter = COLORSPACE_MAP.find(sbGamut);
    if (iter == COLORSPACE_MAP.end()) {
        return ColorSpace::UNKNOWN;
    }
    return iter->second;
}

#ifdef IMAGE_COLORSPACE_FLAG
static ColorManager::ColorSpaceName SbGamut2ColorSpaceName(OHOS::GraphicColorGamut sbGamut)
{
    auto iter = COLORSPACE_NAME_MAP.find(sbGamut);
    if (iter == COLORSPACE_NAME_MAP.end()) {
        return ColorManager::NONE;
    }
    return iter->second;
}
#endif

static ImageInfo MakeImageInfo(int width, int height, PixelFormat pf, AlphaType at, ColorSpace cs)
{
    ImageInfo info;
    info.size.width = width;
    info.size.height = height;
    info.pixelFormat = pf;
    info.alphaType = at;
    info.colorSpace = cs;
    return info;
}

static void SetYuvDataInfo(std::unique_ptr<PixelMap> &pixelMap, sptr<OHOS::SurfaceBuffer> &sBuffer)
{
    if (pixelMap == nullptr || sBuffer == nullptr) {
        return;
    }
    int32_t width = sBuffer->GetWidth();
    int32_t height = sBuffer->GetHeight();
    OH_NativeBuffer_Planes *planes = nullptr;
    GSError retVal = sBuffer->GetPlanesInfo(reinterpret_cast<void**>(&planes));
    YUVDataInfo info;
    info.imageSize = { width, height };
    if (retVal != OHOS::GSERROR_OK || planes == nullptr || planes->planeCount <= NUM_1) {
        IMAGE_LOGE("Get planesInfo failed, retVal:%{public}d", retVal);
        return;
    } else if (planes->planeCount >= NUM_2) {
        info.yStride = planes->planes[NUM_0].columnStride;
        info.uvStride = planes->planes[NUM_1].columnStride;
        info.yOffset = planes->planes[NUM_0].offset;
        info.uvOffset = planes->planes[NUM_1].offset - NUM_1;
    }
    pixelMap->SetImageYUVInfo(info);
}

std::unique_ptr<Picture> Picture::Create(std::shared_ptr<PixelMap> &pixelMap)
{
    if (pixelMap == nullptr) {
        return nullptr;
    }
    std::unique_ptr<Picture> dstPicture = std::make_unique<Picture>();
    dstPicture->mainPixelMap_ = pixelMap;
    return dstPicture;
}

std::unique_ptr<Picture> Picture::Create(sptr<SurfaceBuffer> &surfaceBuffer)
{
    std::shared_ptr<PixelMap> pixelmap = SurfaceBuffer2PixelMap(surfaceBuffer);
    return Create(pixelmap);
}

std::unique_ptr<PixelMap> Picture::SurfaceBuffer2PixelMap(sptr<OHOS::SurfaceBuffer> &surfaceBuffer)
{
    if (surfaceBuffer == nullptr) {
        return nullptr;
    }
    PixelFormat pixelFormat = SbFormat2PixelFormat(surfaceBuffer->GetFormat());
    ColorSpace colorSpace = SbGamut2ColorSpace(surfaceBuffer->GetSurfaceBufferColorGamut());
    AlphaType alphaType = IsAlphaFormat(pixelFormat) ?
             AlphaType::IMAGE_ALPHA_TYPE_PREMUL : AlphaType::IMAGE_ALPHA_TYPE_OPAQUE;
    void* nativeBuffer = surfaceBuffer.GetRefPtr();
    int32_t err = ImageUtils::SurfaceBuffer_Reference(nativeBuffer);
    if (err != OHOS::GSERROR_OK) {
        IMAGE_LOGE("NativeBufferReference failed");
        return nullptr;
    }

    std::unique_ptr<PixelMap> pixelMap;
    if (IsYuvFormat(pixelFormat)) {
#ifdef EXT_PIXEL
        pixelMap = std::make_unique<PixelYuvExt>();
#else
        pixelMap = std::make_unique<PixelYuv>();
#endif
    } else {
        pixelMap = std::make_unique<PixelMap>();
    }
    if (pixelMap == nullptr) {
        return nullptr;
    }

    ImageInfo imageInfo = MakeImageInfo(surfaceBuffer->GetWidth(),
                                        surfaceBuffer->GetHeight(), pixelFormat, alphaType, colorSpace);
    pixelMap->SetImageInfo(imageInfo, true);
    pixelMap->SetPixelsAddr(surfaceBuffer->GetVirAddr(),
                            nativeBuffer, pixelMap->GetRowBytes() * pixelMap->GetHeight(),
                            AllocatorType::DMA_ALLOC, nullptr);
#ifdef IMAGE_COLORSPACE_FLAG
    ColorManager::ColorSpaceName colorSpaceName = SbGamut2ColorSpaceName(surfaceBuffer->GetSurfaceBufferColorGamut());
    pixelMap->InnerSetColorSpace(ColorManager::ColorSpace(colorSpaceName));
#endif
    if (IsYuvFormat(pixelFormat)) {
        SetYuvDataInfo(pixelMap, surfaceBuffer);
    }
    return pixelMap;
}

std::shared_ptr<PixelMap> Picture::GetMainPixel()
{
    return mainPixelMap_;
}

void Picture::SetMainPixel(std::shared_ptr <PixelMap> PixelMap)
{
    mainPixelMap_ = PixelMap;
}

std::unique_ptr<PixelMap> Picture::GetHdrComposedPixelMap()
{
    if (!HasAuxiliaryPicture(AuxiliaryPictureType::GAINMAP)) {
        IMAGE_LOGE("Unsupport HDR compose.");
        return nullptr;
    }
    std::shared_ptr<PixelMap> gainmap = Picture::GetAuxiliaryPicture(AuxiliaryPictureType::GAINMAP)->GetContentPixel();
    ImageHdrType hdrType = gainmap->GetHdrType();
    std::shared_ptr<HdrMetadata> metadata = gainmap->GetHdrMetadata();

    CM_ColorSpaceType baseCmColor = ImageSource::ConvertColorSpaceType(mainPixelMap_->InnerGetGrColorSpace().GetColorSpaceName(), true);
    sptr<SurfaceBuffer> baseSptr(reinterpret_cast<SurfaceBuffer*>(mainPixelMap_->GetFd()));
    VpeUtils::SetSurfaceBufferInfo(baseSptr, false, hdrType, baseCmColor, *metadata);

    sptr<SurfaceBuffer> gainmapSptr(reinterpret_cast<SurfaceBuffer*>(gainmap->GetFd()));
    CM_ColorSpaceType hdrCmColor = CM_BT2020_HLG_FULL;
    CM_ColorSpaceType gainmapCmColor = metadata->extendMeta.metaISO.useBaseColorFlag == 0x01 ? baseCmColor : hdrCmColor;
    IMAGE_LOGD("ComposeHdrImage color flag = %{public}d, gainmapChannelNum = %{public}d",
        metadata->extendMeta.metaISO.useBaseColorFlag, metadata->extendMeta.metaISO.gainmapChannelNum);
    ImageSource::SetVividMetaColor(*metadata, baseCmColor, gainmapCmColor, hdrCmColor);
    VpeUtils::SetSurfaceBufferInfo(gainmapSptr, true, hdrType, gainmapCmColor, *metadata);

    sptr<SurfaceBuffer> hdrSptr = SurfaceBuffer::Create();
    ImageInfo imageInfo;
    mainPixelMap_->GetImageInfo(imageInfo);
    BufferRequestConfig requestConfig = {
        .width = imageInfo.size.width,
        .height = imageInfo.size.height,
        .strideAlignment = imageInfo.size.width,
        .format = GRAPHIC_PIXEL_FMT_RGBA_1010102,
        .usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA | BUFFER_USAGE_MEM_MMZ_CACHE,
        .timeout = 0,
    };
    GSError error = hdrSptr->Alloc(requestConfig);
    if (error != GSERROR_OK) {
        IMAGE_LOGE("HDR SurfaceBuffer Alloc failed, %{public}d", ERR_DMA_NOT_EXIST);
        return nullptr;
    }
    VpeSurfaceBuffers buffers = {
        .sdr = baseSptr,
        .gainmap = gainmapSptr,
        .hdr = hdrSptr,
    };
    std::unique_ptr<VpeUtils> utils;
    bool legacy = hdrType == ImageHdrType::HDR_CUVA;
    int32_t res = utils->ColorSpaceConverterComposeImage(buffers, legacy);
    if (res != VPE_ERROR_OK) {
        IMAGE_LOGI("[ImageSource] composeImage failed");
        return nullptr;
    } else {
        return SurfaceBuffer2PixelMap(hdrSptr);
    }
}

std::shared_ptr<PixelMap> Picture::GetGainmapPixelMap()
{
    if (!HasAuxiliaryPicture(AuxiliaryPictureType::GAINMAP)) {
        IMAGE_LOGE("Unsupport gain map.");
        return nullptr;
    } else {
        return GetAuxiliaryPicture(AuxiliaryPictureType::GAINMAP)->GetContentPixel();
    }
}

std::shared_ptr<AuxiliaryPicture> Picture::GetAuxiliaryPicture(AuxiliaryPictureType type)
{
    auto iter = auxiliaryPictures_.find(type);
    if (iter == auxiliaryPictures_.end()) {
        return nullptr;
    }
    return iter->second;
}

void Picture::SetAuxiliaryPicture(AuxiliaryPictureType type, std::shared_ptr<AuxiliaryPicture> &picture)
{
    auxiliaryPictures_[type] = picture;
}

bool Picture::HasAuxiliaryPicture(AuxiliaryPictureType type)
{
    return auxiliaryPictures_.find(type) != auxiliaryPictures_.end();
}

bool Picture::Marshalling(Parcel &data) const
{
    if (!mainPixelMap_) {
        IMAGE_LOGE("Main PixelMap is null.");
        return false;
    }
    if (!mainPixelMap_->Marshalling(data)) {
        IMAGE_LOGE("Failed to marshal main PixelMap.");
        return false;
    }

    size_t numAuxiliaryPictures = auxiliaryPictures_.size();
    if (!data.WriteUint64(numAuxiliaryPictures)) {
        IMAGE_LOGE("Failed to write number of auxiliary pictures.");
        return false;
    }
    
    for (const auto &auxiliaryPicture : auxiliaryPictures_) {
        AuxiliaryPictureType type =  auxiliaryPicture.first;
        
        if (!data.WriteInt32(static_cast<int32_t>(type))) {
            IMAGE_LOGE("Failed to write auxiliary picture type.");
            return false;
        }
        
        if (!auxiliaryPicture.second || !auxiliaryPicture.second->Marshalling(data)) {
            IMAGE_LOGE("Failed to marshal auxiliary picture of type %d.", static_cast<int>(type));
            return false;
        }
    }
    
    return true;
}

Picture *Picture::Unmarshalling(Parcel &data)
{
    PICTURE_ERR error;
    Picture* dstPicture = Picture::Unmarshalling(data, error);
    if (dstPicture == nullptr || error.errorCode != SUCCESS) {
        IMAGE_LOGE("unmarshalling failed errorCode:%{public}d, errorInfo:%{public}s",
            error.errorCode, error.errorInfo.c_str());
    }
    return dstPicture;
}

Picture *Picture::Unmarshalling(Parcel &parcel, PICTURE_ERR &error)
{
    std::unique_ptr<Picture> picture = std::make_unique<Picture>();
    std::shared_ptr<PixelMap> pixelmapPtr(PixelMap::Unmarshalling(parcel));
    
    if (!pixelmapPtr) {
        IMAGE_LOGE("Failed to unmarshal main PixelMap.");
        return nullptr;
    }
    picture->SetMainPixel(pixelmapPtr);
    uint64_t numAuxiliaryPictures = parcel.ReadUint64();
    
    for (size_t i = NUM_0; i < numAuxiliaryPictures; ++i) {
        int32_t type = parcel.ReadInt32();
        std::shared_ptr<AuxiliaryPicture> auxPtr(AuxiliaryPicture::Unmarshalling(parcel));
        if (!auxPtr) {
            IMAGE_LOGE("Failed to unmarshal auxiliary picture of type %d.", type);
            return nullptr;
        }
        picture->SetAuxiliaryPicture(static_cast<AuxiliaryPictureType>(type), auxPtr);
    }
    
    return picture.release();
}

int32_t Picture::SetExifMetadata(sptr<SurfaceBuffer> &surfaceBuffer)
{
    if (surfaceBuffer == nullptr) {
        return ERR_IMAGE_INVALID_PARAMETER;
    }

    auto size = surfaceBuffer->GetSize();
    if (size <= 0) {
        IMAGE_LOGE("Invalid buffer size: %d.", size);
        return ERR_IMAGE_INVALID_PARAMETER;
    }

    ExifData *exifData;
    TiffParser::Decode(static_cast<const unsigned char *>(surfaceBuffer->GetVirAddr()), size, &exifData);
    if (exifData == nullptr) {
        IMAGE_LOGE("Failed to decode EXIF data from image stream.");
        return ERR_EXIF_DECODE_FAILED;
    }

    auto exifMetadata = std::make_shared<OHOS::Media::ExifMetadata>(exifData);

    mainPixelMap_->SetExifMetadata(exifMetadata);
    return SUCCESS;
}

bool Picture::SetMaintenanceData(sptr<SurfaceBuffer> &surfaceBuffer)
{
    if (surfaceBuffer == nullptr) {
        return false;
    }

    auto size = surfaceBuffer->GetSize();
    if (size <= 0) {
        IMAGE_LOGE("Invalid buffer size: %d.", size);
        return false;
    }

    maintenanceData_ = std::shared_ptr<uint8_t[]>(new uint8_t(size));
    if (!maintenanceData_) {
        return false;
    }

    auto ret = memcpy_s(maintenanceData_.get(), size, surfaceBuffer->GetVirAddr(), size);
    if (ret != EOK) {
        IMAGE_LOGE("Memmoy copy failed, errono: %d.", ret);
        return false;
    }

    return true;
}

std::shared_ptr<uint8_t[]> Picture::GetMaintenanceData() const
{
    return maintenanceData_;
}

} // namespace Media
} // namespace OHOS
