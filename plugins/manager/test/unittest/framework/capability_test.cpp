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
class CapabilityTest : public testing::Test {
public:
    CapabilityTest() {}
    ~CapabilityTest() {}
};

/**
 * @tc.name: SetCapabilityTest001
 * @tc.desc: SetCapability
 * @tc.type: FUNC
 */
HWTEST_F(CapabilityTest, SetCapabilityTest001, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "CapabilityTest: SetCapabilityTest001 start";
    Capability capability;
    const nlohmann::json classInfo;
    uint32_t ret = capability.SetCapability(classInfo);
    ASSERT_NE(ret, SUCCESS);
    GTEST_LOG_(INFO) << "CapabilityTest: SetCapabilityTest001 end";
}

/**
 * @tc.name: SetCapabilityTest001
 * @tc.desc: SetCapability
 * @tc.type: FUNC
 */
HWTEST_F(CapabilityTest, SetCapabilityTest001, TestSize.Level3)
{
    std::map<std::string, AttrData> caps;
    GTEST_LOG_(INFO) << "CapabilityTest: SetCapabilityTest001 start";
    Capability capability(caps);
    const nlohmann::json classInfo;
    uint32_t ret = capability.SetCapability(classInfo);
    capability.IsCompatible(caps);
    const AttrData *dataPtr = capability.GetCapability("");
    std::map<std::string, AttrData> cap = capability.GetCapability();
    ASSERT_NE(ret, SUCCESS);
    GTEST_LOG_(INFO) << "CapabilityTest: SetCapabilityTest001 end";
}
}
}