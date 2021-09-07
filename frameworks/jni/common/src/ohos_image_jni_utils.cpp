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

#include "ohos_image_jni_utils.h"

#include <iostream>
#include <map>
#include "media_errors.h"

namespace OHOS {
namespace Media {
using namespace OHOS::HiviewDFX;

namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, LOG_TAG_DOMAIN_ID_IMAGE, "Image_Jni_Utils" };
const std::map<uint32_t, std::string> CODE_TO_EXCEPTION = {
    { ERR_IMAGE_UNKNOWN_FORMAT, "ohos/media/image/SourceDataMalformedException" },
    { ERR_IMAGE_SOURCE_DATA_INCOMPLETE, "ohos/media/image/SourceDataIncompleteException" },
    { ERR_IMAGE_DECODE_FAILED, "ohos/media/image/SourceDataMalformedException" },
#if defined(_WIN32) || defined(_APPLE)
    { ERR_IMAGE_PLUGIN_CREATE_FAILED, "ohos/media/image/SourceDataMalformedException" },
#endif
};
const std::map<uint32_t, std::string> CODE_TO_MESSAGE = {
    { ERR_IMAGE_UNKNOWN_FORMAT, "image source data error or unknown format" },
    { ERR_IMAGE_SOURCE_DATA_INCOMPLETE, "image source data incomplete" },
    { ERR_IMAGE_DECODE_FAILED, "image decode failed" },
#if defined(_WIN32) || defined(_APPLE)
    { ERR_IMAGE_PLUGIN_CREATE_FAILED, "create plugin failed" },
#endif
};
} // namespace

bool GetFieldId(JNIEnv *env, const std::string &className, const std::string &param, const std::string &paramType,
                jfieldID &fieldId)
{
    if (className.empty() || param.empty() || paramType.empty()) {
        HiLog::Error(LABEL, "GetFieldId params invalid");
        return false;
    }

    jclass clazz = env->FindClass(className.c_str());
    if (clazz == nullptr) {
        HiLog::Error(LABEL, "GetFieldId fail, class: %{public}s not found", className.c_str());
        return false;
    }

    fieldId = env->GetFieldID(clazz, param.c_str(), paramType.c_str());
    env->DeleteLocalRef(clazz);
    if (fieldId == nullptr) {
        HiLog::Error(LABEL, "GetFieldId fail, param %{public}s %{public}s not found", param.c_str(), paramType.c_str());
        return false;
    }
    return true;
}

bool GetFieldId(JNIEnv *env, jobject inObj, const std::string &param, const std::string &paramType, jfieldID &fieldId)
{
    if (inObj == nullptr || param.empty() || paramType.empty()) {
        HiLog::Error(LABEL, "GetJField params invalid");
        return false;
    }

    jclass clazz = env->GetObjectClass(inObj);
    if (clazz == nullptr) {
        HiLog::Error(LABEL, "GetJField fail, inObj not found class");
        return false;
    }

    fieldId = env->GetFieldID(clazz, param.c_str(), paramType.c_str());
    env->DeleteLocalRef(clazz);
    if (fieldId == nullptr) {
        HiLog::Error(LABEL, "GetJField fail, param %{public}s %{public}s not found", param.c_str(), paramType.c_str());
        return false;
    }
    return true;
}

bool GetObjectField(JNIEnv *env, jobject inObj, const std::string &param, const std::string &paramType, jobject &result)
{
    jfieldID fieldId;
    if (GetFieldId(env, inObj, param, paramType, fieldId)) {
        result = env->GetObjectField(inObj, fieldId);
        return true;
    }
    return false;
}

bool GetFieldLongValue(JNIEnv *env, jobject inObj, const std::string &param, int64_t &result)
{
    jfieldID fieldId;
    if (GetFieldId(env, inObj, param, "J", fieldId)) {
        result = env->GetLongField(inObj, fieldId);
        return true;
    }
    return false;
}

bool GetFieldIntValue(JNIEnv *env, jobject inObj, const std::string &param, int32_t &result)
{
    jfieldID fieldId;
    if (GetFieldId(env, inObj, param, "I", fieldId)) {
        result = env->GetIntField(inObj, fieldId);
        return true;
    }
    return false;
}

bool GetFieldStringValue(JNIEnv *env, jobject inObj, const std::string &param, std::string &result)
{
    jfieldID fieldId;
    if (GetFieldId(env, inObj, param, "L/java/lang/String;", fieldId)) {
        jstring strObj = reinterpret_cast<jstring>(env->GetObjectField(inObj, fieldId));
        const char *strChars = env->GetStringUTFChars(strObj, nullptr);
        if (strChars != nullptr) {
            result = strChars;
            env->ReleaseStringUTFChars(strObj, strChars);
        }
        return true;
    }
    return false;
}

bool GetFieldBooleanValue(JNIEnv *env, jobject inObj, const std::string &param, bool &result)
{
    jfieldID fieldId;
    if (GetFieldId(env, inObj, param, "Z", fieldId)) {
        result = env->GetBooleanField(inObj, fieldId);
        return true;
    }
    return false;
}

bool GetSize(JNIEnv *env, jobject sizeObj, Size &size)
{
    if (sizeObj != nullptr) {
        if (!GetFieldIntValue(env, sizeObj, "width", size.width) ||
            !GetFieldIntValue(env, sizeObj, "height", size.height)) {
            HiLog::Error(LABEL, "get size value fail");
            return false;
        }
    }
    HiLog::Debug(LABEL, "size width:%{public}d, height:%{public}d", size.width, size.height);
    return true;
}

bool GetRect(JNIEnv *env, jobject rectObj, Rect &rect)
{
    if (rectObj != nullptr) {
        if (!GetFieldIntValue(env, rectObj, "minX", rect.left) || !GetFieldIntValue(env, rectObj, "minY", rect.top) ||
            !GetFieldIntValue(env, rectObj, "width", rect.width) ||
            !GetFieldIntValue(env, rectObj, "height", rect.height)) {
            HiLog::Error(LABEL, "get rect value fail");
            return false;
        }
    }
    HiLog::Debug(LABEL, "rect:[%{public}d, %{public}d, %{public}d, %{public}d]", rect.left, rect.top, rect.width,
                 rect.height);
    return true;
}

bool GetIntValByMethod(JNIEnv *env, jobject inObj, const std::string &methodName, int32_t &value)
{
    if (inObj == nullptr || methodName.empty()) {
        HiLog::Error(LABEL, "GetIntValByMethod params invalid");
        return false;
    }
    jclass clazz = env->GetObjectClass(inObj);
    if (clazz == nullptr) {
        HiLog::Error(LABEL, "GetIntValByMethod fail, inObj not found class");
        return false;
    }
    jmethodID zMethodId = env->GetMethodID(clazz, methodName.c_str(), "()I");
    env->DeleteLocalRef(clazz);
    if (zMethodId == nullptr) {
        HiLog::Error(LABEL, "GetIntValByMethod fail, method: %{public}s not found.", methodName.c_str());
        return false;
    }
    value = env->CallIntMethod(inObj, zMethodId);
    return true;
}

bool GetBufferPointer(JNIEnv *env, jobject jbuffer, jlong &pointer)
{
    if (!GetFieldLongValue(env, jbuffer, "address", pointer)) {
        HiLog::Error(LABEL, "GetNioBufferPointer get baseAddress fail.");
        return false;
    }
    if (pointer != 0) {
        int pos = 0;
        if (!GetFieldIntValue(env, jbuffer, "position", pos)) {
            HiLog::Error(LABEL, "GetNioBufferPointer get position fail.");
            return false;
        }
        int shift = 0;
        if (!GetFieldIntValue(env, jbuffer, "_elementSizeShift", shift)) {
            HiLog::Error(LABEL, "GetNioBufferPointer get shift fail.");
            return false;
        }
        pointer += static_cast<uint32_t>(pos) << static_cast<uint32_t>(shift);
    }
    return true;
}

bool ThrowException(JNIEnv *env, const char *className, const char *msg)
{
    if ((className == nullptr) || (msg == nullptr)) {
        HiLog::Error(LABEL, "ThrownException param invalid");
        return false;
    }
    jclass exceptionClass = env->FindClass(className);
    if (exceptionClass == nullptr) {
        HiLog::Error(LABEL, "FindClass(%{public}s) fail", className);
        return false;
    }
    jint ret = env->ThrowNew(exceptionClass, msg);
    env->DeleteLocalRef(exceptionClass);
    return ret == 0;
}

bool ThrowImageException(JNIEnv *env, const char *msg)
{
    return ThrowException(env, "ohos/media/image/ImageException", msg);
}

bool ThrowIllegalArgumentException(JNIEnv *env, const char *msg)
{
    return ThrowException(env, "java/lang/IllegalArgumentException", msg);
}

bool ThrowIllegalStateException(JNIEnv *env, const char *msg)
{
    return ThrowException(env, "java/lang/IllegalStateException", msg);
}

bool ErrorCodeToException(JNIEnv *env, uint32_t errorCode, std::string &exception)
{
    auto iter = CODE_TO_EXCEPTION.find(errorCode);
    if (iter == CODE_TO_EXCEPTION.end()) {
        HiLog::Error(LABEL, "can not find error code in exception map");
        return false;
    }

    exception = iter->second;
    return true;
}

bool ErrorCodeToMessage(JNIEnv *env, uint32_t errorCode, std::string &msg)
{
    auto iter = CODE_TO_MESSAGE.find(errorCode);
    if (iter == CODE_TO_MESSAGE.end()) {
        HiLog::Error(LABEL, "can not find error code in message map");
        return false;
    }

    msg = iter->second;
    return true;
}

void ThrowExceptionByErrorCode(JNIEnv *env, uint32_t errorCode)
{
    if (errorCode == SUCCESS) {
        HiLog::Debug(LABEL, "operation is success, no need to throw exception");
        return;
    }
    std::string exception;
    if (!ErrorCodeToException(env, errorCode, exception)) {
        HiLog::Error(LABEL, "can not find exception by error code");
        return;
    }

    std::string msg;
    if (!ErrorCodeToMessage(env, errorCode, msg)) {
        HiLog::Error(LABEL, "can not find error msg by error code");
        return;
    }

    if (!ThrowException(env, exception.c_str(), msg.c_str())) {
        HiLog::Error(LABEL, "throw exception failed, exception:%{public}s, msg:%{public}s", exception.c_str(),
                     msg.c_str());
        return;
    }

    HiLog::Debug(LABEL, "throw exception success, exception:%{public}s, msg:%{public}s", exception.c_str(),
                 msg.c_str());
}
} // namespace Media
} // namespace OHOS
