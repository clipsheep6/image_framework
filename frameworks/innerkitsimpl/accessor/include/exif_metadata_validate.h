/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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
struct TagDetails
{
  int64_t val_;       //!< Tag value
  const char *label_; //!< Translation of the tag value

  //! Comparison operator for use with the find template
  bool operator==(int64_t key) const
  {
    return val_ == key;
  }
}; // struct TagDetails


class ExifMetaDataValidate
{
public:
  static bool HasValueFormatValidate(const std::string &keyName);
  static int32_t IsValueRangeValidate(const std::string &keyName, const std::string &value);
  static int32_t IsValueFormatValidate(const std::string &keyName, std::string &value);
  static int32_t ExifValidateConvert(const std::string &keyName, std::string &value);
  static int32_t ExifValidate(const std::string &keyName, const std::string &value);
  static bool IsSupportKey(const std::string &keyName);
  static bool IsModifyAllow(const std::string &keyName);
private:
  static std::pair<std::function<int32_t (std::string&, const std::string&)>, std::string> doubleIntWithBlank;
  static std::pair<std::function<int32_t (std::string&, const std::string&)>, std::string> doubleIntWithComma;
  static std::pair<std::function<int32_t (std::string&, const std::string&)>, std::string> tribleIntWithBlank;
  static std::pair<std::function<int32_t (std::string&, const std::string&)>, std::string> tribleIntWithComma;
  static std::pair<std::function<int32_t (std::string&, const std::string&)>, std::string> fourIntWithBlank;
  static std::pair<std::function<int32_t (std::string&, const std::string&)>, std::string> fourIntWithComma;
  static std::pair<std::function<int32_t (std::string&, const std::string&)>, std::string> oneRational;
  static std::pair<std::function<int32_t (std::string&, const std::string&)>, std::string> oneIntToRational;
  static std::pair<std::function<int32_t (std::string&, const std::string&)>, std::string> oneDecimalToRational;
  static std::pair<std::function<int32_t (std::string&, const std::string&)>, std::string> tribleRationalWithBlank;
  static std::pair<std::function<int32_t (std::string&, const std::string&)>, std::string> tribleIntToRationalWithBlank;
  static std::pair<std::function<int32_t (std::string&, const std::string&)>, std::string> tribleIntToRationalWithComma;
  static std::pair<std::function<int32_t (std::string&, const std::string&)>, std::string> tribleDecimalToRationalWithBlank;
  static std::pair<std::function<int32_t (std::string&, const std::string&)>, std::string> tribleDecimalToRatiionalWithComma;
  static std::pair<std::function<int32_t (std::string&, const std::string&)>, std::string> fourRationalWithBlank;
  static std::pair<std::function<int32_t (std::string&, const std::string&)>, std::string> fourIntToRationalWithBlank;
  static std::pair<std::function<int32_t (std::string&, const std::string&)>, std::string> fourIntToRationalWithComma;
  static std::pair<std::function<int32_t (std::string&, const std::string&)>, std::string> decimal4Ratiional4;
  static std::pair<std::function<int32_t (std::string&, const std::string&)>, std::string> decimal4Ratiional4Comma;
  static std::pair<std::function<int32_t (std::string&, const std::string&)>, std::string> dateTimeValidation;
  static std::pair<std::function<int32_t (std::string&, const std::string&)>, std::string> dateValidation;
  static std::pair<std::function<int32_t (std::string&, const std::string&)>, std::string> tribleIntToRationalWithColon;
  static std::pair<std::function<int32_t (std::string&, const std::string&)>, std::string> fourIntToRationalWithDot;
private:
  static int Gcd(int a, int b)
  {
    if (b == 0)
    {
      return a;
    }
    else
    {
      return Gcd(b, a % b);
    }
  }
  static bool IsValidValue(const TagDetails *array, const size_t &size, const int64_t &key);
  static bool ValidRegex(const std::string &value, const std::string &regex);
  static bool ValidRegexWithComma(std::string &value, const std::string &regex);
  static bool ValidRegexWithRationalFormat(std::string &value, const std::string &regex);
  static bool ValidRegexWithCommaRationalFormat(std::string &value, const std::string &regex);
  static bool ValidRegexWithColonRationalFormat(std::string &value, const std::string &regex);
  static bool ValidRegexWithDot(std::string &value, const std::string &regex);
  static bool ValidRegxWithCommaDecimalRationalFormat(std::string &value, const std::string &regex);
  static bool ValidRegexWithDecimalRationalFormat(std::string &value, const std::string &regex);
  static void ReplaceAsSpace(std::string &value, const std::string &regex);
  static void RationalFormat(std::string &value);
  static std::string GetFractionFromStr(const std::string &decimal);
  static void DecimalRationalFormat(std::string &value);
private:
  static std::multimap<std::string, std::pair<std::function<int32_t (std::string&, const std::string&)>, std::string>> valueFormatValidateConfig;
  static std::map<std::string, std::tuple<const TagDetails*, const size_t>> valueRangeValidateConfig;
};

} // namespace Media
} // namespace OHOS

#endif // INTERFACES_INNERKITS_INCLUDE_EXIF_META_DATA_VALIDATE_H
