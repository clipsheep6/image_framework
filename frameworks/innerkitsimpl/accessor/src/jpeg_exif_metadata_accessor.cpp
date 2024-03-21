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

#include "jpeg_exif_metadata_accessor.h"

#include <libexif/exif-data.h>

#include "file_metadata_stream.h"
#include "image_log.h"
#include "media_errors.h"
#include "tiff_parser.h"

#undef LOG_DOMAIN
#define LOG_DOMAIN LOG_TAG_DOMAIN_ID_IMAGE

#undef LOG_TAG
#define LOG_TAG "JpegExifMetadataAccssor"

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
    constexpr auto SEGMENT_LENGTH_SIZE = 2;
    constexpr auto READ_BYTES = 2;
    constexpr auto EXIF_ID_SIZE = 6;
    constexpr auto APP1_EXIF_LENGTH = 8;
    constexpr auto APP1_HEADER_LENGTH = 10;
    constexpr auto MARKER_LENGTH_SIZE = 4;
    constexpr auto READ_WRITE_BLOCK = 4096;
    constexpr auto JPEG_MARKER_HEADER = 0xff;
    constexpr auto JPEG_DATA_MAX_SIZE = 0xffff;
}

JpegExifMetadataAccssor::JpegExifMetadataAccssor(std::shared_ptr<MetadataStream> &stream)
    : AbstractExifMetadataAccessor(stream) {}

JpegExifMetadataAccssor::~JpegExifMetadataAccssor() {}

uint32_t JpegExifMetadataAccssor::Read()
{
    DataBuf dataBuf;
    if (!ReadBlob(dataBuf)) {
        IMAGE_LOGE("Image stream doesnot have dataBuf");
        return ERR_IMAGE_SOURCE_DATA;
    }

    ExifData *exifData;
    TiffParser::DecodeJpegExif(reinterpret_cast<const unsigned char *>(dataBuf.CData()), dataBuf.Size(), &exifData);
    if (exifData == nullptr) {
        IMAGE_LOGE("Image stream doesnot have exifData");
        return ERR_EXIF_DECODE_FAILED;
    }

    exifMetadata_ = std::make_shared<OHOS::Media::ExifMetadata>(exifData);

    return SUCCESS;
}

bool JpegExifMetadataAccssor::ReadBlob(DataBuf &blob) const
{
    if (!imageStream_->IsOpen()) {
        IMAGE_LOGE("Output image stream not open");
        return false;
    }
    imageStream_->Seek(0, SeekPos::BEGIN);

    int marker = FindNextMarker();
    if (marker == EOF) {
        IMAGE_LOGE("Image stream doesnot find marker 0xff");
        return false;
    }

    while ((marker != JPEG_MARKER_SOS) && (marker != JPEG_MARKER_EOI)) {
        const auto [sizebuf, size] = ReadSegmentLength(marker);

        if ((marker == JPEG_MARKER_APP1) && (size >= APP1_EXIF_LENGTH)) {
            blob.Resize(size - SEGMENT_LENGTH_SIZE);
            imageStream_->Read(blob.Data(), (size - SEGMENT_LENGTH_SIZE));
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

uint32_t JpegExifMetadataAccssor::Write()
{
    uint8_t *dataBlob = nullptr;
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

uint32_t JpegExifMetadataAccssor::WriteBlob(DataBuf &blob)
{
    byte *dataBlob = nullptr;
    uint32_t size = 0;
    if (!GetExifBlob(blob, &dataBlob, size)) {
        IMAGE_LOGE("blob data empty");
        return ERR_MEDIA_VALUE_INVALID;
    }

    return UpdateData(dataBlob, size);
}

int JpegExifMetadataAccssor::FindNextMarker() const
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

std::pair<std::array<byte, 2>, uint16_t> JpegExifMetadataAccssor::ReadSegmentLength(uint8_t marker) const
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

DataBuf JpegExifMetadataAccssor::ReadNextSegment(byte marker)
{
    const auto [sizebuf, size] = ReadSegmentLength(marker);
    DataBuf buf(size);
    if (size > EXIF_ID_LENGTH) {
        imageStream_->Read(buf.Data(EXIF_ID_LENGTH), (size - EXIF_ID_LENGTH));
        std::copy(sizebuf.begin(), sizebuf.end(), buf.Begin());
    }

    return buf;
}

bool JpegExifMetadataAccssor::GetExifEncodeBlob(uint8_t **dataBlob, uint32_t &size)
{
    if (this->Get() == nullptr) {
        IMAGE_LOGE("Exif metadata empty");
        return false;
    }

    ExifData *exifData = this->Get()->GetExifData();
    TiffParser::EncodeJpegExif(dataBlob, size, exifData);

    if (dataBlob == nullptr || *dataBlob == nullptr) {
        IMAGE_LOGE("Encode Jpeg data failed");
        return false;
    }

    return (size > 0);
}

bool JpegExifMetadataAccssor::GetExifBlob(const DataBuf &blob, uint8_t **dataBlob, uint32_t &size)
{
    if (blob.Empty()) {
        IMAGE_LOGE("Image exif blob data empty");
        return false;
    }

    *dataBlob = (byte *)blob.CData();
    size = blob.Size();

    return true;
}

bool JpegExifMetadataAccssor::WriteHeader(BufferMetadataStream &bufStream)
{
    byte tmpBuf[EXIF_ID_LENGTH];
    tmpBuf[0] = JPEG_MARKER_HEADER;
    tmpBuf[1] = JPEG_MARKER_SOI;
    if (bufStream.Write(tmpBuf, EXIF_ID_LENGTH) != EXIF_ID_LENGTH) {
        return false;
    }

    return true;
}

std::tuple<size_t, size_t> JpegExifMetadataAccssor::GetInsertPosAndMarkerAPP1()
{
    size_t markerCount = 0;
    size_t skipExifSeqNum = -1;
    size_t insertPos = 0;

    imageStream_->Seek(0, SeekPos::BEGIN);

    byte marker = FindNextMarker();
    while ((marker != JPEG_MARKER_SOS) && (marker != JPEG_MARKER_EOI)) {
        DataBuf buf = ReadNextSegment(marker);
        if (marker == JPEG_MARKER_APP0) {
            insertPos = markerCount + 1;
        } else if ((marker == JPEG_MARKER_APP1) && (buf.Size() >= APP1_EXIF_LENGTH) &&
                   (buf.CmpBytes(EXIF_BLOB_OFFSET, EXIF_ID, EXIF_ID_SIZE) == 0)) {
            skipExifSeqNum = markerCount;
        }

        marker = FindNextMarker();
        ++markerCount;
    }

    return std::make_tuple(insertPos, skipExifSeqNum);
}

bool JpegExifMetadataAccssor::WriteData(BufferMetadataStream &bufStream, uint8_t *dataBlob, uint32_t size)
{
    std::array<byte, APP1_HEADER_LENGTH> tmpBuf;
    tmpBuf[0] = JPEG_MARKER_HEADER;
    tmpBuf[1] = JPEG_MARKER_APP1;

    if (size > (JPEG_DATA_MAX_SIZE - APP1_EXIF_LENGTH)) {
        IMAGE_LOGE("Encode Jpeg exif size failed");
        return false;
    }

    uint32_t writeHeaderLength = MARKER_LENGTH_SIZE;
    uint32_t exifHeaderLength = EXIF_ID_LENGTH;

    if (memcmp((char*)dataBlob, EXIF_ID, EXIF_ID_SIZE) != 0) {
        writeHeaderLength = APP1_HEADER_LENGTH;
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

bool JpegExifMetadataAccssor::WriteSegment(BufferMetadataStream &bufStream, uint8_t marker, const DataBuf &buf)
{
    std::array<byte, SEGMENT_LENGTH_SIZE> tmpBuf;
    tmpBuf[0] = JPEG_MARKER_HEADER;
    tmpBuf[1] = marker;
    if (bufStream.Write(tmpBuf.data(), SEGMENT_LENGTH_SIZE) != SEGMENT_LENGTH_SIZE) {
        IMAGE_LOGE("Write marker and a copy of the segment failed");
        return false;
    }
    if (bufStream.Write((byte*)buf.CData(), buf.Size()) != buf.Size()) {
        IMAGE_LOGE("Write buf failed");
        return false;
    }

    return true;
}

bool JpegExifMetadataAccssor::WriteTail(BufferMetadataStream &bufStream)
{
    std::array<byte, SEGMENT_LENGTH_SIZE> tmpBuf;
    tmpBuf[0] = JPEG_MARKER_HEADER;
    tmpBuf[1] = JPEG_MARKER_SOS;
    if (bufStream.Write(tmpBuf.data(), SEGMENT_LENGTH_SIZE) != SEGMENT_LENGTH_SIZE) {
        IMAGE_LOGE("Write the final marker failed");
        return false;
    }

    return true;
}

bool JpegExifMetadataAccssor::CopyRestData(BufferMetadataStream &bufStream)
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

bool JpegExifMetadataAccssor::UpdateExifMetadata(BufferMetadataStream &bufStream, uint8_t *dataBlob, uint32_t size)
{
    size_t markerCount = 0;
    auto [insertPos, skipExifSeqNum] = GetInsertPosAndMarkerAPP1();

    if (!WriteHeader(bufStream)) {
        IMAGE_LOGE("Output image stream write header failed");
        return false;
    }

    imageStream_->Seek(0, SeekPos::BEGIN);

    byte marker = FindNextMarker();
    while (marker != JPEG_MARKER_SOS) {
        DataBuf buf = ReadNextSegment(marker);
        if (markerCount == insertPos) {
            WriteData(bufStream, dataBlob, size);
        }

        if (marker == JPEG_MARKER_EOI) {
            break;
        }

        if ((markerCount != skipExifSeqNum) && (marker != JPEG_MARKER_SOI)) {
            WriteSegment(bufStream, marker, buf);
        } else {
            IMAGE_LOGD("skip existing exifApp segment no:[%{public}u]", skipExifSeqNum);
        }

        marker = FindNextMarker();
        ++markerCount;
    }

    WriteTail(bufStream);

    return CopyRestData(bufStream);
}

uint32_t JpegExifMetadataAccssor::UpdateData(uint8_t *dataBlob, uint32_t size)
{
    BufferMetadataStream tmpBufStream;
    if (!tmpBufStream.Open(OpenMode::ReadWrite)) {
        IMAGE_LOGE("Image temp stream open failed");
        return ERR_IMAGE_SOURCE_DATA;
    }

    if (!imageStream_->IsOpen()) {
        IMAGE_LOGE("Output image stream not open");
        return ERR_IMAGE_SOURCE_DATA;
    }

    if (!UpdateExifMetadata(tmpBufStream, dataBlob, size)) {
        IMAGE_LOGE("Image temp stream write failed");
        return ERROR;
    }

    imageStream_->Seek(0, SeekPos::BEGIN);
    imageStream_->CopyFrom(tmpBufStream);

    return SUCCESS;
}
} // namespace Media
} // namespace OHOS