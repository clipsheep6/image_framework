#ifndef IMAGE_ACCESSOR_H_
#define IMAGE_ACCESSOR_H_

#define MOCK_NATIVE_INCLUDE_HILOG_LOG_C_H 1

#include <memory>
#include "exif_metadata.h"
#include "image_stream.h"
#include "data_buf.h"
#include "output_data_stream.h"

namespace OHOS {
namespace Media {
class ImageAccessor {
public:
    virtual uint32_t ReadMetadata() = 0;
    virtual uint32_t WriteMetadata() = 0;
    virtual bool CreateExifMetadata() = 0;
    virtual bool WriteToOutput(ImagePlugin::OutputDataStream& output) = 0;
    virtual bool ReadExifBlob(DataBuf &blob) const = 0;
    virtual uint32_t WriteExifBlob(DataBuf &blob) = 0;
    virtual std::shared_ptr<ExifMetadata> GetExifMetadata() = 0;
};
} // namespace Media
} // namespace OHOS

#endif
