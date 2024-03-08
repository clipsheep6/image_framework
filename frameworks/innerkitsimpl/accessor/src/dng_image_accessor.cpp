#include "dng_image_accessor.h"

namespace OHOS {
namespace Media {

DngImageAccessor::DngImageAccessor()
{

}

DngImageAccessor::~DngImageAccessor()
{

}

ExifMetadata DngImageAccessor::ReadMetadata(OHOS::Media::ImageStream& stream)
{
    (void)stream;

    return ExifMetadata();
}

} // namespace Media
} // namespace OHOS