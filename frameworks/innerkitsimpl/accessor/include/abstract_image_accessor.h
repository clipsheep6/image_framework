#ifndef ABSTRACT_IMAGE_ACCESSOR_H_
#define ABSTRACT_IMAGE_ACCESSOR_H_

#include "image_accessor.h"

namespace OHOS {
namespace Media {
class AbstractImageAccessor : public ImageAccessor {
public:
    AbstractImageAccessor(std::unique_ptr<ImageStream> &&stream);
    virtual ~AbstractImageAccessor();

    virtual int ReadMetadata() const override;
    virtual bool WriteMetadata() override;
    virtual bool ReadExifBlob(DataBuf &blob) const override;
    virtual bool WriteExifBlob(DataBuf &blob) override;

protected:
    std::unique_ptr<ImageStream> imageStream_ = nullptr;
    std::shared_ptr<ExifMetadata> exifMetadata_ = nullptr;

    virtual std::shared_ptr<ExifMetadata> GetExifMetadata() override;
};
} // namespace Media
} // namespace OHOS

#endif
