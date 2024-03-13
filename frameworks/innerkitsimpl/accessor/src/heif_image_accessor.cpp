#include "heif_image_accessor.h"

namespace OHOS {
namespace Media {

HeifImageAccessor::HeifImageAccessor(std::shared_ptr<ImageStream> &stream)
    : AbstractImageAccessor(stream)
{

}

HeifImageAccessor::~HeifImageAccessor()
{

}

int HeifImageAccessor::ReadMetadata()
{
    return 0;
}

bool HeifImageAccessor::WriteMetadata()
{
    return false;
}

bool HeifImageAccessor::ReadExifBlob(DataBuf &blob) const
{
    return false;
}

bool HeifImageAccessor::WriteExifBlob(DataBuf &blob)
{
    return false;
}

} // namespace Media
} // namespace OHOS
