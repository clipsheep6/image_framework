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
    ExifEntry* CreateEntry(const ExifTag &tag ,const size_t len);
    ExifEntry* GetEntry(const std::string &key, const size_t len);
    void ReallocEntry(ExifEntry *ptrEntry, const size_t len);
    bool SetShort(ExifEntry *ptrEntry, const ExifByteOrder &o, const std::string &value);
    bool SetSShort(ExifEntry *ptrEntry, const ExifByteOrder &o, const std::string &value);
    bool SetLong(ExifEntry *ptrEntry, const ExifByteOrder &o, const std::string &value);
    bool SetSLong(ExifEntry *ptrEntry, const ExifByteOrder &o, const std::string &value);
    bool SetRational(ExifEntry *ptrEntry, const ExifByteOrder &o, const std::string &value);
    bool SetSRational(ExifEntry *ptrEntry, const ExifByteOrder &o, const std::string &value);
    bool SetMemcpy(ExifEntry *ptrEntry, const std::string &value, const size_t len);
    ExifData *exifData_;
};
} // namespace Media
} // namespace OHOS

#endif
