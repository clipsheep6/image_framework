#ifndef HEIF_IMAGE_ACCESSOR_H_
#define HEIF_IMAGE_ACCESSOR_H_

#include "abstract_image_accessor.h"

namespace OHOS {
namespace Media {
class HeifImageAccessor : public AbstractImageAccessor {
public:
    HeifImageAccessor();
    ~HeifImageAccessor();

    ExifMetadata ReadMetadata(OHOS::Media::ImageStream& stream) override;
};
} // namespace Media
} // namespace OHOS

#endif