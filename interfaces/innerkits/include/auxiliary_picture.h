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

#ifndef INTERFACE_INNERKITS_INCLUDE_AUXILIARY_PICTURE_H_
#define INTERFACE_INNERKITS_INCLUDE_AUXILIARY_PICTURE_H_

#include "metadata.h"
#include "pixel_map.h"

namespace OHOS {
    class SurfaceBuffer;
}

namespace OHOS {
namespace Media {
class AuxiliaryPicture: public Parcelable {
public:
    AuxiliaryPicture() = default;
    virtual ~AuxiliaryPicture();

    NATIVEEXPORT static std::unique_ptr<AuxiliaryPicture> Create(std::shared_ptr<PixelMap> &content, AuxiliaryPictureType type, Size size = {0, 0});
    NATIVEEXPORT static std::unique_ptr<AuxiliaryPicture> Create(sptr<SurfaceBuffer> &surfaceBuffer, AuxiliaryPictureType type, Size size = {0, 0});
    NATIVEEXPORT AuxiliaryPictureType GetType();
    NATIVEEXPORT void SetType(AuxiliaryPictureType type);
    NATIVEEXPORT virtual Size GetSize();
    NATIVEEXPORT virtual std::shared_ptr<PixelMap> GetContentPixel();
    NATIVEEXPORT void SetContentPixel(std::shared_ptr<PixelMap> content);
    NATIVEEXPORT uint32_t ReadPixels(const uint64_t &bufferSize, uint8_t *dst);
    NATIVEEXPORT uint32_t WritePixels(const uint8_t *source, const uint64_t &bufferSize);
    NATIVEEXPORT std::shared_ptr<ImageMetadata> GetMetadata(MetadataType type);
    NATIVEEXPORT void SetMetadata(MetadataType type, std::shared_ptr<ImageMetadata> metadata);
    NATIVEEXPORT bool HasMetadata(MetadataType type);
    NATIVEEXPORT virtual bool Marshalling(Parcel &data) const override;
    NATIVEEXPORT static AuxiliaryPicture *Unmarshalling(Parcel &data);
    NATIVEEXPORT static AuxiliaryPicture *Unmarshalling(Parcel &parcel, PIXEL_MAP_ERR &error);

protected:
    std::shared_ptr<PixelMap> content_;
    AuxiliaryPictureType type_ = AuxiliaryPictureType::NONE;
    std::map<MetadataType, std::shared_ptr<ImageMetadata>> metadatas_;
};

class GainmapAuxiliaryPicture: public AuxiliaryPicture {
    virtual std::shared_ptr<PixelMap> GetContentPixel() override;
};
}
}

#endif //INTERFACE_INNERKITS_INCLUDE_AUXILIARY_PICTURE_H_