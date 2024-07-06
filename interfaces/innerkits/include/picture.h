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

#ifndef INTERFACES_INNERKITS_INCLUDE_PICTURE_H_
#define INTERFACES_INNERKITS_INCLUDE_PICTURE_H_

#include "pixel_map.h"
#include "auxiliary_picture.h"
#include <map>

namespace OHOS {
    class SurfaceBuffer;
}

namespace OHOS {
namespace Media {
class Picture: public Parcelable {
public:
    static std::unique_ptr<Picture> Create(std::shared_ptr<PixelMap> &PixelMap);
    static std::unique_ptr<Picture> Create(sptr<SurfaceBuffer> &surfaceBuffer);
    static std::shared_ptr<PixelMap> SurfaceBuffer2PixelMap(sptr<SurfaceBuffer> &surfaceBuffer);
    std::shared_ptr<PixelMap> GetMainPixel();
    void SetMainPixel(std::shared_ptr<PixelMap> PixelMap);
    std::unique_ptr<PixelMap> GetHdrComposedPixelMap();
    std::shared_ptr<AuxiliaryPicture> GetAuxiliaryPicture(AuxiliaryPictureType type);
    void SetAuxiliaryPicture(AuxiliaryPictureType type, std::shared_ptr<AuxiliaryPicture> &picture);
    bool HasAuxiliaryPicture(AuxiliaryPictureType type);
    virtual bool Marshalling(Parcel &data) const override;
    static Picture *Unmarshalling(Parcel &data);
    static Picture *Unmarshalling(Parcel &data, PIXEL_MAP_ERR &error);
private:
    std::shared_ptr<PixelMap> mainPixelMap_;
    std::map<AuxiliaryPictureType, std::shared_ptr<AuxiliaryPicture>> auxiliaryPictures_;
};
}
}

#endif //INTERFACES_INNERKITS_INCLUDE_PICTURE_H_