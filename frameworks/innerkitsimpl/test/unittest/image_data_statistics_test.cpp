/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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
#include <string>
#include "image_data_statistics.h"

using namespace OHOS::Media;
using namespace testing::ext;
namespace OHOS {
namespace Multimedia {
static const std::string inputString = "hello,title";
static const std::string inputFmtString = "hello10,title";
class ImageDataStatisticsTest : public testing::Test {
public:
    ImageDataStatisticsTest() {}
    ~ImageDataStatisticsTest() {}
};

/**
 * @tc.name: DataStatistics001
 * @tc.desc: test DataStatistics
 * @tc.type: FUNC
 */
HWTEST_F(ImageDataStatisticsTest, DataStatistics001, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageDataStatisticsTest: StartPacking001 start";
    ImageDataStatistics imageDataStatistics(inputString);
    std::string outputString = imageDataStatistics.GetTitle();
    ASSERT_EQ(inputString, outputString);
    GTEST_LOG_(INFO) << "ImagePackerTest: DataStatistics001 end";
}

/**
 * @tc.name: DataStatistics002
 * @tc.desc: test DataStatistics fmt
 * @tc.type: FUNC
 */
HWTEST_F(ImageDataStatisticsTest, DataStatistics002, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageDataStatisticsTest: StartPacking002 start";
    ImageDataStatistics imageDataStatistics("%s%d,%s", "hello", 10, "title");
    std::string outputString = imageDataStatistics.GetTitle();
    ASSERT_EQ(inputFmtString, outputString);
    GTEST_LOG_(INFO) << "ImagePackerTest: StartPacking002 end";
}
} // namespace Multimedia
} // namespace OHOS