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
    dupFD = dup(fileno(p));
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

ssize_t FileImageStream::Write(byte* data, size_t size) {
    if (fp == nullptr) {
        // File is not open
        IMAGE_LOGE("Write file failed: %{public}s, reason: %{public}s", filePath.c_str(), "fp is nullptr");
        return -1;
    }

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
    byte buffer[4096];
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

ssize_t FileImageStream::Read(byte* buf, size_t size) {
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

//todo，改成直接暴露
long FileImageStream::Seek(int offset, SeekPos pos) {
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

long FileImageStream::Tell() {
    if (fp == nullptr) {
        if(initPath == INIT_FROM_FD){
            IMAGE_LOGE("Tell file failed: %{public}d, reason: %{public}s", dupFD, "fp is nullptr");
        }else if(initPath == INIT_FROM_PATH){
            IMAGE_LOGE("Tell file failed: %{public}s, reason: %{public}s", filePath.c_str(), "fp is nullptr");
        }else if(initPath == INIT_FROM_UNKNOWN){
            IMAGE_LOGE("Tell file failed: %{public}s, reason: %{public}s", "initPath is INIT_FROM_UNKNOWN", "fp is nullptr");
        }
        IMAGE_LOGE("Tell file failed: %{public}s, reason: %{public}s", filePath.c_str(), "fp is nullptr");
        return -1;
    }

    return ftell(fp);
}

bool FileImageStream::IsEof() {
    if (fp == nullptr) {
        // File is not open
        IMAGE_LOGE("Check EOF failed: fp is nullptr");
        return true;
    }

    if (ferror(fp)) {
        char errstr[256];
        strerror_r(errno, errstr, sizeof(errstr));
        IMAGE_LOGE("Check EOF failed: %{public}s", errstr);
        clearerr(fp); // Clear the error
        return true;
    }

    return feof(fp) != 0;
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
    // Todo 这里是否需要关闭dupFD
    int tmpFD = dupFD;
    if (dupFD != -1){
        close(dupFD);
        dupFD = -1;
    }

    // Reset all member variables
    fileSize = 0;
    if(initPath == INIT_FROM_FD){
        IMAGE_LOGD("File closed: %{public}d", tmpFD);
    }else if(initPath == INIT_FROM_PATH){
        IMAGE_LOGD("File closed: %{public}s", filePath.c_str());
    }
    initPath = INIT_FROM_UNKNOWN;
    return;
}

bool FileImageStream::Open(){
    return Open(OpenMode::ReadWrite);
}

bool FileImageStream::OpenFromFD(const char* modeStr){
    if(dupFD == -1){
        IMAGE_LOGE("FileImageStream: Open file failed: %{public}s, reason: %{public}s", filePath.c_str(), "dupFD is -1");
        return false;
    }
    // Decide how to create FILE* fp based on the mode parameter
    fp = fdopen(dupFD, modeStr);
    if (fp == NULL || ferror(fp)) {
        // Handle errors, such as throwing exceptions or returning error codes
        char buf[256];        
        strerror_r(errno, buf, sizeof(buf));
        IMAGE_LOGE("FileImageStream: Open file failed: %{public}s, reason: %{public}s.", filePath.c_str(), buf);
        return false;
    }else{
        IMAGE_LOGD("File opened: %{public}d", dupFD);
    }
    return true;
}

bool FileImageStream::OpenFromPath(const char* modeStr){
    fp = fopen(filePath.c_str(), modeStr);
    if (fp == nullptr) {
        if (strcmp(modeStr, "r+") == 0) {
            // If opening the file in read-write mode fails, try creating a new file
            fp = fopen(filePath.c_str(), "w");
            if (fp == nullptr) {
                // Failed to create file
                char buf[256];        
                strerror_r(errno, buf, sizeof(buf));
                IMAGE_LOGE("Open file failed: %{public}s, reason: %{public}s", filePath.c_str(), buf);
                return false;
            }
            // Close the file, then reopen it in "r+" mode
            fclose(fp);
            fp = fopen(filePath.c_str(), "r+");
            if (fp == nullptr) {
                // Failed to reopen the file
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
    IMAGE_LOGD("File opened: %{public}s", filePath.c_str());
    return true;
}

bool FileImageStream::Open(OpenMode mode){
    if(initPath == INIT_FROM_UNKNOWN){
        IMAGE_LOGE("initPath is INIT_FROM_UNKNOWN. It seems that the file has been closed before.");
        return false;
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

    bool openResult = false;
    if(initPath == INIT_FROM_FD){
        IMAGE_LOGD("initPath is INIT_FROM_FD");
        openResult = OpenFromFD(modeStr);
    }
    if(initPath == INIT_FROM_PATH){
        openResult = OpenFromPath(modeStr);
    }

    if (!openResult) {
        return false;
    }

    // Get the file size
    fseek(fp, 0, SEEK_END);
    fileSize = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    return true;
}

bool FileImageStream::Flush(){
    if (fp == nullptr) {
        if(initPath == INIT_FROM_FD){
            IMAGE_LOGE("Flush file failed: %{public}d, reason: %{public}s", dupFD, "fp is nullptr");
        }else if(initPath == INIT_FROM_PATH){
            IMAGE_LOGE("Flush file failed: %{public}s, reason: %{public}s", filePath.c_str(), "fp is nullptr");
        }else if(initPath == INIT_FROM_UNKNOWN){
            IMAGE_LOGE("Flush file failed: %{public}s, reason: %{public}s", "initPath is INIT_FROM_UNKNOWN", "fp is nullptr");
        }        
        return false;
    }

    if (fflush(fp) != 0) {
        char errstr[100];
        strerror_r(errno, errstr, sizeof(errstr));
        IMAGE_LOGE("Flush file failed: %{public}s", errstr);
        return false;
    }

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
    byte tempBuffer[4096];
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
