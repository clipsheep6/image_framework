/*
 * Copyright (C) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
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
#include <memory>
#include <sys/stat.h>
#include <vector>

#include "image_stream.h"
#include "image_type.h"

namespace OHOS {
namespace Media {
class FileWrapper {
public:
    virtual ~FileWrapper() {}
    virtual size_t FWrite(const void *src, size_t size, size_t nmemb, FILE *file);
    virtual size_t FRead(void *destv, size_t size, size_t nmemb, FILE *file);
};

class FileImageStream : public ImageStream {
public:
    /* *
     * @brief Constructs a new FileImageStream object from a file descriptor.
     * @param fileDescriptor The file descriptor.
     */
    NATIVEEXPORT FileImageStream(int fileDescriptor);

    /* *
     * @brief Constructs a new FileImageStream object from a file path.
     * @param filePath The file path.
     */
    NATIVEEXPORT FileImageStream(const std::string &filePath);

    /* *
     * @brief Constructs a new FileImageStream object from a FILE pointer.
     * @param filePointer The FILE pointer.
     */
    NATIVEEXPORT FileImageStream(FILE *filePointer);

    /* *
     * @brief Destructs the FileImageStream object.
     */
    NATIVEEXPORT virtual ~FileImageStream();

    /* *
     * @brief Writes data to the FileImageStream.
     * @param data The data to be written.
     * @param size The size of the data.
     * @return The number of bytes written.
     */
    NATIVEEXPORT virtual ssize_t Write(byte *data, size_t size) override;

    /* *
     * @brief Writes the content of the source ImageStream to the current
     * FileImageStream.
     * @param src The source ImageStream.
     * @return The number of bytes written.
     */
    NATIVEEXPORT virtual ssize_t Write(ImageStream &src) override;

    /* *
     * @brief Reads data from the FileImageStream.
     * @param buf The buffer to store the data.
     * @param size The size of the data.
     * @return The number of bytes read.
     */
    NATIVEEXPORT virtual ssize_t Read(byte *buf, size_t size) override;

    /* *
     * @brief Reads a byte from the FileImageStream.
     * @return The byte read.
     */
    NATIVEEXPORT virtual int ReadByte() override;

    /* *
     * @brief Seeks to a specific position in the FileImageStream.
     * @param offset The offset.
     * @param pos The starting position of the offset.
     * @return The new position.
     */
    NATIVEEXPORT virtual long Seek(int offset, SeekPos pos) override;

    /* *
     * @brief Gets the current position in the FileImageStream.
     * @return The current position.
     */
    NATIVEEXPORT virtual long Tell() override;

    /* *
     * @brief Checks if the end of the FileImageStream has been reached.
     * @return true if the end has been reached, false otherwise.
     */
    NATIVEEXPORT virtual bool IsEof() override;

    /* *
     * @brief Checks if the FileImageStream is open.
     * @return true if it is open, false otherwise.
     */
    NATIVEEXPORT virtual bool IsOpen() override;

    /* *
     * @brief Opens the FileImageStream. The default mode is "rb".
     * @return true if it opens successfully, false otherwise.
     */
    NATIVEEXPORT virtual bool Open() override;

    /* *
     * @brief Opens the FileImageStream with a specific mode.
     * The Open operation will reset the read and write position of the
     * file
     * @param mode The mode to open the FileImageStream.
     * @return true if it opens successfully, false otherwise.
     */
    NATIVEEXPORT virtual bool Open(OpenMode mode) override;

    /* *
     * @brief Flushes the FileImageStream.
     * @return true if it flushes successfully, false otherwise.
     */
    NATIVEEXPORT virtual bool Flush() override;

    /* *
     * @brief Creates a memory map of the file.
     * @param isWriteable If true, the created memory map will be writable;
     * otherwise, it will be read-only.
     * @return A pointer to the memory map if it is created successfully,
     * nullptr otherwise.
     */
    NATIVEEXPORT virtual byte *GetAddr(bool isWriteable = false) override;

    /* *
     * @brief Should call Open first.Transfers the content of the source
     * ImageStream to the current FileImageStream.
     * @param src The source ImageStream.
     */
    NATIVEEXPORT bool CopyFrom(ImageStream &src) override;

    /* *
     * @brief Gets the size of the FileImageStream.
     * @return The size of the FileImageStream.
     */
    NATIVEEXPORT size_t GetSize() override;

private:
    /* *
     * @brief Closes the FileImageStream.
     */
    virtual void Close() override;

    /* *
     * @brief Releases a memory map.
     * @param mmap The pointer to the memory map that needs to be released.
     * @return true if the memory map is released successfully, false otherwise.
     */
    bool ReleaseAddr();

    /* *
     * @brief Constructs a new FileImageStream object from a file path and a
     * file wrapper.
     * @param filePath The file path.
     * @param fileWrapper The file wrapper.
     */
    FileImageStream(const std::string &filePath, std::unique_ptr<FileWrapper> fileWrapper);

    /* *
     * @brief Opens the FileImageStream from a file descriptor.
     * @param modeStr The mode string.
     * @return true if it opens successfully, false otherwise.
     */
    bool OpenFromFD(const char *modeStr);

    /* *
     * @brief Opens the FileImageStream from a file path.
     * @param modeStr The mode string.
     * @return true if it opens successfully, false otherwise.
     */
    bool OpenFromPath(const char *modeStr);

    /* *
     * @brief Copies data from the source ImageStream to the current file.
     * @param src The source ImageStream.
     * @param tempBuffer The temporary buffer to store the data.
     * @param buffer_size The size of the temporary buffer.
     * @param totalBytesWritten The total number of bytes written to the current file.
     * @return true if the data is copied successfully, false otherwise.
     */
    bool CopyDataFromSource(ImageStream &src, byte *tempBuffer, size_t buffer_size, size_t &totalBytesWritten);

    /* *
     * @brief Handles the error when writing data to the current file fails.
     * @param src The source ImageStream.
     * @param src_cur The current position of the source ImageStream.
     */
    void HandleWriteError(ImageStream &src, ssize_t src_cur);

    /* *
     * @brief Flushes the current file.
     * @param src The source ImageStream.
     * @param src_cur The current position of the source ImageStream.
     * @return true if the file is flushed successfully, false otherwise.
     */
    bool FlushFile(ImageStream &src, ssize_t src_cur);

    /* *
     * @brief Truncates the current file to the specified size.
     * @param totalBytesWritten The new size of the file.
     * @param src The source ImageStream.
     * @param src_cur The current position of the source ImageStream.
     * @return true if the file is truncated successfully, false otherwise.
     */
    bool TruncateFile(size_t totalBytesWritten, ImageStream &src, ssize_t src_cur);

    FILE *fp_;                                 // File descriptor
    int dupFD_;                                // Duplicated file descriptor
    std::string filePath_;                     // File path
    size_t fileSize_;                          // File size
    void *mappedMemory_;                       // Address of memory mapping
    std::unique_ptr<FileWrapper> fileWrapper_; // File wrapper class, used for testing

    enum {
        INIT_FROM_FD,
        INIT_FROM_PATH,
        INIT_FROM_UNKNOWN,
    } initPath_;
};
} // namespace Media
} // namespace OHOS
#endif // INTERFACES_INNERKITS_INCLUDE_FILE_IMAGE_STREAM_H
