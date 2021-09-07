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
 * Enumerates memory use preferences for image decoding.
 *
 * @Syscap {@link SystemCapability.Multimedia#IMAGE}
 * @since 4
 */
@SystemCap("SystemCapability.Multimedia.Image")
public enum MemoryUsagePreference {
    /**
     * Indicates the default preference, which preferentially uses the provided decoding parameters.
     */
    DEFAULT(0),

    /**
     * Indicates low memory usage, which saves memory by sacrificing the image quality.
     * For example, an image format with an opaque alpha channel can be
     * decoded to {@link ohos.media.image.common.PixelFormat#RGB_565}.
     */
    LOW_RAM(1);

    private final int memoryUsagePreference;

    MemoryUsagePreference(int value) {
        this.memoryUsagePreference = value;
    }

    /**
     * Obtains the value of this enum.
     *
     * @return Returns the enum value.
     * @since 3
     */
    public int getValue() {
        return memoryUsagePreference;
    }
}
