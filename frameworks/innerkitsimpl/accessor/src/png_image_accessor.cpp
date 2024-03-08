#include "png_image_accessor.h"

namespace OHOS {
namespace Media {

PngImageAccessor::PngImageAccessor()
{

}

PngImageAccessor::~PngImageAccessor()
{

}

ExifMetadata PngImageAccessor::ReadMetadata(OHOS::Media::ImageStream& stream)
{
    (void)stream;

    return ExifMetadata();
}

} // namespace Media
} // namespace OHOS