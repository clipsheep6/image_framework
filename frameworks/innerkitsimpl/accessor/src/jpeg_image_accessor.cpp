#include "jpeg_image_accessor.h"

namespace OHOS {
namespace Media {

JpegImageAccessor::JpegImageAccessor()
{

}

JpegImageAccessor::~JpegImageAccessor()
{

}

ExifMetadata JpegImageAccessor::ReadMetadata(const OHOS::Media::SourceStream& stream) const
{
    (void)stream;
    return ExifMetadata();
}

} // namespace Media
} // namespace OHOS