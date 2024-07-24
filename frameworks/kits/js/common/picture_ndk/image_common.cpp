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
#include "image_common.h"
#include "image_common_impl.h"
#include "exif_metadata.h"
#include "stdio.h"
#include "securec.h"
#include "common_utils.h"
#ifdef __cplusplus
extern "C" {
#endif

MIDK_EXPORT
Image_ErrorCode OH_PictureMetadata_Create(MetadataType metadataType, OH_PictureMetadata **metadata)
{
    if (metadata == nullptr) {
        return IMAGE_BAD_PARAMETER;
    }
    //根据type子类指针指向父类 多态
    std::shared_ptr<OHOS::Media::ImageMetadata> metadataPtr = nullptr;
    if (metadataType == EXIF_METADATA) {
        metadataPtr = std::make_shared<OHOS::Media::ExifMetadata>();
        auto exifMetadata = static_cast<OHOS::Media::ExifMetadata *>(metadataPtr.get());
        exifMetadata->CreateExifdata();
    } else if (metadataType == FRAGMENT_METADATA) {
        //TODO: 子类内部接口还没完成
        return IMAGE_UNSUPPORTED_OPERATION;
    } else {
        return IMAGE_BAD_PARAMETER;
    }

    if (metadataPtr == nullptr) {
        return IMAGE_ALLOC_FAILED;
    }
    *metadata = new OH_PictureMetadata(metadataPtr);
    return IMAGE_SUCCESS;
}

MIDK_EXPORT
Image_ErrorCode OH_PictureMetadata_GetProperty(OH_PictureMetadata *metadata, Image_String *key, Image_String *value)
{
    if (metadata == nullptr || key == nullptr || key->data == nullptr || key->size == 0) {
        return IMAGE_BAD_PARAMETER;
    }
    if (value == nullptr) {
        return IMAGE_BAD_PARAMETER;
    }
    std::string keyString(key->data, key->size);
    if (keyString.empty()) {
        return IMAGE_BAD_PARAMETER;
    }
    std::string val;
    uint32_t errorCode = metadata->GetInnerAuxiliaryMetadata()->GetValue(keyString, val);
    if (errorCode != IMAGE_SUCCESS || val.empty()) {
        return IMAGE_BAD_PARAMETER;
    }

    if (value->size != 0 && value->size < val.size()) {
        return IMAGE_BAD_PARAMETER;
    }
    value->size = (value->size == 0) ? val.size() : value->size;
    value->data = static_cast<char *>(malloc(value->size));
    if (value->data == nullptr) {
        return IMAGE_ALLOC_FAILED;
    }
    if (EOK != memcpy_s(value->data, value->size, val.c_str(), val.size())) {
        return IMAGE_COPY_FAILED;
    }
    return IMAGE_SUCCESS;
}

MIDK_EXPORT
Image_ErrorCode OH_PictureMetadata_SetProperty(OH_PictureMetadata *metadata, Image_String *key, Image_String *value)
{
    if (metadata == nullptr || key == nullptr || key->data == nullptr || key->size == 0 ||
        value == nullptr || value->data == nullptr || value->data == nullptr) {
        return IMAGE_BAD_PARAMETER;
    }
    std::string keyString(key->data, key->size);
    std::string valueString(value->data, value->size);
    if (keyString.empty() || valueString.empty()) {
        return IMAGE_BAD_PARAMETER;
    }
    bool isSucc = metadata->GetInnerAuxiliaryMetadata()->SetValue(keyString,valueString);
    if (isSucc != true) {
        return IMAGE_BAD_PARAMETER;
    }
    return IMAGE_SUCCESS;
}

MIDK_EXPORT
Image_ErrorCode OH_PictureMetadata_Release(OH_PictureMetadata *metadata)
{
    if (metadata) {
        delete metadata;
    }
    return IMAGE_SUCCESS;
}
#ifdef __cplusplus
};
#endif