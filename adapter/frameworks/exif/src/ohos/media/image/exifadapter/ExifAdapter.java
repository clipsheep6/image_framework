/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
 * Copyright (C) 2007 The Android Open Source Project
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

import android.media.ExifInterface;

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
    private final ExifInterface exifInterface;

    /**
     * Instantiates a new Exif adapter.
     *
     * @param pathName the path name
     * @throws IOException the io exception
     * @since 3
     */
    public ExifAdapter(String pathName) throws IOException {
        exifInterface = new ExifInterface(pathName);
    }

    /**
     * Instantiates a new Exif adapter.
     *
     * @param inputStream the input stream
     * @throws IOException the io exception
     * @since 3
     */
    public ExifAdapter(InputStream inputStream) throws IOException {
        exifInterface = new ExifInterface(inputStream);
    }

    /**
     * Instantiates a new Exif adapter.
     *
     * @param data the data
     * @param offset the offset
     * @param length the length
     * @throws IOException the io exception
     * @since 3
     */
    public ExifAdapter(byte[] data, int offset, int length) throws IOException {
        exifInterface = new ExifInterface(new ByteArrayInputStream(data, offset, length));
    }

    /**
     * Instantiates a new Exif adapter.
     *
     * @param file the input file
     * @throws IOException the io exception
     * @since 3
     */
    public ExifAdapter(File file) throws IOException {
        exifInterface = new ExifInterface(file);
    }

    /**
     * Instantiates a new Exif adapter.
     *
     * @param fd the input fd
     * @throws IOException the io exception
     * @since 3
     */
    public ExifAdapter(FileDescriptor fd) throws IOException {
        exifInterface = new ExifInterface(fd);
    }

    /**
     * Gets image property.
     *
     * @param key the key
     * @return the image property
     * @since 3
     */
    public String getImagePropertyString(String key) {
        return exifInterface.getAttribute(key);
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
        return exifInterface.getAttributeInt(key, defaultValue);
    }

    /**
     * Get thumbnail bytes byte.
     *
     * @return the byte
     * @since 3
     */
    public byte[] getThumbnailBytes() {
        return exifInterface.getThumbnailBytes();
    }

    /**
     * Get thumbnail byte.
     *
     * @return the byte
     * @since 3
     */
    public byte[] getThumbnail() {
        return exifInterface.getThumbnail();
    }

    /**
     * Get thumbnail offset and length.
     *
     * @return two-element array, the offset in the first value, and length in the
     *         second, or {@code null} if no thumbnail was found or the thumbnail
     *         strips are not placed consecutively.
     * @since 6
     */
    public long[] getThumbnailRange() {
        return exifInterface.getThumbnailRange();
    }

    /**
     * Gets image property double.
     *
     * @param key the key
     * @param defaultValue the default value
     * @return the image property double
     */
    public double getImagePropertyDouble(String key, double defaultValue) {
        return exifInterface.getAttributeDouble(key, defaultValue);
    }

    /**
     * set image property.
     *
     * @param key the property key
     * @param property value
     * @since 3
     */
    public void setImageProperty(String key, String value) {
        exifInterface.setAttribute(key, value);
    }

    /**
     * Save the setImageProperty property data into the original image file
     *
     * @throws IOException the io exception
     * @since 3
     */
    public void saveAttributes() throws IOException {
        try {
            exifInterface.saveAttributes();
        } catch (IOException e) {
            throw new IOException("only supports JPEG format for changing and saving Exif properties");
        }
    }
}
