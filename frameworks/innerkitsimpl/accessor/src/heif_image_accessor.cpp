#include "heif_image_accessor.h"

#undef LOG_DOMAIN
#define LOG_DOMAIN LOG_TAG_DOMAIN_ID_IMAGE

#undef LOG_TAG
#define LOG_TAG "HeifImageAccessor"

namespace OHOS {
namespace Media {

HeifImageAccessor::HeifImageAccessor(std::shared_ptr<ImageStream> &stream)
    : AbstractImageAccessor(stream)
{

}

HeifImageAccessor::~HeifImageAccessor()
{

}

uint32_t HeifImageAccessor::ReadMetadata()
{
    return 0;
}

uint32_t HeifImageAccessor::WriteMetadata()
{
    return 0;
}

bool HeifImageAccessor::ReadExifBlob(DataBuf &blob) const
{
    return false;
}

uint32_t HeifImageAccessor::WriteExifBlob(DataBuf &blob)
{
    return 0;
}

} // namespace Media
} // namespace OHOS
