#include "jpeg_image_accessor.h"

#include "data_buf.h"
#include "file_image_stream.h"
#include "image_log.h"
#include "tiff_parser.h"
#include "libexif/exif-data.h"

#undef LOG_DOMAIN
#define LOG_DOMAIN LOG_TAG_DOMAIN_ID_IMAGE

#undef LOG_TAG
#define LOG_TAG "JpegImageAccessor"

namespace OHOS {
namespace Media {

using uint_8 = byte;
// constexpr byte JPEG_MARKER_APP0 = 0xe0;
constexpr byte JPEG_MARKER_APP1 = 0xe1;
// constexpr byte JPEG_MARKER_SOI = 0xd8;
constexpr byte JPEG_MARKER_EOI = 0xd9;
constexpr byte JPEG_MARKER_RST1 = 0xd0;
constexpr byte JPEG_MARKER_SOS = 0xda;
constexpr auto EXIF_ID = "Exif\0\0";
constexpr auto EXIF_BLOB_OFFSET = 2;

JpegImageAccessor::JpegImageAccessor(std::unique_ptr<ImageStream> &stream)
    : AbstractImageAccessor(std::move(stream))
{

}

JpegImageAccessor::~JpegImageAccessor()
{

}

int JpegImageAccessor::FindNextMarker() const
{
    int marker = -1;
    byte ch = 0;
    ssize_t result = imageStream_->Read(&ch, 1);
    while (ch != 0xff) {
        result = imageStream_->Read(&ch, 1);
        if (static_cast<int>(ch) == EOF) {
            return marker;
        }
    }

    if (ch == 0xff) {
        result = imageStream_->Read(&ch, 1);
    }

    return static_cast<int>(ch);
}

bool HasLength(byte marker)
{
    if ( (marker >= JPEG_MARKER_RST1) && (marker <= JPEG_MARKER_EOI) ) {
        return false;
    } else {
        return true;
    }
}

std::pair<std::array<byte, 2>, uint16_t> JpegImageAccessor::ReadSegmentSize(uint8_t marker) const
{
    std::array<byte, 2> buf{0, 0};
    uint16_t size{0};
    if (HasLength(marker)) {
        imageStream_->Read(buf.data(), buf.size());
        size = getUShort(buf.data(), bigEndian);
    }
    return {buf, size};
}

int JpegImageAccessor::ReadMetadata() const
{
    if (!imageStream_->Open()) {
        IMAGE_LOGE("Image stream open failed");
        return 1;
    }

    ExifData *exifData;

    int marker = FindNextMarker();
    if (marker == EOF) {
        return 1;
    }

    while (marker != JPEG_MARKER_SOS && marker != JPEG_MARKER_EOI) {
        const auto [sizebuf, size] = ReadSegmentSize(marker);

        DataBuf buf(size);
        if (size > 2) {
            imageStream_->Read(buf.Data(2), size - 2);
            std::copy(sizebuf.begin(), sizebuf.end(), buf.begin());
        }

        if (marker == JPEG_MARKER_APP1 && size >= 8) {
            if (buf.CmpBytes(2, EXIF_ID, 6) == 0) {
                TiffParser::DecodeJpegExif(reinterpret_cast<const unsigned char *>(buf.C_Data(EXIF_BLOB_OFFSET)),
                                           buf.size() - EXIF_BLOB_OFFSET, &exifData);
            }
        }

        marker = FindNextMarker();
    }

    // std::shared_ptr<ExifData> exifDataPtr(exifData);
    // exifMetadata_ = std::make_shared<ExifMetadata>(exifDataPtr);

    return 0;
}

} // namespace Media
} // namespace OHOS
