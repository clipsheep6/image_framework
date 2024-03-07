#include "exif_metadata.h"

namespace OHOS {
namespace Media {

std::string ExifMetadata::GetValue(const std::string& key) const
{
    (void)key;
    return std::string();
}

void ExifMetadata::SetValue(const std::string& key, const std::string& value)
{
    (void)value;
    (void)key;
}

ExifData ExifMetadata::GetData() const
{
    return exifData_;
}

} // namespace Media
} // namespace OHOS