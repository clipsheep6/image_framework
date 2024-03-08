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

#ifndef INTERFACES_INNERKITS_INCLUDE_BUFFER_IMAGE_STREAM_H
#define INTERFACES_INNERKITS_INCLUDE_BUFFER_IMAGE_STREAM_H

#include <cstddef>
#include <cstdint>
#include <fstream>
#include <vector>
#include "image_type.h"
#include "image_stream.h"

namespace OHOS {
namespace Media {

class BufferImageStream : public ImageStream {
public:
    NATIVEEXPORT BufferImageStream();
    NATIVEEXPORT virtual ~BufferImageStream();

    NATIVEEXPORT virtual ssize_t Write(uint8_t* data, size_t size) override;
    NATIVEEXPORT virtual ssize_t Write(ImageStream& src) override;
    NATIVEEXPORT virtual ssize_t Read(uint8_t* buf, size_t size) override;
    NATIVEEXPORT virtual int ReadByte() override;
    NATIVEEXPORT virtual int Seek(int offset, SeekPos pos) override;
    NATIVEEXPORT virtual ssize_t Tell() override;
    NATIVEEXPORT virtual bool IsEof() override;
    NATIVEEXPORT virtual bool IsOpen() override;
    NATIVEEXPORT virtual void Close() override;
    NATIVEEXPORT virtual bool Open() override;

    /**
     * For BufferImageStream, the Open function with a mode is not applicable, 
     * as the data for BufferImageStream is already in memory and there are no read-only scenarios.
     * 
     * @param mode This parameter is ignored, as there are no read-only scenarios for BufferImageStream.
     * @return Returns false, as this function is not applicable for BufferImageStream.
     */
    NATIVEEXPORT virtual bool Open(OpenMode mode) override;

    /**
     * Get the memory map of BufferImageStream.
     * Since the data of BufferImageStream is already in memory, this function directly returns the pointer to the data.
     * Note that this function ignores the isWriteable parameter, because the data of BufferImageStream is always writable.
     * 
     * @param isWriteable This parameter is ignored, the data of BufferImageStream is always writable.
     * @return Returns a pointer to the data of BufferImageStream.
     */
    NATIVEEXPORT virtual byte* MMap(bool isWriteable =false) override;

    /**
     * Release a memory map.
     * 
     * @param mmap The pointer to the memory map that needs to be released.
     * @return Returns true if the memory map is released successfully; otherwise, returns false.
     */
    NATIVEEXPORT virtual bool MUnmap(byte* mmap) override;

    /**
     * Transfer the content of the source ImageStream to the current BufferImageStream.
     * This function first clears the current buffer and sets the current offset to 0.
     * Then, this function reads data from the source ImageStream and appends the read data to the current buffer.
     * If an error occurs during the reading process, this function will return immediately and log the error information.
     * 
     * @param src The source ImageStream, this function will read data from this ImageStream.
     */
    NATIVEEXPORT virtual void CopyFrom(ImageStream& src) override;

    /**
     * Get the size sof the BufferImageStream.
     * 
     * @return Returns the size of the BufferImageStream.
     */
    NATIVEEXPORT virtual size_t GetSize() override;
private:
    std::vector<uint8_t> buffer;  // Memory buffer
    size_t currentOffset;  // Current offset
};

} // namespace MultimediaPlugin
} // namespace OHOS
#endif // INTERFACES_INNERKITS_INCLUDE_BUFFER_IMAGE_STREAM_H
