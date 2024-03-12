#include "webp_image_accessor.h"

namespace OHOS {
namespace Media {

WebpImageAccessor::WebpImageAccessor(std::shared_ptr<ImageStream> &stream)
    : AbstractImageAccessor(stream)
{

}

WebpImageAccessor::~WebpImageAccessor()
{

}

int WebpImageAccessor::ReadMetadata()
{
    return 0;
}

bool WebpImageAccessor::WriteMetadata()
{
    return false;
}

bool WebpImageAccessor::ReadExifBlob(DataBuf &blob) const
{
    return false;
}

bool WebpImageAccessor::WriteExifBlob(DataBuf &blob)
{
    return false;
}

} // namespace Media
} // namespace OHOS
