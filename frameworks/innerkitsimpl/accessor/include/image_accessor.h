#ifndef IMAGE_ACCESSOR_H_
#define IMAGE_ACCESSOR_H_

#include "exif_metadata.h"
#include "source_stream.h"

namespace OHOS {
namespace Media {
class ImageAccessor {
public:
    virtual ExifMetadata ReadMetadata(const OHOS::Media::SourceStream& stream) const = 0;
    virtual void WriteMetadata(const ExifMetadata& metadata) = 0;
};
} // namespace Media
} // namespace OHOS

#endif