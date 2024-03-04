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

#ifndef INTERFACES_INNERKITS_INCLUDE_IMAGE_STREAM_H
#define INTERFACES_INNERKITS_INCLUDE_IMAGE_STREAM_H

#include <cstddef>
#include <cstdint>
#include <fstream>
#include <vector>
#include "image_type.h"

namespace OHOS {
namespace Media {

enum SeekPos { BEGIN, CURRENT, END };

typedef uint8_t byte;

class ImageStream {
public:
    virtual ~ImageStream() {}

    // 写入数据到图像流中
    // data: 要写入的数据
    // size: 要写入的数据的大小
    // 返回值: 实际写入的数据的大小
    virtual ssize_t Write(uint8_t* data, size_t size) = 0;

    // 从另一个图像流中写入数据
    // src: 数据来源的图像流
    // 返回值: 实际写入的数据的大小
    virtual ssize_t Write(ImageStream& src) = 0;

    // 从图像流中读取数据
    // buf: 用于存储读取的数据的缓冲区
    // size: 要读取的数据的大小
    // 返回值: 实际读取的数据的大小
    virtual ssize_t Read(uint8_t* buf, size_t size) = 0;

    // 在图像流中寻找特定的位置
    // offset: 偏移量
    // pos: 偏移的起始位置（从头部，当前位置，或尾部开始）
    // 返回值: 新的位置
    virtual int Seek(int offset, SeekPos pos) = 0;

    // 获取当前在图像流中的位置
    // 返回值: 当前的位置
    virtual ssize_t Tell() = 0;

    // 检查是否已经到达图像流的末尾
    // 返回值: 如果已经到达末尾，则返回true，否则返回false
    virtual bool IsEof() = 0;

    // 检查图像流是否已经打开
    // 返回值: 如果已经打开，则返回true，否则返回false
    virtual bool IsOpen() = 0;

    // 关闭图像流
    virtual void Close() = 0;

    // 打开图像流
    // 返回值: 如果打开成功，则返回true，否则返回false
    virtual bool Open() = 0;

    // 创建内存映射
    // isWriteable: 如果为true，创建的内存映射将是可写的；否则，创建的内存映射将是只读的。
    // 返回值: 如果内存映射创建成功，返回指向内存映射的指针；否则，返回nullptr。
    virtual byte* MMap(bool isWriteable =false) =0;

    // 将源ImageStream的内容传输到当前的ImageStream中。
    // src: 源ImageStream，该函数会从这个ImageStream中读取数据。
    virtual void Transfer(ImageStream& src) = 0;

    // 获取ImageStream的大小
    // 返回值: ImageStream的大小
    virtual size_t GetSize() = 0;
};

} // namespace MultimediaPlugin
} // namespace OHOS
#endif // IMAGE_STREAM_H
