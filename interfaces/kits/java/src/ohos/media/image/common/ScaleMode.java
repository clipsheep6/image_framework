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
 * Represents the image scaling effect.
 *
 * @Syscap {@link SystemCapability.Multimedia#IMAGE}
 * @since 3
 */
@SystemCap("SystemCapability.Multimedia.Image")
public enum ScaleMode {
    /**
     * Indicates the effect that fits the image into the target size.
     *
     * <p> The image will be stretched when target aspect ratio differs from the original.
     */
    FIT_TARGET_SIZE(0),

    /**
     * Indicates the effect that scales an image to fill the target image area and center-crops
     * the part outside the area.
     *
     * <p>The original image will be scaled up or down at the original aspect ratio to fill the
     * target dimensions, and any additional height or width that does not fit will be cropped
     * out from each side. For example to scale a 200 x 100 image to 500 x 300, the effect scales
     * the image to 600 x 300, centers the image horizontally, and then crops out the extra 50
     * pixels of width from each side.
     */
    CENTER_CROP(1);

    private final int modeValue;

    ScaleMode(int value) {
        this.modeValue = value;
    }

    /**
     * Obtains the enum value of the image scaling effect.
     *
     * @return Returns the integer enum value.
     * @since 3
     */
    public int getValue() {
        return modeValue;
    }
}
