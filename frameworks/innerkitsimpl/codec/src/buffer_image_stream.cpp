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

BufferImageStream::BufferImageStream() {
    buffer = nullptr;
    capacity = 0;
    bufferSize = 0;
    currentOffset = 0;
    memoryMode = Dynamic;
}

BufferImageStream::~BufferImageStream() {}

BufferImageStream::BufferImageStream(uint8_t *originData, size_t size){
    buffer = originData;
    capacity = size;
    bufferSize = size;
    currentOffset = 0;
    memoryMode = Fix;
}

ssize_t BufferImageStream::Write(uint8_t* data, size_t size) {
    if(currentOffset + static_cast<long>(size) > capacity){
        if(memoryMode == Fix){
            IMAGE_LOGE("BufferImageStream::Write failed, currentOffset:%{public}ld, size:%{public}u, capacity:%{public}ld", currentOffset, size, capacity);
            return -1;
        }
        long newCapacity = ((currentOffset + size + 4095) / 4096) * 4096; // Ensure it is a multiple of 4k
        uint8_t *newBuffer = new uint8_t[newCapacity];
        if(newBuffer == nullptr){
            IMAGE_LOGE("BufferImageStream::Write failed, newBuffer is nullptr");
            return -1;
        }
        if(buffer != nullptr){
            memcpy(newBuffer, buffer, capacity);
            delete[] buffer;
        }
        buffer = newBuffer;
        capacity = newCapacity;
    }
    memcpy(buffer + currentOffset, data, size);
    currentOffset += size;
    bufferSize = std::max(currentOffset, bufferSize);
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
    bufferSize = src.GetSize();

    return totalBytesWritten;
}

ssize_t BufferImageStream::Read(uint8_t* buf, size_t size) {
    if (currentOffset >= bufferSize) {
        IMAGE_LOGE("BufferImageStream::Read failed, currentOffset:%{public}ld, bufferSize:%{public}ld", currentOffset, bufferSize);
        return -1;
    }

    long bytesToRead = std::min(static_cast<long>(size), bufferSize - currentOffset);
    memcpy(buf, buffer + currentOffset, bytesToRead);
    currentOffset += bytesToRead;
    return bytesToRead;
}

int BufferImageStream::ReadByte(){
    if (currentOffset >= bufferSize) {
        IMAGE_LOGE("BufferImageStream::ReadByte failed, currentOffset:%{public}ld, bufferSize:%{public}ld", currentOffset, bufferSize);
        return -1;
    }

    if (currentOffset < bufferSize) {
        return buffer[currentOffset++];
    }
    return -1;
}

long BufferImageStream::Seek(int offset, SeekPos pos) {
    switch (pos) {
        case SeekPos::BEGIN:
            currentOffset = offset;
            break;
        case SeekPos::CURRENT:
            currentOffset += offset;
            break;
        case SeekPos::END:
            currentOffset = bufferSize + offset;
            break;
        default:
            return -1;
    }

    if (currentOffset > bufferSize) {
        currentOffset = bufferSize;
    }

    return currentOffset;
}

long BufferImageStream::Tell() {
    return currentOffset;
}

bool BufferImageStream::IsEof() {
    return currentOffset >= bufferSize;
}

bool BufferImageStream::IsOpen() {
    return true;
}

void BufferImageStream::Close() {
    if(memoryMode == Dynamic){
        delete[] buffer;
    }
    currentOffset = 0;
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
    return buffer;
}

bool BufferImageStream::MUnmap(byte* mmap) {
    return true;
}

void BufferImageStream::CopyFrom(ImageStream& src) {
    // Clear the current buffer
    if(memoryMode == Dynamic){
        delete[] buffer;
    }
    if(memoryMode == Fix){
        if(src.GetSize() > static_cast<size_t>(capacity)){
            //固定内存，且超长就不拷贝了，拷了数据也不完整
            IMAGE_LOGE("BufferImageStream::CopyFrom failed, src size:%{public}zu, capacity:%{public}ld", src.GetSize(), capacity);
            return;
        }
    }

    // Pre-allocate memory based on the estimated size
    size_t estimatedSize = ((src.GetSize() + 4095) / 4096) * 4096; // Ensure it is a multiple of 4k
    buffer = new uint8_t[estimatedSize];
    currentOffset = 0;
    bufferSize = 0;
    capacity = estimatedSize;

    // Read data from the source ImageStream and write it to the current buffer
    uint8_t tempBuffer[4096];
    while (!src.IsEof()) {
        size_t bytesRead = src.Read(tempBuffer, sizeof(tempBuffer));
        if (bytesRead > 0) {
            Write(tempBuffer, bytesRead);
        }
    }
}

size_t BufferImageStream::GetSize() {
    return bufferSize;
}

} // namespace Media
} // namespace OHOS
