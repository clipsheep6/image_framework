/*
* Copyright (C) 2022 Huawei Device Co., Ltd.
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
#include "object_info.h"
#include "media_log.h"
#include "mtp_packet_tools.h"
using namespace std;
namespace OHOS {
namespace Media {
ObjectInfo::ObjectInfo(uint32_t h) : handle(h)
{
}

ObjectInfo::~ObjectInfo()
{
}

void ObjectInfo::Dump()
{
    MEDIA_DEBUG_LOG("=== ObjectInfo {handle=%{public}x, name=[%{public}s]} begin ===", handle, name.c_str());

    MEDIA_DEBUG_LOG("    storageID=%{public}x, format=%{public}s(%{public}x), parent=%{public}x",
        storageID, MtpPacketTool::GetFormatName(format).c_str(), format, parent);

    MEDIA_DEBUG_LOG("    association={type=%{public}s(%{public}x), desc=%{public}x}",
        MtpPacketTool::GetAssociationName(associationType).c_str(), associationType, associationDesc);

    std::string dateCreatedStr = MtpPacketTool::FormatDateTime(dateCreated);
    std::string dateModifiedStr = MtpPacketTool::FormatDateTime(dateModified);
    MEDIA_DEBUG_LOG("    sequenceNumber=%{public}u,"
        " date={create=%{public}s(%{public}lld), modify=%{public}s(%{public}lld)}",
        sequenceNumber, dateCreatedStr.c_str(), dateCreated, dateModifiedStr.c_str(), dateModified);

    MEDIA_DEBUG_LOG("    protectionStatus=%{public}u, compressedSize=%{public}u, size=%{public}u",
        protectionStatus, compressedSize, size);

    MEDIA_DEBUG_LOG("    image={pixWidth=%{public}u, pixHeight=%{public}u, pixDepth=%{public}u}",
        imagePixWidth, imagePixHeight, imagePixDepth);

    MEDIA_DEBUG_LOG("    thumb={format=%{public}s(%{public}x), compressedSize=%{public}u,"
        " pixWidth=%{public}u, pixHeight=%{public}u}",
        MtpPacketTool::GetFormatName(thumbFormat).c_str(), thumbFormat, thumbCompressedSize,
        thumbPixWidth, thumbPixHeight);

    MEDIA_DEBUG_LOG("    keywords=[%{public}s]", keywords.c_str());
    MEDIA_DEBUG_LOG("--- ObjectInfo end ---");
}
} // namespace Media
} // namespace OHOS