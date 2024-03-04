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

#include "image_stream.h"
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
#define LOG_TAG "ImageStream"

namespace OHOS {
namespace Media {
FileImageStream::FileImageStream(const std::string& filePath)
    : fd(-1), filePath(filePath), fileSize(0), currentOffset(0), mappedMemory(nullptr) {
}

FileImageStream::~FileImageStream() { Close(); }

ssize_t FileImageStream::Write(uint8_t* data, size_t size) {
    if (fd == -1) {
        // 文件没有打开
        return -1;
    }

    ssize_t result = write(fd, data, size);
    if (result == -1) {
        // 写入失败
        char buf[256];        
        strerror_r(errno, buf, sizeof(buf));
        IMAGE_LOGE("Write file failed: %{public}s, reason: %{public}s", filePath.c_str(), buf);
        return -1;
    }

    currentOffset += result;
    return result;
}

ssize_t FileImageStream::Write(ImageStream& src) {
    // 创建一个缓冲区
    uint8_t buffer[4096];
    size_t totalBytesWritten = 0;

    while (!src.IsEof()) {
        // 从源图像流中读取数据
        size_t bytesRead = src.Read(buffer, sizeof(buffer));
        if (bytesRead == 0) {
            break;
        }

        // 将数据写入到当前图像流中
        size_t bytesWritten = Write(buffer, bytesRead);
        if (bytesWritten == static_cast<size_t>(-1)) {
            // 写入失败
            char buf[256];        
            strerror_r(errno, buf, sizeof(buf));
            IMAGE_LOGE("Write file failed: %{public}s, reason: %{public}s", filePath.c_str(), buf);
            return -1;
        }

        totalBytesWritten += bytesWritten;
    }

    return totalBytesWritten;
}

ssize_t FileImageStream::Read(uint8_t* buf, size_t size) {
    if (fd == -1) {
        // 文件没有打开
        return -1;
    }

    ssize_t result = read(fd, buf, size);
    if (result == -1) {
        // 读取失败
        char buf[256];        
        strerror_r(errno, buf, sizeof(buf));
        IMAGE_LOGE("Read file failed: %{public}s, reason: %{public}s", filePath.c_str(), buf);
        return -1;
    }

    currentOffset += result;
    return result;
}

int FileImageStream::Seek(int offset, SeekPos pos) {
    if (fd == -1) {
        // 文件没有打开
        return -1;
    }

    off_t result;
    switch (pos) {
        case SeekPos::BEGIN:
            result = lseek(fd, offset, SEEK_SET);
            break;
        case SeekPos::CURRENT:
            result = lseek(fd, offset, SEEK_CUR);
            break;
        case SeekPos::END:
            result = lseek(fd, offset, SEEK_END);
            break;
        default:
            return -1;
    }

    if (result == -1) {
        // seek失败
        return -1;
    }

    currentOffset = result;
    return currentOffset;
}

ssize_t FileImageStream::Tell() {
    if (fd == -1) {
        // 文件没有打开
        return -1;
    }

    return currentOffset;
}

bool FileImageStream::IsEof() {
    if (fd == -1) {
        // 文件没有打开
        return true;
    }

    return currentOffset >= fileSize;
}

bool FileImageStream::IsOpen() {
    return fd != -1;
}

void FileImageStream::Close() {
    // 如果文件没有打开，直接返回
    if (fd == -1) {
        return;
    }

    // 如果有一个内存映射，删除它
    if (mappedMemory != nullptr) {
        munmap(mappedMemory, fileSize);
        mappedMemory = nullptr;
    }

    // 关闭文件
    close(fd);

    // 重置所有的成员变量
    fd = -1;
    fileSize = 0;
    currentOffset = 0;
    IMAGE_LOGD("File closed: %{public}s", filePath.c_str());
    return;
}

bool FileImageStream::Open() {
    // 如果文件已经打开，先关闭它
    if (fd != -1) {
        IMAGE_LOGD("File already opened, close it first");
        Close();
    }

    // 打开文件，root用户可以忽略只读限制，直接打开这个文件
    fd = open(filePath.c_str(), O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    if (fd == -1) {
        // 打开文件失败
        char buf[256];        
        strerror_r(errno, buf, sizeof(buf));
        IMAGE_LOGE("Open file failed: %{public}s, reason: %{public}s", filePath.c_str(), buf);
        return false;
    }

    // 获取文件大小
    struct stat sb;
    if (fstat(fd, &sb) == -1) {
        // 获取文件大小失败
        char buf[256];        
        strerror_r(errno, buf, sizeof(buf));
        IMAGE_LOGE("Get file size failed: %{public}s, reason: %{public}s", filePath.c_str(), buf);
        close(fd);
        fd = -1;
        return false;
    }
    fileSize = sb.st_size;

    // 重置当前偏移量
    currentOffset = 0;
    IMAGE_LOGD("File opened: %{public}s, size: %{public}zu", filePath.c_str(), fileSize);
    return true;
}

byte* FileImageStream::MMap(bool isWriteable) {
    // 如果文件没有打开，先打开它
    if (fd == -1) {
        if (!Open()) {
            // 打开文件失败
            char buf[256];        
            strerror_r(errno, buf, sizeof(buf));
            IMAGE_LOGE("mmap: Open file failed: %{public}s, reason: %{public}s", filePath.c_str(), buf);
            return nullptr;
        }
    }

    // 如果已经有一个内存映射，先删除它
    if (mappedMemory != nullptr) {
        IMAGE_LOGW("mmap: There is already a memory mapping, remove it first");
        munmap(mappedMemory, fileSize);
    }

    // 创建内存映射
    mappedMemory = static_cast<byte*>(::mmap(nullptr, fileSize, isWriteable ? (PROT_READ | PROT_WRITE) : PROT_READ, MAP_SHARED, fd, 0));
    if (mappedMemory == MAP_FAILED) {
        // 内存映射失败
        char buf[256];        
        strerror_r(errno, buf, sizeof(buf));
        IMAGE_LOGE("mmap: Memory mapping failed: %{public}s, reason: %{public}s", filePath.c_str(), buf);
        mappedMemory = nullptr;
    }
    IMAGE_LOGD("mmap: Memory mapping created: %{public}s, size: %{public}zu", filePath.c_str(), fileSize);
    return mappedMemory;
}

void FileImageStream::Transfer(ImageStream& src) {
    // 如果文件已经打开，先关闭它
    if (IsOpen()) {
        Close();
    }

    // 打开新的文件
    if (!Open()) {
        // 打开文件失败
        char buf[256];        
        strerror_r(errno, buf, sizeof(buf));
        IMAGE_LOGE("transfer: Open file failed: %{public}s, reason: %{public}s", filePath.c_str(), buf);
        return;
    }

    // 从源ImageStream中读取数据并写入到当前的文件中
    uint8_t tempBuffer[4096];
    size_t totalBytesWritten = 0;
    while (!src.IsEof()) {
        size_t bytesRead = src.Read(tempBuffer, sizeof(tempBuffer));
        if (bytesRead > 0) {
            size_t bytesWritten = Write(tempBuffer, bytesRead);
            if (bytesWritten == static_cast<size_t>(-1)) {
                // 写入失败
                char buf[256];        
                strerror_r(errno, buf, sizeof(buf));
                IMAGE_LOGE("transfer: Write file failed: %{public}s, reason: %{public}s", filePath.c_str(), buf);
                return;
            }
            totalBytesWritten += bytesWritten;
        }
    }

    // 截断文件
    if (ftruncate(fd, totalBytesWritten) == -1) {
        // 截断文件失败
        char buf[256];        
        strerror_r(errno, buf, sizeof(buf));
        IMAGE_LOGE("transfer: Truncate file failed: %{public}s, reason: %{public}s", filePath.c_str(), buf);
        return;
    }
}

size_t FileImageStream::GetSize() {
    return fileSize;
}

BufferImageStream::BufferImageStream() : currentOffset(0) {}

BufferImageStream::~BufferImageStream() {}

ssize_t BufferImageStream::Write(uint8_t* data, size_t size) {
    if (buffer.capacity() < currentOffset + size) {
        // 计算需要的内存大小，确保它是4k的整数倍
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
    // 清空当前的缓冲区
    buffer.clear();
    currentOffset = 0;

    // 预先估计需要的内存大小并预分配内存
    size_t estimatedSize = src.GetSize();
    buffer.reserve(estimatedSize);

    // 从源ImageStream中读取数据并写入到当前的缓冲区
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
