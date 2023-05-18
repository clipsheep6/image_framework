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

#ifndef JPEG_HARDWARE_DECODER_H
#define JPEG_HARDWARE_DECODER_H

#include "v1_0/icodec_image_jpeg.h"
#include "hilog/log.h"
#include "log_tags.h"
namespace OHOS {
namespace ImagePlugin {
using namespace HDI::Codec::Image::V1_0;
class JpegHardwareDecoder {
public:
    JpegHardwareDecoder();
    ~JpegHardwareDecoder();
    void GetHardwareInfo(); 
};
}// namespace ImagePlugin
} // namespace OHOS

#endif // JPEG_HARDWARE_DECODER_H