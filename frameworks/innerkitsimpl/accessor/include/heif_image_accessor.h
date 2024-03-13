#ifndef HEIF_IMAGE_ACCESSOR_H_
#define HEIF_IMAGE_ACCESSOR_H_

#include "abstract_image_accessor.h"

namespace OHOS {
namespace Media {
class HeifImageAccessor : public AbstractImageAccessor {
public:
    HeifImageAccessor(std::shared_ptr<ImageStream> &stream);
    ~HeifImageAccessor();

    virtual uint32_t ReadMetadata() override;
    virtual uint32_t WriteMetadata() override;
    virtual bool ReadExifBlob(DataBuf &blob) const override;
    virtual uint32_t WriteExifBlob(DataBuf &blob) override;
};
} // namespace Media
} // namespace OHOS

#endif
