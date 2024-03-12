#ifndef WEBP_IMAGE_ACCESSOR_H_
#define WEBP_IMAGE_ACCESSOR_H_

#include "abstract_image_accessor.h"

namespace OHOS {
namespace Media {
class WebpImageAccessor : public AbstractImageAccessor {
public:
    WebpImageAccessor(std::shared_ptr<ImageStream> &stream);
    ~WebpImageAccessor();

    virtual int ReadMetadata() override;
    virtual bool WriteMetadata() override;
    virtual bool ReadExifBlob(DataBuf &blob) const override;
    virtual bool WriteExifBlob(DataBuf &blob) override;
};
} // namespace Media
} // namespace OHOS

#endif
