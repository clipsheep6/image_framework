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
 * Describes location information of image pixels in a two-dimensional coordinate system.
 *
 * <p>The location information includes abscissa and ordinate values of pixels.
 *
 * @Syscap {@link SystemCapability.Multimedia#IMAGE}
 * @since 1
 */
@SystemCap("SystemCapability.Multimedia.Image")
public class Position {
    /**
     * Indicates the abscissa of a pixel in the coordinate system.
     *
     * @since 1
     */
    public int posX;

    /**
     * Indicates the ordinate of a pixel in the coordinate system.
     *
     * @since 1
     */
    public int posY;

    /**
     * A constructor used to create a {@link Position} instance.
     *
     * <p>Abscissa and ordinate values of the image pixel are set to 0.
     *
     * @since 1
     */
    public Position() {
    }

    /**
     * A constructor used to create a {@link Position} instance based on specified abscissa and ordinate values.
     *
     * @since 1
     */
    public Position(int px, int py) {
        posX = px;
        posY = py;
    }

    /**
     * Obtains the string representation of the {@link Position} object.
     *
     * @return Returns the string representation.
     * @since 1
     */
    @Override
    public String toString() {
        return "Position{" + "posX=" + posX + ", posY=" + posY + '}';
    }

    /**
     * Checks whether the {@link Position} object is the same as the one
     * passed through the input parameter.
     *
     * <p>This method overrides the {@code equals} method.
     *
     * @param obj Indicates the {@link Position} object.
     * @return Returns {@code true} if the {@link Position} objects are the same;
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
        if (obj instanceof Position) {
            Position position = (Position) obj;
            return posX == position.posX && posY == position.posY;
        }
        return false;
    }

    /**
     * Generates a hash code value based on the {@link Position} object.
     *
     * @return Returns the hash code value.
     * @since 1
     */
    @Override
    public int hashCode() {
        return Objects.hash(posX, posY);
    }
}
