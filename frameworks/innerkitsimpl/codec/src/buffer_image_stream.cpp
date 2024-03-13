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
#include "image_stream.h"

#include <errno.h>
#include <fcntl.h>
#include <string>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#undef LOG_DOMAIN
#define LOG_DOMAIN LOG_TAG_DOMAIN_ID_IMAGE

#undef LOG_TAG
#define LOG_TAG "BufferImageStream"

namespace OHOS {
namespace Media {

BufferImageStream::BufferImageStream() : currentOffset_(0) {}

BufferImageStream::~BufferImageStream() { Close(); }

ssize_t BufferImageStream::Write(uint8_t *data, size_t size)
{
    if (buffer_.capacity() < currentOffset_ + size) {
        // Calculate the required memory size, ensuring it is a multiple of 4k
        size_t newCapacity =
            ((currentOffset_ + size + IMAGE_STREAM_PAGE_SIZE - 1) /
             IMAGE_STREAM_PAGE_SIZE) *
            IMAGE_STREAM_PAGE_SIZE;
        buffer_.reserve(newCapacity);
    }

    buffer_.insert(buffer_.end(), data, data + size);
    currentOffset_ += size;
    return size;
}

ssize_t BufferImageStream::Write(ImageStream &src)
{
    // Determine the size of the buffer
    size_t bufferSize = std::min<size_t>(src.GetSize(), IMAGE_STREAM_PAGE_SIZE);
    std::vector<uint8_t> buffer(bufferSize);
    size_t totalBytesWritten = 0;

    while (!src.IsEof()) {
        size_t bytesRead = src.Read(buffer.data(), buffer.size());
        if (bytesRead == 0) {
            break;
        }

        size_t bytesWritten = Write(buffer.data(), bytesRead);
        totalBytesWritten += bytesWritten;
    }

    return totalBytesWritten;
}

ssize_t BufferImageStream::Read(uint8_t *buf, size_t size)
{
    size_t bytesToRead =
        std::min(size, buffer_.size() - static_cast<size_t>(currentOffset_));
    std::copy(buffer_.begin() + currentOffset_,
              buffer_.begin() + currentOffset_ + bytesToRead, buf);
    currentOffset_ += bytesToRead;
    return bytesToRead;
}

int BufferImageStream::ReadByte()
{
    if (static_cast<size_t>(currentOffset_) >= buffer_.size()) {
        return -1;
    }

    return buffer_[currentOffset_++];
}

long BufferImageStream::Seek(int offset, SeekPos pos)
{
    switch (pos) {
    case SeekPos::BEGIN:
        currentOffset_ = offset;
        break;
    case SeekPos::CURRENT:
        currentOffset_ += offset;
        break;
    case SeekPos::END:
        currentOffset_ = buffer_.size() + offset;
        break;
    default:
        return -1;
    }

    if (static_cast<size_t>(currentOffset_) > buffer_.size()) {
        currentOffset_ = buffer_.size();
    }

    return currentOffset_;
}

long BufferImageStream::Tell() { return currentOffset_; }

bool BufferImageStream::IsEof()
{
    return static_cast<size_t>(currentOffset_) >= buffer_.size();
}

bool BufferImageStream::IsOpen() { return true; }

void BufferImageStream::Close()
{
    buffer_.clear();
    currentOffset_ = 0;
}

bool BufferImageStream::Open() { return true; }

bool BufferImageStream::Open(OpenMode mode) { return true; }

bool BufferImageStream::Flush() { return true; }

byte *BufferImageStream::GetAddr(bool isWriteable) { return buffer_.data(); }

bool BufferImageStream::CopyFrom(ImageStream &src)
{
    // Clear the current buffer
    buffer_.clear();
    currentOffset_ = 0;

    // Pre-allocate memory based on the estimated size
    size_t estimatedSize = src.GetSize();
    // Adjust estimatedSize to be a multiple of 4096
    estimatedSize = ((estimatedSize + IMAGE_STREAM_PAGE_SIZE - 1) /
                     IMAGE_STREAM_PAGE_SIZE) *
                    IMAGE_STREAM_PAGE_SIZE;
    buffer_.reserve(estimatedSize);

    // Determine the size of the tempBuffer
    size_t tempBufferSize =
        std::min<size_t>(estimatedSize, IMAGE_STREAM_PAGE_SIZE);
    std::vector<uint8_t> tempBuffer(tempBufferSize);

    // Read data from the source ImageStream and write it to the current buffer
    src.Seek(0, SeekPos::BEGIN);
    Seek(0, SeekPos::BEGIN);
    while (!src.IsEof()) {
        size_t bytesRead = src.Read(tempBuffer.data(), tempBuffer.size());
        if (bytesRead > 0) {
            buffer_.insert(buffer_.end(), tempBuffer.begin(),
                           tempBuffer.begin() + bytesRead);
        }
    }
    return true;
}

size_t BufferImageStream::GetSize() { return buffer_.size(); }

} // namespace Media
} // namespace OHOS
