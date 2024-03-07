#ifndef EXIF_METADATA_H_
#define EXIF_METADATA_H_

#include <string>
#include <libexif/exif-data.h>

namespace OHOS {
namespace Media {
class ExifMetadata {
public:
    std::string GetValue(const std::string& key) const;
    void SetValue(const std::string& key, const std::string& value);
    ExifData GetData() const;

private:
    ExifData exifData_;
};
} // namespace Media
} // namespace OHOS

#endif