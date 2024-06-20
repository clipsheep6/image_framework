#include "picture.h"

namespace OHOS {
namespace Media {
std::unique_ptr<Picture> Picture::Create(std::shared_ptr<PixelMap> &pixelMap)
{
    std::unique_ptr<Picture> dstPicture = std::make_unique<Picture>();
    dstPicture->mainPixelMap_ = pixelMap;
    return dstPicture;
}

std::shared_ptr<PixelMap> Picture::GetMainPixel()
{
    return mainPixelMap_;
}

std::unique_ptr<PixelMap> Picture::GetHdrComposedPixelMap()
{
    return nullptr;
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

}
}
