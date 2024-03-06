#include "webp_image_accessor.h"

namespace OHOS {
namespace Media {

WebpImageAccessor::WebpImageAccessor()
{

}

WebpImageAccessor::~WebpImageAccessor()
{

}

ExifMetadata WebpImageAccessor::ReadMetadata(const OHOS::Media::SourceStream& stream) const
{
    (void)stream;
    return ExifMetadata();
}

} // namespace Media
} // namespace OHOS