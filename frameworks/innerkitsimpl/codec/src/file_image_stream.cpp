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

#include <errno.h>
#include <fcntl.h>
#include <memory>
#include <string>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#include "file_image_stream.h"
#include "image_log.h"
#include "image_stream.h"

#undef LOG_DOMAIN
#define LOG_DOMAIN LOG_TAG_DOMAIN_ID_IMAGE

#undef LOG_TAG
#define LOG_TAG "FileImageStream"

namespace OHOS {
namespace Media {

size_t FileWrapper::fwrite(const void *src, size_t size, size_t nmemb,
                           FILE *file)
{
    return ::fwrite(src, size, nmemb, file);
}

size_t FileWrapper::fread(void *destv, size_t size, size_t nmemb, FILE *file)
{
    return ::fread(destv, size, nmemb, file);
}

FileImageStream::FileImageStream(int fileDescriptor)
{
    dupFD_ = dup(fileDescriptor);
    if (dupFD_ == -1) {
        char buf[IMAGE_STREAM_ERROR_BUFFER_SIZE];
        strerror_r(errno, buf, sizeof(buf));
        IMAGE_LOGE("dup: Duplicate file descriptor failed: %{public}s, reason: "
                   "%{public}s",
                   filePath_.c_str(), buf);
    }
    initPath_ = INIT_FROM_FD;

    fileSize_ = 0;
    mappedMemory_ = nullptr;
    this->fileWrapper_ = std::make_unique<FileWrapper>();
}

FileImageStream::FileImageStream(FILE *filePointer)
{
    dupFD_ = dup(fileno(filePointer));
    if (dupFD_ == -1) {
        char buf[IMAGE_STREAM_ERROR_BUFFER_SIZE];
        strerror_r(errno, buf, sizeof(buf));
        IMAGE_LOGE("fileno: Get file descriptor failed: %{public}s, reason: "
                   "%{public}s",
                   filePath_.c_str(), buf);
    }
    fileSize_ = 0;
    mappedMemory_ = nullptr;
    this->fileWrapper_ = std::make_unique<FileWrapper>();
    initPath_ = INIT_FROM_FD;
}

FileImageStream::FileImageStream(const std::string &filePath)
    : fp_(nullptr), filePath_(filePath), fileSize_(0), mappedMemory_(nullptr)
{
    this->fileWrapper_ = std::make_unique<FileWrapper>();
    dupFD_ = -1;
    initPath_ = INIT_FROM_PATH;
}

FileImageStream::FileImageStream(const std::string &filePath,
                                 std::unique_ptr<FileWrapper> fileWrapper)
    : fp_(nullptr), filePath_(filePath), fileSize_(0), mappedMemory_(nullptr)
{
    if (fileWrapper == nullptr) {
        this->fileWrapper_ = std::make_unique<FileWrapper>();
    } else {
        this->fileWrapper_ = std::move(fileWrapper);
    }
    initPath_ = INIT_FROM_PATH;
}

FileImageStream::~FileImageStream() { Close(); }

ssize_t FileImageStream::Write(byte *data, size_t size)
{
    if (fp_ == nullptr) {
        // File is not open
        IMAGE_LOGE("Write file failed: %{public}s, reason: %{public}s",
                   filePath_.c_str(), "fp is nullptr");
        return -1;
    }

    size_t result = fileWrapper_->fwrite(data, 1, size, fp_);
    if (result != size || (ferror(fp_) != 0)) {
        // Write failed
        char buf[IMAGE_STREAM_ERROR_BUFFER_SIZE];
        strerror_r(errno, buf, sizeof(buf));
        if (initPath_ == INIT_FROM_FD) {
            IMAGE_LOGE("Write file failed: %{public}d, reason: "
                       "%{public}s。result is %{public}d, size is %{public}d",
                       dupFD_, buf, result, size);
        } else {
            IMAGE_LOGE("Write file failed: %{public}s, reason: "
                       "%{public}s。result is %{public}d, size is %{public}d",
                       filePath_.c_str(), buf, result, size);
        }
        return -1;
    }

    // Update fileSize_
    long currentPos = ftell(fp_);
    fseek(fp_, 0, SEEK_END);
    fileSize_ = ftell(fp_);
    fseek(fp_, currentPos,
          SEEK_SET); // Restore the file pointer to its original position

    return result;
}

ssize_t FileImageStream::Write(ImageStream &src)
{
    // Create a buffer
    byte buffer[IMAGE_STREAM_PAGE_SIZE];
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
            char buf[IMAGE_STREAM_ERROR_BUFFER_SIZE];
            strerror_r(errno, buf, sizeof(buf));
            IMAGE_LOGE("Write file failed: %{public}s, reason: %{public}s",
                       filePath_.c_str(), buf);
            return -1;
        }

        totalBytesWritten += bytesWritten;
    }

    return totalBytesWritten;
}

ssize_t FileImageStream::Read(byte *buf, size_t size)
{
    if (fp_ == nullptr) {
        // File is not open
        return -1;
    }
    if (size == 0) {
        return 0;
    }

    size_t result = fileWrapper_->fread(buf, 1, size, fp_);
    if (result == 0 && ferror(fp_) != 0) {
        // Read failed
        char buf[IMAGE_STREAM_ERROR_BUFFER_SIZE];
        strerror_r(errno, buf, sizeof(buf));
        IMAGE_LOGE("Read file failed: %{public}s, reason: %{public}s",
                   filePath_.c_str(), buf);
        return -1;
    }

    return result;
}

int FileImageStream::ReadByte()
{
    if (fp_ == nullptr) {
        // File is not open
        return -1;
    }

    int byte = fgetc(fp_);
    if (byte == EOF) {
        // Read failed
        char buf[IMAGE_STREAM_ERROR_BUFFER_SIZE];
        strerror_r(errno, buf, sizeof(buf));
        IMAGE_LOGE("Read file failed: %{public}s, reason: %{public}s",
                   filePath_.c_str(), buf);
        return -1;
    }

    return byte;
}

long FileImageStream::Seek(int offset, SeekPos pos)
{
    if (fp_ == nullptr) {
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

    int result = fseek(fp_, offset, origin);
    if (result != 0) {
        // Seek failed
        return -1;
    }

    return ftell(fp_);
}

long FileImageStream::Tell()
{
    if (fp_ == nullptr) {
        if (initPath_ == INIT_FROM_FD) {
            IMAGE_LOGE("Tell file failed: %{public}d, reason: %{public}s",
                       dupFD_, "fp is nullptr");
        } else if (initPath_ == INIT_FROM_PATH) {
            IMAGE_LOGE("Tell file failed: %{public}s, reason: %{public}s",
                       filePath_.c_str(), "fp is nullptr");
        } else if (initPath_ == INIT_FROM_UNKNOWN) {
            IMAGE_LOGE("Tell file failed: %{public}s, reason: %{public}s",
                       "initPath is INIT_FROM_UNKNOWN", "fp is nullptr");
        }
        IMAGE_LOGE("Tell file failed: %{public}s, reason: %{public}s",
                   filePath_.c_str(), "fp is nullptr");
        return -1;
    }

    return ftell(fp_);
}

bool FileImageStream::IsEof()
{
    if (fp_ == nullptr) {
        // File is not open
        IMAGE_LOGE("Check EOF failed: fp is nullptr");
        return true;
    }

    if (ferror(fp_) != 0) {
        char errstr[IMAGE_STREAM_ERROR_BUFFER_SIZE];
        strerror_r(errno, errstr, sizeof(errstr));
        IMAGE_LOGE("Check EOF failed: %{public}s", errstr);
        return true;
    }

    long currentPos = ftell(fp_);
    fseek(fp_, 0, SEEK_END);
    long fileSize = ftell(fp_);

    bool isEof = currentPos == fileSize;

    fseek(fp_, currentPos, SEEK_SET); // Restore the file pointer to its
                                      // original position after comparison

    return isEof;
}

bool FileImageStream::IsOpen() { return fp_ != nullptr; }

void FileImageStream::Close()
{
    // If there is a memory map, delete it
    MUnmap();

    // If the file is not open, return directly
    if (fp_ != nullptr) {
        fclose(fp_);
        fp_ = nullptr;
    }

    // Close the file
    int tmpFD = dupFD_;
    if (dupFD_ != -1) {
        // close(dupFD); fclose已经释放了
        dupFD_ = -1;
    }

    // Reset all member variables
    fileSize_ = 0;
    if (initPath_ == INIT_FROM_FD) {
        IMAGE_LOGD("File closed: %{public}d", tmpFD);
    } else if (initPath_ == INIT_FROM_PATH) {
        IMAGE_LOGD("File closed: %{public}s", filePath_.c_str());
    }
    initPath_ = INIT_FROM_UNKNOWN;
}

// Todo 移走
bool FileImageStream::Open() { return Open(OpenMode::ReadWrite); }

bool FileImageStream::OpenFromFD(const char *modeStr)
{
    if (dupFD_ == -1) {
        IMAGE_LOGE(
            "FileImageStream: Open file failed: %{public}s, reason: %{public}s",
            filePath_.c_str(), "dupFD is -1");
        return false;
    }
    // Decide how to create FILE* fp based on the mode parameter
    fp_ = fdopen(dupFD_, modeStr);
    if (fp_ == NULL || ferror(fp_) != 0) {
        // Handle errors, such as throwing exceptions or returning error codes
        char buf[IMAGE_STREAM_ERROR_BUFFER_SIZE];
        strerror_r(errno, buf, sizeof(buf));
        IMAGE_LOGE("FileImageStream: Open file failed: %{public}s, reason: "
                   "%{public}s.",
                   filePath_.c_str(), buf);
        return false;
    }
    IMAGE_LOGD("File opened: %{public}d", dupFD_);

    return true;
}

bool FileImageStream::OpenFromPath(const char *modeStr)
{
    fp_ = fopen(filePath_.c_str(), modeStr);
    if (fp_ == nullptr) {
        if (strcmp(modeStr, "r+") == 0) {
            // If opening the file in read-write mode fails, try creating a new
            // file
            fp_ = fopen(filePath_.c_str(), "w");
            if (fp_ == nullptr) {
                // Failed to create file
                char buf[IMAGE_STREAM_ERROR_BUFFER_SIZE];
                strerror_r(errno, buf, sizeof(buf));
                IMAGE_LOGE("Open file failed: %{public}s, reason: %{public}s",
                           filePath_.c_str(), buf);
                return false;
            }
            // Close the file, then reopen it in "r+" mode
            fclose(fp_);
            fp_ = fopen(filePath_.c_str(), "r+");
            if (fp_ == nullptr) {
                // Failed to reopen the file
                char buf[IMAGE_STREAM_ERROR_BUFFER_SIZE];
                strerror_r(errno, buf, sizeof(buf));
                IMAGE_LOGE("Reopen file failed: %{public}s, reason: %{public}s",
                           filePath_.c_str(), buf);
                return false;
            }
        } else {
            // Open failed
            char buf[IMAGE_STREAM_ERROR_BUFFER_SIZE];
            strerror_r(errno, buf, sizeof(buf));
            IMAGE_LOGE("Open file failed: %{public}s, reason: %{public}s",
                       filePath_.c_str(), buf);
            return false;
        }
    }
    IMAGE_LOGD("File opened: %{public}s", filePath_.c_str());
    return true;
}

bool FileImageStream::Open(OpenMode mode)
{
    if (initPath_ == INIT_FROM_UNKNOWN) {
        IMAGE_LOGE("initPath is INIT_FROM_UNKNOWN. It seems that the file has "
                   "been closed before.");
        return false;
    }

    const char *modeStr;
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
    if (initPath_ == INIT_FROM_FD) {
        IMAGE_LOGD("initPath is INIT_FROM_FD");
        openResult = OpenFromFD(modeStr);
    }
    if (initPath_ == INIT_FROM_PATH) {
        openResult = OpenFromPath(modeStr);
    }

    if (!openResult) {
        return false;
    }

    // Get the file size
    fseek(fp_, 0, SEEK_END);
    fileSize_ = ftell(fp_);
    fseek(fp_, 0, SEEK_SET);

    return true;
}

bool FileImageStream::Flush()
{
    if (fp_ == nullptr) {
        if (initPath_ == INIT_FROM_FD) {
            IMAGE_LOGE("Flush file failed: %{public}d, reason: %{public}s",
                       dupFD_, "fp is nullptr");
        } else if (initPath_ == INIT_FROM_PATH) {
            IMAGE_LOGE("Flush file failed: %{public}s, reason: %{public}s",
                       filePath_.c_str(), "fp is nullptr");
        } else if (initPath_ == INIT_FROM_UNKNOWN) {
            IMAGE_LOGE("Flush file failed: %{public}s, reason: %{public}s",
                       "initPath is INIT_FROM_UNKNOWN", "fp is nullptr");
        }
        return false;
    }

    if (fflush(fp_) != 0) {
        char errstr[IMAGE_STREAM_ERROR_BUFFER_SIZE];
        strerror_r(errno, errstr, sizeof(errstr));
        IMAGE_LOGE("Flush file failed: %{public}s", errstr);
        return false;
    }

    return true;
}

byte *FileImageStream::MMap(bool isWriteable)
{
    // If there is already a memory map, return it directly
    if (mappedMemory_ != nullptr) {
        IMAGE_LOGE("mmap: There is already a memory mapping, return it directly");
        return (byte *)mappedMemory_;
    }

    // If the file is not open, open it first
    if (fp_ == nullptr) {
        IMAGE_LOGE("mmap: File is not open: %{public}s", filePath_.c_str());
        return nullptr;
    }

    // Get the file descriptor from the file pointer
    int fileDescriptor = fileno(fp_);

    // Create a memory map
    mappedMemory_ = ::mmap(nullptr, fileSize_,
                           isWriteable ? (PROT_READ | PROT_WRITE) : PROT_READ,
                           MAP_SHARED, fileDescriptor, 0);
    if (mappedMemory_ == (void *)MAP_FAILED) {
        // Memory mapping failed
        char buf[IMAGE_STREAM_ERROR_BUFFER_SIZE];
        strerror_r(errno, buf, sizeof(buf));
        IMAGE_LOGE(
            "mmap: Memory mapping failed: %{public}s, reason: %{public}s",
            filePath_.c_str(), buf);
        mappedMemory_ = nullptr;
    }
    IMAGE_LOGD("mmap: Memory mapping created: %{public}s, size: %{public}zu",
               filePath_.c_str(), fileSize_);
    return (byte *)mappedMemory_;
}

bool FileImageStream::MUnmap()
{
    if (mappedMemory_ == nullptr) {
        // The memory map is nullptr
        return true;
    }

    // Delete the memory map
    if (munmap(mappedMemory_, fileSize_) == -1) {
        // Memory mapping failed
        char buf[IMAGE_STREAM_ERROR_BUFFER_SIZE];
        strerror_r(errno, buf, sizeof(buf));
        IMAGE_LOGE(
            "munmap: Memory mapping failed: %{public}s, reason: %{public}s",
            filePath_.c_str(), buf);
        return false;
    }
    IMAGE_LOGD("munmap: Memory mapping removed: %{public}s, size: %{public}zu",
               filePath_.c_str(), fileSize_);

    mappedMemory_ = nullptr;
    return true;
}

bool FileImageStream::CopyFrom(ImageStream &src)
{
    if (!src.IsOpen()) {
        IMAGE_LOGE("transfer: Source file is not open");
        return false;
    }

    if (!IsOpen()) {
        IMAGE_LOGE("transfer: File is not open: %{public}s", filePath_.c_str());
        return false;
    }

    // Read data from the source ImageStream and write it to the current file
    size_t buffer_size =
        std::min((size_t)IMAGE_STREAM_PAGE_SIZE, src.GetSize());
    byte tempBuffer[buffer_size];
    size_t totalBytesWritten = 0;
    ssize_t src_cur = src.Tell(); // Temporarily store the position of src

    Seek(0, SeekPos::BEGIN);
    src.Seek(0, SeekPos::BEGIN); // Set the position of src to 0

    while (!src.IsEof()) {
        size_t bytesRead = src.Read(tempBuffer, sizeof(tempBuffer));
        if (bytesRead > 0) {
            size_t bytesWritten = Write(tempBuffer, bytesRead);
            if (bytesWritten == static_cast<size_t>(-1)) {
                // Write failed
                char buf[IMAGE_STREAM_ERROR_BUFFER_SIZE];
                strerror_r(errno, buf, sizeof(buf));
                IMAGE_LOGE("transfer: Write file failed: %{public}s, reason: "
                           "%{public}s",
                           filePath_.c_str(), buf);
                src.Seek(src_cur,
                         SeekPos::BEGIN); // Restore the position of src
                return false;
            }
            totalBytesWritten += bytesWritten;
        }
        if (bytesRead < 0 && !src.IsEof()) {
            IMAGE_LOGE("CopyFrom: read file error");
            return false;
        }
    }
    IMAGE_LOGD("transfer: Write file done: %{public}s, size: %{public}zu",
               filePath_.c_str(), totalBytesWritten);

    // Flush the file
    if (fflush(fp_) != 0) {
        // Failed to flush the file
        char buf[IMAGE_STREAM_ERROR_BUFFER_SIZE];
        strerror_r(errno, buf, sizeof(buf));
        IMAGE_LOGE(
            "transfer: Flush file failed: %{public}s, reason: %{public}s",
            filePath_.c_str(), buf);
        src.Seek(src_cur, SeekPos::BEGIN); // Restore the position of src
        return false;
    }

    // Truncate the file
    int fileDescriptor = fileno(fp_);
    if (ftruncate(fileDescriptor, totalBytesWritten) == -1) {
        // Failed to truncate the file
        char buf[IMAGE_STREAM_ERROR_BUFFER_SIZE];
        strerror_r(errno, buf, sizeof(buf));
        IMAGE_LOGE(
            "transfer: Truncate file failed: %{public}s, reason: %{public}s",
            filePath_.c_str(), buf);
        src.Seek(src_cur, SeekPos::BEGIN); // Restore the position of src
        return false;
    }

    // Set the file size to the new size
    fileSize_ = totalBytesWritten;
    src.Seek(src_cur, SeekPos::BEGIN);
    return true;
}

size_t FileImageStream::GetSize() { return fileSize_; }
} // namespace Media
} // namespace OHOS
