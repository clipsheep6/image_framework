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

#ifndef INTERFACES_INNERKITS_INCLUDE_BUFFER_IMAGE_STREAM_H
#define INTERFACES_INNERKITS_INCLUDE_BUFFER_IMAGE_STREAM_H

#include <cstddef>
#include <cstdint>
#include <fstream>
#include <vector>

#include "image_stream.h"
#include "image_type.h"

namespace OHOS {
namespace Media {
/**
 * @class BufferImageStream
 * @brief A class for handling image streams in memory.
 *
 * This class provides methods for reading from and seeking within an image
 * stream in memory. The maximum size of the stream is limited by
 * std::vector<uint8_t>::size_type, which is 4GB. Although ImageStream can
 * address a maximum space of 'long', in memory mode, the maximum space is
 * limited by std::vector<uint8_t>::size_type.
 */
class BufferImageStream : public ImageStream {
public:
    /* *
     * @brief Constructs a new BufferImageStream object.
     */
    NATIVEEXPORT BufferImageStream();

    /* *
     * @brief Destructs the BufferImageStream object.
     */
    NATIVEEXPORT virtual ~BufferImageStream();

    /* *
     * @brief Writes data to the BufferImageStream.
     * @param data The data to be written.
     * @param size The size of the data. On a 32-bit system, the maximum size
     * that can be written at once is 2GB or 4GB.
     * @return The number of bytes written. Returns -1 if an error occurred
     * during writing.
     */
    NATIVEEXPORT virtual ssize_t Write(uint8_t *data, ssize_t size) override;

    /* *
     * @brief Writes the content of the source ImageStream to the current
     * BufferImageStream.
     * @param src The source ImageStream.
     * @return The number of bytes written.
     */
    NATIVEEXPORT virtual ssize_t Write(ImageStream &src) override;

    /* *
     * @brief Reads data from the BufferImageStream.
     * @param buf The buffer to store the data.
     * @param size The size of the data.
     * @return The number of bytes read. Returns -1 if the buffer pointer is null.
     */
    NATIVEEXPORT virtual ssize_t Read(uint8_t *buf, ssize_t size) override;

    /* *
     * @brief Reads a byte from the BufferImageStream.
     * @return The byte read.
     */
    NATIVEEXPORT virtual int ReadByte() override;

    /* *
     * @brief Seeks to a specific position in the image stream.
     * @param offset The offset to seek to. This can be positive or negative.
     * @param pos The position to seek from. This can be the beginning, current position, or end of the stream.
     * @return The new position in the stream. Returns -1 if an invalid seek position is provided.
     */
    NATIVEEXPORT virtual long Seek(long offset, SeekPos pos) override;

    /* *
     * @brief Gets the current position in the BufferImageStream.
     * @return The current position.
     */
    NATIVEEXPORT virtual long Tell() override;

    /* *
     * @brief Checks if the end of the BufferImageStream has been reached.
     * @return true if the end has been reached, false otherwise.
     */
    NATIVEEXPORT virtual bool IsEof() override;

    /* *
     * @brief Checks if the BufferImageStream is open.
     * @return true if it is open, false otherwise.
     */
    NATIVEEXPORT virtual bool IsOpen() override;

    /* *
     * For BufferImageStream, the Open function with a mode is not applicable,
     * as the data for BufferImageStream is already in memory and there are no
     * read-only scenarios.
     *
     * @param mode This parameter is ignored, as there are no read-only
     * scenarios for BufferImageStream.
     * @return Returns false, as this function is not applicable for
     * BufferImageStream.
     */
    NATIVEEXPORT virtual bool Open(OpenMode mode = OpenMode::ReadWrite) override;

    /* *
     * For BufferImageStream, the Flush function is not applicable,
     * as the data for BufferImageStream is already in memory and there are no
     * write operations that need to be flushed.
     *
     * @return Returns true, as this function is not applicable for
     * BufferImageStream, but it is assumed that the data is always "flushed" in
     * memory.
     */
    NATIVEEXPORT virtual bool Flush() override;

    /* *
     * Get the memory address of the BufferImageStream.
     * Since the data of BufferImageStream is stored in a std::vector<uint8_t>,
     * this function directly returns the pointer to the data using the
     * std::vector::data() function. Note that this function ignores the
     * isWriteable parameter, because the data of BufferImageStream is always
     * writable.
     *
     * @param isWriteable This parameter is ignored, the data of
     * BufferImageStream is always writable.
     * @return Returns a pointer to the data of BufferImageStream.
     */
    NATIVEEXPORT virtual byte *GetAddr(bool isWriteable = false) override;

    /* *
     * Transfer the content of the source ImageStream to the current
     * BufferImageStream. This function first clears the current buffer and sets
     * the current offset to 0. Then, this function reads data from the source
     * ImageStream and appends the read data to the current buffer. If an error
     * occurs during the reading process, this function will return immediately
     * and log the error information.
     *
     * @param src The source ImageStream, this function will read data from this
     * ImageStream.
     */
    NATIVEEXPORT virtual bool CopyFrom(ImageStream &src) override;

    /* *
     * Get the size sof the BufferImageStream.
     *
     * @return Returns the size of the BufferImageStream.
     */
    NATIVEEXPORT virtual ssize_t GetSize() override;

private:
    /* *
     * @brief Closes the BufferImageStream.
     */
    virtual void Close() override;

    /* *
     * @brief The memory buffer of the BufferImageStream.
     */
    std::vector<uint8_t> buffer_;

    /* *
     * @brief The current offset in the BufferImageStream.
     */
    long currentOffset_;
};
} // namespace Media
} // namespace OHOS
#endif // INTERFACES_INNERKITS_INCLUDE_BUFFER_IMAGE_STREAM_H
