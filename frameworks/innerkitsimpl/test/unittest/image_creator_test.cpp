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
#include "directory_ex.h"
#include "hilog/log.h"
#include "image_creator.h"
#include "image_packer.h"
#include "image_source.h"
#include "image_utils.h"
#include "hilog/log.h"
#include "image_creator_manager.h"

using namespace testing::ext;
using namespace OHOS::Media;

namespace OHOS {
namespace Multimedia {
static constexpr int32_t WIDTH = 8192;
static constexpr int32_t HEIGHT = 8;
static constexpr int32_t FORMAT = 4;
static constexpr int32_t CAPACITY = 8;
}

class ImageCreatorTest : public testing::Test {
public:
    ImageCreatorTest(){}
    ~ImageCreatorTest(){}
};

/**
 * @tc.name: OnBufferRelease001
 * @tc.desc: test OnBufferRelease
 * @tc.type: FUNC
 */
HWTEST_F(ImageCreatorTest, OnBufferRelease001, TestSize.Level3)
{
    std::shared_ptr<ImageCreator> iva = std::make_shared<ImageCreator>();
    iva->iraContext_->SetWidth(WIDTH);
    iva->iraContext_->SetHeight(HEIGHT);
    iva->iraContext_->SetFormat(FORMAT);
    iva->iraContext_->SetCapicity(CAPICITY);
    OHOS::sptr<OHOS::SurfaceBuffer> buffer;
    buffer = iva->DequeueImage();
    iva->OnBufferRelease(buffer);
}

/**
 * @tc.name: OnBufferRelease002
 * @tc.desc: test OnBufferRelease is nullptr
 * @tc.type: FUNC
 */
HWTEST_F(ImageCreatorTest, OnBufferRelease002, TestSize.Level3)
{
    std::shared_ptr<ImageCreator> iva = std::make_shared<ImageCreator>();
    iva->iraContext_->SetWidth(WIDTH;
    iva->iraContext_->SetHeight(HEIGHT);
    iva->iraContext_->SetFormat(FORMAT);
    iva->iraContext_->SetCapicity(CAPICITY);
    OHOS::sptr<OHOS::SurfaceBuffer> buffer;
    buffer = nullptr;
    iva->OnBufferRelease(buffer);
}

/**
 * @tc.name: CreateImageCreatorContext001
 * @tc.desc: test CreateImageCreatorContext
 * @tc.type: FUNC
 */
HWTEST_F(ImageCreatorTest, CreateImageCreatorContext001, TestSize.Level3)
{
    std::shared_ptr<ImageCreator> iva = std::make_shared<ImageCreator>();
    iva->iraContext_->SetWidth(WIDTH);
    iva->iraContext_->SetHeight(HEIGHT);
    iva->iraContext_->SetFormat(FORMAT);
    iva->iraContext_->SetCapicity(CAPICITY);
    iva->iraContext_ = ImageCreatorContext::CreateImageCreatorContext();
}

/**
 * @tc.name: OnBufferAvailable001
 * @tc.desc: test OnBufferAvailable
 * @tc.type: FUNC
 */
HWTEST_F(ImageCreatorTest, OnBufferAvailable001, TestSize.Level3)
{
    sptr<ImageCreatorSurfaceListener> listener = new ImageReceiverSurfaceListener();
    listener->OnBufferAvailable();
}

/**
 * @tc.name: CreateImageCreator001
 * @tc.desc: test CreateImageCreator is nullptr
 * @tc.type: FUNC
 */
HWTEST_F(ImageCreatorTest, CreateImageCreator001, TestSize.Level3)
{
    std::shared_ptr<ImageCreator> CreateImageCreator(int32_t width,
                                                     int32_t height,
                                                     int32_t format,
                                                     int32_t capicity);
    std::shared_ptr<ImageCreator> iva = std::make_shared<ImageCreator>();
    iva->creatorConsumerSurface_ = Surface::CreateSurfaceAsConsumer();
    creatorConsumerSurface_ = nullptr;
    iva->CreateImageCreator();
    
}

/**
 * @tc.name: CreateImageCreator002
 * @tc.desc: test CreateImageCreator
 * @tc.type: FUNC
 */
HWTEST_F(ImageCreatorTest, CreateImageCreator002, TestSize.Level3)
{
    // std::shared_ptr<ImageCreator> iva = std::make_shared<ImageCreator>();
    // std::unique_ptr<ImageCreatorNapi> reference = std::make_unique<ImageCreatorNapi>();
    // iva->creatorConsumerSurface_ = Surface::CreateSurfaceAsConsumer();
    // reference->imageCreator_ = staticInstance_;
    // staticInstance_ = ImageCreator::CreateImageCreator(WIDTH, HEIGHT, FORMAT, CAPICITY);
}

/**
 * @tc.name: SaveSTP001
 * @tc.desc: test SaveSTP
 * @tc.type: FUNC
 */
HWTEST_F(ImageCreatorTest, SaveSTP001, TestSize.Level3)
{
    int32_t errorcode = 0;
    OHOS::sptr<OHOS::SurfaceBuffer> buffer;
    uint32_t *addr = (uint32_t *)buffer->GetVirAddr();
    uint8_t *addr2 = nullptr;
    int32_t size = buffer->GetSize();
    errorcode = SaveSTP(addr, addr2, (uint32_t)size, initializationOpts);
}

/**
 * @tc.name: SaveSTP002
 * @tc.desc: test SaveSTP is nullptr
 * @tc.type: FUNC
 */
HWTEST_F(ImageCreatorTest, SaveSTP002, TestSize.Level3)
{
    int32_t errorcode = 0;
    OHOS::sptr<OHOS::SurfaceBuffer> buffer;
    buffer = nullptr;
    errorCode = ERR_MEDIA_INVALID_VALUE;
}

/**
 * @tc.name: SaveSenderBufferAsImage001
 * @tc.desc: test SaveSenderBufferAsImage
 * @tc.type: FUNC
 */
HWTEST_F(ImageCreatorTest, SaveSenderBufferAsImage001, TestSize.Level3)
{
    AllocatorType allocatorType_ = AllocatorType::HEAP_ALLOC;
    uint32_t bufferSize = dstPixelMap->GetByteCount();
    *buffer = static_cast<uint8_t *>(malloc(bufferSize));
    ReleaseBuffer(AllocatorType::HEAP_ALLOC, buffer);
}

/**
 * @tc.name: DequeueImage001
 * @tc.desc: test DequeueImage
 * @tc.type: FUNC
 */
HWTEST_F(ImageCreatorTest, DequeueImage001, TestSize.Level3)
{
    DecodeContext context;
    ImageReceiverNapi *constructor_ = nullptr;
    auto native = context->constructor_->imageCreator_;
    auto surfacebuffer = native->DequeueImage();
}

/**
 * @tc.name: QueueImage001
 * @tc.desc: test QueueImage
 * @tc.type: FUNC
 */
HWTEST_F(ImageCreatorTest, QueueImage001, TestSize.Level3)
{
    sptr<SurfaceBuffer> sSurfaceBuffer_;
    imageNapi_ = ImageNapi::GetImageSource(env, argv[PARAM0]);
    auto surfacebuffer = context->imageNapi_->sSurfaceBuffer_;
    native->QueueImage(surfacebuffer);
}

/**
 * @tc.name: GetCreatorSurface001
 * @tc.desc: test GetCreatorSurface
 * @tc.type: FUNC
 */
HWTEST_F(ImageCreatorTest, GetCreatorSurface001, TestSize.Level3)
{
    
}

/**
 * @tc.name: getSurfaceById001
 * @tc.desc: test getSurfaceById
 * @tc.type: FUNC
 */
HWTEST_F(ImageCreatorTest, getSurfaceById001, TestSize.Level3)
{
    std::string creatorKey = imageCreator->iraContext_->GetCreatorKey();
    OHOS::sptr<OHOS::Surface> creatorSurface = ImageCreator::getSurfaceById(creatorKey);
}

/**
 * @tc.name: ReleaseCreator001
 * @tc.desc: test ReleaseCreator
 * @tc.type: FUNC
 */
HWTEST_F(ImageCreatorTest, ReleaseCreator001, TestSize.Level3)
{
    
}

}
}