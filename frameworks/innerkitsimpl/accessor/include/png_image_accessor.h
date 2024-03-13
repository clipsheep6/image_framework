#ifndef PNG_IMAGE_ACCESSOR_H_
#define PNG_IMAGE_ACCESSOR_H_

#include "abstract_image_accessor.h"

#define PNG_CHUNK_IEND "IEND"
#define PNG_CHUNK_TEXT "tEXt"
#define PNG_CHUNK_ZTXT "zTXt"
#define PNG_CHUNK_ITXT "iTXt"
#define PNG_CHUNK_EXIF "eXIf"
#define PNG_CHUNK_HEAD_SIZE 8
#define PNG_CHUNK_LENGTH_SIZE 4
#define PNG_CHUNK_TYPE_SIZE 4
#define PNG_CHUNK_CRC_SIZE 4
#define PNG_SIGN_SIZE 8

namespace OHOS {
namespace Media {

constexpr unsigned char pngSignature[] = {
    0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a,
};

class PngImageAccessor : public AbstractImageAccessor {
public:
    PngImageAccessor(std::shared_ptr<ImageStream> &stream);
    ~PngImageAccessor();

    virtual uint32_t ReadMetadata() override;
    virtual uint32_t WriteMetadata() override;
    virtual bool ReadExifBlob(DataBuf &blob) const override;
    virtual uint32_t WriteExifBlob(DataBuf &blob) override;

private:
    bool IsPngType() const;
    size_t ReadChunk(DataBuf &buffer) const;
    int32_t TextFindTiff(const DataBuf &data, const std::string chunkType, DataBuf &tiffData) const;
};
} // namespace Media
} // namespace OHOS

#endif
