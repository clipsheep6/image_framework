/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "imageframeworkimagesource_fuzzer.h"

#include <cstdint>
#include <string>

#include "image_source.h"

namespace OHOS {
namespace Media {
static inline std::string FuzzString(const uint8_t *data, size_t size)
{
    return {reinterpret_cast<const char*>(data), size};
}

static inline int FuzzInt(const uint8_t *data, size_t size)
{
    return static_cast<int>(*data);
}

void CreatePixelMapFuzzer(const uint8_t* data, size_t size)
{
    uint32_t errCode = 0;
    Media::SourceOptions opts;
    std::unique_ptr<ImageSource> imageSource = ImageSource::CreateImageSource(data, size, opts, errCode);
    Media::DecodeOptions dopts;
    uint32_t errorCode;
    uint32_t index = 1;
    imageSource->CreateIncrementalPixelMap(index, dopts, errorCode);
}

void CreateImageSourceFuzzer(const uint8_t* data, size_t size)
{
    Media::SourceOptions opts;
    uint32_t errorCode;
    uint32_t offset = 0;
    uint32_t length = 1;
    Media::ImageSource::CreateImageSource(data, size, opts, errorCode);
    Media::ImageSource::CreateImageSource(FuzzString(data, size), opts, errorCode);
    Media::ImageSource::CreateImageSource(FuzzInt(data, size), opts, errorCode);
    Media::ImageSource::CreateImageSource(FuzzInt(data, size), offset, length, opts, errorCode);
}

} // namespace Media
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    /* Run your code on data */
    OHOS::Media::CreatePixelMapFuzzer(data, size);
    OHOS::Media::CreateImageSourceFuzzer(data, size);
    return 0;
}