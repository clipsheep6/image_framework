#include "abstract_image_accessor.h"
#include <optional>

#undef LOG_DOMAIN
#define LOG_DOMAIN LOG_TAG_DOMAIN_ID_IMAGE

#undef LOG_TAG
#define LOG_TAG "AbstractImageAccessor"

namespace OHOS {
namespace Media {

AbstractImageAccessor::AbstractImageAccessor(std::shared_ptr<ImageStream> &stream) : imageStream_(stream)
{

}

AbstractImageAccessor::~AbstractImageAccessor()
{

}

bool AbstractImageAccessor::CreateExifMetadata()
{
    exifMetadata_ = std::make_shared<ExifMetadata>();
    return exifMetadata_->CreateExifdata();
}

bool AbstractImageAccessor::WriteToOutput(ImagePlugin::OutputDataStream &output)
{
    if (imageStream_ == nullptr) {
        return false;
    }

    output.Write(imageStream_->GetAddr(), imageStream_->GetSize());
    return true;
}

std::shared_ptr<ExifMetadata> AbstractImageAccessor::GetExifMetadata()
{
    return exifMetadata_;
}

// void AbstractImageAccessor::WriteExifBlob(DataBuf& blob)
// {
//     (void)blob;
// }

} // namespace Media
} // namespace OHOS
