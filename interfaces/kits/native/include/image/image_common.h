/*
 * Copyright (C) 2024 Huawei Device Co., Ltd.
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
 * @syscap SystemCapability.Multimedia.Image
 * @since 12
 */

/**
 * @file image_common.h
 *
 * @brief Declares the common enums and structs used by the image interface.
 * 
 * @library NA
 * @syscap SystemCapability.Multimedia.Image
 * @since 12
 */

#ifndef INTERFACES_KITS_NATIVE_INCLUDE_IMAGE_IMAGE_COMMON_H_
#define INTERFACES_KITS_NATIVE_INCLUDE_IMAGE_IMAGE_COMMON_H_
#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif
#define IMAGE_ERRORCODE_BASE 62980096

/**
 * @brief Defines the image size.
 *
 * @since 12
 */
struct Image_Size {
    /** Image width, in pixels. */
    int32_t width;
    /** Image height, in pixels. */
    int32_t height;
};

/**
 * @brief Declaration the image size.
 *
 * @since 12
 */
typedef struct Image_Size Image_Size;

/**
 * @brief Defines the region of the image source to decode.
 *
 * @since 12
 */
struct Image_Region {
    /** X coordinate of the start point, in pixels. */
    uint32_t x;
    /** Y coordinate of the start point, in pixels. */
    uint32_t y;
    /** Width of the region, in pixels. */
    uint32_t width;
    /** Height of the region, in pixels. */
    uint32_t height;
};

/**
 * @brief Declaration the image region.
 *
 * @since 12
 */
typedef struct Image_Region Image_Region;

/**
 * @brief Defines the region of the image source to decode.
 *
 * @since 12
 */
struct Image_String {
    /** data for string type */
    char *format = nullptr;
    /** data lenth for string type */
    size_t size = 0;
};

/**
 * @brief Defines the property string (in key-value format) of the image source.
 *
 * @since 12
 */
typedef Image_String Image_String;

/**
 * @brief Defines the image packing options.
 *
 * @since 12
 */
typedef Image_String Image_MimeType;

/**
 * @brief Enumerates the return values that may be used by the interface.
 *
 * @since 12
 */
typedef enum {
    IMAGE_ERRORCODE_SUCCESS = 0,                                      // Operation success
    IMAGE_ERRORCODE_BAD_PARAMETER = -1,                               // Invalid parameter
    IMAGE_ERRORCODE_IMAGE_ERRORCODE_BASE = IMAGE_ERRORCODE_BASE,            // Operation failed
    IMAGE_ERRORCODE_ERR_IPC = IMAGE_ERRORCODE_BASE + 1,                  // ipc error
    IMAGE_ERRORCODE_ERR_SHAMEM_NOT_EXIST = IMAGE_ERRORCODE_BASE + 2,     // sharememory error
    IMAGE_ERRORCODE_ERR_SHAMEM_DATA_ABNORMAL = IMAGE_ERRORCODE_BASE + 3, // sharememory data abnormal
    IMAGE_ERRORCODE_DECODE_ABNORMAL = IMAGE_ERRORCODE_BASE + 4,          // image decode error
    IMAGE_ERRORCODE_DATA_ABNORMAL = IMAGE_ERRORCODE_BASE + 5,            // image input data error
    IMAGE_ERRORCODE_MALLOC_ABNORMAL = IMAGE_ERRORCODE_BASE + 6,          // image malloc error
    IMAGE_ERRORCODE_DATA_UNSUPPORT = IMAGE_ERRORCODE_BASE + 7,           // image type unsupported
    IMAGE_ERRORCODE_INIT_ABNORMAL = IMAGE_ERRORCODE_BASE + 8,            // image init error
    IMAGE_ERRORCODE_GET_DATA_ABNORMAL = IMAGE_ERRORCODE_BASE + 9,        // image get data error
    IMAGE_ERRORCODE_TOO_LARGE = IMAGE_ERRORCODE_BASE + 10,               // image data too large
    IMAGE_ERRORCODE_TRANSFORM = IMAGE_ERRORCODE_BASE + 11,               // image transform error
    IMAGE_ERRORCODE_COLOR_CONVERT = IMAGE_ERRORCODE_BASE + 12,           // image color convert error
    IMAGE_ERRORCODE_CROP = IMAGE_ERRORCODE_BASE + 13,                    // crop error
    IMAGE_ERRORCODE_SOURCE_DATA = IMAGE_ERRORCODE_BASE + 14,             // image source data error
    IMAGE_ERRORCODE_SOURCE_DATA_INCOMPLETE = IMAGE_ERRORCODE_BASE + 15,  // image source data incomplete
    IMAGE_ERRORCODE_MISMATCHED_FORMAT = IMAGE_ERRORCODE_BASE + 16,       // image mismatched format
    IMAGE_ERRORCODE_UNKNOWN_FORMAT = IMAGE_ERRORCODE_BASE + 17,          // image unknown format
    IMAGE_ERRORCODE_SOURCE_UNRESOLVED = IMAGE_ERRORCODE_BASE + 18,       // image source unresolved
    IMAGE_ERRORCODE_INVALID_PARAMETER = IMAGE_ERRORCODE_BASE + 19,       // image invalid parameter
    IMAGE_ERRORCODE_DECODE_FAILED = IMAGE_ERRORCODE_BASE + 20,           // decode fail
    IMAGE_ERRORCODE_PLUGIN_REGISTER_FAILED = IMAGE_ERRORCODE_BASE + 21,  // register plugin fail
    IMAGE_ERRORCODE_PLUGIN_CREATE_FAILED = IMAGE_ERRORCODE_BASE + 22,    // create plugin fail
    IMAGE_ERRORCODE_ENCODE_FAILED = IMAGE_ERRORCODE_BASE + 23,           // image encode fail
    IMAGE_ERRORCODE_ADD_PIXEL_MAP_FAILED = IMAGE_ERRORCODE_BASE + 24,    // image add pixel map fail
    IMAGE_ERRORCODE_HW_DECODE_UNSUPPORT = IMAGE_ERRORCODE_BASE + 25,     // image hardware decode unsupported
    IMAGE_ERRORCODE_DECODE_HEAD_ABNORMAL = IMAGE_ERRORCODE_BASE + 26,    // image decode head error
    IMAGE_ERRORCODE_DECODE_EXIF_UNSUPPORT = IMAGE_ERRORCODE_BASE + 27,   // image decode exif unsupport
    IMAGE_ERRORCODE_PROPERTY_NOT_EXIST = IMAGE_ERRORCODE_BASE + 28,      // image property not exist

    IMAGE_ERRORCODE_MEDIA_DATA_UNSUPPORT = IMAGE_ERRORCODE_BASE + 30,               // media type unsupported
    IMAGE_ERRORCODE_MEDIA_TOO_LARGE = IMAGE_ERRORCODE_BASE + 31,                    // media data too large
    IMAGE_ERRORCODE_MEDIA_MALLOC_FAILED = IMAGE_ERRORCODE_BASE + 32,                // media malloc memory failed
    IMAGE_ERRORCODE_MEDIA_END_OF_STREAM = IMAGE_ERRORCODE_BASE + 33,                // media end of stream error
    IMAGE_ERRORCODE_MEDIA_IO_ABNORMAL = IMAGE_ERRORCODE_BASE + 34,                  // media io error
    IMAGE_ERRORCODE_MEDIA_MALFORMED = IMAGE_ERRORCODE_BASE + 35,                    // media malformed error
    IMAGE_ERRORCODE_MEDIA_BUFFER_TOO_SMALL = IMAGE_ERRORCODE_BASE + 36,             // media buffer too small error
    IMAGE_ERRORCODE_MEDIA_OUT_OF_RANGE = IMAGE_ERRORCODE_BASE + 37,                 // media out of range error
    IMAGE_ERRORCODE_MEDIA_STATUS_ABNORMAL = IMAGE_ERRORCODE_BASE + 38,              // media status abnormal error
    IMAGE_ERRORCODE_MEDIA_VALUE_INVALID = IMAGE_ERRORCODE_BASE + 39,                // media value invalid
    IMAGE_ERRORCODE_MEDIA_NULL_POINTER = IMAGE_ERRORCODE_BASE + 40,                 // media error operation
    IMAGE_ERRORCODE_MEDIA_INVALID_OPERATION = IMAGE_ERRORCODE_BASE + 41,            // media invalid operation
    IMAGE_ERRORCODE_MEDIA_ERR_PLAYER_NOT_INIT = IMAGE_ERRORCODE_BASE + 42,          // media init error
    IMAGE_ERRORCODE_MEDIA_EARLY_PREPARE = IMAGE_ERRORCODE_BASE + 43,                // media early prepare
    IMAGE_ERRORCODE_MEDIA_SEEK_ERR = IMAGE_ERRORCODE_BASE + 44,                     // media rewind error
    IMAGE_ERRORCODE_MEDIA_PERMISSION_DENIED = IMAGE_ERRORCODE_BASE + 45,            // media permission denied
    IMAGE_ERRORCODE_MEDIA_DEAD_OBJECT = IMAGE_ERRORCODE_BASE + 46,                  // media dead object
    IMAGE_ERRORCODE_MEDIA_TIMED_OUT = IMAGE_ERRORCODE_BASE + 47,                    // media time out
    IMAGE_ERRORCODE_MEDIA_TRACK_NOT_ALL_SUPPORTED = IMAGE_ERRORCODE_BASE + 48,      // media track subset support
    IMAGE_ERRORCODE_MEDIA_ADAPTER_INIT_FAILED = IMAGE_ERRORCODE_BASE + 49,          // media recorder adapter init failed
    IMAGE_ERRORCODE_MEDIA_WRITE_PARCEL_FAIL = IMAGE_ERRORCODE_BASE + 50,            // write parcel failed
    IMAGE_ERRORCODE_MEDIA_READ_PARCEL_FAIL = IMAGE_ERRORCODE_BASE + 51,             // read parcel failed
    IMAGE_ERRORCODE_MEDIA_NO_AVAIL_BUFFER = IMAGE_ERRORCODE_BASE + 52,              // read parcel failed
    IMAGE_ERRORCODE_MEDIA_INVALID_PARAM = IMAGE_ERRORCODE_BASE + 53,                // media function found invalid param
    IMAGE_ERRORCODE_MEDIA_CODEC_ADAPTER_NOT_EXIST = IMAGE_ERRORCODE_BASE + 54,      // media zcodec adapter not init
    IMAGE_ERRORCODE_MEDIA_CREATE_CODEC_ADAPTER_FAILED = IMAGE_ERRORCODE_BASE + 55,  // media create zcodec adapter failed
    IMAGE_ERRORCODE_MEDIA_CODEC_ADAPTER_NOT_INIT = IMAGE_ERRORCODE_BASE + 56,       // media adapter inner not init
    IMAGE_ERRORCODE_MEDIA_ZCODEC_CREATE_FAILED = IMAGE_ERRORCODE_BASE + 57,         // media adapter inner not init
    IMAGE_ERRORCODE_MEDIA_ZCODEC_NOT_EXIST = IMAGE_ERRORCODE_BASE + 58,             // media zcodec not exist
    IMAGE_ERRORCODE_MEDIA_JNI_CLASS_NOT_EXIST = IMAGE_ERRORCODE_BASE + 59,          // media jni class not found
    IMAGE_ERRORCODE_MEDIA_JNI_METHOD_NOT_EXIST = IMAGE_ERRORCODE_BASE + 60,         // media jni method not found
    IMAGE_ERRORCODE_MEDIA_JNI_NEW_OBJ_FAILED = IMAGE_ERRORCODE_BASE + 61,           // media jni obj new failed
    IMAGE_ERRORCODE_MEDIA_JNI_COMMON_ERROR = IMAGE_ERRORCODE_BASE + 62,             // media jni normal error
    IMAGE_ERRORCODE_MEDIA_DISTRIBUTE_NOT_SUPPORT = IMAGE_ERRORCODE_BASE + 63,       // media distribute not support
    IMAGE_ERRORCODE_MEDIA_SOURCE_NOT_SET = IMAGE_ERRORCODE_BASE + 64,               // media source not set
    IMAGE_ERRORCODE_MEDIA_RTSP_ADAPTER_NOT_INIT = IMAGE_ERRORCODE_BASE + 65,        // media rtsp adapter not init
    IMAGE_ERRORCODE_MEDIA_RTSP_ADAPTER_NOT_EXIST = IMAGE_ERRORCODE_BASE + 66,       // media rtsp adapter not exist
    IMAGE_ERRORCODE_MEDIA_RTSP_SURFACE_UNSUPPORT = IMAGE_ERRORCODE_BASE + 67,       // media rtsp surface not support
    IMAGE_ERRORCODE_MEDIA_RTSP_CAPTURE_NOT_INIT = IMAGE_ERRORCODE_BASE + 68,        // media rtsp capture init error
    IMAGE_ERRORCODE_MEDIA_RTSP_SOURCE_URL_INVALID = IMAGE_ERRORCODE_BASE + 69,      // media rtsp source url invalid
    IMAGE_ERRORCODE_MEDIA_RTSP_VIDEO_TRACK_NOT_FOUND = IMAGE_ERRORCODE_BASE + 70,   // media rtsp can't find video track
    IMAGE_ERRORCODE_MEDIA_RTSP_CAMERA_NUM_REACH_MAX = IMAGE_ERRORCODE_BASE + 71,    // rtsp camera num reach to max num
    IMAGE_ERRORCODE_MEDIA_SET_VOLUME = IMAGE_ERRORCODE_BASE + 72,                   // media set volume error
    IMAGE_ERRORCODE_MEDIA_NUMBER_OVERFLOW = IMAGE_ERRORCODE_BASE + 73,              // media number operation overflow
    IMAGE_ERRORCODE_MEDIA_DIS_PLAYER_UNSUPPORTED = IMAGE_ERRORCODE_BASE + 74,       // media distribute player unsupporteded
    IMAGE_ERRORCODE_MEDIA_DENCODE_ICC_FAILED = IMAGE_ERRORCODE_BASE + 75,           // image dencode ICC fail
    IMAGE_ERRORCODE_MEDIA_ENCODE_ICC_FAILED = IMAGE_ERRORCODE_BASE + 76,            // image encode ICC fail

    IMAGE_ERRORCODE_MEDIA_READ_PIXELMAP_FAILED = IMAGE_ERRORCODE_BASE + 150,        // read pixelmap failed
    IMAGE_ERRORCODE_MEDIA_WRITE_PIXELMAP_FAILED = IMAGE_ERRORCODE_BASE + 151,       // write pixelmap failed
    IMAGE_ERRORCODE_MEDIA_PIXELMAP_NOT_ALLOW_MODIFY = IMAGE_ERRORCODE_BASE + 152,   // pixelmap not allow modify
    IMAGE_ERRORCODE_MEDIA_CONFIG_FAILED = IMAGE_ERRORCODE_BASE + 153,               // config error
    IMAGE_ERRORCODE_JNI_ENV_ABNORMAL = IMAGE_ERRORCODE_BASE + 154,                  // Abnormal JNI environment
    IMAGE_ERRORCODE_SURFACE_GRALLOC_BUFFER_FAILED = IMAGE_ERRORCODE_BASE + 155,     // surface gralloc buffer failed
    IMAGE_ERRORCODE_CREATE_SURFACE_FAILED = IMAGE_ERRORCODE_BASE + 156,             // create surface failed
    IMAGE_ERRORCODE_SURFACE_GET_PARAMETER_FAILED = IMAGE_ERRORCODE_BASE + 157,      // Failed to obtain parameters for surface
    IMAGE_ERRORCODE_GET_SURFACE_FAILED = IMAGE_ERRORCODE_BASE + 158,                // get sufrace failed
    IMAGE_ERRORCODE_SURFACE_ACQUIRE_BUFFER_FAILED = IMAGE_ERRORCODE_BASE + 159,     // Acquire Buffer failed
    IMAGE_ERRORCODE_SURFACE_REQUEST_BUFFER_FAILED = IMAGE_ERRORCODE_BASE + 160,     // request Buffer failed
    IMAGE_ERRORCODE_REGISTER_LISTENER_FAILED = IMAGE_ERRORCODE_BASE + 161,          // Failed to register listener
    IMAGE_ERRORCODE_REGISTER_BUFFER_FAILED = IMAGE_ERRORCODE_BASE + 162,            // Failed to register buffer
    IMAGE_ERRORCODE_FREAD_FAILED = IMAGE_ERRORCODE_BASE + 163,                      // read file failed
    IMAGE_ERRORCODE_PEEK_FAILED = IMAGE_ERRORCODE_BASE + 164,                       // peek file failed
    IMAGE_ERRORCODE_SEEK_FAILED = IMAGE_ERRORCODE_BASE + 165,                       // seek file failed
    IMAGE_ERRORCODE_STREAM_SIZE_ERROR = IMAGE_ERRORCODE_BASE + 166,                 // stream bad
    IMAGE_ERRORCODE_FILE_FD_ERROR = IMAGE_ERRORCODE_BASE + 167,                     // file fd is bad
    IMAGE_ERRORCODE_FILE_DAMAGED = IMAGE_ERRORCODE_BASE + 168,                      // file damaged
    IMAGE_ERRORCODE_CREATE_DECODER_FAILED = IMAGE_ERRORCODE_BASE + 169,             // create decoder failed
    IMAGE_ERRORCODE_CREATE_ENCODER_FAILED = IMAGE_ERRORCODE_BASE + 170,             // create encoder failed
    IMAGE_ERRORCODE_CHECK_FORMAT_ERROR = IMAGE_ERRORCODE_BASE + 171,                // check format failed
    IMAGE_ERRORCODE_THIRDPART_SKIA_ERROR = IMAGE_ERRORCODE_BASE + 172,              // skia error
    IMAGE_ERRORCODE_HW_DECODE_FAILED = IMAGE_ERRORCODE_BASE + 173,                  // hard decode failed
    IMAGE_ERRORCODE_ALLOCATER_TYPE_ERROR = IMAGE_ERRORCODE_BASE + 174,              // hard decode failed
    IMAGE_ERRORCODE_ALPHA_TYPE_ERROR = IMAGE_ERRORCODE_BASE + 175,                  // hard decode failed
    IMAGE_ERRORCODE_INDEX_INVALID = IMAGE_ERRORCODE_BASE + 176,                     // invalid index

    IMAGE_ERRORCODE_MEDIA_UNKNOWN = IMAGE_ERRORCODE_BASE + 200,                     // media unknown error
} Image_ErrorCode;

/**
 * @brief Defines the mbp mime type.
 *
 * @since 12
 */
static const char* MIME_TYPE_BMP = "image/bmp";

/**
 * @brief Defines the jpeg mime type.
 *
 * @since 12
 */
static const char* MIME_TYPE_JPEG = "image/jpeg";

/**
 * @brief Defines the heic mime type.
 *
 * @since 12
 */
static const char* MIME_TYPE_HEIC = "image/heic";

/**
 * @brief Defines the png mime type.
 *
 * @since 12
 */
static const char* MIME_TYPE_PNG = "image/png";

/**
 * @brief Defines the webp mime type.
 *
 * @since 12
 */
static const char* MIME_TYPE_WEBP = "image/webp";

/**
 * @brief Defines the gif mime type.
 *
 * @since 12
 */
static const char* MIME_TYPE_GIF = "image/gif";

/**
 * @brief Defines the x-icon mime type.
 *
 * @since 12
 */
static const char* MIME_TYPE_ICON = "image/x-icon";

#ifdef __cplusplus
};
#endif
/** @} */

#endif // INTERFACES_KITS_NATIVE_INCLUDE_IMAGE_IMAGE_COMMON_H_