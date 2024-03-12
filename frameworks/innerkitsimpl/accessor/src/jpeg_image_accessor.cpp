#include <iostream>
#include "jpeg_image_accessor.h"
#include "file_image_stream.h"
#include "image_log.h"
#include "media_errors.h"
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
constexpr auto EXIF_ID_SIZE = 6;
constexpr auto EXIF_HEAD_SIZE = 8;

JpegImageAccessor::JpegImageAccessor(std::shared_ptr<ImageStream> &stream)
    : AbstractImageAccessor(stream)
{

}

JpegImageAccessor::~JpegImageAccessor()
{

}

bool JpegImageAccessor::WriteMetadata()
{
    return 0;
}


bool JpegImageAccessor::WriteExifBlob(DataBuf &blob)
{
    return false;
}

int JpegImageAccessor::ReadMetadata()
{
    DataBuf exifBuf;
    if (!ReadExifBlob(exifBuf)) {
        return IMAGE_RESULT_READ_EXIFBLOB_FAILED;
    }

    ExifData *exifData;
    TiffParser::DecodeJpegExif(reinterpret_cast<const unsigned char *>(exifBuf.C_Data(EXIF_BLOB_OFFSET)),
                               (exifBuf.size() - EXIF_BLOB_OFFSET), &exifData);
    if (exifData == nullptr) {
        return IMAGE_RESULT_DENCODE_EXIF_FAILED;
    }

    exifMetadata_ = std::make_shared<OHOS::Media::ExifMetadata>(exifData);

    return SUCCESS;
}

bool JpegImageAccessor::ReadExifBlob(DataBuf &blob) const
{
    if (!imageStream_->Open()) {
        IMAGE_LOGE("Image stream open failed");
        return false;
    }

    int marker = FindNextMarker();
    if (marker == EOF) {
        IMAGE_LOGE("Image stream doesnot find marker 0xff");
        return false;
    }

    while (marker != JPEG_MARKER_SOS && marker != JPEG_MARKER_EOI) {
        const auto [sizebuf, size] = ReadSegmentSize(marker);

        if (marker == JPEG_MARKER_APP1 && size >= EXIF_HEAD_SIZE) {
            blob.Resize(size);
            imageStream_->Read(blob.Data(EXIF_BLOB_OFFSET), (size - EXIF_BLOB_OFFSET));
            std::copy(sizebuf.begin(), sizebuf.end(), blob.begin());
            if (blob.CmpBytes(EXIF_BLOB_OFFSET, EXIF_ID, EXIF_ID_SIZE) == 0) {
                break;
            }
        }

        marker = FindNextMarker();
        if (marker == EOF) {
            IMAGE_LOGE("Image stream doesnot have Exif");
            return false;
        }
    }
    return true;
}

int JpegImageAccessor::FindNextMarker() const
{
    int marker = -1;
    do {
        marker = imageStream_->ReadByte();
        if (marker == EOF) {
            return marker;
        }
    } while(marker != 0xff);

    do {
        marker = imageStream_->ReadByte();
        if (marker == EOF) {
            return marker;
        }
    } while(marker == 0xff);

    return marker;
}

bool HasLength(byte marker)
{
    if ( (marker >= JPEG_MARKER_RST1) && (marker <= JPEG_MARKER_EOI) ) {
        return false;
    }
    return true;
}

std::pair<std::array<byte, 2>, uint16_t> JpegImageAccessor::ReadSegmentSize(uint8_t marker) const
{
    std::array<byte, 2> buf{0, 0};
    uint16_t size{0};
    if (HasLength(marker)) {
        if ( imageStream_->Read(buf.data(), buf.size()) == -1 ) {
            IMAGE_LOGE("Image stream read failed");
            return {buf, size};
        }
        size = getUShort(buf.data(), bigEndian);
    }
    return {buf, size};
}

} // namespace Media
} // namespace OHOS
