#include "buffer_image_stream.h"
#include "dng_image_accessor.h"
#include "file_image_stream.h"
#include "heif_image_accessor.h"
#include "image_accessor_factory.h"
#include "image_log.h"
#include "image_type.h"
#include "jpeg_image_accessor.h"
#include "png_image_accessor.h"
#include "webp_image_accessor.h"

namespace OHOS {
namespace Media {

const int IMAGE_HEADER_SIZE = 10;
const byte jpegHeader[] = { 0xff, 0xd8, 0xff };
const byte pngHeader[] = { 0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A };

std::shared_ptr<ImageAccessor> ImageAccessorFactory::CreateImageAccessor(uint8_t *buffer, const uint32_t size)
{
    std::shared_ptr<ImageStream> stream = std::make_shared<BufferImageStream>(buffer, size, BufferImageStream::MemoryMode::Dynamic);
    return CreateImageAccessor(stream);
}

std::shared_ptr<ImageAccessor> ImageAccessorFactory::CreateImageAccessor(const int fd)
{
    std::shared_ptr<ImageStream> stream = std::make_shared<FileImageStream>(fd);
    if (!stream->Open(OpenMode::ReadWrite)) {
        IMAGE_LOGE("Failed to open stream by fd");
        return nullptr;
    }
    return CreateImageAccessor(stream);
}

std::shared_ptr<ImageAccessor> ImageAccessorFactory::CreateImageAccessor(const std::string &path)
{
    std::shared_ptr<ImageStream> stream = std::make_shared<FileImageStream>(path);
    if (!stream->Open(OpenMode::ReadWrite)) {
        IMAGE_LOGE("Failed to open stream by path");
        return nullptr;
    }
    return CreateImageAccessor(stream);
}

std::shared_ptr<ImageAccessor> ImageAccessorFactory::CreateImageAccessor(std::shared_ptr<ImageStream> &stream)
{
    EncodedFormat type = GetImageType(stream);

    switch (type) {
        case EncodedFormat::JPEG:
            return std::make_shared<JpegImageAccessor>(stream);
        case EncodedFormat::PNG:
            return std::make_shared<PngImageAccessor>(stream);
        case EncodedFormat::HEIF:
            return std::make_shared<HeifImageAccessor>(stream);
        case EncodedFormat::WEBP:
            return std::make_shared<WebpImageAccessor>(stream);
        case EncodedFormat::DNG:
            return std::make_shared<DngImageAccessor>(stream);
        default:
            return nullptr;
    }
}

EncodedFormat ImageAccessorFactory::GetImageType(std::shared_ptr<ImageStream> &stream)
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

    return EncodedFormat::UNKNOWN;
}

} // namespace Media
} // namespace OHOS
