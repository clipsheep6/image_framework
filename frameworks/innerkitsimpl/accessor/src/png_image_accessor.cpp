#include "png_image_accessor.h"

namespace OHOS {
namespace Media {

PngImageAccessor::PngImageAccessor()
{

}

PngImageAccessor::~PngImageAccessor()
{

}

ExifMetadata PngImageAccessor::ReadMetadata(const OHOS::Media::SourceStream& stream) const
{
    (void)stream;

    return ExifMetadata();
}

} // namespace Media
} // namespace OHOS