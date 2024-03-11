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

BufferImageStream::BufferImageStream() {
    buffer_ = nullptr;
    capacity_ = 0;
    bufferSize_ = 0;
    currentOffset_ = 0;
    memoryMode_ = Dynamic;
    originData_ = nullptr;
}

BufferImageStream::~BufferImageStream() {}

BufferImageStream::BufferImageStream(byte *originData, size_t size, MemoryMode mode){
    buffer_ = originData;
    this->originData_ = originData;
    capacity_ = size;
    bufferSize_ = size;
    currentOffset_ = 0;
    memoryMode_ = mode;
}

ssize_t BufferImageStream::Write(byte* data, size_t size) {
    if(currentOffset_ + static_cast<long>(size) > capacity_){
        if(memoryMode_ == Fix){
            IMAGE_LOGE("BufferImageStream::Write failed, currentOffset:%{public}ld, size:%{public}u, capacity:%{public}ld", currentOffset_, size, capacity_);
            return -1;
        }
        long newCapacity = ((currentOffset_ + size + BUFFER_IMAGE_STREAM_PAGE_SIZE-1) / BUFFER_IMAGE_STREAM_PAGE_SIZE) * BUFFER_IMAGE_STREAM_PAGE_SIZE; // Ensure it is a multiple of 4k
        byte *newBuffer = new byte[newCapacity];
        if(newBuffer == nullptr){
            IMAGE_LOGE("BufferImageStream::Write failed, newBuffer is nullptr");
            return -1;
        }
        std::fill_n(newBuffer, newCapacity, 0); // Initialize the new memory
        if(buffer_ != nullptr){
            IMAGE_LOGD("BufferImageStream::Write, before memcpy1, currentOffset:%{public}ld, size:%{public}u, capacity:%{public}ld, newCapacity:%{public}ld", currentOffset_, size, capacity_, newCapacity);
            memcpy(newBuffer, buffer_, capacity_);
            IMAGE_LOGD("BufferImageStream::Write, after memcpy1, currentOffset:%{public}ld, size:%{public}u, capacity:%{public}ld, newCapacity:%{public}ld", currentOffset_, size, capacity_, newCapacity);
            if(originData_ != buffer_){
                //不能删外面传入的内存，只能删自己的内存
                delete[] buffer_;
            }
            
        }
        buffer_ = newBuffer;
        capacity_ = newCapacity;
    }
    IMAGE_LOGD("BufferImageStream::Write, before memcpy2, currentOffset:%{public}ld, size:%{public}u, capacity:%{public}ld", currentOffset_, size, capacity_);
    memmove(buffer_ + currentOffset_, data, size); // Use memmove instead of memcpy
    IMAGE_LOGD("BufferImageStream::Write, after memcpy2, currentOffset:%{public}ld, size:%{public}u, capacity:%{public}ld", currentOffset_, size, capacity_);
    currentOffset_ += size;
    bufferSize_ = std::max(currentOffset_, bufferSize_);
    return size;
}

ssize_t BufferImageStream::Write(ImageStream& src) {
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

ssize_t BufferImageStream::Read(byte* buf, size_t size) {
    if (currentOffset_ >= bufferSize_) {
        IMAGE_LOGE("BufferImageStream::Read failed, currentOffset:%{public}ld, bufferSize:%{public}ld", currentOffset_, bufferSize_);
        return -1;
    }

    long bytesToRead = std::min(static_cast<long>(size), bufferSize_ - currentOffset_);
    memcpy(buf, buffer_ + currentOffset_, bytesToRead);
    currentOffset_ += bytesToRead;
    return bytesToRead;
}

int BufferImageStream::ReadByte(){
    if (currentOffset_ >= bufferSize_) {
        IMAGE_LOGE("BufferImageStream::ReadByte failed, currentOffset:%{public}ld, bufferSize:%{public}ld", currentOffset_, bufferSize_);
        return -1;
    }

    if (currentOffset_ < bufferSize_) {
        return buffer_[currentOffset_++];
    }
    return -1;
}

long BufferImageStream::Seek(int offset, SeekPos pos) {
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

long BufferImageStream::Tell() {
    return currentOffset_;
}

bool BufferImageStream::IsEof() {
    return currentOffset_ >= bufferSize_;
}

bool BufferImageStream::IsOpen() {
    return true;
}

void BufferImageStream::Close() {
    if(memoryMode_ == Dynamic && buffer_ != originData_ && buffer_ != nullptr){
        delete[] buffer_;
    }
    currentOffset_ = 0;
}

bool BufferImageStream::Open() {
    return true;
}

bool BufferImageStream::Open(OpenMode mode) {
    return true;
}

bool BufferImageStream::Flush() {
    return true;
}

byte* BufferImageStream::MMap(bool isWriteable) {
    return buffer_;
}

bool BufferImageStream::MUnmap(byte* mmap) {
    return true;
}

bool BufferImageStream::CopyFrom(ImageStream& src) {
    if(!src.IsOpen()){
        IMAGE_LOGE("BufferImageStream::CopyFrom failed, src is not open");
        return false;
    }
    // Clear the current buffer
    if(memoryMode_ == Dynamic){
        delete[] buffer_;
    }
    if(memoryMode_ == Fix){
        if(src.GetSize() > static_cast<size_t>(capacity_)){
            //固定内存，且超长就不拷贝了，拷了数据也不完整
            IMAGE_LOGE("BufferImageStream::CopyFrom failed, src size:%{public}zu, capacity:%{public}ld", src.GetSize(), capacity_);
            return false;
        }
    }

    // Pre-allocate memory based on the estimated size
    size_t estimatedSize = ((src.GetSize() + BUFFER_IMAGE_STREAM_PAGE_SIZE-1) / BUFFER_IMAGE_STREAM_PAGE_SIZE) * BUFFER_IMAGE_STREAM_PAGE_SIZE; // Ensure it is a multiple of 4k
    buffer_ = new byte[estimatedSize];
    currentOffset_ = 0;
    bufferSize_ = 0;
    capacity_ = estimatedSize;

    // Read data from the source ImageStream and write it to the current buffer
    byte tempBuffer[BUFFER_IMAGE_STREAM_PAGE_SIZE];
    while (!src.IsEof()) {
        size_t bytesRead = src.Read(tempBuffer, sizeof(tempBuffer));
        if (bytesRead > 0) {
            Write(tempBuffer, bytesRead);
        }
    }
    return true;
}

size_t BufferImageStream::GetSize() {
    return bufferSize_;
}

byte* BufferImageStream::Release() {
    byte* ret = buffer_;
    buffer_ = nullptr;
    capacity_ = 0;
    bufferSize_ = 0;
    currentOffset_ = 0;
    return ret;
}

} // namespace Media
} // namespace OHOS
