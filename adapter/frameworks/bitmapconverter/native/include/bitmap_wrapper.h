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
#ifndef BITMAP_WRAPPER_H
#define BITMAP_WRAPPER_H

#include "Bitmap.h"
#include "SkBitmap.h"

namespace android {
class BitmapWrapper {
public:
    BitmapWrapper(android::Bitmap *bitmap) : mBitmap(bitmap) {}
    ~BitmapWrapper() {}
    void freePixels();
    bool valid() const;
    android::Bitmap &bitmap();
    void assertValid() const;
    void getSkBitmap(SkBitmap *outBitmap) const;
    bool hasHardwareMipMap();
    void setHasHardwareMipMap(bool hasMipMap);
    void setAlphaType(SkAlphaType alphaType);
    const SkImageInfo &info();
    size_t getAllocationByteCount() const;
    size_t rowBytes() const;
    uint32_t getGenerationID() const;
    bool isHardware();

private:
    sk_sp<android::Bitmap> mBitmap = nullptr;
    SkImageInfo mInfo;
    bool mHasHardwareMipMap = false;
    size_t mAllocationSize = 0;
    size_t mRowBytes = 0;
    uint32_t mGenerationId = 0;
    bool mIsHardware = false;
};
} // namespace android
#endif // BITMAP_WRAPPER_H
