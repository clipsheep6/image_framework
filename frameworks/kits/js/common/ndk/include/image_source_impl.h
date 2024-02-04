/*
 * Copyright (c) 2023-2023 Huawei Device Co., Ltd.
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

#ifndef FRAMEWORKS_KITS_JS_COMMON_INCLUDE_IMAGE_SOURCE_IMPL_H
#define FRAMEWORKS_KITS_JS_COMMON_INCLUDE_IMAGE_SOURCE_IMPL_H

#include "raw_file.h"
#include "image_source.h"

namespace OHOS {
namespace Media {
struct ImageSourceImpl {
    /** Pointer to the image source URI. Only a file URI or Base64 URI is accepted. */
    char* uri = nullptr;
    /** Length of the image source URI. */
    size_t uriSize = 0;
    /** Descriptor of the image source. */
    int32_t fd = -1;
    /** Pointer to the image source buffer. Only a formatted packet buffer or Base64 buffer is accepted. */
    uint8_t* buffer = nullptr;
    /** Size of the image source buffer. */
    size_t bufferSize = 0;
};
} // namespace Media
} // namespace OHOS
struct ImageSourceCapi {
public:
    ImageSourceCapi(char* uri, size_t size, OHOS::Media::SourceOptions ops);   
    ImageSourceCapi(int32_t fd, OHOS::Media::SourceOptions ops);
    ImageSourceCapi(uint8_t* data, size_t dataSize, OHOS::Media::SourceOptions ops);
    ImageSourceCapi(RawFileDescriptor rawFile, OHOS::Media::SourceOptions ops);
    ImageSourceCapi(OHOS::Media::ImageSourceImpl* source, OHOS::Media::IncrementalSourceOptions incOpts);
    ~ImageSourceCapi();
    std::shared_ptr<OHOS::Media::ImageSource> GetInnerImageSource();
private:
    std::string UrlToPath(const std::string &path);
    std::shared_ptr<OHOS::Media::ImageSource> innerImageSource_;
};

#endif // FRAMEWORKS_KITS_JS_COMMON_INCLUDE_IMAGE_SOURCE_IMPL_H