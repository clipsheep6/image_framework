#ifndef PNG_IMAGE_ACCESSOR_H_
#define PNG_IMAGE_ACCESSOR_H_

#include "abstract_image_accessor.h"

namespace OHOS {
namespace Media {
class PngImageAccessor : public AbstractImageAccessor {
public:
    PngImageAccessor(std::shared_ptr<ImageStream> &stream);
    ~PngImageAccessor();

    virtual int ReadMetadata() override;
    virtual bool WriteMetadata() override;
    virtual bool ReadExifBlob(DataBuf &blob) const override;
    virtual bool WriteExifBlob(DataBuf &blob) override;
};
} // namespace Media
} // namespace OHOS

#endif
