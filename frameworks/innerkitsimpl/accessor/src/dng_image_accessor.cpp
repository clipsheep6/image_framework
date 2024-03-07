#include "dng_image_accessor.h"

namespace OHOS {
namespace Media {

DngImageAccessor::DngImageAccessor()
{

}

DngImageAccessor::~DngImageAccessor()
{

}

ExifMetadata DngImageAccessor::ReadMetadata(const OHOS::Media::SourceStream& stream) const
{
    (void)stream;

    return ExifMetadata();
}

} // namespace Media
} // namespace OHOS