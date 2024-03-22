/*
 * Copyright (C) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <iostream>
#include <map>
#include <ostream>
#include <set>
#include <sstream>
#include <vector>

#include "exif_metadata.h"
#include "exif_metadata_formatter.h"
#include "image_log.h"
#include "libexif/exif-format.h"
#include "libexif/exif-tag.h"
#include "libexif/huawei/exif-mnote-data-huawei.h"
#include "libexif/huawei/mnote-huawei-entry.h"
#include "libexif/huawei/mnote-huawei-tag.h"
#include "libexif/huawei/mnote-huawei-data-type.h"
#include "media_errors.h"
#include "securec.h"
#include "string_ex.h"

#undef LOG_DOMAIN
#define LOG_DOMAIN LOG_TAG_DOMAIN_ID_IMAGE

#undef LOG_TAG
#define LOG_TAG "ExifMetadata"

namespace OHOS {
namespace Media {
const auto KEY_SIZE = 2;
const auto TAG_VALUE_SIZE = 1024;

template <typename T> std::istream &OutputRational(std::istream &is, T &r)
{
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

std::istream &operator >> (std::istream &is, ExifRational &r)
{
    return OutputRational(is, r);
}

std::istream &operator >> (std::istream &is, ExifSRational &r)
{
    return OutputRational(is, r);
}

std::map<ExifTag, ExifIfd> TagIfdTable = { { EXIF_TAG_ORIENTATION, EXIF_IFD_0 },
                                           { EXIF_TAG_BITS_PER_SAMPLE, EXIF_IFD_0 },
                                           { EXIF_TAG_IMAGE_LENGTH, EXIF_IFD_0 },
                                           { EXIF_TAG_IMAGE_WIDTH, EXIF_IFD_0 },
                                           { EXIF_TAG_DATE_TIME, EXIF_IFD_0 },
                                           { EXIF_TAG_IMAGE_DESCRIPTION, EXIF_IFD_0 },
                                           { EXIF_TAG_MAKE, EXIF_IFD_0 },
                                           { EXIF_TAG_MODEL, EXIF_IFD_0 },
                                           { EXIF_TAG_DATE_TIME_ORIGINAL, EXIF_IFD_EXIF },
                                           { EXIF_TAG_EXPOSURE_TIME, EXIF_IFD_EXIF },
                                           { EXIF_TAG_FNUMBER, EXIF_IFD_EXIF },
                                           { EXIF_TAG_ISO_SPEED_RATINGS, EXIF_IFD_EXIF },
                                           { EXIF_TAG_SCENE_TYPE, EXIF_IFD_EXIF },
                                           { EXIF_TAG_COMPRESSED_BITS_PER_PIXEL, EXIF_IFD_EXIF },
                                           { EXIF_TAG_SENSITIVITY_TYPE, EXIF_IFD_EXIF },
                                           { EXIF_TAG_STANDARD_OUTPUT_SENSITIVITY, EXIF_IFD_EXIF },
                                           { EXIF_TAG_RECOMMENDED_EXPOSURE_INDEX, EXIF_IFD_EXIF },
                                           { EXIF_TAG_APERTURE_VALUE, EXIF_IFD_EXIF },
                                           { EXIF_TAG_EXPOSURE_BIAS_VALUE, EXIF_IFD_EXIF },
                                           { EXIF_TAG_METERING_MODE, EXIF_IFD_EXIF },
                                           { EXIF_TAG_LIGHT_SOURCE, EXIF_IFD_EXIF },
                                           { EXIF_TAG_METERING_MODE, EXIF_IFD_EXIF },
                                           { EXIF_TAG_FLASH, EXIF_IFD_EXIF },
                                           { EXIF_TAG_FOCAL_LENGTH, EXIF_IFD_EXIF },
                                           { EXIF_TAG_USER_COMMENT, EXIF_IFD_EXIF },
                                           { EXIF_TAG_PIXEL_X_DIMENSION, EXIF_IFD_EXIF },
                                           { EXIF_TAG_PIXEL_Y_DIMENSION, EXIF_IFD_EXIF },
                                           { EXIF_TAG_WHITE_BALANCE, EXIF_IFD_EXIF },
                                           { EXIF_TAG_FOCAL_LENGTH_IN_35MM_FILM, EXIF_IFD_EXIF },
                                           { EXIF_TAG_GPS_VERSION_ID, EXIF_IFD_GPS },
                                           { EXIF_TAG_GPS_LATITUDE_REF, EXIF_IFD_GPS },
                                           { EXIF_TAG_GPS_LATITUDE, EXIF_IFD_GPS },
                                           { EXIF_TAG_GPS_LONGITUDE_REF, EXIF_IFD_GPS },
                                           { EXIF_TAG_GPS_LONGITUDE, EXIF_IFD_GPS },
                                           { EXIF_TAG_GPS_ALTITUDE_REF, EXIF_IFD_GPS },
                                           { EXIF_TAG_GPS_ALTITUDE, EXIF_IFD_GPS },
                                           { EXIF_TAG_GPS_TIME_STAMP, EXIF_IFD_GPS },
                                           { EXIF_TAG_GPS_SATELLITES, EXIF_IFD_GPS },
                                           { EXIF_TAG_GPS_STATUS, EXIF_IFD_GPS },
                                           { EXIF_TAG_GPS_MEASURE_MODE, EXIF_IFD_GPS },
                                           { EXIF_TAG_GPS_DOP, EXIF_IFD_GPS },
                                           { EXIF_TAG_GPS_SPEED_REF, EXIF_IFD_GPS },
                                           { EXIF_TAG_GPS_SPEED, EXIF_IFD_GPS },
                                           { EXIF_TAG_GPS_TRACK_REF, EXIF_IFD_GPS },
                                           { EXIF_TAG_GPS_TRACK, EXIF_IFD_GPS },
                                           { EXIF_TAG_GPS_IMG_DIRECTION_REF, EXIF_IFD_GPS },
                                           { EXIF_TAG_GPS_IMG_DIRECTION, EXIF_IFD_GPS },
                                           { EXIF_TAG_GPS_MAP_DATUM, EXIF_IFD_GPS },
                                           { EXIF_TAG_GPS_DEST_LATITUDE_REF, EXIF_IFD_GPS },
                                           { EXIF_TAG_GPS_DEST_LATITUDE, EXIF_IFD_GPS },
                                           { EXIF_TAG_GPS_DEST_LONGITUDE_REF, EXIF_IFD_GPS },
                                           { EXIF_TAG_GPS_DEST_LONGITUDE, EXIF_IFD_GPS },
                                           { EXIF_TAG_GPS_DEST_BEARING_REF, EXIF_IFD_GPS },
                                           { EXIF_TAG_GPS_DEST_BEARING, EXIF_IFD_GPS },
                                           { EXIF_TAG_GPS_DEST_DISTANCE_REF, EXIF_IFD_GPS },
                                           { EXIF_TAG_GPS_DEST_DISTANCE, EXIF_IFD_GPS },
                                           { EXIF_TAG_GPS_PROCESSING_METHOD, EXIF_IFD_GPS },
                                           { EXIF_TAG_GPS_AREA_INFORMATION, EXIF_IFD_GPS },
                                           { EXIF_TAG_GPS_DATE_STAMP, EXIF_IFD_GPS },
                                           { EXIF_TAG_GPS_DIFFERENTIAL, EXIF_IFD_GPS } };

std::set<ExifTag> UndefinedByte = { EXIF_TAG_SCENE_TYPE, EXIF_TAG_COMPONENTS_CONFIGURATION, EXIF_TAG_FILE_SOURCE };

ExifMetadata::ExifMetadata() : exifData_(nullptr) {}

ExifMetadata::ExifMetadata(ExifData *exifData) : exifData_(exifData) {}

ExifMetadata::~ExifMetadata()
{
    if (exifData_ != nullptr) {
        exif_data_unref(exifData_);
        exifData_ = nullptr;
    }
}

int ExifMetadata::GetValue(const std::string &key, std::string &value) const
{
    IMAGE_LOGD("Retrieving value for key: %{public}s", key.c_str());
    char tagValueChar[TAG_VALUE_SIZE];
    if (exifData_ == nullptr) {
        IMAGE_LOGE("Exif data is null for key: %{public}s", key.c_str());
        value = "";
        return ERR_MEDIA_NO_EXIF_DATA;
    }
    if (key.size() > KEY_SIZE && key.substr(0, KEY_SIZE) == "Hw") {
        ExifMnoteData *md = exif_data_get_mnote_data(exifData_);
        if (!md) {
            IMAGE_LOGE("Exif data returned null for key: %{public}s", key.c_str());
            exif_data_unref(exifData_);
            value = "";
            return ERR_MEDIA_NO_EXIF_DATA;
        }

        MnoteHuaweiEntryCount *ec = nullptr;
        mnote_huawei_get_entry_count((ExifMnoteDataHuawei *)md, &ec);
        for (unsigned int i = 0; i < ec->size; i++) {
            MnoteHuaweiEntry *entry = ec->entries[i];
            if (key == mnote_huawei_tag_get_name(entry->tag)) {
                mnote_huawei_entry_get_value(entry, tagValueChar, sizeof(tagValueChar));
                value = tagValueChar;
            }
        }
        mnote_huawei_free_entry_count(ec);
    } else {
        auto tag = exif_tag_from_name(key.c_str());
        auto entry = exif_data_get_entry(exifData_, tag);
        if (entry == nullptr) {
            IMAGE_LOGE("Exif data entry returned null for key: %{public}s, tag: %{public}d", key.c_str(), tag);
            value = "";
            return ERR_MEDIA_NO_EXIF_DATA;
        }
        IMAGE_LOGD("Using exif_entry_get_value for key: %{public}s, tag: %{public}d", key.c_str(), entry->tag);
        exif_entry_get_value(entry, tagValueChar, sizeof(tagValueChar));
        value = tagValueChar;
    }
    IMAGE_LOGD("Retrieved value for key: %{public}s is: %{public}s", key.c_str(), value.c_str());
    return SUCCESS;
}

ExifData *ExifMetadata::GetExifData()
{
    return exifData_;
}

bool ExifMetadata::CreateExifdata()
{
    if (exifData_ != nullptr) {
        exif_data_unref(exifData_);
        exifData_ = nullptr;
        exifData_ = exif_data_new();
        if (exifData_ == nullptr) {
            IMAGE_LOGE("Failed to recreate exif data after unref.");
            return false;
        }

        // Set the image options
        exif_data_set_option(exifData_, EXIF_DATA_OPTION_FOLLOW_SPECIFICATION);
        exif_data_set_data_type(exifData_, EXIF_DATA_TYPE_COMPRESSED);
        exif_data_set_byte_order(exifData_, EXIF_BYTE_ORDER_INTEL);

        // Create the mandatory EXIF fields with default data
        exif_data_fix(exifData_);
        return true;
    }
    exifData_ = exif_data_new();
    if (exifData_ == nullptr) {
        IMAGE_LOGE("Failed to create new exif data.");
        return false;
    }

    // Set the image options
    exif_data_set_option(exifData_, EXIF_DATA_OPTION_FOLLOW_SPECIFICATION);
    exif_data_set_data_type(exifData_, EXIF_DATA_TYPE_COMPRESSED);
    exif_data_set_byte_order(exifData_, EXIF_BYTE_ORDER_INTEL);

    // Create the mandatory EXIF fields with default data
    exif_data_fix(exifData_);
    IMAGE_LOGD("New exif data created.");
    return true;
}

ExifEntry *ExifMetadata::CreateEntry(const ExifTag &tag, const size_t valueLen)
{
    ExifEntry *entry = exif_entry_new();
    if (entry == nullptr) {
        IMAGE_LOGE("Failed to create new ExifEntry.");
        return nullptr;
    }
    entry->tag = tag; // tag must be set before calling exif_content_add_entry
    exif_content_add_entry(exifData_->ifd[TagIfdTable[tag]], entry);
    exif_entry_initialize(entry, tag);

    if (entry->format == EXIF_FORMAT_UNDEFINED && entry->size != valueLen) {
        exif_content_remove_entry(exifData_->ifd[TagIfdTable[tag]], entry);

        // Create a memory allocator to manage this ExifEntry
        ExifMem *exifMem = exif_mem_new_default();
        if (exifMem == nullptr) {
            IMAGE_LOGE("Failed to create memory allocator for ExifEntry.");
            return nullptr;
        }

        // Create a new ExifEntry using our allocator
        entry = exif_entry_new_mem(exifMem);
        if (entry == nullptr) {
            IMAGE_LOGE("Failed to create new ExifEntry using memory allocator.");
            exif_mem_unref(exifMem);
            return nullptr;
        }

        // Allocate memory to use for holding the tag data
        void *buffer = exif_mem_alloc(exifMem, valueLen);
        if (buffer == nullptr) {
            IMAGE_LOGE("Failed to allocate memory for tag data.");
            exif_entry_unref(entry);
            exif_mem_unref(exifMem);
            return nullptr;
        }

        // Fill in the entry
        entry->data = static_cast<unsigned char *>(buffer);
        entry->size = valueLen;
        entry->tag = tag;
        entry->components = valueLen;
        entry->format = EXIF_FORMAT_UNDEFINED;

        // Attach the ExifEntry to an IFD
        exif_content_add_entry(exifData_->ifd[TagIfdTable[tag]], entry);

        // The ExifMem and ExifEntry are now owned elsewhere
        exif_mem_unref(exifMem);
        exif_entry_unref(entry);
    }
    return entry;
}

void ExifMetadata::ReallocEntry(ExifEntry *ptrEntry, const size_t valueLen)
{
    // Create a memory allocator to manage this ExifEntry
    ExifMem *exifMem = exif_mem_new_default();
    if (exifMem == nullptr) {
        IMAGE_LOGE("Failed to create memory allocator for ExifEntry. Value length: %{public}zu", valueLen);
        return;
    }
    auto buf = exif_mem_realloc(exifMem, ptrEntry->data, valueLen);
    if (buf != nullptr) {
        ptrEntry->data = static_cast<unsigned char *>(buf);
        ptrEntry->size = exif_format_get_size(ptrEntry->format) * valueLen;
        ptrEntry->components = exif_format_get_size(ptrEntry->format) * valueLen;
    } else {
        IMAGE_LOGE("Failed to reallocate memory for ExifEntry. Requested size: %{public}zu", valueLen);
    }
    exif_mem_unref(exifMem);
}

ExifEntry *ExifMetadata::GetEntry(const std::string &key, const size_t valueLen)
{
    IMAGE_LOGD("GetEntry key is %{public}s.", key.c_str());
    ExifTag tag = exif_tag_from_name(key.c_str());
    ExifEntry *entry;
    if (tag == 0x0001 || tag == 0x0002) {
        ExifIfd ifd = exif_ifd_from_name(key.c_str());
        entry = exif_content_get_entry(exifData_->ifd[ifd], tag);
    } else {
        entry = exif_data_get_entry(exifData_, tag);
    }

    if (entry == nullptr) {
        IMAGE_LOGD("GetEntry entry is nullptr and try to create entry.");
        entry = CreateEntry(tag, valueLen);
    }

    if (entry == nullptr) {
        IMAGE_LOGE("GetEntry entry is nullptr fail.");
        return nullptr;
    }

    if ((entry->format == EXIF_FORMAT_UNDEFINED || entry->format == EXIF_FORMAT_ASCII)
        && (entry->size != static_cast<unsigned int>(valueLen))) {
        ReallocEntry(entry, valueLen);
    }
    return entry;
}

bool ExifMetadata::SetShort(ExifEntry *ptrEntry, const ExifByteOrder &order, const std::string &value)
{
    std::istringstream is(value);
    unsigned long icount = 0;
    ExifShort tmp;
    while (!is.eof() && ptrEntry->components > icount) {
        is >> tmp;
        if (is.fail()) {
            IMAGE_LOGE("Failed to read ExifShort from string. Current count: %{public}lu", icount);
            return false;
        }
        exif_set_short(ptrEntry->data + icount * exif_format_get_size(ptrEntry->format), order, tmp);
        icount++;
    }
    return true;
}

bool ExifMetadata::SetLong(ExifEntry *ptrEntry, const ExifByteOrder &order, const std::string &value)
{
    std::istringstream is(value);
    unsigned long icount = 0;
    ExifLong tmp;
    while (!is.eof() && ptrEntry->components > icount) {
        is >> tmp;
        if (is.fail()) {
            IMAGE_LOGE("Failed to read ExifLong from string. Current count: %{public}lu", icount);
            return false;
        }
        exif_set_long(ptrEntry->data + icount * exif_format_get_size(ptrEntry->format), order, tmp);
        icount++;
    }
    return true;
}

bool ExifMetadata::SetSShort(ExifEntry *ptrEntry, const ExifByteOrder &order, const std::string &value)
{
    std::istringstream is(value);
    unsigned long icount = 0;
    ExifSShort tmp;
    while (!is.eof() && ptrEntry->components > icount) {
        is >> tmp;
        if (is.fail()) {
            IMAGE_LOGE("Failed to read ExifSShort from string. Current count: %{public}lu", icount);
            return false;
        }
        exif_set_sshort(ptrEntry->data + icount * exif_format_get_size(ptrEntry->format), order, tmp);
        icount++;
    }
    return true;
}

bool ExifMetadata::SetSLong(ExifEntry *ptrEntry, const ExifByteOrder &order, const std::string &value)
{
    std::istringstream is(value);
    unsigned long icount = 0;
    ExifSLong tmp;
    while (!is.eof() && ptrEntry->components > icount) {
        is >> tmp;
        if (is.fail()) {
            IMAGE_LOGE("Failed to read ExifSLong from string. Current count: %{public}lu", icount);
            return false;
        }
        exif_set_slong(ptrEntry->data + icount * exif_format_get_size(ptrEntry->format), order, tmp);
        icount++;
    }
    return true;
}

bool ExifMetadata::SetRational(ExifEntry *ptrEntry, const ExifByteOrder &order, const std::string &value)
{
    std::istringstream is(value);
    unsigned long icount = 0;
    ExifRational rat;
    while (!is.eof() && ptrEntry->components > icount) {
        is >> rat;
        if (is.fail()) {
            IMAGE_LOGE("Failed to read ExifRational from string. Current count: %{public}lu", icount);
            return false;
        }
        int offset = icount * exif_format_get_size(ptrEntry->format);
        exif_set_rational(ptrEntry->data + offset, order, rat);
        icount++;
    }
    return true;
}

bool ExifMetadata::SetSRational(ExifEntry *ptrEntry, const ExifByteOrder &order, const std::string &value)
{
    std::istringstream is(value);
    unsigned long icount = 0;
    ExifSRational rat;
    while (!is.eof() && ptrEntry->components > icount) {
        is >> rat;
        if (is.fail()) {
            IMAGE_LOGE("Failed to read ExifSRational from string. Current count: %{public}lu", icount);
            return false;
        }
        int offset = icount * exif_format_get_size(ptrEntry->format);
        exif_set_srational(ptrEntry->data + offset, order, rat);
        icount++;
    }
    return true;
}

bool ExifMetadata::SetByte(ExifEntry *ptrEntry, const std::string &value)
{
    std::string result;
    for (char c : value) {
        if (c != ' ') {
            result += c;
        }
    }
    const char *p = result.c_str();
    int valueLen = result.length();
    for (int i = 0; i < valueLen && i < static_cast<int>(ptrEntry->size); i++) {
        *(ptrEntry->data + i) = p[i] - '0';
    }
    return true;
}

bool ExifMetadata::SetMem(ExifEntry *ptrEntry, const std::string &value, const size_t valueLen)
{
    if (UndefinedByte.find(ptrEntry->tag) != UndefinedByte.end()) {
        return SetByte(ptrEntry, value);
    }
    if (memcpy_s((ptrEntry)->data, valueLen, value.c_str(), valueLen) != 0) {
        IMAGE_LOGE("Failed to copy memory for ExifEntry. Requested size: %{public}zu", valueLen);
        return false;
    }
    return true;
}

bool ExifMetadata::SetValue(const std::string &key, const std::string &value)
{
    if (exifData_ == nullptr) {
        IMAGE_LOGE("Exif data is null. Cannot set value for key: %{public}s", key.c_str());
        return false;
    }

    auto result = ExifMetadatFormatter::Format(key, value);
    if (result.first) {
        IMAGE_LOGE("Failed to validate and convert value for key: %{public}s", key.c_str());
        return false;
    }
    IMAGE_LOGD("Formatted value for key %{public}s is: %{public}s", key.c_str(), result.second.c_str());
    size_t valueLen = result.second.length();

    ExifEntry *ptrEntry = GetEntry(key, valueLen);
    if (ptrEntry == nullptr) {
        IMAGE_LOGE("Failed to get ExifEntry for key: %{public}s", key.c_str());
        return false;
    }

    ExifByteOrder order = exif_data_get_byte_order(ptrEntry->parent->parent);

    bool isSetSuccess = false;
    switch (ptrEntry->format) {
        case EXIF_FORMAT_SHORT:
            isSetSuccess = SetShort(ptrEntry, order, result.second);
            break;
        case EXIF_FORMAT_LONG:
            isSetSuccess = SetLong(ptrEntry, order, result.second);
            break;
        case EXIF_FORMAT_SSHORT:
            isSetSuccess = SetSShort(ptrEntry, order, result.second);
            break;
        case EXIF_FORMAT_SLONG:
            isSetSuccess = SetSLong(ptrEntry, order, result.second);
            break;
        case EXIF_FORMAT_RATIONAL:
            isSetSuccess = SetRational(ptrEntry, order, result.second);
            break;
        case EXIF_FORMAT_SRATIONAL:
            isSetSuccess = SetSRational(ptrEntry, order, result.second);
            break;
        case EXIF_FORMAT_BYTE: {
            IMAGE_LOGD("handle EXIF_FORMAT_BYTE type.");
            isSetSuccess = SetByte(ptrEntry, result.second);
            break;
        }
        case EXIF_FORMAT_UNDEFINED:
        case EXIF_FORMAT_ASCII:
            isSetSuccess = SetMem(ptrEntry, result.second, valueLen);
            break;
        default:
            IMAGE_LOGE("Unsupported Exif format for key: %{public}s", key.c_str());
            break;
    }

    return isSetSuccess;
}
} // namespace Media
} // namespace OHOS
