#include "exif_metadata.h"
#include "image_log.h"


namespace OHOS {
namespace Media {

// joseph.xu codereview 遵循华为原有的逻辑 如果找不到exif需要返回错误码
std::string ExifMetadata::GetValue(const std::string& key)
{
    auto tag = exif_tag_from_name(key.c_str());
    auto entry = exif_data_get_entry(exifData_, tag);
    if (entry == nullptr) {
        IMAGE_LOGD("exif_data_get_entry leave");
        return "";
    }
    char tagValueChar[1024];
    exif_entry_get_value(entry, tagValueChar, sizeof(tagValueChar));
    return tagValueChar;
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