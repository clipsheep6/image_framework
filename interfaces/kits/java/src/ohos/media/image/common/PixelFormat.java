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
 * Describes image pixel formats.
 *
 * <p>A pixel format of image data describes the components of a pixel and the layout of the components
 * stored in memory. Common pixel formats include ARGB_8888 and RGBA_8888.
 *
 * @Syscap {@link SystemCapability.Multimedia#IMAGE}
 * @since 1
 */
@SystemCap("SystemCapability.Multimedia.Image")
public enum PixelFormat {
    /**
     * Indicates an unknown format.
     */
    UNKNOWN(0),

    /**
     * Indicates that each pixel is stored on 32 bits. Components A, R, G, and B each occupies 8 bits
     * and are stored from the higher-order to the lower-order bits.
     */
    ARGB_8888(1),

    /**
     * Indicates that each pixel is stored on 16 bits. Only the R, G, and B components are encoded
     * from the higher-order to the lower-order bits: red is stored with 5 bits of precision,
     * green is stored with 6 bits of precision, and blue is stored with 5 bits of precision.
     */
    RGB_565(2),

    /**
     * Indicates that each pixel is stored on 32 bits. Components R, G, B, and A each occupies 8 bits
     * and are stored from the higher-order to the lower-order bits.\
     *
     * @hide internal use
     */
    RGBA_8888(3),

    /**
     * Indicates that each pixel is stored on 32 bits. Components B, G, R, and A each occupies 8 bits
     * and are stored from the higher-order to the lower-order bits.
     *
     * @hide internal use
     */
    BGRA_8888(4),

    /**
     * Indicates that each pixel is stored on 8 bit.
     * Only A occupies 8 bits are stored.
     *
     * @hide internal use for now
     * @since 5
     */
    ALPHA_8(6),

    /**
     * Indicates that each pixel is stored on 32 bits. CMYK color format.
     * CMYK format.
     *
     * @hide internal use for now
     * @since 5
     */
    CMYK(10);


    private final int pixelFormatValue;

    PixelFormat(int value) {
        this.pixelFormatValue = value;
    }

    /**
     * Obtains an enum value.
     *
     * @return Returns the enum value.
     * @since 1
     */
    public int getValue() {
        return pixelFormatValue;
    }
}
