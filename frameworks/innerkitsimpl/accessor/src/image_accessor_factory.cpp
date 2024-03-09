#include <map>
#include <vector>
#include "buffer_image_stream.h"
#include "dng_image_accessor.h"
#include "file_image_stream.h"
#include "heif_image_accessor.h"
#include "image_accessor_factory.h"
#include "image_type.h"
#include "jpeg_image_accessor.h"
#include "media_errors.h"
#include "png_image_accessor.h"
#include "webp_image_accessor.h"

namespace OHOS {
namespace Media {

uint8_t jpegHeader[] = { 0xFF, 0xD8, 0xFF };
uint8_t pngHeader[] = { 137, 80, 78, 71, 13, 10, 26, 10 };

std::shared_ptr<ImageAccessor> ImageAccessorFactory::CreateImageAccessor(uint8_t *buffer, const uint32_t size)
{
    std::unique_ptr<ImageStream> stream(new BufferImageStream());
    stream->Write(buffer, size);
    return GetImageAccessor(stream);
}

std::shared_ptr<ImageAccessor> ImageAccessorFactory::CreateImageAccessor(const int fd)
{
    // todo: 改为从fd获取
    std::string path("");
    std::unique_ptr<ImageStream> stream(new FileImageStream(path));
    if (!stream->Open(OpenMode::ReadWrite)) {
        return nullptr; // IMAGE_RESULT_FILE_FD_ERROR
    }
    return GetImageAccessor(stream);
}

std::shared_ptr<ImageAccessor> ImageAccessorFactory::CreateImageAccessor(const std::string &path)
{
    std::unique_ptr<ImageStream> stream(new FileImageStream(path));
    if (!stream->Open(OpenMode::ReadWrite)) {
        return nullptr; // IMAGE_RESULT_FREAD_FAILED
    }
    return GetImageAccessor(stream);
}

std::shared_ptr<ImageAccessor> ImageAccessorFactory::GetImageAccessor(std::unique_ptr<ImageStream> &stream)
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

EncodedFormat ImageAccessorFactory::GetImageType(std::unique_ptr<ImageStream> &stream)
{
    uint8_t buff[10] = {0};
    stream->Read(buff, 10);

    if (memcmp(buff, jpegHeader, sizeof(jpegHeader)) != 0) {
        return EncodedFormat::JPEG;
    }

    if (memcmp(buff, pngHeader, sizeof(pngHeader)) != 0) {
        return EncodedFormat::PNG;
    }

    return EncodedFormat::UNKNOWN;
}

} // namespace Media
} // namespace OHOS
