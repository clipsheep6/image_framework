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

#ifndef INTERFACES_INNERKITS_INCLUDE_IMAGE_STREAM_H
#define INTERFACES_INNERKITS_INCLUDE_IMAGE_STREAM_H

#include <cstddef>
#include <cstdint>
#include <fstream>
#include <vector>
#include "image_type.h"

namespace OHOS {
namespace Media {

enum SeekPos { 
    BEGIN, 
    CURRENT, 
    END 
};

typedef uint8_t byte;

enum class OpenMode {
    Read,       // Read mode
    ReadWrite   // Read-write mode
};

class ImageStream {
public:
    virtual ~ImageStream() {}

    /**
     * Open the image stream
     * @return true if it opens successfully, false otherwise
     */
    virtual bool Open() = 0;

    /**
     * Open the image stream with a specific mode
     * @param mode The mode to open the image stream
     * @return true if it opens successfully, false otherwise
     */
    virtual bool Open(OpenMode mode) = 0;

    /**
     * Check if the image stream is open
     * @return true if it is open, false otherwise
     */
    virtual bool IsOpen() = 0;

    /**
     * Flush the image stream
     * @return true if it flushes successfully, false otherwise
     */
    virtual bool Flush() = 0;

    /**
     * Write data to the image stream
     * @param data The data to be written
     * @param size The size of the data to be written
     * @return The actual size of the data written
     */
    virtual ssize_t Write(uint8_t* data, size_t size) = 0;

    /**
     * Write data from another image stream
     * @param src The image stream where the data comes from
     * @return The actual size of the data written
     */
    virtual ssize_t Write(ImageStream& src) = 0;

    /**
     * Read data from the image stream
     * @param buf The buffer to store the data read
     * @param size The size of the data to be read
     * @return The actual size of the data read
     */
    virtual ssize_t Read(uint8_t* buf, size_t size) = 0;
    virtual int ReadByte() = 0;

    /**
     * Seek a specific position in the image stream
     * @param offset The offset
     * @param pos The starting position of the offset (from the head, current position, or tail)
     * @return The new position
     */
    virtual int Seek(int offset, SeekPos pos) = 0;

    /**
     * Get the current position in the image stream
     * @return The current position
     */
    virtual long Tell() = 0;

    /**
     * Check if the end of the image stream has been reached
     * @return true if the end has been reached, false otherwise
     */
    virtual bool IsEof() = 0;

    /**
     * Create a memory map
     * @param isWriteable If true, the created memory map will be writable; otherwise, the created memory map will be read-only.
     * @return If the memory map is created successfully, return a pointer to the memory map; otherwise, return nullptr.
     */
    virtual byte* MMap(bool isWriteable = false) = 0;

    /**
     * Release a memory map
     * @param mmap The pointer to the memory map that needs to be released.
     * @return If the memory map is released successfully, return true; otherwise, return false.
     */
    virtual bool MUnmap(byte* mmap) = 0;

    /**
     * Copy the entire content from the source ImageStream to the current ImageStream.
     * @param src The source ImageStream, this function will read data from this ImageStream.
     */
    virtual void CopyFrom(ImageStream& src) = 0;

    /**
     * Get the size of the ImageStream
     * @return The size of the ImageStream
     */
    virtual size_t GetSize() = 0;

private:
    /**
     * Close the image stream
     */
    virtual void Close() = 0;
};

} // namespace MultimediaPlugin
} // namespace OHOS
#endif // INTERFACES_INNERKITS_INCLUDE_IMAGE_STREAM_H
