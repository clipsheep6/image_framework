#include "dng_image_accessor.h"

#undef LOG_DOMAIN
#define LOG_DOMAIN LOG_TAG_DOMAIN_ID_IMAGE

#undef LOG_TAG
#define LOG_TAG "DngImageAccessor"

namespace OHOS {
namespace Media {

DngImageAccessor::DngImageAccessor(std::shared_ptr<ImageStream> &stream)
    : AbstractImageAccessor(stream)
{

}

DngImageAccessor::~DngImageAccessor()
{

}

uint32_t DngImageAccessor::ReadMetadata()
{
    return 0;
}

uint32_t DngImageAccessor::WriteMetadata()
{
    return 0;
}

bool DngImageAccessor::ReadExifBlob(DataBuf &blob) const
{
    return false;
}

uint32_t DngImageAccessor::WriteExifBlob(DataBuf &blob)
{
    return 0;
}

} // namespace Media
} // namespace OHOS
