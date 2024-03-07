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

#include "buffer_image_stream.h"
#include "image_log.h"

#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string>

#undef LOG_DOMAIN
#define LOG_DOMAIN LOG_TAG_DOMAIN_ID_IMAGE

#undef LOG_TAG
#define LOG_TAG "BufferImageStream"

namespace OHOS {
namespace Media {

BufferImageStream::BufferImageStream() : currentOffset(0) {}

BufferImageStream::~BufferImageStream() {}

ssize_t BufferImageStream::Write(uint8_t* data, size_t size) {
    if (buffer.capacity() < currentOffset + size) {
        // Calculate the required memory size, ensuring it is a multiple of 4k
        size_t newCapacity = ((currentOffset + size + 4095) / 4096) * 4096;
        buffer.reserve(newCapacity);
    }

    buffer.insert(buffer.end(), data, data + size);
    currentOffset += size;
    return size;
}

ssize_t BufferImageStream::Write(ImageStream& src) {
    uint8_t buffer[4096];
    size_t totalBytesWritten = 0;

    while (!src.IsEof()) {
        size_t bytesRead = src.Read(buffer, sizeof(buffer));
        if (bytesRead == 0) {
            break;
        }

        size_t bytesWritten = Write(buffer, bytesRead);
        totalBytesWritten += bytesWritten;
    }

    return totalBytesWritten;
}

ssize_t BufferImageStream::Read(uint8_t* buf, size_t size) {
    size_t bytesToRead = std::min(size, buffer.size() - currentOffset);
    std::copy(buffer.begin() + currentOffset, buffer.begin() + currentOffset + bytesToRead, buf);
    currentOffset += bytesToRead;
    return bytesToRead;
}

int BufferImageStream::Seek(int offset, SeekPos pos) {
    switch (pos) {
        case SeekPos::BEGIN:
            currentOffset = offset;
            break;
        case SeekPos::CURRENT:
            currentOffset += offset;
            break;
        case SeekPos::END:
            currentOffset = buffer.size() + offset;
            break;
        default:
            return -1;
    }

    if (currentOffset > buffer.size()) {
        currentOffset = buffer.size();
    }

    return currentOffset;
}

ssize_t BufferImageStream::Tell() {
    return currentOffset;
}

bool BufferImageStream::IsEof() {
    return currentOffset >= buffer.size();
}

bool BufferImageStream::IsOpen() {
    return true;
}

void BufferImageStream::Close() {
    buffer.clear();
    currentOffset = 0;
}

bool BufferImageStream::Open() {
    return true;
}

byte* BufferImageStream::MMap(bool isWriteable) {
    return buffer.data();
}

void BufferImageStream::Transfer(ImageStream& src) {
    // Clear the current buffer
    buffer.clear();
    currentOffset = 0;

    // Pre-allocate memory based on the estimated size
    size_t estimatedSize = src.GetSize();
    buffer.reserve(estimatedSize);

    // Read data from the source ImageStream and write it to the current buffer
    uint8_t tempBuffer[4096];
    while (!src.IsEof()) {
        size_t bytesRead = src.Read(tempBuffer, sizeof(tempBuffer));
        if (bytesRead > 0) {
            buffer.insert(buffer.end(), tempBuffer, tempBuffer + bytesRead);
        }
    }
}

size_t BufferImageStream::GetSize() {
    return buffer.size();
}

} // namespace Media
} // namespace OHOS
