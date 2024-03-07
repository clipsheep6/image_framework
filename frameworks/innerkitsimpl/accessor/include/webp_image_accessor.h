#ifndef WEBP_IMAGE_ACCESSOR_H_
#define WEBP_IMAGE_ACCESSOR_H_

#include "abstract_image_accessor.h"

namespace OHOS {
namespace Media {
class WebpImageAccessor : public AbstractImageAccessor {
public:
    WebpImageAccessor();
    ~WebpImageAccessor();

    ExifMetadata ReadMetadata(OHOS::Media::ImageStream& stream) override;
};
} // namespace Media
} // namespace OHOS

#endif