#include "png_image_accessor.h"

namespace OHOS {
namespace Media {

PngImageAccessor::PngImageAccessor(std::shared_ptr<ImageStream> &stream)
    : AbstractImageAccessor(stream)
{

}

PngImageAccessor::~PngImageAccessor()
{

}

int PngImageAccessor::ReadMetadata()
{
    return 0;
}

bool PngImageAccessor::WriteMetadata()
{
    return false;
}

bool PngImageAccessor::ReadExifBlob(DataBuf &blob) const
{
    return false;
}

bool PngImageAccessor::WriteExifBlob(DataBuf &blob)
{
    return false;
}

} // namespace Media
} // namespace OHOS
