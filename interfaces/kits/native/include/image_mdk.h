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
 * @brief Provides APIs for access to the image interface.
 *
 * @Syscap SystemCapability.Multimedia.Image
 * @since 10
 * @version 2.0
 */

/**
 * @file image_mdk.h
 *
 * @brief Declares functions that access the image rectangle, size, format, and component data.
 * Need link <b>libimagendk.z.so</b>
 *
 * @since 10
 * @version 2.0
 */

#ifndef INTERFACES_KITS_NATIVE_INCLUDE_IMAGE_MDK_H
#define INTERFACES_KITS_NATIVE_INCLUDE_IMAGE_MDK_H
#include "napi/native_api.h"
#include "image_mdk_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Defines an image object at the Capi for the image interface.
 *
 * @since 10
 * @version 2.0
 */
typedef struct ImagePackerCapi  ImagePackerCapi;
typedef struct ImageSourceCapi  ImageSourceCapi;
typedef struct PixelMapCapi  PixelMapCapi;

struct ImageNative_;

/**
 * @brief Defines an image object at the native layer for the image interface.
 *
 * @since 10
 * @version 2.0
 */
typedef struct ImageNative_ ImageNative;

/**
 * @brief Enumerates the image formats.
 *
 * @since 10
 * @version 2.0
 */
enum {
    /** YCbCr422 semi-planar format. */
    OHOS_IMAGE_FORMAT_YCBCR_422_SP = 1000,
    /** JPEG encoding format. */
    OHOS_IMAGE_FORMAT_JPEG = 2000
};

/**
 * @brief Enumerates the image components.
 *
 * @since 10
 * @version 2.0
 */
enum {
    /** Luminance component. */
    OHOS_IMAGE_COMPONENT_FORMAT_YUV_Y = 1,
    /** Chrominance component - blue projection. */
    OHOS_IMAGE_COMPONENT_FORMAT_YUV_U = 2,
    /** Chrominance component - red projection. */
    OHOS_IMAGE_COMPONENT_FORMAT_YUV_V = 3,
    /** JPEG format. */
    OHOS_IMAGE_COMPONENT_FORMAT_JPEG = 4,
};

/**
 * @brief Defines the information about an image rectangle.
 *
 * @since 10
 * @version 2.0
 */
struct OhosImageRect {
    /** X coordinate of the rectangle. */
    int32_t x;
    /** Y coordinate of the rectangle. */
    int32_t y;
    /** Width of the rectangle, in pixels. */
    int32_t width;
    /** Height of the rectangle, in pixels. */
    int32_t height;
};

/**
 * @brief Defines the image composition information.
 *
 * @since 10
 * @version 2.0
 */
struct OhosImageComponent {
    /** Buffer that stores the pixel data. */
    uint8_t* byteBuffer;
    /** Size of the pixel data in the memory. */
    size_t size;
    /** Type of the pixel data. */
    int32_t componentType;
    /** Row stride of the pixel data. */
    int32_t rowStride;
    /** Pixel stride of the pixel data */
    int32_t pixelStride;
};

/**
 * @brief Defines the image packing options.
 *
 * @since 11
 * @version 4.1
 */
struct ImagePacker_Opts_ {
    /** Encoding format. */
    const char* format;
    /** Encoding quality. */
    int quality;
};

/**
 * @brief Defines alias of image packing options.
 *
 * @since 11
 * @version 4.1
 */
typedef struct ImagePacker_Opts_ ImagePacker_Opts;

/**
 * @brief Parses an {@link ImageNative} object at the native layer from a JavaScript native API <b>image </b> object.
 *
 * @param env Indicates the pointer to the Java Native Interface (JNI) environment.
 * @param source Indicates a JavaScript native API <b>image </b> object.
 * @return Returns an {@link ImageNative} pointer object if the operation is successful
 * returns a null pointer otherwise.
 * @see ImageNative, OH_Image_Release
 * @since 10
 * @version 2.0
 */
ImageNative* OH_Image_InitImageNative(napi_env env, napi_value source);

/**
 * @brief Obtains {@link OhosImageRect} of an {@link ImageNative} at the native layer.
 *
 * @param native Indicates the pointer to an {@link ImageNative} object at the native layer.
 * @param rect Indicates the pointer to the {@link OhosImageRect} object obtained.
 * @return Returns {@link IRNdkErrCode} IMAGE_RESULT_SUCCESS - if the operation is successful.
 * returns {@link IRNdkErrCode} IMAGE_RESULT_JNI_ENV_ABNORMAL - if Abnormal JNI environment.
 * returns {@link IRNdkErrCode} IMAGE_RESULT_INVALID_PARAMETER - if invalid parameter.
 * returns {@link IRNdkErrCode} IMAGE_RESULT_SURFACE_GET_PARAMETER_FAILED - if Failed to obtain parameters for surface.
 * returns {@link IRNdkErrCode} IMAGE_RESULT_BAD_PARAMETER - if bad parameter.
 * @see ImageNative, OhosImageRect
 * @since 10
 * @version 2.0
 */
int32_t OH_Image_ClipRect(const ImageNative* native, struct OhosImageRect* rect);

/**
 * @brief Obtains {@link OhosImageSize} of an {@link ImageNative} object at the native layer.
 *
 * @param native Indicates the pointer to an {@link ImageNative} object at the native layer.
 * @param size Indicates the pointer to the {@link OhosImageSize} object obtained.
 * @return Returns {@link IRNdkErrCode} IMAGE_RESULT_SUCCESS - if the operation is successful.
 * returns {@link IRNdkErrCode} IMAGE_RESULT_JNI_ENV_ABNORMAL - if Abnormal JNI environment.
 * returns {@link IRNdkErrCode} IMAGE_RESULT_INVALID_PARAMETER - if invalid parameter.
 * returns {@link IRNdkErrCode} IMAGE_RESULT_SURFACE_GET_PARAMETER_FAILED - if Failed to obtain parameters for surface.
 * returns {@link IRNdkErrCode} IMAGE_RESULT_BAD_PARAMETER - if bad parameter.
 * @see ImageNative, OhosImageSize
 * @since 10
 * @version 2.0
 */
int32_t OH_Image_Size(const ImageNative* native, struct OhosImageSize* size);

/**
 * @brief Obtains the image format of an {@link ImageNative} object at the native layer.
 *
 * @param native Indicates the pointer to an {@link ImageNative} object at the native layer.
 * @param format Indicates the pointer to the image format obtained.
 * @return Returns {@link IRNdkErrCode} IMAGE_RESULT_SUCCESS - if the operation is successful.
 * returns {@link IRNdkErrCode} IMAGE_RESULT_JNI_ENV_ABNORMAL - if Abnormal JNI environment.
 * returns {@link IRNdkErrCode} IMAGE_RESULT_INVALID_PARAMETER - if invalid parameter.
 * returns {@link IRNdkErrCode} IMAGE_RESULT_SURFACE_GET_PARAMETER_FAILED - if Failed to obtain parameters for surface.
 * returns {@link IRNdkErrCode} IMAGE_RESULT_BAD_PARAMETER - if bad parameter.
 * @see ImageNative
 * @since 10
 * @version 2.0
 */
int32_t OH_Image_Format(const ImageNative* native, int32_t* format);

/**
 * @brief Obtains {@link OhosImageComponent} of an {@link ImageNative} object at the native layer.
 *
 * @param native Indicates the pointer to an {@link ImageNative} object at the native layer.
 * @param componentType Indicates the type of the required component.
 * @param componentNative Indicates the pointer to the {@link OhosImageComponent} object obtained.
 * @return Returns {@link IRNdkErrCode} IMAGE_RESULT_SUCCESS - if the operation is successful.
 * returns {@link IRNdkErrCode} IMAGE_RESULT_JNI_ENV_ABNORMAL - if Abnormal JNI environment.
 * returns {@link IRNdkErrCode} IMAGE_RESULT_INVALID_PARAMETER - if invalid parameter.
 * returns {@link IRNdkErrCode} IMAGE_RESULT_SURFACE_GET_PARAMETER_FAILED - if Failed to obtain parameters for surface.
 * returns {@link IRNdkErrCode} IMAGE_RESULT_BAD_PARAMETER - if bad parameter.
 * @see ImageNative, OhosImageComponent
 * @since 10
 * @version 2.0
 */
int32_t OH_Image_GetComponent(const ImageNative* native,
    int32_t componentType, struct OhosImageComponent* componentNative);

/**
 * @brief Releases an {@link ImageNative} object at the native layer.
 * Note: This API is not used to release a JavaScript native API <b>Image</b> object.
 * It is used to release the object {@link ImageNative} at the native layer
 * parsed by calling {@link OH_Image_InitImageNative}.
 *
 * @param native Indicates the pointer to an {@link ImageNative} object at the native layer.
 * @return Returns {@link IRNdkErrCode} IMAGE_RESULT_SUCCESS - if the operation is successful.
 * returns {@link IRNdkErrCode} IMAGE_RESULT_JNI_ENV_ABNORMAL - if Abnormal JNI environment.
 * returns {@link IRNdkErrCode} IMAGE_RESULT_INVALID_PARAMETER - if invalid parameter.
 * returns {@link IRNdkErrCode} IMAGE_RESULT_BAD_PARAMETER - if bad parameter.
 * @see ImageNative, OH_Image_InitImageNative
 * @since 10
 * @version 2.0
 */
int32_t OH_Image_Release(ImageNative* native);

/**
 * @brief This function converts a C array to a JavaScript ArrayBuffer.  
 *
 * @param env Indicates the pointer to the Java Native Interface (JNI) environment.  
 * @param tsArrayBuffer The JavaScript ArrayBuffer object.  
 * @param outData A pointer to the unsigned 8-bit integer array that stores the converted data.  
 * @param size The size of the data.  
 * @return Returns {@link IRNdkErrCode} IMAGE_RESULT_SUCCESS - if the operation is successful.
 * returns {@link IRNdkErrCode} IMAGE_RESULT_BAD_PARAMETER - if bad parameter.
 * @since 11
 * @version 2.0
 */
int32_t OH_Image_CToJs_Array(napi_env env, napi_value tsArrayBuffer, uint8_t* outData, int64_t size);

/**
 * @brief This function converts a JavaScript ImagePacker object to a C++ ImagePacker object.  
 *
 * @param env Indicates the pointer to the Java Native Interface (JNI) environment.  
 * @param tsImagePacker The JavaScript ImagePacker object.  
 * @param imagePacker A pointer to the converted C++ ImagePacker object.
 * @return Returns {@link IRNdkErrCode} IMAGE_RESULT_SUCCESS - if the operation is successful.
 * returns {@link IRNdkErrCode} IMAGE_RESULT_BAD_PARAMETER - if bad parameter.
 * @since 10
 * @version 2.0
 */
int32_t OH_Image_JsToC_ImagePacker(napi_env env, napi_value tsImagePacker, ImagePackerCapi** imagePackerCapi);

/**
 * @brief This function converts a JavaScript value to a 64-bit integer using the specified buffer size.
 *
 * @param env Indicates the pointer to the Java Native Interface (JNI) environment.  
 * @param bufferSize The size of the buffer used for the conversion.
 * @param result A pointer to the converted 64-bit integer value.
 * @return Returns {@link IRNdkErrCode} IMAGE_RESULT_SUCCESS - if the operation is successful.
 * returns {@link IRNdkErrCode} IMAGE_RESULT_BAD_PARAMETER - if bad parameter.
 * @since 10
 * @version 2.0
 */
int32_t OH_Image_JsToC_Int64_t(napi_env env, napi_value bufferSize, int64_t* result);

/**
 * @brief This function converts a JavaScript PixelMap object to a C++ PixelMap object.
 *
 * @param env Indicates the pointer to the Java Native Interface (JNI) environment.  
 * @param tsPixelMap The JavaScript PixelMap object.
 * @param pixelMap A pointer to the converted C++ PixelMap object.
 * @return Returns {@link IRNdkErrCode} IMAGE_RESULT_SUCCESS - if the operation is successful.
 * returns {@link IRNdkErrCode} IMAGE_RESULT_BAD_PARAMETER - if bad parameter.
 * @since 10
 * @version 2.0
 */
int32_t OH_Image_JsToC_PixelMap(napi_env env, napi_value tsPixelMap, PixelMapCapi* pixelMapCapi);

/**
 * @brief This function converts a JavaScript ImageSource object to a C++ ImageSource object.
 *
 * @param env Indicates the pointer to the Java Native Interface (JNI) environment.  
 * @param tsImageSource The JavaScript ImageSource object.
 * @param imageSource A pointer to the converted C++ ImageSource object.
 * @return Returns {@link IRNdkErrCode} IMAGE_RESULT_SUCCESS - if the operation is successful.
 * returns {@link IRNdkErrCode} IMAGE_RESULT_BAD_PARAMETER - if bad parameter.
 * @since 10
 * @version 2.0
 */
int32_t OH_Image_JsToC_ImageSource(napi_env env, napi_value tsImageSource, ImageSourceCapi** imageSourceCapi);

/**
 * @brief This function converts a JavaScript file descriptor to a C integer using the specified environment.
 *
 * @param env Indicates the pointer to the Java Native Interface (JNI) environment.  
 * @param fd The JavaScript file descriptor.
 * @param result A pointer to the converted integer value.  
 * @return Returns {@link IRNdkErrCode} IMAGE_RESULT_SUCCESS - if the operation is successful.
 * returns {@link IRNdkErrCode} IMAGE_RESULT_BAD_PARAMETER - if bad parameter.
 * @since 10
 * @version 2.0
 */
int32_t OH_Image_JsToC_Int(napi_env env, napi_value fd, int* result);

#ifdef __cplusplus
};
#endif
/** @} */
#endif // INTERFACES_KITS_NATIVE_INCLUDE_IMAGE_MDK_H
