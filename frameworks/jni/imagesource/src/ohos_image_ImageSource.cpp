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

#include <sstream>
#include <vector>
#include "ohos_image_jni_utils.h"
#include "hilog/log.h"
#include "image_source.h"
#include "jkit_utils.h"
#include "log_tags.h"
#include "media_errors.h"
#include "pixel_map_manager.h"
#ifndef _WIN32
#include "securec.h"
#else
#include "memory.h"
#endif
#include "jni_decode_event_listener.h"
#ifdef _WIN32
#include <string.h>
#endif
#if !defined(_WIN32) && !defined(_APPLE)
#include "xcollie/xcollie.h"
#endif

using namespace OHOS::HiviewDFX;
using namespace OHOS::Media;

static constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, LOG_TAG_DOMAIN_ID_IMAGE, "ImageSource_JNI" };
static jclass g_imageSourceClass;
static jmethodID g_imageSourceConstructMethodId;
static jmethodID g_imageSourcePostFromNativeMethodId;
static std::unique_ptr<JniDecodeEventListener> g_decodeEventListener;
static jclass g_pixelMapClass;
static jmethodID g_pixelMapCtorMethodId;
static jmethodID g_setBaseDensityMethodId;
static jclass g_allocatorClass;
static jclass g_pixelFormatClass;
static jclass g_colorSpaceClass;

// DecodingOptions fields
static jfieldID g_optionsFitDensityFieldId;
static jfieldID g_optionsAllocatorFieldId;
static jfieldID g_optionsRectFieldId;
static jfieldID g_optionsSizeFieldId;
static jfieldID g_optionsRotateDegreesFieldId;
static jfieldID g_optionsSampleSizeFieldId;
static jfieldID g_optionsPixelFormatFieldId;
static jfieldID g_optionsColorSpaceFieldId;
static jfieldID g_optionsAllowPartialImageFieldId;
static jfieldID g_optionsEditableFieldId;

static inline void PixelFormatToNative(PixelFormat &pixelFormat)
{
    if (pixelFormat == PixelFormat::ARGB_8888) {
        pixelFormat = PixelFormat::RGBA_8888;
    }
}

static bool InitImageSource(JNIEnv *env)
{
    jclass imageClazz = env->FindClass("ohos/media/image/ImageSource");
    if (imageClazz == nullptr) {
        HiLog::Error(LABEL, "find ImageSource class fail");
        return false;
    }
    g_imageSourceClass = static_cast<jclass>(env->NewGlobalRef(imageClazz));
    g_imageSourceConstructMethodId = env->GetMethodID(g_imageSourceClass, "<init>", "(J)V");
    g_imageSourcePostFromNativeMethodId = env->GetMethodID(g_imageSourceClass, "postEventFromNative",
                                                           "(Lohos/media/image/ImageSource;I)V");
    env->DeleteLocalRef(imageClazz);
    return true;
}

static bool CheckInitDecodeOption2(JNIEnv *env, jclass decodOptionClazz)
{
    if (decodOptionClazz == nullptr) {
        HiLog::Error(LABEL, "find DecodeOption class fail");
        return false;
    }
    g_optionsPixelFormatFieldId = env->GetFieldID(decodOptionClazz, "desiredPixelFormat",
                                                  "Lohos/media/image/common/PixelFormat;");
    if (g_optionsPixelFormatFieldId == nullptr) {
        HiLog::Error(LABEL, "GetFieldID desiredPixelFormat not found.");
        env->DeleteLocalRef(decodOptionClazz);
        return false;
    }
    return true;
}

static bool InitPixelMap(JNIEnv *env)
{
    jclass pixelMapClazz = env->FindClass("ohos/media/image/PixelMap");
    if (pixelMapClazz == nullptr) {
        HiLog::Error(LABEL, "CreatePixelmap fail, not found PixelMap class.");
        return false;
    }
    g_pixelMapClass = static_cast<jclass>(env->NewGlobalRef(pixelMapClazz));
    g_pixelMapCtorMethodId = env->GetMethodID(pixelMapClazz, "<init>", "(JJ[B)V");
    if (g_pixelMapCtorMethodId == nullptr) {
        env->DeleteLocalRef(pixelMapClazz);
        HiLog::Error(LABEL, "InitPixelMapId fail, PixelMap construction method not found.");
        return false;
    }
    g_setBaseDensityMethodId = env->GetMethodID(pixelMapClazz, "setBaseDensity", "(I)V");
    if (g_setBaseDensityMethodId == nullptr) {
        env->DeleteLocalRef(pixelMapClazz);
        HiLog::Error(LABEL, "InitPixelMapId fail, PixelMap setBaseDensity method not found.");
        return false;
    }
    env->DeleteLocalRef(pixelMapClazz);
    return true;
}

static bool InitDecodeOption(JNIEnv *env)
{
    jclass decodOptionClazz = env->FindClass("ohos/media/image/ImageSource$DecodingOptions");
    if (decodOptionClazz == nullptr) {
        HiLog::Error(LABEL, "find DecodeOption class fail");
        return false;
    }
    g_optionsFitDensityFieldId = env->GetFieldID(decodOptionClazz, "fitDensity", "I");
    if (g_optionsFitDensityFieldId == nullptr) {
        HiLog::Error(LABEL, "GetFieldID fail, fitDensity not found.");
        env->DeleteLocalRef(decodOptionClazz);
        return false;
    }
    g_optionsAllocatorFieldId = env->GetFieldID(decodOptionClazz, "allocator",
                                                "Lohos/media/image/common/AllocatorType;");
    if (g_optionsAllocatorFieldId == nullptr) {
        HiLog::Error(LABEL, "GetFieldID  allocator not found.");
        env->DeleteLocalRef(decodOptionClazz);
        return false;
    }
    g_optionsRectFieldId = env->GetFieldID(decodOptionClazz, "desiredRegion",
                                           "Lohos/media/image/common/Rect;");
    if (g_optionsRectFieldId == nullptr) {
        HiLog::Error(LABEL, "GetFieldID fail, fitDensity not found.");
        env->DeleteLocalRef(decodOptionClazz);
        return false;
    }
    g_optionsSizeFieldId = env->GetFieldID(decodOptionClazz, "desiredSize",
                                           "Lohos/media/image/common/Size;");
    if (g_optionsSizeFieldId == nullptr) {
        HiLog::Error(LABEL, "GetFieldID fail, fitDensity not found.");
        env->DeleteLocalRef(decodOptionClazz);
        return false;
    }
    g_optionsRotateDegreesFieldId = env->GetFieldID(decodOptionClazz, "rotateDegrees", "F");
    if (g_optionsRotateDegreesFieldId == nullptr) {
        HiLog::Error(LABEL, "GetFieldID fail, rotateDegrees not found.");
        env->DeleteLocalRef(decodOptionClazz);
        return false;
    }
    g_optionsSampleSizeFieldId = env->GetFieldID(decodOptionClazz, "sampleSize", "I");
    if (g_optionsSampleSizeFieldId == nullptr) {
        HiLog::Error(LABEL, "GetFieldID fail, sampleSize not found.");
        env->DeleteLocalRef(decodOptionClazz);
        return false;
    }
    env->DeleteLocalRef(decodOptionClazz);
    return true;
}

static bool InitDecodeOption2(JNIEnv *env)
{
    jclass decodOptionClazz = env->FindClass("ohos/media/image/ImageSource$DecodingOptions");
    if (CheckInitDecodeOption2(env, decodOptionClazz) != true) {
        return false;
    }
    g_optionsColorSpaceFieldId = env->GetFieldID(decodOptionClazz, "desiredColorSpace",
                                                 "Lohos/media/image/common/ColorSpace;");
    if (g_optionsColorSpaceFieldId == nullptr) {
        HiLog::Error(LABEL, "GetFieldID allocator not found.");
        env->DeleteLocalRef(decodOptionClazz);
        return false;
    }
    g_optionsAllowPartialImageFieldId = env->GetFieldID(decodOptionClazz, "allowPartialImage", "Z");
    if (g_optionsAllowPartialImageFieldId == nullptr) {
        HiLog::Error(LABEL, "GetFieldID allowPartialImage not found.");
        env->DeleteLocalRef(decodOptionClazz);
        return false;
    }
    g_optionsEditableFieldId = env->GetFieldID(decodOptionClazz, "editable", "Z");
    if (g_optionsEditableFieldId == nullptr) {
        HiLog::Error(LABEL, "GetFieldID editable not found.");
        env->DeleteLocalRef(decodOptionClazz);
        return false;
    }
    jclass allocatorClazz = env->FindClass("ohos/media/image/common/AllocatorType");
    if (allocatorClazz == nullptr) {
        HiLog::Error(LABEL, "FindClass AllocatorType not found.");
        return false;
    }
    g_allocatorClass = static_cast<jclass>(env->NewGlobalRef(allocatorClazz));
    env->DeleteLocalRef(allocatorClazz);
    jclass pixelFormatClazz = env->FindClass("ohos/media/image/common/PixelFormat");
    if (pixelFormatClazz == nullptr) {
        HiLog::Error(LABEL, "FindClass PixelFormat not found.");
        return false;
    }
    g_pixelFormatClass = static_cast<jclass>(env->NewGlobalRef(pixelFormatClazz));
    env->DeleteLocalRef(pixelFormatClazz);
    jclass colorClazz = env->FindClass("ohos/media/image/common/ColorSpace");
    if (colorClazz == nullptr) {
        HiLog::Error(LABEL, "FindClass ColorSpace not found.");
        return false;
    }
    g_colorSpaceClass = static_cast<jclass>(env->NewGlobalRef(colorClazz));
    env->DeleteLocalRef(colorClazz);
    env->DeleteLocalRef(decodOptionClazz);
    return true;
}

void ohos_media_image_ImageSource_nativeInit(JNIEnv *env, jclass thiz)
{
    HiLog::Debug(LABEL, "nativeInit begin");
    if (!InitImageSource(env)) {
        HiLog::Error(LABEL, "nativeInit InitImageSource failed");
        return;
    }
    if (!InitPixelMap(env)) {
        HiLog::Error(LABEL, "nativeInit InitPixelMap failed");
        return;
    }
    if (!InitDecodeOption(env)) {
        HiLog::Error(LABEL, "nativeInit InitDecodeOption failed");
        return;
    }
    if (!InitDecodeOption2(env)) {
        HiLog::Error(LABEL, "nativeInit InitDecodeOption2 failed");
        return;
    }
    HiLog::Debug(LABEL, "nativeInit end");
}

jobject GetObject(JNIEnv *env, jobject inObj, const std::string &className, const std::string &para,
                  const std::string &type)
{
    if (inObj == nullptr) {
        HiLog::Error(LABEL, "GetObject para obj is null.");
        return nullptr;
    }
    jfieldID zFieldId;
    bool isSuccess = GetFieldId(env, className, para, type, zFieldId);
    if (!isSuccess) {
        HiLog::Error(LABEL, "GetObject fail, para: %{public}s, type: %{public}s not found.", para.c_str(),
                     type.c_str());
        return nullptr;
    }
    if (zFieldId == nullptr) {
        HiLog::Error(LABEL, "zFieldId is null.");
        return nullptr;
    }
    jobject outValue = env->GetObjectField(inObj, zFieldId);
    return outValue;
}

bool GetIntValue(JNIEnv *env, jobject inObj, const std::string &className, const std::string &para, jint &value)
{
    if (inObj == nullptr) {
        HiLog::Error(LABEL, "GetIntValue para obj is null.");
        return false;
    }
    jfieldID zFieldId;
    bool isSuccess = GetFieldId(env, className, para, "I", zFieldId);
    if (!isSuccess) {
        HiLog::Error(LABEL, "GetIntValue fail, para: %{public}s.", para.c_str());
        return false;
    }
    value = env->GetIntField(inObj, zFieldId);
    return true;
}

bool GetStringValue(JNIEnv *env, jobject inObj, const std::string &className, const std::string &para, jstring &value)
{
    if (inObj == nullptr) {
        HiLog::Error(LABEL, "GetStringValue para obj is null.");
        return false;
    }
    jfieldID zFieldId;
    bool isSuccess = GetFieldId(env, className, para, "L/java/lang/String;", zFieldId);
    if (!isSuccess) {
        HiLog::Error(LABEL, "GetStringValue fail, para: %{public}s.", para.c_str());
        return false;
    }
    if (zFieldId == nullptr) {
        HiLog::Error(LABEL, "GetStringValue fail, zMethodId is null");
        return false;
    }
    value = reinterpret_cast<jstring>(env->GetObjectField(inObj, zFieldId));
    return true;
}

bool ReadInputStreamToBytesArray(JNIEnv *env, jobject inObj, jbyteArray buf, jint &value)
{
    if (inObj == nullptr) {
        HiLog::Error(LABEL, "ReadInputStreamToBytesArray : obj is null.");
        return false;
    }

    if (buf == nullptr) {
        HiLog::Error(LABEL, "ReadInputStreamToBytesArray : buf is null.");
        return false;
    }

    jclass zClazz = env->GetObjectClass(inObj);
    if (zClazz == nullptr) {
        HiLog::Error(LABEL, "ReadInputStreamToBytesArray class(java/io/InputStream) not found.");
        return false;
    }
    jmethodID zMethodId = env->GetMethodID(zClazz, "read", "([B)I");
    env->DeleteLocalRef(zClazz);
    if (zMethodId == nullptr) {
        HiLog::Error(LABEL, "ReadInputStreamToBytesArray fail, method: read not found.");
        return false;
    }
    value = env->CallIntMethod(inObj, zMethodId, buf);
    return true;
}

static jobject ohos_media_image_ImageSource_nativeGetSupportedFormats(JNIEnv *env, jobject thiz)
{
    std::set<std::string> formats;
    if (ImageSource::GetSupportedFormats(formats) != SUCCESS) {
        HiLog::Error(LABEL, "GetSupportedFormats is fail.");
        return nullptr;
    }

    jclass listClazz = env->FindClass("java/util/HashSet");
    if (listClazz == nullptr) {
        HiLog::Error(LABEL, "nativeGetSupportedFormats HashSet class not find!");
        return nullptr;
    }

    jmethodID listInit = env->GetMethodID(listClazz, "<init>", "()V");
    jmethodID listAdd = env->GetMethodID(listClazz, "add", "(Ljava/lang/Object;)Z");
    if ((listInit == nullptr) || (listAdd == nullptr)) {
        HiLog::Error(LABEL, "nativeGetSupportedFormats mehtoid id is null");
        env->DeleteLocalRef(listClazz);
        return nullptr;
    }

    jobject listObj = env->NewObject(listClazz, listInit, "");
    env->DeleteLocalRef(listClazz);
    if (listObj == nullptr) {
        HiLog::Error(LABEL, "nativeGetSupportedFormats listObj is null");
        return nullptr;
    }

    std::set<std::string>::iterator iter = formats.begin();
    while (iter != formats.end()) {
        env->CallBooleanMethod(listObj, listAdd, env->NewStringUTF((*iter).c_str()));
        ++iter;
    }
    return listObj;
}

void ohos_media_image_ImageSource_nativeSetDecodeEventListener(JNIEnv *env, jobject thiz,
                                                               jobject sourceObj, jlong nativePtr)
{
    ImageSource *imgSource = reinterpret_cast<ImageSource *>(nativePtr);
    if (imgSource == nullptr) {
        HiLog::Error(LABEL, "nativeSetDecodeEventListener, get image source fail.");
        return;
    }

    if (g_decodeEventListener != nullptr) {
        g_decodeEventListener.reset();
    }

    // init listener
    g_decodeEventListener = std::make_unique<JniDecodeEventListener>(env,
        sourceObj, g_imageSourcePostFromNativeMethodId);
    imgSource->AddDecodeListener(g_decodeEventListener.release());
    HiLog::Debug(LABEL, "nativeSetDecodeEventListener end");
}

void ohos_media_image_ImageSource_nativeRelease(JNIEnv *env, jobject thiz, jlong nativePtr)
{
    HiLog::Debug(LABEL, "nativeRelease begin");
    ImageSource *imageSource = reinterpret_cast<ImageSource *>(nativePtr);
    if (imageSource != nullptr) {
        HiLog::Error(LABEL, "do release ImageSource pointer.");
        if (g_decodeEventListener != nullptr) {
            imageSource->RemoveDecodeListener(g_decodeEventListener.release());
            g_decodeEventListener.reset();
        }
        delete imageSource;
    }
    HiLog::Debug(LABEL, "nativeRelease end");
}

bool ConvertSourceOptions(JNIEnv *env, jobject opts, SourceOptions &sourceOpts)
{
    if (opts == nullptr) {
        HiLog::Debug(LABEL, "ConvertSourceOptions source opts is null, use default option");
        return true;
    }

    jclass optClazz = env->GetObjectClass(opts);
    if (optClazz == nullptr) {
        HiLog::Error(LABEL, "ConvertSourceOptions by path, optClazz is null");
        return false;
    }
    jfieldID formatFileId = env->GetFieldID(optClazz, "formatHint", "Ljava/lang/String;");
    jfieldID baseDensityFileId = env->GetFieldID(optClazz, "baseDensity", "I");
    env->DeleteLocalRef(optClazz);
    if (formatFileId == nullptr) {
        HiLog::Error(LABEL, "ConvertSourceOptions by path, formatFileId is null.");
        return false;
    }
    if (baseDensityFileId == nullptr) {
        HiLog::Error(LABEL, "ConvertSourceOptions by path, baseDensityFileId is null.");
        return false;
    }

    jobject formatValue = env->GetObjectField(opts, formatFileId);
    jstring formatHint = reinterpret_cast<jstring>(formatValue);
    const char *formatHintChars = env->GetStringUTFChars(formatHint, nullptr);
    // formatHint is optional
    if (formatHintChars != nullptr) {
        sourceOpts.formatHint = formatHintChars;
        env->ReleaseStringUTFChars(formatHint, formatHintChars);  // Release formatHintChars
        env->DeleteLocalRef(formatValue);
    }

    jint baseDensityValue = env->GetIntField(opts, baseDensityFileId);
    sourceOpts.baseDensity = baseDensityValue;
    return true;
}

jobject ohos_media_image_ImageSource_nativeCreateImageSourceFromFilePath(JNIEnv *env, jobject thiz,
                                                                         jstring pathName, jobject opts)
{
    if (pathName == nullptr) {
        HiLog::Error(LABEL, "createImageSource by path, pathName is null.");
        return nullptr;
    }

    SourceOptions sourceOpt;
    if (!ConvertSourceOptions(env, opts, sourceOpt)) {
        HiLog::Error(LABEL, "createImageSource ConvertSourceOptions fail.ed");
        return nullptr;
    }

    const char *fileChars = env->GetStringUTFChars(pathName, nullptr);
    if (fileChars == nullptr) {
        HiLog::Error(LABEL, "createImageSource by path, get parameter str fail.");
        return nullptr;
    }

    uint32_t errCode = ERROR;
    std::unique_ptr<ImageSource> imageSource = ImageSource::CreateImageSource(fileChars, sourceOpt, errCode);
    env->ReleaseStringUTFChars(pathName, fileChars);  // Release fileChars
    if (imageSource == nullptr) {
        HiLog::Error(LABEL, "createImageSource by path, create image source fail.");
        return nullptr;
    }
    return env->NewObject(g_imageSourceClass, g_imageSourceConstructMethodId,
                          reinterpret_cast<jlong>(imageSource.release()));
}

std::unique_ptr<std::stringstream> ConvertInputStreamToStringStream(JNIEnv *env, jobject inputStream)
{
    constexpr jsize buffSize = 4096;
    jbyteArray inputBuf = env->NewByteArray(buffSize);
    if (inputBuf == nullptr) {
        HiLog::Error(LABEL, "createImageSource by InputStream, new bytearray fail.");
        return nullptr;
    }

    jbyte *outBuf = static_cast<jbyte *>(malloc(buffSize));
    if (outBuf == nullptr) {
        env->DeleteLocalRef(inputBuf);
        HiLog::Error(LABEL, "createImageSource by InputStream, malloc fail.");
        return nullptr;
    }
    std::unique_ptr<std::stringstream> dataStream = std::make_unique<std::stringstream>();
    if (dataStream == nullptr) {
        HiLog::Error(LABEL, "createImageSource by InputStream, new fail.");
        env->DeleteLocalRef(inputBuf);
        free(outBuf);
        outBuf = nullptr;
        return nullptr;
    }
#if !defined(_WIN32) && !defined(_APPLE)
    constexpr int timeoutSeconds = 4;
    // set XCollie timer, baseline: 5ms each 1M
    int xCollieTimerid = OHOS::HiviewDFX::XCollie::GetInstance().SetTimer("ImageSourceTimer", timeoutSeconds,
        nullptr, nullptr, OHOS::HiviewDFX::XCOLLIE_FLAG_LOG | OHOS::HiviewDFX::XCOLLIE_FLAG_RECOVERY);
#endif
    jint bytes = 0;
    while (bytes != -1) {
        if (!ReadInputStreamToBytesArray(env, inputStream, inputBuf, bytes)) {
            HiLog::Error(LABEL, "createImageSource: get input data fail.");
            env->DeleteLocalRef(inputBuf);
            free(outBuf);
            outBuf = nullptr;
            return nullptr;
        }

        if (bytes == -1) {
            break;
        }
        env->GetByteArrayRegion(inputBuf, 0, bytes, outBuf);
        dataStream->write(reinterpret_cast<const char *>(outBuf), bytes);
    }
    env->DeleteLocalRef(inputBuf);
    free(outBuf);
    outBuf = nullptr;
#if !defined(_WIN32) && !defined(_APPLE)
    // cancel XCollie timer
    OHOS::HiviewDFX::XCollie::GetInstance().CancelTimer(xCollieTimerid);
#endif
    HiLog::Debug(LABEL, "ConvertInputStreamToStringStream: done.");
    return dataStream;
}

jobject ohos_media_image_ImageSource_nativeCreateImageSourceFromInputStream(JNIEnv *env, jobject thiz,
                                                                            jobject inputStream, jobject opts)
{
    if (inputStream == nullptr) {
        HiLog::Error(LABEL, "createImageSource by inputStream, param is null.");
        return nullptr;
    }

    SourceOptions sourceOpt;
    if (!ConvertSourceOptions(env, opts, sourceOpt)) {
        HiLog::Error(LABEL, "createImageSource ConvertSourceOptions fail.ed");
        return nullptr;
    }

    std::unique_ptr<std::stringstream> dataStream = ConvertInputStreamToStringStream(env, inputStream);
    if (dataStream == nullptr) {
        HiLog::Error(LABEL, "ConvertInputStreamToStringStream fail.");
        return nullptr;
    }
    uint32_t errCode = ERROR;
    std::unique_ptr<ImageSource> imageSource =
        ImageSource::CreateImageSource(std::move(dataStream), sourceOpt, errCode);
    if (imageSource == nullptr) {
        HiLog::Error(LABEL, "createImageSource by inputStream, create image source fail.");
        return nullptr;
    }
    return env->NewObject(g_imageSourceClass, g_imageSourceConstructMethodId,
                          reinterpret_cast<jlong>(imageSource.release()));
}

jobject ohos_media_image_ImageSource_nativeCreateImageSourceFromByteArray(JNIEnv *env, jobject thiz,
                                                                          jbyteArray data, jint offset,
                                                                          jint length, jobject opts)
{
    if (data == nullptr) {
        HiLog::Error(LABEL, "createImageSource by data, data is null.");
        return nullptr;
    }

    SourceOptions sourceOpt;
    if (!ConvertSourceOptions(env, opts, sourceOpt)) {
        HiLog::Error(LABEL, "createImageSource ConvertSourceOptions fail.ed");
        return nullptr;
    }

    jbyte *dataBuf = env->GetByteArrayElements(data, nullptr);
    if (dataBuf == nullptr) {
        HiLog::Error(LABEL, "createImageSource by data, get parameter str fail.");
        return nullptr;
    }
    uint32_t errCode = ERROR;
    std::unique_ptr<ImageSource> imageSource =
        ImageSource::CreateImageSource(reinterpret_cast<uint8_t *>(dataBuf) + offset, length, sourceOpt, errCode);
    env->ReleaseByteArrayElements(data, dataBuf, 0);  // Release dataBuf
    if (imageSource == nullptr) {
        HiLog::Error(LABEL, "createImageSource by byte array, create image source fail.");
        return nullptr;
    }
    return env->NewObject(g_imageSourceClass, g_imageSourceConstructMethodId,
                          reinterpret_cast<jlong>(imageSource.release()));
}

bool SetIncrementalSourceOpts(JNIEnv *env, jobject opts, IncrementalSourceOptions &increSourceOpts)
{
    if (opts == nullptr) {
        HiLog::Debug(LABEL, "SetIncrementalSourceOpts use default options");
        return true;
    }

    jobject sourceOpts = GetObject(env, opts, "ohos/media/image/ImageSource$IncrementalSourceOptions", "opts",
                                   "Lohos/media/image/ImageSource$SourceOptions;");
    if (sourceOpts != nullptr) {
        jclass optClazz = env->GetObjectClass(sourceOpts);
        if (optClazz == nullptr) {
            env->DeleteLocalRef(sourceOpts);
            return false;
        }
        jfieldID formatFileId = env->GetFieldID(optClazz, "formatHint", "Ljava/lang/String;");
        env->DeleteLocalRef(optClazz);
        if (formatFileId == nullptr) {
            env->DeleteLocalRef(sourceOpts);
            return false;
        }
        jstring format = reinterpret_cast<jstring>(env->GetObjectField(sourceOpts, formatFileId));
        env->DeleteLocalRef(sourceOpts);
        const char *formatHintChars = env->GetStringUTFChars(format, nullptr);
        if (formatHintChars != nullptr) {
            increSourceOpts.sourceOptions.formatHint = formatHintChars;
            env->ReleaseStringUTFChars(format, formatHintChars);  // Release formatHintChars
            env->DeleteLocalRef(format);
        }
    }

    jobject updateModeObj = GetObject(env, opts, "ohos/media/image/ImageSource$IncrementalSourceOptions", "mode",
                                      "Lohos/media/image/ImageSource$UpdateMode;");
    if (updateModeObj != nullptr) {
        jclass updateModeClazz = env->FindClass("ohos/media/image/ImageSource$UpdateMode");
        if (updateModeClazz == nullptr) {
            env->DeleteLocalRef(updateModeObj);
            return false;
        }
        jmethodID getUpdateModeMID = env->GetMethodID(updateModeClazz, "getValue", "()I");
        if (getUpdateModeMID == nullptr) {
            env->DeleteLocalRef(updateModeObj);
            env->DeleteLocalRef(updateModeClazz);
            return false;
        }
        jint incrementalMode = env->CallIntMethod(updateModeObj, getUpdateModeMID);
        increSourceOpts.incrementalMode = static_cast<IncrementalMode>(incrementalMode);
        env->DeleteLocalRef(updateModeObj);
        env->DeleteLocalRef(updateModeClazz);
    }
    return true;
}

jobject ohos_media_image_ImageSource_nativeCreateIncrementalImageSource(JNIEnv *env, jobject thiz, jobject opts)
{
    IncrementalSourceOptions increSourceOpts;
    if (!SetIncrementalSourceOpts(env, opts, increSourceOpts)) {
        HiLog::Error(LABEL, "createIncrementalImageSource : incremental opts para invalid.");
        return nullptr;
    }
    uint32_t errCode = ERROR;
    std::unique_ptr<ImageSource> imageSource = ImageSource::CreateIncrementalImageSource(increSourceOpts, errCode);
    if (imageSource == nullptr) {
        HiLog::Error(LABEL, "createIncrementalImageSource, create image source fail.");
        return nullptr;
    }
    return env->NewObject(g_imageSourceClass, g_imageSourceConstructMethodId,
                          reinterpret_cast<jlong>(imageSource.release()));
}

jint ohos_media_image_ImageSource_nativeUpdateData(JNIEnv *env, jobject thiz, jlong nativePtr, jbyteArray data,
                                                   jint offset, jint size, jboolean isCompleted)
{
    if (data == nullptr) {
        HiLog::Error(LABEL, "nativeUpdateData data is null.");
        return ERR_IMAGE_INVALID_PARAMETER;
    }
    ImageSource *imgSource = reinterpret_cast<ImageSource *>(nativePtr);
    if (imgSource == nullptr) {
        HiLog::Error(LABEL, "nativeUpdateData, get image source fail.");
        return ERR_IMAGE_INIT_ABNORMAL;
    }
    jbyte *dataBuf = env->GetByteArrayElements(data, nullptr);
    jint errCode = imgSource->UpdateData(reinterpret_cast<uint8_t *>(dataBuf) + offset, size, isCompleted);
    env->ReleaseByteArrayElements(data, dataBuf, 0);  // Release dataBuf
    return errCode;
}

bool InitDecOptRect(JNIEnv *env, jobject opts, DecodeOptions &decodeOpts)
{
    jobject rectObj = env->GetObjectField(opts, g_optionsRectFieldId);
    jint left = 0;
    jint top = 0;
    jint rectWidth = 0;
    jint rectHeight = 0;
    if (rectObj != nullptr) {
        if (!GetIntValue(env, rectObj, "ohos/media/image/common/Rect", "minX", left) ||
            !GetIntValue(env, rectObj, "ohos/media/image/common/Rect", "minY", top) ||
            !GetIntValue(env, rectObj, "ohos/media/image/common/Rect", "width", rectWidth) ||
            !GetIntValue(env, rectObj, "ohos/media/image/common/Rect", "height", rectHeight)) {
            HiLog::Error(LABEL, "InitDecOptRect: get rect value fail.");
            env->DeleteLocalRef(rectObj);
            return false;
        }
    }
    decodeOpts.CropRect.left = left;
    decodeOpts.CropRect.top = top;
    decodeOpts.CropRect.width = rectWidth;
    decodeOpts.CropRect.height = rectHeight;

    HiLog::Debug(LABEL,
                 "InitDecOptRect, desiredRegion minX:%{public}d, minY:%{public}d, width:%{public}d, height:%{public}d",
                 left, top, rectWidth, rectHeight);
    if (rectObj != nullptr) {
        env->DeleteLocalRef(rectObj);
    }
    return true;
}

bool InitDecOptSize(JNIEnv *env, jobject opts, DecodeOptions &decodeOpts)
{
    jobject desSizeObj = env->GetObjectField(opts, g_optionsSizeFieldId);
    jint width = 0;
    jint height = 0;
    if (desSizeObj != nullptr) {
        if (!GetIntValue(env, desSizeObj, "ohos/media/image/common/Size", "width", width) ||
            !GetIntValue(env, desSizeObj, "ohos/media/image/common/Size", "height", height)) {
            HiLog::Error(LABEL, "InitDecOptSize: get size value fail.");
            env->DeleteLocalRef(desSizeObj);
            return false;
        }
    }
    decodeOpts.desiredSize.width = width;
    decodeOpts.desiredSize.height = height;
    HiLog::Debug(LABEL, "InitDecOptSize, desiredSize width:%{public}d, height:%{public}d", width, height);
    if (desSizeObj != nullptr) {
        env->DeleteLocalRef(desSizeObj);
    }
    return true;
}

bool InitDecOptPixelFormat(JNIEnv *env, jobject opts, DecodeOptions &decodeOpts)
{
    // desiredPixelFormat
    jmethodID zMethodId = env->GetMethodID(g_pixelFormatClass, "getValue", "()I");
    if (zMethodId == nullptr) {
        HiLog::Error(LABEL, "GetMethodID fail, method: getValue not found.");
        return false;
    }
    jobject pixelFormat = env->GetObjectField(opts, g_optionsPixelFormatFieldId);
    decodeOpts.desiredPixelFormat = static_cast<PixelFormat>(env->CallIntMethod(pixelFormat, zMethodId));
    env->DeleteLocalRef(pixelFormat);
    return true;
}

bool InitDecOptColor(JNIEnv *env, jobject opts, DecodeOptions &decodeOpts)
{
    // desiredColorSpace
    jmethodID zMethodId = env->GetMethodID(g_colorSpaceClass, "getValue", "()I");
    if (zMethodId == nullptr) {
        HiLog::Error(LABEL, "GetMethodID fail, method: getValue not found.");
        return false;
    }
    jobject color = env->GetObjectField(opts, g_optionsColorSpaceFieldId);
    decodeOpts.desiredColorSpace = static_cast<ColorSpace>(env->CallIntMethod(color, zMethodId));
    env->DeleteLocalRef(color);
    return true;
}

bool InitDecOptAllocatorType(JNIEnv *env, jobject opts, DecodeOptions &decodeOpts)
{
    // allocator
    jmethodID zMethodId = env->GetMethodID(g_allocatorClass, "getValue", "()I");
    if (zMethodId == nullptr) {
        HiLog::Error(LABEL, "GetMethodID fail, method: getValue not found.");
        return false;
    }
    jobject allocator = env->GetObjectField(opts, g_optionsAllocatorFieldId);
    decodeOpts.allocatorType = static_cast<AllocatorType>(env->CallIntMethod(allocator, zMethodId));
    env->DeleteLocalRef(allocator);
    return true;
}

bool InitDecOpts(JNIEnv *env, jobject opts, DecodeOptions &decodeOpts)
{
    // fitDensity
    decodeOpts.fitDensity = env->GetIntField(opts, g_optionsFitDensityFieldId);
    // rotateDegrees
    decodeOpts.rotateDegrees = env->GetFloatField(opts, g_optionsRotateDegreesFieldId);
    // sampleSize
    decodeOpts.sampleSize = env->GetIntField(opts, g_optionsSampleSizeFieldId);
    // allowPartialImage
    decodeOpts.allowPartialImage = env->GetBooleanField(opts, g_optionsAllowPartialImageFieldId);
    // editable
    decodeOpts.editable = env->GetBooleanField(opts, g_optionsEditableFieldId);
    return true;
}

bool ConvertDecodingOptions(JNIEnv *env, jobject opts, DecodeOptions &decodeOpts)
{
    if (opts == nullptr) {
        HiLog::Debug(LABEL, "ConvertDecodingOptions use default options");
        return true;
    }

    if (!InitDecOpts(env, opts, decodeOpts) ||
        !InitDecOptRect(env, opts, decodeOpts) || !InitDecOptSize(env, opts, decodeOpts) ||
        !InitDecOptPixelFormat(env, opts, decodeOpts) || !InitDecOptColor(env, opts, decodeOpts) ||
        !InitDecOptAllocatorType(env, opts, decodeOpts)) {
        HiLog::Error(LABEL, "ConvertDecodingOptions : convert para fail");
        return false;
    }

    return true;
}

bool GetNinePatchChunk(JNIEnv *env, const NinePatchInfo &ninePatchInfo, jbyteArray &ninePatchChunk)
{
    if (ninePatchInfo.ninePatch != nullptr && ninePatchInfo.patchSize != 0) {
        ninePatchChunk = env->NewByteArray(ninePatchInfo.patchSize);
        if (ninePatchChunk == nullptr) {
            return false;
        }

        jbyte *array = reinterpret_cast<jbyte *>(env->GetPrimitiveArrayCritical(ninePatchChunk, nullptr));
        if (array == nullptr) {
            HiLog::Error(LABEL, "array is null");
            env->DeleteLocalRef(ninePatchChunk);
            return false;
        }
        errno_t ret = memcpy_s(array, ninePatchInfo.patchSize, ninePatchInfo.ninePatch, ninePatchInfo.patchSize);
        if (ret != 0) {
            HiLog::Error(LABEL, "mempcy nine patch info failed, error is %{public}d.", ret);
            env->ReleasePrimitiveArrayCritical(ninePatchChunk, array, 0);
            env->DeleteLocalRef(ninePatchChunk);
            return false;
        }
        env->ReleasePrimitiveArrayCritical(ninePatchChunk, array, 0);
    }
    return true;
}

jobject ohos_media_image_ImageSource_nativeCreatePixelmap(JNIEnv *env, jobject thiz, jobject sourceObj,
                                                          jlong nativePtr, jint index, jobject opts)
{
    DecodeOptions decodeOpts;
    if (!ConvertDecodingOptions(env, opts, decodeOpts)) {
        HiLog::Error(LABEL, "CreatePixelmap : deopt para is invalid.");
        return nullptr;
    }
    ImageSource *imgSource = reinterpret_cast<ImageSource *>(nativePtr);
    if (imgSource == nullptr) {
        HiLog::Error(LABEL, "CreatePixelmap, get image source fail.");
        return nullptr;
    }
    PixelFormatToNative(decodeOpts.desiredPixelFormat);
    uint32_t err = 0;
    jlong nativePixelMapPtr;
    jlong nativeBytes;
    jbyteArray ninePatchChunk = nullptr;
    jint baseDensity = 0;
    if (imgSource->IsIncrementalSource()) {
        std::unique_ptr<IncrementalPixelMap> incPixelMap = imgSource->CreateIncrementalPixelMap(index, decodeOpts, err);
        if (err != SUCCESS) {
            HiLog::Error(LABEL, "CreateIncrementalPixelMap fail, error is %{public}d.", err);
            return nullptr;
        }
        uint8_t decodeProgress = 0;
        incPixelMap->PromoteDecoding(decodeProgress);
        incPixelMap->DetachFromDecoding();
        nativeBytes = static_cast<jlong>(incPixelMap->GetByteCount());
        PixelMapManager *pixelMapManager = new (std::nothrow) PixelMapManager(incPixelMap.release());
        if (pixelMapManager == nullptr) {
            HiLog::Error(LABEL, "new pixelMapManager failed.");
            return nullptr;
        }
        nativePixelMapPtr = reinterpret_cast<jlong>(pixelMapManager);
    } else {
        std::unique_ptr<PixelMap> pixelMap = imgSource->CreatePixelMap(index, decodeOpts, err);
        if (err != SUCCESS) {
            HiLog::Error(LABEL, "CreatePixelmap fail, error is %{public}d.", err);
            ThrowExceptionByErrorCode(env, err);
            return nullptr;
        }
        const NinePatchInfo &ninePatchInfo = imgSource->GetNinePatchInfo();
        if (!GetNinePatchChunk(env, ninePatchInfo, ninePatchChunk)) {
            HiLog::Error(LABEL, "get nine patch chunk failed.");
            return nullptr;
        }
        nativeBytes = static_cast<jlong>(pixelMap->GetByteCount());
        baseDensity = static_cast<jint>(pixelMap->GetBaseDensity());
        PixelMapManager *pixelMapManager = new (std::nothrow) PixelMapManager(pixelMap.release());
        if (pixelMapManager == nullptr) {
            HiLog::Error(LABEL, "new pixelMapManager failed.");
            env->DeleteLocalRef(ninePatchChunk);
            return nullptr;
        }
        nativePixelMapPtr = reinterpret_cast<jlong>(pixelMapManager);
    }
    jobject pixelMapObj = env->NewObject(g_pixelMapClass, g_pixelMapCtorMethodId,
                                         nativePixelMapPtr, nativeBytes, ninePatchChunk);
    if (pixelMapObj != nullptr && baseDensity > 0) {
        env->CallVoidMethod(pixelMapObj, g_setBaseDensityMethodId, baseDensity);
    }
    return pixelMapObj;
}

jobject ohos_media_image_ImageSource_nativeCreateIncrementalPixelmap(JNIEnv *env, jobject thiz, jlong nativePtr,
                                                                     jint index, jobject opts)
{
    DecodeOptions decodeOpts;
    if (!ConvertDecodingOptions(env, opts, decodeOpts)) {
        HiLog::Error(LABEL, "nativeCreateIncrementalPixelmap : deopt para is invalid.");
        return nullptr;
    }

    ImageSource *imgSource = reinterpret_cast<ImageSource *>(nativePtr);
    if (imgSource == nullptr) {
        HiLog::Error(LABEL, "nativeCreateIncrementalPixelmap, get image source fail.");
        return nullptr;
    }

    jclass pixelMapClazz = env->FindClass("ohos/media/image/IncrementalPixelMap");
    if (pixelMapClazz == nullptr) {
        HiLog::Error(LABEL, "nativeCreateIncrementalPixelmap fail, not found IncrementalPixelMap class.");
        return nullptr;
    }
    jmethodID pixelMapConstrocMID = env->GetMethodID(pixelMapClazz, "<init>", "(J)V");
    if (pixelMapConstrocMID == nullptr) {
        env->DeleteLocalRef(pixelMapClazz);
        HiLog::Error(LABEL, "nativeCreateIncrementalPixelmap fail, PixelMap construction method not found.");
        return nullptr;
    }

    uint32_t err = 0;
    PixelFormatToNative(decodeOpts.desiredPixelFormat);
    std::unique_ptr<IncrementalPixelMap> incPixelMap = imgSource->CreateIncrementalPixelMap(index, decodeOpts, err);
    if (err != SUCCESS) {
        HiLog::Error(LABEL, "CreateIncrementalPixelMap fail, error is %{public}u.", err);
        env->DeleteLocalRef(pixelMapClazz);
        return nullptr;
    }

    jlong nativeBytes = static_cast<jlong>(incPixelMap->GetByteCount());
    return env->NewObject(pixelMapClazz, pixelMapConstrocMID, reinterpret_cast<jlong>(incPixelMap.release()),
                          nativeBytes);
}

jobject ConstructSize(JNIEnv *env, Size &size)
{
    jclass sizeClazz = env->FindClass("ohos/media/image/common/Size");
    if (sizeClazz == nullptr) {
        HiLog::Error(LABEL, "ConstructSize, Size class not found .");
        return nullptr;
    }

    jmethodID sizeConstrocMID = env->GetMethodID(sizeClazz, "<init>", "(II)V");
    if (sizeConstrocMID == nullptr) {
        HiLog::Error(LABEL, "ConstructSize, sizeConstrocMID is null.");
        env->DeleteLocalRef(sizeClazz);
        return nullptr;
    }

    return env->NewObject(sizeClazz, sizeConstrocMID, size.width, size.height);
}

jobject ConstructImageInfo(JNIEnv *env, ImageInfo &imgInfo)
{
    jclass imageInfoClazz = env->FindClass("ohos/media/image/common/ImageInfo");
    if (imageInfoClazz == nullptr) {
        HiLog::Error(LABEL, "ConstructImageInfo, ImageInfo class not found .");
        return nullptr;
    }
    jmethodID imgInfoMID = env->GetMethodID(imageInfoClazz, "<init>", "()V");
    if (imgInfoMID == nullptr) {
        env->DeleteLocalRef(imageInfoClazz);
        return nullptr;
    }
    jobject imgInfoObj = env->NewObject(imageInfoClazz, imgInfoMID);
    if (imgInfoObj == nullptr) {
        HiLog::Error(LABEL, "ConstructImageInfo, imgInfoObj is null.");
        env->DeleteLocalRef(imageInfoClazz);
        return nullptr;
    }

    jobject sizeObj = ConstructSize(env, imgInfo.size);
    if (sizeObj == nullptr) {
        HiLog::Error(LABEL, "ConstructImageInfo, sizeObj is null.");
        env->DeleteLocalRef(imageInfoClazz);
        env->DeleteLocalRef(imgInfoObj);
        return nullptr;
    }
    jfieldID sizeFileID = env->GetFieldID(imageInfoClazz, "size", "Lohos/media/image/common/Size;");
    if (sizeFileID == nullptr) {
        env->DeleteLocalRef(imageInfoClazz);
        env->DeleteLocalRef(imgInfoObj);
        return nullptr;
    }
    env->SetObjectField(imgInfoObj, sizeFileID, sizeObj);

    jmethodID setPixelFormatID = env->GetMethodID(imageInfoClazz, "setPixelFormat", "(I)V");
    if (setPixelFormatID == nullptr) {
        env->DeleteLocalRef(imageInfoClazz);
        env->DeleteLocalRef(imgInfoObj);
        return nullptr;
    }
    env->CallVoidMethod(imgInfoObj, setPixelFormatID, jint(imgInfo.pixelFormat));

    jmethodID setColorSpaceID = env->GetMethodID(imageInfoClazz, "setColorSpace", "(I)V");
    if (setColorSpaceID == nullptr) {
        env->DeleteLocalRef(imageInfoClazz);
        env->DeleteLocalRef(imgInfoObj);
        return nullptr;
    }
    env->CallVoidMethod(imgInfoObj, setColorSpaceID, jint(imgInfo.colorSpace));

    env->DeleteLocalRef(imageInfoClazz);

    return imgInfoObj;
}

jobject ohos_media_image_ImageSource_nativeGetImageInfo(JNIEnv *env, jobject thiz, jlong nativePtr, jint index)
{
    ImageSource *imgSource = reinterpret_cast<ImageSource *>(nativePtr);
    if (imgSource == nullptr) {
        HiLog::Error(LABEL, "nativeGetImageInfo imgSource pointer is null.");
        return nullptr;
    }
    ImageInfo imgInfo;
    uint32_t err = imgSource->GetImageInfo(index, imgInfo);
    if (err != SUCCESS) {
        HiLog::Error(LABEL, "nativeGetImageInfo fail, ret:%{public}u.", err);
        return nullptr;
    }

    return ConstructImageInfo(env, imgInfo);
}

jobject ConstructSourceInfo(JNIEnv *env, SourceInfo &sourceInfo)
{
    jclass sourceInfoClazz = env->FindClass("ohos/media/image/ImageSource$SourceInfo");
    if (sourceInfoClazz == nullptr) {
        HiLog::Error(LABEL, "ConstructSourceInfo, SourceInfo class not found .");
        return nullptr;
    }
    jmethodID sourceInfoConstrocMID = env->GetMethodID(sourceInfoClazz, "<init>", "()V");
    if (sourceInfoConstrocMID == nullptr) {
        HiLog::Error(LABEL, "ConstructSourceInfo, sourceInfoConstrocMID is null.");
        env->DeleteLocalRef(sourceInfoClazz);
        return nullptr;
    }
    jobject sourceInfoObj = env->NewObject(sourceInfoClazz, sourceInfoConstrocMID);
    if (sourceInfoObj == nullptr) {
        HiLog::Error(LABEL, "ConstructSourceInfo, sourceInfoObj is null.");
        env->DeleteLocalRef(sourceInfoClazz);
        return nullptr;
    }

    jfieldID topLevelImageNumFileID = env->GetFieldID(sourceInfoClazz, "topLevelImageNum", "I");
    jfieldID encodedFormatFileID = env->GetFieldID(sourceInfoClazz, "encodedFormat", "Ljava/lang/String;");
    if ((topLevelImageNumFileID == nullptr) || (encodedFormatFileID == nullptr)) {
        HiLog::Error(LABEL, "ConstructSourceInfo, fieldID is null.");
        env->DeleteLocalRef(sourceInfoClazz);
        env->DeleteLocalRef(sourceInfoObj);
        return nullptr;
    }

    jstring encodedFormat = env->NewStringUTF((sourceInfo.encodedFormat).c_str());
    env->SetIntField(sourceInfoObj, topLevelImageNumFileID, sourceInfo.topLevelImageNum);
    env->SetObjectField(sourceInfoObj, encodedFormatFileID, encodedFormat);
    env->DeleteLocalRef(sourceInfoClazz);

    return sourceInfoObj;
}

jobject ohos_media_image_ImageSource_nativeGetSourceInfo(JNIEnv *env, jobject thiz, jlong nativePtr)
{
    ImageSource *imgSource = reinterpret_cast<ImageSource *>(nativePtr);
    if (imgSource == nullptr) {
        HiLog::Error(LABEL, "nativeGetImageInfo imgSource pointer is null.");
        return nullptr;
    }
    uint32_t err = 0;
    SourceInfo sourceInfo = imgSource->GetSourceInfo(err);
    if (err != SUCCESS) {
        HiLog::Error(LABEL, "nativeGetImageInfo fail, ret:%{public}u.", err);
        return nullptr;
    }

    return ConstructSourceInfo(env, sourceInfo);
}

void ohos_media_image_ImageSource_nativeSetMemoryUsagePreference(JNIEnv *env, jobject thiz,
                                                                 jlong nativePtr, jobject preference)
{
    if (preference == nullptr) {
        HiLog::Error(LABEL, "GetObject preference obj is null, use default preference");
        return;
    }
    jclass preferenceClazz = env->GetObjectClass(preference);
    if (preferenceClazz == nullptr) {
        HiLog::Error(LABEL, "SetMemoryUsagePreference: preferenceClazz is null.");
        return;
    }
    jmethodID preferenceValueId = env->GetMethodID(preferenceClazz, "getValue", "()I");
    if (preferenceValueId == nullptr) {
        HiLog::Error(LABEL, "SetMemoryUsagePreference: preferenceValueId is null.");
        return;
    }
    MemoryUsagePreference nativePreference =
        static_cast<MemoryUsagePreference>(env->CallIntMethod(preference,preferenceValueId));

    ImageSource *imgSource = reinterpret_cast<ImageSource *>(nativePtr);
    if (imgSource == nullptr) {
        HiLog::Error(LABEL, "nativeSetMemoryUsagePreference imgSource pointer is null.");
        return;
    }
    imgSource->SetMemoryUsagePreference(nativePreference);
}

jint ohos_media_image_ImageSource_nativeGetImagePropertyInt(JNIEnv *env, jobject thiz, jlong nativePtr, jint index,
                                                            jstring key, jint defaultValue)
{
    ImageSource *imgSource = reinterpret_cast<ImageSource *>(nativePtr);
    if (imgSource == nullptr) {
        HiLog::Error(LABEL, "nativeGetImageInfo imgSource pointer is null.");
        return defaultValue;
    }
    const char *keyChars = env->GetStringUTFChars(key, nullptr);
    HiLog::Error(LABEL, "enter GetImageProperty key:%{public}s", keyChars);
    int32_t value;
    uint32_t errorCode = imgSource->GetImagePropertyInt(index, keyChars, value);
    if (errorCode != SUCCESS) {
        HiLog::Error(LABEL, "GetImageProperty fail, ret:%{public}u.", errorCode);
        env->ReleaseStringUTFChars(key, keyChars);
        return defaultValue;
    }

    env->ReleaseStringUTFChars(key, keyChars);
    HiLog::Error(LABEL, "GetImageProperty key:%{public}s, value:%{public}d.", keyChars, value);
    return value;
}

static const JNINativeMethod METHODS[] = {
    { "nativeSetDecodeEventListener", "(Lohos/media/image/ImageSource;J)V",
      reinterpret_cast<void *>(ohos_media_image_ImageSource_nativeSetDecodeEventListener) },
    { "nativeGetSupportedFormats", "()Ljava/util/HashSet;",
      reinterpret_cast<void *>(ohos_media_image_ImageSource_nativeGetSupportedFormats) },
    { "nativeCreateImageSource",
      "(Ljava/lang/String;Lohos/media/image/ImageSource$SourceOptions;)Lohos/media/image/ImageSource;",
      reinterpret_cast<void *>(ohos_media_image_ImageSource_nativeCreateImageSourceFromFilePath) },
    { "nativeCreateImageSource",
      "(Ljava/io/InputStream;Lohos/media/image/ImageSource$SourceOptions;)Lohos/media/image/ImageSource;",
      reinterpret_cast<void *>(ohos_media_image_ImageSource_nativeCreateImageSourceFromInputStream) },
    { "nativeCreateImageSource",
      "([BIILohos/media/image/ImageSource$SourceOptions;)Lohos/media/image/ImageSource;",
      reinterpret_cast<void *>(ohos_media_image_ImageSource_nativeCreateImageSourceFromByteArray) },
    { "nativeCreateIncrementalImageSource",
      "(Lohos/media/image/ImageSource$IncrementalSourceOptions;)Lohos/media/image/ImageSource;",
      reinterpret_cast<void *>(ohos_media_image_ImageSource_nativeCreateIncrementalImageSource) },
    { "nativeUpdateData", "(J[BIIZ)I", reinterpret_cast<void *>(ohos_media_image_ImageSource_nativeUpdateData) },
    { "nativeCreatePixelmap",
      "(Lohos/media/image/ImageSource;JILohos/media/image/ImageSource$DecodingOptions;)Lohos/media/image/PixelMap;",
      reinterpret_cast<void *>(ohos_media_image_ImageSource_nativeCreatePixelmap) },
    { "nativeGetImageInfo", "(JI)Lohos/media/image/common/ImageInfo;",
      reinterpret_cast<void *>(ohos_media_image_ImageSource_nativeGetImageInfo) },
    { "nativeGetSourceInfo", "(J)Lohos/media/image/ImageSource$SourceInfo;",
      reinterpret_cast<void *>(ohos_media_image_ImageSource_nativeGetSourceInfo) },
    { "nativeGetImagePropertyInt", "(JILjava/lang/String;I)I",
      reinterpret_cast<void *>(ohos_media_image_ImageSource_nativeGetImagePropertyInt) },
    { "nativeInit", "()V", reinterpret_cast<void *>(ohos_media_image_ImageSource_nativeInit) },
    { "nativeRelease", "(J)V", reinterpret_cast<void *>(ohos_media_image_ImageSource_nativeRelease) },
    { "nativeSetMemoryUsagePreference", "(JLohos/media/image/common/MemoryUsagePreference;)V",
      reinterpret_cast<void *>(ohos_media_image_ImageSource_nativeSetMemoryUsagePreference) },
};

jint JNI_OnLoad(JavaVM *vm, void *reserved)
{
    HiLog::Debug(LABEL, "JNI_OnLoad begin");
    JNIEnv *env = nullptr;
    if (vm->GetEnv(reinterpret_cast<void **>(&env), JNI_VERSION_1_4) != JNI_OK) {
        HiLog::Error(LABEL, "JNI_OnLoad: GetEnv failed");
        return ERROR;
    }
    int ret = JkitRegisterNativeMethods(env, "ohos/media/image/ImageSource", METHODS, ARRCOUNT(METHODS));
    if (ret == JNI_ERR) {
        HiLog::Error(LABEL, "JkitRegisterNativeMethods failed, ret=%{public}d", ret);
        return ERROR;
    }
    Jkit::nativeInit(vm);
    HiLog::Debug(LABEL, "JNI_OnLoad end");
    return JNI_VERSION_1_4;
}
