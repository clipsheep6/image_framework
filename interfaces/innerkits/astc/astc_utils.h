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

#ifndef INTERFACES_INNERKITS_ASTC_ASTC_UTILS_H_
#define INTERFACES_INNERKITS_ASTC_ASTC_UTILS_H_

#include "pixel_astc.h"
#include "pixel_map.h"
#include "image_source.h"
#include "hilog/log.h"
#include "log_tags.h"

namespace OHOS{
namespace Media{
class AstcUtils
{
public:
    AstcUtils() = default;
    ~AstcUtils(){};
    std::shared_ptr<PixelMap> CreatePixelMapFromASTC(std::shared_ptr<ImageSource> imageSource, uint32_t &errorCode);
    std::shared_ptr<PixelMap> CreatePixelMapFromASTC(std::shared_ptr<PixelMap> pixelAstc, uint32_t &errorCode);

private:
    std::shared_ptr<PixelMap> TextureConversionRequest(std::shared_ptr<PixelMap> pixelAstc, uint32_t &errorCode);
};

template <class T>
class AstcFuture
{
public:
    T GetResult(long timeOut)
    {
        std::unique_lock<std::mutex> lock(mutex_);
        if (!conditionVariable_.wait_for(lock, std::chrono::milliseconds(timeOut), [this]{return IsReady();})) {
            OHOS::HiviewDFX::HiLog::Error({LOG_CORE, LOG_TAG_DOMAIN_ID_IMAGE, "astc_utils"}, "wait for %{public}d, timeout", timeOut);
        }
        return FetchResult();
    }

protected:
    virtual bool IsReady() = 0;

    virtual T FetchResult() = 0;

    virtual void Call(T) = 0;

    void FutureCall(T t)
    {
        std::unique_lock<std::mutex> lock(mutex_);
        Call(t);
        conditionVariable_.notify_one();
    }
    std::mutex mutex_;

private:
    std::condition_variable conditionVariable_;
};








} // namespace Media
} // namespace OHOS
