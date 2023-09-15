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

#include <gtest/gtest.h>
#include <cstdint>
#include <cstddef>
#include <memory>
#include <string>
#include "image_type.h"
#include "memory_manager.h"

#include <cerrno>
#include <unistd.h>
#include "hilog/log.h"
#include "image_utils.h"
#include "log_tags.h"
#include "media_errors.h"
#include "securec.h"

#define MAP_TRUE ((void *) 1)

using namespace testing::ext;
using namespace OHOS::Media;
namespace OHOS {
namespace Multimedia {
class MemoryManagerTest : public testing::Test {
public:
    MemoryManagerTest() {}
    ~MemoryManagerTest() {}
};


/**
 * @tc.name: MemoryManagerTest001
 * @tc.desc: Test of GetPixel8, GetPixel16, GetPixel32
 * @tc.type: FUNC
 */
HWTEST_F(MemoryManagerTest, MemoryManagerTest001, TestSize.Level3)
{
    HeapMemory heapMemory;
    heapMemory.data.data = nullptr;
    auto res = heapMemory.Release();
    ASSERT_EQ(res, ERR_IMAGE_DATA_ABNORMAL);
    GTEST_LOG_(INFO) << "MemoryManagerTest: MemoryManagerTest001 end";
}

/**
 * @tc.name: MemoryManagerTest002
 * @tc.desc: Test of GetPixel8, GetPixel16, GetPixel32
 * @tc.type: FUNC
 */
HWTEST_F(MemoryManagerTest, MemoryManagerTest002, TestSize.Level3)
{
    SharedMemory sharedMemory;
    sharedMemory.extend.data = MAP_TRUE;
    sharedMemory.release();
    ASSERT_EQ(sharedMemory.extend.data, nullptr);
    ASSERT_EQ(sharedMemory.extend.size, SIZE_ZERO);
    GTEST_LOG_(INFO) << "MemoryManagerTest: MemoryManagerTest002 end";
}

/**
 * @tc.name: MemoryManagerTest003
 * @tc.desc: Test of GetPixel8, GetPixel16, GetPixel32
 * @tc.type: FUNC
 */
HWTEST_F(MemoryManagerTest, MemoryManagerTest003, TestSize.Level3)
{
    DmaMemory dmaMemory;
    dmaMemory.extend.data = MAP_TRUE;
    auto res = dmaMemory.Release();
    ASSERT_EQ(res, ERR_DMA_DATA_ABNORMAL);
    ASSERT_EQ(dmaMemory.extend.data, nullptr);
    ASSERT_EQ(dmaMemory.extend.size, SIZE_ZERO);
    GTEST_LOG_(INFO) << "MemoryManagerTest: MemoryManagerTest003 end";
}
}
}
