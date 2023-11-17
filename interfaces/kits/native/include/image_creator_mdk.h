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
 * @brief Provides APIs for obtaining image data transfer to the native layer.
 *
 * The channel for transmitting raw image data module part of image module.
 * It provided a queue used to transmit raw image data from application layer
 * to native layer components
 *
 * @since 11
 * @version 4.1
 */

/**
 * @file image_creator_mdk.h
 *
 * @brief Declares the APIs for obtaining image data transfer to the native layer.
 *
 * The image creator module used to obtain a image buffer and transmist raw image data
 * via buffer to native layer components.
 *
 * The following steps are recommended for this process:
 * Create an image creator object by calling OH_ImageCreator_Create function.
 * And then covert the image creator object to ImageCreator_Native by OH_ImageCreator_InitNative.
 * Next obtaining a image object by OH_ImageCreator_Dequeue and fill it with raw image data.
 * Queueing the obtained image object by OH_ImageCreator_Queue and the native layer components can
 * get the raw image data by graphic private inner apis
 * We can register an event listener OH_ImageCreator_On_Callback by OH_ImageCreator_On listener any
 * image data has been used.
 * Finally, release the ImagePakcerNative by OH_ImageCreator_Release.
 *
 * @library libimagendk.z.so
 * @library libimage_creator_ndk.z.so
 * @syscap SystemCapability.Multimedia.Image
 * @since 11
 * @version 4.1
 */

#ifndef INTERFACES_KITS_NATIVE_INCLUDE_IMAGE_CREATOR_MDK_H_
#define INTERFACES_KITS_NATIVE_INCLUDE_IMAGE_CREATOR_MDK_H_
#include "napi/native_api.h"
#include "image_mdk_common.h"
#include "image_mdk.h"

#ifdef __cplusplus
extern "C" {
#endif

struct ImageCreator_Native_;

/**
 * @brief Defines an image creator object at the native layer for the image creator interface.
 *
 * @since 11
 * @version 4.1
 */
typedef struct ImageCreator_Native_ ImageCreator_Native;

/**
 * @brief Defines the callbacks for image creator at the native layer.
 *
 * @since 11
 * @version 4.1
 */
typedef void (*OH_ImageCreator_On_Callback)(void);

/**
 * @brief Defines the image creator options.
 *
 * @since 11
 * @version 4.1
 */
struct ImageCreator_Opts_ {
    /** Default width of the image creator, in pixels. */
    int32_t width;
    /** Default height of the image creator, in pixels. */
    int32_t height;
    /** Image format created by using the creator. */
    int32_t format;
    /** Maximum number of images that can be cached. */
    int32_t capicity;
};

/**
 * @brief Defines alias of image creator options.
 *
 * @since 11
 * @version 4.1
 */
typedef struct ImageCreator_Opts_ ImageCreator_Opts;

/**
 * @brief Creates an <b>ImageCreator</b> object at the application layer.
 *
 * @param env Indicates a pointer to the JavaScript Native Interface (JNI) environment.
 * @param opts Indicates the encoding {@link ImageCreator_Opts}.
 * @param res Indicates a pointer to the <b>ImageCreator</b> object created at the JavaScript native layer.
 * @return Returns {@link IRNdkErrCode} IMAGE_RESULT_SUCCESS - if the operation is successful.
 * returns {@link IRNdkErrCode} ERR_IMAGE_INVALID_PARAMETER - if invalid parameter.
 * returns {@link IRNdkErrCode} ERR_IMAGE_INIT_ABNORMAL - if environment init abnormal.
 * @since 11
 * @version 4.1
 */
int32_t OH_ImageCreator_Create(napi_env env, ImageCreator_Opts opts, napi_value* res);

/**
 * @brief Initializes an {@link ImageCreator_Native} object at the native layer
 * through an <b>ImageCreator</b> object at the application layer.
 *
 * @param env Indicates a pointer to the JavaScript Native Interface (JNI) environment.
 * @param source Indicates a JavaScript native API <b>ImageCreator</b> object.
 * @return Returns the pointer to the {@link ImageCreator_Native} object obtained if the operation is successful;
 * returns a null pointer otherwise.
 * @see {@link OH_ImageCreator_Release}
 * @since 11
 * @version 4.1
 */
ImageCreator_Native* OH_ImageCreator_InitNative(napi_env env, napi_value source);

/**
 * @brief Obtains an image object from queue through an {@link ImageCreator_Native} object.
 *
 * @param native Indicates the pointer to an {@link ImageCreator_Native} object at the native layer.
 * @param image Indicates the pointer to the buffer that stores image object obtained.
 * @return Returns {@link IRNdkErrCode} IMAGE_RESULT_SUCCESS - if the operation is successful.
 * returns {@link IRNdkErrCode} IMAGE_RESULT_INVALID_PARAMETER - if invalid parameter.
 * returns {@link IRNdkErrCode} IMAGE_RESULT_CREATE_SURFACE_FAILED - if create sufrace failed.
 * returns {@link IRNdkErrCode} IMAGE_RESULT_GET_SURFACE_FAILED - if get sufrace failed.
 * @see {@link OH_ImageCreator_Queue}
 * @since 11
 * @version 4.1
 */
int32_t OH_ImageCreator_Dequeue(const ImageCreator_Native* native, napi_value* image);

/**
 * @brief Queues an image object into queue through an {@link ImageCreator_Native} object.
 *
 * @param native Indicates the pointer to an {@link ImageCreator_Native} object at the native layer.
 * @param image Indicates image object obtained.
 * @return Returns {@link IRNdkErrCode} IMAGE_RESULT_SUCCESS - if the operation is successful.
 * returns {@link IRNdkErrCode} IMAGE_RESULT_INVALID_PARAMETER - if invalid parameter.
 * returns {@link IRNdkErrCode} IMAGE_RESULT_MEDIA_DATA_UNSUPPORT - if invalid parameter.
 * @see {@link OH_ImageCreator_Dequeue}
 * @since 11
 * @version 4.1
 */
int32_t OH_ImageCreator_Queue(const ImageCreator_Native* native, napi_value image);

/**
 * @brief Registers an {@link OH_ImageCreator_On_Callback} callback event.
 *
 * This callback event is triggered whenever an image is release by consumer release surface.
 *
 * @param native Indicates the pointer to an {@link ImageCreator_Native} object at the native layer.
 * @param callback Indicates the {@link OH_ImageCreator_On_Callback} callback event to register.
 * @return Returns {@link IRNdkErrCode} IMAGE_RESULT_SUCCESS - if the operation is successful.
 * returns {@link IRNdkErrCode} IMAGE_RESULT_INVALID_PARAMETER - if invalid parameter.
 * @since 11
 * @version 4.1
 */
int32_t OH_ImageCreator_On(const ImageCreator_Native* native, OH_ImageCreator_On_Callback callback);

/**
 * @brief Gets the capacity of the image creator through an {@link ImageCreator_Native} object.
 *
 * @param native Indicates the pointer to an {@link ImageCreator_Native} object at the native layer.
 * @param capacity Indicates the pointer to the capacity obtained.
 * @return Returns {@link IRNdkErrCode} IMAGE_RESULT_SUCCESS - if the operation is successful.
 * returns {@link IRNdkErrCode} IMAGE_RESULT_INVALID_PARAMETER - if invalid parameter.
 * @see {@link OH_ImageCreator_Create}
 * @since 11
 * @version 4.1
 */
int32_t OH_ImageCreator_GetCapacity(const ImageCreator_Native* native, int32_t* capacity);

/**
 * @brief Gets the format of the image creator through an {@link ImageCreator_Native} object.
 *
 * @param native Indicates the pointer to an {@link ImageCreator_Native} object at the native layer.
 * @param format Indicates the pointer to the format obtained.
 * @return Returns {@link IRNdkErrCode} IMAGE_RESULT_SUCCESS - if the operation is successful.
 * returns {@link IRNdkErrCode} IMAGE_RESULT_INVALID_PARAMETER - if invalid parameter.
 * @see {@link OH_ImageCreator_Create}
 * @since 11
 * @version 4.1
 */
int32_t OH_ImageCreator_GetFormat(const ImageCreator_Native* native, int32_t* format);

/**
 * @brief Releases an {@link ImageCreator_Native} object at the native layer.
 *
 * This API is not used to release an <b>ImageCreator</b> object at the application layer.
 *
 * @param native Indicates the pointer to an {@link ImageCreator_Native} object at the native layer.
 * @return Returns {@link IRNdkErrCode} IMAGE_RESULT_SUCCESS - if the operation is successful.
 * @see {@link OH_ImageCreator_InitNative}
 * @since 11
 * @version 4.1
 */
int32_t OH_ImageCreator_Release(ImageCreator_Native* native);
#ifdef __cplusplus
};
#endif
/** @} */

#endif // INTERFACES_KITS_NATIVE_INCLUDE_IMAGE_CREATOR_MDK_H_