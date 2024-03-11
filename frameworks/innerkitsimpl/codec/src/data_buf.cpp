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

// included header files
#include "image_log.h"
#include "data_buf.h"
// + standard includes
#include <array>
#include <cctype>
#include <climits>
#include <cmath>
#include <cstring>
#include <iomanip>
#include <numeric>
#include <sstream>
#include <utility>

#undef LOG_DOMAIN
#define LOG_DOMAIN LOG_TAG_DOMAIN_ID_IMAGE

#undef LOG_TAG
#define LOG_TAG "DataBuffer"

// *****************************************************************************
// class member definitions
namespace OHOS {
namespace Media {

std::string upper(const std::string& str) {
  std::string result = str;
  std::transform(str.begin(), str.end(), result.begin(), [](int c) { return static_cast<char>(toupper(c)); });
  return result;
}

std::string lower(const std::string& a) {
  std::string b = a;
  std::transform(a.begin(), a.end(), b.begin(), [](int c) { return static_cast<char>(tolower(c)); });
  return b;
}

DataBuf::DataBuf(size_t size) : pData_(size) {
}

DataBuf::DataBuf(const byte* pData, size_t size) : pData_(size) {
  std::copy_n(pData, size, pData_.begin());
}

void DataBuf::Alloc(size_t size) {
  pData_.resize(size);
}

void DataBuf::Resize(size_t size) {
  pData_.resize(size);
}

void DataBuf::Reset() {
  pData_.clear();
}

uint8_t DataBuf::ReadUInt8(size_t offset) const {
  if (offset >= pData_.size()) {
    IMAGE_LOGE("Overflow in DataBuf::read_uint8");
    return 0;
    // throw std::out_of_range("Overflow in DataBuf::read_uint8");
  }
  return pData_[offset];
}

void DataBuf::WriteUInt8(size_t offset, uint8_t x) {
  if (offset >= pData_.size()) {
    IMAGE_LOGE("Overflow in DataBuf::write_uint8");
    return;
    // throw std::out_of_range("Overflow in DataBuf::write_uint8");
  }
  pData_[offset] = x;
}

int DataBuf::CmpBytes(size_t offset, const void* buf, size_t bufsize) const {
  if (pData_.size() < bufsize || offset > pData_.size() - bufsize) {
    IMAGE_LOGE("Overflow in DataBuf::cmpBytes");
    return 0;
    // throw std::out_of_range("Overflow in DataBuf::cmpBytes");
  }
  return memcmp(&pData_[offset], buf, bufsize);
}

byte* DataBuf::Data(size_t offset) {
  return const_cast<byte*>(C_Data(offset));
}

const byte* DataBuf::C_Data(size_t offset) const {
  if (pData_.empty() || offset == pData_.size()) {
    return nullptr;
  }
  if (offset > pData_.size()) {
    IMAGE_LOGE("Overflow in DataBuf::c_data");
    return nullptr;
    // throw std::out_of_range("Overflow in DataBuf::c_data");
  }
  return &pData_[offset];
}

uint16_t getUShort(const byte* buf, ByteOrder byteOrder) {
  return getUShort(makeSliceUntil(buf, 2), byteOrder);
}

size_t us2Data(byte* buf, uint16_t s, ByteOrder byteOrder) {
  if (byteOrder == littleEndian) {
    buf[0] = static_cast<byte>(s & 0x00ffU);
    buf[1] = static_cast<byte>((s & 0xff00U) >> 8);
  } else {
    buf[0] = static_cast<byte>((s & 0xff00U) >> 8);
    buf[1] = static_cast<byte>(s & 0x00ffU);
  }
  return 2;
}

size_t s2Data(byte* buf, int16_t s, ByteOrder byteOrder) {
  if (byteOrder == littleEndian) {
    buf[0] = static_cast<byte>(s & 0x00ffU);
    buf[1] = static_cast<byte>((s & 0xff00U) >> 8);
  } else {
    buf[0] = static_cast<byte>((s & 0xff00U) >> 8);
    buf[1] = static_cast<byte>(s & 0x00ffU);
  }
  return 2;
}

}
}  // namespace Exiv2
