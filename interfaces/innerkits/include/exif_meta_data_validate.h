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
  static bool hasValueFormatValidate(const std::string &tagname);
  static int32_t isValueRangeValidate(const std::string &tagname, const std::string &value);
  static int32_t isValueFormatValidate(const std::string &tagname, std::string &value);
  static bool GetExiv2TagByName(const std::string &name, std::string &tag);
  static int32_t exifValidate(const std::string &name, std::string &tagname, std::string &value);
  static bool isModifyAllow(const std::string &tagname);
private:
  static std::pair<std::function<int32_t (std::string&, const std::string&)>, std::string> int2Blank;
  static std::pair<std::function<int32_t (std::string&, const std::string&)>, std::string> int2Comma;
  static std::pair<std::function<int32_t (std::string&, const std::string&)>, std::string> int3Blank;
  static std::pair<std::function<int32_t (std::string&, const std::string&)>, std::string> int3Comma;
  static std::pair<std::function<int32_t (std::string&, const std::string&)>, std::string> int4Blank;
  static std::pair<std::function<int32_t (std::string&, const std::string&)>, std::string> int4Comma;
  static std::pair<std::function<int32_t (std::string&, const std::string&)>, std::string> rational1;
  static std::pair<std::function<int32_t (std::string&, const std::string&)>, std::string> int1Rational1;
  static std::pair<std::function<int32_t (std::string&, const std::string&)>, std::string> decimal1Rational1;
  static std::pair<std::function<int32_t (std::string&, const std::string&)>, std::string> rational3Blank;
  static std::pair<std::function<int32_t (std::string&, const std::string&)>, std::string> int3Rational3Blank;
  static std::pair<std::function<int32_t (std::string&, const std::string&)>, std::string> int3Ratiional3Comma;
  static std::pair<std::function<int32_t (std::string&, const std::string&)>, std::string> decimal3Ratiional3;
  static std::pair<std::function<int32_t (std::string&, const std::string&)>, std::string> decimal3Ratiional3Comma;
  static std::pair<std::function<int32_t (std::string&, const std::string&)>, std::string> rational4Blank;
  static std::pair<std::function<int32_t (std::string&, const std::string&)>, std::string> int4Rational4Blank;
  static std::pair<std::function<int32_t (std::string&, const std::string&)>, std::string> int4Ratiional4Comma;
  static std::pair<std::function<int32_t (std::string&, const std::string&)>, std::string> decimal4Ratiional4;
  static std::pair<std::function<int32_t (std::string&, const std::string&)>, std::string> decimal4Ratiional4Comma;
  static std::pair<std::function<int32_t (std::string&, const std::string&)>, std::string> datetimeRegex;
  static std::pair<std::function<int32_t (std::string&, const std::string&)>, std::string> dateRegex;
  static std::pair<std::function<int32_t (std::string&, const std::string&)>, std::string> int3Rational3Colon;
  static std::pair<std::function<int32_t (std::string&, const std::string&)>, std::string> int4Rational4;
  static std::pair<std::function<int32_t (std::string&, const std::string&)>, std::string> int4Rational4Pointer;
private:
  static int gcd(int a, int b)
  {
    if (b == 0)
    {
      return a;
    }
    else
    {
      return gcd(b, a % b);
    }
  }
  static bool isValidValue(const TagDetails *array, const size_t &size, const int64_t &key);
  static bool validRegex(const std::string &value, const std::string &regex);
  static bool validRegexWithComma(std::string &value, const std::string &regex);
  static bool validRegexWithRationalFix(std::string &value, const std::string &regex);
  static bool validRegexWithCommaRationalFix(std::string &value, const std::string &regex);
  static bool validRegexWithColonRationalFix(std::string &value, const std::string &regex);
  static bool validRegexWithPoint(std::string &value, const std::string &regex);
  static bool validRegxWithCommaDecimalRationalFix(std::string &value, const std::string &regex);
  static bool validRegexWithDecimalRationalFix(std::string &value, const std::string &regex);
  static void replaceAsSpace(std::string &value, const std::string &regex);
  static void rationalFix(std::string &value);
  static std::string getFractionFromStr(const std::string &decimal);
  static void decimalRationalFix(std::string &value);
private:
  static std::multimap<std::string, std::pair<std::function<int32_t (std::string&, const std::string&)>, std::string>> valueFormatValidateConfig;
  static std::map<std::string, std::tuple<const TagDetails*, const size_t>> valueRangeValidateConfig;
};

} // namespace Media
} // namespace OHOS

#endif // INTERFACES_INNERKITS_INCLUDE_EXIF_META_DATA_VALIDATE_H
