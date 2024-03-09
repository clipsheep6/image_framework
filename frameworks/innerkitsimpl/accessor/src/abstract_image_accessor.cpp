#include "abstract_image_accessor.h"

namespace OHOS {
namespace Media {

AbstractImageAccessor::AbstractImageAccessor(std::unique_ptr<ImageStream> &&stream) : imageStream_(std::move(stream))
{

}

AbstractImageAccessor::~AbstractImageAccessor()
{

}

int AbstractImageAccessor::ReadMetadata() const
{
    return 0;
}

bool AbstractImageAccessor::WriteMetadata()
{
    return true;
}

bool AbstractImageAccessor::ReadExifBlob(DataBuf &blob) const
{
    (void)blob;
    return true;
}

bool AbstractImageAccessor::WriteExifBlob(DataBuf &blob)
{
    (void)blob;
    return true;
}

std::shared_ptr<ExifMetadata> AbstractImageAccessor::GetExifMetadata()
{
    return exifMetadata_;
}

} // namespace Media
} // namespace OHOS
