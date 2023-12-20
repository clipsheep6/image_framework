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

#include <gtest/gtest.h>

#include "common_utils.h"
#include "opencv_native.h"

using namespace testing::ext;
namespace OHOS {
namespace Media {
static constexpr int32_t M_SIZE_1 = 1;
static constexpr int32_t M_SIZE_2 = 2;
static constexpr int32_t M_SIZE_3 = 3;
static constexpr int32_t M_SIZE_4 = 4;
static constexpr int32_t M_SIZE_5 = 5;
static constexpr int32_t M_SIZE_7 = 7;
static constexpr int32_t M_SIZE_8 = 8;
static constexpr int32_t M_SIZE_9 = 9;
static constexpr int32_t M_SIZE_10 = 10;
static constexpr int32_t M_SIZE_11 = 11;
static constexpr int32_t M_SIZE_12 = 12;
static constexpr int32_t M_DATA_SIZE_8 = 8;
static constexpr int32_t M_DATA_SIZE_9 = 9;
static constexpr int32_t M_DATA_SIZE_10 = 10;
static constexpr int32_t M_DATA_SIZE_49 = 49;
static constexpr int32_t M_DATA_SIZE_147 = 147;
static constexpr int32_t M_DATA_SIZE_192 = 192;

class OpenCVNativeTest : public testing::Test {
public:
    OpenCVNativeTest() {}
    ~OpenCVNativeTest() {}
};

/**
 * @tc.name: OpenCVNativeTest001
 * @tc.desc: OH_CV_Erode
 * @tc.type: FUNC
 */
HWTEST_F(OpenCVNativeTest, OpenCVNativeTest001, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "OpenCVNativeTest: OpenCVNativeTest001 start";
    uint8_t srcData[M_DATA_SIZE_49] = {
         0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
         0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
         0xff, 0xff, 0,    0,    0,    0xff, 0xff,
         0xff, 0xff, 0,    0,    0,    0xff, 0xff,
         0xff, 0xff, 0,    0,    0,    0xff, 0xff,
         0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
         0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    };

    uint8_t kernelData[M_DATA_SIZE_9] = {
        0, 1, 0,
        1, 1, 1,
        0, 1, 0,
    };

    CV_Mat dst;
    CV_Morph_Opts opts = {
        .src = {M_SIZE_7, M_SIZE_7, CV_MAT_TYPE_8UC1, &srcData, sizeof(srcData)},
        .dst = &dst,
        .kernel = {M_SIZE_3, M_SIZE_3, CV_MAT_TYPE_8UC1, &kernelData, sizeof(kernelData)},
        .anchor = {0, 0},
        .iterations = 1
    };

    auto res = OH_CV_Erode(opts);
    ASSERT_EQ(res, IMAGE_RESULT_SUCCESS);
    ASSERT_EQ(dst.type, CV_MAT_TYPE_8UC1);
    ASSERT_EQ(dst.rows, M_SIZE_7);
    ASSERT_EQ(dst.cols, M_SIZE_7);
    ASSERT_NE(dst.data, nullptr);
    ASSERT_NE(dst.dataSize, 0);
    GTEST_LOG_(INFO) << "OpenCVNativeTest: OpenCVNativeTest001 end";
}

/**
 * @tc.name: OpenCVNativeTest002
 * @tc.desc: OH_CV_Dilate
 * @tc.type: FUNC
 */
HWTEST_F(OpenCVNativeTest, OpenCVNativeTest002, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "OpenCVNativeTest: OpenCVNativeTest002 start";
    uint8_t srcData[M_DATA_SIZE_49] = {
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0,    0,    0,    0,    0,    0xff,
        0xff, 0,    0,    0,    0,    0,    0xff,
        0xff, 0,    0,    0,    0,    0,    0xff,
        0xff, 0,    0,    0,    0,    0,    0xff,
        0xff, 0,    0,    0,    0,    0,    0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    };

    uint8_t kernelData[M_DATA_SIZE_9] = {
        1, 1, 1,
        1, 1, 1,
        1, 1, 1,
    };

    CV_Mat dst;
    CV_Morph_Opts opts = {
        .src = {M_SIZE_7, M_SIZE_7, CV_MAT_TYPE_8UC1, &srcData, sizeof(srcData)},
        .dst = &dst,
        .kernel = {M_SIZE_3, M_SIZE_3, CV_MAT_TYPE_8UC1, &kernelData, sizeof(kernelData)},
        .anchor = {0, 0},
        .iterations = 1
    };

    auto res = OH_CV_Dilate(opts);
    ASSERT_EQ(res, IMAGE_RESULT_SUCCESS);
    ASSERT_EQ(dst.type, CV_MAT_TYPE_8UC1);
    ASSERT_EQ(dst.rows, M_SIZE_7);
    ASSERT_EQ(dst.cols, M_SIZE_7);
    ASSERT_NE(dst.data, nullptr);
    ASSERT_NE(dst.dataSize, 0);

    GTEST_LOG_(INFO) << "OpenCVNativeTest: OpenCVNativeTest002 end";
}

/**
 * @tc.name: OpenCVNativeTest003
 * @tc.desc: OH_CV_MorphologyEx
 * @tc.type: FUNC
 */
HWTEST_F(OpenCVNativeTest, OpenCVNativeTest003, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "OpenCVNativeTest: OpenCVNativeTest003 start";
    uint8_t srcData[M_DATA_SIZE_49] = {
         0xff, 0xff, 0xff, 0,    0,    0,    0,
         0xff, 0xff, 0xff, 0,    0,    0,    0,
         0xff, 0xff, 0xff, 0,    0,    0,    0,
         0,    0,    0,    0xff, 0xff, 0xff, 0,
         0,    0,    0,    0xff, 0xff, 0xff, 0,
         0,    0,    0,    0xff, 0xff, 0xff, 0,
         0,    0,    0xff, 0xff, 0xff, 0,    0,
    };

    uint8_t kernelData[M_DATA_SIZE_9] = {
        0, 1, 0,
        1, 1, 1,
        0, 1, 0,
    };

    CV_Mat dst;
    CV_Morph_Opts opts = {
        .src = {M_SIZE_7, M_SIZE_7, CV_MAT_TYPE_8UC1, &srcData, sizeof(srcData)},
        .dst = &dst,
        .kernel = {M_SIZE_3, M_SIZE_3, CV_MAT_TYPE_8UC1, &kernelData, sizeof(kernelData)},
        .anchor = {0, 0},
        .iterations = 1,
        .morphType = CV_MORPH_TYPE_OPEN
    };

    auto res = OH_CV_MorphologyEx(opts);
    ASSERT_EQ(res, IMAGE_RESULT_SUCCESS);
    ASSERT_EQ(dst.type, CV_MAT_TYPE_8UC1);
    ASSERT_EQ(dst.rows, M_SIZE_7);
    ASSERT_EQ(dst.cols, M_SIZE_7);
    ASSERT_NE(dst.data, nullptr);
    ASSERT_NE(dst.dataSize, 0);
    GTEST_LOG_(INFO) << "OpenCVNativeTest: OpenCVNativeTest003 end";
}

/**
 * @tc.name: OpenCVNativeTest004
 * @tc.desc: OH_CV_Resize
 * @tc.type: FUNC
 */
HWTEST_F(OpenCVNativeTest, OpenCVNativeTest004, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "OpenCVNativeTest: OpenCVNativeTest004 start";
    uint8_t srcData[M_DATA_SIZE_49] = {0};
    for (uint8_t i = 0; i < M_DATA_SIZE_49; i++) {
        srcData[i] = i;
    }

    CV_Mat dst;
    CV_Resize_Opts opts = {
        .src = {M_SIZE_7, M_SIZE_7, CV_MAT_TYPE_8UC1, &srcData, sizeof(srcData)},
        .dst = &dst,
        .dstSize = {M_SIZE_9, M_SIZE_10}
    };

    auto res = OH_CV_Resize(opts);
    ASSERT_EQ(res, IMAGE_RESULT_SUCCESS);
    ASSERT_EQ(dst.type, CV_MAT_TYPE_8UC1);
    ASSERT_EQ(dst.rows, M_SIZE_10);
    ASSERT_EQ(dst.cols, M_SIZE_9);
    ASSERT_NE(dst.data, nullptr);
    ASSERT_NE(dst.dataSize, 0);
    GTEST_LOG_(INFO) << "OpenCVNativeTest: OpenCVNativeTest004 end";
}

/**
 * @tc.name: OpenCVNativeTest005
 * @tc.desc: OH_CV_WarpAffine
 * @tc.type: FUNC
 */
HWTEST_F(OpenCVNativeTest, OpenCVNativeTest005, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "OpenCVNativeTest: OpenCVNativeTest005 start";
    static constexpr float M_CENTER = 3.5f;
    static constexpr float ANGLE = 90.0f;
    static constexpr float SCALE = 1.0f;
    CV_Mat rotationMatrix;
    CV_GetRotationMatrix2D_Opts getRotationMatrix2DOpts = {
        .center = {M_CENTER, M_CENTER},
        .dst = &rotationMatrix,
        .angle = ANGLE,
        .scale = SCALE
    };
    auto resGetRotationMatrix2D = OH_CV_GetRotationMatrix2D(getRotationMatrix2DOpts);
    ASSERT_EQ(resGetRotationMatrix2D, IMAGE_RESULT_SUCCESS);
    ASSERT_EQ(rotationMatrix.type, CV_MAT_TYPE_64FC1);
    ASSERT_EQ(rotationMatrix.rows, M_SIZE_2);
    ASSERT_EQ(rotationMatrix.cols, M_SIZE_3);
    ASSERT_NE(rotationMatrix.data, nullptr);
    ASSERT_NE(rotationMatrix.dataSize, 0);
    uint8_t srcData[M_DATA_SIZE_49] = {0};
    for (uint8_t i = 0; i < M_DATA_SIZE_49; i++) {
        srcData[i] = i;
    }

    CV_Mat dst;
    CV_Warp_Opts opts = {
        .src = {M_SIZE_7, M_SIZE_7, CV_MAT_TYPE_8UC1, &srcData, sizeof(srcData)},
        .dst = &dst,
        .m = rotationMatrix,
        .dstSize = {M_SIZE_7, M_SIZE_7}
    };

    auto res = OH_CV_WarpAffine(opts);
    ASSERT_EQ(res, IMAGE_RESULT_SUCCESS);
    ASSERT_EQ(dst.type, CV_MAT_TYPE_8UC1);
    ASSERT_EQ(dst.rows, M_SIZE_7);
    ASSERT_EQ(dst.cols, M_SIZE_7);
    ASSERT_NE(dst.data, nullptr);
    ASSERT_NE(dst.dataSize, 0);
    GTEST_LOG_(INFO) << "OpenCVNativeTest: OpenCVNativeTest005 end";
}

/**
 * @tc.name: OpenCVNativeTest006
 * @tc.desc: OH_CV_WarpPerspective
 * @tc.type: FUNC
 */
HWTEST_F(OpenCVNativeTest, OpenCVNativeTest006, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "OpenCVNativeTest: OpenCVNativeTest006 start";
    float perspectiveData1[M_DATA_SIZE_8] = {
        0.0f, 0.0f,
        6.0f, 0.0f,
        0.0f, 6.0f,
        6.0f, 6.0f
    };
    float perspectiveData2[M_DATA_SIZE_8] = {
        1.0f, 1.0f,
        5.0f, 1.0f,
        0.0f, 6.0f,
        6.0f, 6.0f
    };
    CV_Mat perspectiveTransformMatrix;
    CV_GetPerspectiveTransform_Opts getPerspectiveTransformOpts = {
        .src1 = {M_SIZE_4, M_SIZE_1, CV_MAT_TYPE_32FC2, &perspectiveData1, sizeof(perspectiveData1)},
        .src2 = {M_SIZE_4, M_SIZE_1, CV_MAT_TYPE_32FC2, &perspectiveData2, sizeof(perspectiveData2)},
        .dst = &perspectiveTransformMatrix
    };
    auto resGetPerspectiveTransform = OH_CV_GetPerspectiveTransform(getPerspectiveTransformOpts);
    ASSERT_EQ(resGetPerspectiveTransform, IMAGE_RESULT_SUCCESS);
    ASSERT_EQ(perspectiveTransformMatrix.type, CV_MAT_TYPE_64FC1);
    ASSERT_EQ(perspectiveTransformMatrix.rows, M_SIZE_3);
    ASSERT_EQ(perspectiveTransformMatrix.cols, M_SIZE_3);
    ASSERT_NE(perspectiveTransformMatrix.data, nullptr);
    ASSERT_NE(perspectiveTransformMatrix.dataSize, 0);
    uint8_t srcData[M_DATA_SIZE_49] = {0};
    for (uint8_t i = 0; i < M_DATA_SIZE_49; i++) {
        srcData[i] = i;
    }

    CV_Mat dst;
    CV_Warp_Opts opts = {
        .src = {M_SIZE_7, M_SIZE_7, CV_MAT_TYPE_8UC1, &srcData, sizeof(srcData)},
        .dst = &dst,
        .m = perspectiveTransformMatrix,
        .dstSize = {M_SIZE_7, M_SIZE_7}
    };

    auto res = OH_CV_WarpPerspective(opts);
    ASSERT_EQ(res, IMAGE_RESULT_SUCCESS);
    ASSERT_EQ(dst.type, CV_MAT_TYPE_8UC1);
    ASSERT_EQ(dst.rows, M_SIZE_7);
    ASSERT_EQ(dst.cols, M_SIZE_7);
    ASSERT_NE(dst.data, nullptr);
    ASSERT_NE(dst.dataSize, 0);
    GTEST_LOG_(INFO) << "OpenCVNativeTest: OpenCVNativeTest006 end";
}

/**
 * @tc.name: OpenCVNativeTest007
 * @tc.desc: OH_CV_Remap
 * @tc.type: FUNC
 */
HWTEST_F(OpenCVNativeTest, OpenCVNativeTest007, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "OpenCVNativeTest: OpenCVNativeTest007 start";
    float map1[M_DATA_SIZE_49] = {
        0.0f, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f,
        0.0f, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f,
        0.0f, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f,
        0.0f, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f,
        0.0f, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f,
        0.0f, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f,
        0.0f, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f
    };
    float map2[M_DATA_SIZE_49] = {
        6.0f, 6.0f, 6.0f, 6.0f, 6.0f, 6.0f, 6.0f,
        5.0f, 5.0f, 5.0f, 5.0f, 5.0f, 5.0f, 5.0f,
        4.0f, 4.0f, 4.0f, 4.0f, 4.0f, 4.0f, 4.0f,
        3.0f, 3.0f, 3.0f, 3.0f, 3.0f, 3.0f, 3.0f,
        2.0f, 2.0f, 2.0f, 2.0f, 2.0f, 2.0f, 2.0f,
        1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
        0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f
    };
    uint8_t srcData[M_DATA_SIZE_49] = {0};
    for (uint8_t i = 0; i < M_DATA_SIZE_49; i++) {
        srcData[i] = i;
    }

    CV_Mat dst;
    CV_Remap_Opts opts = {
        .src = {M_SIZE_7, M_SIZE_7, CV_MAT_TYPE_8UC1, &srcData, sizeof(srcData)},
        .dst = &dst,
        .map1 = {M_SIZE_7, M_SIZE_7, CV_MAT_TYPE_32FC1, &map1, sizeof(map1)},
        .map2 = {M_SIZE_7, M_SIZE_7, CV_MAT_TYPE_32FC1, &map2, sizeof(map2)}
    };

    auto res = OH_CV_Remap(opts);
    ASSERT_EQ(res, IMAGE_RESULT_SUCCESS);
    ASSERT_EQ(dst.type, CV_MAT_TYPE_8UC1);
    ASSERT_EQ(dst.rows, M_SIZE_7);
    ASSERT_EQ(dst.cols, M_SIZE_7);
    ASSERT_NE(dst.data, nullptr);
    ASSERT_NE(dst.dataSize, 0);
    GTEST_LOG_(INFO) << "OpenCVNativeTest: OpenCVNativeTest007 end";
}

/**
 * @tc.name: OpenCVNativeTest008
 * @tc.desc: OH_CV_GetRotationMatrix2D
 * @tc.type: FUNC
 */
HWTEST_F(OpenCVNativeTest, OpenCVNativeTest008, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "OpenCVNativeTest: OpenCVNativeTest008 start";
    static constexpr float M_CENTER = 3.5f;
    static constexpr float ANGLE = 45.0f;
    static constexpr float SCALE = 1.0f;
    CV_Mat dst;
    CV_GetRotationMatrix2D_Opts getRotationMatrix2DOpts = {
        .center = {M_CENTER, M_CENTER},
        .dst = &dst,
        .angle = ANGLE,
        .scale = SCALE
    };
    auto res = OH_CV_GetRotationMatrix2D(getRotationMatrix2DOpts);
    GTEST_LOG_(INFO) << "OpenCVNativeTest: OpenCVNativeTest008 " << dst.cols << " " << dst.rows << " " << dst.type << " " ;
    ASSERT_EQ(res, IMAGE_RESULT_SUCCESS);
    ASSERT_NE(dst.data, nullptr);
    ASSERT_NE(dst.dataSize, 0);
    GTEST_LOG_(INFO) << "OpenCVNativeTest: OpenCVNativeTest008 end";
}

/**
 * @tc.name: OpenCVNativeTest009
 * @tc.desc: OH_CV_GetPerspectiveTransform
 * @tc.type: FUNC
 */
HWTEST_F(OpenCVNativeTest, OpenCVNativeTest009, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "OpenCVNativeTest: OpenCVNativeTest009 start";
    float data1[M_DATA_SIZE_8] = {
        0.0f, 0.0f,
        6.0f, 0.0f,
        0.0f, 6.0f,
        6.0f, 6.0f
    };
    float data2[M_DATA_SIZE_8] = {
        1.0f, 1.0f,
        5.0f, 1.0f,
        0.0f, 6.0f,
        6.0f, 6.0f
    };
    CV_Mat dst;
    CV_GetPerspectiveTransform_Opts getPerspectiveTransformOpts = {
        .src1 = {M_SIZE_4, M_SIZE_1, CV_MAT_TYPE_32FC2, &data1, sizeof(data1)},
        .src2 = {M_SIZE_4, M_SIZE_1, CV_MAT_TYPE_32FC2, &data2, sizeof(data2)},
        .dst = &dst
    };
    auto res = OH_CV_GetPerspectiveTransform(getPerspectiveTransformOpts);
    GTEST_LOG_(INFO) << "OpenCVNativeTest: OpenCVNativeTest009 " << dst.cols << " " << dst.rows << " " << dst.type << " " ;
    ASSERT_EQ(res, IMAGE_RESULT_SUCCESS);
    ASSERT_NE(dst.data, nullptr);
    ASSERT_NE(dst.dataSize, 0);
    GTEST_LOG_(INFO) << "OpenCVNativeTest: OpenCVNativeTest009 end";
}

/**
 * @tc.name: OpenCVNativeTest010
 * @tc.desc: OH_CV_Sobel
 * @tc.type: FUNC
 */
HWTEST_F(OpenCVNativeTest, OpenCVNativeTest010, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "OpenCVNativeTest: OpenCVNativeTest010 start";
    uint8_t srcData[M_DATA_SIZE_49] = {
         0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
         0xff, 0xff, 0,    0,    0xff, 0xff, 0xff,
         0xff, 0xff, 0,    0,    0,    0xff, 0xff,
         0xff, 0xff, 0,    0,    0,    0xff, 0xff,
         0xff, 0xff, 0,    0,    0,    0xff, 0xff,
         0xff, 0xff, 0,    0,    0xff, 0xff, 0xff,
         0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    };

    CV_Mat dst;
    CV_Sobel_Opts opts = {
        .src = {M_SIZE_7, M_SIZE_7, CV_MAT_TYPE_8UC1, &srcData, sizeof(srcData)},
        .dst = &dst,
        .dDepth = -1,
        .dx = 1,
        .dy = 0,
        .kSize = 3
    };

    auto res = OH_CV_Sobel(opts);
    ASSERT_EQ(res, IMAGE_RESULT_SUCCESS);
    ASSERT_EQ(dst.type, CV_MAT_TYPE_8UC1);
    ASSERT_EQ(dst.rows, M_SIZE_7);
    ASSERT_EQ(dst.cols, M_SIZE_7);
    ASSERT_NE(dst.data, nullptr);
    ASSERT_NE(dst.dataSize, 0);
    GTEST_LOG_(INFO) << "OpenCVNativeTest: OpenCVNativeTest010 end";
}

/**
 * @tc.name: OpenCVNativeTest011
 * @tc.desc: OH_CV_Threshold
 * @tc.type: FUNC
 */
HWTEST_F(OpenCVNativeTest, OpenCVNativeTest011, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "OpenCVNativeTest: OpenCVNativeTest011 start";
    uint8_t srcData[M_DATA_SIZE_49] = {0};
    for (uint8_t i = 0; i < M_DATA_SIZE_49; i++) {
        srcData[i] = i;
    }

    CV_Mat dst;
    CV_Threshold_Opts opts = {
        .src = {M_SIZE_7, M_SIZE_7, CV_MAT_TYPE_8UC1, &srcData, sizeof(srcData)},
        .dst = &dst,
        .thresh = 23.5,
        .maxval = 255
    };

    auto res = OH_CV_Threshold(opts);
    ASSERT_EQ(res, IMAGE_RESULT_SUCCESS);
    ASSERT_EQ(dst.type, CV_MAT_TYPE_8UC1);
    ASSERT_EQ(dst.rows, M_SIZE_7);
    ASSERT_EQ(dst.cols, M_SIZE_7);
    ASSERT_NE(dst.data, nullptr);
    ASSERT_NE(dst.dataSize, 0);
    GTEST_LOG_(INFO) << "OpenCVNativeTest: OpenCVNativeTest011 end";
}

/**
 * @tc.name: OpenCVNativeTest012
 * @tc.desc: OH_CV_CvtColor
 * @tc.type: FUNC
 */
HWTEST_F(OpenCVNativeTest, OpenCVNativeTest012, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "OpenCVNativeTest: OpenCVNativeTest012 start";
    uint8_t srcData[M_DATA_SIZE_192] = {
        0xff, 0x80, 0, 0xff, 0x80, 0, 0xff, 0x80, 0, 0xff, 0x80, 0,
        0xff, 0x80, 0, 0xff, 0x80, 0, 0xff, 0x80, 0, 0xff, 0x80, 0,
        0xff, 0x80, 0, 0xff, 0x80, 0, 0xff, 0x80, 0, 0xff, 0x80, 0,
        0xff, 0x80, 0, 0xff, 0x80, 0, 0xff, 0x80, 0, 0xff, 0x80, 0,
        0xff, 0x80, 0, 0xff, 0x80, 0, 0xff, 0x80, 0, 0xff, 0x80, 0,
        0xff, 0x80, 0, 0xff, 0x80, 0, 0xff, 0x80, 0, 0xff, 0x80, 0,
        0xff, 0x80, 0, 0xff, 0x80, 0, 0xff, 0x80, 0, 0xff, 0x80, 0,
        0xff, 0x80, 0, 0xff, 0x80, 0, 0xff, 0x80, 0, 0xff, 0x80, 0,
        0xff, 0x80, 0, 0xff, 0x80, 0, 0xff, 0x80, 0, 0xff, 0x80, 0,
        0xff, 0x80, 0, 0xff, 0x80, 0, 0xff, 0x80, 0, 0xff, 0x80, 0,
        0xff, 0x80, 0, 0xff, 0x80, 0, 0xff, 0x80, 0, 0xff, 0x80, 0,
        0xff, 0x80, 0, 0xff, 0x80, 0, 0xff, 0x80, 0, 0xff, 0x80, 0,
        0xff, 0x80, 0, 0xff, 0x80, 0, 0xff, 0x80, 0, 0xff, 0x80, 0,
        0xff, 0x80, 0, 0xff, 0x80, 0, 0xff, 0x80, 0, 0xff, 0x80, 0,
        0xff, 0x80, 0, 0xff, 0x80, 0, 0xff, 0x80, 0, 0xff, 0x80, 0,
        0xff, 0x80, 0, 0xff, 0x80, 0, 0xff, 0x80, 0, 0xff, 0x80, 0,
    };

    CV_Mat dst;
    CV_CvtColor_Opts opts = {
        .src = {M_SIZE_8, M_SIZE_8, CV_MAT_TYPE_8UC3, &srcData, sizeof(srcData)},
        .dst = &dst,
        .code = CV_COLOR_CONVERSION_CODE_RGB2YUV_I420
    };

    auto res = OH_CV_CvtColor(opts);
    ASSERT_EQ(res, IMAGE_RESULT_SUCCESS);
    ASSERT_EQ(dst.type, CV_MAT_TYPE_8UC1);
    ASSERT_EQ(dst.rows, M_SIZE_12);
    ASSERT_EQ(dst.cols, M_SIZE_8);
    ASSERT_NE(dst.data, nullptr);
    ASSERT_NE(dst.dataSize, 0);
    GTEST_LOG_(INFO) << "OpenCVNativeTest: OpenCVNativeTest012 end";
}

/**
 * @tc.name: OpenCVNativeTest013
 * @tc.desc: OH_CV_Blur
 * @tc.type: FUNC
 */
HWTEST_F(OpenCVNativeTest, OpenCVNativeTest013, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "OpenCVNativeTest: OpenCVNativeTest013 start";
    uint8_t srcData[M_DATA_SIZE_49] = {0};
    for (uint8_t i = 0; i < M_DATA_SIZE_49; i++) {
        srcData[i] = i;
    }

    CV_Mat dst;
    CV_Blur_Opts opts = {
        .src = {M_SIZE_7, M_SIZE_7, CV_MAT_TYPE_8UC1, &srcData, sizeof(srcData)},
        .dst = &dst,
        .kSize = {5, 5}
    };

    auto res = OH_CV_Blur(opts);
    ASSERT_EQ(res, IMAGE_RESULT_SUCCESS);
    ASSERT_EQ(dst.type, CV_MAT_TYPE_8UC1);
    ASSERT_EQ(dst.rows, M_SIZE_7);
    ASSERT_EQ(dst.cols, M_SIZE_7);
    ASSERT_NE(dst.data, nullptr);
    ASSERT_NE(dst.dataSize, 0);
    GTEST_LOG_(INFO) << "OpenCVNativeTest: OpenCVNativeTest013 end";
}

/**
 * @tc.name: OpenCVNativeTest014
 * @tc.desc: OH_CV_FillPoly
 * @tc.type: FUNC
 */
HWTEST_F(OpenCVNativeTest, OpenCVNativeTest014, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "OpenCVNativeTest: OpenCVNativeTest014 start";
    uint8_t srcData[M_DATA_SIZE_147] = {0};
    for (uint8_t i = 0; i < M_DATA_SIZE_147; i++) {
        srcData[i] = 0;
    }
    int32_t ptsData[M_DATA_SIZE_10] = {
        0, 3,
        3, 6,
        6, 4,
        6, 2,
        3, 0
    };

    CV_Mat dst;
    CV_FillPoly_Opts opts = {
        .src = {M_SIZE_7, M_SIZE_7, CV_MAT_TYPE_8UC3, &srcData, sizeof(srcData)},
        .pts = {M_SIZE_5, M_SIZE_2, CV_MAT_TYPE_32SC1, &ptsData, sizeof(ptsData)},
        .dst = &dst,
        .color = {255, 128, 64}
    };

    auto res = OH_CV_FillPoly(opts);
    ASSERT_EQ(res, IMAGE_RESULT_SUCCESS);
    ASSERT_EQ(dst.type, CV_MAT_TYPE_8UC3);
    ASSERT_EQ(dst.rows, M_SIZE_7);
    ASSERT_EQ(dst.cols, M_SIZE_7);
    ASSERT_NE(dst.data, nullptr);
    ASSERT_NE(dst.dataSize, 0);
    GTEST_LOG_(INFO) << "OpenCVNativeTest: OpenCVNativeTest014 end";
}

/**
 * @tc.name: OpenCVNativeTest015
 * @tc.desc: OH_CV_AdaptiveThreshold
 * @tc.type: FUNC
 */
HWTEST_F(OpenCVNativeTest, OpenCVNativeTest015, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "OpenCVNativeTest: OpenCVNativeTest015 start";
    uint8_t srcData[M_DATA_SIZE_49] = {0};
    for (uint8_t i = 0; i < M_DATA_SIZE_49; i++) {
        srcData[i] = i;
    }

    CV_Mat dst;
    CV_AdaptiveThreshold_Opts opts = {
        .src = {M_SIZE_7, M_SIZE_7, CV_MAT_TYPE_8UC1, &srcData, sizeof(srcData)},
        .dst = &dst,
        .maxValue = 255,
        .adaptiveMethod = CV_ADAPTIVE_THRESHOLD_TYPE_MEAN_C,
        .thresholdType = CV_THRESHOLD_TYPE_BINARY,
        .blockSize = 3,
        .delta = 1.0
    };

    auto res = OH_CV_AdaptiveThreshold(opts);
    ASSERT_EQ(res, IMAGE_RESULT_SUCCESS);
    ASSERT_EQ(dst.type, CV_MAT_TYPE_8UC1);
    ASSERT_EQ(dst.rows, M_SIZE_7);
    ASSERT_EQ(dst.cols, M_SIZE_7);
    ASSERT_NE(dst.data, nullptr);
    ASSERT_NE(dst.dataSize, 0);
    GTEST_LOG_(INFO) << "OpenCVNativeTest: OpenCVNativeTest015 end";
}

/**
 * @tc.name: OpenCVNativeTest016
 * @tc.desc: OH_CV_FindContours
 * @tc.type: FUNC
 */
HWTEST_F(OpenCVNativeTest, OpenCVNativeTest016, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "OpenCVNativeTest: OpenCVNativeTest016 start";
    uint8_t srcData[M_DATA_SIZE_49] = {
         0, 0,    0,    0,    0,    0,    0,
         0, 0xFF, 0xff, 0xff, 0xff, 0xff, 0,
         0, 0xff, 0xff, 0xff, 0xff, 0xff, 0,
         0, 0xff, 0xff, 0xff, 0xff, 0xff, 0,
         0, 0xff, 0xff, 0xff, 0xff, 0xff, 0,
         0, 0xff, 0xff, 0xff, 0xff, 0xff, 0,
         0, 0,    0,    0,    0,    0,    0,
    };

    CV_Contours conts;
    CV_Hierarchys hiers;
    CV_FindContours_Opts opts = {
        .src = {M_SIZE_7, M_SIZE_7, CV_MAT_TYPE_8UC1, &srcData, sizeof(srcData)},
        .contours = &conts,
        .hierarchys = &hiers,
        .mode = CV_RETRIEVAL_MODE_LIST,
        .method = CV_APPROXIMATION_METHOD_NONE
    };

    auto res = OH_CV_FindContours(opts);
    ASSERT_EQ(res, IMAGE_RESULT_SUCCESS);
    ASSERT_NE(conts.contours, nullptr);
    ASSERT_NE(conts.size, 0);
    ASSERT_NE(hiers.hierarchys, nullptr);
    ASSERT_NE(hiers.size, 0);
    GTEST_LOG_(INFO) << "OpenCVNativeTest: OpenCVNativeTest016 end";
}

/**
 * @tc.name: OpenCVNativeTest017
 * @tc.desc: OH_CV_BoxFilter
 * @tc.type: FUNC
 */
HWTEST_F(OpenCVNativeTest, OpenCVNativeTest017, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "OpenCVNativeTest: OpenCVNativeTest017 start";
    uint8_t srcData[M_DATA_SIZE_49] = {0};
    for (uint8_t i = 0; i < M_DATA_SIZE_49; i++) {
        srcData[i] = i;
    }

    CV_Mat dst;
    CV_BoxFilter_Opts opts = {
        .src = {M_SIZE_7, M_SIZE_7, CV_MAT_TYPE_8UC1, &srcData, sizeof(srcData)},
        .dst = &dst,
        .dDepth = -1,
        .kSize = {3, 3},
        .anchor = {-1, -1}
    };

    auto res = OH_CV_BoxFilter(opts);
    ASSERT_EQ(res, IMAGE_RESULT_SUCCESS);
    ASSERT_EQ(dst.type, CV_MAT_TYPE_8UC1);
    ASSERT_EQ(dst.rows, M_SIZE_7);
    ASSERT_EQ(dst.cols, M_SIZE_7);
    ASSERT_NE(dst.data, nullptr);
    ASSERT_NE(dst.dataSize, 0);
    GTEST_LOG_(INFO) << "OpenCVNativeTest: OpenCVNativeTest017 end";
}

/**
 * @tc.name: OpenCVNativeTest018
 * @tc.desc: OH_CV_BorderInterpolate
 * @tc.type: FUNC
 */
HWTEST_F(OpenCVNativeTest, OpenCVNativeTest018, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "OpenCVNativeTest: OpenCVNativeTest018 start";

    int borderInterpolate = 0;
    CV_BorderInterpolate_Opts opts = {
        .p = 100,
        .len = M_SIZE_7,
        .res = &borderInterpolate
    };

    auto result = OH_CV_BorderInterpolate(opts);
    ASSERT_EQ(result, IMAGE_RESULT_SUCCESS);
    ASSERT_NE(borderInterpolate, 0);
    GTEST_LOG_(INFO) << "OpenCVNativeTest: OpenCVNativeTest018 end";
}

/**
 * @tc.name: OpenCVNativeTest019
 * @tc.desc: OH_CV_CountNonZero
 * @tc.type: FUNC
 */
HWTEST_F(OpenCVNativeTest, OpenCVNativeTest019, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "OpenCVNativeTest: OpenCVNativeTest019 start";
    uint8_t srcData[M_DATA_SIZE_49] = {0};
    for (uint8_t i = 0; i < M_DATA_SIZE_49; i++) {
        srcData[i] = i;
    }

    int nonZeroCount = 0;
    CV_CountNonZero_Opts opts = {
        .src = {M_SIZE_7, M_SIZE_7, CV_MAT_TYPE_8UC1, &srcData, sizeof(srcData)},
        .res = &nonZeroCount
    };

    auto res = OH_CV_CountNonZero(opts);
    ASSERT_EQ(res, IMAGE_RESULT_SUCCESS);
    ASSERT_NE(nonZeroCount, 0);
    GTEST_LOG_(INFO) << "OpenCVNativeTest: OpenCVNativeTest019 end";
}

/**
 * @tc.name: OpenCVNativeTest020
 * @tc.desc: OH_CV_CopyMakeBorder
 * @tc.type: FUNC
 */
HWTEST_F(OpenCVNativeTest, OpenCVNativeTest020, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "OpenCVNativeTest: OpenCVNativeTest020 start";
    uint8_t srcData[M_DATA_SIZE_49] = {0};
    for (uint8_t i = 0; i < M_DATA_SIZE_49; i++) {
        srcData[i] = i;
    }

    CV_Mat dst;
    CV_CopyMakeBorder_Opts opts = {
        .src = {M_SIZE_7, M_SIZE_7, CV_MAT_TYPE_8UC1, &srcData, sizeof(srcData)},
        .dst = &dst,
        .position = {3, 2, 3, 1}
    };

    auto res = OH_CV_CopyMakeBorder(opts);
    ASSERT_EQ(res, IMAGE_RESULT_SUCCESS);
    ASSERT_EQ(dst.type, CV_MAT_TYPE_8UC1);
    ASSERT_EQ(dst.rows, M_SIZE_12);
    ASSERT_EQ(dst.cols, M_SIZE_11);
    ASSERT_NE(dst.data, nullptr);
    ASSERT_NE(dst.dataSize, 0);
    GTEST_LOG_(INFO) << "OpenCVNativeTest: OpenCVNativeTest020 end";
}

/**
 * @tc.name: OpenCVNativeTest021
 * @tc.desc: OH_CV_Split
 * @tc.type: FUNC
 */
HWTEST_F(OpenCVNativeTest, OpenCVNativeTest021, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "OpenCVNativeTest: OpenCVNativeTest021 start";
    uint8_t srcData[M_DATA_SIZE_147] = {0};
    for (uint8_t i = 0; i < M_DATA_SIZE_49; i++) {
        srcData[i] = i % 3;
    }

    CV_Mat* dst = nullptr;
    unsigned int dstSize = 3;
    CV_Split_Opts opts = {
        .src = {M_SIZE_7, M_SIZE_7, CV_MAT_TYPE_8UC3, &srcData, sizeof(srcData)},
        .dst = &dst,
        .dstSize = &dstSize
    };

    auto res = OH_CV_Split(opts);
    ASSERT_EQ(res, IMAGE_RESULT_SUCCESS);
    ASSERT_NE(dst, nullptr);
    ASSERT_EQ(dstSize, 3);
    GTEST_LOG_(INFO) << "OpenCVNativeTest: OpenCVNativeTest021 end";
}

/**
 * @tc.name: OpenCVNativeTest022
 * @tc.desc: OH_CV_Merge
 * @tc.type: FUNC
 */
HWTEST_F(OpenCVNativeTest, OpenCVNativeTest022, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "OpenCVNativeTest: OpenCVNativeTest022 start";
    uint8_t srcData[M_DATA_SIZE_49] = {0};
    for (uint8_t i = 0; i < M_DATA_SIZE_49; i++) {
        srcData[i] = i;
    }

    CV_Mat srcs[3] = {
        {M_SIZE_7, M_SIZE_7, CV_MAT_TYPE_8UC1, &srcData, sizeof(srcData)},
        {M_SIZE_7, M_SIZE_7, CV_MAT_TYPE_8UC1, &srcData, sizeof(srcData)},
        {M_SIZE_7, M_SIZE_7, CV_MAT_TYPE_8UC1, &srcData, sizeof(srcData)}
    };

    CV_Mat dst;
    CV_Merge_Opts opts = {
        .src = srcs,
        .srcSize = 3,
        .dst = &dst
    };

    auto res = OH_CV_Merge(opts);
    ASSERT_EQ(res, IMAGE_RESULT_SUCCESS);
    ASSERT_EQ(dst.type, CV_MAT_TYPE_8UC3);
    ASSERT_EQ(dst.rows, M_SIZE_7);
    ASSERT_EQ(dst.cols, M_SIZE_7);
    ASSERT_NE(dst.data, nullptr);
    ASSERT_NE(dst.dataSize, 0);
    GTEST_LOG_(INFO) << "OpenCVNativeTest: OpenCVNativeTest022 end";
}

/**
 * @tc.name: OpenCVNativeTest023
 * @tc.desc: OH_CV_MinMaxLoc
 * @tc.type: FUNC
 */
HWTEST_F(OpenCVNativeTest, OpenCVNativeTest023, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "OpenCVNativeTest: OpenCVNativeTest023 start";
    uint8_t srcData[M_DATA_SIZE_49] = {0};
    for (uint8_t i = 0; i < M_DATA_SIZE_49; i++) {
        srcData[i] = i;
    }

    double minValue = -1;
    double maxValue = -1;
    CV_Point minPoint;
    CV_Point maxPoint;
    CV_MinMaxLoc_Opts opts = {
        .src = {M_SIZE_7, M_SIZE_7, CV_MAT_TYPE_8UC1, &srcData, sizeof(srcData)},
        .minValue = &minValue,
        .maxValue = &maxValue,
        .minIndex = &minPoint,
        .maxIndex = &maxPoint
    };

    auto res = OH_CV_MinMaxLoc(opts);
    ASSERT_EQ(res, IMAGE_RESULT_SUCCESS);
    ASSERT_NE(minValue, -1);
    ASSERT_NE(maxValue, -1);
    ASSERT_NE(minPoint.x, -1);
    ASSERT_NE(minPoint.y, -1);
    ASSERT_NE(maxPoint.x, -1);
    ASSERT_NE(maxPoint.y, -1);
    GTEST_LOG_(INFO) << "OpenCVNativeTest: OpenCVNativeTest023 end";
}

/**
 * @tc.name: OpenCVNativeTest024
 * @tc.desc: OH_CV_Normalize
 * @tc.type: FUNC
 */
HWTEST_F(OpenCVNativeTest, OpenCVNativeTest024, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "OpenCVNativeTest: OpenCVNativeTest024 start";
    uint8_t srcData[M_DATA_SIZE_49] = {0};
    for (uint8_t i = 0; i < M_DATA_SIZE_49; i++) {
        srcData[i] = i;
    }

    CV_Mat dst;
    CV_Normalize_Opts opts = {
        .src = {M_SIZE_7, M_SIZE_7, CV_MAT_TYPE_8UC1, &srcData, sizeof(srcData)},
        .dst = &dst,
        .alpha = 15.0,
        .beta = 38.5,
        .normType = CV_NORMALIZE_TYPE_MINMAX
    };

    auto res = OH_CV_Normalize(opts);
    ASSERT_EQ(res, IMAGE_RESULT_SUCCESS);
    ASSERT_EQ(dst.type, CV_MAT_TYPE_8UC1);
    ASSERT_EQ(dst.rows, M_SIZE_7);
    ASSERT_EQ(dst.cols, M_SIZE_7);
    ASSERT_NE(dst.data, nullptr);
    ASSERT_NE(dst.dataSize, 0);
    GTEST_LOG_(INFO) << "OpenCVNativeTest: OpenCVNativeTest024 end";
}

/**
 * @tc.name: OpenCVNativeTest025
 * @tc.desc: OH_CV_MixChannels
 * @tc.type: FUNC
 */
HWTEST_F(OpenCVNativeTest, OpenCVNativeTest025, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "OpenCVNativeTest: OpenCVNativeTest025 start";
    uint8_t srcData[M_DATA_SIZE_147] = {0};
    for (uint8_t i = 0; i < M_DATA_SIZE_49; i++) {
        srcData[i] = i % 3;
    }

    CV_Mat srcs[1] = {
        {M_SIZE_7, M_SIZE_7, CV_MAT_TYPE_8UC3, &srcData, sizeof(srcData)}
    };
    CV_Mat dsts[2] = {
        {M_SIZE_7, M_SIZE_7, CV_MAT_TYPE_8UC1, nullptr, 0},
        {M_SIZE_7, M_SIZE_7, CV_MAT_TYPE_8UC2, nullptr, 0}
    };
    int32_t formTo[] = {0, 1, 1, 0, 2, 2};

    CV_MixChannels_Opts opts = {
        .src = srcs,
        .srcSize = 1,
        .dst = dsts,
        .dstSize = 2, 
        .formTo = formTo,
        .formToSize = 6,
        .parisCount = 3
    };

    auto res = OH_CV_MixChannels(opts);
    ASSERT_EQ(res, IMAGE_RESULT_SUCCESS);
    ASSERT_EQ(dsts[0].type, CV_MAT_TYPE_8UC1);
    ASSERT_EQ(dsts[0].rows, M_SIZE_7);
    ASSERT_EQ(dsts[0].cols, M_SIZE_7);
    ASSERT_NE(dsts[0].data, nullptr);
    ASSERT_NE(dsts[0].dataSize, 0);
    ASSERT_EQ(dsts[1].type, CV_MAT_TYPE_8UC2);
    ASSERT_EQ(dsts[1].rows, M_SIZE_7);
    ASSERT_EQ(dsts[1].cols, M_SIZE_7);
    ASSERT_NE(dsts[1].data, nullptr);
    ASSERT_NE(dsts[1].dataSize, 0);
    GTEST_LOG_(INFO) << "OpenCVNativeTest: OpenCVNativeTest025 end";
}
}
}
