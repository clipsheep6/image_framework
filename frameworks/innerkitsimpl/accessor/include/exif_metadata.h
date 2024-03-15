/*
 * Copyright (C) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef FRAMEWORKS_INNERKITSIMPL_ACCESSOR_INCLUDE_EXIF_METADATA_H
#define FRAMEWORKS_INNERKITSIMPL_ACCESSOR_INCLUDE_EXIF_METADATA_H

#include <string>

#include <libexif/exif-data.h>
#include <libexif/exif-entry.h>
#include <libexif/exif-tag.h>

namespace OHOS {
namespace Media {
class ExifMetadata {
public:
    ExifMetadata();
    ExifMetadata(ExifData *exifData);
    ~ExifMetadata();
    int GetValue(const std::string &key, std::string &value) const;
    bool SetValue(const std::string &key, const std::string &value);
    ExifData* GetExifData();
    bool CreateExifdata();

private:
    ExifEntry* InitExifTag(ExifData *exif, ExifIfd ifd, ExifTag tag);
    ExifEntry* CreateExifTag(ExifData *exif, ExifIfd ifd, ExifTag tag, size_t len, ExifFormat format);
    ExifEntry* GetExifTag(ExifData *exif, ExifIfd ifd, ExifTag tag, size_t len);
    ExifIfd GetExifIfdByExifTag(const ExifTag &tag);
    ExifFormat GetExifFormatByExifTag(const ExifTag &tag);
    std::string GetExifNameByExifTag(const ExifTag &tag);
    bool CreateExifEntryOfBitsPerSample(const ExifTag &tag, ExifData *data, const std::string &value,
        ExifByteOrder order, ExifEntry **ptrEntry);
    bool CreateExifEntryOfRationalExif(const ExifTag &tag, ExifData *data, const std::string &value,
        ExifByteOrder order, ExifEntry **ptrEntry, const std::string& separator, size_t sepSize);
    bool CreateExifEntryOfGpsTimeStamp(const ExifTag &tag, ExifData *data, const std::string &value,
        ExifByteOrder order, ExifEntry **ptrEntry);
    bool CreateExifEntryOfCompressedBitsPerPixel(const ExifTag &tag, ExifData *data, const std::string &value,
        ExifByteOrder order, ExifEntry **ptrEntry);
    bool CreateExifEntryOfGpsLatitudeOrLongitude(const ExifTag &tag, ExifData *data, const std::string &value,
        ExifByteOrder order, ExifEntry **ptrEntry);
    bool SetGpsDegreeRational(ExifData *data, ExifEntry **ptrEntry, ExifByteOrder order, const ExifTag &tag,
    const std::vector<std::string> &dataVec);
    bool SetGpsRationals(ExifData *data, ExifEntry **ptrEntry, ExifByteOrder order, const ExifTag &tag,
        const std::vector<ExifRational> &exifRationals);

private:
    ExifData *exifData_;
};
} // namespace Media
} // namespace OHOS

#endif
