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

 #include "data_buf.h"
#include <gtest/gtest.h>


using namespace testing::ext;
using namespace testing;
using namespace OHOS::Media;

namespace OHOS {
namespace Media {
class DataBufTest : public testing::Test {
public:
    DataBufTest() {}
    ~DataBufTest() {}
};

/**
 * @tc.name: DataBufTest_Write001
 * @tc.desc: 验证DataBuf的write_uint8函数
 * @tc.type: FUNC
 */
HWTEST_F(DataBufTest, DataBufTest_Write001, TestSize.Level3){
    DataBuf dataBuf(10);
    dataBuf.write_uint8(0, 123);
    EXPECT_EQ(dataBuf.ReadUInt8(0), 123);
}

}
}