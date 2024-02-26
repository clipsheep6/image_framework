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

#define private public
#include <gtest/gtest.h>
#include "image_type.h"
#include "basic_transformer.h"

using namespace testing::ext;
using namespace OHOS::Media;
namespace OHOS {
namespace Multimedia {
constexpr int32_t PIXEL_MAP_MAX_RAM_SIZE = 600 * 1024 * 1024;
class BasicTransformerTest : public testing::Test {
public:
    BasicTransformerTest() {}
    ~BasicTransformerTest() {}
};

/**
 * @tc.name: CheckAllocateBufferTest001
 * @tc.desc: CheckAllocateBuffer
 * @tc.type: FUNC
 */
HWTEST_F(BasicTransformerTest, CheckAllocateBufferTest001, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "BasicTransformerTest: CheckAllocateBufferTest001 start";
    BasicTransformer basicTransformer;
    PixmapInfo outPixmap;
    BasicTransformer::AllocateMem allocate = nullptr;
    int fd = 0;
    uint64_t bufferSize = 0;
    Size dstSize;
    bool ret = basicTransformer.CheckAllocateBuffer(outPixmap, allocate, fd, bufferSize, dstSize);
    ASSERT_EQ(ret ,false);
    bufferSize = 128;
    ret = basicTransformer.CheckAllocateBuffer(outPixmap, allocate, fd, bufferSize, dstSize);
    ASSERT_EQ(ret ,true);
    GTEST_LOG_(INFO) << "BasicTransformerTest: CheckAllocateBufferTest001 end";
}


/**
 * @tc.name: ReleaseBufferTest001
 * @tc.desc: ReleaseBuffer
 * @tc.type: FUNC
 */
HWTEST_F(BasicTransformerTest, ReleaseBufferTest001, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "BasicTransformerTest: ReleaseBufferTest001 start";
    BasicTransformer basicTransformer;
    AllocatorType allocatorType = AllocatorType::SHARE_MEM_ALLOC;
    int fd = 0;
    int dataSize = 2;
    uint8_t *buffer = new uint8_t;
    basicTransformer.ReleaseBuffer(allocatorType, fd, dataSize, buffer);
    ASSERT_EQ(sizeof(buffer), 8);
    allocatorType = AllocatorType::HEAP_ALLOC;
    basicTransformer.ReleaseBuffer(allocatorType, fd, dataSize, buffer);
    delete buffer;
    GTEST_LOG_(INFO) << "BasicTransformerTest: ReleaseBufferTest001 end";
}

/**
 * @tc.name: TransformPixmapTest001
 * @tc.desc: TransformPixmap
 * @tc.type: FUNC
 */
HWTEST_F(BasicTransformerTest, TransformPixmapTest001, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "BasicTransformerTest: TransformPixmapTest001 start";
    BasicTransformer basicTransformer;
    PixmapInfo inPixmap;
    PixmapInfo outPixmap;
    BasicTransformer::AllocateMem allocate = nullptr;
    uint32_t ret = basicTransformer.TransformPixmap(inPixmap, outPixmap, allocate);
    ASSERT_EQ(ret, ERR_IMAGE_GENERAL_ERROR);
    inPixmap.data = new uint8_t;
    ret = basicTransformer.TransformPixmap(inPixmap, outPixmap, allocate);
    ASSERT_EQ(ret, ERR_IMAGE_INVALID_PIXEL);
    inPixmap.imageInfo.pixelFormat = PixelFormat::ARGB_8888;
    basicTransformer.matrix_.operType_ = 0x02;
    basicTransformer.matrix_.fMat_[IMAGE_SCALEX] = 1;
    inPixmap.imageInfo.size.width = -FHALF;
    basicTransformer.matrix_.fMat_[IMAGE_SCALEY] = 1;
    inPixmap.imageInfo.size.height = -FHALF;
    ret = basicTransformer.TransformPixmap(inPixmap, outPixmap, allocate);
    ASSERT_EQ(ret, ERR_IMAGE_ALLOC_MEMORY_FAILED);
    delete inPixmap.data;
    GTEST_LOG_(INFO) << "BasicTransformerTest: TransformPixmapTest001 end";
}
}
}