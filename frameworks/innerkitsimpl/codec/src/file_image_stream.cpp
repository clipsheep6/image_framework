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
        // File is not open
        return -1;
    }

    ssize_t result = fileWrapper->write(fd, data, size);
    if (result == -1) {
        // Write failed
        char buf[256];        
        strerror_r(errno, buf, sizeof(buf));
        IMAGE_LOGE("Write file failed: %{public}s, reason: %{public}s", filePath.c_str(), buf);
        return -1;
    }

    currentOffset += result;
    return result;
}

ssize_t FileImageStream::Write(ImageStream& src) {
    // Create a buffer
    uint8_t buffer[4096];
    ssize_t totalBytesWritten = 0;

    while (!src.IsEof()) {
        // Read data from the source image stream
        ssize_t bytesRead = src.Read(buffer, sizeof(buffer));
        if (bytesRead == 0) {
            break;
        }

        // If reading fails, return the error immediately
        if (bytesRead == -1) {
            IMAGE_LOGE("Read from source stream failed.");
            return -1;
        }

        // Write the data to the current image stream
        ssize_t bytesWritten = Write(buffer, bytesRead);
        if (bytesWritten == -1) {
            // Write failed
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
        // File is not open
        return -1;
    }

    ssize_t result = fileWrapper->read(fd, buf, size);
    if (result == -1) {
        // Read failed
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
        // File is not open
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
        // Seek failed
        return -1;
    }

    currentOffset = result;
    return currentOffset;
}

ssize_t FileImageStream::Tell() {
    if (fd == -1) {
        // File is not open
        return -1;
    }

    return currentOffset;
}

bool FileImageStream::IsEof() {
    if (fd == -1) {
        // File is not open
        return true;
    }

    return currentOffset >= fileSize;
}

bool FileImageStream::IsOpen() {
    return fd != -1;
}

void FileImageStream::Close() {
    // If the file is not open, return directly
    if (fd == -1) {
        return;
    }

    // If there is a memory map, delete it
    if (mappedMemory != nullptr) {
        munmap(mappedMemory, fileSize);
        mappedMemory = nullptr;
    }

    // Close the file
    close(fd);

    // Reset all member variables
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
    // If the file is already open, close it first
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

    // Open the file, root user can ignore read-only restrictions and directly open this file
    fd = open(filePath.c_str(), flags, S_IRUSR | S_IWUSR);
    if (fd == -1) {
        // Failed to open the file
        char buf[256];        
        strerror_r(errno, buf, sizeof(buf));
        IMAGE_LOGE("Open file failed: %{public}s, reason: %{public}s", filePath.c_str(), buf);
        return false;
    }

    // Get the file size
    struct stat sb;
    if (fileWrapper->fstat(fd, &sb) == -1) {
        // Failed to get the file size
        char buf[256];        
        strerror_r(errno, buf, sizeof(buf));
        IMAGE_LOGE("Get file size failed: %{public}s, reason: %{public}s", filePath.c_str(), buf);
        close(fd);
        fd = -1;
        return false;
    }
    fileSize = sb.st_size;

    // Reset the current offset
    currentOffset = 0;
    IMAGE_LOGD("File opened: %{public}s, size: %{public}zu", filePath.c_str(), fileSize);
    return true;
}

byte* FileImageStream::MMap(bool isWriteable) {
    // If the file is not open, open it first
    if (fd == -1) {
        if (!Open()) {
            // Failed to open the file
            char buf[256];        
            strerror_r(errno, buf, sizeof(buf));
            IMAGE_LOGE("mmap: Open file failed: %{public}s, reason: %{public}s", filePath.c_str(), buf);
            return nullptr;
        }
    }

    // If there is already a memory map, delete it first
    if (mappedMemory != nullptr) {
        IMAGE_LOGW("mmap: There is already a memory mapping, remove it first");
        munmap(mappedMemory, fileSize);
    }

    // Create a memory map
    mappedMemory = static_cast<byte*>(::mmap(nullptr, fileSize, isWriteable ? (PROT_READ | PROT_WRITE) : PROT_READ, MAP_SHARED, fd, 0));
    if (mappedMemory == MAP_FAILED) {
        // Memory mapping failed
        char buf[256];        
        strerror_r(errno, buf, sizeof(buf));
        IMAGE_LOGE("mmap: Memory mapping failed: %{public}s, reason: %{public}s", filePath.c_str(), buf);
        mappedMemory = nullptr;
    }
    IMAGE_LOGD("mmap: Memory mapping created: %{public}s, size: %{public}zu", filePath.c_str(), fileSize);
    return mappedMemory;
}

void FileImageStream::Transfer(ImageStream& src) {
    // If the file is already open, close it first
    if (IsOpen()) {
        Close();
    }

    // Open the new file
    if (!Open()) {
        // Failed to open the file
        char buf[256];        
        strerror_r(errno, buf, sizeof(buf));
        IMAGE_LOGE("transfer: Open file failed: %{public}s, reason: %{public}s", filePath.c_str(), buf);
        return;
    }

    // Read data from the source ImageStream and write it to the current file
    uint8_t tempBuffer[4096];
    size_t totalBytesWritten = 0;
    if(!src.IsOpen()) src.Open();               // If src is not open, open src
    ssize_t src_cur = src.Tell();               // Temporarily store the position of src
    src.Seek(0, SeekPos::BEGIN);    // Set the position of src to 0
    while (!src.IsEof()) {
        size_t bytesRead = src.Read(tempBuffer, sizeof(tempBuffer));
        if (bytesRead > 0) {
            size_t bytesWritten = Write(tempBuffer, bytesRead);
            if (bytesWritten == static_cast<size_t>(-1)) {
                // Write failed
                char buf[256];        
                strerror_r(errno, buf, sizeof(buf));
                IMAGE_LOGE("transfer: Write file failed: %{public}s, reason: %{public}s", filePath.c_str(), buf);
                src.Seek(src_cur, SeekPos::BEGIN); // Restore the position of src
                return;
            }
            totalBytesWritten += bytesWritten;
        }
    }
    IMAGE_LOGD("transfer: Write file done: %{public}s, size: %{public}zu", filePath.c_str(), totalBytesWritten);

    // Truncate the file
    if (ftruncate(fd, totalBytesWritten) == -1) {
        // Failed to truncate the file
        char buf[256];        
        strerror_r(errno, buf, sizeof(buf));
        IMAGE_LOGE("transfer: Truncate file failed: %{public}s, reason: %{public}s", filePath.c_str(), buf);
        src.Seek(src_cur, SeekPos::BEGIN); // Restore the position of src
        return;
    }

    // Set the file size to the new size
    fileSize = totalBytesWritten;
    src.Seek(src_cur, SeekPos::BEGIN); // Restore the position of src
    return;
}

size_t FileImageStream::GetSize() {
    return fileSize;
}
} // namespace Media
} // namespace OHOS
