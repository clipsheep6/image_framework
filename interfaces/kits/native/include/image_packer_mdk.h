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

/**
 * @addtogroup image
 * @{
 *
 * @brief Provides native APIs for encoding image data
 *
 * @Syscap SystemCapability.Multimedia.Image
 * @since 10
 * @version 5.0
 */

/**
 * @file image_packer_mdk.h
 *
 * @brief Declares APIs for encoding image data into buffer or file.
 * Need link <b>libimage_packerndk.z.so</b>
 *
 * @since 10
 * @version 5.0
 */

#ifndef INTERFACES_KITS_NATIVE_INCLUDE_IMAGE_PACKER_MDK_H_
#define INTERFACES_KITS_NATIVE_INCLUDE_IMAGE_PACKER_MDK_H_
#include "napi/native_api.h"
#include "image_mdk_common.h"

#ifdef __cplusplus
extern "C" {
#endif

struct ImagePackerNative_;

/**
 * @brief Defines an image packer object at the native layer for the image packer interface.
 *
 * @since 10
 * @version 5.0
 */
typedef struct ImagePackerNative_ ImagePackerNative;

/**
 * @brief Defines the image packing options.
 *
 * @since 10
 * @version 5.0
 */
struct OhosImagePackerOpts {
    /** Encoding format. */
    const char* format;
    /** Encoding quality. */
    int quality;
    /** Encoding source, a JS pixel map object or a JS image source object. */
    napi_value source;
};

/**
 * @brief Creates an <b>ImagePacker</b> object at the JavaScript native layer.
 *
 * @param env Indicates a pointer to the JavaScript Native Interface (JNI) environment.
 * @param res Indicates a pointer to the <b>ImagePacker</b> object created at the JavaScript native layer.
 * @return Returns {@link IRNdkErrCode} IMAGE_RESULT_SUCCESS - if the operation is successful.
 * returns {@link IRNdkErrCode} IMAGE_RESULT_JNI_ENV_ABNORMAL - if Abnormal NAPI environment.
 * @see {@link ImagePackerNative}
 *
 * @Syscap SystemCapability.Multimedia.Image
 * @since 10
 * @version 5.0
 */
int32_t OH_ImagePacker_Create(napi_env env, napi_value *res);

/**
 * @brief Parses an {@link ImagePackerNative} object at the native layer
 * from a JavaScript native API <b>ImagePacker</b> object.
 *
 * @param env Indicates the pointer to the JavaScript Native Interface (JNI) environment.
 * @param packer Indicates a JavaScript native API <b>ImagePacker</b> object.
 * @return Returns an {@link ImagePackerNative} pointer object if the operation is successful
 * returns a null pointer otherwise.
 * @see ImagePackerNative, OH_ImagePacker_Release
 * @since 10
 * @version 5.0
 */
ImagePackerNative* OH_ImagePacker_InitNative(napi_env env, napi_value packer);

/**
 * @brief Encoding an <b>ImageSource</b> or a <b>PixelMap</b> into the buffer with required format
 *
 * @param native Indicates the pointer to an {@link ImagePacker} object at the native layer.
 * @param opts Indicates the encoding {@link OhosImagePackerOpts} .
 * @param outBuffer Indicates the pointer to the encoded data.
 * @param size Indicates the pointer to the {@link OhosImageComponent} object obtained.
 * @return Returns {@link IRNdkErrCode} IMAGE_RESULT_SUCCESS - if the operation is successful.
 * returns {@link IRNdkErrCode} IMAGE_RESULT_JNI_ENV_ABNORMAL - if Abnormal JNI environment.
 * returns {@link IRNdkErrCode} IMAGE_RESULT_INVALID_PARAMETER - if invalid parameter.
 * returns {@link IRNdkErrCode} IMAGE_RESULT_BAD_PARAMETER - if bad parameter.
 * @see ImageNative, OhosImageComponent
 * @since 10
 * @version 5.0
 */
int32_t OH_ImagePacker_Packing(ImagePackerNative* native, struct OhosImagePackerOpts* opts,
    uint8_t* outBuffer, size_t* size);

/**
 * @brief Encoding an <b>ImageSource</b> or a <b>PixelMap</b> into the a file with fd with required format
 *
 * @param native Indicates the pointer to an {@link ImagePacker} object at the native layer.
 * @param opts Indicates the encoding {@link OhosImagePackerOpts} .
 * @param fd Indicates the a writable file descriptor.
 * @return Returns {@link IRNdkErrCode} IMAGE_RESULT_SUCCESS - if the operation is successful.
 * returns {@link IRNdkErrCode} IMAGE_RESULT_JNI_ENV_ABNORMAL - if Abnormal JNI environment.
 * returns {@link IRNdkErrCode} IMAGE_RESULT_INVALID_PARAMETER - if invalid parameter.
 * returns {@link IRNdkErrCode} IMAGE_RESULT_BAD_PARAMETER - if bad parameter.
 * @see ImageNative, OhosImageComponent
 * @since 10
 * @version 5.0
 */
int32_t OH_ImagePacker_PackingToFile(ImagePackerNative* native, struct OhosImagePackerOpts* opts, int fd);


/**
 * @brief Releases an {@link ImagePackerNative} object at the native layer.
 * Note: This API is not used to release a JavaScript native API <b>ImagePacker</b> object.
 * It is used to release the object {@link ImagePackerNative} at the native layer
 * parsed by calling {@link OH_ImagePacker_InitNative}.
 *
 * @param native Indicates the pointer to an {@link ImageNative} object at the native layer.
 * @return Returns {@link IRNdkErrCode} IMAGE_RESULT_SUCCESS - if the operation is successful.
 * returns {@link IRNdkErrCode} IMAGE_RESULT_JNI_ENV_ABNORMAL - if Abnormal JNI environment.
 * returns {@link IRNdkErrCode} IMAGE_RESULT_INVALID_PARAMETER - if invalid parameter.
 * returns {@link IRNdkErrCode} IMAGE_RESULT_BAD_PARAMETER - if bad parameter.
 * @see ImageNative, OH_Image_InitImageNative
 * @since 10
 * @version 5.0
 */
int32_t OH_ImagePacker_Release(ImagePackerNative* native);
#ifdef __cplusplus
};
#endif
/** @} */
#endif // INTERFACES_KITS_NATIVE_INCLUDE_IMAGE_PACKER_MDK_H_
