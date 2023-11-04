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

#include "astc_utils.h"
#include "hitrace_meter.h"
#include "media_errors.h"
#include "transaction/rs_render_service_client.h"
#include "transaction/re_interfaces.h"

namespace OHOS{
namespace Media{
using namespace OHOS::HiviewDFX;
using namespace OHOS::Rosen;
using namespace std;
static constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {LOG_CORE, LOG_TAG_DOMAIN_ID_IMAGE, "astc_utils"};

class TextureConversionCallback : public Rosen::SurfaceCaptureCallback, public AstcFuture<std::shared_ptr<Media::PixelMap>>
{
public:
    TextureConversionCallback() = default;
    ~TextureConversionCallback() {};
    void OnSurfaceCapture(std::shared_ptr<Media::PixelMap> pixelmap) override
    {
        FutureCall(pixelmap);
    }

protected:
    void call(std::shared_ptr<Media::PixelMap> pixelmap) override
    {
        if (!flag_) {
            pixelmap_ = pixelmap;
            flag_ = true;
        }
    }
    bool IsReady() override
    {
        return flag_;
    }
    std::shared_ptr<Media::PixelMap> FetchResult() override
    {
        return pixelMap_;
    }

private:
    bool flag_ = false;
    std::shared_ptr<Media::PixelMap> pixelmap_ = nullptr;
}

std::shared_ptr<PixelMap> AstcUtils::CreatePixelMapFromASTC(std::shared_ptr<ImageSource> imageSource, uint32_t &errorCode)
{
    Decodeoptions decodeOpts;
    auto pixelAstc = imageSource->CreatePixelMap(decodeOpts, errorCode);
    if (pixelAstc == nullptr || errorCode != SUCCESS || !pixelAstc->IsAstc()) {
        Hilog::Error(LABEL, "CreatePixelMapFromASTC by imageSource pixelAstc == nullptr || errorCode != SUCCESS");
        return nullptr;
    }
    return AstcUtils::TextureConversionRequest(std::move(pixelAstc), errorCode);
}

std::shared_ptr<PixelMap> AstcUtils::CreatePixelMapFromASTC(std::shared_ptr<PixelMap> pixelAstc, uint32_t &errorCode)
{
    if (pixelAstc == nullptr || !pixelAstc->IsAstc()) {
        errorCode = ERROR;
        Hilog::Error(LABEL, "TextureConversionRequest pixelAstc is not astc");
        return nullptr;
    }
    return AstcUtils::TextureConversionRequest(pixelAstc, errorCode);
}

std::shared_ptr<PixelMap> AstcUtils::TextureConversionRequest(std::shared_ptr<PixelMap> pixelAstc, uint32_t &errorCode)
{
    StartTrace(HITRACE_TAG_ZIMAGE, "TextureConversionRequest");

    if (pixelAstc == nullptr) {
        errorCode = ERROR;
        Hilog::Error(LABEL, "TextureConversionRequest pixelAstc == nullptr");
        return nullptr;
    }
    auto callback = std::make_shared<TextureConversionCallback>();
    bool ret = Rosen::RSInterfaces::GetInstance().TextureConversion(callback, pixelAstc);
    if (!ret) {
        errorCode = ERROR;
        Hilog::Error(LABEL, "TakeSurfaceCapture error");
        return nullptr;
    }
    auto pixelMap = callback->GetResult(2000);
    if (pixelMap == nullptr) {
        Hilog::Error(LABEL, "pixelMap == nullptr");
        return nullptr;
    }
    FinishTrace(HITRACE_TAG_ZIMAGE);
    return pixelMap;
}
} // namespace Media
} // namespace OHOS