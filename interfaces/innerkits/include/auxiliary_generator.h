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

#ifndef INTERFACE_INNERKITS_INCLUDE_AUXILIARY_GENERATOR_H_
#define INTERFACE_INNERKITS_INCLUDE_AUXILIARY_GENERATOR_H_

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
        AbsImageDecoder* extDecoder, AuxiliaryPictureType type, uint32_t &errorCode);
    static shared_ptr<AuxiliaryPicture> GenerateJpegAuxiliaryPicture(
        unique_ptr<InputDataStream>& auxiliaryStream, AuxiliaryPictureType type, uint32_t &errorCode);

private:
    static ImageInfo MakeImageInfo(int width, int height, PixelFormat pf, AlphaType at, ColorSpace cs);
    static AuxiliaryPictureInfo MakeAuxiliaryPictureInfo(
        AuxiliaryPictureType type, Size size, int32_t rowStride, PixelFormat format, ColorSpace colorSpace);
    static bool DecodeHeifMetaData(AbsImageDecoder* extDecoder, unique_ptr<AuxiliaryPicture> &auxPicture,
                                   AuxiliaryPictureType type, uint32_t &errorCode);
    static bool DecodeJpegMetaData(unique_ptr<InputDataStream> &auxStream, shared_ptr<AuxiliaryPicture> &auxPicture,
                                   uint32_t &errorCode);
    static shared_ptr<ImageMetadata> CreateExifMetadata(uint8_t *buffer, const uint32_t size, uint32_t &errorCode);
    static void FreeContextBuffer(const Media::CustomFreePixelMap &func, AllocatorType allocType, PlImageBuffer &buffer);
    static AbsImageDecoder* DoCreateDecoder(string codecFormat, PluginServer &pluginServer, InputDataStream &sourceData,
                                            uint32_t &errorCode) __attribute__((no_sanitize("cfi")));
    static bool SetHdrMetadata(AbsImageDecoder *extDecoder, unique_ptr<AuxiliaryPicture> &auxPicture);
};
} // namespace Media
} // namespace OHOS

#endif //INTERFACE_INNERKITS_INCLUDE_AUXILIARY_GENERATOR_H_