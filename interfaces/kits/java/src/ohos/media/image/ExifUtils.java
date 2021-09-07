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
import ohos.utils.system.SystemCap;
import ohos.utils.system.SystemCapability;

/**
 * Provides methods to obtain a combination of Exchangeable Image File Format (Exif) properties, such as the longitude,
 * latitude, and altitude.
 *
 * @Syscap {@link SystemCapability.Multimedia#IMAGE}
 * @since 4
 */
@SystemCap("SystemCapability.Multimedia.Image")
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
     * @since 4
     */
    public static Pair<Float, Float> getLatLong(ImageSource imageSource) {
        if (imageSource == null) {
            throw new IllegalArgumentException("imageSource is null");
        }

        String latitudeValue = imageSource.getImagePropertyString(PropertyKey.Exif.GPS_LATITUDE);
        String latitudeRef = imageSource.getImagePropertyString(PropertyKey.Exif.GPS_LATITUDE_REF);
        String longitudeValue = imageSource.getImagePropertyString(PropertyKey.Exif.GPS_LONGITUDE);
        String longitudeRef = imageSource.getImagePropertyString(PropertyKey.Exif.GPS_LONGITUDE_REF);
        if (latitudeValue != null && latitudeRef != null && longitudeValue != null && longitudeRef != null) {
            try {
                float latitude = convertRationalToFloat(latitudeValue, latitudeRef);
                float longitude = convertRationalToFloat(longitudeValue, longitudeRef);
                return Pair.create(latitude, longitude);
            } catch (IllegalArgumentException e) {
                return null;
            }
        }

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
     * @since 4
     */
    public static double getAltitude(ImageSource imageSource, double defaultValue) {
        if (imageSource == null) {
            throw new IllegalArgumentException("imageSource is null");
        }

        double altitude = imageSource.getImagePropertyDouble(PropertyKey.Exif.GPS_ALTITUDE, DEFAULT_VALUE);
        int ref = imageSource.getImagePropertyInt(PropertyKey.Exif.GPS_ALTITUDE_REF, DEFAULT_VALUE);

        if (altitude >= 0 && ref >= 0) {
            return altitude * ((ref == 1) ? DEFAULT_VALUE : 1);
        } else {
            return defaultValue;
        }
    }

    private static double checkDivisor(double num1, double num2) {
        if (Math.abs(num2 - 0.0f) < 0.000001f) {
            throw new NumberFormatException();
        }
        return num1 / num2;
    }

    private static float convertRationalToFloat(String rationalNum, String ref) {
        try {
            String[] components = rationalNum.split(",");
            if (components.length < 3) {
                throw new IllegalArgumentException("parameter is invalid");
            }

            String[] rationalParts;
            rationalParts = components[0].split(RATIONAL_SEPARATOR);
            double degrees = checkDivisor(Double.parseDouble(rationalParts[0].trim()),
                Double.parseDouble(rationalParts[1].trim()));

            rationalParts = components[1].split(RATIONAL_SEPARATOR);
            double minutes = checkDivisor(Double.parseDouble(rationalParts[0].trim()),
                Double.parseDouble(rationalParts[1].trim()));

            rationalParts = components[2].split(RATIONAL_SEPARATOR);
            double seconds = checkDivisor(Double.parseDouble(rationalParts[0].trim()),
                Double.parseDouble(rationalParts[1].trim()));

            double result = degrees + (minutes / MINUTES) + (seconds / SECONDS);
            if ("S".equals(ref) || "W".equals(ref)) {
                return (float) -result;
            }
            return (float) result;
        } catch (NumberFormatException | ArrayIndexOutOfBoundsException e) {
            throw new IllegalArgumentException();
        }
    }
}
