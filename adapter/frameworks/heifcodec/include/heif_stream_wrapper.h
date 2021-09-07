/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
 * Copyright (C) 2017 The Android Open Source Project
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

#ifndef HEIF_STREAM_WRAPPER_H
#define HEIF_STREAM_WRAPPER_H

#include "HeifDecoderAPI.h"
#include "input_data_stream.h"
#include "nocopyable.h"

namespace OHOS {
namespace Media {
class HeifStreamWrapper : public HeifStream {
public:
    HeifStreamWrapper() = default;
    explicit HeifStreamWrapper(ImagePlugin::InputDataStream *stream);
    virtual ~HeifStreamWrapper() override{};
    virtual size_t read(void *buffer, size_t size) override;
    virtual bool rewind() override;
    virtual bool seek(size_t position) override;
    virtual bool hasLength() const override;
    virtual size_t getLength() const override;

private:
    DISALLOW_COPY_AND_MOVE(HeifStreamWrapper);
    ImagePlugin::InputDataStream *inputData_ = nullptr;
};
} // namespace Media
} // namespace OHOS

#endif // HEIF_STREAM_WRAPPER_H