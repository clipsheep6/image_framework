#ifndef JPEG_IMAGE_ACCESSOR_H_
#define JPEG_IMAGE_ACCESSOR_H_

#include "abstract_image_accessor.h"
#include "image_stream.h"

namespace OHOS {
namespace Media {
class JpegImageAccessor : public AbstractImageAccessor {
public:
    JpegImageAccessor(std::unique_ptr<ImageStream> &stream);
    ~JpegImageAccessor();

    virtual int ReadMetadata() const override;

private:
    int FindNextMarker() const;
    std::pair<std::array<byte, 2>, uint16_t> ReadSegmentSize(uint8_t marker) const;

};
} // namespace Media
} // namespace OHOS

#endif
