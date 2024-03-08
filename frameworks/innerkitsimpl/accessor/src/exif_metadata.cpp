#include "exif_metadata.h"
#include "image_log.h"

namespace OHOS {
namespace Media {

ExifMetadata::ExifMetadata(ExifData* exifData)
    : exifData_(exifData)
{
}

ExifMetadata::~ExifMetadata()
{
    if (exifData_ != nullptr) {
        exif_data_unref(exifData_);
        exifData_ = nullptr;
    }
}

std::string ExifMetadata::GetValue(const std::string& key)
{
    if (exifData_ == nullptr) {
        return std::string();
    }

    auto tag = exif_tag_from_name(key.c_str());
    auto entry = exif_data_get_entry(exifData_, tag);
    if (entry == nullptr) {
        IMAGE_LOGD("exif_data_get_entry leave");
        return "";
    }
    char tagValueChar[1024];
    exif_entry_get_value(entry, tagValueChar, sizeof(tagValueChar));
    return std::string(tagValueChar);
}

void ExifMetadata::SetValue(const std::string& key, const std::string& value)
{
    (void)value;
    (void)key;
}

ExifData ExifMetadata::GetData()
{
    return *exifData_;
}

} // namespace Media
} // namespace OHOS