#include "exif_metadata.h"
#include "exif_metadata_validate.h"
#include "image_log.h"
#include "media_errors.h"
#include "securec.h"
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <stdint.h>

namespace OHOS {
namespace Media {

template <typename T>
std::istream& fromStreamToRational(std::istream& is, T& r) {
    int32_t nominator = 0;
    int32_t denominator = 0;
    char c('\0');
    is >> nominator >> c >> denominator;
    if (c != '/')
        is.setstate(std::ios::failbit);
    if (is)
        r = { nominator, denominator };
    return is;
}

std::istream& operator>>(std::istream& is, ExifRational& r) {
    return fromStreamToRational(is, r);
}
std::istream& operator>>(std::istream& is, ExifSRational& r) {
    return fromStreamToRational(is, r);
}

std::map<ExifTag, ExifIfd> TagIfdTable = {
    {EXIF_TAG_ORIENTATION, EXIF_IFD_0},
    {EXIF_TAG_BITS_PER_SAMPLE, EXIF_IFD_0},
    {EXIF_TAG_IMAGE_LENGTH, EXIF_IFD_0},
    {EXIF_TAG_IMAGE_WIDTH, EXIF_IFD_0},
    {EXIF_TAG_DATE_TIME, EXIF_IFD_0},
    {EXIF_TAG_IMAGE_DESCRIPTION, EXIF_IFD_0},
    {EXIF_TAG_MAKE, EXIF_IFD_0},
    {EXIF_TAG_MODEL, EXIF_IFD_0},
    {EXIF_TAG_DATE_TIME_ORIGINAL, EXIF_IFD_EXIF},
    {EXIF_TAG_EXPOSURE_TIME, EXIF_IFD_EXIF},
    {EXIF_TAG_FNUMBER, EXIF_IFD_EXIF},
    {EXIF_TAG_ISO_SPEED_RATINGS, EXIF_IFD_EXIF},
    {EXIF_TAG_SCENE_TYPE, EXIF_IFD_EXIF},
    {EXIF_TAG_COMPRESSED_BITS_PER_PIXEL, EXIF_IFD_EXIF},
    {EXIF_TAG_SENSITIVITY_TYPE, EXIF_IFD_EXIF},
    {EXIF_TAG_STANDARD_OUTPUT_SENSITIVITY, EXIF_IFD_EXIF},
    {EXIF_TAG_RECOMMENDED_EXPOSURE_INDEX, EXIF_IFD_EXIF},
    {EXIF_TAG_APERTURE_VALUE, EXIF_IFD_EXIF},
    {EXIF_TAG_EXPOSURE_BIAS_VALUE, EXIF_IFD_EXIF},
    {EXIF_TAG_METERING_MODE, EXIF_IFD_EXIF},
    {EXIF_TAG_LIGHT_SOURCE, EXIF_IFD_EXIF},
    {EXIF_TAG_METERING_MODE, EXIF_IFD_EXIF},
    {EXIF_TAG_FLASH, EXIF_IFD_EXIF},
    {EXIF_TAG_FOCAL_LENGTH, EXIF_IFD_EXIF},
    {EXIF_TAG_USER_COMMENT, EXIF_IFD_EXIF},
    {EXIF_TAG_PIXEL_X_DIMENSION, EXIF_IFD_EXIF},
    {EXIF_TAG_PIXEL_Y_DIMENSION, EXIF_IFD_EXIF},
    {EXIF_TAG_WHITE_BALANCE, EXIF_IFD_EXIF},
    {EXIF_TAG_FOCAL_LENGTH_IN_35MM_FILM, EXIF_IFD_EXIF},
    {EXIF_TAG_GPS_LATITUDE, EXIF_IFD_GPS},
    {EXIF_TAG_GPS_LONGITUDE, EXIF_IFD_GPS},
    {EXIF_TAG_GPS_LATITUDE_REF, EXIF_IFD_GPS},
    {EXIF_TAG_GPS_LONGITUDE_REF, EXIF_IFD_GPS},
};

std::set<ExifTag> UndefinedFormat = { EXIF_TAG_USER_COMMENT, EXIF_TAG_SCENE_TYPE };

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

int32_t ExifMetadata::SetValue_(const std::string &key, const std::string &value, std::string &errorstr)
{
    std::string valuefix = value;
    auto error = ExifMetaDataValidate::ExifValidateConvert(key, valuefix);
    if(error){
        IMAGE_LOGE("[SetValue_] ExifValidateConvert fail.");
        errorstr = "[SetValue_] ExifValidateConvert fail.";
        return error;
    }
    IMAGE_LOGD("[SetValue_] valuefix is [%{public}s]", valuefix.c_str());
    int valueLen = valuefix.length();

    ExifTag tag = exif_tag_from_name(key.c_str());
     /*需要特别处理 TagId冲突字段
     EXIF_TAG_GPS_LATITUDE vs INTEROPERABILITY_VERSION 0x0002
     EXIF_TAG_GPS_LATITUDE_REF vs INTEROPERABILITY_INDEX 0x0001*/
    ExifEntry* entry;
    if (tag == 0x0001 || tag == 0x0002)
    {
        ExifIfd ifd = exif_ifd_from_name(key.c_str());
        entry = exif_content_get_entry(exifData_->ifd[ifd], tag);
    }else {
        entry = exif_data_get_entry(exifData_, tag);
    }

    // 如果原有exifData不存在此tag需要初始化
    if (entry == nullptr)
    {
        IMAGE_LOGD("[SetValue_] entry is nullptr.");
        errorstr = "[SetValue_] entry is nullptr.";
        // 需要判断是否为undefined tag
        if (UndefinedFormat.find(tag) != UndefinedFormat.end())
        {
            IMAGE_LOGD("[SetValue_] key name is [%{public}s] format is Undefined.", key.c_str());
            /* Create a memory allocator to manage this ExifEntry */
            ExifMem* exifMem = exif_mem_new_default();
            if (exifMem == nullptr) {
                IMAGE_LOGD("[SetValue_] exif_mem_new_default fail.");
                return Media::ERR_IMAGE_DECODE_EXIF_UNSUPPORT;
            }
            /* Create a new ExifEntry using our allocator */
            entry = exif_entry_new_mem(exifMem);
            if (entry == nullptr) {
                IMAGE_LOGD("[SetValue_] exif_entry_new_mem fail.");
                return Media::ERR_IMAGE_DECODE_EXIF_UNSUPPORT;
            }
            /* Allocate memory to use for holding the tag data */
            void* buffer = exif_mem_alloc(exifMem, valueLen);
            if (buffer == nullptr) {
                IMAGE_LOGD("[SetValue_] allocate memory exif_mem_alloc fail.");
                return Media::ERR_IMAGE_DECODE_EXIF_UNSUPPORT;
            }
            /* Fill in the entry */
            entry->data = static_cast<unsigned char*>(buffer);
            entry->size = valueLen;
            entry->tag = tag;
            entry->components = valueLen;
            entry->format = EXIF_FORMAT_UNDEFINED;
            /* Attach the ExifEntry to an IFD */
            exif_content_add_entry(exifData_->ifd[TagIfdTable[tag]], entry);
            /* The ExifMem and ExifEntry are now owned elsewhere */
            exif_mem_unref(exifMem);
            exif_entry_unref(entry);
        }
        else {
            entry = exif_entry_new();
            if (entry == nullptr) {
                IMAGE_LOGD("[SetValue_] exif_entry_new fail.");
                return Media::ERR_IMAGE_DECODE_EXIF_UNSUPPORT;
            }
            entry->tag = tag; // tag must be set before calling exif_content_add_entry
            exif_content_add_entry(exifData_->ifd[TagIfdTable[tag]], entry);
            exif_entry_initialize(entry, tag);
            exif_entry_unref(entry);
        }
    }

    // 如果空间不一致需重新分配
    if ((entry->format == EXIF_FORMAT_UNDEFINED || entry->format == EXIF_FORMAT_ASCII) && (entry->size != (unsigned int)valueLen)) {
        /* Create a memory allocator to manage this ExifEntry */
        ExifMem* exifMem = exif_mem_new_default();
        if (exifMem == nullptr) {
            IMAGE_LOGD("[SetValue_] undeinfed or ascii exif_mem_new_default fail.");
            return Media::ERR_IMAGE_DECODE_EXIF_UNSUPPORT;
        }
        auto buf = exif_mem_realloc(exifMem, entry->data, valueLen);
        if (buf != nullptr) {
            entry->data = static_cast<unsigned char*>(buf);
            entry->size = exif_format_get_size(entry->format) * valueLen;
            entry->components = exif_format_get_size(entry->format) * valueLen; 
        }
        exif_mem_unref(exifMem);
    }

    ExifByteOrder o = exif_data_get_byte_order(entry->parent->parent);

    switch (entry->format)
    {
    case EXIF_FORMAT_SHORT: {
        std::istringstream is(valuefix);
        unsigned long icount = 0;
        ExifShort tmp;
        while (!(is.eof()) && entry->components > icount) {
            is >> tmp;
            if (is.fail()){
                IMAGE_LOGE("[SetValue_] istringstream read ExifShort fail.");
                return Media::ERR_IMAGE_DECODE_EXIF_UNSUPPORT;
            }
            exif_set_short(
                entry->data + icount * exif_format_get_size(entry->format),
                o, tmp);
            icount++;
        }
        break;
    }
    case EXIF_FORMAT_LONG: {
        std::istringstream is(valuefix);
        unsigned long icount = 0;
        ExifLong tmp;
        while (!(is.eof()) && entry->components > icount) {
            is >> tmp;
            if (is.fail()){
                IMAGE_LOGE("[SetValue_] istringstream read ExifLong fail.");
                return Media::ERR_IMAGE_DECODE_EXIF_UNSUPPORT;
            }
            exif_set_long(
                entry->data + icount * exif_format_get_size(entry->format),
                o, tmp);
            icount++;
        }
        break;
    }
    case EXIF_FORMAT_SSHORT: {
        std::istringstream is(valuefix);
        unsigned long icount = 0;
        ExifSShort tmp;
        while (!(is.eof()) && entry->components > icount) {
            is >> tmp;
            if (is.fail()){
                IMAGE_LOGE("[SetValue_] istringstream read ExifShort fail.");
                return Media::ERR_IMAGE_DECODE_EXIF_UNSUPPORT;
            }
            exif_set_sshort(
                entry->data + icount * exif_format_get_size(entry->format),
                o, tmp);
            icount++;
        }
        break;
    }
    case EXIF_FORMAT_SLONG: {
        std::istringstream is(valuefix);
        unsigned long icount = 0;
        ExifSLong tmp;
        while (!(is.eof()) && entry->components > icount) {
            is >> tmp;
            if (is.fail()){
                IMAGE_LOGE("[SetValue_] istringstream read ExifSLong fail.");
                return Media::ERR_IMAGE_DECODE_EXIF_UNSUPPORT;
            }
            exif_set_slong(
                entry->data + icount * exif_format_get_size(entry->format),
                o, tmp);
            icount++;
        }
        break;
    }
    case EXIF_FORMAT_RATIONAL: {
        std::istringstream is(valuefix);
        unsigned long icount = 0;
        ExifRational rat;
        errorstr = valuefix;
        while (!(is.eof()) && entry->components > icount) {
            is >> rat;
            if (is.fail()){
                IMAGE_LOGE("[SetValue_] istringstream read ExifRational fail.");
                return Media::ERR_IMAGE_DECODE_EXIF_UNSUPPORT;
            }
            int offset = icount * exif_format_get_size(entry->format);
            exif_set_rational(
                entry->data + offset,
                o, rat);
            icount++;
        }
        break;
    }
    case EXIF_FORMAT_SRATIONAL: {
        std::istringstream is(valuefix);
        unsigned long icount = 0;
        ExifSRational rat;
        while (!(is.eof()) && entry->components > icount) {
            is >> rat;
            if (is.fail()){
                IMAGE_LOGE("[SetValue_] istringstream read ExifSRational fail.");
                return Media::ERR_IMAGE_DECODE_EXIF_UNSUPPORT;
            }
            int offset = icount * exif_format_get_size(entry->format);
            exif_set_srational(
                entry->data + offset,
                o, rat);
            icount++;
        }
        break;
    }
    case EXIF_FORMAT_UNDEFINED:
    case EXIF_FORMAT_ASCII:{
        if (memcpy_s((entry)->data, valueLen, valuefix.c_str(), valueLen) != 0) {
            IMAGE_LOGE("[SetValue_] memcpy_s error tag is [%{public}d].", tag);
            errorstr = "[SetValue_] memcpy_s error";
            return Media::ERR_IMAGE_DECODE_EXIF_UNSUPPORT;
        }
    }
        break;
    default:
        break;
    }

    return Media::SUCCESS;
}

ExifData ExifMetadata::GetData() const
{
    return *exifData_;
}

} // namespace Media
} // namespace OHOS
