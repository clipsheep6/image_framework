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

#include <mutex>
#include "ohos_image_jni_utils.h"
#include "hilog/log.h"
#include "image_packer_ex.h"
#include "image_source.h"
#include "jkit_utils.h"
#include "log_tags.h"
#include "media_errors.h"
#include "pixel_map_manager.h"

using namespace OHOS::HiviewDFX;
using namespace OHOS::Media;

static constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, LOG_TAG_DOMAIN_ID_IMAGE, "ImagePacker_JNI" };
static std::mutex g_imageMutex;

jobject ohos_media_image_ImagePacker_nativeCreateImagePacker(JNIEnv *env, jobject thiz)
{
    HiLog::Debug(LABEL, "nativeCreateImagePacker");
    std::lock_guard<std::mutex> lg(g_imageMutex);

    jclass imageClazz = env->FindClass("ohos/media/image/ImagePacker");
    if (imageClazz == nullptr) {
        HiLog::Error(LABEL, "find ImagePacker class fail");
        return nullptr;
    }

    jmethodID constructorMethodID = env->GetMethodID(imageClazz, "<init>", "(J)V");
    if (constructorMethodID == nullptr) {
        HiLog::Error(LABEL, "find constructorMethod fail");
        env->DeleteLocalRef(imageClazz);
        return nullptr;
    }

    ImagePackerEx *imagePacker = new (std::nothrow) ImagePackerEx();
    if (imagePacker == nullptr) {
        HiLog::Error(LABEL, "ImagePacker is nullptr");
        env->DeleteLocalRef(imageClazz);
        return nullptr;
    }

    jobject newImagePacker = env->NewObject(imageClazz, constructorMethodID, reinterpret_cast<jlong>(imagePacker));
    env->DeleteLocalRef(imageClazz);
    return newImagePacker;
}

void ohos_media_image_ImagePacker_nativeRelease(JNIEnv *env, jobject thiz, jlong nativePtr)
{
    HiLog::Debug(LABEL, "nativeRelease begin");
    ImagePackerEx *imagePacker = reinterpret_cast<ImagePackerEx *>(nativePtr);
    if (imagePacker != nullptr) {
        delete imagePacker;
        imagePacker = nullptr;
    }
    HiLog::Debug(LABEL, "nativeRelease end");
}

static jobject ohos_media_image_ImagePacker_nativeGetSupportedFormats(JNIEnv *env, jobject thiz)
{
    std::set<std::string> formats;
    if (ImagePacker::GetSupportedFormats(formats) != SUCCESS) {
        HiLog::Error(LABEL, "GetSupportedFormats is fail.");
        return nullptr;
    }

    jclass listClazz = env->FindClass("java/util/HashSet");
    if (listClazz == nullptr) {
        HiLog::Error(LABEL, "HashSet class not find!");
        return nullptr;
    }

    jmethodID listInit = env->GetMethodID(listClazz, "<init>", "()V");
    if (listInit == nullptr) {
        HiLog::Error(LABEL, "listInit not find!");
        env->DeleteLocalRef(listClazz);
        return nullptr;
    }

    jobject listObj = env->NewObject(listClazz, listInit, "");
    if (listObj == nullptr) {
        HiLog::Error(LABEL, "listObj not find!");
        env->DeleteLocalRef(listClazz);
        return nullptr;
    }
    jmethodID listAdd = env->GetMethodID(listClazz, "add", "(Ljava/lang/Object;)Z");
    env->DeleteLocalRef(listClazz);
    if (listAdd == nullptr) {
        HiLog::Error(LABEL, "listInit not find!");
        env->DeleteLocalRef(listObj);
        return nullptr;
    }
    std::set<std::string>::iterator iter = formats.begin();
    while (iter != formats.end()) {
        env->CallBooleanMethod(listObj, listAdd, env->NewStringUTF((*iter).c_str()));
        ++iter;
    }
    return listObj;
}

bool SetPackOption(JNIEnv *env, jobject option, PackOption &packOption)
{
    jclass packOptionClazz = env->GetObjectClass(option);
    if (packOptionClazz == nullptr) {
        HiLog::Error(LABEL, "packOptionClazz is null.");
        return false;
    }
    jfieldID formatFileId = env->GetFieldID(packOptionClazz, "format", "Ljava/lang/String;");
    jfieldID qualityFileId = env->GetFieldID(packOptionClazz, "quality", "I");
    jfieldID numberHintFileId = env->GetFieldID(packOptionClazz, "numberHint", "I");
    env->DeleteLocalRef(packOptionClazz);
    if (formatFileId == nullptr) {
        HiLog::Error(LABEL, "formatFileId is null.");
        return false;
    }
    if (qualityFileId == nullptr) {
        HiLog::Error(LABEL, "qualityFileId is null.");
        return false;
    }
    if (numberHintFileId == nullptr) {
        HiLog::Error(LABEL, "numberHintFileId is null.");
        return false;
    }
    jstring format = reinterpret_cast<jstring>(env->GetObjectField(option, formatFileId));
    if (format == nullptr) {
        HiLog::Error(LABEL, "nativeStartPacking format is null.");
        return false;
    }
    const char *formatStr = env->GetStringUTFChars(format, nullptr);
    if (formatStr == nullptr) {
        HiLog::Error(LABEL, "format is invalid");
        return false;
    }
    packOption.format = formatStr;
    packOption.quality = env->GetIntField(option, qualityFileId);
    packOption.numberHint = env->GetIntField(option, numberHintFileId);
    env->ReleaseStringUTFChars(format, formatStr);
    return true;
}

jint ohos_media_image_ImagePacker_nativeStartPackingByArray(JNIEnv *env, jobject thiz, jlong nativePtr,
                                                            jbyteArray data, jint offset, jobject option)
{
    if ((data == nullptr) || (option == nullptr)) {
        HiLog::Error(LABEL, "nativeStartPackingByArray parameter is illegal.");
        return ERR_IMAGE_INVALID_PARAMETER;
    }
    jsize size = env->GetArrayLength(data);
    if (size == 0) {
        HiLog::Error(LABEL, "nativeStartPackingByArray data size is zero.");
        return ERR_IMAGE_INVALID_PARAMETER;
    }
    if (offset < 0 || size - offset < 0) {
        HiLog::Error(LABEL, "nativeStartPackingByArray offset %{public}d and size %{public}d invalid", offset, size);
        return ERR_IMAGE_INVALID_PARAMETER;
    }

    PackOption packOption;
    if (!SetPackOption(env, option, packOption)) {
        return ERR_IMAGE_INVALID_PARAMETER;
    }

    ImagePackerEx *imgPacker = reinterpret_cast<ImagePackerEx *>(nativePtr);
    if (imgPacker == nullptr) {
        HiLog::Error(LABEL, "nativeStartPackingByArray get image packer native obj is null.");
        return ERR_IMAGE_INIT_ABNORMAL;
    }
    jbyte *dataBuf = env->GetByteArrayElements(data, nullptr);
    jint errCode = imgPacker->StartPacking(reinterpret_cast<uint8_t *>(dataBuf) + offset, size - offset, packOption);
    env->ReleaseByteArrayElements(data, dataBuf, 0);  // Release dataBuf
    return errCode;
}

class OutputStreamWrapper : public PackerStream {
public:
    OutputStreamWrapper(JNIEnv *env, jobject outputStream, jbyteArray buffer, jmethodID writeMethodId,
                        jmethodID flushMethodId)
        : env_(env),
          outputStream_(env->NewGlobalRef(outputStream)),
          buffer_(reinterpret_cast<jbyteArray>(env->NewGlobalRef(buffer))),
          bufferSize_(env->GetArrayLength(buffer)),
          writeMethodId_(writeMethodId),
          flushMethodId_(flushMethodId),
          bytesWritten_(0)
    {}

    ~OutputStreamWrapper()
    {
        if (outputStream_ != nullptr) {
            env_->DeleteGlobalRef(outputStream_);
        }

        if (buffer_ != nullptr) {
            env_->DeleteGlobalRef(buffer_);
        }
    }

    virtual int64_t BytesWritten()
    {
        return bytesWritten_;
    }

    virtual bool Write(const uint8_t *buffer, uint32_t size)
    {
        if (outputStream_ == nullptr || buffer_ == nullptr) {
            HiLog::Error(LABEL, "Write outputStream or buffer is null, check constructor firstly.");
            return false;
        }

        int64_t wSize = size;
        while (wSize > 0) {
            jint needSize;
            if (wSize > bufferSize_) {
                needSize = bufferSize_;
            } else {
                needSize = static_cast<jint>(wSize);
            }

            env_->SetByteArrayRegion(buffer_, 0, needSize, reinterpret_cast<const jbyte *>(buffer));
            if (env_->ExceptionCheck()) {
                env_->ExceptionClear();
                HiLog::Error(LABEL, "set byte array elements throw exception");
                return false;
            }

            env_->CallVoidMethod(outputStream_, writeMethodId_, buffer_, 0, needSize);
            if (env_->ExceptionCheck()) {
                env_->ExceptionClear();
                HiLog::Error(LABEL, "write throw exception.");
                return false;
            }

            buffer += needSize;
            wSize -= needSize;
            bytesWritten_ += needSize;
        }
        return true;
    }

    virtual void Flush()
    {
        if (outputStream_ == nullptr) {
            HiLog::Error(LABEL, "Flush outputStream is null, check constructor firstly.");
            return;
        }
        env_->CallVoidMethod(outputStream_, flushMethodId_);
    }

private:
    JNIEnv *env_;
    jobject outputStream_;
    jbyteArray buffer_;
    jint bufferSize_;
    jmethodID writeMethodId_;
    jmethodID flushMethodId_;
    int64_t bytesWritten_;
};

jint ohos_media_image_ImagePacker_nativeStartPackingByStream(JNIEnv *env, jobject thiz, jlong nativePtr,
                                                             jobject outputStream, jobject option,
                                                             jbyteArray tmpBuffer)
{
    if ((outputStream == nullptr) || (option == nullptr) || tmpBuffer == nullptr) {
        HiLog::Error(LABEL, "nativeStartPackingByStream parameter is illegal.");
        return ERR_IMAGE_INVALID_PARAMETER;
    }

    PackOption packOption;
    if (!SetPackOption(env, option, packOption)) {
        return ERR_IMAGE_INVALID_PARAMETER;
    }

    ImagePackerEx *imgPacker = reinterpret_cast<ImagePackerEx *>(nativePtr);
    if (imgPacker == nullptr) {
        HiLog::Error(LABEL, "nativeStartPackingByStream get image packer native obj is null.");
        return ERR_IMAGE_INIT_ABNORMAL;
    }

    jclass outputStreamClazz = env->GetObjectClass(outputStream);
    if (outputStreamClazz == nullptr) {
        HiLog::Error(LABEL, "nativeStartPackingByStream class(java/io/OutputStream) not found.");
        return ERR_IMAGE_INVALID_PARAMETER;
    }

    jmethodID writeMethodId = env->GetMethodID(outputStreamClazz, "write", "([BII)V");
    jmethodID flushMethodId = env->GetMethodID(outputStreamClazz, "flush", "()V");
    env->DeleteLocalRef(outputStreamClazz);
    if (writeMethodId == nullptr || flushMethodId == nullptr) {
        HiLog::Error(LABEL, "nativeStartPackingByStream java/io/OutputStream write or flush method not found.");
        return ERR_IMAGE_INVALID_PARAMETER;
    }

    // ImagePacker manage the outputWrapper lifecycle
    PackerStream *outputWrapper =
        new (std::nothrow) OutputStreamWrapper(env, outputStream, tmpBuffer, writeMethodId, flushMethodId);
    if (outputWrapper == nullptr) {
        HiLog::Error(LABEL, "nativeStartPackingByStream by outputStream, new bytearray fail.");
        return ERR_IMAGE_MALLOC_ABNORMAL;
    }
    return imgPacker->StartPacking(*outputWrapper, packOption);
}

jint ohos_media_image_ImagePacker_nativeAddImage(JNIEnv *env, jobject thiz, jlong nativePtr, jobject pixelmap)
{
    if (pixelmap == nullptr) {
        HiLog::Error(LABEL, "nativeAddImage parameter is null.");
        return ERR_IMAGE_INVALID_PARAMETER;
    }
    jclass pixelMapClazz = env->GetObjectClass(pixelmap);
    if (pixelMapClazz == nullptr) {
        HiLog::Error(LABEL, "GetNativePixelMap, PixelMap class not found.");
        return ERR_IMAGE_INVALID_PARAMETER;
    }
    jfieldID pixelMapFileId = env->GetFieldID(pixelMapClazz, "nativeImagePixelMap", "J");
    env->DeleteLocalRef(pixelMapClazz);
    if (pixelMapFileId == nullptr) {
        HiLog::Error(LABEL, "get nativeImagePixelMap error.");
        return ERR_IMAGE_INVALID_PARAMETER;
    }
    jlong pixelMapPtr = env->GetLongField(pixelmap, pixelMapFileId);
    PixelMapManager *pixelMapManager = reinterpret_cast<PixelMapManager *>(pixelMapPtr);
    if (pixelMapManager == nullptr || pixelMapManager->Invalid()) {
        HiLog::Error(LABEL, "GetNativePixelMap pixel map is null.");
        return ERR_IMAGE_INVALID_PARAMETER;
    }
    ImagePackerEx *imgPacker = reinterpret_cast<ImagePackerEx *>(nativePtr);
    if (imgPacker == nullptr) {
        HiLog::Error(LABEL, "get image packer native obj is null.");
        return ERR_IMAGE_INIT_ABNORMAL;
    }
    return imgPacker->AddImage(pixelMapManager->GetPixelMap());
}

jint ohos_media_image_ImagePacker_nativeAddIndexImageFormSource(JNIEnv *env, jobject thiz, jlong nativePtr,
                                                                jobject source, jint index)
{
    jclass sourceClazz = env->GetObjectClass(source);
    if (sourceClazz == nullptr) {
        HiLog::Error(LABEL, "sourceClazz is null.");
        return ERR_IMAGE_INVALID_PARAMETER;
    }
    jfieldID sourceFileId = env->GetFieldID(sourceClazz, "nativeImageSource", "J");
    if (sourceFileId == nullptr) {
        HiLog::Error(LABEL, "sourceFileId is null.");
        env->DeleteLocalRef(sourceClazz);
        return ERR_IMAGE_INVALID_PARAMETER;
    }
    env->DeleteLocalRef(sourceClazz);
    jlong sourcePtr = env->GetLongField(source, sourceFileId);
    ImageSource *imgSource = reinterpret_cast<ImageSource *>(sourcePtr);
    if (imgSource == nullptr) {
        HiLog::Error(LABEL, "get image source native obj is null.");
        return ERR_IMAGE_INIT_ABNORMAL;
    }
    ImagePackerEx *imgPacker = reinterpret_cast<ImagePackerEx *>(nativePtr);
    if (imgPacker == nullptr) {
        HiLog::Error(LABEL, "get image packer native obj is null.");
        return ERR_IMAGE_INIT_ABNORMAL;
    }
    return imgPacker->AddImage(*imgSource, index);
}

jint ohos_media_image_ImagePacker_nativeAddImageFormSource(JNIEnv *env, jobject thiz, jlong nativePtr,
                                                           jobject source)
{
    return ohos_media_image_ImagePacker_nativeAddIndexImageFormSource(env, thiz, nativePtr, source, 0);
}

jlong ohos_media_image_ImagePacker_nativeFinalizePacking(JNIEnv *env, jobject thiz, jlong nativePtr)
{
    ImagePackerEx *imgPacker = reinterpret_cast<ImagePackerEx *>(nativePtr);
    if (imgPacker == nullptr) {
        HiLog::Error(LABEL, "get image packer native obj is null.");
        return -1L;
    }
    int64_t bytes = 0;
    uint32_t errorCode = imgPacker->FinalizePacking(bytes);
    if (errorCode != SUCCESS) {
        HiLog::Error(LABEL, "FinalizePacking fail: %{public}u.", errorCode);
        return -1L;
    }
    HiLog::Debug(LABEL, "FinalizePacking write %{public}lld bytes.", static_cast<long long>(bytes));
    jlong ret{ bytes };
    return ret;
}

static const JNINativeMethod METHODS[] = {
    { "nativeGetSupportedFormats", "()Ljava/util/HashSet;",
      reinterpret_cast<void *>(ohos_media_image_ImagePacker_nativeGetSupportedFormats) },
    { "nativeStartPacking", "(J[BILohos/media/image/ImagePacker$PackingOptions;)I",
      reinterpret_cast<void *>(ohos_media_image_ImagePacker_nativeStartPackingByArray) },
    { "nativeStartPacking", "(JLjava/io/OutputStream;Lohos/media/image/ImagePacker$PackingOptions;[B)I",
      reinterpret_cast<void *>(ohos_media_image_ImagePacker_nativeStartPackingByStream) },
    { "nativeAddImage", "(JLohos/media/image/PixelMap;)I",
      reinterpret_cast<void *>(ohos_media_image_ImagePacker_nativeAddImage) },
    { "nativeAddImage", "(JLohos/media/image/ImageSource;)I",
      reinterpret_cast<void *>(ohos_media_image_ImagePacker_nativeAddImageFormSource) },
    { "nativeAddImage", "(JLohos/media/image/ImageSource;I)I",
      reinterpret_cast<void *>(ohos_media_image_ImagePacker_nativeAddIndexImageFormSource) },
    { "nativeFinalizePacking", "(J)J",
      reinterpret_cast<void *>(ohos_media_image_ImagePacker_nativeFinalizePacking) },
    { "nativeCreateImagePacker", "()Lohos/media/image/ImagePacker;",
      reinterpret_cast<void *>(ohos_media_image_ImagePacker_nativeCreateImagePacker) },
    { "nativeRelease", "(J)V", reinterpret_cast<void *>(ohos_media_image_ImagePacker_nativeRelease) },
};

jint JNI_OnLoad(JavaVM *vm, void *reserved)
{
    HiLog::Debug(LABEL, "JNI_OnLoad begin");
    JNIEnv *env = nullptr;
    if (vm->GetEnv(reinterpret_cast<void **>(&env), JNI_VERSION_1_4) != JNI_OK) {
        HiLog::Error(LABEL, "JNI_OnLoad: GetEnv failed");
        return ERROR;
    }
    int ret = JkitRegisterNativeMethods(env, "ohos/media/image/ImagePacker", METHODS, ARRCOUNT(METHODS));
    if (ret == JNI_ERR) {
        HiLog::Error(LABEL, "JkitRegisterNativeMethods failed, ret=%{public}d", ret);
        return ERROR;
    }
    Jkit::nativeInit(vm);
    HiLog::Debug(LABEL, "JNI_OnLoad end");
    return JNI_VERSION_1_4;
}
