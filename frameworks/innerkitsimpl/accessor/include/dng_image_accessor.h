#ifndef PNG_IMAGE_ACCESSOR_H_
#define PNG_IMAGE_ACCESSOR_H_

#include "abstract_image_accessor.h"

namespace OHOS {
namespace Media {
class DngImageAccessor : public AbstractImageAccessor {
public:
    DngImageAccessor();
    ~DngImageAccessor();

    ExifMetadata ReadMetadata(const OHOS::Media::SourceStream& stream) const override;
};
} // namespace Media
} // namespace OHOS

#endif