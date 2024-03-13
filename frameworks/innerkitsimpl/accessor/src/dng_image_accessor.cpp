#include "dng_image_accessor.h"

namespace OHOS {
namespace Media {

DngImageAccessor::DngImageAccessor(std::shared_ptr<ImageStream> &stream)
    : AbstractImageAccessor(stream)
{

}

DngImageAccessor::~DngImageAccessor()
{

}

int DngImageAccessor::ReadMetadata()
{
    return 0;
}

bool DngImageAccessor::WriteMetadata()
{
    return false;
}

bool DngImageAccessor::ReadExifBlob(DataBuf &blob) const
{
    return false;
}

bool DngImageAccessor::WriteExifBlob(DataBuf &blob)
{
    return false;
}

} // namespace Media
} // namespace OHOS
