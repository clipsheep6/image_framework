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
 * Enumerates the memory allocation type for decoded image data.
 *
 * @Syscap {@link SystemCapability.Multimedia#IMAGE}
 * @since 4
 */
@SystemCap("SystemCapability.Multimedia.Image")
public enum AllocatorType {
    /**
     * Indicates that the system automatically selects the most appropriate storage allocation.
     */
    DEFAULT(0),

    /**
     * Indicates that heap memory is allocated to stored decoded image data.
     */
    HEAP(1),

    /**
     * Indicates that shared memory is allocated to stored decoded image data.
     */
    SHARED_MEMORY(2);

    private final int typeValue;

    AllocatorType(int value) {
        this.typeValue = value;
    }

    /**
     * Obtains the value of this enum.
     *
     * @return Returns the enum value.
     * @since 4
     */
    public int getValue() {
        return typeValue;
    }
}
