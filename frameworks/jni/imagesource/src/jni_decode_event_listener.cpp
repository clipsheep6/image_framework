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


#include "jni_decode_event_listener.h"
#include "ohos_image_jni_utils.h"
#include "hilog/log.h"

using namespace OHOS::HiviewDFX;

static constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, LOG_TAG_DOMAIN_ID_IMAGE, "JniDecodeEventListener" };

JniDecodeEventListener::JniDecodeEventListener(JNIEnv* env, jobject thiz, jmethodID postEventFromNative)
{
    HiLog::Debug(LABEL, "%{public}s", __FUNCTION__);

    env->GetJavaVM(&mJavaVM);

    // We use a weak reference so the ImageSource object can be garbage collected.
    // The reference is only used as a proxy for callbacks.
    mObject = env->NewGlobalRef(thiz);

    mPostEventFromNative = postEventFromNative;
}

JNIEnv* JniDecodeEventListener::getJNIEnv()
{
    JNIEnv* env = nullptr;
    mJavaVM->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_4);
    return env;
}

JniDecodeEventListener::~JniDecodeEventListener()
{
    HiLog::Debug(LABEL, "%{public}s", __FUNCTION__);

    // remove global references
    JNIEnv *env = getJNIEnv();
    if (env == nullptr) {
        return;
    }
    env->DeleteGlobalRef(mObject);
}

void JniDecodeEventListener::OnEvent(int event)
{
    JNIEnv *env = getJNIEnv();
    if (env == nullptr) {
        HiLog::Error(LABEL, "env is null");
        return;
    }

    HiLog::Debug(LABEL, "%{public}s event %{public}d", __FUNCTION__, event);
    if (mPostEventFromNative == nullptr) {
        HiLog::Error(LABEL, "JniDecodeEventListener: mPostEventFromNative is null.");
        return;
    }

    env->CallVoidMethod(mObject, mPostEventFromNative, mObject, jint(event));
    if (env->ExceptionCheck()) {
        HiLog::Error(LABEL, "An exception occurred while notifying an event.");
        env->ExceptionClear();
    }
}
