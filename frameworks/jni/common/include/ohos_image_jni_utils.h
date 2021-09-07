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

#ifndef OHOS_IMAGE_JNI_UTILS_H
#define OHOS_IMAGE_JNI_UTILS_H

#include <iostream>
#include "hilog/log.h"
#include "image_type.h"
#include "jkit_utils.h"
#include "log_tags.h"
#include "pixel_map.h"

namespace OHOS {
namespace Media {
bool GetFieldId(JNIEnv *env, const std::string &className, const std::string &param, const std::string &paramType,
                jfieldID &fieldId);
bool GetFieldId(JNIEnv *env, jobject inObj, const std::string &param, const std::string &paramType, jfieldID &fieldId);
bool GetObjectField(JNIEnv *env, jobject inObj, const std::string &param, const std::string &paramType,
                    jobject &result);
bool GetFieldLongValue(JNIEnv *env, jobject inObj, const std::string &param, int64_t &result);
bool GetFieldIntValue(JNIEnv *env, jobject inObj, const std::string &param, int32_t &result);
bool GetFieldStringValue(JNIEnv *env, jobject inObj, const std::string &param, std::string &result);
bool GetFieldBooleanValue(JNIEnv *env, jobject inObj, const std::string &param, bool &result);
bool GetIntValByMethod(JNIEnv *env, jobject inObj, const std::string &methodName, int32_t &value);
bool GetSize(JNIEnv *env, jobject sizeObj, Size &size);
bool GetRect(JNIEnv *env, jobject rectObj, Rect &rect);
bool GetBufferPointer(JNIEnv *env, jobject jbuffer, jlong &pointer);
bool ThrowException(JNIEnv *env, const char *className, const char *msg);
bool ThrowImageException(JNIEnv *env, const char *msg);
bool ThrowIllegalArgumentException(JNIEnv *env, const char *msg);
bool ThrowIllegalStateException(JNIEnv *env, const char *msg);
void ThrowExceptionByErrorCode(JNIEnv *env, uint32_t errorCode);
} // namespace Media
} // namespace OHOS

#endif // OHOS_IMAGE_JNI_UTILS_H
