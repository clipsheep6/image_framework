#include "exif_metadata.h"
#include "image_log.h"

namespace OHOS {
namespace Media {

ExifMetadata::ExifMetadata(std::shared_ptr<ExifData> &exifData)
    : exifData_(exifData)
{
}

ExifMetadata::~ExifMetadata()
{
    if (exifData_ != nullptr) {
        exif_data_unref(exifData_.get());
        exifData_ = nullptr;
    }
}

int ExifMetadata::GetValue(const std::string &key, std::string &value) const
{
    if (exifData_ == nullptr) {
        value = "";
        return 1;
    }

    auto tag = exif_tag_from_name(key.c_str());
    auto entry = exif_data_get_entry(exifData_, tag);
    if (entry == nullptr) {
        IMAGE_LOGD("exif_data_get_entry leave");
        value = "";
        return 1;
    }
    char tagValueChar[1024];
    exif_entry_get_value(entry, tagValueChar, sizeof(tagValueChar));
    value = tagValueChar;
    return 0;
}

void ExifMetadata::SetValue(const std::string &key, const std::string &value)
{
    (void)value;
    (void)key;
}

ExifData ExifMetadata::GetData() const
{
    return *exifData_;
}

} // namespace Media
} // namespace OHOS
