#include "picture.h"
#include "auxiliary_picture.h"
#include "media_errors.h"

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