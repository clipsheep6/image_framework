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

#include "heif_exif_metadata_accessor.h"
#include "heif_error.h"
#include "heif_image.h"
#include "image_log.h"
#include "media_errors.h"
#include "tiff_parser.h"

#undef LOG_DOMAIN
#define LOG_DOMAIN LOG_TAG_DOMAIN_ID_IMAGE

#undef LOG_TAG
#define LOG_TAG "HeifExifMetadataAccessor"

namespace OHOS {
namespace Media {
using namespace ImagePlugin;
const auto EXIF_ID = "Exif\0\0";

HeifExifMetadataAccessor::HeifExifMetadataAccessor(std::shared_ptr<MetadataStream> &stream)
    : AbstractExifMetadataAccessor(stream)
{}

HeifExifMetadataAccessor::~HeifExifMetadataAccessor() {}

uint32_t HeifExifMetadataAccessor::Read()
{
    std::shared_ptr<HeifParser> parser;
    heif_error parseRet = HeifParser::MakeFromMemory(imageStream_->GetAddr(),
        imageStream_->GetSize(), false, &parser);
    if (parseRet != heif_error_ok) {
        return ERR_IMAGE_SOURCE_DATA;
    }

    DataBuf dataBuf;
    GetExifItemData(parser, dataBuf);

    // find the byte order "II 0x2a00" "MM 0x002a"
    size_t byteOrderPos = TiffParser::FindTiffPos(dataBuf.CData(), dataBuf.Size());
    if (byteOrderPos == std::numeric_limits<size_t>::max()) {
        IMAGE_LOGE("Failed to parse Exif metadata: cannot find tiff byte order");
        return ERR_IMAGE_SOURCE_DATA;
    }
    ExifData *exifData = nullptr;
    TiffParser::Decode(dataBuf.CData(byteOrderPos), dataBuf.Size() - byteOrderPos , &exifData);
    if (exifData == nullptr) {
        IMAGE_LOGE("Decode tiffBuf error.");
        return ERR_EXIF_DECODE_FAILED;
    }

    exifMetadata_ = std::make_shared<ExifMetadata>(exifData);

    return SUCCESS;
}

bool HeifExifMetadataAccessor::ReadBlob(DataBuf &blob) const
{
    return false;
}

uint32_t HeifExifMetadataAccessor::Write()
{
    return 0;
}

uint32_t HeifExifMetadataAccessor::WriteBlob(DataBuf &blob)
{
    return 0;
}

bool HeifExifMetadataAccessor::GetExifItemData(std::shared_ptr<HeifParser> &parser, DataBuf &dataBuf)
{
    int32_t exifItemId = 0xffff;
    std::vector<heif_item_id> itemIdList;
    parser->GetAllItemId(itemIdList);
    for (auto id:itemIdList) {
        auto type = parser->GetItemType(id);
        if (type == EXIF_ID) {
           exifItemId = id;
           break;
        }
    }
    if (exifItemId == 0xffff) {
        return false;
    }

    std::vector<uint8_t> item;
    parser->GetItemData(exifItemId, &item);
    dataBuf = DataBuf(item.data(), item.size());
    return true;
}
} // namespace Media
} // namespace OHOS
