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
    ExifMetadata(std::shared_ptr<ExifData> &exifData);
    ~ExifMetadata();
    int GetValue(const std::string &key, std::string &value) const;
    void SetValue(const std::string &key, const std::string &value);
    ExifData GetData() const;

private:
    std::shared_ptr<ExifData> exifData_;
};
} // namespace Media
} // namespace OHOS

#endif
