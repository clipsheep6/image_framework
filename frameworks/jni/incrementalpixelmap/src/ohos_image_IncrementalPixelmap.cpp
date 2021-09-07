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

#include <unistd.h>
#include <mutex>
#include "hilog/log.h"
#include "image_type.h"
#include "incremental_pixel_map.h"
#include "jkit_utils.h"
#include "log_tags.h"
#include "media_errors.h"
#include "pixel_map.h"
#if !defined(_WIN32) && !defined(_APPLE)
#include "parcel.h"
#endif
#ifndef _WIN32
#include "securec.h"
#endif

using namespace OHOS::HiviewDFX;
using namespace OHOS::Media;

static constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, LOG_TAG_DOMAIN_ID_IMAGE,
                                                       "IncrementalPixelMap_JNI" };

jint ohos_media_image_IncrementalPixelMap_nativePromoteDecoding(JNIEnv *env, jclass thiz, jlong nativePtr)
{
    IncrementalPixelMap *incrementalPixelMap = reinterpret_cast<IncrementalPixelMap *>(nativePtr);
    if (incrementalPixelMap == nullptr) {
        HiLog::Error(LABEL, "nativePromoteDecoding incremental pixelmap pointer is null.");
        return -1;
    }

    uint8_t decodeProgress = 0;
    uint32_t ret = incrementalPixelMap->PromoteDecoding(decodeProgress);
    if ((ret != SUCCESS) && (ret != ERR_IMAGE_SOURCE_DATA_INCOMPLETE)) {
        HiLog::Error(LABEL, "nativePromoteDecoding PromoteDecoding fail,ret=%{public}u.", ret);
        return ret;
    }

    return decodeProgress;
}

void ohos_media_image_IncrementalPixelMap_nativeDetachFromDecoding(JNIEnv *env, jclass thiz, jlong nativePtr)
{
    IncrementalPixelMap *incrementalPixelMap = reinterpret_cast<IncrementalPixelMap *>(nativePtr);
    if (incrementalPixelMap == nullptr) {
        HiLog::Error(LABEL, "nativeDetachFromDecoding incremental pixelmap pointer is null.");
        return;
    }

    incrementalPixelMap->DetachFromDecoding();
}

void ohos_media_image_IncrementalPixelMap_nativeRelease(JNIEnv *env, jobject thiz, jlong nativePtr)
{
    HiLog::Debug(LABEL, "nativeRelease begin");
    IncrementalPixelMap *incrementalPixelMap = reinterpret_cast<IncrementalPixelMap *>(nativePtr);
    if (incrementalPixelMap != nullptr) {
        HiLog::Debug(LABEL, "do release incremental pixel map");
        delete incrementalPixelMap;
        incrementalPixelMap = nullptr;
    }
    HiLog::Debug(LABEL, "nativeRelease end");
}

static const JNINativeMethod METHODS[] = {
    { "nativePromoteDecoding", "(J)I",
      reinterpret_cast<void *>(ohos_media_image_IncrementalPixelMap_nativePromoteDecoding) },
    { "nativeDetachFromDecoding", "(J)V",
      reinterpret_cast<void *>(ohos_media_image_IncrementalPixelMap_nativeDetachFromDecoding) },
    { "nativeRelease", "(J)V", reinterpret_cast<void *>(ohos_media_image_IncrementalPixelMap_nativeRelease) },
};

jint JNI_OnLoad(JavaVM *vm, void *reserved)
{
    HiLog::Debug(LABEL, "JNI_OnLoad begin");
    JNIEnv *env = nullptr;
    if (vm->GetEnv(reinterpret_cast<void **>(&env), JNI_VERSION_1_4) != JNI_OK) {
        HiLog::Error(LABEL, "JNI_OnLoad: GetEnv failed");
        return ERROR;
    }
    int32_t ret =
        JkitRegisterNativeMethods(env, "ohos/media/image/IncrementalPixelMap", METHODS, ARRCOUNT(METHODS));
    if (ret == JNI_ERR) {
        HiLog::Error(LABEL, "JkitRegisterNativeMethods failed, ret=%{public}d", ret);
        return ERROR;
    }
    Jkit::nativeInit(vm);
    HiLog::Debug(LABEL, "JNI_OnLoad end");
    return JNI_VERSION_1_4;
}
