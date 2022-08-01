/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

#include "heif_format_agent.h"
#include "hilog/log.h"
#include "heif.h"

namespace OHOS {
namespace ImagePlugin {
using namespace OHOS::HiviewDFX;
using namespace MultimediaPlugin;

const std::string FORMAT_TYPE = "image/heif";
constexpr uint32_t HEADER_SIZE = 12;
static constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, LOG_TAG_DOMAIN_ID_PLUGIN, "HeifFormatAgent" };

std::string HeifFormatAgent::GetFormatType()
{
    return FORMAT_TYPE;
}

uint32_t HeifFormatAgent::GetHeaderSize()
{
    return HEADER_SIZE;
}

bool HeifFormatAgent::CheckFormat(const void *headerData, uint32_t dataSize)
{
    if (headerData == nullptr) {
        HiLog::Error(LABEL, "check format failed: header data is null.");
        return false;
    }
    if (dataSize < HEADER_SIZE) {
        HiLog::Error(LABEL, "data size[%{public}u] less than [%{public}u].", dataSize, HEADER_SIZE);
        return false;
    }
    auto result = heif_check_filetype(static_cast<const uint8_t*>(headerData), dataSize);
    switch (result)
    {
        case heif_filetype_maybe:
            HiLog::Warn(LABEL, "Input file maybe HEIF/AVIF file type, check after.");
            return true;
        case heif_filetype_yes_supported:
            HiLog::Info(LABEL, "Input file is a supported HEIF/AVIF file type.");
            return true;
        case heif_filetype_yes_unsupported:
            HiLog::Error(LABEL, "Input file is an unsupported HEIF/AVIF file type.");
            break;
        case heif_filetype_no:
            HiLog::Error(LABEL, "Input file is not an HEIF/AVIF file.");
            break;
        default:
            HiLog::Error(LABEL, "Unknown error.");
            break;
    }
    return false;
}
} // namespace ImagePlugin
} // namespace OHOS
