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
#include <fstream>
#include <fcntl.h>
#include "file_packer_stream.h"
#include "image_type.h"
#include "image_utils.h"
#include "image_source.h"
#include "image_source_util.h"
#include "media_errors.h"
#include "pixel_map.h"

using namespace testing::ext;
using namespace OHOS::Media;
using namespace OHOS::ImageSourceUtil;

namespace OHOS {
namespace Multimedia {
class ImplClassMgrTest : public testing::Test {
public:
    ImplClassMgrTest() {}
    ~ImplClassMgrTest() {}
};

/**
 * @tc.name: AddClass001
 * @tc.desc: CreateObject
 * @tc.type: FUNC
 */
HWTEST_F(ImplClassMgrTest, AddClass001, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "PluginsManagerSrcFrameWorkTest: AddClass001 start";
    ImplClassMgr &implClassMgr = DelayedRefSingleton<ImplClassMgr>::GetInstance();
    const string implClassName = "";
    uint16_t id = 0;
    uint32_t errorCode;
    PluginClassBase *obj = implClassMgr.CreateObject(id, implClassName, errorCode);
    EXPECT_EQ(errorCode, ERR_MATCHING_PLUGIN);
    ASSERT_EQ(obj, nullptr);
    GTEST_LOG_(INFO) << "PluginsManagerSrcFrameWorkTest: AddClass001 end";
}
}
}