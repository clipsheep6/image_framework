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
 * Describes types of image color space.
 *
 * <p>Image color space defines how colors are organized and represented.
 * Common color space standards include SRGB and DISPLAY_P3.
 *
 * @Syscap {@link SystemCapability.Multimedia#IMAGE}
 * @since 1
 */
@SystemCap("SystemCapability.Multimedia.Image")
public enum ColorSpace {
    /**
     * Indicates an unknown color space.
     */
    UNKNOWN(0),

    /**
     * Indicates the color space based on SMPTE RP 431-2-2007 and IEC 61966-2.1:1999.
     */
    DISPLAY_P3(1),

    /**
     * Indicates the standard red green blue (SRGB) color space based on IEC 61966-2.1:1999.
     */
    SRGB(2),

    /**
     * Indicates the SRGB using a linear transfer function based on the IEC 61966-2.1:1999 standard.
     */
    LINEAR_SRGB(3),

    /**
     * Indicates the color space based on IEC 61966-2-2:2003.
     */
    EXTENDED_SRGB(4),

    /**
     * Indicates the color space based on IEC 61966-2-2:2003.
     */
    LINEAR_EXTENDED_SRGB(5),

    /**
     * Indicates the color space based on the standard illuminant with D50 as the white point.
     */
    GENERIC_XYZ(6),

    /**
     * Indicates the color space using CIE XYZ D50 as the profile conversion space.
     */
    GENERIC_LAB(7),

    /**
     * Indicates the color space based on SMPTE ST 2065-1:2012.
     */
    ACES(8),

    /**
     * Indicates the color space based on Academy S-2014-004.
     */
    ACES_CG(9),

    /**
     * Indicates the color space based on Adobe RGB (1998).
     */
    ADOBE_RGB_1998(10),

    /**
     * Indicates the color space based on SMPTE RP 431-2-2007.
     */
    DCI_P3(11),

    /**
     * Indicates the color space based on Rec.ITU-R BT.709-5.
     */
    ITU_709(12),

    /**
     * Indicates the color space based on Rec.ITU-R BT.2020-1.
     */
    ITU_2020(13),

    /**
     * Indicates the color space based on ISO 22028-2:2013.
     */
    ROMM_RGB(14),

    /**
     * Indicates the color space based on the NTSC 1953 standard.
     */
    NTSC_1953(15),

    /**
     * Indicates the color space based on SMPTE C.
     */
    SMPTE_C(16);

    private final int colorValue;

    ColorSpace(int value) {
        this.colorValue = value;
    }

    /**
     * Obtains an enum value.
     *
     * @return Returns the enum value.
     * @since 1
     */
    public int getValue() {
        return colorValue;
    }
}
