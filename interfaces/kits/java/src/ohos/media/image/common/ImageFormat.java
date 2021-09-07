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

import java.util.HashMap;
import java.util.Map;

/**
 * Defines the image format and provides interfaces for obtaining image format information.
 *
 * <p>This class defines how image data is organized and represented, and provides interfaces for obtaining
 * the number of image format components and bits per pixel adopted by a format.
 *
 * @Syscap {@link SystemCapability.Multimedia#IMAGE}
 * @since 1
 */
@SystemCap("SystemCapability.Multimedia.Image")
public final class ImageFormat {
    /**
     * Indicates an unknown encoding format.
     *
     * @since 1
     */
    public static final int UNKNOWN = 0;

    /**
     * Indicates the YUV420 format with NV21 encoding.
     *
     * @since 1
     */
    public static final int NV21 = 1;

    /**
     * Indicates the generic YUV420 format with 8 bits per component (Y, U, and V).
     *
     * @since 1
     */
    public static final int YUV420_888 = 2;

    /**
     * Indicates the JPEG encoding format.
     *
     * @since 1
     */
    public static final int JPEG = 3;

    /**
     * Indicates the RAW10 format with 10 bits per component.
     *
     * @since 1
     */
    public static final int RAW10 = 4;

    /**
     * Indicates the RAW10 format with 16 bits per component.
     *
     * @since 1
     */
    public static final int RAW16 = 5;

    /**
     * Indicates the H264 encoding format.
     *
     * @since 5
     */
    public static final int H264 = 6;

    /**
     * Indicates the H265 encoding format.
     *
     * @since 5
     */
    public static final int H265 = 7;

    /**
     * Describes the component type of encoding formats.
     *
     * <p>This enum defines the meaning and form of the components, which represent the structure model of image data.
     *
     * @since 1
     */
    public enum ComponentType {
        /**
         * Indicates the Y component in the YUV format.
         */
        YUV_Y(1),

        /**
         * Indicates the U component in the YUV format.
         */
        YUV_U(2),

        /**
         * Indicates the V component in the YUV format.
         */
        YUV_V(3),

        /**
         * Indicates the JPEG format, which has no component.
         */
        JPEG(4),

        /**
         * Indicates the RAW10 format, which has no component.
         */
        RAW10(5),

        /**
         * Indicates the RAW16 format, which has no component.
         */
        RAW16(6),

        /**
         * Indicates the H264 format, which has no component.
         */
        H264(7),

        /**
         * Indicates the H265 format, which has no component.
         */
        H265(8);

        private final int value;

        ComponentType(int value) {
            this.value = value;
        }

        /**
         * Obtains a {@link ComponentType} enum name based on its enum value.
         *
         * @param value Indicates the enum value.
         * @return Returns the enum name if any; returns {@code null} otherwise.
         */
        public static ComponentType valueOf(int value) {
            for (ComponentType componentType : values()) {
                if (componentType.value == value) {
                    return componentType;
                }
            }
            return null;
        }
    }

    // format mapping.
    private static final Map<Integer, Integer> FORMAT_MAPPING = new HashMap<>();

    private static final int BITS_SHIFT = 8;

    private static final int SHIFT_MASK = 0xFF;

    // format pixel bits num
    private static final int BITS_12 = 12;

    private static final int BITS_10 = 10;

    private static final int BITS_16 = 16;

    // format component num
    private static final int COMPONENT_3 = 3;

    private static final int COMPONENT_2 = 2;

    private static final int COMPONENT_1 = 1;

    static {
        FORMAT_MAPPING.put(NV21, BITS_12 << BITS_SHIFT | COMPONENT_3);
        FORMAT_MAPPING.put(YUV420_888, BITS_12 << BITS_SHIFT | COMPONENT_3);
        FORMAT_MAPPING.put(RAW10, BITS_10 << BITS_SHIFT | COMPONENT_1);
        FORMAT_MAPPING.put(RAW16, BITS_16 << BITS_SHIFT | COMPONENT_1);
        FORMAT_MAPPING.put(JPEG, COMPONENT_1);
        FORMAT_MAPPING.put(H264, COMPONENT_1);
        FORMAT_MAPPING.put(H265, COMPONENT_1);
    }

    private ImageFormat() {
    }

    /**
     * Obtains the number of bits per pixel of the specified image format.
     *
     * @param format Indicates an image format.
     * @return Returns the number of bits per pixel.
     * @since 1
     */
    public static int getBitsNumberPerPixel(int format) {
        return FORMAT_MAPPING.getOrDefault(format, 0) >> BITS_SHIFT & SHIFT_MASK;
    }

    /**
     * Obtains the number of components of a specified image format.
     *
     * @param format Indicates an image format.
     * @return Returns the number of components.
     * @since 1
     */
    public static int getComponentNumber(int format) {
        return FORMAT_MAPPING.getOrDefault(format, 0) & SHIFT_MASK;
    }
}
