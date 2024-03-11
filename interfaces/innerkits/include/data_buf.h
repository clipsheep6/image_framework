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

// included header files
#include "data_buf_slice.hpp"

// standard includes
#include <algorithm>
#include <cstdint>
#include <limits>
#include <sstream>
#include <vector>

// *****************************************************************************
// namespace extensions
namespace OHOS {
namespace Media {
// *****************************************************************************
// type definitions

//! 1 byte unsigned integer type.
using byte = uint8_t;

//! 8 byte unsigned rational type.
using URational = std::pair<uint32_t, uint32_t>;
//! 8 byte signed rational type.
using Rational = std::pair<int32_t, int32_t>;

//! Type to express the byte order (little or big endian)
enum ByteOrder {
  invalidByteOrder,
  littleEndian,
  bigEndian,
};

//! Container for binary data
using Blob = std::vector<byte>;

/*!
  @brief Utility class containing a character array. All it does is to take
         care of memory allocation and deletion. Its primary use is meant to
         be as a stack variable in functions that need a temporary data
         buffer.
 */
struct DataBuf {
  //! @name Creators
  //@{
  //! Default constructor
  DataBuf() = default;
  //! Constructor with an initial buffer size
  explicit DataBuf(size_t size);
  //! Constructor, copies an existing buffer
  DataBuf(const byte* pData, size_t size);
  //@}

  //! @name Manipulators
  //@{
  /*!
    @brief Allocate a data buffer of at least the given size. Note that if
           the requested \em size is less than the current buffer size, no
           new memory is allocated and the buffer size doesn't change.
   */
  void Alloc(size_t size);
  /*!
    @brief Resize the buffer. Existing data is preserved (like std::realloc()).
   */
  void Resize(size_t size);

  //! Reset value
  void Reset();
  //@}

  using iterator = std::vector<byte>::iterator;
  using const_iterator = std::vector<byte>::const_iterator;

  iterator begin() noexcept {
    return pData_.begin();
  }
  [[nodiscard]] const_iterator cbegin() const noexcept {
    return pData_.cbegin();
  }
  iterator end() noexcept {
    return pData_.end();
  }
  [[nodiscard]] const_iterator cend() const noexcept {
    return pData_.end();
  }

  [[nodiscard]] size_t size() const {
    return pData_.size();
  }

  [[nodiscard]] uint8_t ReadUInt8(size_t offset) const;
  void WriteUInt8(size_t offset, uint8_t x);

  //! Equivalent to: memcmp(&pData_[offset], buf, bufsize)
  int CmpBytes(size_t offset, const void* buf, size_t bufsize) const;

  //! Returns a data pointer.
  [[nodiscard]] byte* Data(size_t offset = 0);

  //! Returns a (read-only) data pointer.
  [[nodiscard]] const byte* C_Data(size_t offset = 0) const;

  [[nodiscard]] bool empty() const {
    return pData_.empty();
  }

 private:
  std::vector<byte> pData_;
};

// *****************************************************************************
// free functions

//! Read a 2 byte unsigned short value from the data buffer
 uint16_t getUShort(const byte* buf, ByteOrder byteOrder);
//! Read a 2 byte unsigned short value from a Slice
template <typename T>
uint16_t getUShort(const Slice<T>& buf, ByteOrder byteOrder) {
  if (byteOrder == littleEndian) {
    return static_cast<byte>(buf.at(1)) << 8 | static_cast<byte>(buf.at(0));
  }
  return static_cast<byte>(buf.at(0)) << 8 | static_cast<byte>(buf.at(1));
}


/*!
  @brief Convert an unsigned short to data, write the data to the buffer,
         return number of bytes written.
 */
 size_t us2Data(byte* buf, uint16_t s, ByteOrder byteOrder);
/*!
  @brief Convert a signed short to data, write the data to the buffer,
         return number of bytes written.
 */
 size_t s2Data(byte* buf, int16_t s, ByteOrder byteOrder);


}
}

#endif  // #ifndef INTERFACES_INNERKITS_INCLUDE_DATA_BUF
