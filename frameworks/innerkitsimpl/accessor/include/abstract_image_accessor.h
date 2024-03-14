#ifndef ABSTRACT_IMAGE_ACCESSOR_H_
#define ABSTRACT_IMAGE_ACCESSOR_H_

#include "image_accessor.h"

namespace OHOS {
namespace Media {
class AbstractImageAccessor : public ImageAccessor {
public:
    AbstractImageAccessor(std::shared_ptr<ImageStream> &stream);
    virtual ~AbstractImageAccessor();

    virtual bool CreateExifMetadata() override;
    virtual std::shared_ptr<ExifMetadata> GetExifMetadata() override;
    virtual bool WriteToOutput(ImagePlugin::OutputDataStream &soutput) override;

protected:
    std::shared_ptr<ExifMetadata> exifMetadata_ = nullptr;
    std::shared_ptr<ImageStream> imageStream_ = nullptr;
};
} // namespace Media
} // namespace OHOS

#endif
