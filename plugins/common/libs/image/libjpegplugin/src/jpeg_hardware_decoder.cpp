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
#include "jpeg_hardware_decoder.h"
namespace OHOS {
namespace ImagePlugin {
using namespace OHOS::HiviewDFX;
static constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, LOG_TAG_DOMAIN_ID_PLUGIN, "JpegHardwareDecoder" };
JpegHardwareDecoder::JpegHardwareDecoder()
{
    HiLog::Error(LABEL, "JpegHardwareDecoder start.");
}

JpegHardwareDecoder::~JpegHardwareDecoder()
{
    HiLog::Error(LABEL, "JpegHardwareDecoder end.");
}

void JpegHardwareDecoder::GetHardwareInfo()
{
    CodecImageBuffer cib;
    cib.id = 1;
    HiLog::Error(LABEL, "JpegHardwareDecoder GetHardwareInfo.");
}
}// namespace ImagePlugin
} // namespace OHOS