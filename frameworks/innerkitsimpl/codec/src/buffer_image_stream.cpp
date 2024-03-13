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
#include <errno.h>
#include <fcntl.h>
#include <string>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

// Project includes
#include "buffer_image_stream.h"
#include "image_log.h"
#include "image_stream.h"

#undef LOG_DOMAIN
#define LOG_DOMAIN LOG_TAG_DOMAIN_ID_IMAGE

#undef LOG_TAG
#define LOG_TAG "BufferImageStream"

namespace OHOS {
namespace Media {

BufferImageStream::BufferImageStream()
{
    buffer_ = nullptr;
    capacity_ = 0;
    bufferSize_ = 0;
    currentOffset_ = 0;
    memoryMode_ = Dynamic;
    originData_ = nullptr;
}

BufferImageStream::~BufferImageStream() { Close(); }

BufferImageStream::BufferImageStream(byte *originData, size_t size,
                                     MemoryMode mode)
{
    buffer_ = originData;
    this->originData_ = originData;
    capacity_ = size;
    bufferSize_ = size;
    currentOffset_ = 0;
    memoryMode_ = mode;
}

ssize_t BufferImageStream::Write(byte *data, size_t size)
{
    // Check if the new data will fit into the current buffer
    if (currentOffset_ + static_cast<long>(size) > capacity_) {
        // If the memory mode is Fix, we cannot expand the buffer
        if (memoryMode_ == Fix) {
            IMAGE_LOGE("BufferImageStream::Write failed, currentOffset:%ld, "
                       "size:%u, capacity:%ld",
                       currentOffset_, size, capacity_);
            return -1;
        }
        // Calculate the new capacity, ensuring it is a multiple of
        // BUFFER_IMAGE_STREAM_PAGE_SIZE
        long newCapacity =
            ((currentOffset_ + size + BUFFER_IMAGE_STREAM_PAGE_SIZE - 1) /
             BUFFER_IMAGE_STREAM_PAGE_SIZE) *
            BUFFER_IMAGE_STREAM_PAGE_SIZE;
        // Allocate the new buffer
        byte *newBuffer = new byte[newCapacity];
        // Check if the allocation was successful
        if (newBuffer == nullptr) {
            IMAGE_LOGE("BufferImageStream::Write failed, newBuffer is nullptr");
            return -1;
        }
        // Initialize the new memory
        std::fill_n(newBuffer, newCapacity, 0);
        // If there is existing data, copy it to the new buffer
        if (buffer_ != nullptr) {
            IMAGE_LOGD(
                "BufferImageStream::Write, before memcpy1, currentOffset:%ld, "
                "size:%u, capacity:%ld, newCapacity:%ld",
                currentOffset_, size, capacity_, newCapacity);
            memcpy(newBuffer, buffer_, capacity_);
            IMAGE_LOGD(
                "BufferImageStream::Write, after memcpy1, currentOffset:%ld, "
                "size:%u, capacity:%ld, newCapacity:%ld",
                currentOffset_, size, capacity_, newCapacity);
            // If the old buffer was not externally allocated, delete it
            if (originData_ != buffer_) {
                delete[] buffer_;
            }
        }
        // Update the buffer and capacity
        buffer_ = newBuffer;
        capacity_ = newCapacity;
    }
    // Copy the new data into the buffer
    IMAGE_LOGD("BufferImageStream::Write, before memcpy2, currentOffset:%ld, "
               "size:%u, capacity:%ld",
               currentOffset_, size, capacity_);
    memmove(buffer_ + currentOffset_, data, size);
    IMAGE_LOGD("BufferImageStream::Write, after memcpy2, currentOffset:%ld, "
               "size:%u, capacity:%ld",
               currentOffset_, size, capacity_);
    // Update the current offset and buffer size
    currentOffset_ += size;
    bufferSize_ = std::max(currentOffset_, bufferSize_);
    // Return the number of bytes written
    return size;
}

ssize_t BufferImageStream::Write(ImageStream &src)
{
    byte buffer[BUFFER_IMAGE_STREAM_PAGE_SIZE];
    size_t totalBytesWritten = 0;

    while (!src.IsEof()) {
        size_t bytesRead = src.Read(buffer, sizeof(buffer));
        if (bytesRead == 0) {
            break;
        }

        size_t bytesWritten = Write(buffer, bytesRead);
        totalBytesWritten += bytesWritten;
    }
    bufferSize_ = src.GetSize();

    return totalBytesWritten;
}

ssize_t BufferImageStream::Read(byte *buf, size_t size)
{
    if (currentOffset_ >= bufferSize_) {
        IMAGE_LOGE("BufferImageStream::Read failed, currentOffset:%{public}ld, "
                   "bufferSize:%{public}ld",
                   currentOffset_, bufferSize_);
        return -1;
    }

    long bytesToRead =
        std::min(static_cast<long>(size), bufferSize_ - currentOffset_);
    memcpy(buf, buffer_ + currentOffset_, bytesToRead);
    currentOffset_ += bytesToRead;
    return bytesToRead;
}

int BufferImageStream::ReadByte()
{
    if (currentOffset_ >= bufferSize_) {
        IMAGE_LOGE("BufferImageStream::ReadByte failed, "
                   "currentOffset:%{public}ld, bufferSize:%{public}ld",
                   currentOffset_, bufferSize_);
        return -1;
    }

    if (currentOffset_ < bufferSize_) {
        return buffer_[currentOffset_++];
    }
    return -1;
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
        currentOffset_ = bufferSize_ + offset;
        break;
    default:
        return -1;
    }

    if (currentOffset_ > bufferSize_) {
        currentOffset_ = bufferSize_;
    }

    return currentOffset_;
}

long BufferImageStream::Tell() { return currentOffset_; }

bool BufferImageStream::IsEof() { return currentOffset_ >= bufferSize_; }

bool BufferImageStream::IsOpen() { return true; }

void BufferImageStream::Close()
{
    if (memoryMode_ == Dynamic && buffer_ != originData_ &&
        buffer_ != nullptr) {
        delete[] buffer_;
        buffer_ = nullptr;
    }
    currentOffset_ = 0;
}

bool BufferImageStream::Open() { return true; }

bool BufferImageStream::Open(OpenMode mode) { return true; }

bool BufferImageStream::Flush() { return true; }

byte *BufferImageStream::GetAddr(bool isWriteable) { return buffer_; }

bool BufferImageStream::CopyFrom(ImageStream &src)
{
    IMAGE_LOGD("BufferImageStream::CopyFrom come in");
    if (!src.IsOpen()) {
        IMAGE_LOGE("BufferImageStream::CopyFrom failed, src is not open");
        return false;
    }
    if (memoryMode_ == Fix) {
        if (src.GetSize() > static_cast<size_t>(capacity_)) {
            // 固定内存，且超长就不拷贝了，拷了数据也不完整
            IMAGE_LOGE("BufferImageStream::CopyFrom failed, src "
                       "size:%{public}zu, capacity:%{public}ld",
                       src.GetSize(), capacity_);
            return false;
        }
    }
    // Clear the current buffer
    if (memoryMode_ == Dynamic) {
        delete[] buffer_;
        size_t estimatedSize =
            ((src.GetSize() + BUFFER_IMAGE_STREAM_PAGE_SIZE - 1) /
             BUFFER_IMAGE_STREAM_PAGE_SIZE) *
            BUFFER_IMAGE_STREAM_PAGE_SIZE; // Ensure it is a multiple of 4k
        buffer_ = new byte[estimatedSize];
        capacity_ = estimatedSize;
    }

    // Pre-allocate memory based on the estimated size
    currentOffset_ = 0;
    bufferSize_ = 0;
    // Read data from the source ImageStream and write it to the current buffer
    size_t buffer_size =
        std::min((size_t)BUFFER_IMAGE_STREAM_PAGE_SIZE, src.GetSize());
    byte tempBuffer[buffer_size];
    src.Seek(0, SeekPos::BEGIN);
    while (!src.IsEof()) {
        size_t bytesRead = src.Read(tempBuffer, sizeof(tempBuffer));
        if (bytesRead > 0) {
            IMAGE_LOGD("BufferImageStream::CopyFrom, bytesRead:%{public}zu",
                       bytesRead);
            if (Write(tempBuffer, bytesRead) == -1) {
                // Write failed, return false, and release the buffer
                IMAGE_LOGE("BufferImageStream::CopyFrom failed, Write failed");
                if (memoryMode_ == Dynamic) {
                    delete[] buffer_;
                    buffer_ = nullptr;
                    capacity_ = 0;
                }
                bufferSize_ = 0;
                currentOffset_ = 0;
                return false;
            }
        }
    }
    return true;
}

size_t BufferImageStream::GetSize() { return bufferSize_; }

byte *BufferImageStream::Release()
{
    byte *ret = buffer_;
    buffer_ = nullptr;
    capacity_ = 0;
    bufferSize_ = 0;
    currentOffset_ = 0;
    return ret;
}

} // namespace Media
} // namespace OHOS
