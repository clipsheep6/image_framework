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
    NATIVEEXPORT virtual int Seek(int offset, SeekPos pos) override;
    NATIVEEXPORT virtual ssize_t Tell() override;
    NATIVEEXPORT virtual bool IsEof() override;
    NATIVEEXPORT virtual bool IsOpen() override;
    NATIVEEXPORT virtual void Close() override;
    NATIVEEXPORT virtual bool Open() override;

    /**
     * 获取BufferImageStream的内存映射。
     * 由于BufferImageStream的数据已经在内存中，所以这个函数直接返回数据的指针。
     * 注意，这个函数忽略了isWriteable参数，因为BufferImageStream的数据总是可写的。
     * 
     * @param isWriteable 这个参数被忽略，BufferImageStream的数据总是可写的。
     * @return 返回指向BufferImageStream数据的指针。
     */
    NATIVEEXPORT virtual byte* MMap(bool isWriteable =false) override;

    /**
     * 将源ImageStream的内容传输到当前的BufferImageStream中。
     * 该函数首先清空当前的缓冲区，并将当前的偏移量设置为0。
     * 然后，该函数从源ImageStream中读取数据，并将读取到的数据追加到当前的缓冲区。
     * 如果在读取过程中发生错误，该函数会立即返回，并记录错误信息。
     * 
     * @param src 源ImageStream，该函数会从这个ImageStream中读取数据。
     */
    NATIVEEXPORT virtual void Transfer(ImageStream& src) override;

    // 获取BufferImageStream的大小
    // 返回值: BufferImageStream的大小
    NATIVEEXPORT virtual size_t GetSize() override;
private:
    std::vector<uint8_t> buffer;  // 内存缓冲区
    size_t currentOffset;  // 当前偏移量
};

} // namespace MultimediaPlugin
} // namespace OHOS
#endif // INTERFACES_INNERKITS_INCLUDE_BUFFER_IMAGE_STREAM_H
