#ifndef JPEG_IMAGE_ACCESSOR_H_
#define JPEG_IMAGE_ACCESSOR_H_

#include <tuple>

#include "abstract_image_accessor.h"
#include "buffer_image_stream.h"
#include "image_stream.h"
#include "data_buf.h"

namespace OHOS {
namespace Media {
class JpegImageAccessor : public AbstractImageAccessor {
public:
    JpegImageAccessor(std::shared_ptr<ImageStream>& stream);
    ~JpegImageAccessor();

    virtual uint32_t ReadMetadata() override;
    virtual bool ReadExifBlob(DataBuf& blob) const override;
    virtual uint32_t WriteMetadata() override;
    virtual uint32_t WriteExifBlob(DataBuf& blob) override;

private:
    int FindNextMarker() const;
    std::pair<std::array<byte, 2>, uint16_t> ReadSegmentLength(uint8_t marker) const;
    DataBuf ReadNextSegment(byte marker);
    bool GetExifEncodeBlob(uint8_t** dataBlob, uint32_t& size);
    bool GetExifBlob(const DataBuf& blob, uint8_t** dataBlob, uint32_t& size);
    bool OpenOrSeek();
    bool WriteHeader(BufferImageStream& tempStream);
    std::tuple<size_t, size_t> GetInsertPosAndMarkerAPP1();
    bool WriteSegment(BufferImageStream& bufStream, uint8_t marker, const DataBuf& buf);
    bool CopyRestData(BufferImageStream& bufStream);
    bool WriteData(BufferImageStream& bufStream, uint8_t* dataBlob, uint32_t size);
    bool UpdateExifMetadata(BufferImageStream& tempStream, uint8_t* dataBlob, uint32_t size);
    bool UpdateData(uint8_t* dataBlob, uint32_t size);
};
} // namespace Media
} // namespace OHOS

#endif