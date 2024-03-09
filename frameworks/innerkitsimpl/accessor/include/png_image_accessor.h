#ifndef PNG_IMAGE_ACCESSOR_H_
#define PNG_IMAGE_ACCESSOR_H_

#include "abstract_image_accessor.h"

namespace OHOS {
namespace Media {
class PngImageAccessor : public AbstractImageAccessor {
public:
    PngImageAccessor(std::unique_ptr<ImageStream> &stream);
    ~PngImageAccessor();

    virtual int ReadMetadata() const override;
};
} // namespace Media
} // namespace OHOS

#endif
