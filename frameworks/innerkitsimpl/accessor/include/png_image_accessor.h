#ifndef PNG_IMAGE_ACCESSOR_H_
#define PNG_IMAGE_ACCESSOR_H_

#include "abstract_image_accessor.h"

namespace OHOS {
namespace Media {
class PngImageAccessor : public AbstractImageAccessor {
public:
    PngImageAccessor();
    ~PngImageAccessor();

    ExifMetadata ReadMetadata(const OHOS::Media::SourceStream& stream) const override;
};
} // namespace Media
} // namespace OHOS

#endif