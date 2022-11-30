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
#include <iostream>
#include "gst_plugin_fw.h"
#include "__mutex_base"
#include "map"
#include "plugin_errors.h"
#include "priority_scheme.h"
#include "vector"

using OHOS::DelayedRefSingleton;
using std::string;
using std::vector;
using namespace testing::ext;
using namespace OHOS::MultimediaPlugin;
using namespace testing::ext;

namespace OHOS {
namespace Multimedia {
class PluginMgrTest : public testing::Test {
public:
    PluginMgrTest() {}
    ~PluginMgrTest() {}
};

/**
 * @tc.name: PluginMgrTest001
 * @tc.desc: Register
 * @tc.type: FUNC
 */
HWTEST_F(PluginMgrTest, PluginMgrTest001, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "PluginMgrTest: PluginMgrTest001 start";
    PluginMgr &pluginMgr = DelayedRefSingleton<PluginMgr>::GetInstance();
    const std::vector<std::string> canonicalPaths;
    uint32_t ret = pluginMgr.Register(canonicalPaths);
    ASSERT_EQ(ret, SUCCESS);
    GTEST_LOG_(INFO) << "PluginMgrTest: PluginMgrTest001 end";
}
}
}