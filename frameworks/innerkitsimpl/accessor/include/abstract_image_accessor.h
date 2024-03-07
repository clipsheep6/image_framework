#ifndef ABSTRACT_IMAGE_ACCESSOR_H_
#define ABSTRACT_IMAGE_ACCESSOR_H_

#include "image_accessor.h"

namespace OHOS {
namespace Media {
class AbstractImageAccessor : public ImageAccessor {
public:
    AbstractImageAccessor();
    virtual ~AbstractImageAccessor();

    ExifMetadata ReadMetadata(OHOS::Media::ImageStream& stream) override;
    void WriteMetadata(const ExifMetadata& metadata) override;
};
} // namespace Media
} // namespace OHOS

#endif