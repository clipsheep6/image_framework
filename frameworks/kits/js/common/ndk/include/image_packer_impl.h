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
#ifndef INTERFACES_KITS_NATIVE_INCLUDE_IMAGE_PACKER_IMP_H
#define INTERFACES_KITS_NATIVE_INCLUDE_IMAGE_PACKER_IMP_H

// #include "image_packer_mdk.h"
// #include "image_mdk.h"
#include "image_packer.h"
#include "image_source_impl.h"
#include "image_pixel_map_impl.h"


struct ImagePackerCapi {
public:
    ImagePackerCapi();
    ImagePackerCapi(std::shared_ptr<OHOS::Media::ImagePacker> imagePacker);
    ~ImagePackerCapi();

    int32_t PackingFromImageSource(OHOS::Media::PackOption* option, struct ImageSourceCapi* imageSourceCapi,
        uint8_t** outData, int64_t* size);

    int32_t PackingFromPixelMap(OHOS::Media::PackOption* option, struct PixelMapCapi* pixelMapCapi,
        uint8_t** outData, int64_t* size);

    int32_t PackToFileFromImageSource(OHOS::Media::PackOption* option, struct ImageSourceCapi* imageSourceCapi, const int fd);

    int32_t PackToFileFromPixelMap(OHOS::Media::PackOption* option, struct PixelMapCapi* pixelMapCapi, const int fd);

    std::shared_ptr<OHOS::Media::ImagePacker> GetImagePacker();

private:
    std::shared_ptr<OHOS::Media::ImagePacker> imagePacker_;
};
#endif // INTERFACES_KITS_NATIVE_INCLUDE_IMAGE_PACKER_IMP_H