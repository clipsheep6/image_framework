#ifndef WEBP_IMAGE_ACCESSOR_H_
#define WEBP_IMAGE_ACCESSOR_H_

#include "abstract_image_accessor.h"

namespace OHOS {
namespace Media {
class WebpImageAccessor : public AbstractImageAccessor {
public:
    WebpImageAccessor(std::unique_ptr<ImageStream> &stream);
    ~WebpImageAccessor();

    virtual int ReadMetadata() const override;
};
} // namespace Media
} // namespace OHOS

#endif
