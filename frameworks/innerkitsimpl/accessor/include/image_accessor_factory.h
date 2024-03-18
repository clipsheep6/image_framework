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

#ifndef FRAMEWORKS_INNERKITSIMPL_ACCESSOR_INCLUDE_IMAGE_ACCESSOR_FACTORY
#define FRAMEWORKS_INNERKITSIMPL_ACCESSOR_INCLUDE_IMAGE_ACCESSOR_FACTORY

#include <memory>
#include <string>

#include "image_accessor_interface.h"
#include "image_stream.h"

namespace OHOS {
namespace Media {
class ImageAccessorFactory {
public:
    static std::shared_ptr<ImageAccessorInterface> CreateImageAccessor(uint8_t *buffer, const uint32_t size);
    static std::shared_ptr<ImageAccessorInterface> CreateImageAccessor(const int fd);
    static std::shared_ptr<ImageAccessorInterface> CreateImageAccessor(const std::string &path);

private:
    static std::shared_ptr<ImageAccessorInterface> CreateImageAccessor(std::shared_ptr<ImageStream> &stream);
    static EncodedFormat GetImageType(std::shared_ptr<ImageStream> &stream);
};
} // namespace Media
} // namespace OHOS

#endif
