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

#include "interfaces/kits/native/include/image_packer_mdk.h"
#include "interfaces/kits/native/include/image_mdk.h"


struct ImagePackerCapi {
public:
    ImagePackerCapi();
    ImagePackerCapi(ImagePacekr* imagePacker);
    ~ImagePackerCapi();

    int32_t PackingFromImageSource(ImagePacker_Opts* option, ImageSourceCapi* imageSourceCapi,
        uint8_t** outData, int64_t* size);

    int32_t PackingFromPixelMap(ImagePacker_Opts* option, PixelMapCapi* pixelMapCapi,
        uint8_t** outData, int64_t* size);

    int32_t PackToFileFromImageSource(ImagePacker_Opts* option, ImageSourceCapi* imageSourceCapi, const int fd);

    int32_t PackToFileFromPixelMap(ImagePacker_Opts* option, PixelMapCapi* pixelMapCapi, const int fd);

    shared_ptr<OHOS::Media::ImagePacker> GetImagePacker();

private:
    shared_ptr<OHOS::Media::ImagePacker> imagePacker_;
};
