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

package ohos.media.image.common;

import ohos.utils.system.SystemCap;
import ohos.utils.system.SystemCapability;

/**
 * Represents the alpha type of an image.
 *
 * @Syscap {@link SystemCapability.Multimedia#IMAGE}
 * @since 3
 */
@SystemCap("SystemCapability.Multimedia.Image")
public enum AlphaType {
    /**
     * Indicates an unknown alpha type.
     */
    UNKNOWN(0),

    /**
     * Indicates that the image format does not include the alpha channel, or the alpha type of all pixels is opaque.
     */
    OPAQUE(1),

    /**
     * Indicates that the color channel of each pixel is premultiplied by the alpha channel value.
     */
    PREMUL(2),

    /**
     * Indicates that the color component of each pixel is independent from the alpha channel and is not
     * premultiplied by the value.
     */
    UNPREMUL(3);

    private final int typeValue;

    AlphaType(int value) {
        this.typeValue = value;
    }

    /**
     * Obtains the enum value of the alpha type.
     *
     * @return Returns the integer enum value.
     * @since 3
     */
    public int getValue() {
        return typeValue;
    }
}
