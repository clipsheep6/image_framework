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

import java.util.Objects;

/**
 * Provides image size information, includes the width and height.
 *
 * @Syscap {@link SystemCapability.Multimedia#IMAGE}
 * @since 1
 */
@SystemCap("SystemCapability.Multimedia.Image")
public class Size {
    /**
     * Indicates the image width.
     *
     * @since 1
     */
    public int width;

    /**
     * Indicates the image height.
     *
     * @since 1
     */
    public int height;

    /**
     * A constructor used to create a {@link Size} instance.
     *
     * <p>The width and height of an image are set to 0.
     *
     * @since 1
     */
    public Size() {
    }

    /**
     * A constructor used to create a {@link Size} instance based on a specified width and height.
     *
     * @param width Indicates the image width.
     * @param height Indicates the image height.
     * @since 1
     */
    public Size(int width, int height) {
        this.width = width;
        this.height = height;
    }

    /**
     * Checks whether the {@link Size} object is the same as the one
     * passed through the input parameter.
     *
     * <p>This method overrides the {@code equals} method.
     *
     * @param obj Indicates the {@link Size} object.
     * @return Returns {@code true} if the {@link Size} objects are the same;
     * returns {@code false} otherwise.
     * @since 1
     */
    @Override
    public boolean equals(Object obj) {
        if (this == obj) {
            return true;
        }
        if (obj == null || getClass() != obj.getClass()) {
            return false;
        }
        if (obj instanceof Size) {
            Size size = (Size) obj;
            return width == size.width && height == size.height;
        }
        return false;
    }

    /**
     * Generates a hash code value based on the {@link Size} object.
     *
     * @return Returns the hash code value.
     * @since 1
     */
    @Override
    public int hashCode() {
        return Objects.hash(width, height);
    }

    /**
     * Obtains the string representation of the {@link Size} object.
     *
     * @return Returns the string representation.
     * @since 1
     */
    @Override
    public String toString() {
        return "Size{" + "width=" + width + ", height=" + height + '}';
    }

}
