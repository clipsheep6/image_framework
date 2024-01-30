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

#ifndef FRAMEWORKS_KITS_JS_COMMON_INCLUDE_IMAGE_FORMAT_CONVERT_MDK_KITS_H
#define FRAMEWORKS_KITS_JS_COMMON_INCLUDE_IMAGE_FORMAT_CONVERT_MDK_KITS_H

#include "common_utils.h"
#include "image_format_convert.h"

namespace OHOS {
namespace Media {

#ifdef __cplusplus
extern "C" {
#endif
typedef struct ImageFormatConvertArgs {
    PixelMap *srcPixelMap;
    PixelMap *destPixelMap;
    PixelFormat destPixelFormat;
    ColorSpace colorSpace;
    int32_t srcFormatType;
    int32_t destFormatType;
} ImageFormatCovnertArgs;

enum {
    UNKNOWN,
    YUV_TYPE,
    RGB_TYPE
};

enum {
    CTX_FUNC_IMAGE_CONVERT_CREATE,
    CTX_FUNC_IMAGE_CONVERT_EXEC,
    CTX_FUNC_IMAGE_CONVERT_RELEASE
};

int32_t ImageConvertNativeCall(int32_t mode, ImageFormatConvertArgs *args);

#ifdef __cplusplus
};
#endif
} // namespace Media
} // namespace OHOS

#endif // FRAMEWORKS_KITS_JS_COMMON_INCLUDE_IMAGE_FORMAT_CONVERT_MDK_KITS_H