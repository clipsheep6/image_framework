#ifndef JPEG_IMAGE_ACCESSOR_H_
#define JPEG_IMAGE_ACCESSOR_H_

#include "abstract_image_accessor.h"
#include "image_stream.h"
#include "data_buf.h"

namespace OHOS {
namespace Media {
class JpegImageAccessor : public AbstractImageAccessor {
public:
    JpegImageAccessor(std::shared_ptr<ImageStream> &stream);
    ~JpegImageAccessor();

    virtual int ReadMetadata() override;
    virtual bool WriteMetadata() override;
    virtual bool ReadExifBlob(DataBuf &blob) const override;
    virtual bool WriteExifBlob(DataBuf &blob) override;
private:
    int FindNextMarker() const;
    std::pair<std::array<byte, 2>, uint16_t> ReadSegmentSize(uint8_t marker) const;

};
} // namespace Media
} // namespace OHOS

#endif
