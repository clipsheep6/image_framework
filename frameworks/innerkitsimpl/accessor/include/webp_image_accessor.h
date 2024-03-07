#ifndef WEBP_IMAGE_ACCESSOR_H_
#define WEBP_IMAGE_ACCESSOR_H_

#include "abstract_image_accessor.h"

namespace OHOS {
namespace Media {
class WebpImageAccessor : public AbstractImageAccessor {
public:
    WebpImageAccessor();
    ~WebpImageAccessor();

    ExifMetadata ReadMetadata(const OHOS::Media::SourceStream& stream) const override;
};
} // namespace Media
} // namespace OHOS

#endif