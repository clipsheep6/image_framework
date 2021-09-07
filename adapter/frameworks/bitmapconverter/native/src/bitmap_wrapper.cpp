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
#include "bitmap_wrapper.h"

namespace android {
void BitmapWrapper::freePixels()
{
    if (!valid()) {
        ALOGE("BitmapWrapper freePixels already done, so we do nothing");
        return;
    }
    ALOGE("BitmapWrapper freePixels");
}

bool BitmapWrapper::valid() const
{
    return mBitmap != nullptr;
}

android::Bitmap &BitmapWrapper::bitmap()
{
    assertValid();
    return *mBitmap;
}

void BitmapWrapper::assertValid() const
{
    LOG_ALWAYS_FATAL_IF(!valid(), "Error, cannot access an invalid/free'd bitmap here!");
}

void BitmapWrapper::getSkBitmap(SkBitmap *outBitmap) const
{
    assertValid();
    mBitmap->getSkBitmap(outBitmap);
}

bool BitmapWrapper::hasHardwareMipMap()
{
    if (mBitmap) {
        return mBitmap->hasHardwareMipMap();
    }
    return mHasHardwareMipMap;
}

void BitmapWrapper::setHasHardwareMipMap(bool hasMipMap)
{
    assertValid();
    mBitmap->setHasHardwareMipMap(hasMipMap);
}

void BitmapWrapper::setAlphaType(SkAlphaType alphaType)
{
    assertValid();
    mBitmap->setAlphaType(alphaType);
}

const SkImageInfo &BitmapWrapper::info()
{
    if (mBitmap) {
        return mBitmap->info();
    }
    return mInfo;
}

size_t BitmapWrapper::getAllocationByteCount() const
{
    if (mBitmap) {
        return mBitmap->getAllocationByteCount();
    }
    return mAllocationSize;
}

size_t BitmapWrapper::rowBytes() const
{
    if (mBitmap) {
        return mBitmap->rowBytes();
    }
    return mRowBytes;
}

uint32_t BitmapWrapper::getGenerationID() const
{
    if (mBitmap) {
        return mBitmap->getGenerationID();
    }
    return mGenerationId;
}

bool BitmapWrapper::isHardware()
{
    if (mBitmap) {
        return mBitmap->isHardware();
    }
    return mIsHardware;
}
} // namespace android