/*
 * Copyright (C) 2023 Huawei Device Co., Ltd.
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

#ifndef FRAMEWORKS_INNERKITSIMPL_UTILS_INCLUDE_IMAGE_TYPE_CONVERTER_H
#define FRAMEWORKS_INNERKITSIMPL_UTILS_INCLUDE_IMAGE_TYPE_CONVERTER_H
#include <string>

#include "image_type.h"
#include "include/core/SkImageInfo.h"
namespace OHOS {
namespace Media {
class ImageTypeConverter {
public:
    static SkColorType ToSkColorType(const PixelFormat pixelFormat);
    static SkAlphaType ToSkAlphaType(const AlphaType alphaType);
    static PixelFormat ToPixelFormat(const SkColorType type);
    static AlphaType ToAlphaType(const SkAlphaType type);

    static const std::string ToName(const PixelFormat pixelFormat);
    static const std::string ToName(const AlphaType alphaType);
    static const std::string ToName(const SkColorType type);
    static const std::string ToName(const SkAlphaType type);
};
} // namespace Media
} // namespace OHOS
#endif // FRAMEWORKS_INNERKITSIMPL_UTILS_INCLUDE_IMAGE_TYPE_CONVERTER_H
