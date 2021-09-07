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

#ifndef HW_JPEG_DECOMPRESSOR_INTERFACE_H
#define HW_JPEG_DECOMPRESSOR_INTERFACE_H

#include <cstdint>
#include <cstdio>
#include "abs_image_decoder.h"
#include "jpeglib.h"

namespace OHOS {
namespace ImagePlugin {
class HwJpegDecompressorInterface {
public:
    HwJpegDecompressorInterface() = default;
    virtual ~HwJpegDecompressorInterface() = default;
    virtual bool LockDevice() = 0;
    virtual void UnlockDevice() = 0;
    virtual bool CheckHardwareSupport(const jpeg_decompress_struct *cinfo) = 0;
    virtual uint8_t *GetInputBuffer(size_t bufferSize) = 0;
    virtual bool HardwareDecode(const jpeg_decompress_struct *cinfo, const uint8_t *inputData, size_t bufferSize,
                                ImagePlugin::DecodeContext &context) = 0;
    virtual void ReleaseInputBuffer(uint8_t *addr) = 0;
};
} // namespace ImagePlugin
} // namespace OHOS

#endif // HW_JPEG_DECOMPRESSOR_INTERFACE_H