#ifndef IMAGE_ACCESSOR_FACTORY
#define IMAGE_ACCESSOR_FACTORY

#include <string>
#include <memory>
#include "image_accessor.h"
#include "image_stream.h"


namespace OHOS {
namespace Media {
class ImageAccessorFactory {
public:
    ~ImageAccessorFactory() {}

    static std::shared_ptr<ImageAccessor> CreateImageAccessor(uint8_t *buffer, const uint32_t size);
    static std::shared_ptr<ImageAccessor> CreateImageAccessor(const int fd);
    static std::shared_ptr<ImageAccessor> CreateImageAccessor(const std::string &path);

private:
    static std::shared_ptr<ImageAccessor> GetImageAccessor(std::unique_ptr<ImageStream> &stream);
    static EncodedFormat GetImageType(std::unique_ptr<ImageStream> &stream);
};
} // namespace Media
} // namespace OHOS

#endif
