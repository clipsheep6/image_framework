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

#include "pixel_map_jni_utils.h"

namespace OHOS {
namespace Media {
using namespace OHOS::HiviewDFX;

static constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {
    LOG_CORE, LOG_TAG_DOMAIN_ID_IMAGE, "PixelMapJniUtilsAdapter"
};

jarray PixelMapJniUtilsAdapter::GetBufferBaseArray(JNIEnv *env, jobject jbuffer)
{
    jclass nioAccessClass = env->FindClass("java/nio/NIOAccess");
    if (nioAccessClass == nullptr) {
        HiLog::Error(LABEL, "GetNioBufferBaseArray nioAccessClass is null.");
        return nullptr;
    }
    jmethodID getBaseArrayMethod =
        env->GetStaticMethodID(nioAccessClass, "getBaseArray", "(Ljava/nio/Buffer;)Ljava/lang/Object;");
    if (getBaseArrayMethod == nullptr) {
        HiLog::Error(LABEL, "GetNioBufferBaseArray getBaseArrayMethod is null.");
        env->DeleteLocalRef(nioAccessClass);
        return nullptr;
    }
    jarray bufferArray = static_cast<jarray>(env->CallStaticObjectMethod(nioAccessClass, getBaseArrayMethod, jbuffer));
    env->DeleteLocalRef(nioAccessClass);
    return bufferArray;
}

bool PixelMapJniUtilsAdapter::GetBufferBaseArrayOffset(JNIEnv *env, jobject jbuffer, jint &offset)
{
    jclass nioAccessClass = env->FindClass("java/nio/NIOAccess");
    if (nioAccessClass == nullptr) {
        HiLog::Error(LABEL, "GetNioBufferBaseArray nioAccessClass is null.");
        return false;
    }
    jmethodID getOffsetMethod = env->GetStaticMethodID(nioAccessClass, "getBaseArrayOffset", "(Ljava/nio/Buffer;)I");
    if (getOffsetMethod == nullptr) {
        HiLog::Error(LABEL, "GetNioBufferBaseArray getOffsetMethod is null.");
        env->DeleteLocalRef(nioAccessClass);
        return false;
    }
    offset = env->CallStaticIntMethod(nioAccessClass, getOffsetMethod, jbuffer);
    env->DeleteLocalRef(nioAccessClass);
    return true;
}
} // namespace Media
} // namespace OHOS
