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
#include "image_type.h"
#include "image_stream.h"

namespace OHOS {
namespace Media {

class FileImageStream : public ImageStream {
public:

    enum class FileMode {
        Read,       // 只读模式
        ReadWrite   // 读写模式
    };

    NATIVEEXPORT FileImageStream(const std::string& filePath);
    NATIVEEXPORT virtual ~FileImageStream();

    NATIVEEXPORT virtual ssize_t Write(uint8_t* data, size_t size) override;
    NATIVEEXPORT virtual ssize_t Write(ImageStream& src) override;
    NATIVEEXPORT virtual ssize_t Read(uint8_t* buf, size_t size) override;
    NATIVEEXPORT virtual int Seek(int offset, SeekPos pos) override;
    NATIVEEXPORT virtual ssize_t Tell() override;
    NATIVEEXPORT virtual bool IsEof() override;
    NATIVEEXPORT virtual bool IsOpen() override;
    NATIVEEXPORT virtual void Close() override;
    NATIVEEXPORT virtual bool Open() override;
    NATIVEEXPORT bool Open(FileMode mode);

    /**
     * 创建文件的内存映射。
     * 如果文件没有打开，该函数会尝试打开它。
     * 如果已经存在一个内存映射，该函数会先删除它，然后创建新的内存映射。
     * 
     * @param isWriteable 如果为true，创建的内存映射将是可写的；否则，创建的内存映射将是只读的。
     * @return 如果内存映射创建成功，返回指向内存映射的指针；否则，返回nullptr。
     */
    NATIVEEXPORT virtual byte* MMap(bool isWriteable =false) override;

    /**
     * 将源ImageStream的内容传输到当前的FileImageStream中。
     * 如果当前的文件已经打开，该函数会先关闭它，然后打开新的文件。
     * 该函数会从源ImageStream中读取数据，并写入到当前的文件中。
     * 如果在读取或写入过程中发生错误，该函数会立即返回，并记录错误信息。
     * 在所有数据都写入到文件后，该函数会截断文件到实际写入的长度。
     * 
     * @param src 源ImageStream，该函数会从这个ImageStream中读取数据。
     */
    NATIVEEXPORT void Transfer(ImageStream& src) override;

    // 获取FileImageStream的大小
    // 返回值: FileImageStream的大小
    NATIVEEXPORT size_t GetSize() override;
private:
    int fd;  // 文件描述符
    std::string filePath;  // 文件路径
    size_t fileSize;  // 文件大小
    size_t currentOffset;  // 当前偏移量
    byte* mappedMemory;  // 内存映射的地址
};

} // namespace MultimediaPlugin
} // namespace OHOS
#endif // INTERFACES_INNERKITS_INCLUDE_FILE_IMAGE_STREAM_H
