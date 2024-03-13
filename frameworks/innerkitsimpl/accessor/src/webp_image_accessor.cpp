#include "webp_image_accessor.h"

#undef LOG_DOMAIN
#define LOG_DOMAIN LOG_TAG_DOMAIN_ID_IMAGE

#undef LOG_TAG
#define LOG_TAG "WebpImageAccessor"

namespace OHOS {
namespace Media {

WebpImageAccessor::WebpImageAccessor(std::shared_ptr<ImageStream> &stream)
    : AbstractImageAccessor(stream)
{

}

WebpImageAccessor::~WebpImageAccessor()
{

}

uint32_t WebpImageAccessor::ReadMetadata()
{
    return 0;
}

uint32_t WebpImageAccessor::WriteMetadata()
{
    return 0;
}

bool WebpImageAccessor::ReadExifBlob(DataBuf &blob) const
{
    return false;
}

uint32_t WebpImageAccessor::WriteExifBlob(DataBuf &blob)
{
    return 0;
}

} // namespace Media
} // namespace OHOS
