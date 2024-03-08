#ifndef JPEG_IMAGE_ACCESSOR_H_
#define JPEG_IMAGE_ACCESSOR_H_

#include "abstract_image_accessor.h"
#include "image_stream.h"

namespace OHOS {
namespace Media {
class JpegImageAccessor : public AbstractImageAccessor {
public:
    JpegImageAccessor();
    ~JpegImageAccessor();

    ExifMetadata ReadMetadata(OHOS::Media::ImageStream& stream) override;

private:
    int FindNextMarker(ImageStream& imageStream);
    std::pair<std::array<byte, 2>, uint16_t> ReadSegmentSize(ImageStream& imageStream, uint8_t marker);
};
} // namespace Media
} // namespace OHOS

#endif