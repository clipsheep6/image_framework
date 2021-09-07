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

package ohos.media.image;

import ohos.media.image.common.PropertyKey;
import ohos.media.utils.log.Logger;
import ohos.media.utils.log.LoggerFactory;
import ohos.utils.Pair;

import java.util.Optional;

/**
 * Provides methods to obtain a combination of Exchangeable Image File Format (Exif) properties, such as the longitude,
 * latitude, and altitude.
 *
 * @since 3
 */
public class ExifUtils {
    private static final Logger LOGGER = LoggerFactory.getImageLogger(ExifUtils.class);

    private static final double MINUTES = 60.0;

    private static final double SECONDS = 3600.0;

    private static final int DEFAULT_VALUE = -1;

    private static final String RATIONAL_SEPARATOR = "/";

    /**
     * Obtains longitude and latitude information about the image in its Exif property.
     *
     * @param imageSource Indicates the image data source.
     * @return Returns the {@code Pair} object containing the longitude and latitude information if the information
     * exists; returns {@code null} otherwise. The {@code f} member of {@code Pair} indicates the latitude, and {@code
     * s} indicates the longitude.
     * @throws IllegalStateException Throws this exception if native resources associated with {@code imageSource} have
     * been released.
     * @throws IllegalArgumentException Throws this exception if {@code imageSource} is null.
     * @since 3
     */
    public static Pair<Float, Float> getLatLong(ImageSource imageSource) {
        return null;
    }

    /**
     * Obtains altitude information about the image in its Exif property.
     *
     * @param imageSource Indicates the image data source.
     * @param defaultValue Indicates the custom default value of the property.
     * @return Returns the altitude information if it exists; returns the custom default value otherwise.
     * @throws IllegalStateException Throws this exception if native resources associated with {@code imageSource} have
     * been released.
     * @throws IllegalArgumentException Throws this exception if {@code imageSource} is null.
     * @since 3
     */
    public static double getAltitude(ImageSource imageSource, double defaultValue) {
        return DEFAULT_VALUE;
    }

    private static float convertRationalToFloat(String rationalNum, String ref) {
        return DEFAULT_VALUE;
    }
}
