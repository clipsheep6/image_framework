//
// Created by canx on 6/20/24.
//

#ifndef INTERFACES_INNERKITS_INCLUDE_AUXILIARY_PICTURE_H_
#define INTERFACES_INNERKITS_INCLUDE_AUXILIARY_PICTURE_H_

#include "pixel_map.h"

namespace OHOS {
namespace Media {
class AuxiliaryPicture {
public:
    static std::unique_ptr<AuxiliaryPicture> Create(std::shared_ptr<PixelMap> &content);
    void SetType(AuxiliaryPictureType type);
    AuxiliaryPictureType GetType();
    Size GetSize();
    uint32_t ReadPixels(const uint64_t &bufferSize, uint8_t *dst);
    uint32_t WritePixels(const uint8_t *source, const uint64_t &bufferSize);

private:
    std::shared_ptr<PixelMap> content_;
    AuxiliaryPictureType type_ = AuxiliaryPictureType::NONE;
};
}
}

#endif //INTERFACES_INNERKITS_INCLUDE_AUXILIARY_PICTURE_H_
