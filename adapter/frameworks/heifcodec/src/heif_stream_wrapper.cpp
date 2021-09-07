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
#include "heif_stream_wrapper.h"
#include "hilog/log.h"
#include "log_tags.h"
#include "securec.h"

namespace OHOS {
namespace Media {
using namespace OHOS::HiviewDFX;
constexpr HiLogLabel LABEL = { LOG_CORE, LOG_TAG_DOMAIN_ID_IMAGE, "HeifStreamWrapper" };

HeifStreamWrapper::HeifStreamWrapper(ImagePlugin::InputDataStream *stream) : inputData_(stream){};
size_t HeifStreamWrapper::read(void *buffer, size_t size)
{
    if (buffer == nullptr) {
        HiLog::Error(LABEL, "input buffer is null.");
        return size;
    }

    uint32_t desireSize = static_cast<uint32_t>(size);
    uint32_t bufferSize = desireSize;
    uint32_t readSize = desireSize;
    if (!inputData_->Read(desireSize, static_cast<uint8_t *>(buffer), bufferSize, readSize)) {
        HiLog::Error(LABEL, "read data failed.");
        return 0;
    }
    return readSize;
}

bool HeifStreamWrapper::rewind()
{
    return inputData_->Seek(0);
}

bool HeifStreamWrapper::seek(size_t position)
{
    return inputData_->Seek(position);
}

bool HeifStreamWrapper::hasLength() const
{
    return inputData_->Tell() != inputData_->GetStreamSize();
}

size_t HeifStreamWrapper::getLength() const
{
    return inputData_->GetStreamSize();
}
} // namespace Media
} // namespace OHOS