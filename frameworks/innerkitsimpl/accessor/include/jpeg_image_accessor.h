#ifndef JPEG_IMAGE_ACCESSOR_H_
#define JPEG_IMAGE_ACCESSOR_H_

#include "abstract_image_accessor.h"

namespace OHOS {
namespace Media {
class JpegImageAccessor : public AbstractImageAccessor {
public:
    JpegImageAccessor();
    ~JpegImageAccessor();

    ExifMetadata ReadMetadata(const OHOS::Media::SourceStream& stream) const override;
};
} // namespace Media
} // namespace OHOS

#endif