/*
 * Copyright (C) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

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
namespace {
    using uint_8 = byte;
    constexpr byte JPEG_MARKER_APP0 = 0xe0;
    constexpr byte JPEG_MARKER_APP1 = 0xe1;
    constexpr byte JPEG_MARKER_SOI = 0xd8;
    constexpr byte JPEG_MARKER_EOI = 0xd9;
    constexpr byte JPEG_MARKER_RST1 = 0xd0;
    constexpr byte JPEG_MARKER_SOS = 0xda;
    constexpr auto EXIF_ID = "Exif\0\0";
    constexpr auto EXIF_BLOB_OFFSET = 2;
    constexpr auto EXIF_ID_LENGTH = 2;
    constexpr auto READ_BYTES = 2;
    constexpr auto EXIF_ID_SIZE = 6;
    constexpr auto APP1_EXIF_LENGTH = 8;
    constexpr auto APP1_HEADER_LENGTH = 10;
    constexpr auto MARKER_LENGTH_SIZE = 4;
    constexpr auto READ_WRITE_BLOCK = 4096;
    constexpr auto JPEG_MARKER_HEADER = 0xff;
    constexpr auto JPEG_DATA_MAX_SIZE = 0xffff;
}

JpegImageAccessor::JpegImageAccessor(std::shared_ptr<ImageStream>& stream)
    : AbstractImageAccessor(stream) {}

JpegImageAccessor::~JpegImageAccessor() {}

uint32_t JpegImageAccessor::ReadMetadata()
{
    DataBuf dataBuf;
    if (!ReadExifBlob(dataBuf)) {
        IMAGE_LOGE("Image stream doesnot have dataBuf");
        return ERR_IMAGE_SOURCE_DATA;
    }

    ExifData *exifData;
    TiffParser::DecodeJpegExif(reinterpret_cast<const unsigned char *>(dataBuf.CData()), dataBuf.Size(), &exifData);
    if (exifData == nullptr) {
        IMAGE_LOGE("Image stream doesnot have exifData");
        return ERR_IMAGE_DECODE_FAILED;
    }

    exifMetadata_ = std::make_shared<OHOS::Media::ExifMetadata>(exifData);

    return SUCCESS;
}

bool JpegImageAccessor::ReadExifBlob(DataBuf& blob) const
{
    if (!imageStream_->IsOpen()) {
        if (!imageStream_->Open(OpenMode::ReadWrite)) {
            IMAGE_LOGE("Output image stream open failed");
            return false;
        }
    } else {
        imageStream_->Seek(0, SeekPos::BEGIN);
    }

    int marker = FindNextMarker();
    if (marker == EOF) {
        IMAGE_LOGE("Image stream doesnot find marker 0xff");
        return false;
    }

    while ((marker != JPEG_MARKER_SOS) && (marker != JPEG_MARKER_EOI)) {
        const auto [sizebuf, size] = ReadSegmentLength(marker);

        if ((marker == JPEG_MARKER_APP1) && (size >= APP1_EXIF_LENGTH)) {
            blob.Resize(size - EXIF_BLOB_OFFSET);
            imageStream_->Read(blob.Data(), (size - EXIF_BLOB_OFFSET));
            if (blob.CmpBytes(0, EXIF_ID, EXIF_ID_SIZE) == 0) {
                return true;
            }
        }

        marker = FindNextMarker();
        if (marker == EOF) {
            IMAGE_LOGE("Image stream doesnot find marker 0xff");
            return false;
        }
    }
    IMAGE_LOGE("Image stream doesnot find APP1");
    return false;
}

uint32_t JpegImageAccessor::WriteMetadata()
{
    uint8_t* dataBlob = nullptr;
    uint32_t size = 0;
    if (!GetExifEncodeBlob(&dataBlob, size)) {
        IMAGE_LOGE("Encode Metadata failed");
        return ERR_MEDIA_VALUE_INVALID;
    }

    uint32_t result = UpdateData(dataBlob, size);

     if (dataBlob != nullptr) {
        free(dataBlob);
        dataBlob = nullptr;
    }

    return result;
}

uint32_t JpegImageAccessor::WriteExifBlob(DataBuf& blob)
{
    byte* dataBlob = nullptr;
    uint32_t size = 0;
    if (!GetExifBlob(blob, &dataBlob, size)) {
        IMAGE_LOGE("blob data empty");
        return ERR_MEDIA_VALUE_INVALID;
    }

    return UpdateData(dataBlob, size);
}

int JpegImageAccessor::FindNextMarker() const
{
    int marker = -1;
    do {
        marker = imageStream_->ReadByte();
        if (marker == EOF) {
            return marker;
        }
    } while (marker != JPEG_MARKER_HEADER);

    do {
        marker = imageStream_->ReadByte();
        if (marker == EOF) {
            return marker;
        }
    } while (marker == JPEG_MARKER_HEADER);

    return marker;
}

bool HasLength(byte marker)
{
    if ((marker >= JPEG_MARKER_RST1) && (marker <= JPEG_MARKER_EOI)) {
        return false;
    }
    return true;
}

std::pair<std::array<byte, 2>, uint16_t> JpegImageAccessor::ReadSegmentLength(uint8_t marker) const
{
    std::array<byte, READ_BYTES> buf{0, 0};
    uint16_t size{0};
    if (HasLength(marker)) {
        if (imageStream_->Read(buf.data(), buf.size()) == -1) {
            IMAGE_LOGE("Image stream read failed");
            return {buf, size};
        }
        size = GetUShort(buf.data(), bigEndian);
    }
    return {buf, size};
}

DataBuf JpegImageAccessor::ReadNextSegment(byte marker)
{
    const auto [sizebuf, size] = ReadSegmentLength(marker);
    DataBuf buf(size);
    if (size > EXIF_ID_LENGTH) {
        imageStream_->Read(buf.Data(EXIF_ID_LENGTH), (size - EXIF_ID_LENGTH));
        std::copy(sizebuf.begin(), sizebuf.end(), buf.Begin());
    }

    return buf;
}

bool JpegImageAccessor::GetExifEncodeBlob(uint8_t** dataBlob, uint32_t& size)
{
    if (this->GetExifMetadata() == nullptr) {
        return false;
    }

    ExifData* exifData = this->GetExifMetadata()->GetData();
    TiffParser::EncodeJpegExif(dataBlob, size, exifData);

    if (dataBlob == nullptr || *dataBlob == nullptr) {
        return false;
    }

    return (size > 0);
}

bool JpegImageAccessor::GetExifBlob(const DataBuf& blob, uint8_t** dataBlob, uint32_t& size)
{
    if (blob.Empty()) {
        return false;
    }

    if (blob.CmpBytes(0, EXIF_ID, EXIF_ID_SIZE) == 0) {
        *dataBlob = (byte *)blob.CData(EXIF_ID_SIZE);
        size = blob.Size() - EXIF_ID_SIZE;
    } else {
        *dataBlob = (byte *)blob.CData();
        size = blob.Size();
    }

    return true;
}

bool JpegImageAccessor::OpenOrSeek()
{
    if (!imageStream_->IsOpen()) {
        if (!imageStream_->Open(OpenMode::ReadWrite)) {
            IMAGE_LOGE("Output image stream open failed");
            return false;
        }
    } else {
        imageStream_->Seek(0, SeekPos::BEGIN);
    }

    return true;
}

bool JpegImageAccessor::WriteHeader(BufferImageStream& bufStream)
{
    byte tmpBuf[EXIF_ID_LENGTH];
    tmpBuf[0] = JPEG_MARKER_HEADER;
    tmpBuf[1] = JPEG_MARKER_SOI;
    if (bufStream.Write(tmpBuf, EXIF_ID_LENGTH) != EXIF_ID_LENGTH) {
        return false;
    }

    return true;
}

std::tuple<size_t, size_t> JpegImageAccessor::GetInsertPosAndMarkerAPP1()
{
    size_t markerCount = 0;
    size_t skipExifSeqNum = -1;
    size_t insertPos = 0;

    byte marker = FindNextMarker();
    while ((marker != JPEG_MARKER_SOS) && (marker != JPEG_MARKER_EOI)) {
        DataBuf buf = ReadNextSegment(marker);
        if (marker == JPEG_MARKER_APP0) {
            insertPos = markerCount + 1;
        } else if ((marker == JPEG_MARKER_APP1) && (buf.Size() >= APP1_EXIF_LENGTH) &&
                   (buf.CmpBytes(EXIF_BLOB_OFFSET, EXIF_ID, EXIF_ID_LENGTH) == 0)) {
            skipExifSeqNum = markerCount;
        }

        marker = FindNextMarker();
        ++markerCount;
    }

    return std::make_tuple(insertPos, skipExifSeqNum);
}

bool JpegImageAccessor::WriteData(BufferImageStream& bufStream, uint8_t* dataBlob, uint32_t size)
{
    std::array<byte, APP1_HEADER_LENGTH> tmpBuf;
    tmpBuf[0] = JPEG_MARKER_HEADER;
    tmpBuf[1] = JPEG_MARKER_APP1;

    if (size > (JPEG_DATA_MAX_SIZE - APP1_EXIF_LENGTH)) {
        IMAGE_LOGE("Encode Jpeg exif size failed");
        return false;
    }

    uint32_t writeHeaderLength = 4;
    uint32_t exifHeaderLength = 2;

    if (memcmp((char*)dataBlob, EXIF_ID, EXIF_ID_SIZE) != 0) {
        writeHeaderLength = EXIF_ID_LENGTH;
        exifHeaderLength = APP1_EXIF_LENGTH;
        std::copy_n(EXIF_ID, EXIF_ID_SIZE, tmpBuf.data() + MARKER_LENGTH_SIZE);
    }

    US2Data(tmpBuf.data() + EXIF_BLOB_OFFSET, static_cast<uint16_t>(size + exifHeaderLength), bigEndian);
    if (bufStream.Write(tmpBuf.data(), writeHeaderLength) != writeHeaderLength) {
        IMAGE_LOGE("Write EXIF_ID temp stream failed");
        return false;
    }

    if (bufStream.Write(dataBlob, size) != size) {
        IMAGE_LOGE("Write dataBlob temp stream failed");
        return false;
    }

    return true;
}

bool JpegImageAccessor::WriteSegment(BufferImageStream& bufStream, uint8_t marker, const DataBuf& buf)
{
    std::array<byte, EXIF_ID_LENGTH> tmpBuf;
    tmpBuf[0] = JPEG_MARKER_HEADER;
    tmpBuf[1] = marker;
    if (bufStream.Write(tmpBuf.data(), EXIF_ID_LENGTH) != EXIF_ID_LENGTH) {
        IMAGE_LOGE("Write marker and a copy of the segment failed");
        return false;
    }
    if (bufStream.Write((byte*)buf.CData(), buf.Size()) != buf.Size()) {
        IMAGE_LOGE("Write buf failed");
        return false;
    }

    return true;
}

bool JpegImageAccessor::WriteFinalMarker(BufferImageStream& bufStream)
{
    std::array<byte, EXIF_ID_LENGTH> tmpBuf;
    tmpBuf[0] = JPEG_MARKER_HEADER;
    tmpBuf[1] = JPEG_MARKER_SOS;
    if (bufStream.Write(tmpBuf.data(), EXIF_ID_LENGTH) != EXIF_ID_LENGTH) {
        IMAGE_LOGE("Write the final marker failed");
        return false;
    }

    return true;
}

bool JpegImageAccessor::CopyRestData(BufferImageStream& bufStream)
{
    DataBuf buf(READ_WRITE_BLOCK);
    size_t readSize = imageStream_->Read(buf.Data(), buf.Size());
    while (readSize != 0) {
        if (bufStream.Write((byte*)buf.CData(), readSize) != readSize) {
            IMAGE_LOGE("Write block data to temp stream failed");
            return false;
        }
        readSize = imageStream_->Read(buf.Data(), buf.Size());
    }

    return true;
}

bool JpegImageAccessor::UpdateExifMetadata(BufferImageStream& bufStream, uint8_t* dataBlob, uint32_t size)
{
    size_t markerCount = 0;
    auto [insertPos, skipExifSeqNum] = GetInsertPosAndMarkerAPP1();

    imageStream_->Seek(0, SeekPos::BEGIN);

    byte marker = FindNextMarker();
    while (marker != JPEG_MARKER_SOS) {
        DataBuf buf = ReadNextSegment(marker);
        if (insertPos == markerCount) {
            WriteData(bufStream, dataBlob, size);
        }

        if (marker == JPEG_MARKER_EOI) {
            break;
        }

        if ((markerCount != skipExifSeqNum) && (marker != JPEG_MARKER_SOI)) {
            WriteSegment(bufStream, marker, buf);
        } else {
            IMAGE_LOGE("skip existing exifApp segment no:[%{public}u]", skipExifSeqNum);
        }

        marker = FindNextMarker();
        ++markerCount;
    }

    WriteFinalMarker(bufStream);

    return CopyRestData(bufStream);
}

uint32_t JpegImageAccessor::UpdateData(uint8_t* dataBlob, uint32_t size)
{
    BufferImageStream tmpBufStream;
    if (!tmpBufStream.Open(OpenMode::ReadWrite)) {
        IMAGE_LOGE("Image temp stream open failed");
        return ERR_IMAGE_SOURCE_DATA;
    }

    if (!OpenOrSeek()) {
        IMAGE_LOGE("Image stream open failed");
        return ERR_IMAGE_SOURCE_DATA;
    }

    if (!WriteHeader(tmpBufStream)) {
        IMAGE_LOGE("Output image stream write header failed");
        return ERROR;
    }

    if (!UpdateExifMetadata(tmpBufStream, dataBlob, size)) {
        IMAGE_LOGE("Image temp stream write failed");
        return ERROR;
    }

    imageStream_->CopyFrom(tmpBufStream);

    return SUCCESS;
}
} // namespace Media
} // namespace OHOS