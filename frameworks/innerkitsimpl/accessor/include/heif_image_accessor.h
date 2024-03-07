#ifndef HEIF_IMAGE_ACCESSOR_H_
#define HEIF_IMAGE_ACCESSOR_H_

#include "abstract_image_accessor.h"

namespace OHOS {
namespace Media {
class HeifImageAccessor : public AbstractImageAccessor {
public:
    HeifImageAccessor();
    ~HeifImageAccessor();

    ExifMetadata ReadMetadata(const OHOS::Media::SourceStream& stream) const override;
};
} // namespace Media
} // namespace OHOS

#endif