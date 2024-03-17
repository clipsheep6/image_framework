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

#ifndef INTERFACES_INNERKITS_INCLUDE_EXIF_META_DATA_VALIDATE_H
#define INTERFACES_INNERKITS_INCLUDE_EXIF_META_DATA_VALIDATE_H

#include <map>
#include <string>
#include <vector>

namespace OHOS {
namespace Media {
struct TagDetails {
    int64_t val_;       // Tag value
    const char *label_;
}; // struct TagDetails

using ValueFormatDelegate = std::pair<std::function<int32_t (std::string&, const std::string&)>, std::string>;

class ExifMetadataConverter {
private:
    static int32_t ValidateValueRange(const std::string &keyName, const std::string &value);
    static int32_t ValidateValueFormat(const std::string &keyName, const std::string &value);
    static bool IsKeySupported(const std::string &keyName);
    static bool IsModifyAllowed(const std::string &keyName);
    static bool IsFormatValidationConfigExisting(const std::string &keyName);
    static bool IsValidValue(const TagDetails *array, const size_t &size, const int64_t &key);
    static bool ValidRegex(const std::string &value, const std::string &regex);
    static std::multimap<std::string, std::string> valueFormatValidateConfig;
    static std::map<std::string, std::tuple<const TagDetails*, const size_t>> valueRangeValidateConfig;
public:
    static int32_t Validate(const std::string &keyName, const std::string &value);
};
} // namespace Media
} // namespace OHOS

#endif // INTERFACES_INNERKITS_INCLUDE_EXIF_META_DATA_VALIDATE_H
