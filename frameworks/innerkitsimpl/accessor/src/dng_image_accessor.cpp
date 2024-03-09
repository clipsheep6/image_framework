#include "dng_image_accessor.h"

namespace OHOS {
namespace Media {

DngImageAccessor::DngImageAccessor(std::unique_ptr<ImageStream> &stream)
    : AbstractImageAccessor(std::move(stream))
{

}

DngImageAccessor::~DngImageAccessor()
{

}

int DngImageAccessor::ReadMetadata() const
{
    return 0;
}

} // namespace Media
} // namespace OHOS
