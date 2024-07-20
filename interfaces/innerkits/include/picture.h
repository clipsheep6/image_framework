//
// Created by canx on 6/20/24.
//

#ifndef INTERFACES_INNERKITS_INCLUDE_PICTURE_H_
#define INTERFACES_INNERKITS_INCLUDE_PICTURE_H_

#include "pixel_map.h"
#include "auxiliary_picture.h"
#include <map>

namespace OHOS {
namespace Media {
class Picture {
public:
    static std::unique_ptr<Picture> Create(std::shared_ptr<PixelMap> &pixelMap);
    std::shared_ptr<PixelMap> GetMainPixel();
    std::unique_ptr<PixelMap> GetHdrComposedPixelMap();
    std::shared_ptr<AuxiliaryPicture> GetAuxiliaryPicture(AuxiliaryPictureType type);
    void SetAuxiliaryPicture(AuxiliaryPictureType type, std::shared_ptr<AuxiliaryPicture> &outPicture);
    bool HasAuxiliaryPicture(AuxiliaryPictureType type);
private:
    std::shared_ptr<PixelMap> mainPixelMap_;
    std::map<AuxiliaryPictureType, std::shared_ptr<AuxiliaryPicture>> auxiliaryPictures_;
};
}
}

#endif //INTERFACES_INNERKITS_INCLUDE_PICTURE_H_
