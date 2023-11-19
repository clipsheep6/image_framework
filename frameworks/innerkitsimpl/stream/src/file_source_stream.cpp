/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

#include <unistd.h>
#include "image_log.h"
#include "image_utils.h"
#include "media_errors.h"
#include "directory_ex.h"
#include "file_packer_stream.h"
#include "file_source_stream.h"

#if !defined(_WIN32) && !defined(_APPLE) &&!defined(IOS_PLATFORM) &&!defined(A_PLATFORM)
#include <sys/mman.h>
#include "ashmem.h"
#define SUPPORT_MMAP
#endif

namespace OHOS {
namespace Media {
using namespace OHOS::HiviewDFX;
using namespace std;
using namespace ImagePlugin;

FileSourceStream::FileSourceStream(std::FILE *file, size_t size, size_t offset, size_t original)
    : filePtr_(file), fileSize_(size), fileOffset_(offset), fileOriginalOffset_(original)
{}

FileSourceStream::~FileSourceStream()
{
    IMAGE_LOGE("[FileSourceStream]destructor enter.");
    fclose(filePtr_);
    ResetReadBuffer();
}

unique_ptr<FileSourceStream> FileSourceStream::CreateSourceStream(const string &pathName)
{
    string realPath;
    if (!PathToRealPath(pathName, realPath)) {
        IMAGE_LOGE("[FileSourceStream]input the file path exception.");
        return nullptr;
    }
    size_t size = 0;
    if (!ImageUtils::GetFileSize(realPath, size)) {
        IMAGE_LOGE("[FileSourceStream]get the file size fail.");
        return nullptr;
    }
    FILE *filePtr = fopen(realPath.c_str(), "rb");
    if (filePtr == nullptr) {
        IMAGE_LOGE("[FileSourceStream]open file fail.");
        return nullptr;
    }
    int64_t offset = ftell(filePtr);
    if (offset < 0) {
        IMAGE_LOGE("[FileSourceStream]get the position fail.");
        fclose(filePtr);
        return nullptr;
    }
    return (unique_ptr<FileSourceStream>(new FileSourceStream(filePtr, size, offset, offset)));
}

unique_ptr<FileSourceStream> FileSourceStream::CreateSourceStream(const int fd)
{
    size_t size = 0;

    int dupFd = dup(fd);
    if (dupFd < 0) {
        IMAGE_LOGE("[FileSourceStream]Fail to dup fd.");
        return nullptr;
    }

    if (!ImageUtils::GetFileSize(dupFd, size)) {
        IMAGE_LOGE("[FileSourceStream]get the file size fail.");
        return nullptr;
    }
    FILE *filePtr = fdopen(dupFd, "rb");
    if (filePtr == nullptr) {
        IMAGE_LOGE("[FileSourceStream]open file fail.");
        return nullptr;
    }

    int ret = fseek(filePtr, 0, SEEK_SET);
    if (ret != 0) {
        IMAGE_LOGE("[FileSourceStream]Go to 0 position fail, ret:%{public}d.", ret);
    }

    int64_t offset = ftell(filePtr);
    if (offset < 0) {
        IMAGE_LOGE("[FileSourceStream]get the position fail.");
        fclose(filePtr);
        return nullptr;
    }
    return (unique_ptr<FileSourceStream>(new FileSourceStream(filePtr, size, offset, offset)));
}

unique_ptr<FileSourceStream> FileSourceStream::CreateSourceStream(
    const int fd, int32_t offset, int32_t length)
{
    int dupFd = dup(fd);
    if (dupFd < 0) {
        IMAGE_LOGE("[FileSourceStream]Fail to dup fd.");
        return nullptr;
    }

    FILE *filePtr = fdopen(dupFd, "rb");
    if (filePtr == nullptr) {
        IMAGE_LOGE("[FileSourceStream]open file fail.");
        return nullptr;
    }

    int ret = fseek(filePtr, offset, SEEK_SET);
    if (ret != 0) {
        IMAGE_LOGE("[FileSourceStream]Go to %{public}d position fail, ret:%{public}d.", offset, ret);
        return nullptr;
    }
    return make_unique<FileSourceStream>(filePtr, length, offset, offset);
}

bool FileSourceStream::Read(uint32_t desiredSize, DataStreamBuffer &outData)
{
    if (desiredSize == 0 || filePtr_ == nullptr) {
        IMAGE_LOGE("[FileSourceStream]read stream input parameter exception.");
        return false;
    }
    if (!GetData(desiredSize, outData)) {
        IMAGE_LOGE("[FileSourceStream]read fail.");
        return false;
    }
    fileOffset_ += outData.dataSize;
    return true;
}

bool FileSourceStream::Peek(uint32_t desiredSize, DataStreamBuffer &outData)
{
    if (desiredSize == 0 || filePtr_ == nullptr) {
        IMAGE_LOGE("[FileSourceStream]peek stream input parameter exception.");
        return false;
    }
    if (!GetData(desiredSize, outData)) {
        IMAGE_LOGE("[FileSourceStream]peek fail.");
        return false;
    }
    int ret = fseek(filePtr_, fileOffset_, SEEK_SET);
    if (ret != 0) {
        IMAGE_LOGE("[FileSourceStream]go to original position fail, ret:%{public}d.", ret);
        return false;
    }
    return true;
}

bool FileSourceStream::Read(uint32_t desiredSize, uint8_t *outBuffer, uint32_t bufferSize, uint32_t &readSize)
{
    if (desiredSize == 0 || outBuffer == nullptr || desiredSize > bufferSize || desiredSize > fileSize_) {
        IMAGE_LOGE("[FileSourceStream]input parameter exception, desiredSize:%{public}u, bufferSize:%{public}u, \
                   fileSize_:%{public}zu.",
                   desiredSize, bufferSize, fileSize_);
        return false;
    }
    if (!GetData(desiredSize, outBuffer, bufferSize, readSize)) {
        IMAGE_LOGE("[FileSourceStream]read fail.");
        return false;
    }
    fileOffset_ += readSize;
    return true;
}

bool FileSourceStream::Peek(uint32_t desiredSize, uint8_t *outBuffer, uint32_t bufferSize, uint32_t &readSize)
{
    if (desiredSize == 0 || outBuffer == nullptr || desiredSize > bufferSize || desiredSize > fileSize_) {
        IMAGE_LOGE("[FileSourceStream]input parameter exception, desiredSize:%{public}u, bufferSize:%{public}u, \
                   fileSize_:%{public}zu.",
                   desiredSize, bufferSize, fileSize_);
        return false;
    }
    if (!GetData(desiredSize, outBuffer, bufferSize, readSize)) {
        IMAGE_LOGE("[FileSourceStream]peek fail.");
        return false;
    }
    int ret = fseek(filePtr_, fileOffset_, SEEK_SET);
    if (ret != 0) {
        IMAGE_LOGE("[FileSourceStream]go to original position fail, ret:%{public}d.", ret);
        return false;
    }
    return true;
}

bool FileSourceStream::Seek(uint32_t position)
{
    if (position > fileSize_) {
        IMAGE_LOGE("[FileSourceStream]Seek the position greater than the file size, position:%{public}u.", position);
        return false;
    }
    size_t targetPosition = position + fileOriginalOffset_;
    fileOffset_ = ((targetPosition < fileSize_) ? targetPosition : fileSize_);
    int ret = fseek(filePtr_, fileOffset_, SEEK_SET);
    if (ret != 0) {
        IMAGE_LOGE("[FileSourceStream]go to offset position fail, ret:%{public}d.", ret);
        return false;
    }
    return true;
}

uint32_t FileSourceStream::Tell()
{
    return fileOffset_ - fileOriginalOffset_;
}

bool FileSourceStream::GetData(uint32_t desiredSize, uint8_t *outBuffer, uint32_t bufferSize, uint32_t &readSize)
{
    if (fileSize_ == fileOffset_) {
        IMAGE_LOGE("[FileSourceStream]read finish, offset:%{public}zu ,dataSize:%{public}zu.", fileOffset_, fileSize_);
        return false;
    }
    if (desiredSize > (fileSize_ - fileOffset_)) {
        desiredSize = fileSize_ - fileOffset_;
    }
    size_t bytesRead = fread(outBuffer, sizeof(outBuffer[0]), desiredSize, filePtr_);
    if (bytesRead < desiredSize) {
        IMAGE_LOGE("[FileSourceStream]read fail, bytesRead:%{public}zu", bytesRead);
        return false;
    }
    readSize = desiredSize;
    return true;
}

bool FileSourceStream::GetData(uint32_t desiredSize, DataStreamBuffer &outData)
{
    if (fileSize_ == fileOffset_) {
        IMAGE_LOGE("[FileSourceStream]read finish, offset:%{public}zu ,dataSize%{public}zu.", fileOffset_, fileSize_);
        return false;
    }

    if (desiredSize == 0 || desiredSize > MALLOC_MAX_LENTH) {
        IMAGE_LOGE("[FileSourceStream]Invalid value, desiredSize out of size.");
        return false;
    }

    ResetReadBuffer();
    readBuffer_ = static_cast<uint8_t *>(malloc(desiredSize));
    if (readBuffer_ == nullptr) {
        IMAGE_LOGE("[FileSourceStream]malloc the desiredSize fail.");
        return false;
    }
    outData.bufferSize = desiredSize;
    if (desiredSize > (fileSize_ - fileOffset_)) {
        desiredSize = fileSize_ - fileOffset_;
    }
    size_t bytesRead = fread(readBuffer_, sizeof(uint8_t), desiredSize, filePtr_);
    if (bytesRead < desiredSize) {
        IMAGE_LOGE("[FileSourceStream]read fail, bytesRead:%{public}zu", bytesRead);
        free(readBuffer_);
        readBuffer_ = nullptr;
        return false;
    }
    outData.inputStreamBuffer = static_cast<uint8_t *>(readBuffer_);
    outData.dataSize = desiredSize;
    return true;
}

size_t FileSourceStream::GetStreamSize()
{
    return fileSize_ - fileOriginalOffset_;
}

static bool DupFd(FILE *f, int &res)
{
    res = fileno(f);
    if (res < 0) {
        IMAGE_LOGE("[FileSourceStream]Fail to fileno fd.");
        return false;
    }
    res = dup(res);
    if (res < 0) {
        IMAGE_LOGE("[FileSourceStream]Fail to dup fd.");
        return false;
    }
    return true;
}

uint8_t *FileSourceStream::GetDataPtr()
{
    if (fileData_ != nullptr) {
        return fileData_;
    }
#ifdef SUPPORT_MMAP
    int fd = -1;
    if (!DupFd(filePtr_, fd)) {
        return nullptr;
    }
    auto mmptr = ::mmap(nullptr, fileSize_, PROT_READ, MAP_SHARED, fd, 0);
    if (mmptr == MAP_FAILED) {
        HiLog::Error(LABEL, "[FileSourceStream] mmap failed, errno:%{public}d", errno);
        ::close(fd);

        // fallback to read entire file into shared memory
        fd = AshmemCreate("FileSourceStream GetDataPtr", fileSize_);
        if (fd < 0) {
            IMAGE_LOGE("[FileSourceStream]GetDataPtr AshmemCreate fd < 0.");
            return nullptr;
        }
        int result = AshmemSetProt(fd, PROT_READ | PROT_WRITE);
        if (result < 0) {
            IMAGE_LOGE("[FileSourceStream]GetDataPtr AshmemSetProt error.");
            ::close(fd);
            return nullptr;
        }
        mmptr = ::mmap(nullptr, fileSize_, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
        if (mmptr == MAP_FAILED || mmptr == nullptr) {
            IMAGE_LOGE("[FileSourceStream]GetDataPtr data is nullptr.");
            ::close(fd);
            return nullptr;
        }
        uint32_t readSize = 0;
        auto oldFileOffset = fileOffset_;
        Seek(0);
        if (!Peek(fileSize_, static_cast<uint8_t*>(mmptr), fileSize_, readSize)) {
            Seek(oldFileOffset);
            IMAGE_LOGE("[FileSourceStream] read file data into mmptr failed.");
            ::munmap(static_cast<uint8_t*>(mmptr), fileSize_);
            ::close(fd);
            return nullptr;
        }
        Seek(oldFileOffset);
    }
    mmapFd_ = fd;
    fileData_ = static_cast<uint8_t*>(mmptr);
#endif
    return fileData_;
}

uint32_t FileSourceStream::GetStreamType()
{
    return ImagePlugin::FILE_STREAM_TYPE;
}

void FileSourceStream::ResetReadBuffer()
{
    if (readBuffer_ != nullptr) {
        free(readBuffer_);
        readBuffer_ = nullptr;
    }
    if (fileData_ != nullptr && mmapFd_ >= 0) {
#ifdef SUPPORT_MMAP
        ::munmap(fileData_, fileSize_);
        ::close(mmapFd_);
        mmapFd_ = -1;
#endif
    }
    fileData_ = nullptr;
}

OutputDataStream* FileSourceStream::ToOutputDataStream()
{
    int dupFd = -1;
    if (DupFd(filePtr_, dupFd)) {
        HiLog::Error(LABEL, "[FileSourceStream] ToOutputDataStream fd failed");
        return nullptr;
    }
    return new (std::nothrow) FilePackerStream(dupFd);
}

bool FileSourceStream::GetMMapFd(int& res)
{
    if (mmapFd_ < 0) {
        res = -1;
        return false;
    }
    res = dup(mmapFd_);
    if (res < 0) {
        IMAGE_LOGE("[FileSourceStream] fail to get mmap fd.");
        return false;
    }
    ::close(mmapFd_);
    mmapFd_ = -1;
    return true;
}
} // namespace Media
} // namespace OHOS
