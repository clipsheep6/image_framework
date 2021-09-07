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
 * Describes rectangle areas of images.
 *
 * <p>A rectangle area is represented by the abscissa and ordinate of the upper-left point, width and height.
 *
 * @Syscap {@link SystemCapability.Multimedia#IMAGE}
 * @since 1
 */
@SystemCap("SystemCapability.Multimedia.Image")
public class Rect {
    /**
     * Indicates the minimum abscissa value of a rectangle, which is the abscissa of the upper left point.
     *
     * @since 1
     */
    public int minX;

    /**
     * Indicates the minimum ordinate value of a rectangle, which is the ordinate of the upper left point.
     *
     * @since 1
     */
    public int minY;

    /**
     * Indicates the width of a rectangle.
     *
     * @since 1
     */
    public int width;

    /**
     * Indicates the height of a rectangle.
     *
     * @since 1
     */
    public int height;

    /**
     * A constructor used to create a {@link Rect} instance,
     * with the abscissa and coordinate of the upper-left point, width, and height set to {@code 0}.
     *
     * @since 1
     */
    public Rect() {
    }

    /**
     * A constructor used to create a {@link Rect} instance.
     *
     * <p>The abscissa and coordinate of the upper-left point,
     * and the width and height of the rectangle are used as input parameters.
     *
     * @since 1
     */
    public Rect(int minX, int minY, int width, int height) {
        this.minX = minX;
        this.minY = minY;
        this.width = width;
        this.height = height;
    }

    /**
     * A constructor used to create a {@link Rect} instance with another one as the input parameter.
     *
     * <p>If the input parameter is null, this method creates an empty {@link Rect} object
     * with the abscissa and coordinate of the upper-left point, width, and height set to {@code 0}.
     *
     * @since 1
     */
    public Rect(Rect rect) {
        if (rect == null) {
            minX = 0;
            minY = 0;
            width = 0;
            height = 0;
        } else {
            minX = rect.minX;
            minY = rect.minY;
            width = rect.width;
            height = rect.height;
        }
    }

    /**
     * Obtains the string representation of the {@link Rect} object.
     *
     * @return Returns the string representation.
     * @since 1
     */
    @Override
    public String toString() {
        return "Rect{" + "minX=" + minX + ", minY=" + minY + ", width=" + width + ", height=" + height + '}';
    }

    /**
     * Checks whether the {@link Rect} object is the same as the one
     * passed through the input parameter.
     *
     * <p>This method overrides the {@code equals} method.
     *
     * @param obj Indicates the {@link Rect} object.
     * @return Returns {@code true} if the {@link Rect} objects are the same;
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
        if (obj instanceof Rect) {
            Rect rect = (Rect) obj;
            return minX == rect.minX && minY == rect.minY && width == rect.width && height == rect.height;
        }
        return false;
    }

    /**
     * Generates a hash code value based on the {@link Rect} object.
     *
     * @return Returns the hash code value.
     * @since 1
     */
    @Override
    public int hashCode() {
        return Objects.hash(minX, minY, width, height);
    }

    /**
     * Sets the abscissa, coordinate, width, and height to {@code 0}.
     * <p>
     * Sets the abscissa and coordinate of the upper-left point, width, and height to {@code 0}.
     *
     * @since 1
     */
    public void setEmpty() {
        minX = 0;
        minY = 0;
        width = 0;
        height = 0;
    }

    /**
     * Crops a rectangle area.
     *
     * <p>The intersection area of the original rectangle and the input rectangle is cropped to
     * adjust the original rectangle area.
     *
     * @param minX Indicates the upper-left point abscissa of the input rectangle.
     * @param minY Indicates the upper-left point ordinate of the input rectangle.
     * @param width Indicates the width of the input rectangle.
     * @param height Indicates the height of the input rectangle.
     * @return Returns {@code true} if the original rectangle area is cropped; returns {@code false} otherwise.
     * @since 1
     */
    public boolean cropRect(int minX, int minY, int width, int height) {
        if ((this.minX < minX + width) && (minX < this.minX + this.width) && (this.minY < minY + height) && (minY
            < this.minY + this.height)) {
            if (this.minX < minX + width) {
                this.minX = minX;
            }
            if (this.minY < minY + height) {
                this.minY = minY;
            }
            if ((this.minX + this.width) > (minX + width)) {
                this.width = width;
            }
            if ((this.minY + this.height) > (minY + height)) {
                this.height = height;
            }
            return true;
        }
        return false;
    }
}
