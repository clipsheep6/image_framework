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

enum SeekPos { BEGIN, CURRENT, END };

typedef uint8_t byte;

class ImageStream {
public:
    virtual ~ImageStream() {}

    // Write data to the image stream
    // data: The data to be written
    // size: The size of the data to be written
    // Return: The actual size of the data written
    virtual ssize_t Write(uint8_t* data, size_t size) = 0;

    // Write data from another image stream
    // src: The image stream where the data comes from
    // Return: The actual size of the data written
    virtual ssize_t Write(ImageStream& src) = 0;

    // Read data from the image stream
    // buf: The buffer to store the data read
    // size: The size of the data to be read
    // Return: The actual size of the data read
    virtual ssize_t Read(uint8_t* buf, size_t size) = 0;

    // Seek a specific position in the image stream
    // offset: The offset
    // pos: The starting position of the offset (from the head, current position, or tail)
    // Return: The new position
    virtual int Seek(int offset, SeekPos pos) = 0;

    // Get the current position in the image stream
    // Return: The current position
    virtual ssize_t Tell() = 0;

    // Check if the end of the image stream has been reached
    // Return: true if the end has been reached, false otherwise
    virtual bool IsEof() = 0;

    // Check if the image stream is open
    // Return: true if it is open, false otherwise
    virtual bool IsOpen() = 0;

    // Close the image stream
    virtual void Close() = 0;

    // Open the image stream
    // Return: true if it opens successfully, false otherwise
    virtual bool Open() = 0;

    // Create a memory map
    // isWriteable: If true, the created memory map will be writable; otherwise, the created memory map will be read-only.
    // Return: If the memory map is created successfully, return a pointer to the memory map; otherwise, return nullptr.
    virtual byte* MMap(bool isWriteable =false) =0;

    // Transfer the content of the source ImageStream to the current ImageStream.
    // src: The source ImageStream, this function will read data from this ImageStream.
    virtual void Transfer(ImageStream& src) = 0;

    // Get the size of the ImageStream
    // Return: The size of the ImageStream
    virtual size_t GetSize() = 0;
};

} // namespace MultimediaPlugin
} // namespace OHOS
#endif // IMAGE_STREAM_H
