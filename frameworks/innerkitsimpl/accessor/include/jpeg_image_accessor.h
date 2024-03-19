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

#ifndef FRAMEWORKS_INNERKITSIMPL_ACCESSOR_INCLUDE_JPEG_IMAGE_ACCESSOR_H
#define FRAMEWORKS_INNERKITSIMPL_ACCESSOR_INCLUDE_JPEG_IMAGE_ACCESSOR_H

#include <tuple>

#include "abstract_image_accessor.h"
#include "buffer_image_stream.h"
#include "data_buf.h"
#include "image_stream.h"

namespace OHOS {
namespace Media {
class JpegImageAccessor : public AbstractImageAccessor {
public:
    JpegImageAccessor(std::shared_ptr<ImageStream> &stream);
    ~JpegImageAccessor();

    virtual uint32_t ReadMetadata() override;
    virtual bool ReadExifBlob(DataBuf &blob) const override;
    virtual uint32_t WriteMetadata() override;
    virtual uint32_t WriteExifBlob(DataBuf &blob) override;

private:
    int FindNextMarker() const;
    std::pair<std::array<byte, 2>, uint16_t> ReadSegmentLength(uint8_t marker) const;
    DataBuf ReadNextSegment(byte marker);
    bool GetExifEncodeBlob(uint8_t **dataBlob, uint32_t &size);
    bool GetExifBlob(const DataBuf &blob, uint8_t **dataBlob, uint32_t &size);
    bool WriteHeader(BufferImageStream &tempStream);
    std::tuple<size_t, size_t> GetInsertPosAndMarkerAPP1();
    bool WriteSegment(BufferImageStream &bufStream, uint8_t marker, const DataBuf &buf);
    bool WriteTail(BufferImageStream &bufStream);
    bool CopyRestData(BufferImageStream &bufStream);
    bool WriteData(BufferImageStream &bufStream, uint8_t *dataBlob, uint32_t size);
    bool UpdateExifMetadata(BufferImageStream &tempStream, uint8_t *dataBlob, uint32_t size);
    uint32_t UpdateData(uint8_t *dataBlob, uint32_t size);
};
} // namespace Media
} // namespace OHOS

#endif