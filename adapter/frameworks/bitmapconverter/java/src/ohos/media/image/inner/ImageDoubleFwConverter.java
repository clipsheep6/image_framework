/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
 * Copyright (C) 2006 The Android Open Source Project
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

package ohos.media.image.inner;

import android.graphics.Bitmap;

import ohos.utils.Parcel;
import ohos.rpc.MessageParcel;
import ohos.media.image.PixelMap;
import ohos.media.utils.log.Logger;
import ohos.media.utils.log.LoggerFactory;

/**
 * Bitmap and PixelMap Converter
 *
 * @since 1
 */
public class ImageDoubleFwConverter {
    private static final Logger LOGGER = LoggerFactory.getImageLogger(ImageDoubleFwConverter.class);

    static {
        LOGGER.debug("Begin loading image_converter_jni library");
        try {
            System.loadLibrary("image_converter_jni.z");
        } catch (UnsatisfiedLinkError | NullPointerException e) {
            LOGGER.error("loadLibrary image_converter_jni.z fail");
        }
    }

    /**
     * convert PixelMap to Bitmap
     *
     * @param pixelMap create form source image
     * @return android Bitmap on success, otherwise return null
     * @since 1
     */
    public static Bitmap createShadowBitmap(PixelMap pixelMap) {
        if (pixelMap == null) {
            LOGGER.error("createShadowBitmap pixelMap is null");
            return null;
        }

        return nativeCreateBitmap(pixelMap);
    }

    /**
     * Convert Bitmap to PixelMap
     *
     * @param bitmap android's Bitmap object
     * @return PixelMap on success, otherwise return null
     * @since 1
     */
    public static PixelMap createShellPixelMap(Bitmap bitmap) {
        if (bitmap == null) {
            LOGGER.error("createShellPixelMap bitmap is null");
            return null;
        }

        return nativeCreatePixelMap(bitmap);
    }

    /**
     * Write PixelMap to Parcel
     *
     * @param pixelMap source image
     * @param parcel ohos parcel object
     * @since 3
     */
    public static void writeToParcel(PixelMap pixelMap, Parcel parcel) {
        if (pixelMap == null) {
            LOGGER.error("writeToParcel pixelMap is null");
            return;
        }

        if (parcel instanceof MessageParcel) {
            nativeWriteToParcel(pixelMap, parcel);
        } else {
            LOGGER.error("writeToParcel not a MessageParcel");
        }
    }

    private static native Bitmap nativeCreateBitmap(PixelMap pixelMap);

    private static native PixelMap nativeCreatePixelMap(Bitmap bitmap);

    private static native void nativeWriteToParcel(PixelMap pixelMap, Parcel parcel);
}
