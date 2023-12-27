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

#ifndef PLUGINS_COMMON_LIBS_IMAGE_LIBEXTPLUGIN_INCLUDE_TEXTURE_ENCODE_ASTC_ENCODE_H
#define PLUGINS_COMMON_LIBS_IMAGE_LIBEXTPLUGIN_INCLUDE_TEXTURE_ENCODE_ASTC_ENCODE_H
#include "abs_image_encoder.h"
#include "texture_type.h"

namespace OHOS {
namespace ImagePlugin {
uint32_t AstcSoftwareEncode(uint8_t *input, TextureEncodeOptions *param, bool enableQualityCheck, uint8_t *output);
class AstcCodec {
public:
    AstcCodec() {};
    ~AstcCodec() {};
    uint32_t SetAstcEncode(OutputDataStream* outputStream, PlEncodeOptions &option, Media::PixelMap* pixelMap);
    uint32_t ASTCEncode();

private:
    DISALLOW_COPY_AND_MOVE(AstcCodec);
    OutputDataStream* astcOutput_ = nullptr;
    PlEncodeOptions astcOpts_;
    Media::PixelMap* astcPixelMap_ = nullptr;
};
} // namespace ImagePlugin
} // namespace OHOS
#endif // PLUGINS_COMMON_LIBS_IMAGE_LIBEXTPLUGIN_INCLUDE_TEXTURE_ENCODE_ASTC_ENCODE_H