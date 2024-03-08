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
#include "image_stream.h"
#include "out/rk3568/obj/third_party/musl/intermidiates/linux/musl_src_ported/include/stdio.h"

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

size_t FileWrapper::fwrite(const void* src, size_t size, size_t nmemb, FILE* f){
    return ::fwrite(src, size, nmemb, f);
}

size_t FileWrapper::fread(void* destv, size_t size, size_t nmemb, FILE* f){
    return ::fread(destv, size, nmemb, f);
}

FileImageStream::FileImageStream(int fd){
    dupFD = dup(fd);
    if(dupFD == -1){
        char buf[256];        
        strerror_r(errno, buf, sizeof(buf));
        IMAGE_LOGE("dup: Duplicate file descriptor failed: %{public}s, reason: %{public}s", filePath.c_str(), buf);
    }
    initPath = INIT_FROM_FD;

    fileSize = 0;
    mappedMemory = nullptr;
    this->fileWrapper = std::make_unique<FileWrapper>();
}

FileImageStream::FileImageStream(FILE *p){
    dupFD = fileno(p);
    if(dupFD == -1){
        char buf[256];        
        strerror_r(errno, buf, sizeof(buf));
        IMAGE_LOGE("fileno: Get file descriptor failed: %{public}s, reason: %{public}s", filePath.c_str(), buf);
    }
    fileSize = 0;
    mappedMemory = nullptr;
    this->fileWrapper = std::make_unique<FileWrapper>();
    initPath = INIT_FROM_FD;
}

FileImageStream::FileImageStream(const std::string& filePath)
    : fp(nullptr), filePath(filePath), fileSize(0), mappedMemory(nullptr) {
    this->fileWrapper = std::make_unique<FileWrapper>();
    dupFD = -1;
    initPath = INIT_FROM_PATH;
}

FileImageStream::FileImageStream(const std::string& filePath, std::unique_ptr<FileWrapper> fileWrapper)
    : fp(nullptr), filePath(filePath), fileSize(0), mappedMemory(nullptr){
    if (fileWrapper == nullptr) {
        this->fileWrapper = std::make_unique<FileWrapper>();
    } else {
        this->fileWrapper = std::move(fileWrapper);
    }
    initPath = INIT_FROM_PATH;
}

FileImageStream::~FileImageStream() { 
    Close(); 
}

ssize_t FileImageStream::Write(uint8_t* data, size_t size) {
    if (fp == nullptr) {
        // File is not open
        IMAGE_LOGE("Write file failed: %{public}s, reason: %{public}s", filePath.c_str(), "fp is nullptr");
        return -1;
    }

    // size_t result = ::fwrite(data, 1, size, fp);
    size_t result = fileWrapper->fwrite(data, 1, size, fp);
    if (result != size || ferror(fp)) {
        // Write failed
        char buf[256];        
        strerror_r(errno, buf, sizeof(buf));
        if(initPath == INIT_FROM_FD){
            IMAGE_LOGE("Write file failed: %{public}d, reason: %{public}s。result is %{public}d, size is %{public}d", dupFD, buf, result, size);
        }else{
            IMAGE_LOGE("Write file failed: %{public}s, reason: %{public}s。result is %{public}d, size is %{public}d", filePath.c_str(), buf, result, size);
        }
        return -1;
    }
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
    if (fp == nullptr) {
        // File is not open
        return -1;
    }

    size_t result = fileWrapper->fread(buf, 1, size, fp);
    if (result == 0 && ferror(fp)) {
        // Read failed
        char buf[256];        
        strerror_r(errno, buf, sizeof(buf));
        IMAGE_LOGE("Read file failed: %{public}s, reason: %{public}s", filePath.c_str(), buf);
        return -1;
    }

    return result;
}

int FileImageStream::ReadByte(){
    if (fp == nullptr) {
        // File is not open
        return -1;
    }

    int byte = fgetc(fp);
    if (byte == EOF) {
        // Read failed
        char buf[256]; 
        strerror_r(errno, buf, sizeof(buf));
        IMAGE_LOGE("Read file failed: %{public}s, reason: %{public}s", filePath.c_str(), buf);
        return -1;
    }

    return byte;
}

int FileImageStream::Seek(int offset, SeekPos pos) {
    if (fp == nullptr) {
        // File is not open
        return -1;
    }

    int origin;
    switch (pos) {
        case SeekPos::BEGIN:
            origin = SEEK_SET;
            break;
        case SeekPos::CURRENT:
            origin = SEEK_CUR;
            break;
        case SeekPos::END:
            origin = SEEK_END;
            break;
        default:
            return -1;
    }

    int result = fseek(fp, offset, origin);
    if (result != 0) {
        // Seek failed
        return -1;
    }
    
    return ftell(fp);
}

ssize_t FileImageStream::Tell() {
    if (fp == nullptr) {
        // File is not open
        return -1;
    }

    return ftell(fp);
}

bool FileImageStream::IsEof() {
    if (fp == nullptr) {
        // File is not open
        return true;
    }

    return feof(fp);
}

bool FileImageStream::IsOpen() {
    return fp != nullptr;
}

void FileImageStream::Close() {
    // If there is a memory map, delete it
    if (mappedMemory != nullptr) {
        if(munmap(mappedMemory, fileSize) == -1){
            char buf[256];        
            strerror_r(errno, buf, sizeof(buf));
            IMAGE_LOGE("munmap: Memory mapping failed: %{public}s, reason: %{public}s", filePath.c_str(), buf);
        }
        mappedMemory = nullptr;
    }

    // If the file is not open, return directly
    if (fp != nullptr) {
        fclose(fp);
        fp = nullptr;
    }
    // Close the file
    if (dupFD != -1){
        close(dupFD);
        dupFD = -1;
    }

    // Reset all member variables
    fileSize = 0;
    IMAGE_LOGD("File closed: %{public}s", filePath.c_str());
    return;
}

bool FileImageStream::Open(){
    return Open(OpenMode::ReadWrite);
}

bool FileImageStream::Open(OpenMode mode){
    if(initPath == INIT_FROM_FD){
        IMAGE_LOGD("initPath is INIT_FROM_FD");
        if(dupFD == -1){
            IMAGE_LOGE("FileImageStream: Open file failed: %{public}s, reason: %{public}s", filePath.c_str(), "dupFD is -1");
            return false;
        }
        // 根据mode参数决定如何创建FILE* fp
        if (mode == OpenMode::ReadWrite) {
            fp = fdopen(dupFD, "r+");
            IMAGE_LOGD("openMode is ReadWrite");
        } else if (mode == OpenMode::Read) {
            fp = fdopen(dupFD, "r");
            IMAGE_LOGD("openMode is Read");
        }

        if (fp == NULL || ferror(fp)) {
            // 处理错误，例如抛出异常或返回错误代码
            char buf[256];        
            strerror_r(errno, buf, sizeof(buf));
            IMAGE_LOGE("FileImageStream: Open file failed: %{public}s, reason: %{public}s。", filePath.c_str(), buf);
            return false;
        }else{
            IMAGE_LOGD("File opened: %{public}d", dupFD);
            return true;
        }
    }

    const char* modeStr;
    switch (mode) {
        case OpenMode::Read:
            modeStr = "r";
            break;
        case OpenMode::ReadWrite:
            modeStr = "r+";
            break;
        default:
            return false;
    }

    fp = fopen(filePath.c_str(), modeStr);
    if (fp == nullptr) {
        if (mode == OpenMode::ReadWrite) {
            // 如果以读写模式打开文件失败，尝试创建新文件
            fp = fopen(filePath.c_str(), "w");
            if (fp == nullptr) {
                // 创建文件失败
                char buf[256];        
                strerror_r(errno, buf, sizeof(buf));
                IMAGE_LOGE("Open file failed: %{public}s, reason: %{public}s", filePath.c_str(), buf);
                return false;
            }
            // 关闭文件，然后以 "r+" 模式重新打开
            fclose(fp);
            fp = fopen(filePath.c_str(), "r+");
            if (fp == nullptr) {
                // 重新打开文件失败
                char buf[256];        
                strerror_r(errno, buf, sizeof(buf));
                IMAGE_LOGE("Reopen file failed: %{public}s, reason: %{public}s", filePath.c_str(), buf);
                return false;
            }
        } else {
            // Open failed
            char buf[256];        
            strerror_r(errno, buf, sizeof(buf));
            IMAGE_LOGE("Open file failed: %{public}s, reason: %{public}s", filePath.c_str(), buf);
            return false;
        }
    }

    // Get the file size
    fseek(fp, 0, SEEK_END);
    fileSize = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    IMAGE_LOGD("File opened: %{public}s", filePath.c_str());
    return true;
}

byte* FileImageStream::MMap(bool isWriteable) {
    // If the file is not open, open it first
    if (fp == nullptr) {
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

    // Get the file descriptor from the file pointer
    int fd = fileno(fp);

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

bool FileImageStream::MUnmap(byte* mmap){
    if (mmap == nullptr) {
        // The memory map is nullptr
        return false;
    }

    // Delete the memory map
    if (munmap(mmap, fileSize) == -1) {
        // Memory mapping failed
        char buf[256];        
        strerror_r(errno, buf, sizeof(buf));
        IMAGE_LOGE("munmap: Memory mapping failed: %{public}s, reason: %{public}s", filePath.c_str(), buf);
        return false;
    }
    IMAGE_LOGD("munmap: Memory mapping removed: %{public}s, size: %{public}zu", filePath.c_str(), fileSize);
    return true;
}

void FileImageStream::CopyFrom(ImageStream& src) {
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

    // Flush the file
    if (fflush(fp) != 0) {
        // Failed to flush the file
        char buf[256];        
        strerror_r(errno, buf, sizeof(buf));
        IMAGE_LOGE("transfer: Flush file failed: %{public}s, reason: %{public}s", filePath.c_str(), buf);
        src.Seek(src_cur, SeekPos::BEGIN); // Restore the position of src
        return;
    }

    // Truncate the file
    int fd = fileno(fp);
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
