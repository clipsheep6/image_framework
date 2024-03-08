#ifndef EXIF_METADATA_H_
#define EXIF_METADATA_H_

#include <string>
#include <libexif/exif-data.h>
#include <libexif/exif-entry.h>
#include <libexif/exif-tag.h>

namespace OHOS {
namespace Media {
class ExifMetadata {
public:
    ExifMetadata() = default;
    ExifMetadata(ExifData* exifData);
    ~ExifMetadata();
    std::string GetValue(const std::string& key);
    void SetValue(const std::string& key, const std::string& value);
    ExifData GetData();

private:
    ExifData *exifData_;
};
} // namespace Media
} // namespace OHOS

#endif