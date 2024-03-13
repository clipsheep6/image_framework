#ifndef DNG_IMAGE_ACCESSOR_H_
#define DNG_IMAGE_ACCESSOR_H_

#include "abstract_image_accessor.h"

namespace OHOS {
namespace Media {
class DngImageAccessor : public AbstractImageAccessor {
public:
    DngImageAccessor(std::shared_ptr<ImageStream> &stream);
    ~DngImageAccessor();

    virtual int ReadMetadata() override;
    virtual bool WriteMetadata() override;
    virtual bool ReadExifBlob(DataBuf &blob) const override;
    virtual bool WriteExifBlob(DataBuf &blob) override;
};
} // namespace Media
} // namespace OHOS

#endif
