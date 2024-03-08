#include "webp_image_accessor.h"

namespace OHOS {
namespace Media {

WebpImageAccessor::WebpImageAccessor()
{

}

WebpImageAccessor::~WebpImageAccessor()
{

}

ExifMetadata WebpImageAccessor::ReadMetadata(OHOS::Media::ImageStream& stream)
{
    (void)stream;
    return ExifMetadata();
}

} // namespace Media
} // namespace OHOS