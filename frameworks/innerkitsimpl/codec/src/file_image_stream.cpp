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

#include "file_image_stream.h"
#include "image_log.h"

#include <cwchar>
#include <memory>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string>

#undef LOG_DOMAIN
#define LOG_DOMAIN LOG_TAG_DOMAIN_ID_IMAGE

#undef LOG_TAG
#define LOG_TAG "FileImageStream"

namespace OHOS {
namespace Media {

int FileWrapper::fstat(int fd, struct stat *st){
    return ::fstat(fd, st);
}

ssize_t FileWrapper::write(int fd, const void* buf, size_t count){
    return ::write(fd, buf, count);
}

ssize_t FileWrapper::read(int fd, void *buf, size_t count){
    return ::read(fd, buf, count);
}

FileImageStream::FileImageStream(const std::string& filePath)
    : fd(-1), filePath(filePath), fileSize(0), currentOffset(0), mappedMemory(nullptr) {
        this->fileWrapper = std::make_unique<FileWrapper>();
}

FileImageStream::FileImageStream(const std::string& filePath, std::unique_ptr<FileWrapper> fileWrapper)
    : fd(-1), filePath(filePath), fileSize(0), currentOffset(0), mappedMemory(nullptr){
    if (fileWrapper == nullptr) {
        this->fileWrapper = std::make_unique<FileWrapper>();
    } else {
        this->fileWrapper = std::move(fileWrapper);
    }
}

FileImageStream::~FileImageStream() { 
    Close(); 
}

ssize_t FileImageStream::Write(uint8_t* data, size_t size) {
    if (fd == -1) {
        // 文件没有打开
        return -1;
    }

    ssize_t result = fileWrapper->write(fd, data, size);
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
    ssize_t totalBytesWritten = 0;

    while (!src.IsEof()) {
        // 从源图像流中读取数据
        ssize_t bytesRead = src.Read(buffer, sizeof(buffer));
        if (bytesRead == 0) {
            break;
        }

        // 如果读取失败，立即返回错误
        if (bytesRead == -1) {
            IMAGE_LOGE("Read from source stream failed.");
            return -1;
        }

        // 将数据写入到当前图像流中
        ssize_t bytesWritten = Write(buffer, bytesRead);
        if (bytesWritten == -1) {
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

    ssize_t result = fileWrapper->read(fd, buf, size);
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

bool FileImageStream::Open(){
    return Open(FileMode::ReadWrite);
}

bool FileImageStream::Open(FileMode mode) {
    // 如果文件已经打开，先关闭它
    if (fd != -1) {
        IMAGE_LOGD("File already opened, close it first");
        Close();
    }

    int flags;
    switch (mode) {
        case FileMode::Read:
            flags = O_RDONLY;
            break;
        case FileMode::ReadWrite:
            flags = O_RDWR | O_CREAT;
            break;
    }

    // 打开文件，root用户可以忽略只读限制，直接打开这个文件
    fd = open(filePath.c_str(), flags, S_IRUSR | S_IWUSR);
    if (fd == -1) {
        // 打开文件失败
        char buf[256];        
        strerror_r(errno, buf, sizeof(buf));
        IMAGE_LOGE("Open file failed: %{public}s, reason: %{public}s", filePath.c_str(), buf);
        return false;
    }

    // 获取文件大小
    struct stat sb;
    if (fileWrapper->fstat(fd, &sb) == -1) {
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
    if(!src.IsOpen()) src.Open();               //如果src没有打开，就打开src
    ssize_t src_cur = src.Tell();               //暂存src的位置
    src.Seek(0, SeekPos::BEGIN);    //将src的位置设置为0
    while (!src.IsEof()) {
        size_t bytesRead = src.Read(tempBuffer, sizeof(tempBuffer));
        if (bytesRead > 0) {
            size_t bytesWritten = Write(tempBuffer, bytesRead);
            if (bytesWritten == static_cast<size_t>(-1)) {
                // 写入失败
                char buf[256];        
                strerror_r(errno, buf, sizeof(buf));
                IMAGE_LOGE("transfer: Write file failed: %{public}s, reason: %{public}s", filePath.c_str(), buf);
                src.Seek(src_cur, SeekPos::BEGIN); //恢复src的位置
                return;
            }
            totalBytesWritten += bytesWritten;
        }
    }
    IMAGE_LOGD("transfer: Write file done: %{public}s, size: %{public}zu", filePath.c_str(), totalBytesWritten);

    // 截断文件
    if (ftruncate(fd, totalBytesWritten) == -1) {
        // 截断文件失败
        char buf[256];        
        strerror_r(errno, buf, sizeof(buf));
        IMAGE_LOGE("transfer: Truncate file failed: %{public}s, reason: %{public}s", filePath.c_str(), buf);
        src.Seek(src_cur, SeekPos::BEGIN); //恢复src的位置
        return;
    }

    //将文件的尺寸设置成新尺寸
    fileSize = totalBytesWritten;
    src.Seek(src_cur, SeekPos::BEGIN); //恢复src的位置
    return;
}

size_t FileImageStream::GetSize() {
    return fileSize;
}

} // namespace Media
} // namespace OHOS
