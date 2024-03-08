#include "heif_image_accessor.h"

namespace OHOS {
namespace Media {

HeifImageAccessor::HeifImageAccessor()
{

}

HeifImageAccessor::~HeifImageAccessor()
{

}

ExifMetadata HeifImageAccessor::ReadMetadata(OHOS::Media::ImageStream& stream)
{
    (void)stream;

    return ExifMetadata();
}

} // namespace Media
} // namespace OHOS