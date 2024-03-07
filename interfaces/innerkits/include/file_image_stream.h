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

#ifndef INTERFACES_INNERKITS_INCLUDE_FILE_IMAGE_STREAM_H
#define INTERFACES_INNERKITS_INCLUDE_FILE_IMAGE_STREAM_H

#include <cstddef>
#include <cstdint>
#include <fstream>
#include <vector>
#include <sys/stat.h>
#include <memory>
#include "image_type.h"
#include "image_stream.h"

namespace OHOS {
namespace Media {

class FileWrapper{
public:
    virtual ~FileWrapper(){}
    virtual size_t fwrite(const void* src, size_t size, size_t nmemb, FILE* f);
    virtual size_t fread(void* destv, size_t size, size_t nmemb, FILE* f);
};

class FileImageStream : public ImageStream {
public:

    enum class FileMode {
        Read,       // Read mode
        ReadWrite   // Read-write mode
    };
    NATIVEEXPORT FileImageStream(FILE *p);
    NATIVEEXPORT FileImageStream(const std::string& filePath);
    FileImageStream(const std::string& filePath, std::unique_ptr<FileWrapper> fileWrapper);
    NATIVEEXPORT virtual ~FileImageStream();

    NATIVEEXPORT virtual ssize_t Write(uint8_t* data, size_t size) override;

    /**
     * Write the content of the source ImageStream to the current FileImageStream.
     * Unlike the CopyFrom function, this function will not modify the offset positions of the source ImageStream and the current FileImageStream.
     * 
     * @param src The source ImageStream, this function will read data from this ImageStream.
     * @return The number of bytes written. If an error occurs, a negative number will be returned.
     */    
    NATIVEEXPORT virtual ssize_t Write(ImageStream& src) override;
    NATIVEEXPORT virtual ssize_t Read(uint8_t* buf, size_t size) override;
    NATIVEEXPORT virtual int ReadByte() override;
    NATIVEEXPORT virtual int Seek(int offset, SeekPos pos) override;
    NATIVEEXPORT virtual ssize_t Tell() override;
    NATIVEEXPORT virtual bool IsEof() override;
    NATIVEEXPORT virtual bool IsOpen() override;
    NATIVEEXPORT virtual void Close() override;
    NATIVEEXPORT virtual bool Open() override;
    NATIVEEXPORT bool Open(FileMode mode);

    /**
     * Create a memory map of the file.
     * If the file is not open, this function will try to open it.
     * If a memory map already exists, this function will delete it first, then create a new memory map.
     * 
     * @param isWriteable If true, the created memory map will be writable; otherwise, the created memory map will be read-only.
     * @return If the memory map is created successfully, return a pointer to the memory map; otherwise, return nullptr.
     */
    NATIVEEXPORT virtual byte* MMap(bool isWriteable =false) override;

    // Release a memory map
    // mmap: The pointer to the memory map that needs to be released.
    // Return: If the memory map is released successfully, return true; otherwise, return false.
    NATIVEEXPORT virtual bool MUnmap(byte* mmap) override;

    /**
     * Transfer the content of the source ImageStream to the current FileImageStream.
     * If the current file is already open, this function will close it first, then open a new file. (There is
     * a side effect, that is, Transfer has a hidden Open behavior, the general FileImageStream initialization
     * needs to explicitly call Open)
     * This function will read data from the source ImageStream and write it to the current file.
     * If an error occurs during the reading or writing process, this function will return immediately and record the error information.
     * After all data is written to the file, this function will truncate the file to the actual written length.
     * 
     * @param src The source ImageStream, this function will read data from this ImageStream.
     */
    NATIVEEXPORT void CopyFrom(ImageStream& src) override;

    // Get the size of the FileImageStream
    // Return value: The size of the FileImageStream
    NATIVEEXPORT size_t GetSize() override;
private:
    FILE *fp;                 // File descriptor
    std::string filePath;   // File path
    size_t fileSize;        // File size
    byte* mappedMemory;     // Address of memory mapping
    std::unique_ptr<FileWrapper> fileWrapper;   // File wrapper class, used for testing
};

} // namespace MultimediaPlugin
} // namespace OHOS
#endif // INTERFACES_INNERKITS_INCLUDE_FILE_IMAGE_STREAM_H
