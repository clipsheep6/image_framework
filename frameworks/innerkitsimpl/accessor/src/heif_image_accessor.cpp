#include "heif_image_accessor.h"

namespace OHOS {
namespace Media {

HeifImageAccessor::HeifImageAccessor(std::unique_ptr<ImageStream> &stream)
    : AbstractImageAccessor(std::move(stream))
{

}

HeifImageAccessor::~HeifImageAccessor()
{

}

int HeifImageAccessor::ReadMetadata() const
{
    return 0;
}

} // namespace Media
} // namespace OHOS
