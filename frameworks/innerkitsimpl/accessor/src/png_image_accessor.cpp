#include "png_image_accessor.h"

#undef LOG_DOMAIN
#define LOG_DOMAIN LOG_TAG_DOMAIN_ID_IMAGE

#undef LOG_TAG
#define LOG_TAG "PngImageAccessor"

namespace OHOS {
namespace Media {

PngImageAccessor::PngImageAccessor(std::shared_ptr<ImageStream> &stream)
    : AbstractImageAccessor(stream)
{

}

PngImageAccessor::~PngImageAccessor()
{

}

uint32_t PngImageAccessor::ReadMetadata()
{
    return 0;
}

uint32_t PngImageAccessor::WriteMetadata()
{
    return 0;
}

bool PngImageAccessor::ReadExifBlob(DataBuf &blob) const
{
    return false;
}

uint32_t PngImageAccessor::WriteExifBlob(DataBuf &blob)
{
    return 0;
}

} // namespace Media
} // namespace OHOS
