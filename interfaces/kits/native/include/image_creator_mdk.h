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
 * 
 * @since 11
 * @version 4.1
 */

/**
 * @file image_creator_mdk.h
 *
 * @brief Declares the APIs for obtaining image data transfer to the native layer.
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

/**
 * @brief Defines an <b>ImageCreator</b> object at the native layer.
 *
 * @since 11
 * @version 4.1
 */
struct ImageCreatorNative_;

/**
 * @brief Defines the data type name of a native image creator.
 *
 * @since 11
 * @version 4.1
 */
typedef struct ImageCreatorNative_ ImageCreatorNative;

/**
 * @brief Defines the callbacks for images at the native layer.
 *
 * @since 11
 * @version 4.1
 */
typedef void (*OH_Image_Creator_On_Callback)(void);

/**
 * @brief Defines the information about an image creator.
 *
 * @since 11
 * @version 4.1
 */
struct OhosImageCreatorInfo {
    /* Default width of the image received by the consumer, in pixels. */
    int32_t width;
    /* Default height of the image received by the consumer, in pixels. */
    int32_t height;
    /* Image format {@link OHOS_IMAGE_FORMAT_JPEG} created by using the creator. */
    int32_t format;
    /* Maximum number of images that can be cached. */
    int32_t capicity;
};

/**
 * @brief Creates an <b>ImageCreator</b> object at the application layer.
 *
 * @param env Indicates the NAPI environment pointer.
 * @param info Indicates the options for setting the <b>ImageCreator</b> object.
 * @param res Indicates the pointer to the <b>ImageCreator</b> object obtained.
 * @return Returns {@link IRNdkErrCode} IMAGE_RESULT_SUCCESS - if the operation is successful.
 * returns {@link IRNdkErrCode} IMAGE_RESULT_BAD_PARAMETER - if bad parameter.
 * returns {@link IRNdkErrCode} IMAGE_RESULT_JNI_ENV_ABNORMAL - if Abnormal JNI environment.
 * returns {@link IRNdkErrCode} IMAGE_RESULT_INVALID_PARAMETER - if invalid parameter.
 * returns {@link IRNdkErrCode} IMAGE_RESULT_SURFACE_GET_PARAMETER_FAILED - if Failed to obtain parameters for surface.
 * returns {@link IRNdkErrCode} IMAGE_RESULT_CREATE_SURFACE_FAILED - if create surface failed.
 * returns {@link IRNdkErrCode} IMAGE_RESULT_SURFACE_GRALLOC_BUFFER_FAILED - if surface gralloc buffer failed.
 * returns {@link IRNdkErrCode} IMAGE_RESULT_GET_SURFACE_FAILED - if get sufrace failed.
 * returns {@link IRNdkErrCode} IMAGE_RESULT_MEDIA_RTSP_SURFACE_UNSUPPORT - if media rtsp surface not support.
 * returns {@link IRNdkErrCode} IMAGE_RESULT_DATA_UNSUPPORT - if image type unsupported.
 * returns {@link IRNdkErrCode} IMAGE_RESULT_MEDIA_DATA_UNSUPPORT - if media type unsupported.
 * @since 11
 * @version 4.1
 */
int32_t OH_Image_Creator_Create(napi_env env, struct OhosImageCreatorInfo info, napi_value* res);

/**
 * @brief Initializes an {@link ImageCreatorNative} object at the native layer
 * through an <b>ImageCreator</b> object at the application layer.
 *
 * @param env Indicates the NAPI environment pointer.
 * @param source Indicates an <b>ImageCreator</b> object.
 * @return Returns the pointer to the {@link ImageCreatorNative} object obtained if the operation is successful;
 * returns a null pointer otherwise.
 * @see {@link OH_Image_Creator_Release}
 * @since 11
 * @version 4.1
 */
ImageCreatorNative* OH_Image_Creator_InitNative(napi_env env, napi_value source);

/**
 * @brief Obtains the creator ID through an {@link ImageCreatorNative} object.
 *
 * @param native Indicates the pointer to an {@link ImageCreatorNative} object at the native layer.
 * @param id Indicates the pointer to the buffer that stores the ID string obtained.
 * @param len Indicates the size of the buffer.
 * @return Returns {@link IRNdkErrCode} IMAGE_RESULT_SUCCESS - if the operation is successful.
 * returns {@link IRNdkErrCode} IMAGE_RESULT_BAD_PARAMETER - if bad parameter.
 * returns {@link IRNdkErrCode} IMAGE_RESULT_JNI_ENV_ABNORMAL - if Abnormal JNI environment.
 * returns {@link IRNdkErrCode} IMAGE_RESULT_INVALID_PARAMETER - if invalid parameter.
 * returns {@link IRNdkErrCode} IMAGE_RESULT_SURFACE_GET_PARAMETER_FAILED - if Failed to obtain parameters for surface.
 * returns {@link IRNdkErrCode} IMAGE_RESULT_GET_SURFACE_FAILED - if get sufrace failed.
 * returns {@link IRNdkErrCode} IMAGE_RESULT_DATA_UNSUPPORT - if image type unsupported.
 * returns {@link IRNdkErrCode} IMAGE_RESULT_MEDIA_DATA_UNSUPPORT - if media type unsupported.
 * @see ImageCreatorNative
 * @since 11
 * @version 4.1
 */
int32_t OH_Image_Creator_Dequeue(const ImageCreatorNative* native, napi_value* image);

/**
 * @brief Obtains the latest image through an {@link ImageCreatorNative} object.
 *
 * @param native Indicates the pointer to an {@link ImageCreatorNative} object at the native layer.
 * @param image Indicates the pointer to an <b>Image</b> object at the application layer.
 * @return Returns {@link IRNdkErrCode} IMAGE_RESULT_SUCCESS - if the operation is successful.
 * returns {@link IRNdkErrCode} IMAGE_RESULT_BAD_PARAMETER - if bad parameter.
 * returns {@link IRNdkErrCode} IMAGE_RESULT_JNI_ENV_ABNORMAL - if Abnormal JNI environment.
 * returns {@link IRNdkErrCode} IMAGE_RESULT_INVALID_PARAMETER - if invalid parameter.
 * returns {@link IRNdkErrCode} IMAGE_RESULT_SURFACE_GET_PARAMETER_FAILED - if Failed to obtain parameters for surface.
 * returns {@link IRNdkErrCode} IMAGE_RESULT_CREATE_SURFACE_FAILED - if create surface failed.
 * returns {@link IRNdkErrCode} IMAGE_RESULT_SURFACE_GRALLOC_BUFFER_FAILED - if surface gralloc buffer failed.
 * returns {@link IRNdkErrCode} IMAGE_RESULT_GET_SURFACE_FAILED - if get sufrace failed.
 * returns {@link IRNdkErrCode} IMAGE_RESULT_MEDIA_RTSP_SURFACE_UNSUPPORT - if media rtsp surface not support.
 * returns {@link IRNdkErrCode} IMAGE_RESULT_DATA_UNSUPPORT - if image type unsupported.
 * returns {@link IRNdkErrCode} IMAGE_RESULT_SURFACE_REQUEST_BUFFER_FAILED - if request Buffer failed.
 * @see ImageCreatorNative
 * @since 11
 * @version 4.1
 */
int32_t OH_Image_Creator_Queue(const ImageCreatorNative* native, napi_value image);

/**
 * @brief Registers an {@link OH_Image_Creator_On_Callback} callback event.
 *
 * This callback event is triggered whenever a new image is received.
 *
 * @param native Indicates the pointer to an {@link ImageCreatorNative} object at the native layer.
 * @param callback Indicates the {@link OH_Image_Creator_On_Callback} callback event to register.
 * @return Returns {@link IRNdkErrCode} IMAGE_RESULT_SUCCESS - if the operation is successful.
 * returns {@link IRNdkErrCode} IMAGE_RESULT_BAD_PARAMETER - if bad parameter.
 * returns {@link IRNdkErrCode} IMAGE_RESULT_JNI_ENV_ABNORMAL - if Abnormal JNI environment.
 * returns {@link IRNdkErrCode} IMAGE_RESULT_INVALID_PARAMETER - if invalid parameter.
 * returns {@link IRNdkErrCode} IMAGE_RESULT_GET_SURFACE_FAILED - if get sufrace failed.
 * returns {@link IRNdkErrCode} IMAGE_RESULT_DATA_UNSUPPORT - if image type unsupported.
 * returns {@link IRNdkErrCode} IMAGE_RESULT_REGISTER_LISTENER_FAILED - if Failed to register listener.
 * returns {@link IRNdkErrCode} IMAGE_RESULT_REGISTER_BUFFER_FAILED - if Failed to register buffer.
 * @see ImageCreatorNative
 * @since 11
 * @version 4.1
 */
int32_t OH_Image_Creator_On(const ImageCreatorNative* native, OH_Image_Creator_On_Callback callback);

/**
 * @brief Obtains the capacity of the image creator through an {@link ImageCreatorNative} object.
 *
 * @param native Indicates the pointer to an {@link ImageCreatorNative} object at the native layer.
 * @param capacity Indicates the pointer to the capacity obtained.
 * @return Returns {@link IRNdkErrCode} IMAGE_RESULT_SUCCESS - if the operation is successful.
 * returns {@link IRNdkErrCode} IMAGE_RESULT_BAD_PARAMETER - if bad parameter.
 * returns {@link IRNdkErrCode} IMAGE_RESULT_JNI_ENV_ABNORMAL - if Abnormal JNI environment.
 * returns {@link IRNdkErrCode} IMAGE_RESULT_INVALID_PARAMETER - if invalid parameter.
 * returns {@link IRNdkErrCode} IMAGE_RESULT_DATA_UNSUPPORT - if image type unsupported.
 * @see ImageCreatorNative, OhosImageSize
 * @since 11
 * @version 4.1
 */
int32_t OH_Image_Creator_GetCapacity(const ImageCreatorNative* native, int32_t* capacity);

/**
 * @brief Obtains the format of the image creator through an {@link ImageCreatorNative} object.
 *
 * @param native Indicates the pointer to an {@link ImageCreatorNative} object at the native layer.
 * @param format Indicates the pointer to the format obtained.
 * @return Returns {@link IRNdkErrCode} IMAGE_RESULT_SUCCESS - if the operation is successful.
 * returns {@link IRNdkErrCode} IMAGE_RESULT_BAD_PARAMETER - if bad parameter.
 * returns {@link IRNdkErrCode} IMAGE_RESULT_JNI_ENV_ABNORMAL - if Abnormal JNI environment.
 * returns {@link IRNdkErrCode} IMAGE_RESULT_INVALID_PARAMETER - if invalid parameter.
 * returns {@link IRNdkErrCode} IMAGE_RESULT_DATA_UNSUPPORT - if image type unsupported.
 * @see ImageCreatorNative

 * @since 11
 * @version 4.1
 */
int32_t OH_Image_Creator_GetFormat(const ImageCreatorNative* native, int32_t* format);

/**
 * @brief Releases an {@link ImageCreatorNative} object at the native layer.
 *
 * This API is not used to release an <b>ImageCreator</b> object at the application layer.
 *
 * @param native Indicates the pointer to an {@link ImageCreatorNative} object at the native layer.
 * @return Returns {@link IRNdkErrCode} IMAGE_RESULT_SUCCESS - if the operation is successful.
 * returns {@link IRNdkErrCode} IMAGE_RESULT_BAD_PARAMETER - if bad parameter.
 * returns {@link IRNdkErrCode} IMAGE_RESULT_JNI_ENV_ABNORMAL - if Abnormal JNI environment.
 * returns {@link IRNdkErrCode} IMAGE_RESULT_INVALID_PARAMETER - if invalid parameter.
 * 
 * @see {@link OH_Image_Creator_InitNative}
 * @see ImageCreatorNative
 * @since 11
 * @version 4.1
 */
int32_t OH_Image_Creator_Release(ImageCreatorNative* native);
#ifdef __cplusplus
};
#endif
/** @} */

#endif // INTERFACES_KITS_NATIVE_INCLUDE_IMAGE_CREATOR_MDK_H_