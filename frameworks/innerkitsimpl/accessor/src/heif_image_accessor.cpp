#include "heif_image_accessor.h"

namespace OHOS {
namespace Media {

HeifImageAccessor::HeifImageAccessor()
{

}

HeifImageAccessor::~HeifImageAccessor()
{

}

ExifMetadata HeifImageAccessor::ReadMetadata(const OHOS::Media::SourceStream& stream) const
{
    (void)stream;

    return ExifMetadata();
}

} // namespace Media
} // namespace OHOS