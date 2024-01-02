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

#include "nine_chunk_reader.h"
#include "hilog/log.h"
#include "log_tags.h"
#include "memory.h"
#include "securec.h"
#include "png_ninepatch_res.h"

namespace OHOS {
namespace Media {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, LOG_TAG_DOMAIN_ID_IMAGE, "NineChunkReader" };
const std::string CHUNK_NAME = "npTc";
NineChunkReader::NineChunkReader() {}

bool NineChunkReader::readChunk(const char tag[], const void *data, size_t length)
{
    if (tag == CHUNK_NAME) {
        const ImagePlugin::PngNinePatchRes *patch = static_cast<const ImagePlugin::PngNinePatchRes *>(data);
        size_t patchSize = patch->SerializedSize();
        if (length != patchSize) {
            HiviewDFX::HiLog::Error(LABEL, "length(%{public}zu) ne patchSize(%{public}zu)", length, patchSize);
            return false;
        }
        if (patchSize <= 0) {
            HiviewDFX::HiLog::Error(LABEL, "patchSize is failed (%{public}zu)", patchSize);
            return false;
        }
        auto *patchNew = new (std::nothrow) uint8_t[patchSize];

        if (patchNew == nullptr) {
            HiviewDFX::HiLog::Error(LABEL, "malloc failed");
            return false;
        }
        errno_t err = memcpy_s(patchNew, patchSize, patch, patchSize);
        if (err != EOK) {
            HiviewDFX::HiLog::Error(LABEL, "memcpy failed. errno:%{public}d", err);
            delete[] (patchNew);
            patchNew = nullptr;
            return false;
        }
        ImagePlugin::PngNinePatchRes::Deserialize(patchNew)->FileToDevice();
        if (nineData_ != nullptr) {
            free(nineData_);
        }
        nineData_ = patchNew;
        nineLength_ = length;
    }
    return true;
}

}
}