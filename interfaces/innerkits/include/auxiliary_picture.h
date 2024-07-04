//
// Created by canx on 6/20/24
//

#ifndef INTERFACE_INNERKITS_INCLUDE_AUXILIARY_PICTURE_H_
#define INTERFACE_INNERKITS_INCLUDE_AUXILIARY_PICTURE_H_

#include "pixel_map.h"
#include "surface_buffer.h"

namespace OHOS {
namespace Media {
class AuxiliaryPicture: public Parcelable {
public:
    static std::unique_ptr<AuxiliaryPicture> Create(std::shared_ptr<PixelMap> &content, AuxiliaryPictureType type, Size size = {0, 0});
    static std::unique_ptr<AuxiliaryPicture> Create(sptr<SurfaceBuffer> &surfaceBuffer, AuxiliaryPictureType type, Size size = {0, 0});
    AuxiliaryPictureType GetType();
    virtual Size GetSize();
    virtual std::shared_ptr<PixelMap> GetContentPixel();
    uint32_t ReadPixels(const uint64_t &bufferSize, uint8_t *dst);
    uint32_t WritePixels(const uint8_t *source, const uint64_t &bufferSize);
    virtual bool Marshalling(Parcel &data) const override;
    static AuxiliaryPicture *Unmarshalling(Parcel &data);
    static AuxiliaryPicture *Unmarshalling(Parcel &parcel, PIXEL_MAP_ERR &error);

protected:
    std::shared_ptr<PixelMap> content_;
    AuxiliaryPictureType type_ = AuxiliaryPictureType::NONE;
};

class GainmapAuxiliaryPicture: public AuxiliaryPicture {
    virtual std::shared_ptr<PixelMap> GetContentPixel() override;
};
}
}

#endif //INTERFACE_INNERKITS_INCLUDE_AUXILIARY_PICTURE_H_