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

#ifndef OHOS_IMAGE_PIXELMAP_H
#define OHOS_IMAGE_PIXELMAP_H
#include "image_pixel_map.h"
#include "jkit_utils.h"
#include "pixel_map.h"

namespace OHOS {
namespace Media {
void ohos_media_image_GetImageInfo(JNIEnv *env, jobject pixelMapObject, OhosPixelMapInfo &info);
void *ohos_media_image_AccessPixels(JNIEnv *env, jobject pixelMapObject);
bool ohos_media_image_UnAccessPixels(JNIEnv *env, jobject pixelMapObject);
} // namespace Media
} // namespace OHOS
#endif // OHOS_IMAGE_PIXELMAP_H
