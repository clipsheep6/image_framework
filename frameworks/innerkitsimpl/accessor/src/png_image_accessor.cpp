#include "png_image_accessor.h"

namespace OHOS {
namespace Media {

PngImageAccessor::PngImageAccessor(std::unique_ptr<ImageStream> &stream)
    : AbstractImageAccessor(std::move(stream))
{

}

PngImageAccessor::~PngImageAccessor()
{

}

int PngImageAccessor::ReadMetadata() const
{
    return 0;
}

} // namespace Media
} // namespace OHOS
