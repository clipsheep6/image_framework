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

import java.util.Locale;

/**
 * Describes basic image information, including image dimensions, pixel formats,
 * and color space.
 *
 * @Syscap {@link SystemCapability.Multimedia#IMAGE}
 * @since 1
 */
@SystemCap("SystemCapability.Multimedia.Image")
public class ImageInfo {
    /**
     * Indicates image dimensions specified by a {@link Size} object.
     *
     * @since 1
     */
    public Size size;

    /**
     * Indicates a pixel format specified by a {@link PixelFormat} enum.
     *
     * @since 1
     */
    public PixelFormat pixelFormat;

    /**
     * Indicates the color space specified by a {@link ColorSpace} enum.
     *
     * @since 1
     */
    public ColorSpace colorSpace;

    /**
     * Indicates the alpha type specified by an {@link AlphaType} enum.
     *
     * @since 3
     */
    public AlphaType alphaType;

    /**
     * Default constructor used to create a {@link ImageInfo} instance.
     *
     * @since 1
     */
    public ImageInfo() {
    }

    /**
     * ImageInfo as string.
     *
     * @since 5
     */
    @Override
    public String toString() {
        return ((size == null) ? "size:UNKNOWN" : size.toString())
            + ", pixelFormat:" + pixelFormat
            + ", colorSpace:" + colorSpace
            + ", alphaType:" + alphaType;
    }

    /**
     * Called by JNI
     */
    private void setPixelFormat(int pixelFormat) {
        for (PixelFormat item : PixelFormat.values()) {
            if (item.getValue() == pixelFormat) {
                this.pixelFormat = item;
                break;
            }
        }
    }

    /**
     * Called by JNI
     */
    private void setColorSpace(int colorSpace) {
        for (ColorSpace item : ColorSpace.values()) {
            if (item.getValue() == colorSpace) {
                this.colorSpace = item;
                break;
            }
        }
    }

    /**
     * Called by JNI
     */
    private void setAlphaType(int alphaType) {
        for (AlphaType item : AlphaType.values()) {
            if (item.getValue() == alphaType) {
                this.alphaType = item;
                break;
            }
        }
    }
}
