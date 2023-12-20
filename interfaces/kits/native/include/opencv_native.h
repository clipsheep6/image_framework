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
 * @brief Provides native APIs for accessing to opencv computer vision library processing.
 *
 * The native APIs library of computer vision processing module part of image module.
 * It provide multiple visual computing operations.
 *
 * @since 11
 * @version 4.1
 */

/**
 * @file opencv_native.h
 *
 * @brief Declares native APIs for accessing to opencv computer vision library processing.
 *
 * The native APIs library of computer vision processing module part of image module.
 *
 * It provide ability of opencv as following:
 * erode, dilate, morphologyEx, resize, warpAffine, warpPerspective, remap,
 * getRotationMatrix2D, getPerspectiveTransform, sobel, threshold, cvtColor,
 * blur, fillPoly, adaptiveThreshold, findCounter, boxFilter, borderInterpolate,
 * countNonZero, copyMakeBorder, split, merge, minMaxLoc, normalize, mixChannels
 *
 * @library libopencv_ndk.z.so
 * @syscap SystemCapability.Multimedia.Image
 * @since 11
 * @version 4.1
 */

#ifndef INTERFACES_KITS_NATIVE_INCLUDE_OPENCV_NATIVE_H_
#define INTERFACES_KITS_NATIVE_INCLUDE_OPENCV_NATIVE_H_

#include "image_mdk_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Enumerates the matrix type for CV_Mat struct
 *
 * @since 11
 * @version 4.1
 */
enum CV_Mat_Type_ {
    /**
     * Unsigned 8 bits base type
     */
    CV_MAT_TYPE_8U = 0,
    /**
     * Signed 8 bits base type
     */
    CV_MAT_TYPE_8S = 1,
    /**
     * Unsigned 16 bits base type
     */
    CV_MAT_TYPE_16U = 2,
    /**
     * Signed 16 bits base type
     */
    CV_MAT_TYPE_16S = 3,
    /**
     * Signed 32 bits base type
     */
    CV_MAT_TYPE_32S = 4,
    /**
     * 32 bits float base type
     */
    CV_MAT_TYPE_32F = 5,
    /**
     * 64 bits float base type
     */
    CV_MAT_TYPE_64F = 6,
    /**
     * 16 bits float base type
     */
    CV_MAT_TYPE_16F = 7,
    /**
     * Unsigned 8 bits 1 channel type
     */
    CV_MAT_TYPE_8UC1 = CV_MAT_TYPE_8U,
    /**
     * Unsigned 8 bits 2 channels type
     */
    CV_MAT_TYPE_8UC2 = CV_MAT_TYPE_8U + 8,
    /**
     * Unsigned 8 bits 3 channels type
     */
    CV_MAT_TYPE_8UC3 = CV_MAT_TYPE_8U + 16,
    /**
     * Unsigned 8 bits 4 channels type
     */
    CV_MAT_TYPE_8UC4 = CV_MAT_TYPE_8U + 24,
    /**
     * Signed 8 bits 1 channel type
     */
    CV_MAT_TYPE_8SC1 = CV_MAT_TYPE_8S,
    /**
     * Signed 8 bits 2 channels type
     */
    CV_MAT_TYPE_8SC2 = CV_MAT_TYPE_8S + 8,
    /**
     * Signed 8 bits 3 channels type
     */
    CV_MAT_TYPE_8SC3 = CV_MAT_TYPE_8S + 16,
    /**
     * Signed 8 bits 4 channels type
     */
    CV_MAT_TYPE_8SC4 = CV_MAT_TYPE_8S + 24,
    /**
     * Unsigned 16 bits 1 channel type
     */
    CV_MAT_TYPE_16UC1 = CV_MAT_TYPE_16U,
    /**
     * Unsigned 16 bits 2 channels type
     */
    CV_MAT_TYPE_16UC2 = CV_MAT_TYPE_16U + 8,
    /**
     * Unsigned 16 bits 3 channels type
     */
    CV_MAT_TYPE_16UC3 = CV_MAT_TYPE_16U + 16,
    /**
     * Unsigned 16 bits 4 channels type
     */
    CV_MAT_TYPE_16UC4 = CV_MAT_TYPE_16U + 24,
    /**
     * Signed 16 bits 1 channel type
     */
    CV_MAT_TYPE_16SC1 = CV_MAT_TYPE_16S,
    /**
     * Signed 16 bits 2 channels type
     */
    CV_MAT_TYPE_16SC2 = CV_MAT_TYPE_16S + 8,
    /**
     * Signed 16 bits 3 channels type
     */
    CV_MAT_TYPE_16SC3 = CV_MAT_TYPE_16S + 16,
    /**
     * Signed 16 bits 4 channels type
     */
    CV_MAT_TYPE_16SC4 = CV_MAT_TYPE_16S + 24,
    /**
     * Signed 32 bits 1 channel type
     */
    CV_MAT_TYPE_32SC1 = CV_MAT_TYPE_32S,
    /**
     * Signed 32 bits 2 channels type
     */
    CV_MAT_TYPE_32SC2 = CV_MAT_TYPE_32S + 8,
    /**
     * Signed 32 bits 3 channels type
     */
    CV_MAT_TYPE_32SC3 = CV_MAT_TYPE_32S + 16,
    /**
     * Signed 32 bits 4 channels type
     */
    CV_MAT_TYPE_32SC4 = CV_MAT_TYPE_32S + 24,
    /**
     * 32 bits float 1 channel type
     */
    CV_MAT_TYPE_32FC1 = CV_MAT_TYPE_32F,
    /**
     * 32 bits float 2 channels type
     */
    CV_MAT_TYPE_32FC2 = CV_MAT_TYPE_32F + 8,
    /**
     * 32 bits float 3 channels type
     */
    CV_MAT_TYPE_32FC3 = CV_MAT_TYPE_32F + 16,
    /**
     * 32 bits float 4 channels type
     */
    CV_MAT_TYPE_32FC4 = CV_MAT_TYPE_32F + 24,
    /**
     * 64 bits float 1 channel type
     */
    CV_MAT_TYPE_64FC1 = CV_MAT_TYPE_64F,
    /**
     * 64 bits float 2 channels type
     */
    CV_MAT_TYPE_64FC2 = CV_MAT_TYPE_64F + 8,
    /**
     * 64 bits float 3 channels type
     */
    CV_MAT_TYPE_64FC3 = CV_MAT_TYPE_64F + 16,
    /**
     * 64 bits float 4 channels type
     */
    CV_MAT_TYPE_64FC4 = CV_MAT_TYPE_64F + 24,
    /**
     * 16 bits float 1 channel type
     */
    CV_MAT_TYPE_16FC1 = CV_MAT_TYPE_16F,
    /**
     * 16 bits float 2 channels type
     */
    CV_MAT_TYPE_16FC2 = CV_MAT_TYPE_16F + 8,
    /**
     * 16 bits float 3 channels type
     */
    CV_MAT_TYPE_16FC3 = CV_MAT_TYPE_16F + 16,
    /**
     * 16 bits float 4 channels type
     */
    CV_MAT_TYPE_16FC4 = CV_MAT_TYPE_16F + 24,
};

/**
 * @brief Defines alias of opencv mat type enumerates.
 *
 * @since 11
 * @version 4.1
 */
typedef enum CV_Mat_Type_ CV_Mat_Type;

/**
 * @brief Enumerates the border type for border extend operations
 *
 * @since 11
 * @version 4.1
 */
enum CV_Border_Type_ {
    /**
     * Border type constant
     */
    CV_BORDER_TYPE_CONSTANT = 0,
    /**
     * Border type replicate
     */
    CV_BORDER_TYPE_REPLICATE = 1,
    /**
     * Border type reflect
     */
    CV_BORDER_TYPE_REFLECT = 2,
    /**
     * Border type wrap
     */
    CV_BORDER_TYPE_WRAP = 3,
    /**
     * Border type reflect 101
     */
    CV_BORDER_TYPE_REFLECT101 = 4,
    /**
     * Border type transparent
     */
    CV_BORDER_TYPE_TRANSPARENT = 5,
    /**
     * Border type default
     */
    CV_BORDER_TYPE_DEFAULT = CV_BORDER_TYPE_REFLECT101,
    /**
     * Border type isolated
     */
    CV_BORDER_TYPE_ISOLATED = 16
};

/**
 * @brief Defines alias of border type enumerates.
 *
 * @since 11
 * @version 4.1
 */
typedef enum CV_Border_Type_ CV_Border_Type;

/**
 * @brief Enumerates the morph operation type
 *
 * @since 11
 * @version 4.1
 */
enum CV_Morph_Type_ {
    /**
     * Morph type invaild operation
     */
    CV_MORPH_TYPE_INVAILD = -1,
    /**
     * Morph type erode operation
     */
    CV_MORPH_TYPE_ERODE = 0,
    /**
     * Morph type dilate operation
     */
    CV_MORPH_TYPE_DILATE = 1,
    /**
     * Morph type open operation
     */
    CV_MORPH_TYPE_OPEN = 2,
    /**
     * Morph type close operation
     */
    CV_MORPH_TYPE_CLOSE = 3,
    /**
     * Morph type gradient operation
     */
    CV_MORPH_TYPE_GRADIENT = 4,
    /**
     * Morph type top hat operation
     */
    CV_MORPH_TYPE_TOPHAT = 5,
    /**
     * Morph type black hat operation
     */
    CV_MORPH_TYPE_BLACKHAT = 6,
    /**
     * Morph type hit miss operation
     */
    CV_MORPH_TYPE_HITMISS = 7
};

/**
 * @brief Defines alias of morph operation type enumerates.
 *
 * @since 11
 * @version 4.1
 */
typedef enum CV_Morph_Type_ CV_Morph_Type;


/**
 * @brief Enumerates the interpolation operation type
 *
 * @since 11
 * @version 4.1
 */
enum CV_Interpolation_Flag_ {
    /**
     * Interpolation type nearest
     */
    CV_INTERPOLATION_NEAREST = 0,
    /**
     * Interpolation type linear
     */
    CV_INTERPOLATION_LINEAR = 1,
    /**
     * Interpolation type cubic
     */
    CV_INTERPOLATION_CUBIC = 2,
    /**
     * Interpolation type area
     */
    CV_INTERPOLATION_AREA = 3,
    /**
     * Interpolation type lanczos4
     */
    CV_INTERPOLATION_LANCZOS4 = 4,
    /**
     * Interpolation type linear exact
     */
    CV_INTERPOLATION_LINEAR_EXACT = 5,
    /**
     * Interpolation type nearest exact
     */
    CV_INTERPOLATION_NEAREST_EXACT = 6,
    /**
     * Interpolation type max
     */
    CV_INTERPOLATION_MAX = 7,
    /**
     * Interpolation type warp fill outliers
     */
    CV_INTERPOLATION_WARP_FILL_OUTLIERS = 8,
    /**
     * Interpolation type warp inverse map
     */
    CV_INTERPOLATION_WARP_INVERSE_MAP = 16
};

/**
 * @brief Defines alias of interpolation operation type enumerates.
 *
 * @since 11
 * @version 4.1
 */
typedef enum CV_Interpolation_Flag_ CV_Interpolation_Flag;

/**
 * @brief Enumerates the matrix decomposition type
 *
 * @since 11
 * @version 4.1
 */
enum CV_Decomp_Type_ {
    /**
     * LU triangular decomposition type
     */
    CV_DECOMP_TYPE_LU = 0,
    /**
     * Singular value decomposition type
     */
    CV_DECOMP_TYPE_SVD = 1,
    /**
     * Eigen value decomposition type
     */
    CV_DECOMP_TYPE_EIG = 2,
    /**
     * Cholesky LLT decomposition type
     */
    CV_DECOMP_TYPE_CHOLESKY = 3,
    /**
     * Orthogonal triangular decomposition type
     */
    CV_DECOMP_TYPE_QR = 4,
    /**
     * Normal decomposition type
     */
    CV_DECOMP_TYPE_NORMAL = 16
};

/**
 * @brief Defines alias of matrix decomposition type enumerates.
 *
 * @since 11
 * @version 4.1
 */
typedef enum CV_Decomp_Type_ CV_Decomp_Type;

/**
 * @brief Enumerates the threshold type
 *
 * @since 11
 * @version 4.1
 */
enum CV_Threshold_Type_ {
    /**
     * threshold type binary
     */
    CV_THRESHOLD_TYPE_BINARY = 0,
    /**
     * threshold type binary inverted
     */
    CV_THRESHOLD_TYPE_BINARY_INV = 1,
    /**
     * threshold type truncate
     */
    CV_THRESHOLD_TYPE_TRUNC = 2,
    /**
     * threshold type to zero
     */
    CV_THRESHOLD_TYPE_TOZERO = 3,
    /**
     * threshold type to zero inverted
     */
    CV_THRESHOLD_TYPE_TOZERO_INV = 4,
    /**
     * threshold type mask
     */
    CV_THRESHOLD_TYPE_MASK = 7,
    /**
     * threshold type OTSU algorithm
     */
    CV_THRESHOLD_TYPE_OTSU = 8,
    /**
     * threshold type triangle algorithm
     */
    CV_THRESHOLD_TYPE_TRIANGLE = 16
};

/**
 * @brief Defines alias of threshold type enumerates.
 *
 * @since 11
 * @version 4.1
 */
typedef enum CV_Threshold_Type_ CV_Threshold_Type;

/**
 * @brief Enumerates the color conversion type
 *
 * @since 11
 * @version 4.1
 */
enum CV_Color_Conversion_Code_ {
    /**
     * color conversion BGR to BGRA
     */
    CV_COLOR_CONVERSION_CODE_BGR2BGRA = 0,
    /**
     * color conversion RGB to RGBA
     */
    CV_COLOR_CONVERSION_CODE_RGB2RGBA = CV_COLOR_CONVERSION_CODE_BGR2BGRA,
    /**
     * color conversion BGRA to BGR
     */
    CV_COLOR_CONVERSION_CODE_BGRA2BGR = 1,
    /**
     * color conversion RGBA to RGB
     */
    CV_COLOR_CONVERSION_CODE_RGBA2RGB = CV_COLOR_CONVERSION_CODE_BGRA2BGR,
    /**
     * color conversion BGR to RGBA
     */
    CV_COLOR_CONVERSION_CODE_BGR2RGBA = 2,
    /**
     * color conversion RGB to BGRA
     */
    CV_COLOR_CONVERSION_CODE_RGB2BGRA = CV_COLOR_CONVERSION_CODE_BGR2RGBA,
    /**
     * color conversion RGBA to BGR
     */
    CV_COLOR_CONVERSION_CODE_RGBA2BGR = 3,
    /**
     * color conversion BGRA to RGB
     */
    CV_COLOR_CONVERSION_CODE_BGRA2RGB = CV_COLOR_CONVERSION_CODE_RGBA2BGR,
    /**
     * color conversion BGR to RGB
     */
    CV_COLOR_CONVERSION_CODE_BGR2RGB = 4,
    /**
     * color conversion RGB to BGR
     */
    CV_COLOR_CONVERSION_CODE_RGB2BGR = CV_COLOR_CONVERSION_CODE_BGR2RGB,
    /**
     * color conversion BGRA to RGBA
     */
    CV_COLOR_CONVERSION_CODE_BGRA2RGBA = 5,
    /**
     * color conversion RGBA to BGRA
     */
    CV_COLOR_CONVERSION_CODE_RGBA2BGRA = CV_COLOR_CONVERSION_CODE_BGRA2RGBA,
    /**
     * color conversion BGR to GRAY
     */
    CV_COLOR_CONVERSION_CODE_BGR2GRAY = 6,
    /**
     * color conversion RGB to GRAY
     */
    CV_COLOR_CONVERSION_CODE_RGB2GRAY = 7,
    /**
     * color conversion GRAY to BGR
     */
    CV_COLOR_CONVERSION_CODE_GRAY2BGR = 8,
    /**
     * color conversion GRAY to RGB
     */
    CV_COLOR_CONVERSION_CODE_GRAY2RGB = CV_COLOR_CONVERSION_CODE_GRAY2BGR,
    /**
     * color conversion GRAY to BGRA
     */
    CV_COLOR_CONVERSION_CODE_GRAY2BGRA = 9,
    /**
     * color conversion GRAY to RGBA
     */
    CV_COLOR_CONVERSION_CODE_GRAY2RGBA = CV_COLOR_CONVERSION_CODE_GRAY2BGRA,
    /**
     * color conversion BGRA to GRAY
     */
    CV_COLOR_CONVERSION_CODE_BGRA2GRAY = 10,
    /**
     * color conversion RGBA to GRAY
     */
    CV_COLOR_CONVERSION_CODE_RGBA2GRAY = 11,
    /**
     * color conversion BGR to BGR565
     */
    CV_COLOR_CONVERSION_CODE_BGR2BGR565 = 12,
    /**
     * color conversion RGB to BGR565
     */
    CV_COLOR_CONVERSION_CODE_RGB2BGR565 = 13,
    /**
     * color conversion BGR565 to BGR
     */
    CV_COLOR_CONVERSION_CODE_BGR5652BGR = 14,
    /**
     * color conversion BGR565 to RGB
     */
    CV_COLOR_CONVERSION_CODE_BGR5652RGB = 15,
    /**
     * color conversion BGRA to BGR565
     */
    CV_COLOR_CONVERSION_CODE_BGRA2BGR565 = 16,
    /**
     * color conversion RGBA to BGR565
     */
    CV_COLOR_CONVERSION_CODE_RGBA2BGR565 = 17,
    /**
     * color conversion BGR565 to BGRA
     */
    CV_COLOR_CONVERSION_CODE_BGR5652BGRA = 18,
    /**
     * color conversion BGR565 to RGBA
     */
    CV_COLOR_CONVERSION_CODE_BGR5652RGBA = 19,
    /**
     * color conversion GRAY to BGR565
     */
    CV_COLOR_CONVERSION_CODE_GRAY2BGR565 = 20,
    /**
     * color conversion BGR565 to GRAY
     */
    CV_COLOR_CONVERSION_CODE_BGR5652GRAY = 21,
    /**
     * color conversion BGR to BGR555
     */
    CV_COLOR_CONVERSION_CODE_BGR2BGR555 = 22,
    /**
     * color conversion RGB to BGR555
     */
    CV_COLOR_CONVERSION_CODE_RGB2BGR555 = 23,
    /**
     * color conversion BGR555 to BGR
     */
    CV_COLOR_CONVERSION_CODE_BGR5552BGR = 24,
    /**
     * color conversion BGR555 to RGB
     */
    CV_COLOR_CONVERSION_CODE_BGR5552RGB = 25,
    /**
     * color conversion BGRA to BGR555
     */
    CV_COLOR_CONVERSION_CODE_BGRA2BGR555 = 26,
    /**
     * color conversion RGBA to BGR555
     */
    CV_COLOR_CONVERSION_CODE_RGBA2BGR555 = 27,
    /**
     * color conversion BGR555 to BGRA
     */
    CV_COLOR_CONVERSION_CODE_BGR5552BGRA = 28,
    /**
     * color conversion BGR555 to RGBA
     */
    CV_COLOR_CONVERSION_CODE_BGR5552RGBA = 29,
    /**
     * color conversion GRAY to BGR555
     */
    CV_COLOR_CONVERSION_CODE_GRAY2BGR555 = 30,
    /**
     * color conversion BGR555 to GRAY
     */
    CV_COLOR_CONVERSION_CODE_BGR5552GRAY = 31,
    /**
     * color conversion BGR to XYZ
     */
    CV_COLOR_CONVERSION_CODE_BGR2XYZ = 32,
    /**
     * color conversion RGB to XYZ
     */
    CV_COLOR_CONVERSION_CODE_RGB2XYZ = 33,
    /**
     * color conversion XYZ to BGR
     */
    CV_COLOR_CONVERSION_CODE_XYZ2BGR = 34,
    /**
     * color conversion XYZ to RGB
     */
    CV_COLOR_CONVERSION_CODE_XYZ2RGB = 35,
    /**
     * color conversion BGR to YCrCb
     */
    CV_COLOR_CONVERSION_CODE_BGR2YCRCB = 36,
    /**
     * color conversion RGB to YCrCb
     */
    CV_COLOR_CONVERSION_CODE_RGB2YCRCB = 37,
    /**
     * color conversion YCrCb to BGR
     */
    CV_COLOR_CONVERSION_CODE_YCRCB2BGR = 38,
    /**
     * color conversion YCrCb to RGB
     */
    CV_COLOR_CONVERSION_CODE_YCRCB2RGB = 39,
    /**
     * color conversion BGR to HSV
     */
    CV_COLOR_CONVERSION_CODE_BGR2HSV = 40,
    /**
     * color conversion RGB to HSV
     */
    CV_COLOR_CONVERSION_CODE_RGB2HSV = 41,
    /**
     * color conversion BGR to Lab
     */
    CV_COLOR_CONVERSION_CODE_BGR2LAB = 44,
    /**
     * color conversion RGB to Lab
     */
    CV_COLOR_CONVERSION_CODE_RGB2LAB = 45,
    /**
     * color conversion BGR to Luv
     */
    CV_COLOR_CONVERSION_CODE_BGR2LUV = 50,
    /**
     * color conversion RGB to Luv
     */
    CV_COLOR_CONVERSION_CODE_RGB2LUV = 51,
    /**
     * color conversion BGR to HLS
     */
    CV_COLOR_CONVERSION_CODE_BGR2HLS = 52,
    /**
     * color conversion RGB to HLS
     */
    CV_COLOR_CONVERSION_CODE_RGB2HLS = 53,
    /**
     * color conversion HSV to BGR
     */
    CV_COLOR_CONVERSION_CODE_HSV2BGR = 54,
    /**
     * color conversion HSV to RGB
     */
    CV_COLOR_CONVERSION_CODE_HSV2RGB = 55,
    /**
     * color conversion Lab to BGR
     */
    CV_COLOR_CONVERSION_CODE_LAB2BGR = 56,
    /**
     * color conversion Lab to RGB
     */
    CV_COLOR_CONVERSION_CODE_LAB2RGB = 57,
    /**
     * color conversion Luv to BGR
     */
    CV_COLOR_CONVERSION_CODE_LUV2BGR = 58,
    /**
     * color conversion Luv to RGB
     */
    CV_COLOR_CONVERSION_CODE_LUV2RGB = 59,
    /**
     * color conversion HLS to BGR
     */
    CV_COLOR_CONVERSION_CODE_HLS2BGR = 60,
    /**
     * color conversion HLS to RGB
     */
    CV_COLOR_CONVERSION_CODE_HLS2RGB = 61,
    /**
     * color conversion BGR to HSV_FULL
     */
    CV_COLOR_CONVERSION_CODE_BGR2HSV_FULL = 66,
    /**
     * color conversion RGB to HSV_FULL
     */
    CV_COLOR_CONVERSION_CODE_RGB2HSV_FULL = 67,
    /**
     * color conversion BGR to HLS_FULL
     */
    CV_COLOR_CONVERSION_CODE_BGR2HLS_FULL = 68,
    /**
     * color conversion RGB to HLS_FULL
     */
    CV_COLOR_CONVERSION_CODE_RGB2HLS_FULL = 69,
    /**
     * color conversion HSV to BGR_FULL
     */
    CV_COLOR_CONVERSION_CODE_HSV2BGR_FULL = 70,
    /**
     * color conversion HSV to RGB_FULL
     */
    CV_COLOR_CONVERSION_CODE_HSV2RGB_FULL = 71,
    /**
     * color conversion HLS to BGR_FULL
     */
    CV_COLOR_CONVERSION_CODE_HLS2BGR_FULL = 72,
    /**
     * color conversion HLS to RGB_FULL
     */
    CV_COLOR_CONVERSION_CODE_HLS2RGB_FULL = 73,
    /**
     * color conversion LBGR to Lab
     */
    CV_COLOR_CONVERSION_CODE_LBGR2LAB = 74,
    /**
     * color conversion LRGB to Lab
     */
    CV_COLOR_CONVERSION_CODE_LRGB2LAB = 75,
    /**
     * color conversion LBGR to Luv
     */
    CV_COLOR_CONVERSION_CODE_LBGR2LUV = 76,
    /**
     * color conversion LRGB to Luv
     */
    CV_COLOR_CONVERSION_CODE_LRGB2LUV = 77,
    /**
     * color conversion Lab to LBGR
     */
    CV_COLOR_CONVERSION_CODE_LAB2LBGR = 78,
    /**
     * color conversion Lab to LRGB
     */
    CV_COLOR_CONVERSION_CODE_LAB2LRGB = 79,
    /**
     * color conversion Luv to LBGR
     */
    CV_COLOR_CONVERSION_CODE_LUV2LBGR = 80,
    /**
     * color conversion Luv to LRGB
     */
    CV_COLOR_CONVERSION_CODE_LUV2LRGB = 81,
    /**
     * color conversion BGR to YUV
     */
    CV_COLOR_CONVERSION_CODE_BGR2YUV = 82,
    /**
     * color conversion RGB to YUV
     */
    CV_COLOR_CONVERSION_CODE_RGB2YUV = 83,
    /**
     * color conversion YUV to BGR
     */
    CV_COLOR_CONVERSION_CODE_YUV2BGR = 84,
    /**
     * color conversion YUV to RGB
     */
    CV_COLOR_CONVERSION_CODE_YUV2RGB = 85,
    /**
     * color conversion YUV to RGB_NV12
     */
    CV_COLOR_CONVERSION_CODE_YUV2RGB_NV12 = 90,
    /**
     * color conversion YUV to BGR_NV12
     */
    CV_COLOR_CONVERSION_CODE_YUV2BGR_NV12 = 91,
    /**
     * color conversion YUV to RGB_NV21
     */
    CV_COLOR_CONVERSION_CODE_YUV2RGB_NV21 = 92,
    /**
     * color conversion YUV to BGR_NV21
     */
    CV_COLOR_CONVERSION_CODE_YUV2BGR_NV21 = 93,
    /**
     * color conversion YUV420sp to RGB
     */
    CV_COLOR_CONVERSION_CODE_YUV420SP2RGB = CV_COLOR_CONVERSION_CODE_YUV2RGB_NV21,
    /**
     * color conversion YUV420sp to BGR
     */
    CV_COLOR_CONVERSION_CODE_YUV420SP2BGR = CV_COLOR_CONVERSION_CODE_YUV2BGR_NV21,
    /**
     * color conversion YUV to RGBA_NV12
     */
    CV_COLOR_CONVERSION_CODE_YUV2RGBA_NV12 = 94,
    /**
     * color conversion YUV to BGRA_NV12
     */
    CV_COLOR_CONVERSION_CODE_YUV2BGRA_NV12 = 95,
    /**
     * color conversion YUV to RGBA_NV21
     */
    CV_COLOR_CONVERSION_CODE_YUV2RGBA_NV21 = 96,
    /**
     * color conversion YUV to BGRA_NV21
     */
    CV_COLOR_CONVERSION_CODE_YUV2BGRA_NV21 = 97,
    /**
     * color conversion YUV420sp to RGBA
     */
    CV_COLOR_CONVERSION_CODE_YUV420SP2RGBA = CV_COLOR_CONVERSION_CODE_YUV2RGBA_NV21,
    /**
     * color conversion YUV420sp to BGRA
     */
    CV_COLOR_CONVERSION_CODE_YUV420SP2BGRA = CV_COLOR_CONVERSION_CODE_YUV2BGRA_NV21,
    /**
     * color conversion YUV to RGB_YV12
     */
    CV_COLOR_CONVERSION_CODE_YUV2RGB_YV12 = 98,
    /**
     * color conversion YUV to BGR_YV12
     */
    CV_COLOR_CONVERSION_CODE_YUV2BGR_YV12 = 99,
    /**
     * color conversion YUV to RGB_IYUV
     */
    CV_COLOR_CONVERSION_CODE_YUV2RGB_IYUV = 100,
    /**
     * color conversion YUV to BGR_IYUV
     */
    CV_COLOR_CONVERSION_CODE_YUV2BGR_IYUV = 101,
    /**
     * color conversion YUV to RGB_I420
     */
    CV_COLOR_CONVERSION_CODE_YUV2RGB_I420 = CV_COLOR_CONVERSION_CODE_YUV2RGB_IYUV,
    /**
     * color conversion YUV to BGR_I420
     */
    CV_COLOR_CONVERSION_CODE_YUV2BGR_I420 = CV_COLOR_CONVERSION_CODE_YUV2BGR_IYUV,
    /**
     * color conversion YUV420p to RGB
     */
    CV_COLOR_CONVERSION_CODE_YUV420P2RGB = CV_COLOR_CONVERSION_CODE_YUV2RGB_YV12,
    /**
     * color conversion YUV420p to BGR
     */
    CV_COLOR_CONVERSION_CODE_YUV420P2BGR = CV_COLOR_CONVERSION_CODE_YUV2BGR_YV12,
    /**
     * color conversion YUV to RGBA_YV12
     */
    CV_COLOR_CONVERSION_CODE_YUV2RGBA_YV12 = 102,
    /**
     * color conversion YUV to BGRA_YV12
     */
    CV_COLOR_CONVERSION_CODE_YUV2BGRA_YV12 = 103,
    /**
     * color conversion YUV to RGBA_IYUV
     */
    CV_COLOR_CONVERSION_CODE_YUV2RGBA_IYUV = 104,
    /**
     * color conversion YUV to BGRA_IYUV
     */
    CV_COLOR_CONVERSION_CODE_YUV2BGRA_IYUV = 105,
    /**
     * color conversion YUV to RGBA_I420
     */
    CV_COLOR_CONVERSION_CODE_YUV2RGBA_I420 = CV_COLOR_CONVERSION_CODE_YUV2RGBA_IYUV,
    /**
     * color conversion YUV to BGRA_I420
     */
    CV_COLOR_CONVERSION_CODE_YUV2BGRA_I420 = CV_COLOR_CONVERSION_CODE_YUV2BGRA_IYUV,
    /**
     * color conversion YUV420p to RGBA
     */
    CV_COLOR_CONVERSION_CODE_YUV420P2RGBA = CV_COLOR_CONVERSION_CODE_YUV2RGBA_YV12,
    /**
     * color conversion YUV420p to BGRA
     */
    CV_COLOR_CONVERSION_CODE_YUV420P2BGRA = CV_COLOR_CONVERSION_CODE_YUV2BGRA_YV12,
    /**
     * color conversion YUV to GRAY_420
     */
    CV_COLOR_CONVERSION_CODE_YUV2GRAY_420 = 106,
    /**
     * color conversion YUV to GRAY_NV21
     */
    CV_COLOR_CONVERSION_CODE_YUV2GRAY_NV21 = CV_COLOR_CONVERSION_CODE_YUV2GRAY_420,
    /**
     * color conversion YUV to GRAY_NV12
     */
    CV_COLOR_CONVERSION_CODE_YUV2GRAY_NV12 = CV_COLOR_CONVERSION_CODE_YUV2GRAY_420,
    /**
     * color conversion YUV to GRAY_YV12
     */
    CV_COLOR_CONVERSION_CODE_YUV2GRAY_YV12 = CV_COLOR_CONVERSION_CODE_YUV2GRAY_420,
    /**
     * color conversion YUV to GRAY_IYUV
     */
    CV_COLOR_CONVERSION_CODE_YUV2GRAY_IYUV = CV_COLOR_CONVERSION_CODE_YUV2GRAY_420,
    /**
     * color conversion YUV to GRAY_I420
     */
    CV_COLOR_CONVERSION_CODE_YUV2GRAY_I420 = CV_COLOR_CONVERSION_CODE_YUV2GRAY_420,
    /**
     * color conversion YUV420sp to GRAY
     */
    CV_COLOR_CONVERSION_CODE_YUV420SP2GRAY = CV_COLOR_CONVERSION_CODE_YUV2GRAY_420,
    /**
     * color conversion YUV420p to GRAY
     */
    CV_COLOR_CONVERSION_CODE_YUV420P2GRAY = CV_COLOR_CONVERSION_CODE_YUV2GRAY_420,
    /**
     * color conversion YUV to RGB_UYVY
     */
    CV_COLOR_CONVERSION_CODE_YUV2RGB_UYVY = 107,
    /**
     * color conversion YUV to BGR_UYVY
     */
    CV_COLOR_CONVERSION_CODE_YUV2BGR_UYVY = 108,
    /**
     * color conversion YUV to RGB_Y422
     */
    CV_COLOR_CONVERSION_CODE_YUV2RGB_Y422 = CV_COLOR_CONVERSION_CODE_YUV2RGB_UYVY,
    /**
     * color conversion YUV to BGR_Y422
     */
    CV_COLOR_CONVERSION_CODE_YUV2BGR_Y422 = CV_COLOR_CONVERSION_CODE_YUV2BGR_UYVY,
    /**
     * color conversion YUV to RGB_UYNV
     */
    CV_COLOR_CONVERSION_CODE_YUV2RGB_UYNV = CV_COLOR_CONVERSION_CODE_YUV2RGB_UYVY,
    /**
     * color conversion YUV to BGR_UYNV
     */
    CV_COLOR_CONVERSION_CODE_YUV2BGR_UYNV = CV_COLOR_CONVERSION_CODE_YUV2BGR_UYVY,
    /**
     * color conversion YUV to RGBA_UYVY
     */
    CV_COLOR_CONVERSION_CODE_YUV2RGBA_UYVY = 111,
    /**
     * color conversion YUV to BGRA_UYVY
     */
    CV_COLOR_CONVERSION_CODE_YUV2BGRA_UYVY = 112,
    /**
     * color conversion YUV to RGBA_Y422
     */
    CV_COLOR_CONVERSION_CODE_YUV2RGBA_Y422 = CV_COLOR_CONVERSION_CODE_YUV2RGBA_UYVY,
    /**
     * color conversion YUV to BGRA_Y422
     */
    CV_COLOR_CONVERSION_CODE_YUV2BGRA_Y422 = CV_COLOR_CONVERSION_CODE_YUV2BGRA_UYVY,
    /**
     * color conversion YUV to RGBA_UYNV
     */
    CV_COLOR_CONVERSION_CODE_YUV2RGBA_UYNV = CV_COLOR_CONVERSION_CODE_YUV2RGBA_UYVY,
    /**
     * color conversion YUV to BGRA_UYNV
     */
    CV_COLOR_CONVERSION_CODE_YUV2BGRA_UYNV = CV_COLOR_CONVERSION_CODE_YUV2BGRA_UYVY,
    /**
     * color conversion YUV to RGB_YUY2
     */
    CV_COLOR_CONVERSION_CODE_YUV2RGB_YUY2 = 115,
    /**
     * color conversion YUV to BGR_YUY2
     */
    CV_COLOR_CONVERSION_CODE_YUV2BGR_YUY2 = 116,
    /**
     * color conversion YUV to RGB_YVYU
     */
    CV_COLOR_CONVERSION_CODE_YUV2RGB_YVYU = 117,
    /**
     * color conversion YUV to BGR_YVYU
     */
    CV_COLOR_CONVERSION_CODE_YUV2BGR_YVYU = 118,
    /**
     * color conversion YUV to RGB_YUYV
     */
    CV_COLOR_CONVERSION_CODE_YUV2RGB_YUYV = CV_COLOR_CONVERSION_CODE_YUV2RGB_YUY2,
    /**
     * color conversion YUV to BGR_YUYV
     */
    CV_COLOR_CONVERSION_CODE_YUV2BGR_YUYV = CV_COLOR_CONVERSION_CODE_YUV2BGR_YUY2,
    /**
     * color conversion YUV to RGB_YUNV
     */
    CV_COLOR_CONVERSION_CODE_YUV2RGB_YUNV = CV_COLOR_CONVERSION_CODE_YUV2RGB_YUY2,
    /**
     * color conversion YUV to BGR_YUNV
     */
    CV_COLOR_CONVERSION_CODE_YUV2BGR_YUNV = CV_COLOR_CONVERSION_CODE_YUV2BGR_YUY2,
    /**
     * color conversion YUV to RGBA_YUY2
     */
    CV_COLOR_CONVERSION_CODE_YUV2RGBA_YUY2 = 119,
    /**
     * color conversion YUV to BGRA_YUY2
     */
    CV_COLOR_CONVERSION_CODE_YUV2BGRA_YUY2 = 120,
    /**
     * color conversion YUV to RGBA_YVYU
     */
    CV_COLOR_CONVERSION_CODE_YUV2RGBA_YVYU = 121,
    /**
     * color conversion YUV to BGRA_YVYU
     */
    CV_COLOR_CONVERSION_CODE_YUV2BGRA_YVYU = 122,
    /**
     * color conversion YUV to RGBA_YUYV
     */
    CV_COLOR_CONVERSION_CODE_YUV2RGBA_YUYV = CV_COLOR_CONVERSION_CODE_YUV2RGBA_YUY2,
    /**
     * color conversion YUV to BGRA_YUYV
     */
    CV_COLOR_CONVERSION_CODE_YUV2BGRA_YUYV = CV_COLOR_CONVERSION_CODE_YUV2BGRA_YUY2,
    /**
     * color conversion YUV to RGBA_YUNV
     */
    CV_COLOR_CONVERSION_CODE_YUV2RGBA_YUNV = CV_COLOR_CONVERSION_CODE_YUV2RGBA_YUY2,
    /**
     * color conversion YUV to BGRA_YUNV
     */
    CV_COLOR_CONVERSION_CODE_YUV2BGRA_YUNV = CV_COLOR_CONVERSION_CODE_YUV2BGRA_YUY2,
    /**
     * color conversion YUV to GRAY_UYVY
     */
    CV_COLOR_CONVERSION_CODE_YUV2GRAY_UYVY = 123,
    /**
     * color conversion YUV to GRAY_YUY2
     */
    CV_COLOR_CONVERSION_CODE_YUV2GRAY_YUY2 = 124,
    /**
     * color conversion YUV to GRAY_Y422
     */
    CV_COLOR_CONVERSION_CODE_YUV2GRAY_Y422 = CV_COLOR_CONVERSION_CODE_YUV2GRAY_UYVY,
    /**
     * color conversion YUV to GRAY_UYNV
     */
    CV_COLOR_CONVERSION_CODE_YUV2GRAY_UYNV = CV_COLOR_CONVERSION_CODE_YUV2GRAY_UYVY,
    /**
     * color conversion YUV to GRAY_YVYU
     */
    CV_COLOR_CONVERSION_CODE_YUV2GRAY_YVYU = CV_COLOR_CONVERSION_CODE_YUV2GRAY_YUY2,
    /**
     * color conversion YUV to GRAY_YUYV
     */
    CV_COLOR_CONVERSION_CODE_YUV2GRAY_YUYV = CV_COLOR_CONVERSION_CODE_YUV2GRAY_YUY2,
    /**
     * color conversion YUV to GRAY_YUNV
     */
    CV_COLOR_CONVERSION_CODE_YUV2GRAY_YUNV = CV_COLOR_CONVERSION_CODE_YUV2GRAY_YUY2,
    /**
     * color conversion RGBA to mRGBA
     */
    CV_COLOR_CONVERSION_CODE_RGBA2MRGBA = 125,
    /**
     * color conversion mRGBA to RGBA
     */
    CV_COLOR_CONVERSION_CODE_MRGBA2RGBA = 126,
    /**
     * color conversion RGB to YUV_I420
     */
    CV_COLOR_CONVERSION_CODE_RGB2YUV_I420 = 127,
    /**
     * color conversion BGR to YUV_I420
     */
    CV_COLOR_CONVERSION_CODE_BGR2YUV_I420 = 128,
    /**
     * color conversion RGB to YUV_IYUV
     */
    CV_COLOR_CONVERSION_CODE_RGB2YUV_IYUV = CV_COLOR_CONVERSION_CODE_RGB2YUV_I420,
    /**
     * color conversion BGR to YUV_IYUV
     */
    CV_COLOR_CONVERSION_CODE_BGR2YUV_IYUV = CV_COLOR_CONVERSION_CODE_BGR2YUV_I420,
    /**
     * color conversion RGBA to YUV_I420
     */
    CV_COLOR_CONVERSION_CODE_RGBA2YUV_I420 = 129,
    /**
     * color conversion BGRA to YUV_I420
     */
    CV_COLOR_CONVERSION_CODE_BGRA2YUV_I420 = 130,
    /**
     * color conversion RGBA to YUV_IYUV
     */
    CV_COLOR_CONVERSION_CODE_RGBA2YUV_IYUV = CV_COLOR_CONVERSION_CODE_RGBA2YUV_I420,
    /**
     * color conversion BGRA to YUV_IYUV
     */
    CV_COLOR_CONVERSION_CODE_BGRA2YUV_IYUV = CV_COLOR_CONVERSION_CODE_BGRA2YUV_I420,
    /**
     * color conversion RGB to YUV_YV12
     */
    CV_COLOR_CONVERSION_CODE_RGB2YUV_YV12 = 131,
    /**
     * color conversion BGR to YUV_YV12
     */
    CV_COLOR_CONVERSION_CODE_BGR2YUV_YV12 = 132,
    /**
     * color conversion RGBA to YUV_YV12
     */
    CV_COLOR_CONVERSION_CODE_RGBA2YUV_YV12 = 133,
    /**
     * color conversion BGRA to YUV_YV12
     */
    CV_COLOR_CONVERSION_CODE_BGRA2YUV_YV12 = 134,
    /**
     * color conversion BayerBG to BGR
     */
    CV_COLOR_CONVERSION_CODE_BAYERBG2BGR = 46,
    /**
     * color conversion BayerGB to BGR
     */
    CV_COLOR_CONVERSION_CODE_BAYERGB2BGR = 47,
    /**
     * color conversion BayerRG to BGR
     */
    CV_COLOR_CONVERSION_CODE_BAYERRG2BGR = 48,
    /**
     * color conversion BayerGR to BGR
     */
    CV_COLOR_CONVERSION_CODE_BAYERGR2BGR = 49,
    /**
     * color conversion BayerRGGB to BGR
     */
    CV_COLOR_CONVERSION_CODE_BAYERRGGB2BGR = CV_COLOR_CONVERSION_CODE_BAYERBG2BGR,
    /**
     * color conversion BayerGRBG to BGR
     */
    CV_COLOR_CONVERSION_CODE_BAYERGRBG2BGR = CV_COLOR_CONVERSION_CODE_BAYERGB2BGR,
    /**
     * color conversion BayerBGGR to BGR
     */
    CV_COLOR_CONVERSION_CODE_BAYERBGGR2BGR = CV_COLOR_CONVERSION_CODE_BAYERRG2BGR,
    /**
     * color conversion BayerGBRG to BGR
     */
    CV_COLOR_CONVERSION_CODE_BAYERGBRG2BGR = CV_COLOR_CONVERSION_CODE_BAYERGR2BGR,
    /**
     * color conversion BayerRGGB to RGB
     */
    CV_COLOR_CONVERSION_CODE_BAYERRGGB2RGB = CV_COLOR_CONVERSION_CODE_BAYERBGGR2BGR,
    /**
     * color conversion BayerGRBG to RGB
     */
    CV_COLOR_CONVERSION_CODE_BAYERGRBG2RGB = CV_COLOR_CONVERSION_CODE_BAYERGBRG2BGR,
    /**
     * color conversion BayerBGGR to RGB
     */
    CV_COLOR_CONVERSION_CODE_BAYERBGGR2RGB = CV_COLOR_CONVERSION_CODE_BAYERRGGB2BGR,
    /**
     * color conversion BayerGBRG to RGB
     */
    CV_COLOR_CONVERSION_CODE_BAYERGBRG2RGB = CV_COLOR_CONVERSION_CODE_BAYERGRBG2BGR,
    /**
     * color conversion BayerBG to RGB
     */
    CV_COLOR_CONVERSION_CODE_BAYERBG2RGB = CV_COLOR_CONVERSION_CODE_BAYERRG2BGR,
    /**
     * color conversion BayerGB to RGB
     */
    CV_COLOR_CONVERSION_CODE_BAYERGB2RGB = CV_COLOR_CONVERSION_CODE_BAYERGR2BGR,
    /**
     * color conversion BayerRG to RGB
     */
    CV_COLOR_CONVERSION_CODE_BAYERRG2RGB = CV_COLOR_CONVERSION_CODE_BAYERBG2BGR,
    /**
     * color conversion BayerGR to RGB
     */
    CV_COLOR_CONVERSION_CODE_BAYERGR2RGB = CV_COLOR_CONVERSION_CODE_BAYERGB2BGR,
    /**
     * color conversion BayerBG to GRAY
     */
    CV_COLOR_CONVERSION_CODE_BAYERBG2GRAY = 86,
    /**
     * color conversion BayerGB to GRAY
     */
    CV_COLOR_CONVERSION_CODE_BAYERGB2GRAY = 87,
    /**
     * color conversion BayerRG to GRAY
     */
    CV_COLOR_CONVERSION_CODE_BAYERRG2GRAY = 88,
    /**
     * color conversion BayerGR to GRAY
     */
    CV_COLOR_CONVERSION_CODE_BAYERGR2GRAY = 89,
    /**
     * color conversion BayerRGGB to GRAY
     */
    CV_COLOR_CONVERSION_CODE_BAYERRGGB2GRAY = CV_COLOR_CONVERSION_CODE_BAYERBG2GRAY,
    /**
     * color conversion BayerGRBG to GRAY
     */
    CV_COLOR_CONVERSION_CODE_BAYERGRBG2GRAY = CV_COLOR_CONVERSION_CODE_BAYERGB2GRAY,
    /**
     * color conversion BayerBGGR to GRAY
     */
    CV_COLOR_CONVERSION_CODE_BAYERBGGR2GRAY = CV_COLOR_CONVERSION_CODE_BAYERRG2GRAY,
    /**
     * color conversion BayerGBRG to GRAY
     */
    CV_COLOR_CONVERSION_CODE_BAYERGBRG2GRAY = CV_COLOR_CONVERSION_CODE_BAYERGR2GRAY,
    /**
     * color conversion BayerBG to BGR_VNG
     */
    CV_COLOR_CONVERSION_CODE_BAYERBG2BGR_VNG = 62,
    /**
     * color conversion BayerGB to BGR_VNG
     */
    CV_COLOR_CONVERSION_CODE_BAYERGB2BGR_VNG = 63,
    /**
     * color conversion BayerRG to BGR_VNG
     */
    CV_COLOR_CONVERSION_CODE_BAYERRG2BGR_VNG = 64,
    /**
     * color conversion BayerGR to BGR_VNG
     */
    CV_COLOR_CONVERSION_CODE_BAYERGR2BGR_VNG = 65,
    /**
     * color conversion BayerRGGB to BGR_VNG
     */
    CV_COLOR_CONVERSION_CODE_BAYERRGGB2BGR_VNG = CV_COLOR_CONVERSION_CODE_BAYERBG2BGR_VNG,
    /**
     * color conversion BayerGRBG to BGR_VNG
     */
    CV_COLOR_CONVERSION_CODE_BAYERGRBG2BGR_VNG = CV_COLOR_CONVERSION_CODE_BAYERGB2BGR_VNG,
    /**
     * color conversion BayerBGGR to BGR_VNG
     */
    CV_COLOR_CONVERSION_CODE_BAYERBGGR2BGR_VNG = CV_COLOR_CONVERSION_CODE_BAYERRG2BGR_VNG,
    /**
     * color conversion BayerGBRG to BGR_VNG
     */
    CV_COLOR_CONVERSION_CODE_BAYERGBRG2BGR_VNG = CV_COLOR_CONVERSION_CODE_BAYERGR2BGR_VNG,
    /**
     * color conversion BayerRGGB to RGB_VNG
     */
    CV_COLOR_CONVERSION_CODE_BAYERRGGB2RGB_VNG = CV_COLOR_CONVERSION_CODE_BAYERBGGR2BGR_VNG,
    /**
     * color conversion BayerGRBG to RGB_VNG
     */
    CV_COLOR_CONVERSION_CODE_BAYERGRBG2RGB_VNG = CV_COLOR_CONVERSION_CODE_BAYERGBRG2BGR_VNG,
    /**
     * color conversion BayerBGGR to RGB_VNG
     */
    CV_COLOR_CONVERSION_CODE_BAYERBGGR2RGB_VNG = CV_COLOR_CONVERSION_CODE_BAYERRGGB2BGR_VNG,
    /**
     * color conversion BayerGBRG to RGB_VNG
     */
    CV_COLOR_CONVERSION_CODE_BAYERGBRG2RGB_VNG = CV_COLOR_CONVERSION_CODE_BAYERGRBG2BGR_VNG,
    /**
     * color conversion BayerBG to RGB_VNG
     */
    CV_COLOR_CONVERSION_CODE_BAYERBG2RGB_VNG = CV_COLOR_CONVERSION_CODE_BAYERRG2BGR_VNG,
    /**
     * color conversion BayerGB to RGB_VNG
     */
    CV_COLOR_CONVERSION_CODE_BAYERGB2RGB_VNG = CV_COLOR_CONVERSION_CODE_BAYERGR2BGR_VNG,
    /**
     * color conversion BayerRG to RGB_VNG
     */
    CV_COLOR_CONVERSION_CODE_BAYERRG2RGB_VNG = CV_COLOR_CONVERSION_CODE_BAYERBG2BGR_VNG,
    /**
     * color conversion BayerGR to RGB_VNG
     */
    CV_COLOR_CONVERSION_CODE_BAYERGR2RGB_VNG = CV_COLOR_CONVERSION_CODE_BAYERGB2BGR_VNG,
    /**
     * color conversion BayerBG to BGR_EA
     */
    CV_COLOR_CONVERSION_CODE_BAYERBG2BGR_EA = 135,
    /**
     * color conversion BayerGB to BGR_EA
     */
    CV_COLOR_CONVERSION_CODE_BAYERGB2BGR_EA = 136,
    /**
     * color conversion BayerRG to BGR_EA
     */
    CV_COLOR_CONVERSION_CODE_BAYERRG2BGR_EA = 137,
    /**
     * color conversion BayerGR to BGR_EA
     */
    CV_COLOR_CONVERSION_CODE_BAYERGR2BGR_EA = 138,
    /**
     * color conversion BayerRGGB to BGR_EA
     */
    CV_COLOR_CONVERSION_CODE_BAYERRGGB2BGR_EA = CV_COLOR_CONVERSION_CODE_BAYERBG2BGR_EA,
    /**
     * color conversion BayerGRBG to BGR_EA
     */
    CV_COLOR_CONVERSION_CODE_BAYERGRBG2BGR_EA = CV_COLOR_CONVERSION_CODE_BAYERGB2BGR_EA,
    /**
     * color conversion BayerBGGR to BGR_EA
     */
    CV_COLOR_CONVERSION_CODE_BAYERBGGR2BGR_EA = CV_COLOR_CONVERSION_CODE_BAYERRG2BGR_EA,
    /**
     * color conversion BayerGBRG to BGR_EA
     */
    CV_COLOR_CONVERSION_CODE_BAYERGBRG2BGR_EA = CV_COLOR_CONVERSION_CODE_BAYERGR2BGR_EA,
    /**
     * color conversion BayerRGGB to RGB_EA
     */
    CV_COLOR_CONVERSION_CODE_BAYERRGGB2RGB_EA = CV_COLOR_CONVERSION_CODE_BAYERBGGR2BGR_EA,
    /**
     * color conversion BayerGRBG to RGB_EA
     */
    CV_COLOR_CONVERSION_CODE_BAYERGRBG2RGB_EA = CV_COLOR_CONVERSION_CODE_BAYERGBRG2BGR_EA,
    /**
     * color conversion BayerBGGR to RGB_EA
     */
    CV_COLOR_CONVERSION_CODE_BAYERBGGR2RGB_EA = CV_COLOR_CONVERSION_CODE_BAYERRGGB2BGR_EA,
    /**
     * color conversion BayerGBRG to RGB_EA
     */
    CV_COLOR_CONVERSION_CODE_BAYERGBRG2RGB_EA = CV_COLOR_CONVERSION_CODE_BAYERGRBG2BGR_EA,
    /**
     * color conversion BayerBG to RGB_EA
     */
    CV_COLOR_CONVERSION_CODE_BAYERBG2RGB_EA = CV_COLOR_CONVERSION_CODE_BAYERRG2BGR_EA,
    /**
     * color conversion BayerGB to RGB_EA
     */
    CV_COLOR_CONVERSION_CODE_BAYERGB2RGB_EA = CV_COLOR_CONVERSION_CODE_BAYERGR2BGR_EA,
    /**
     * color conversion BayerRG to RGB_EA
     */
    CV_COLOR_CONVERSION_CODE_BAYERRG2RGB_EA = CV_COLOR_CONVERSION_CODE_BAYERBG2BGR_EA,
    /**
     * color conversion BayerGR to RGB_EA
     */
    CV_COLOR_CONVERSION_CODE_BAYERGR2RGB_EA = CV_COLOR_CONVERSION_CODE_BAYERGB2BGR_EA,
    /**
     * color conversion BayerBG to BGRA
     */
    CV_COLOR_CONVERSION_CODE_BAYERBG2BGRA = 139,
    /**
     * color conversion BayerGB to BGRA
     */
    CV_COLOR_CONVERSION_CODE_BAYERGB2BGRA = 140,
    /**
     * color conversion BayerRG to BGRA
     */
    CV_COLOR_CONVERSION_CODE_BAYERRG2BGRA = 141,
    /**
     * color conversion BayerGR to BGRA
     */
    CV_COLOR_CONVERSION_CODE_BAYERGR2BGRA = 142,
    /**
     * color conversion BayerRGGB to BGRA
     */
    CV_COLOR_CONVERSION_CODE_BAYERRGGB2BGRA = CV_COLOR_CONVERSION_CODE_BAYERBG2BGRA,
    /**
     * color conversion BayerGRBG to BGRA
     */
    CV_COLOR_CONVERSION_CODE_BAYERGRBG2BGRA = CV_COLOR_CONVERSION_CODE_BAYERGB2BGRA,
    /**
     * color conversion BayerBGGR to BGRA
     */
    CV_COLOR_CONVERSION_CODE_BAYERBGGR2BGRA = CV_COLOR_CONVERSION_CODE_BAYERRG2BGRA,
    /**
     * color conversion BayerGBRG to BGRA
     */
    CV_COLOR_CONVERSION_CODE_BAYERGBRG2BGRA = CV_COLOR_CONVERSION_CODE_BAYERGR2BGRA,
    /**
     * color conversion BayerRGGB to RGBA
     */
    CV_COLOR_CONVERSION_CODE_BAYERRGGB2RGBA = CV_COLOR_CONVERSION_CODE_BAYERBGGR2BGRA,
    /**
     * color conversion BayerGRBG to RGBA
     */
    CV_COLOR_CONVERSION_CODE_BAYERGRBG2RGBA = CV_COLOR_CONVERSION_CODE_BAYERGBRG2BGRA,
    /**
     * color conversion BayerBGGR to RGBA
     */
    CV_COLOR_CONVERSION_CODE_BAYERBGGR2RGBA = CV_COLOR_CONVERSION_CODE_BAYERRGGB2BGRA,
    /**
     * color conversion BayerGBRG to RGBA
     */
    CV_COLOR_CONVERSION_CODE_BAYERGBRG2RGBA = CV_COLOR_CONVERSION_CODE_BAYERGRBG2BGRA,
    /**
     * color conversion BayerBG to RGBA
     */
    CV_COLOR_CONVERSION_CODE_BAYERBG2RGBA = CV_COLOR_CONVERSION_CODE_BAYERRG2BGRA,
    /**
     * color conversion BayerGB to RGBA
     */
    CV_COLOR_CONVERSION_CODE_BAYERGB2RGBA = CV_COLOR_CONVERSION_CODE_BAYERGR2BGRA,
    /**
     * color conversion BayerRG to RGBA
     */
    CV_COLOR_CONVERSION_CODE_BAYERRG2RGBA = CV_COLOR_CONVERSION_CODE_BAYERBG2BGRA,
    /**
     * color conversion BayerGR to RGBA
     */
    CV_COLOR_CONVERSION_CODE_BAYERGR2RGBA = CV_COLOR_CONVERSION_CODE_BAYERGB2BGRA,
    /**
     * color conversion max value
     */
    CV_COLOR_CONVERSION_CODE_COLORCVT_MAX = 143
};

/**
 * @brief Defines alias of color conversion type enumerates.
 *
 * @since 11
 * @version 4.1
 */
typedef enum CV_Color_Conversion_Code_ CV_Color_Conversion_Code;

/**
 * @brief Enumerates the line type
 *
 * @since 11
 * @version 4.1
 */
enum CV_Line_Type_ {
    /**
     * Line type filled
     */
    CV_LINE_TYPE_FILLED  = -1,
    /**
     * Line type 4 adjacency
     */
    CV_LINE_TYPE_LINE_4  = 4,
    /**
     * Line type 8 adjacency
     */
    CV_LINE_TYPE_LINE_8  = 8,
    /**
     * Line type anti-aliasing
     */
    CV_LINE_TYPE_LINE_AA = 16
};

/**
 * @brief Defines alias of line type enumerates.
 *
 * @since 11
 * @version 4.1
 */
typedef enum CV_Line_Type_ CV_Line_Type;

/**
 * @brief Enumerates the adaptive threshold type
 *
 * @since 11
 * @version 4.1
 */
enum CV_Adaptive_Threshold_Type_ {
    /**
     * Threshold value is the mean of neighbourhood area.
     */
    CV_ADAPTIVE_THRESHOLD_TYPE_MEAN_C  = 0,
    /**
     * Threshold value is the weighted sum of neighbourhood values
     * where weights are a gaussian window.
     */
    CV_ADAPTIVE_THRESHOLD_TYPE_GAUSSIAN_C  = 1
};

/**
 * @brief Defines alias of adaptive threshold type enumerates.
 *
 * @since 11
 * @version 4.1
 */
typedef enum CV_Adaptive_Threshold_Type_ CV_Adaptive_Threshold_Type;

/**
 * @brief Enumerates the mode of the contour retrieval algorithm
 *
 * @since 11
 * @version 4.1
 */
enum CV_Retrieval_Mode_ {
    /**
     * Retrieves only the extreme outer contours.
     */
    CV_RETRIEVAL_MODE_EXTERNAL = 0,
    /**
     * Retrieves all of the contours without establishing any hierarchical relationships.
     */
    CV_RETRIEVAL_MODE_LIST = 1,
    /**
     * Retrieves all of the contours and organizes them into a two-level hierarchy.
     */
    CV_RETRIEVAL_MODE_CCOMP = 2,
    /**
     * Retrieves all of the contours and reconstructs a full hierarchy of nested contours.
     */
    CV_RETRIEVAL_MODE_TREE = 3,
    /**
     * Retrieves all of the contours flood fill
     */
    CV_RETRIEVAL_MODE_FLOODFILL = 4
};

/**
 * @brief Defines alias of the contour retrieval algorithm mode enumerates.
 *
 * @since 11
 * @version 4.1
 */
typedef enum CV_Retrieval_Mode_ CV_Retrieval_Mode;

/**
 * @brief Enumerates the contour approximation algorithm type
 *
 * @since 11
 * @version 4.1
 */
enum CV_Approximation_Method_ {
    /**
     * Stores all the contour points with freeman link codes.
     */
    CV_APPROXIMATION_METHOD_CODE = 0,
    /**
     * Stores absolutely all the contour points.
     */
    CV_APPROXIMATION_METHOD_NONE = 1,
    /**
     * Compresses horizontal, vertical, and diagonal segments and leaves only their end points.
     */
    CV_APPROXIMATION_METHOD_SIMPLE = 2,
    /**
     * Teh-Chin L1 chain approximation algorithm
     */
    CV_APPROXIMATION_METHOD_TC89_L1 = 3,
    /**
     * Teh-Chin KCOS chain approximation algorithm
     */
    CV_APPROXIMATION_METHOD_TC89_KCOS = 4,
    /**
     * Links all points
     */
    CV_APPROXIMATION_METHOD_LINK_RUNS = 5
};

/**
 * @brief Defines alias of the contour approximation algorithm type enumerates.
 *
 * @since 11
 * @version 4.1
 */
typedef enum CV_Approximation_Method_ CV_Approximation_Method;

/**
 * @brief Enumerates the boolean value
 *
 * @since 11
 * @version 4.1
 */
enum CV_Boolean_Value_ {
    /**
     * Define false value
     */
    CV_BOOLEAN_VALUE_FALSE = 0,
    /**
     * Define true value
     */
    CV_BOOLEAN_VALUE_TRUE = 1
};

/**
 * @brief Defines alias of the boolean value enumerates.
 *
 * @since 11
 * @version 4.1
 */
typedef enum CV_Boolean_Value_ CV_Boolean_Value;

/**
 * @brief Enumerates the normalize type
 *
 * @since 11
 * @version 4.1
 */
enum CV_Normalize_Type_ {
    /**
     * the normalize type inf
     */
    CV_NORMALIZE_TYPE_INF = 1,
    /**
     * the normalize type L1
     */
    CV_NORMALIZE_TYPE_L1 = 2,
    /**
     * the normalize type L2
     */
    CV_NORMALIZE_TYPE_L2 = 4,
    /**
     * the normalize type min max
     */
    CV_NORMALIZE_TYPE_MINMAX = 32
};

/**
 * @brief Defines alias of the normalize type enumerates.
 *
 * @since 11
 * @version 4.1
 */
typedef enum CV_Normalize_Type_ CV_Normalize_Type;

/**
 * @brief Defines the CV matrix properties and data.
 *
 * @since 11
 * @version 4.1
 */
struct CV_Mat_ {
    /** The rows of CV matrix. */
    int rows = 0;
    /** The columns of CV matrix. */
    int cols = 0;
    /** The type of CV matrix. */
    CV_Mat_Type type = CV_MAT_TYPE_8U;
    /** The raw data of CV matrix. */
    void *data = nullptr;
    /** The raw data size of CV matrix. */
    unsigned int dataSize = 0;
};

/**
 * @brief Defines alias of the CV matrix properties and data.
 *
 * @since 11
 * @version 4.1
 */
typedef struct CV_Mat_ CV_Mat;

/**
 * @brief Defines the CV size properties.
 *
 * @since 11
 * @version 4.1
 */
struct CV_Size_ {
    /** The width of CV size. */
    int width = 0;
    /** The height of CV size. */
    int height = 0;
};

/**
 * @brief Defines alias of the CV size properties.
 *
 * @since 11
 * @version 4.1
 */
typedef struct CV_Size_ CV_Size;

/**
 * @brief Defines the CV rect properties.
 *
 * @since 11
 * @version 4.1
 */
struct CV_Rect_ {
    /** The x offset of CV rect. */
    int x = 0;
    /** The y offset of CV rect. */
    int y = 0;
    /** The width of CV rect. */
    int width = 0;
    /** The height of CV rect. */
    int height = 0;
};

/**
 * @brief Defines alias of the CV rect properties.
 *
 * @since 11
 * @version 4.1
 */
typedef struct CV_Rect_ CV_Rect;

/**
 * @brief Defines the CV scalar properties.
 *
 * @since 11
 * @version 4.1
 */
struct CV_Scalar_ {
    /** The x component of CV scalar. */
    double scalarX = 0;
    /** The y component of CV scalar. */
    double scalarY = 0;
    /** The z component of CV scalar. */
    double scalarZ = 0;
};

/**
 * @brief Defines alias of the CV scalar properties.
 *
 * @since 11
 * @version 4.1
 */
typedef struct CV_Scalar_ CV_Scalar;

/**
 * @brief Defines the CV point properties with integer.
 *
 * @since 11
 * @version 4.1
 */
struct CV_Point_ {
    /** The x offset of CV point. */
    int x = -1;
    /** The y offset of CV point. */
    int y = -1;
};

/**
 * @brief Defines alias of the CV point properties with integer.
 *
 * @since 11
 * @version 4.1
 */
typedef struct CV_Point_ CV_Point;

/**
 * @brief Defines the CV point properties with 32 bits float.
 *
 * @since 11
 * @version 4.1
 */
struct CV_Point2f_ {
    /** The x offset of CV point. */
    float x = -1;
    /** The y offset of CV point. */
    float y = -1;
};

/**
 * @brief Defines alias of the CV point properties with 32 bits float.
 *
 * @since 11
 * @version 4.1
 */
typedef struct CV_Point2f_ CV_Point2f;

/**
 * @brief Defines the CV hierarchy properties.
 *
 * @since 11
 * @version 4.1
 */
struct CV_Hierarchy4i_ {
    /** The v0 component of CV hierarchy. */
    int v0 = 0;
    /** The v1 component of CV hierarchy. */
    int v1 = 0;
    /** The v2 component of CV hierarchy. */
    int v2 = 0;
    /** The v3 component of CV hierarchy. */
    int v3 = 0;
};

/**
 * @brief Defines alias of the CV hierarchy properties.
 *
 * @since 11
 * @version 4.1
 */
typedef struct CV_Hierarchy4i_ CV_Hierarchy4i;

/**
 * @brief Defines the CV point list
 *
 * @since 11
 * @version 4.1
 */
struct CV_Points_ {
    /** The size of CV point list. */
    unsigned int size = 0;
    /** The data of CV point list. */
    CV_Point *points = nullptr;
};

/**
 * @brief Defines alias of the CV point list
 *
 * @since 11
 * @version 4.1
 */
typedef struct CV_Points_ CV_Points;

/**
 * @brief Defines the CV contour list
 *
 * @since 11
 * @version 4.1
 */
struct CV_Contours_ {
    /** The size of CV contour list. */
    unsigned int size = 0;
    /** The data of CV contour list. */
    CV_Points *contours = nullptr;
};

/**
 * @brief Defines alias of the CV contour list
 *
 * @since 11
 * @version 4.1
 */
typedef struct CV_Contours_ CV_Contours;

/**
 * @brief Defines the CV hierarchy list
 *
 * @since 11
 * @version 4.1
 */
struct CV_Hierarchys_ {
    /** The size of CV hierarchy list. */
    unsigned int size = 0;
    /** The data of CV hierarchy list. */
    CV_Hierarchy4i *hierarchys = nullptr;
};

/**
 * @brief Defines alias of the CV hierarchy list
 *
 * @since 11
 * @version 4.1
 */
typedef struct CV_Hierarchys_ CV_Hierarchys;

/**
 * @brief Defines the CV position properties.
 *
 * @since 11
 * @version 4.1
 */
struct CV_Position_ {
    /** The top of CV position. */
    int top = 0;
    /** The left of CV position. */
    int left = 0;
    /** The bottom of CV position. */
    int bottom = 0;
    /** The right of CV position. */
    int right = 0;
};

/**
 * @brief Defines alias of the CV position properties.
 *
 * @since 11
 * @version 4.1
 */
typedef struct CV_Position_ CV_Position;

/**
 * @brief Defines the CV morphology operation options.
 *
 * @since 11
 * @version 4.1
 */
struct CV_Morph_Opts_ {
    /** The source matrix. */
    CV_Mat src;
    /** The result matrix. */
    CV_Mat *dst = nullptr;
    /** The kernel matrix. */
    CV_Mat kernel;
    /** The anchor point, the default is center of source matrix. */
    CV_Point anchor;
    /** The iterations of morphology operation. */
    int iterations = 1;
    /** The type of morphology operation, for {@link OH_CV_MorphologyEx} only. */
    CV_Morph_Type morphType = CV_MORPH_TYPE_INVAILD;
    /** The border type of morphology operation. */
    CV_Border_Type borderType = CV_BORDER_TYPE_CONSTANT;
    /** The border scalar data of morphology operation. */
    CV_Scalar *borderValue = nullptr;
};

/**
 * @brief Defines alias of the CV morphology operation options.
 *
 * @since 11
 * @version 4.1
 */
typedef struct CV_Morph_Opts_ CV_Morph_Opts;

/**
 * @brief Defines the CV resize operation options.
 *
 * @since 11
 * @version 4.1
 */
struct CV_Resize_Opts_ {
    /** The source matrix. */
    CV_Mat src;
    /** The result matrix. */
    CV_Mat *dst = nullptr;
    /** The result size required for resize operation. */
    CV_Size dstSize;
    /** The x offset for resize operation. */
    double fx = 0;
    /** The y offset for resize operation. */
    double fy = 0;
    /** The interpolation for resize operation. */
    CV_Interpolation_Flag interpolation = CV_INTERPOLATION_LINEAR;
};

/**
 * @brief Defines alias of the CV resize operation options.
 *
 * @since 11
 * @version 4.1
 */
typedef struct CV_Resize_Opts_ CV_Resize_Opts;

/**
 * @brief Defines the CV warp operation options.
 *
 * @since 11
 * @version 4.1
 */
struct CV_Warp_Opts_ {
    /** The source matrix. */
    CV_Mat src;
    /** The result matrix. */
    CV_Mat *dst = nullptr;
    /** The m matrix. */
    CV_Mat m;
    /** The result size required for warp operation. */
    CV_Size dstSize;
    /** The interpolation for warp operation. */
    CV_Interpolation_Flag interpolation = CV_INTERPOLATION_LINEAR;
    /** The border type of warp operation. */
    CV_Border_Type borderType = CV_BORDER_TYPE_CONSTANT;
    /** The border scalar data of warp operation. */
    CV_Scalar *borderValue = nullptr;
};

/**
 * @brief Defines alias of the CV warp operation options.
 *
 * @since 11
 * @version 4.1
 */
typedef struct CV_Warp_Opts_ CV_Warp_Opts;

/**
 * @brief Defines the CV remap operation options.
 *
 * @since 11
 * @version 4.1
 */
struct CV_Remap_Opts_ {
    /** The source matrix. */
    CV_Mat src;
    /** The result matrix. */
    CV_Mat *dst = nullptr;
    /** The map1 matrix. */
    CV_Mat map1;
    /** The map2 matrix. */
    CV_Mat map2;
    /** The interpolation for remap operation. */
    CV_Interpolation_Flag interpolation = CV_INTERPOLATION_LINEAR;
    /** The border type of remap operation. */
    CV_Border_Type borderType = CV_BORDER_TYPE_CONSTANT;
    /** The border scalar data of remap operation. */
    CV_Scalar *borderValue = nullptr;
};

/**
 * @brief Defines alias of the CV remap operation options.
 *
 * @since 11
 * @version 4.1
 */
typedef struct CV_Remap_Opts_ CV_Remap_Opts;

/**
 * @brief Defines the CV get rotation matrix 2D options.
 *
 * @since 11
 * @version 4.1
 */
struct CV_GetRotationMatrix2D_Opts_ {
    /** The center point of get rotation matrix operation. */
    CV_Point2f center;
    /** The result matrix. */
    CV_Mat *dst = nullptr;
    /** The angle for get rotation matrix operation. */
    double angle = 0;
    /** The scale for get rotation matrix operation. */
    double scale = 0;
};

/**
 * @brief Defines alias of the CV get rotation matrix 2D options.
 *
 * @since 11
 * @version 4.1
 */
typedef struct CV_GetRotationMatrix2D_Opts_ CV_GetRotationMatrix2D_Opts;

/**
 * @brief Defines the CV get perspective transform matrix options.
 *
 * @since 11
 * @version 4.1
 */
struct CV_GetPerspectiveTransform_Opts_ {
    /** The source matrix 1. */
    CV_Mat src1;
    /** The source matrix 2. */
    CV_Mat src2;
    /** The result matrix. */
    CV_Mat *dst = nullptr;
    /** The matrix decomposition type of get perspective transform matrix operation. */
    CV_Decomp_Type solve = CV_DECOMP_TYPE_LU;
};

/**
 * @brief Defines alias of the CV get perspective transform matrix options.
 *
 * @since 11
 * @version 4.1
 */
typedef struct CV_GetPerspectiveTransform_Opts_ CV_GetPerspectiveTransform_Opts;

/**
 * @brief Defines the CV sobel operation options.
 *
 * @since 11
 * @version 4.1
 */
struct CV_Sobel_Opts_ {
    /** The source matrix. */
    CV_Mat src;
    /** The result matrix. */
    CV_Mat *dst = nullptr;
    /** The result matrix depth for sobel operation. */
    int dDepth = 0;
    /** The x offset for sobel operation. */
    int dx = 0;
    /** The y offset for sobel operation. */
    int dy = 0;
    /** The kernel size for sobel operation. */
    int kSize = 3;
    /** The scale for sobel operation. */
    double scale = 1;
    /** The delta for sobel operation. */
    double delta = 0;
    /** The border type of sobel operation. */
    CV_Border_Type borderType = CV_BORDER_TYPE_DEFAULT;
};

/**
 * @brief Defines alias of the CV sobel operation options.
 *
 * @since 11
 * @version 4.1
 */
typedef struct CV_Sobel_Opts_ CV_Sobel_Opts;

/**
 * @brief Defines the CV threshold operation options.
 *
 * @since 11
 * @version 4.1
 */
struct CV_Threshold_Opts_ {
    /** The source matrix. */
    CV_Mat src;
    /** The result matrix. */
    CV_Mat *dst = nullptr;
    /** The threshold value for threshold operation. */
    double thresh = 0;
    /** The max value for threshold operation. */
    double maxval = 0;
    /** The threshold type for threshold operation. */
    CV_Threshold_Type type = CV_THRESHOLD_TYPE_BINARY;
};

/**
 * @brief Defines alias of the CV threshold operation options.
 *
 * @since 11
 * @version 4.1
 */
typedef struct CV_Threshold_Opts_ CV_Threshold_Opts;

/**
 * @brief Defines the CV convert color operation options.
 *
 * @since 11
 * @version 4.1
 */
struct CV_CvtColor_Opts_ {
    /** The source matrix. */
    CV_Mat src;
    /** The result matrix. */
    CV_Mat *dst = nullptr;
    /** The color conversion type for convert color operation. */
    CV_Color_Conversion_Code code = CV_COLOR_CONVERSION_CODE_BGR2BGRA;
    /** The result channel for convert color operation. */
    int dstChannel = 0;
};

/**
 * @brief Defines alias of the CV convert color operation options.
 *
 * @since 11
 * @version 4.1
 */
typedef struct CV_CvtColor_Opts_ CV_CvtColor_Opts;

/**
 * @brief Defines the CV blur operation options.
 *
 * @since 11
 * @version 4.1
 */
struct CV_Blur_Opts_ {
    /** The source matrix. */
    CV_Mat src;
    /** The result matrix. */
    CV_Mat *dst = nullptr;
    /** The kernel size for blur operation. */
    CV_Size kSize;
    /** The anchor point. */
    CV_Point anchor;
    /** The border type of blur operation. */
    CV_Border_Type borderType = CV_BORDER_TYPE_DEFAULT;
};

/**
 * @brief Defines alias of the CV blur operation options.
 *
 * @since 11
 * @version 4.1
 */
typedef struct CV_Blur_Opts_ CV_Blur_Opts;

/**
 * @brief Defines the CV fill poly operation options.
 *
 * @since 11
 * @version 4.1
 */
struct CV_FillPoly_Opts_ {
    /** The source matrix. */
    CV_Mat src;
    /** The fill points matrix. */
    CV_Mat pts;
    /** The result matrix. */
    CV_Mat *dst = nullptr;
    /** The color scalar of fill poly operation. */
    CV_Scalar color;
    /** The line type of fill poly operation. */
    CV_Line_Type lintType = CV_LINE_TYPE_LINE_8;
    /** The shift of fill poly operation. */
    int shift = 0;
    /** The offset point of fill poly operation. */
    CV_Point *offset = nullptr;
};

/**
 * @brief Defines alias of the CV fill poly operation options.
 *
 * @since 11
 * @version 4.1
 */
typedef struct CV_FillPoly_Opts_ CV_FillPoly_Opts;

/**
 * @brief Defines the CV adaptive threshold operation options.
 *
 * @since 11
 * @version 4.1
 */
struct CV_AdaptiveThreshold_Opts_ {
    /** The source matrix. */
    CV_Mat src;
    /** The result matrix. */
    CV_Mat *dst = nullptr;
    /** The max value for adaptive threshold operation. */
    double maxValue = 0;
    /** The adaptive threshold method for adaptive threshold operation. */
    CV_Adaptive_Threshold_Type adaptiveMethod;
    /** The threshold type for adaptive threshold operation. */
    CV_Threshold_Type thresholdType;
    /** The block size for adaptive threshold operation. */
    int blockSize = 0;
    /** The delta for adaptive threshold operation. */
    double delta = 0;
};

/**
 * @brief Defines alias of the CV adaptive threshold operation options.
 *
 * @since 11
 * @version 4.1
 */
typedef struct CV_AdaptiveThreshold_Opts_ CV_AdaptiveThreshold_Opts;

/**
 * @brief Defines the CV find contours operation options.
 *
 * @since 11
 * @version 4.1
 */
struct CV_FindContours_Opts_ {
    /** The source matrix. */
    CV_Mat src;
    /** The contour list as find contours operation result. */
    CV_Contours *contours = nullptr;
    /** The hierarchy struct as find contours operation result. */
    CV_Hierarchys *hierarchys = nullptr;
    /** The retrieval mode for find contours operation. */
    CV_Retrieval_Mode mode;
    /** The approximation method for find contours operation. */
    CV_Approximation_Method method;
    /** The offset point for find contours operation. */
    CV_Point *offset = nullptr;
};

/**
 * @brief Defines alias of the CV find contours operation options.
 *
 * @since 11
 * @version 4.1
 */
typedef struct CV_FindContours_Opts_ CV_FindContours_Opts;

/**
 * @brief Defines the CV box filter operation options.
 *
 * @since 11
 * @version 4.1
 */
struct CV_BoxFilter_Opts_ {
    /** The source matrix. */
    CV_Mat src;
    /** The result matrix. */
    CV_Mat *dst = nullptr;
    /** The result matrix depth for box filter operation. */
    int dDepth = 0;
    /** The kernel size for box filter operation. */
    CV_Size kSize;
    /** The anchor point. */
    CV_Point anchor;
    /** Is normalize for box filter operation. */
    CV_Boolean_Value normalize = CV_BOOLEAN_VALUE_TRUE;
    /** The border type of box filter operation. */
    CV_Border_Type borderType = CV_BORDER_TYPE_DEFAULT;
};

/**
 * @brief Defines alias of the CV box filter operation options.
 *
 * @since 11
 * @version 4.1
 */
typedef struct CV_BoxFilter_Opts_ CV_BoxFilter_Opts;

/**
 * @brief Defines the CV border interpolate operation options.
 *
 * @since 11
 * @version 4.1
 */
struct CV_BorderInterpolate_Opts_ {
    /** The 0-based coordinate of the extrapolated pixel along one of the axes. */
    int p = 0;
    /** The length of the array along the corresponding axis. */
    int len = 0;
    /** The result of border interpolate operation. */
    int *res = nullptr;
    /** The border type of border interpolate operation. */
    CV_Border_Type borderType = CV_BORDER_TYPE_DEFAULT;
};

/**
 * @brief Defines alias of the CV border interpolate operation options.
 *
 * @since 11
 * @version 4.1
 */
typedef struct CV_BorderInterpolate_Opts_ CV_BorderInterpolate_Opts;

/**
 * @brief Defines the CV count non-zero operation options.
 *
 * @since 11
 * @version 4.1
 */
struct CV_CountNonZero_Opts_ {
    /** The source matrix. */
    CV_Mat src;
    /** The result of count non-zero operation. */
    int *res = nullptr;
};

/**
 * @brief Defines alias of the CV count non-zero operation options.
 *
 * @since 11
 * @version 4.1
 */
typedef struct CV_CountNonZero_Opts_ CV_CountNonZero_Opts;

/**
 * @brief Defines the CV copy make border operation options.
 *
 * @since 11
 * @version 4.1
 */
struct CV_CopyMakeBorder_Opts_ {
    /** The source matrix. */
    CV_Mat src;
    /** The result matrix. */
    CV_Mat *dst = nullptr;
    /** The position for copy make border operation. */
    CV_Position position;
    /** The border type of copy make border operation. */
    CV_Border_Type borderType = CV_BORDER_TYPE_CONSTANT;
    /** The border scalar data of copy make border operation. */
    CV_Scalar *borderValue = nullptr;
};

/**
 * @brief Defines alias of the CV copy make border operation options.
 *
 * @since 11
 * @version 4.1
 */
typedef struct CV_CopyMakeBorder_Opts_ CV_CopyMakeBorder_Opts;

/**
 * @brief Defines the CV split operation options.
 *
 * @since 11
 * @version 4.1
 */
struct CV_Split_Opts_ {
    /** The source matrix. */
    CV_Mat src;
    /** The result matrix list data. */
    CV_Mat **dst = nullptr;
    /** The result matrix list data size. */
    unsigned int *dstSize = nullptr;
};

/**
 * @brief Defines alias of the CV split operation options.
 *
 * @since 11
 * @version 4.1
 */
typedef struct CV_Split_Opts_ CV_Split_Opts;

/**
 * @brief Defines the CV merge operation options.
 *
 * @since 11
 * @version 4.1
 */
struct CV_Merge_Opts_ {
    /** The source matrix list data. */
    CV_Mat *src = nullptr;
    /** The source matrix list data size. */
    unsigned int srcSize;
    /** The result matrix. */
    CV_Mat *dst = nullptr;
};

/**
 * @brief Defines alias of the CV merge operation options.
 *
 * @since 11
 * @version 4.1
 */
typedef struct CV_Merge_Opts_ CV_Merge_Opts;

/**
 * @brief Defines the CV min max location operation options.
 *
 * @since 11
 * @version 4.1
 */
struct CV_MinMaxLoc_Opts_ {
    /** The source matrix. */
    CV_Mat src;
    /** The minimum as result for min max location operation. */
    double *minValue = nullptr;
    /** The maximum as result for min max location operation. */
    double *maxValue = nullptr;
    /** The minimum point location as result for min max location operation. */
    CV_Point *minIndex = nullptr;
    /** The maximum point location as result for min max location operation. */
    CV_Point *maxIndex = nullptr;
    /** The mask matrix for min max location operation. */
    CV_Mat *mask = nullptr;
};

/**
 * @brief Defines alias of the CV min max location operation options.
 *
 * @since 11
 * @version 4.1
 */
typedef struct CV_MinMaxLoc_Opts_ CV_MinMaxLoc_Opts;

/**
 * @brief Defines the CV normalize operation options.
 *
 * @since 11
 * @version 4.1
 */
struct CV_Normalize_Opts_ {
    /** The source matrix. */
    CV_Mat src;
    /** The result matrix. */
    CV_Mat *dst = nullptr;
    /** The alpha value for normalize operation. */
    double alpha = 1;
    /** The beta value for normalize operation. */
    double beta = 0;
    /** The normalize type for normalize operation. */
    CV_Normalize_Type normType = CV_NORMALIZE_TYPE_L2;
    /** The result type for normalize operation. */
    int dType = -1;
    /** The mask matrix for normalize operation. */
    CV_Mat *mask = nullptr;
};

/**
 * @brief Defines alias of the CV normalize operation options.
 *
 * @since 11
 * @version 4.1
 */
typedef struct CV_Normalize_Opts_ CV_Normalize_Opts;

/**
 * @brief Defines the CV mix channels operation options.
 *
 * @since 11
 * @version 4.1
 */
struct CV_MixChannels_Opts_ {
    /** The source matrix list. */
    CV_Mat *src = nullptr;
    /** The source matrix list size. */
    unsigned int srcSize = 0;
    /** The result matrix list. */
    CV_Mat *dst = nullptr;
    /** The result matrix list size. */
    unsigned int dstSize = 0;
    /** The formTo list. */
    int *formTo = nullptr;
    /** The formTo list size. */
    unsigned int formToSize = 0;
    /** The paris count in formTo list. */
    unsigned int parisCount = 0;
};

/**
 * @brief Defines alias of the CV mix channels operation options.
 *
 * @since 11
 * @version 4.1
 */
typedef struct CV_MixChannels_Opts_ CV_MixChannels_Opts;

/**
 * @brief Erodes source matrix by using a specific structuring element.
 *
 * @param opts Indicates the erodes options {@link CV_Morph_Opts}.
 * @return Returns {@link IRNdkErrCode} IMAGE_RESULT_SUCCESS - if the operation is successful.
 * returns {@link IRNdkErrCode} IMAGE_RESULT_INVALID_PARAMETER - if invalid parameter.
 * returns {@link IRNdkErrCode} IMAGE_RESULT_MALLOC_ABNORMAL - if malloc internal buffer error
 * @since 11
 * @version 4.1
 */
int32_t OH_CV_Erode(CV_Morph_Opts opts);

/**
 * @brief Dilate source matrix by using a specific structuring element.
 *
 * @param opts Indicates the dilate options {@link CV_Morph_Opts}.
 * @return Returns {@link IRNdkErrCode} IMAGE_RESULT_SUCCESS - if the operation is successful.
 * returns {@link IRNdkErrCode} IMAGE_RESULT_INVALID_PARAMETER - if invalid parameter.
 * returns {@link IRNdkErrCode} IMAGE_RESULT_MALLOC_ABNORMAL - if malloc internal buffer error
 * @since 11
 * @version 4.1
 */
int32_t OH_CV_Dilate(CV_Morph_Opts opts);

/**
 * @brief Extend morphology operating to source matrix by using a specific structuring element.
 *
 * @param opts Indicates the extend morphology options {@link CV_Morph_Opts} .
 * @return Returns {@link IRNdkErrCode} IMAGE_RESULT_SUCCESS - if the operation is successful.
 * returns {@link IRNdkErrCode} IMAGE_RESULT_INVALID_PARAMETER - if invalid parameter.
 * returns {@link IRNdkErrCode} IMAGE_RESULT_MALLOC_ABNORMAL - if malloc internal buffer error
 * @since 11
 * @version 4.1
 */
int32_t OH_CV_MorphologyEx(CV_Morph_Opts opts);

/**
 * @brief Resizes the source matrix to or up to the specified size.
 *
 * @param opts Indicates the resize options {@link CV_Resize_Opts}.
 * @return Returns {@link IRNdkErrCode} IMAGE_RESULT_SUCCESS - if the operation is successful.
 * returns {@link IRNdkErrCode} IMAGE_RESULT_INVALID_PARAMETER - if invalid parameter.
 * returns {@link IRNdkErrCode} IMAGE_RESULT_MALLOC_ABNORMAL - if malloc internal buffer error
 * @since 11
 * @version 4.1
 */
int32_t OH_CV_Resize(CV_Resize_Opts opts);

/**
 * @brief Applies an affine transformation to the source matrix.
 *
 * @param opts Indicates the warp affine options {@link CV_Warp_Opts}.
 * @return Returns {@link IRNdkErrCode} IMAGE_RESULT_SUCCESS - if the operation is successful.
 * returns {@link IRNdkErrCode} IMAGE_RESULT_INVALID_PARAMETER - if invalid parameter.
 * returns {@link IRNdkErrCode} IMAGE_RESULT_MALLOC_ABNORMAL - if malloc internal buffer error
 * @since 11
 * @version 4.1
 */
int32_t OH_CV_WarpAffine(CV_Warp_Opts opts);

/**
 * @brief Applies a perspective transformation to the source matrix.
 *
 * @param opts Indicates the warp perspective options {@link CV_Warp_Opts}.
 * @return Returns {@link IRNdkErrCode} IMAGE_RESULT_SUCCESS - if the operation is successful.
 * returns {@link IRNdkErrCode} IMAGE_RESULT_INVALID_PARAMETER - if invalid parameter.
 * returns {@link IRNdkErrCode} IMAGE_RESULT_MALLOC_ABNORMAL - if malloc internal buffer error
 * @since 11
 * @version 4.1
 */
int32_t OH_CV_WarpPerspective(CV_Warp_Opts opts);

/**
 * @brief Applies a generic geometrical transformation to the source matrix.
 *
 * @param opts Indicates the remap options {@link CV_Remap_Opts}.
 * @return Returns {@link IRNdkErrCode} IMAGE_RESULT_SUCCESS - if the operation is successful.
 * returns {@link IRNdkErrCode} IMAGE_RESULT_INVALID_PARAMETER - if invalid parameter.
 * returns {@link IRNdkErrCode} IMAGE_RESULT_MALLOC_ABNORMAL - if malloc internal buffer error
 * @since 11
 * @version 4.1
 */
int32_t OH_CV_Remap(CV_Remap_Opts opts);

/**
 * @brief Calculates an affine matrix of 2D rotation.
 *
 * @param opts Indicates the get rotation matrix 2D options {@link CV_GetRotationMatrix2D_Opts}.
 * @return Returns {@link IRNdkErrCode} IMAGE_RESULT_SUCCESS - if the operation is successful.
 * returns {@link IRNdkErrCode} IMAGE_RESULT_INVALID_PARAMETER - if invalid parameter.
 * returns {@link IRNdkErrCode} IMAGE_RESULT_MALLOC_ABNORMAL - if malloc internal buffer error
 * @since 11
 * @version 4.1
 */
int32_t OH_CV_GetRotationMatrix2D(CV_GetRotationMatrix2D_Opts opts);

/**
 * @brief Calculates a perspective transform matrix from the source points.
 *
 * @param opts Indicates the get perspective transform options {@link CV_GetPerspectiveTransform_Opts}.
 * @return Returns {@link IRNdkErrCode} IMAGE_RESULT_SUCCESS - if the operation is successful.
 * returns {@link IRNdkErrCode} IMAGE_RESULT_INVALID_PARAMETER - if invalid parameter.
 * returns {@link IRNdkErrCode} IMAGE_RESULT_MALLOC_ABNORMAL - if malloc internal buffer error
 * @since 11
 * @version 4.1
 */
int32_t OH_CV_GetPerspectiveTransform(CV_GetPerspectiveTransform_Opts opts);

/**
 * @brief Applies an extended sobel operator to the source matrix.
 *
 * @param opts Indicates the sobel options {@link CV_Sobel_Opts}.
 * @return Returns {@link IRNdkErrCode} IMAGE_RESULT_SUCCESS - if the operation is successful.
 * returns {@link IRNdkErrCode} IMAGE_RESULT_INVALID_PARAMETER - if invalid parameter.
 * returns {@link IRNdkErrCode} IMAGE_RESULT_MALLOC_ABNORMAL - if malloc internal buffer error
 * @since 11
 * @version 4.1
 */
int32_t OH_CV_Sobel(CV_Sobel_Opts opts);

/**
 * @brief Applies a fixed-level threshold to the source matrix element.
 *
 * @param opts Indicates the threshold options {@link CV_Threshold_Opts}.
 * @return Returns {@link IRNdkErrCode} IMAGE_RESULT_SUCCESS - if the operation is successful.
 * returns {@link IRNdkErrCode} IMAGE_RESULT_INVALID_PARAMETER - if invalid parameter.
 * returns {@link IRNdkErrCode} IMAGE_RESULT_MALLOC_ABNORMAL - if malloc internal buffer error
 * @since 11
 * @version 4.1
 */
int32_t OH_CV_Threshold(CV_Threshold_Opts opts);

/**
 * @brief Converts the source matrix from one color type to another.
 *
 * @param opts Indicates the convert color options {@link CV_CvtColor_Opts}.
 * @return Returns {@link IRNdkErrCode} IMAGE_RESULT_SUCCESS - if the operation is successful.
 * returns {@link IRNdkErrCode} IMAGE_RESULT_INVALID_PARAMETER - if invalid parameter.
 * returns {@link IRNdkErrCode} IMAGE_RESULT_MALLOC_ABNORMAL - if malloc internal buffer error
 * @since 11
 * @version 4.1
 */
int32_t OH_CV_CvtColor(CV_CvtColor_Opts opts);

/**
 * @brief Blurs the source matrix using the normalized box filter.
 *
 * @param opts Indicates the blur options {@link CV_Blur_Opts}.
 * @return Returns {@link IRNdkErrCode} IMAGE_RESULT_SUCCESS - if the operation is successful.
 * returns {@link IRNdkErrCode} IMAGE_RESULT_INVALID_PARAMETER - if invalid parameter.
 * returns {@link IRNdkErrCode} IMAGE_RESULT_MALLOC_ABNORMAL - if malloc internal buffer error
 * @see {@link OH_CV_BoxFilter}
 * @since 11
 * @version 4.1
 */
int32_t OH_CV_Blur(CV_Blur_Opts opts);

/**
 * @brief Fills the source matrix bounded by one or more polygons.
 *
 * @param opts Indicates the fill poly options {@link CV_FillPoly_Opts}.
 * @return Returns {@link IRNdkErrCode} IMAGE_RESULT_SUCCESS - if the operation is successful.
 * returns {@link IRNdkErrCode} IMAGE_RESULT_INVALID_PARAMETER - if invalid parameter.
 * returns {@link IRNdkErrCode} IMAGE_RESULT_MALLOC_ABNORMAL - if malloc internal buffer error
 * @since 11
 * @version 4.1
 */
int32_t OH_CV_FillPoly(CV_FillPoly_Opts opts);

/**
 * @brief Applies an adaptive threshold to the source matrix.
 *
 * @param opts Indicates the adaptive threshold options {@link CV_AdaptiveThreshold_Opts}.
 * @return Returns {@link IRNdkErrCode} IMAGE_RESULT_SUCCESS - if the operation is successful.
 * returns {@link IRNdkErrCode} IMAGE_RESULT_INVALID_PARAMETER - if invalid parameter.
 * returns {@link IRNdkErrCode} IMAGE_RESULT_MALLOC_ABNORMAL - if malloc internal buffer error
 * @since 11
 * @version 4.1
 */
int32_t OH_CV_AdaptiveThreshold(CV_AdaptiveThreshold_Opts opts);

/**
 * @brief Finds contours in the source matrix.
 *
 * @param opts Indicates the find contours options {@link CV_FindContours_Opts}.
 * @return Returns {@link IRNdkErrCode} IMAGE_RESULT_SUCCESS - if the operation is successful.
 * returns {@link IRNdkErrCode} IMAGE_RESULT_INVALID_PARAMETER - if invalid parameter.
 * returns {@link IRNdkErrCode} IMAGE_RESULT_DATA_ABNORMAL - if output target abnormal
 * returns {@link IRNdkErrCode} IMAGE_RESULT_MALLOC_ABNORMAL - if malloc internal buffer error
 * @since 11
 * @version 4.1
 */
int32_t OH_CV_FindContours(CV_FindContours_Opts opts);

/**
 * @brief Blurs the source matrix using the box filter.
 *
 * @param opts Indicates the box filter options {@link CV_BoxFilter_Opts}.
 * @return Returns {@link IRNdkErrCode} IMAGE_RESULT_SUCCESS - if the operation is successful.
 * returns {@link IRNdkErrCode} IMAGE_RESULT_INVALID_PARAMETER - if invalid parameter.
 * returns {@link IRNdkErrCode} IMAGE_RESULT_MALLOC_ABNORMAL - if malloc internal buffer error
 * @see {@link OH_CV_Blur}
 * @since 11
 * @version 4.1
 */
int32_t OH_CV_BoxFilter(CV_BoxFilter_Opts opts);

/**
 * @brief Computes the source location of an extrapolated pixel.
 *
 * @param opts Indicates the border interpolate options {@link CV_BorderInterpolate_Opts}.
 * @return Returns {@link IRNdkErrCode} IMAGE_RESULT_SUCCESS - if the operation is successful.
 * returns {@link IRNdkErrCode} IMAGE_RESULT_INVALID_PARAMETER - if invalid parameter.
 * @since 11
 * @version 4.1
 */
int32_t OH_CV_BorderInterpolate(CV_BorderInterpolate_Opts opts);

/**
 * @brief Counts non-zero matrix elements.
 *
 * @param opts Indicates the count non-zero options {@link CV_CountNonZero_Opts}.
 * @return Returns {@link IRNdkErrCode} IMAGE_RESULT_SUCCESS - if the operation is successful.
 * returns {@link IRNdkErrCode} IMAGE_RESULT_INVALID_PARAMETER - if invalid parameter.
 * @since 11
 * @version 4.1
 */
int32_t OH_CV_CountNonZero(CV_CountNonZero_Opts opts);

/**
 * @brief Forms a border around the source matrix.
 *
 * @param opts Indicates the copy make border options {@link CV_CopyMakeBorder_Opts}.
 * @return Returns {@link IRNdkErrCode} IMAGE_RESULT_SUCCESS - if the operation is successful.
 * returns {@link IRNdkErrCode} IMAGE_RESULT_INVALID_PARAMETER - if invalid parameter.
 * returns {@link IRNdkErrCode} IMAGE_RESULT_MALLOC_ABNORMAL - if malloc internal buffer error
 * @since 11
 * @version 4.1
 */
int32_t OH_CV_CopyMakeBorder(CV_CopyMakeBorder_Opts opts);

/**
 * @brief Divides a multi-channel source matrix into several single-channel matrix.
 *
 * @param opts Indicates the split options {@link CV_Split_Opts} .
 * @return Returns {@link IRNdkErrCode} IMAGE_RESULT_SUCCESS - if the operation is successful.
 * returns {@link IRNdkErrCode} IMAGE_RESULT_INVALID_PARAMETER - if invalid parameter.
 * returns {@link IRNdkErrCode} ERR_IMAGE_DATA_ABNORMAL - if output target abnormal
 * returns {@link IRNdkErrCode} ERR_IMAGE_MALLOC_ABNORMAL - if malloc internal buffer error
 * @since 11
 * @version 4.1
 */
int32_t OH_CV_Split(CV_Split_Opts opts);

/**
 * @brief Creates one multi-channel matrix by merging several single-channel source matrixes.
 *
 * @param opts Indicates the merge options {@link CV_Merge_Opts}.
 * @return Returns {@link IRNdkErrCode} IMAGE_RESULT_SUCCESS - if the operation is successful.
 * returns {@link IRNdkErrCode} IMAGE_RESULT_INVALID_PARAMETER - if invalid parameter.
 * returns {@link IRNdkErrCode} IMAGE_RESULT_MALLOC_ABNORMAL - if malloc internal buffer error
 * @since 11
 * @version 4.1
 */
int32_t OH_CV_Merge(CV_Merge_Opts opts);

/**
 * @brief Finds the global minimum and maximum in the source matrix.
 *
 * @param opts Indicates the find minimum and maximum options {@link CV_MinMaxLoc_Opts}.
 * @return Returns {@link IRNdkErrCode} IMAGE_RESULT_SUCCESS - if the operation is successful.
 * returns {@link IRNdkErrCode} IMAGE_RESULT_INVALID_PARAMETER - if invalid parameter.
 * @since 11
 * @version 4.1
 */
int32_t OH_CV_MinMaxLoc(CV_MinMaxLoc_Opts opts);

/**
 * @brief Normalizes the norm or value range of the source matrix.
 *
 * @param opts Indicates the normalize options {@link CV_Normalize_Opts}.
 * @return Returns {@link IRNdkErrCode} IMAGE_RESULT_SUCCESS - if the operation is successful.
 * returns {@link IRNdkErrCode} IMAGE_RESULT_INVALID_PARAMETER - if invalid parameter.
 * returns {@link IRNdkErrCode} IMAGE_RESULT_MALLOC_ABNORMAL - if malloc internal buffer error
 * @since 11
 * @version 4.1
 */
int32_t OH_CV_Normalize(CV_Normalize_Opts opts);

/**
 * @brief Copies specified channels from input source matrixes to the specified channels of result matrixes.
 *
 * @param opts Indicates the mix channels options {@link CV_MixChannels_Opts}.
 * @return Returns {@link IRNdkErrCode} IMAGE_RESULT_SUCCESS - if the operation is successful.
 * returns {@link IRNdkErrCode} IMAGE_RESULT_INVALID_PARAMETER - if invalid parameter.
 * returns {@link IRNdkErrCode} IMAGE_RESULT_MALLOC_ABNORMAL - if malloc internal buffer error
 * @since 11
 * @version 4.1
 */
int32_t OH_CV_MixChannels(CV_MixChannels_Opts opts);
#ifdef __cplusplus
};
#endif
/** @} */
#endif // INTERFACES_KITS_NATIVE_INCLUDE_OPENCV_NATIVE_H_
