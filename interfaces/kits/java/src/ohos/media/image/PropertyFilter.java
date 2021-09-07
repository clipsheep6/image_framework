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

import ohos.media.image.common.Filter;
import ohos.media.image.exifadapter.ExifAdapter;
import ohos.media.utils.log.Logger;
import ohos.media.utils.log.LoggerFactory;
import ohos.utils.system.SystemCap;
import ohos.utils.system.SystemCapability;

import java.io.IOException;
import java.util.concurrent.ConcurrentHashMap;
import java.util.HashMap;
import java.util.Map;

/**
 * Provides methods for setting image Exif properties, including adding, deleting, and modifying Exif properties.
 *
 * @Syscap {@link SystemCapability.Multimedia#IMAGE}
 * @since 4
 */
@SystemCap("SystemCapability.Multimedia.Image")
public class PropertyFilter extends Filter {
    private static final Logger LOGGER = LoggerFactory.getImageLogger(PropertyFilter.class);
    private ExifAdapter mExifAdapter;
    private ConcurrentHashMap<String, String> mProperties = new ConcurrentHashMap<>();
    private String nullKeyTag = "NULL_KEY";
    private boolean mRestored = false;

    /**
     * Sets an Exif property of the int type.
     *
     * @param key Indicates the property key, which is defined in {@link ohos.media.image.common.PropertyKey}.
     * @param value Indicates the property value.
     * @return Returns this {@code PropertyFilter} object with the Exif property of the int type. This object can
     * be used for subsequent Exif-related operations.
     * @since 4
     */
    public PropertyFilter setPropertyInt(String key, int value) {
        if (key == null) {
            mProperties.put(nullKeyTag, nullKeyTag);
            LOGGER.error("invalid input key of setPropertyInt.");
            return this;
        }
        mProperties.put(key, String.valueOf(value));
        return this;
    }

    /**
     * Sets an Exif property of the double type.
     *
     * @param key Indicates the property key, which is defined in {@link ohos.media.image.common.PropertyKey}.
     * @param value Indicates the property value.
     * @return Returns this {@code PropertyFilter} object with the Exif property of the double type. The object
     * can be used for subsequent Exif-related operations.
     * @since 4
     */
    public PropertyFilter setPropertyDouble(String key, double value) {
        if (key == null) {
            mProperties.put(nullKeyTag, nullKeyTag);
            LOGGER.error("invalid input key of setPropertyDouble.");
            return this;
        }

        mProperties.put(key, String.valueOf(value));
        return this;
    }

    /**
     * Sets an Exif property of the string type.
     *
     * @param key Indicates the property key, which is defined in {@link ohos.media.image.common.PropertyKey}.
     * @param value Indicates the property value.
     * @return Returns this {@code PropertyFilter} object with the Exif property of the string type. The object
     * can be used for subsequent Exif-related operations.
     * @since 4
     */
    public PropertyFilter setPropertyString(String key, String value) {
        if (key == null) {
            mProperties.put(nullKeyTag, nullKeyTag);
            LOGGER.error("invalid input key of setPropertyString.");
            return this;
        }
        mProperties.put(key, value);
        return this;
    }

    /**
     * Restores a specified Exif property. This method applies only to the properties that have not been applied
     * to the source file via {@code applyToSource(ImageSource)}.
     *
     * @param key Indicates the property key, which is defined in {@link ohos.media.image.common.PropertyKey}.
     * @return Returns this {@code PropertyFilter} object with the restored Exif property. The object can be used
     * for subsequent Exif-related operations.
     * @since 4
     */
    public PropertyFilter rollbackProperty(String key) {
        if (key == null) {
            mProperties.remove(nullKeyTag);
            return this;
        }
        mProperties.remove(key);
        return this;
    }

    /**
     * Restores all Exif properties. This method applies only to the properties that have not been applied to the
     * source file via {@code applyToSource(ImageSource)}.
     *
     * @return Returns this {@code PropertyFilter} object with the restored Exif properties. The object can be used
     * for subsequent Exif-related operations.
     * @since 4
     */
    @Override
    public PropertyFilter restore() {
        mProperties.clear();
        mRestored = true;
        return this;
    }

    /**
     * Applies the configured Exif properties to the source file.
     *
     * <p>This method requires the permission to modify the source file represented by the {@code ImageSource}
     * object. The configured Exif properties do not take effect until this method is called.
     * <p>This method is applicable to the {@code ImageSource} object that has been created
     * based on a {@code File} object or a specified file path.
     *
     * @param source Indicates the image source file to which the properties will be applied.
     * @return Returns the size (in bytes) of the image file edited based on the Exif properties if the operation
     * is successful; returns {@code -1} otherwise.
     * @throws IOException Throws this exception if the modification permission has not been obtained.
     * @throws IllegalArgumentException Throws this exception if the image source is empty.
     * @throws IllegalStateException Throws this exception if the {@code ImageSource} object has been released.
     * @since 4
     */
    @Override
    public long applyToSource(ImageSource source) throws IOException {
        if (source == null) {
            throw new IllegalArgumentException("invalid input ImageSource null value of applyToSource.");
        }
        if (source.isReleased()) {
            throw new IllegalStateException("invalid input ImageSource status of applyToSource.");
        }
        if (mProperties.size() == 1 && mProperties.containsKey(nullKeyTag)) {
            LOGGER.error("invalid key set operation");
            return -1;
        }
        mProperties.remove(nullKeyTag);
        if (mProperties.size() == 0) {
            if (mRestored) {
                return -1;
            }
            return source.getFileSize();
        }
        mExifAdapter = source.getExifAdapterInstance();
        int setFailed = 0;
        for (Map.Entry<String, String> oneProperty : mProperties.entrySet()) {
            mExifAdapter.setImageProperty(oneProperty.getKey(), oneProperty.getValue());
            // check all properties
            String readValue = mExifAdapter.getImagePropertyString(oneProperty.getKey());
            if (readValue == null || !readValue.equals(oneProperty.getValue())) {
                setFailed++;
            }
        }
        if (setFailed == mProperties.size()) {
            LOGGER.error("all property set failed");
            return -1;
        }
        mExifAdapter.saveAttributes();

        ImageSource newSource = source.updateImageSource();
        if (newSource == null) {
            LOGGER.error("update source failed");
            return -1;
        }
        return newSource.getFileSize();
    }
}

