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

// Standard library includes
#include <array>
#include <cctype>
#include <climits>
#include <cmath>
#include <cstring>
#include <iomanip>
#include <numeric>
#include <sstream>
#include <utility>

// Project includes
#include "data_buf.h"
#include "image_log.h"

#undef LOG_DOMAIN
#define LOG_DOMAIN LOG_TAG_DOMAIN_ID_IMAGE

#undef LOG_TAG
#define LOG_TAG "DataBuffer"

// *****************************************************************************
// class member definitions
namespace OHOS {
namespace Media {

DataBuf::DataBuf(size_t size) : pData_(size) {}

DataBuf::DataBuf(const byte *pData, size_t size) : pData_(size)
{
    std::copy_n(pData, size, pData_.begin());
}

void DataBuf::Alloc(size_t size) { pData_.resize(size); }

void DataBuf::Resize(size_t size) { pData_.resize(size); }

void DataBuf::Reset() { pData_.clear(); }

uint8_t DataBuf::ReadUInt8(size_t offset) const
{
    if (offset >= pData_.size()) {
        IMAGE_LOGE("Overflow in DataBuf::read_uint8");
        return 0;
    }
    return pData_[offset];
}

void DataBuf::WriteUInt8(size_t offset, uint8_t value)
{
    if (offset >= pData_.size()) {
        IMAGE_LOGE("Overflow in DataBuf::write_uint8");
        return;
    }
    pData_[offset] = value;
}

uint32_t getULong(const byte *buf, ByteOrder byteOrder)
{
    if (byteOrder == littleEndian) {
        return buf[3] << 24 | buf[2] << 16 | buf[1] << 8 | buf[0];
    }
    return buf[0] << 24 | buf[1] << 16 | buf[2] << 8 | buf[3];
}

void DataBuf::WriteUInt32(size_t offset, uint32_t x, ByteOrder byteOrder)
{
    if (pData_.size() < 4 || offset > (pData_.size() - 4)) {
        IMAGE_LOGE("Overflow in DataBuf::write_uint32");
        return;
        // throw std::out_of_range("Overflow in DataBuf::write_uint32");
    }
    ul2Data(&pData_[offset], x, byteOrder);
}

size_t ul2Data(byte *buf, uint32_t l, ByteOrder byteOrder)
{
    if (byteOrder == littleEndian) {
        buf[0] = static_cast<byte>(l & 0x000000ffU);
        buf[1] = static_cast<byte>((l & 0x0000ff00U) >> 8);
        buf[2] = static_cast<byte>((l & 0x00ff0000U) >> 16);
        buf[3] = static_cast<byte>((l & 0xff000000U) >> 24);
    } else {
        buf[0] = static_cast<byte>((l & 0xff000000U) >> 24);
        buf[1] = static_cast<byte>((l & 0x00ff0000U) >> 16);
        buf[2] = static_cast<byte>((l & 0x0000ff00U) >> 8);
        buf[3] = static_cast<byte>(l & 0x000000ffU);
    }
    return 4;
}

uint32_t DataBuf::ReadUInt32(size_t offset, ByteOrder byteOrder)
{
    if (pData_.size() < 4 || offset > (pData_.size() - 4)) {
        IMAGE_LOGE("Overflow in DataBuf::read_uint32");
        return 0;
        // throw std::out_of_range(s"Overflow in DataBuf::read_uint32");
    }
    return getULong(&pData_[offset], byteOrder);
}

int DataBuf::CmpBytes(size_t offset, const void *buf, size_t bufsize) const
{
    if (pData_.size() < bufsize || offset > pData_.size() - bufsize) {
        IMAGE_LOGE("Overflow in DataBuf::cmpBytes");
        return -1;
    }
    return memcmp(&pData_[offset], buf, bufsize);
}

byte *DataBuf::Data(size_t offset)
{
    return const_cast<byte *>(C_Data(offset));
}

const byte *DataBuf::C_Data(size_t offset) const
{
    if (pData_.empty() || offset == pData_.size()) {
        return nullptr;
    }
    if (offset > pData_.size()) {
        IMAGE_LOGE("Overflow in DataBuf::c_data");
        return nullptr;
    }
    return &pData_[offset];
}

uint16_t getUShort(const byte *buf, ByteOrder byteOrder)
{
    return getUShort(makeSliceUntil(buf, 2), byteOrder);
}

const uint16_t LOWER_BYTE_MASK = 0x00ffU;
const uint16_t UPPER_BYTE_MASK = 0xff00U;

size_t us2Data(byte *buf, uint16_t value, ByteOrder byteOrder)
{
    if (byteOrder == littleEndian) {
        buf[0] = static_cast<byte>(value & LOWER_BYTE_MASK);
        buf[1] =
            static_cast<byte>((value & UPPER_BYTE_MASK) >> DATA_BUF_BYTE_SIZE);
    } else {
        buf[0] =
            static_cast<byte>((value & UPPER_BYTE_MASK) >> DATA_BUF_BYTE_SIZE);
        buf[1] = static_cast<byte>(value & LOWER_BYTE_MASK);
    }
    return 2;
}

size_t s2Data(byte *buf, int16_t value, ByteOrder byteOrder)
{
    if (byteOrder == littleEndian) {
        buf[0] = static_cast<byte>(value & LOWER_BYTE_MASK);
        buf[1] =
            static_cast<byte>((value & UPPER_BYTE_MASK) >> DATA_BUF_BYTE_SIZE);
    } else {
        buf[0] =
            static_cast<byte>((value & UPPER_BYTE_MASK) >> DATA_BUF_BYTE_SIZE);
        buf[1] = static_cast<byte>(value & LOWER_BYTE_MASK);
    }
    return 2;
}

} // namespace Media
} // namespace OHOS
