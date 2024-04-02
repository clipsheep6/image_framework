/*
 * Copyright (C) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "buffer_metadata_stream.h"
#include "file_metadata_stream.h"
#include "image_log.h"
#include "image_type.h"
#include "jpeg_exif_metadata_accessor.h"
#include "metadata_accessor_factory.h"
#include "png_exif_metadata_accessor.h"

#undef LOG_DOMAIN
#define LOG_DOMAIN LOG_TAG_DOMAIN_ID_IMAGE

#undef LOG_TAG
#define LOG_TAG "MetadataAccessorFactory"

namespace OHOS {
namespace Media {
const int IMAGE_HEADER_SIZE = 10;
const byte jpegHeader[] = { 0xff, 0xd8, 0xff };
const byte pngHeader[] = { 0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A };

std::shared_ptr<MetadataAccessor> MetadataAccessorFactory::Create(uint8_t *buffer, const uint32_t size)
{
    if (buffer == nullptr) {
        return nullptr;
    }
    std::shared_ptr<MetadataStream> stream = std::make_shared<BufferMetadataStream>();
    stream->Write(buffer, size);
    return Create(stream);
}

std::shared_ptr<MetadataAccessor> MetadataAccessorFactory::Create(const int fd)
{
    std::shared_ptr<MetadataStream> stream = std::make_shared<FileMetadataStream>(fd);
    if (!stream->Open(OpenMode::ReadWrite)) {
        IMAGE_LOGE("Failed to open the stream with file descriptor: %{public}d", fd);
        return nullptr;
    }
    return Create(stream);
}

std::shared_ptr<MetadataAccessor> MetadataAccessorFactory::Create(const std::string &path)
{
    std::shared_ptr<MetadataStream> stream = std::make_shared<FileMetadataStream>(path);
    if (!stream->Open(OpenMode::ReadWrite)) {
        IMAGE_LOGE("Failed to open the stream with file path: %{public}s", path.c_str());
        return nullptr;
    }
    return Create(stream);
}

std::shared_ptr<MetadataAccessor> MetadataAccessorFactory::Create(std::shared_ptr<MetadataStream> &stream)
{
    EncodedFormat type = GetImageType(stream);

    switch (type) {
        case EncodedFormat::JPEG:
            return std::make_shared<JpegExifMetadataAccessor>(stream);
        case EncodedFormat::PNG:
            return std::make_shared<PngExifMetadataAccessor>(stream);
        default:
            return nullptr;
    }
}

EncodedFormat MetadataAccessorFactory::GetImageType(std::shared_ptr<MetadataStream> &stream)
{
    byte buff[IMAGE_HEADER_SIZE] = {0};
    stream->Seek(0, SeekPos::BEGIN);
    stream->Read(buff, IMAGE_HEADER_SIZE * sizeof(byte));

    if (memcmp(buff, jpegHeader, sizeof(jpegHeader) * sizeof(byte)) == 0) {
        return EncodedFormat::JPEG;
    }

    if (memcmp(buff, pngHeader, sizeof(pngHeader) * sizeof(byte)) == 0) {
        return EncodedFormat::PNG;
    }

    stream->Seek(0, SeekPos::BEGIN);
    return EncodedFormat::UNKNOWN;
}
} // namespace Media
} // namespace OHOS