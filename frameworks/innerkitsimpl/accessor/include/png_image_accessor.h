#ifndef PNG_IMAGE_ACCESSOR_H_
#define PNG_IMAGE_ACCESSOR_H_

#include "abstract_image_accessor.h"

namespace OHOS {
namespace Media {
class PngImageAccessor : public AbstractImageAccessor {
public:
    PngImageAccessor(std::shared_ptr<ImageStream> &stream);
    ~PngImageAccessor();

    virtual uint32_t ReadMetadata() override;
    virtual uint32_t WriteMetadata() override;
    virtual bool ReadExifBlob(DataBuf &blob) const override;
    virtual uint32_t WriteExifBlob(DataBuf &blob) override;
};
} // namespace Media
} // namespace OHOS

#endif
