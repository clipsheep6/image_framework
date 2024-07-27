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

#ifndef INTERFACE_INNERKITS_INCLUDE_AUXILIARY_GENERATOR_H
#define INTERFACE_INNERKITS_INCLUDE_AUXILIARY_GENERATOR_H

#include "abs_image_decoder.h"
#include "auxiliary_picture.h"
#include "image_type.h"
#include "auxiliary_picture.h"
#include "image/input_data_stream.h"
#include "plugin_server.h"

namespace OHOS {
namespace Media {
using namespace std;
using namespace ImagePlugin;
using namespace MultimediaPlugin;

class AuxiliaryGenerator {
public:
    static shared_ptr<AuxiliaryPicture> GenerateHeifAuxiliaryPicture(
        unique_ptr<AbsImageDecoder> &extDecoder, AuxiliaryPictureType type, uint32_t &errorCode);
    static shared_ptr<AuxiliaryPicture> GenerateJpegAuxiliaryPicture(
        unique_ptr<InputDataStream> &auxiliaryStream, AuxiliaryPictureType type, uint32_t &errorCode);

private:
    static ImageInfo MakeImageInfo(int width, int height, PixelFormat pf, AlphaType at, ColorSpace cs);
    static AuxiliaryPictureInfo MakeAuxiliaryPictureInfo(AuxiliaryPictureType type,
        const Size &size, int32_t rowStride, PixelFormat format, ColorSpace colorSpace);
    static uint32_t DecodeHdrMetadata(unique_ptr<AbsImageDecoder> &extDecoder,
                                      unique_ptr<AuxiliaryPicture> &auxPicture);
    static uint32_t DecodeFragmentMetadata(AbsImageDecoder *extDecoder, unique_ptr<AuxiliaryPicture> &auxPicture);
    static uint32_t DecodeHeifMetadata(unique_ptr<AbsImageDecoder> &extDecoder, AuxiliaryPictureType type,
                                       unique_ptr<AuxiliaryPicture> &auxPicture);
    static AbsImageDecoder* DoCreateDecoder(string codecFormat, PluginServer &pluginServer,
        InputDataStream &sourceData, uint32_t &errorCode) __attribute__((no_sanitize("cfi")));
    static uint32_t SetJpegAuxiliaryDecodeOption(std::unique_ptr<AbsImageDecoder> &decoder, PlImageInfo &plInfo);
    static void FreeContextBuffer(const Media::CustomFreePixelMap &func, AllocatorType allocType,
                                  PlImageBuffer &buffer);
};
} // namespace Media
} // namespace OHOS

#endif //INTERFACE_INNERKITS_INCLUDE_AUXILIARY_GENERATOR_H