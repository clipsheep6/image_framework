#ifndef DNG_IMAGE_ACCESSOR_H_
#define DNG_IMAGE_ACCESSOR_H_

#include "abstract_image_accessor.h"

namespace OHOS {
namespace Media {
class DngImageAccessor : public AbstractImageAccessor {
public:
    DngImageAccessor(std::unique_ptr<ImageStream> &stream);
    ~DngImageAccessor();

    virtual int ReadMetadata() const override;
};
} // namespace Media
} // namespace OHOS

#endif
