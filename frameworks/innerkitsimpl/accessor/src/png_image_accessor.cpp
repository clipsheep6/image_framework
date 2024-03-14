#include "png_image_accessor.h"
#include "image_stream.h"
#include "image_log.h"
#include "data_buf.h"
#include "tiff_parser.h"
#include "exif_metadata.h"
#include "libexif/exif-data.h"
#include "png_image_chunk.h"
#include "media_errors.h"

#undef LOG_DOMAIN
#define LOG_DOMAIN LOG_TAG_DOMAIN_ID_IMAGE

#undef LOG_TAG
#define LOG_TAG "PngImageAccessor"

namespace OHOS {
namespace Media {

PngImageAccessor::PngImageAccessor(std::shared_ptr<ImageStream> &stream)
    : AbstractImageAccessor(stream)
{

}

PngImageAccessor::~PngImageAccessor()
{

}

bool PngImageAccessor::IsPngType() const
{
    if (imageStream_->IsEof()) {
        return false;
    }
    const int32_t len = PNG_SIGN_SIZE;
    byte buf[len];
    imageStream_->Read(buf, len);
    if (imageStream_->IsEof()) {
        return false;
    }

    return !memcmp(buf, pngSignature, PNG_SIGN_SIZE);
}

size_t PngImageAccessor::ReadChunk(DataBuf &buffer) const
{
    return imageStream_->Read(buffer.Data(), buffer.Size());
}

int32_t PngImageAccessor::TextFindTiff(const DataBuf &data, const std::string chunkType, DataBuf &tiffData) const
{
    PngImageChunk::TextChunkType txtType;
    if (chunkType == PNG_CHUNK_TEXT) {
        txtType = PngImageChunk::tEXt_Chunk;
    } else if (chunkType == PNG_CHUNK_ZTXT) {
        txtType = PngImageChunk::zTXt_Chunk;
    } else if (chunkType == PNG_CHUNK_ITXT) {
        txtType = PngImageChunk::iTXt_Chunk;
    } else {
        return -1;
    }

    return PngImageChunk::ParseTextChunk(data, txtType, tiffData);
}

bool PngImageAccessor::ReadExifBlob(DataBuf &blob) const
{
    if (!imageStream_->IsOpen()) {
        if (!imageStream_->Open(OpenMode::ReadWrite)) {
            IMAGE_LOGE("Output image stream open failed");
            return false;
        }
    } else {
        imageStream_->Seek(0, SeekPos::BEGIN);
    }

    if (!IsPngType()) {
        IMAGE_LOGE("PngImageAccessor::ReadMetadata: Is not a PNG file.");
        return false;
    }

    const size_t imgSize = imageStream_->GetSize();
    DataBuf chunkHead(PNG_CHUNK_HEAD_SIZE);

    while (!imageStream_->IsEof()) {
        if (ReadChunk(chunkHead) != chunkHead.Size()) {
            IMAGE_LOGE("PngImageAccessor::ReadMetadata: Read chunk head error.");
            return false;
        }
        uint32_t chunkLength = chunkHead.ReadUInt32(0, bigEndian);
        if (chunkLength > imgSize - imageStream_->Tell()) {
            IMAGE_LOGE("PngImageAccessor::ReadMetadata: Read chunk length error.");
            return false;
        }
        std::string chunkType(reinterpret_cast<const char*>(chunkHead.CData(PNG_CHUNK_LENGTH_SIZE)), PNG_CHUNK_TYPE_SIZE);
        if (chunkType == PNG_CHUNK_IEND || chunkType == PNG_CHUNK_TEXT || chunkType == PNG_CHUNK_ZTXT ||
            chunkType == PNG_CHUNK_EXIF || chunkType == PNG_CHUNK_ITXT) {
            DataBuf chunkData(chunkLength);
            if (chunkLength > 0) {
                if (ReadChunk(chunkData) != chunkData.Size()) {
                    IMAGE_LOGE("PngImageAccessor::ReadMetadata: Read chunk data error.");
                    return false;
                }
            }

            if (chunkType == PNG_CHUNK_EXIF) {
                blob = chunkData;
                break;
            } else if (chunkType == PNG_CHUNK_IEND) {
                return false;
            } else {
                if (TextFindTiff(chunkData, chunkType, blob) == 0) {
                   break;
                }
            }
            chunkLength = 0;
        }

        imageStream_->Seek(chunkLength + PNG_CHUNK_CRC_SIZE, CURRENT);
        if (imageStream_->IsEof()) {
            IMAGE_LOGE("PngImageAccessor::ReadMetadata: Read file error.");
            return false;
        }
    }
    return true;
}

uint32_t PngImageAccessor::ReadMetadata()
{
    DataBuf tiffBuf;
    if (!ReadExifBlob(tiffBuf)) {
        IMAGE_LOGE("ReadExifBlob error.");
        return ERR_IMAGE_SOURCE_DATA;
    }
    ExifData *exifData;
    size_t byteOrderPos = PngImageChunk::FindTiffPos(tiffBuf, tiffBuf.Size());
    if (byteOrderPos == std::numeric_limits<size_t>::max()) {
        IMAGE_LOGE("Error:failed to parse Exif metadata: cannot find tiff byte order");
        return ERR_IMAGE_SOURCE_DATA;
    }
    TiffParser::Decode(tiffBuf.CData(), tiffBuf.Size(), &exifData);
    if (exifData == nullptr) {
        IMAGE_LOGE("Decode tiffBuf error.");
        return ERR_IMAGE_DECODE_FAILED;
    }

    exifMetadata_ = std::make_shared<OHOS::Media::ExifMetadata>(exifData);
    return SUCCESS;
}

uint32_t PngImageAccessor::WriteMetadata()
{
    return ERR_IMAGE_DATA_UNSUPPORT;
}

uint32_t PngImageAccessor::WriteExifBlob(DataBuf &blob)
{
    return ERR_IMAGE_DATA_UNSUPPORT;
}
} // namespace Media
} // namespace OHOS
