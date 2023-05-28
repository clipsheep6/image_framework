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

#ifndef OUTPUT_DATA_STREAM_H
#define OUTPUT_DATA_STREAM_H

#include <cinttypes>

namespace OHOS {
namespace ImagePlugin {
class OutputDataStream {
public:
    virtual ~OutputDataStream() {}
    virtual bool Write(const uint8_t *buffer, uint32_t size) = 0;
    virtual void Flush() {}
    //False means no limit, true needs return act capicity by size
    virtual bool GetCapicity(size_t &size) {
        return false;
    }
    virtual bool GetCurrentSize(size_t &size) {
        return false;
    }
};
} // namespace ImagePlugin
} // namespace OHOS

#endif // OUTPUT_DATA_STREAM_H
