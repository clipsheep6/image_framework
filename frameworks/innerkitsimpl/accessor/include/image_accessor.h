#ifndef IMAGE_ACCESSOR_H_
#define IMAGE_ACCESSOR_H_

#include <memory>
#include "exif_metadata.h"
#include "image_stream.h"
#include "data_buf.h"
#include "output_data_stream.h"

namespace OHOS {
namespace Media {
class ImageAccessor {
public:
    virtual int ReadMetadata() = 0;
    virtual bool WriteMetadata() = 0;
    virtual bool CreateExifMetadata() = 0;
    virtual bool WriteToOutput(ImagePlugin::OutputDataStream& output) = 0;
    virtual bool ReadExifBlob(DataBuf &blob) const = 0;
    virtual bool WriteExifBlob(DataBuf &blob) = 0;
    virtual std::shared_ptr<ExifMetadata> GetExifMetadata() = 0;
};
} // namespace Media
} // namespace OHOS

#endif
