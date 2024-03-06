#include "abstract_image_accessor.h"

namespace OHOS {
namespace Media {

AbstractImageAccessor::AbstractImageAccessor()
{

}

AbstractImageAccessor::~AbstractImageAccessor()
{

}

ExifMetadata AbstractImageAccessor::ReadMetadata(const OHOS::Media::SourceStream& stream) const
{
    (void)stream;
    return ExifMetadata();
}

void AbstractImageAccessor::WriteMetadata(const ExifMetadata& metadata)
{
    (void)metadata;
    return;
}

} // namespace Media
} // namespace OHOS