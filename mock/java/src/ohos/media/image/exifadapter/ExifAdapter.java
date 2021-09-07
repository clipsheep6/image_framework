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

package ohos.media.image.exifadapter;

import java.io.ByteArrayInputStream;
import java.io.File;
import java.io.FileDescriptor;
import java.io.IOException;
import java.io.InputStream;

/**
 * exif adapter for ImageSource.
 *
 * @since 3
 */
public class ExifAdapter {
    private static final int INT_RETURN = -1;

    private static final String STRING_RETURN = "-1";

    private static final byte[] BYTE_RETURN = new byte[1];

    private static final long[] LONG_RETURN = new long[2];

    private static final double DOUBLE_RETURN = 1.11;

    /**
     * Instantiates a new Exif adapter.
     *
     * @param pathName the path name
     * @since 3
     */
    public ExifAdapter(String pathName) {
    }

    /**
     * Instantiates a new Exif adapter.
     *
     * @param inputStream the input stream
     * @since 3
     */
    public ExifAdapter(InputStream inputStream) {
    }

    /**
     * Instantiates a new Exif adapter.
     *
     * @param data the data
     * @param offset the offset
     * @param length the length
     * @since 3
     */
    public ExifAdapter(byte[] data, int offset, int length) {
    }

    /**
     * Instantiates a new Exif adapter.
     *
     * @param file the input file
     * @since 3
     */
    public ExifAdapter(File file) {
    }

    /**
     * Instantiates a new Exif adapter.
     *
     * @param fd the input fd
     * @since 3
     */
    public ExifAdapter(FileDescriptor fd) {
    }

    /**
     * Gets image property.
     *
     * @param key the key
     * @return the image property
     * @since 3
     */
    public String getImagePropertyString(String key) {
        return STRING_RETURN;
    }

    /**
     * Gets image property int.
     *
     * @param key the key
     * @param defaultValue the default value
     * @return the image property int
     * @since 3
     */
    public int getImagePropertyInt(String key, int defaultValue) {
        return INT_RETURN;
    }

    /**
     * Get thumbnail bytes byte.
     *
     * @return the byte
     * @since 3
     */
    public byte[] getThumbnailBytes() {
        return BYTE_RETURN;
    }

    /**
     * Get thumbnail byte.
     *
     * @return the byte
     * @since 3
     */
    public byte[] getThumbnail() {
        return BYTE_RETURN;
    }

    /**
     * Get thumbnail offset and length.
     *
     * @return the long array.
     * @since 5
     */
    public long[] getThumbnailRange() {
        return LONG_RETURN;
    }

    /**
     * Gets image property double.
     *
     * @param key the key
     * @param defaultValue the default value
     * @return the image property double
     */
    public double getImagePropertyDouble(String key, double defaultValue) {
        return DOUBLE_RETURN;
    }

    /**
     * set image property.
     *
     * @param key the property key
     * @param property value
     * @since 3
     */
    public void setImageProperty(String key, String value) {
        return;
    }

    /**
     * Save the setImageProperty property data into the original image file
     *
     * @throws IOException the io exception
     * @since 3
     */
    public void saveAttributes() {
        return;
    }
}
