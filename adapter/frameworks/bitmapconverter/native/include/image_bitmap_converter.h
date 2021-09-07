/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
 * Copyright (C) 2015 The Android Open Source Project
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

#ifndef IMAGE_BITMAP_CONVERTER_H
#define IMAGE_BITMAP_CONVERTER_H

#include <cstdint>
#include "bitmap_wrapper.h"
#include "hilog/log.h"
#include "log_tags.h"
#include "pixel_map.h"
#include "message_parcel.h"

namespace OHOS {
namespace Media {
class ImageBitmapConverter {
public:
    static android::BitmapWrapper *CreateShadowBitmap(PixelMap *pixelMap);
    static PixelMap *CreateShellPixelMap(android::BitmapWrapper *bitmapWrapper);
    static void PixelMapWriteToParcel(PixelMap *pixelMap, int density, MessageParcel *parcel);
    static PixelMap *createFromAlpha(PixelMap *pixelMapSrc);

private:
    static const size_t SIZE_LIMIT = 16 * 1024;
    static constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {
        LOG_CORE, LOG_TAG_DOMAIN_ID_IMAGE, "ImageBitmapConverter"
    };
    static SkColorType GetColorType(PixelFormat pixelFormat);
    static void FreeBitmap(void *addr, void *context);
    static PixelFormat GetPixelFormat(SkColorType colorType);
    static void FreePixelMapFunc(void *addr, void *context, uint32_t size);
    static SkAlphaType ConvertToSkAlphaType(AlphaType alphaType);
    static AlphaType ConvertToAlphaType(SkAlphaType at);
    static void WriteBigData(MessageParcel *p, const void* data, size_t size, bool isMutable);
    static PixelMap *CreateSkShellPixelMap(SkBitmap &skBitmap,
        void *context = nullptr,
        PixelFormat pixelFormat = PixelFormat::UNKNOWN,
        ColorSpace colorSpace = ColorSpace::SRGB,
        bool copy = false);
};
} // namespace Media
} // namespace OHOS

#endif // IMAGE_BITMAP_CONVERTER_H
