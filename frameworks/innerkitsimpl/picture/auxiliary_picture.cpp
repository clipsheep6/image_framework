#include "auxiliary_picture.h"
#include "media_errors.h"

namespace OHOS {
namespace Media {
std::unique_ptr<AuxiliaryPicture> AuxiliaryPicture::Create(std::shared_ptr<PixelMap> &content)
{
    std::unique_ptr<AuxiliaryPicture> dstAuxPicture = std::make_unique<AuxiliaryPicture>();
    dstAuxPicture->content_ = content;
    return dstAuxPicture;
}

void AuxiliaryPicture::SetType(OHOS::Media::AuxiliaryPictureType type)
{
    type_ = type;
}

AuxiliaryPictureType AuxiliaryPicture::GetType()
{
    return type_;
}

Size AuxiliaryPicture::GetSize()
{
    if (!content_) {
        return {0, 0};
    }
    return {content_->GetWidth(), content_->GetHeight()};
}

uint32_t AuxiliaryPicture::ReadPixels(const uint64_t &bufferSize, uint8_t *dst)
{
    if (content_ == nullptr) {
        return ERR_MEDIA_NULL_POINTER;
    }
    return content_->ReadPixels(bufferSize, dst);
}

uint32_t AuxiliaryPicture::WritePixels(const uint8_t *source, const uint64_t &bufferSize)
{
    if (content_ == nullptr) {
        return ERR_MEDIA_NULL_POINTER;
    }
    return content_->WritePixels(source, bufferSize);
}
}
}