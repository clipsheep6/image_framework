#ifndef WEBP_IMAGE_ACCESSOR_H_
#define WEBP_IMAGE_ACCESSOR_H_

#include "abstract_image_accessor.h"

namespace OHOS {
namespace Media {
class WebpImageAccessor : public AbstractImageAccessor {
public:
    WebpImageAccessor(std::shared_ptr<ImageStream> &stream);
    ~WebpImageAccessor();

    virtual uint32_t ReadMetadata() override;
    virtual uint32_t WriteMetadata() override;
    virtual bool ReadExifBlob(DataBuf &blob) const override;
    virtual uint32_t WriteExifBlob(DataBuf &blob) override;
};
} // namespace Media
} // namespace OHOS

#endif
