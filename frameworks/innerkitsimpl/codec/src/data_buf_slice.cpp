#include "image_log.h"

#include "data_buf_slice.h"


#undef LOG_DOMAIN
#define LOG_DOMAIN LOG_TAG_DOMAIN_ID_IMAGE

#undef LOG_TAG
#define LOG_TAG "DataBufferSlice"

namespace OHOS {
namespace Media {

SliceBase::SliceBase(size_t begin, size_t end) : begin_(begin), end_(end) {
  if (begin >= end) {
    IMAGE_LOGE("Begin must be smaller than end");
  }
}

void SliceBase::rangeCheck(size_t index) const {
  if (index >= size()) {
    IMAGE_LOGE("Index outside of the slice");
  }
}

}
}
