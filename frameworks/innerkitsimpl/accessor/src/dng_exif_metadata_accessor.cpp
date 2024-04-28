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

#include <iostream>
#include <libexif/exif-data.h>

#include "dng_exif_metadata_accessor.h"
#include "data_buf.h"
#include "image_log.h"
#include "media_errors.h"
#include "tiff_parser.h"
#include "file_metadata_stream.h"

#undef LOG_DOMAIN
#define LOG_DOMAIN LOG_TAG_DOMAIN_ID_IMAGE

#undef LOG_TAG
#define LOG_TAG "DngExifMetadataAccessor"

namespace OHOS {
namespace Media {
namespace {
    using uint_8 = byte;
}

constexpr uint32_t IFD0_OFFSET_POSITION = 4;
constexpr uint32_t IFD0_HEAD = 8;
constexpr uint32_t IS_END_THRESHOLD = 10;

DngExifMetadataAccessor::DngExifMetadataAccessor(std::shared_ptr<MetadataStream> &stream)
    : AbstractExifMetadataAccessor(stream)
{
}

DngExifMetadataAccessor::~DngExifMetadataAccessor() {}

uint32_t DngExifMetadataAccessor::Read()
{
    if (!imageStream_->IsOpen()) {
        IMAGE_LOGE("Input image stream is not open.");
        return ERR_IMAGE_SOURCE_DATA;
    }

    imageStream_->Seek(0, SeekPos::BEGIN);
    ssize_t size = imageStream_->GetSize();
    byte *byteStream = imageStream_->GetAddr();
    if ((size == 0) || (byteStream == nullptr)) {
        IMAGE_LOGE("Input image stream is empty.");
        return ERR_IMAGE_SOURCE_DATA;
    }

    size_t tiffHeaderPos = TiffParser::FindTiffPos(byteStream, size);
    if (tiffHeaderPos == std::numeric_limits<size_t>::max()) {
        IMAGE_LOGE("Input image stream is not tiff type.");
        return ERR_IMAGE_SOURCE_DATA;
    }

    ExifData *exifData;
    TiffParser::DecodeDngExif(reinterpret_cast<const unsigned char *>(byteStream + tiffHeaderPos),
                              (size - tiffHeaderPos), &exifData, maxAddr);
    if (exifData == nullptr) {
        IMAGE_LOGE("Failed to decode TIFF buffer.");
        return ERR_EXIF_DECODE_FAILED;
    }

    exifMetadata_ = std::make_shared<OHOS::Media::ExifMetadata>(exifData);
    return SUCCESS;
}

bool DngExifMetadataAccessor::ReadBlob(DataBuf &blob) const
{
    return false;
}

uint32_t DngExifMetadataAccessor::Write()
{
    imageStream_->Seek(0, SeekPos::BEGIN);
    ssize_t size = imageStream_->GetSize();
    IMAGE_LOGD("imageStream size: %{public}d", size);
    byte *imageAddr = imageStream_->GetAddr();
    if ((size == 0) || (imageAddr == nullptr)) {
        IMAGE_LOGE("Input image stream is empty.");
        return ERR_IMAGE_SOURCE_DATA;
    }

    ExifData *exifData = exifMetadata_->GetExifData();
    if (!exifData) {
        IMAGE_LOGE("Failed to get exifData.");
        return ERR_EXIF_DECODE_FAILED;
    }
    uint32_t ifd0Offset = exif_get_long(imageAddr + IFD0_OFFSET_POSITION, exif_data_get_byte_order(exifData));
    IMAGE_LOGD("ifd0Offset: %{public}x", ifd0Offset);
    uint16_t ifd0Components = exif_get_short(imageAddr + ifd0Offset, exif_data_get_byte_order(exifData));
    IMAGE_LOGD("ifd0Components: %{public}u", ifd0Components);
    uint32_t ifd1Offset = exif_get_long(imageAddr + ifd0Offset + 2 + 12 * ifd0Components,
                                        exif_data_get_byte_order(exifData));
    IMAGE_LOGD("ifd1Offset: %{public}x", ifd1Offset);

    imageStream_->Seek(0, SeekPos::END);
    uint32_t endOffset = imageStream_->Tell();
    IMAGE_LOGD("imageStream Tell: %{public}x", endOffset);
    IMAGE_LOGD("compare maxAddr: %{public}x endOffset: %{public}x", maxAddr, endOffset);
    if (abs((int)(maxAddr - endOffset)) < IS_END_THRESHOLD && ifd0Offset >= IFD0_HEAD) {
        IMAGE_LOGD("exif blob is at the end.");
        auto delta = ifd0Offset - IFD0_HEAD;
        return WriteExif(ifd0Offset, ifd1Offset, delta, ifd0Offset);
    }
    IMAGE_LOGD("exif blob is not at the end.");
    auto delta = endOffset - IFD0_HEAD;
    return WriteExif(endOffset, ifd1Offset, delta, endOffset);

    return SUCCESS;
}

uint32_t DngExifMetadataAccessor::WriteBlob(DataBuf &blob)
{
    return ERROR;
}

uint32_t DngExifMetadataAccessor::WriteExif(unsigned int ifd0Offset, unsigned int ifd1Offset,
                                            unsigned int delta, unsigned int writePos)
{
    ExifData *exifData = exifMetadata_->GetExifData();
    if (!exifData) {
        IMAGE_LOGE("Failed to get exifData.");
        return ERR_EXIF_DECODE_FAILED;
    }
    unsigned char* dataBlob = nullptr;
    uint32_t size = 0;
    TiffParser::EncodeDngExif(&dataBlob, size, exifData, delta, ifd1Offset);
    if (dataBlob == nullptr) {
        IMAGE_LOGE("Failed to encode exifData.");
        return ERR_EXIF_DECODE_FAILED;
    }
    IMAGE_LOGD("new exif blob size: %{public}u", size);

    imageStream_->ReleaseAddr();

    imageStream_->Seek(IFD0_OFFSET_POSITION, SeekPos::BEGIN);
    IMAGE_LOGD("set ifd0Offset: %{public}x", ifd0Offset);
    unsigned char buff[4] = {0};
    exif_set_long(buff, exif_data_get_byte_order(exifData), ifd0Offset);
    imageStream_->Write(static_cast<unsigned char*>(buff), sizeof(buff));

    imageStream_->Seek(writePos, SeekPos::BEGIN);
    auto iret = imageStream_->Write(dataBlob + IFD0_HEAD, size - IFD0_HEAD);
    IMAGE_LOGD("image stream write ret: %{public}d", iret);

    imageStream_->Flush();
    return SUCCESS;
}
} // namespace Media
} // namespace OHOS
