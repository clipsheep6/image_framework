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

#include "picture.h"
#include "auxiliary_picture.h"
#include "media_errors.h"
#include "surface_buffer.h"

namespace OHOS {
namespace Media {
std::unique_ptr<AuxiliaryPicture> AuxiliaryPicture::Create(std::shared_ptr<PixelMap> &content, AuxiliaryPictureType type, Size size)
{
    if (content == nullptr) {
        return nullptr;
    }
    std::unique_ptr<AuxiliaryPicture> dstAuxPicture = std::make_unique<AuxiliaryPicture>();
    dstAuxPicture->content_ = content;
    dstAuxPicture->type_ = type;
    return dstAuxPicture;
}

std::unique_ptr<AuxiliaryPicture> AuxiliaryPicture::Create(sptr<SurfaceBuffer> &surfaceBuffer, 
                                                           AuxiliaryPictureType type, Size size)
{
    std::shared_ptr<PixelMap> pixelMap = Picture::SurfaceBuffer2PixelMap(surfaceBuffer);
    return Create(pixelMap, type, size);
}

AuxiliaryPictureType AuxiliaryPicture::GetType()
{
    return type_;
}

AuxiliaryPictureType AuxiliaryPicture::SetType(OHOS::Media::AuxiliaryPicture type)
{
    type_ = type;
}

Size AuxiliaryPicture::GetSize()
{
    if(!content_){
        return {0, 0};
    }
    return {content_->GetWidth(), content_->GetHeight()};
}

std::shared_ptr<PixelMap> AuxiliaryPicture::GetContentPixel()
{
    return nullptr;
}

void AuxiliaryPicture::SetContentPixel(std::shared_ptr<PixelMap> content)
{
    content_ = content;
}

uint32_t AuxiliaryPicture::ReadPixels(const uint64_t &bufferSize, uint8_t *dst)
{
    if(content_ == nullptr) {
        return ERR_MEDIA_NULL_POINTER;
    }
    return content_->ReadPixels(bufferSize, dst);
}

uint32_t AuxiliaryPicture::WritePixels(const uint8_t *source, const uint64_t &bufferSize)
{
    if(content_ == nullptr) {
        return ERR_MEDIA_NULL_POINTER;
    }
    return content_->WritePixels(source, bufferSize);
}

std::shared_ptr<ImageMetadata> AuxiliaryPicture::GetMetadata(OH::Media::MetadataType type)
{
    auto iter = metadatas_.find(type);
    if (iter == metadatas_.end()) {
        return nullptr;
    }
    return iter->second;
}

void AuxiliaryPicture::SetMetadata(MetadataType type, std::shared_ptr<ImageMetadata> metadata)
{
    metadatas_[type] = metadata;
}

bool AuxiliaryPicture::HasMetadata(MetadataType type)
{
    return metadatas_.find(type) != metadatas_.end();
}

bool AuxiliaryPicture::Marshalling(Parcel &data) const
{
    return false;
}

AuxiliaryPicture *AuxiliaryPicture::Unmarshalling(Parcel &data)
{
    return nullptr;
}

AuxiliaryPicture *AuxiliaryPicture::Unmarshalling(Parcel &parcel, PIXEL_MAP_ERR &error)
{
    return nullptr;
}

std::shared_ptr<PixelMap> GainmapAuxiliaryPicture::GetContentPixel()
{
    return content_;
}

}
}