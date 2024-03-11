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

#ifndef INTERFACES_INNERKITS_INCLUDE_DATA_BUF
#define INTERFACES_INNERKITS_INCLUDE_DATA_BUF

// standard includes
#include <algorithm>
#include <cstdint>
#include <limits>
#include <sstream>
#include <vector>

// included header files
#include "data_buf_slice.hpp"

namespace OHOS {
namespace Media {

using byte = uint8_t;
using URational = std::pair<uint32_t, uint32_t>;
using Rational = std::pair<int32_t, int32_t>;
#define DATA_BUF_BYTE_SIZE 8

// Type to express the byte order (little or big endian)
enum ByteOrder {
  invalidByteOrder,
  littleEndian,
  bigEndian,
};

// Container for binary data
using Blob = std::vector<byte>;

/*
  @brief Utility class containing a character array. All it does is to take
         care of memory allocation and deletion. Its primary use is meant to
         be as a stack variable in functions that need a temporary data
         buffer.
 */
struct DataBuf {
  using ByteVector = std::vector<byte>;
  using iterator = ByteVector::iterator;
  using const_iterator = ByteVector::const_iterator;

  /**
   * Default constructor
   */
  DataBuf() = default;

  /**
   * Constructor to create a buffer with a specific size
   * @param size The size of the buffer
   */
  explicit DataBuf(size_t size);

  /**
   * Constructor to create a buffer from a byte array
   * @param pData Pointer to the byte array
   * @param size Size of the byte array
   */
  DataBuf(const byte* pData, size_t size);

  /**
   * Allocate memory for the buffer
   * @param size The size of the memory to allocate
   */
  void Alloc(size_t size);

  /**
   * Resize the buffer
   * @param size The new size of the buffer
   */
  void Resize(size_t size);

  /**
   * Reset the buffer
   */
  void Reset();

  /**
   * Get an iterator pointing to the beginning of the buffer
   * @return An iterator pointing to the beginning of the buffer
   */
  iterator begin() noexcept{
    return pData_.begin();
  }

  /**
   * Get a const iterator pointing to the beginning of the buffer
   * @return A const iterator pointing to the beginning of the buffer
   */
  const_iterator cbegin() const noexcept;

  /**
   * Get an iterator pointing to the end of the buffer
   * @return An iterator pointing to the end of the buffer
   */
  iterator end() noexcept;

  /**
   * Get a const iterator pointing to the end of the buffer
   * @return A const iterator pointing to the end of the buffer
   */
  const_iterator cend() const noexcept;

  /**
   * Get the size of the buffer
   * @return The size of the buffer
   */
  size_t size() const {
    return pData_.size();
  }

  /**
   * Read a uint8_t value from the buffer at a specific offset
   * @param offset The offset to read from
   * @return The uint8_t value at the offset
   */
  uint8_t ReadUInt8(size_t offset) const;

  /**
   * Write a uint8_t value to the buffer at a specific offset
   * @param offset The offset to write to
   * @param value The value to write
   */
  void WriteUInt8(size_t offset, uint8_t value);

  /**
   * Compare a sequence of bytes in the buffer with another sequence of bytes
   * @param offset The offset in the buffer to start the comparison
   * @param buf The sequence of bytes to compare with
   * @param bufsize The size of the sequence of bytes
   * @return 0 if the sequences are equal, a positive value if the sequence in the buffer is greater, a negative value otherwise
   */
  int CmpBytes(size_t offset, const void* buf, size_t bufsize) const;

  /**
   * Get a pointer to the data in the buffer at a specific offset
   * @param offset The offset to get the data from
   * @return A pointer to the data
   */
  byte* Data(size_t offset = 0);

  /**
   * Get a const pointer to the data in the buffer at a specific offset
   * @param offset The offset to get the data from
   * @return A const pointer to the data
   */
  const byte* C_Data(size_t offset = 0) const;

  /**
   * Check if the buffer is empty
   * @return true if the buffer is empty, false otherwise
   */
  bool empty() const;

 private:
  ByteVector pData_;
};

//! Read a 2 byte unsigned short value from the data buffer
 uint16_t getUShort(const byte* buf, ByteOrder byteOrder);
//! Read a 2 byte unsigned short value from a Slice
template <typename T>
uint16_t getUShort(const Slice<T>& buf, ByteOrder byteOrder) {
  if (byteOrder == littleEndian) {
    return static_cast<byte>(buf.at(1)) << DATA_BUF_BYTE_SIZE | static_cast<byte>(buf.at(0));
  }
  return static_cast<byte>(buf.at(0)) << DATA_BUF_BYTE_SIZE | static_cast<byte>(buf.at(1));
}


/*!
  @brief Convert an unsigned short to data, write the data to the buffer,
         return number of bytes written.
 */
 size_t us2Data(byte* buf, uint16_t value, ByteOrder byteOrder);
/*!
  @brief Convert a signed short to data, write the data to the buffer,
         return number of bytes written.
 */
 size_t s2Data(byte* buf, int16_t value, ByteOrder byteOrder);


}
}

#endif  // #ifndef INTERFACES_INNERKITS_INCLUDE_DATA_BUF
