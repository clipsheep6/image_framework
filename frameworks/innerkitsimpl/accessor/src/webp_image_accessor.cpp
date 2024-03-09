#include "webp_image_accessor.h"

namespace OHOS {
namespace Media {

WebpImageAccessor::WebpImageAccessor(std::unique_ptr<ImageStream> &stream)
    : AbstractImageAccessor(std::move(stream))
{

}

WebpImageAccessor::~WebpImageAccessor()
{

}

int WebpImageAccessor::ReadMetadata() const
{
    return 0;
}

} // namespace Media
} // namespace OHOS
