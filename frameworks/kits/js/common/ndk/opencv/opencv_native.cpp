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

#include "opencv_native.h"

#include "common_utils.h"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/core/utility.hpp"
#include "securec.h"

namespace {
    constexpr int32_t NUM_0 = 0;
    constexpr int32_t NUM_1 = 1;
    constexpr int32_t NUM_2 = 2;
    constexpr int32_t NUM_3 = 3;
    constexpr int32_t NUM_4 = 4;
}

#ifdef __cplusplus
extern "C" {
#endif

static inline cv::Mat CVMatToNative(CV_Mat &mat)
{
    return cv::Mat(mat.rows, mat.cols, mat.type, mat.data);
}

static inline cv::Point CVPointToNative(CV_Point &point)
{
    return cv::Point(point.x, point.y);
}

static inline cv::Point2f CVPoint2fToNative(CV_Point2f &point)
{
    return cv::Point2f(point.x, point.y);
}

static inline cv::Scalar CVScalarToNative(CV_Scalar &scalar)
{
    return cv::Scalar(scalar.scalarX, scalar.scalarY, scalar.scalarZ);
}

static inline cv::Size CVSizeToNative(CV_Size &size)
{
    return cv::Size(size.width, size.height);
}

static inline size_t GetMatDataSize(const cv::Mat &mat)
{
    return mat.step[NUM_0] * mat.rows;
}

static int32_t NativeToCVMat(cv::Mat &src, CV_Mat &mat)
{
    size_t size = GetMatDataSize(src);
    if (size == NUM_0) {
        return IMAGE_RESULT_INVALID_PARAMETER;
    }
    mat.rows = src.rows;
    mat.cols = src.cols;
    mat.type = static_cast<CV_Mat_Type>(src.type());
    mat.dataSize = size;
    auto data = std::make_unique<uint8_t[]>(size);
    if (data == nullptr) {
        return IMAGE_RESULT_MALLOC_ABNORMAL;
    }
    mat.data = data.release();
    memcpy_s(mat.data, size, src.data, size);
    return IMAGE_RESULT_SUCCESS;
}

static int32_t NativeToCVMatDataOnly(cv::Mat &src, CV_Mat &mat)
{
    size_t size = GetMatDataSize(src);
    if (size == NUM_0) {
        return IMAGE_RESULT_INVALID_PARAMETER;
    }
    mat.dataSize = size;
    if (mat.data == nullptr) {
        auto data = std::make_unique<uint8_t[]>(size);
        if (data == nullptr) {
            return IMAGE_RESULT_MALLOC_ABNORMAL;
        }
        mat.data = data.release();
    }
    memcpy_s(mat.data, size, src.data, size);
    return IMAGE_RESULT_SUCCESS;
}

static void NativeToCVPointi(cv::Point &src, CV_Point &point)
{
    point.x = src.x;
    point.y = src.y;
}

static void NativeToCVHierarchy(cv::Vec4i &src, CV_Hierarchy4i &hierarchy)
{
    hierarchy.v0 = src[NUM_0];
    hierarchy.v1 = src[NUM_1];
    hierarchy.v2 = src[NUM_2];
    hierarchy.v3 = src[NUM_3];
}

static int32_t NativeToCVContours(std::vector<std::vector<cv::Point>> &src,
    std::vector<cv::Vec4i> &sHier, CV_Contours *dst, CV_Hierarchys *dHier = nullptr)
{
    auto contourCount = src.size();
    if (dst == nullptr || contourCount == NUM_0) {
        return IMAGE_RESULT_INVALID_PARAMETER;
    }

    auto contours = std::make_unique<CV_Points[]>(contourCount);
    if (contours == nullptr) {
        return IMAGE_RESULT_MALLOC_ABNORMAL;
    }

    std::unique_ptr<CV_Hierarchy4i []> hierarchys = nullptr;
    if (dHier != nullptr) {
        hierarchys = std::make_unique<CV_Hierarchy4i[]>(contourCount);
        if (hierarchys == nullptr) {
            return IMAGE_RESULT_MALLOC_ABNORMAL;
        }
    }

    for (size_t i = NUM_0; i < contourCount; i++) {
        auto contourSize = src[i].size();
        if (contourSize == NUM_0) {
            return IMAGE_RESULT_DATA_ABNORMAL;
        }
        auto points = std::make_unique<CV_Point[]>(contourSize);
        if (points == nullptr) {
            return IMAGE_RESULT_MALLOC_ABNORMAL;
        }
        for (size_t j = NUM_0; j < contourSize; j++) {
            NativeToCVPointi(src[i][j], points[j]);
        }
        contours[i].size = contourSize;
        contours[i].points = points.release();
        if (hierarchys != nullptr) {
            NativeToCVHierarchy(sHier[i], hierarchys[i]);
        }
    }
    dst->size = contourCount;
    dst->contours = contours.release();
    if (dHier != nullptr && hierarchys != nullptr) {
        dHier->size = contourCount;
        dHier->hierarchys = hierarchys.release();
    }
    return IMAGE_RESULT_SUCCESS;
}

MIDK_EXPORT
int32_t OH_CV_Erode(CV_Morph_Opts opts)
{
    cv::Mat src = CVMatToNative(opts.src);
    if (src.empty() || opts.dst == nullptr) {
        return IMAGE_RESULT_INVALID_PARAMETER;
    }

    cv::Mat kernel = CVMatToNative(opts.kernel);
    cv::Point anchor = CVPointToNative(opts.anchor);
    cv::Mat dst;
    if (opts.borderValue != nullptr) {
        cv::Scalar scalar = CVScalarToNative(*opts.borderValue);
        cv::erode(src, dst, kernel, anchor, opts.iterations, opts.borderType, scalar);
    } else {
        cv::erode(src, dst, kernel, anchor, opts.iterations, opts.borderType);
    }
    return NativeToCVMat(dst, *opts.dst);
}

MIDK_EXPORT
int32_t OH_CV_Dilate(CV_Morph_Opts opts)
{
    cv::Mat src = CVMatToNative(opts.src);
    if (src.empty() || opts.dst == nullptr) {
        return IMAGE_RESULT_INVALID_PARAMETER;
    }

    cv::Mat kernel = CVMatToNative(opts.kernel);
    cv::Point anchor = CVPointToNative(opts.anchor);
    cv::Mat dst;
    if (opts.borderValue != nullptr) {
        cv::Scalar scalar = CVScalarToNative(*opts.borderValue);
        cv::dilate(src, dst, kernel, anchor, opts.iterations, opts.borderType, scalar);
    } else {
        cv::dilate(src, dst, kernel, anchor, opts.iterations, opts.borderType);
    }

    return NativeToCVMat(dst, *opts.dst);
}

MIDK_EXPORT
int32_t OH_CV_MorphologyEx(CV_Morph_Opts opts)
{
    cv::Mat src = CVMatToNative(opts.src);
    if (src.empty() || opts.dst == nullptr || opts.morphType == CV_MORPH_TYPE_INVAILD) {
        return IMAGE_RESULT_INVALID_PARAMETER;
    }

    cv::Mat kernel = CVMatToNative(opts.kernel);
    cv::Point anchor = CVPointToNative(opts.anchor);
    cv::Mat dst;
    if (opts.borderValue != nullptr) {
        cv::Scalar scalar = CVScalarToNative(*opts.borderValue);
        cv::morphologyEx(src, dst, opts.morphType, kernel, anchor, opts.iterations, opts.borderType, scalar);
    } else {
        cv::morphologyEx(src, dst, opts.morphType, kernel, anchor, opts.iterations, opts.borderType);
    }
    return NativeToCVMat(dst, *opts.dst);
}

MIDK_EXPORT
int32_t OH_CV_Resize(CV_Resize_Opts opts)
{
    cv::Mat src = CVMatToNative(opts.src);
    cv::Size dstSize = CVSizeToNative(opts.dstSize);
    if (src.empty() || opts.dst == nullptr) {
        return IMAGE_RESULT_INVALID_PARAMETER;
    }
    if (!dstSize.empty()) {
        auto srcSize = src.size();
        if (((dstSize.width / srcSize.width) <= NUM_0 ) ||
            ((dstSize.height / srcSize.height) <= NUM_0 )) {
            return IMAGE_RESULT_INVALID_PARAMETER;
        }
    } else if (opts.fx <= NUM_0 || opts.fy <= NUM_0) {
        return IMAGE_RESULT_INVALID_PARAMETER;
    }
    cv::Mat dst;
    cv::resize(src, dst, dstSize, opts.fx, opts.fy, opts.interpolation);
    return NativeToCVMat(dst, *opts.dst);
}

MIDK_EXPORT
int32_t OH_CV_WarpAffine(CV_Warp_Opts opts)
{
    cv::Mat src = CVMatToNative(opts.src);
    cv::Mat m = CVMatToNative(opts.m);
    cv::Size dstSize = CVSizeToNative(opts.dstSize);
    if (src.empty() || opts.dst == nullptr) {
        return IMAGE_RESULT_INVALID_PARAMETER;
    }

    if (!(src.channels() <= NUM_4 ||
        (opts.interpolation != CV_INTERPOLATION_LANCZOS4 &&
            opts.interpolation != CV_INTERPOLATION_CUBIC))) {
        return IMAGE_RESULT_INVALID_PARAMETER;
    }

    if (!((m.type() == CV_32F || m.type() == CV_64F) && m.rows == NUM_2 && m.cols == NUM_3)) {
        return IMAGE_RESULT_INVALID_PARAMETER;
    }

    cv::Mat dst;
    if (opts.borderValue != nullptr) {
        cv::Scalar scalar = CVScalarToNative(*opts.borderValue);
        cv::warpAffine(src, dst, m, dstSize, opts.interpolation, opts.borderType, scalar);
    } else {
        cv::warpAffine(src, dst, m, dstSize, opts.interpolation, opts.borderType);
    }
    return NativeToCVMat(dst, *opts.dst);
}

MIDK_EXPORT
int32_t OH_CV_WarpPerspective(CV_Warp_Opts opts)
{
    cv::Mat src = CVMatToNative(opts.src);
    cv::Mat m = CVMatToNative(opts.m);
    cv::Size dstSize = CVSizeToNative(opts.dstSize);
    if (src.empty() || src.total() <= NUM_0 || opts.dst == nullptr) {
        return IMAGE_RESULT_INVALID_PARAMETER;
    }

    if (!((m.type() == CV_32F || m.type() == CV_64F) && m.rows == NUM_3 && m.cols == NUM_3)) {
        return IMAGE_RESULT_INVALID_PARAMETER;
    }

    cv::Mat dst;
    if (opts.borderValue != nullptr) {
        cv::Scalar scalar = CVScalarToNative(*opts.borderValue);
        cv::warpPerspective(src, dst, m, dstSize, opts.interpolation, opts.borderType, scalar);
    } else {
        cv::warpPerspective(src, dst, m, dstSize, opts.interpolation, opts.borderType);
    }
    return NativeToCVMat(dst, *opts.dst);
}

MIDK_EXPORT
int32_t OH_CV_Remap(CV_Remap_Opts opts)
{
    cv::Mat src = CVMatToNative(opts.src);
    cv::Mat map1 = CVMatToNative(opts.map1);
    cv::Mat map2 = CVMatToNative(opts.map2);
    if (src.empty() || opts.dst == nullptr ||
        map1.empty() || (!map2.empty() && map2.size() != map1.size())) {
        return IMAGE_RESULT_INVALID_PARAMETER;
    }

    cv::Mat dst;
    if (opts.borderValue != nullptr) {
        cv::Scalar scalar = CVScalarToNative(*opts.borderValue);
        cv::remap(src, dst, map1, map2, opts.interpolation, opts.borderType, scalar);
    } else {
        cv::remap(src, dst, map1, map2, opts.interpolation, opts.borderType);
    }
    return NativeToCVMat(dst, *opts.dst);
}

MIDK_EXPORT
int32_t OH_CV_GetRotationMatrix2D(CV_GetRotationMatrix2D_Opts opts)
{
    cv::Point2f center = CVPoint2fToNative(opts.center);
    if (opts.dst == nullptr) {
        return IMAGE_RESULT_INVALID_PARAMETER;
    }
    cv::Mat dst = cv::getRotationMatrix2D(center, opts.angle, opts.scale);
    return NativeToCVMat(dst, *opts.dst);
}

MIDK_EXPORT
int32_t OH_CV_GetPerspectiveTransform(CV_GetPerspectiveTransform_Opts opts)
{
    cv::Mat src1 = CVMatToNative(opts.src1);
    cv::Mat src2 = CVMatToNative(opts.src2);

    if (src1.checkVector(NUM_2, CV_32F) != NUM_4 ||
        src2.checkVector(NUM_2, CV_32F) != NUM_4 ||
        opts.dst == nullptr) {
        return IMAGE_RESULT_INVALID_PARAMETER;
    }
    cv::Mat dst = cv::getPerspectiveTransform(src1, src2, opts.solve);
    return NativeToCVMat(dst, *opts.dst);
}

MIDK_EXPORT
int32_t OH_CV_Sobel(CV_Sobel_Opts opts)
{
    cv::Mat src = CVMatToNative(opts.src);
    if (src.empty() || opts.dst == nullptr) {
        return IMAGE_RESULT_INVALID_PARAMETER;
    }

    cv::Mat dst;
    cv::Sobel(src, dst, opts.dDepth, opts.dx, opts.dy, opts.kSize,
        opts.scale, opts.delta, opts.borderType);
    return NativeToCVMat(dst, *opts.dst);
}

MIDK_EXPORT
int32_t OH_CV_Threshold(CV_Threshold_Opts opts)
{
    cv::Mat src = CVMatToNative(opts.src);
    if (src.empty() || opts.dst == nullptr) {
        return IMAGE_RESULT_INVALID_PARAMETER;
    }

    cv::Mat dst;
    cv::threshold(src, dst, opts.thresh, opts.maxval, opts.type);
    return NativeToCVMat(dst, *opts.dst);
}

MIDK_EXPORT
int32_t OH_CV_CvtColor(CV_CvtColor_Opts opts)
{
    cv::Mat src = CVMatToNative(opts.src);
    if (src.empty() || opts.dst == nullptr) {
        return IMAGE_RESULT_INVALID_PARAMETER;
    }

    cv::Mat dst;
    cv::cvtColor(src, dst, opts.code, opts.dstChannel);
    return NativeToCVMat(dst, *opts.dst);
}

MIDK_EXPORT
int32_t OH_CV_Blur(CV_Blur_Opts opts)
{
    cv::Mat src = CVMatToNative(opts.src);
    if (src.empty() || opts.dst == nullptr) {
        return IMAGE_RESULT_INVALID_PARAMETER;
    }
    cv::Size kSzie = CVSizeToNative(opts.kSize);
    cv::Point anchor = CVPointToNative(opts.anchor);

    cv::Mat dst;
    cv::blur(src, dst, kSzie, anchor, opts.borderType);
    return NativeToCVMat(dst, *opts.dst);
}

MIDK_EXPORT
int32_t OH_CV_FillPoly(CV_FillPoly_Opts opts)
{
    cv::Mat srcDst = CVMatToNative(opts.src);
    if (srcDst.empty() || opts.dst == nullptr) {
        return IMAGE_RESULT_INVALID_PARAMETER;
    }
    cv::Mat pts = CVMatToNative(opts.pts);
    if (pts.checkVector(NUM_2, CV_32S) < NUM_0) {
        return IMAGE_RESULT_INVALID_PARAMETER;
    }
    cv::Scalar color = CVScalarToNative(opts.color);
    if (opts.offset != nullptr) {
        cv::Point offset = CVPointToNative(*(opts.offset));
        cv::fillPoly(srcDst, pts, color, opts.lintType, opts.shift, offset);
    } else {
        cv::fillPoly(srcDst, pts, color, opts.lintType, opts.shift);
    }
    return NativeToCVMat(srcDst, *opts.dst);
}

MIDK_EXPORT
int32_t OH_CV_AdaptiveThreshold(CV_AdaptiveThreshold_Opts opts)
{
    cv::Mat src = CVMatToNative(opts.src);
    if (src.empty() || opts.dst == nullptr || src.type() != CV_8UC1 ||
        opts.blockSize <= NUM_1 || opts.blockSize % NUM_2 != NUM_1 ) {
        return IMAGE_RESULT_INVALID_PARAMETER;
    }
    cv::Mat dst;
    cv::adaptiveThreshold(src, dst, opts.maxValue,
        opts.adaptiveMethod, opts.thresholdType, opts.blockSize, opts.delta);
    return NativeToCVMat(dst, *opts.dst);
}

MIDK_EXPORT
int32_t OH_CV_FindContours(CV_FindContours_Opts opts)
{
    cv::Mat src = CVMatToNative(opts.src);
    if (src.empty() || opts.contours == nullptr ) {
        return IMAGE_RESULT_INVALID_PARAMETER;
    }

    std::vector<std::vector<cv::Point>> contours;

    if (opts.hierarchys != nullptr) {
        std::vector<cv::Vec4i> hierarchy;
        if (opts.offset != nullptr) {
            cv::Point offset = CVPointToNative(*(opts.offset));
            cv::findContours(src, contours, hierarchy, opts.mode, opts.method, offset);
        } else {
            cv::findContours(src, contours, hierarchy, opts.mode, opts.method);
        }
        return NativeToCVContours(contours, hierarchy, opts.contours, opts.hierarchys);
    } else {
        std::vector<cv::Vec4i> hierarchy;
        if (opts.offset != nullptr) {
            cv::Point offset = CVPointToNative(*(opts.offset));
            cv::findContours(src, contours, opts.mode, opts.method, offset);
        } else {
            cv::findContours(src, contours, opts.mode, opts.method);
        }
        return NativeToCVContours(contours, hierarchy, opts.contours);
    }
}

MIDK_EXPORT
int32_t OH_CV_BoxFilter(CV_BoxFilter_Opts opts)
{
    cv::Mat src = CVMatToNative(opts.src);
    if (src.empty() || opts.dst == nullptr ) {
        return IMAGE_RESULT_INVALID_PARAMETER;
    }

    cv::Size kSzie = CVSizeToNative(opts.kSize);
    cv::Point anchor = CVPointToNative(opts.anchor);
    cv::Mat dst;
    cv::boxFilter(src, dst, opts.dDepth, kSzie, anchor,
        opts.normalize == CV_BOOLEAN_VALUE_TRUE, opts.borderType);
    return NativeToCVMat(dst, *opts.dst);
}

MIDK_EXPORT
int32_t OH_CV_BorderInterpolate(CV_BorderInterpolate_Opts opts)
{
    if (opts.res == nullptr || opts.len <= NUM_0 ||
        (opts.borderType != CV_BORDER_TYPE_REPLICATE &&
            opts.borderType != CV_BORDER_TYPE_REFLECT &&
            opts.borderType != CV_BORDER_TYPE_REFLECT101 &&
            opts.borderType != CV_BORDER_TYPE_WRAP &&
            opts.borderType != CV_BORDER_TYPE_CONSTANT)) {
        return IMAGE_RESULT_INVALID_PARAMETER;
    }

    *(opts.res) = cv::borderInterpolate(opts.p, opts.len, opts.borderType);
    return IMAGE_RESULT_SUCCESS;
}

MIDK_EXPORT
int32_t OH_CV_CountNonZero(CV_CountNonZero_Opts opts)
{
    cv::Mat src = CVMatToNative(opts.src);
    if (src.empty() || CV_MAT_CN(src.type()) != NUM_1 || opts.res == nullptr ) {
        return IMAGE_RESULT_INVALID_PARAMETER;
    }
    *(opts.res) = cv::countNonZero(src);
    return IMAGE_RESULT_SUCCESS;
}

MIDK_EXPORT
int32_t OH_CV_CopyMakeBorder(CV_CopyMakeBorder_Opts opts)
{
    cv::Mat src = CVMatToNative(opts.src);
    if (src.empty() || src.dims > NUM_2 ||opts.dst == nullptr || 
        opts.position.top < NUM_0 || opts.position.left < NUM_0 ||
        opts.position.right < NUM_0 || opts.position.bottom < NUM_0) {
        return IMAGE_RESULT_INVALID_PARAMETER;
    }

    cv::Mat dst;
    if (opts.borderValue != nullptr) {
        cv::Scalar scalar = CVScalarToNative(*opts.borderValue);
        cv::copyMakeBorder(src, dst, opts.position.left, opts.position.top,
            opts.position.right, opts.position.bottom, opts.borderType, scalar);
    } else {
        cv::copyMakeBorder(src, dst, opts.position.left, opts.position.top,
            opts.position.right, opts.position.bottom, opts.borderType);
    }
    return NativeToCVMat(dst, *opts.dst);
}

MIDK_EXPORT
int32_t OH_CV_Split(CV_Split_Opts opts)
{
    cv::Mat src = CVMatToNative(opts.src);
    if (src.empty() || opts.dstSize == nullptr) {
        return IMAGE_RESULT_INVALID_PARAMETER;
    }

    std::vector<cv::Mat> dst;
    cv::split(src, dst);
    auto dstSize = dst.size();
    if (dstSize == NUM_0) {
        return IMAGE_RESULT_DATA_ABNORMAL;
    }
    auto result = std::make_unique<CV_Mat[]>(dstSize);
    for (size_t i = NUM_0; i < dstSize; i++) {
        auto ret = NativeToCVMat(dst[i], result[i]);
        if (ret != IMAGE_RESULT_SUCCESS) {
            return ret;
        }
    }
    *(opts.dst) = result.release();
    *(opts.dstSize) = dstSize;
    return IMAGE_RESULT_SUCCESS;
}

MIDK_EXPORT
int32_t OH_CV_Merge(CV_Merge_Opts opts)
{
    if (opts.src == nullptr || opts.srcSize == NUM_0 || opts.dst == nullptr) {
        return IMAGE_RESULT_INVALID_PARAMETER;
    }

    std::vector<cv::Mat> src;
    src.resize(opts.srcSize);
    for (size_t i = NUM_0; i < opts.srcSize; i++) {
        src[i] = CVMatToNative(opts.src[i]);
    }
    cv::Mat dst;
    cv::merge(src, dst);
    return NativeToCVMat(dst, *opts.dst);
}

MIDK_EXPORT
int32_t OH_CV_MinMaxLoc(CV_MinMaxLoc_Opts opts)
{
    cv::Mat src = CVMatToNative(opts.src);
    if (src.empty()) {
        return IMAGE_RESULT_INVALID_PARAMETER;
    }

    cv::Point maxIndex;
    cv::Point minIndex;
    if (opts.mask != nullptr) {
        cv::Mat mask = CVMatToNative(*opts.mask);
        cv::minMaxLoc(src, opts.minValue, opts.maxValue, &minIndex, &maxIndex, mask);
    } else {
        cv::minMaxLoc(src, opts.minValue, opts.maxValue, &minIndex, &maxIndex);
    }
    if (opts.minIndex != nullptr) {
        opts.minIndex->x = minIndex.x;
        opts.minIndex->y = minIndex.y;
    }
    if (opts.maxIndex != nullptr) {
        opts.maxIndex->x = maxIndex.x;
        opts.maxIndex->y = maxIndex.y;
    }
    return IMAGE_RESULT_SUCCESS;
}

MIDK_EXPORT
int32_t OH_CV_Normalize(CV_Normalize_Opts opts)
{
    cv::Mat src = CVMatToNative(opts.src);
    if (src.empty() || opts.dst == nullptr) {
        return IMAGE_RESULT_INVALID_PARAMETER;
    }
    if (opts.mask != nullptr) {
        cv::Mat mask = CVMatToNative(*opts.mask);
        cv::normalize(src, src, opts.alpha, opts.beta, opts.normType, opts.dType, mask);
    } else {
        cv::normalize(src, src, opts.alpha, opts.beta, opts.normType, opts.dType);
    }
    return NativeToCVMat(src, *opts.dst);
}

MIDK_EXPORT
int32_t OH_CV_MixChannels(CV_MixChannels_Opts opts)
{
    if (opts.src == nullptr || opts.dst == nullptr ||
        opts.srcSize == NUM_0 || opts.dstSize == NUM_0 ||
        opts.formTo == nullptr || opts.parisCount == NUM_0) {
        return IMAGE_RESULT_INVALID_PARAMETER;
    }
    std::vector<cv::Mat> src;
    src.resize(opts.srcSize);
    for (size_t i = NUM_0; i < opts.srcSize; i++) {
        src[i] = CVMatToNative(opts.src[i]);
    }
    std::vector<cv::Mat> dst;
    dst.resize(opts.dstSize);
    for (size_t j = NUM_0; j < opts.dstSize; j++) {
        auto dstItem = opts.dst[j];
        dst[j] = cv::Mat(dstItem.rows, dstItem.cols, dstItem.type);
    }
    cv::mixChannels(&src[NUM_0], opts.srcSize, &dst[NUM_0], opts.dstSize, opts.formTo, opts.parisCount);
    for (size_t k = NUM_0; k < opts.dstSize; k++) {
        auto res = NativeToCVMatDataOnly(dst[k], opts.dst[k]);
        if (res != IMAGE_RESULT_SUCCESS) {
            return res;
        }
    }
    return IMAGE_RESULT_SUCCESS;
}
#ifdef __cplusplus
};
#endif
