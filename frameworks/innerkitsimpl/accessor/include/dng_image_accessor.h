#ifndef PNG_IMAGE_ACCESSOR_H_
#define PNG_IMAGE_ACCESSOR_H_

#include "abstract_image_accessor.h"

namespace OHOS {
namespace Media {
class DngImageAccessor : public AbstractImageAccessor {
public:
    DngImageAccessor();
    ~DngImageAccessor();

    ExifMetadata ReadMetadata(OHOS::Media::ImageStream& stream) override;
};
} // namespace Media
} // namespace OHOS

#endif