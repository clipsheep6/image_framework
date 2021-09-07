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

import ohos.media.utils.log.Logger;
import ohos.media.utils.log.LoggerFactory;

import java.io.OutputStream;
import java.util.HashSet;

/**
 * Represents the image packer that packs compressed images into files or other objects.
 *
 * <p>You can call {@code create} to create an image packer, {@code initializePacking} to set packing options,
 * {@code addImage} to add image data to be packed, and {@code finalizePacking} to complete packing and output the
 * target object.
 *
 * @since 3
 */
public class ImagePacker {
    private static final Logger LOGGER = LoggerFactory.getImageLogger(ImagePacker.class);

    /**
     * Success state by image packer native interface return.
     */
    private static final int SUCCESS = 0;

    private static final long FAILED = -1L;

    private static final int BUFFER_SIZE = 4096;

    /**
     * Accessed by native methods, save object address
     */
    private long nativeImagePacker;

    /**
     * native used, write tmp buffer
     */
    private byte[] nativeBuffer = new byte[BUFFER_SIZE];

    /**
     * Private constructor called by JNI.
     *
     * @param nativePtr the native object point.
     */
    private ImagePacker(long nativePtr) {
        nativeImagePacker = nativePtr;
    }

    /**
     * Creates an {@code ImagePacker} instance.
     *
     * @return Returns the {@code ImagePacker} instance if created; returns {@code null} if an exception occurs.
     * @since 3
     */
    public static ImagePacker create() {
        return new ImagePacker(FAILED);
    }

    /**
     * Obtains the supported output image formats for packing.
     *
     * @return Returns the set of supported output image formats, which are represented by MIME type strings, for
     * example, {@code image/jpeg}. Currently, only the JPEG format is supported.
     * @since 3
     */
    public static HashSet<String> getSupportedFormats() {
        HashSet<String> hashSet = new HashSet<>();
        return hashSet;
    }

    /**
     * Describes options for compressing and packing.
     *
     * <p>The options include the output file format, compression quality, and other parameters.
     *
     * @since 3
     */
    public static class PackingOptions {
        /**
         * Indicates the output image format represented by an MIME type string. {@code image/jpeg} is supported.
         *
         * @since 3
         */
        public String format = "image/jpeg";

        /**
         * Indicates the image compression quality. The value ranges from 0 to 100. A larger value indicates better
         * image quality but larger space occupied.
         *
         * @since 3
         */
        public int quality = 100;

        /**
         * Indicates the prompt of the number of images to be packed.
         *
         * @since 3
         */
        public int numberHint = 1;
    }

    /**
     * Initializes the packing task that outputs the result to a byte array.
     *
     * @param data Indicates the byte array containing the packed images.
     * @param opts Indicates the packing options.
     * @return Returns {@code true} if the packing task is successfully initialized; returns {@code false} otherwise.
     * @throws IllegalArgumentException Throws this exception if the input parameter is invalid.
     * @since 3
     */
    public boolean initializePacking(byte[] data, PackingOptions opts) {
        return false;
    }

    /**
     * Initializes the packing task that outputs the result to a byte array at a specific offset.
     *
     * @param data Indicates the byte array containing the packed images.
     * @param offset Indicates the data array offset into which the compressed image data will be written.
     * @param opts Indicates the packing options.
     * @return Returns {@code true} if the packing task is successfully initialized; returns {@code false} otherwise.
     * @throws IllegalArgumentException Throws this exception if the input parameter is invalid.
     * @since 3
     */
    public boolean initializePacking(byte[] data, int offset, PackingOptions opts) {
        return false;
    }

    /**
     * Initializes the packing task that outputs the result to a {@code OutputStream} object.
     *
     * @param outputStream Indicates the output stream of the packed images.
     * @param opts Indicates the packing options.
     * @return Returns {@code true} if the packing task is successfully initialized; returns {@code false} otherwise.
     * @throws IllegalArgumentException Throws this exception if the input parameter is invalid.
     * @since 3
     */
    public boolean initializePacking(OutputStream outputStream, PackingOptions opts) {
        return false;
    }

    /**
     * Adds a {@link PixelMap} to the image packer.
     *
     * <p>This method adds only one {@link PixelMap} object at a time.
     *
     * @param pixelmap Indicates the {@link PixelMap} to be packed.
     * @return Returns {@code true} if the pixel map is added; returns {@code false} otherwise.
     * @throws IllegalArgumentException Throws this exception if the input parameter is invalid.
     * @since 3
     */
    public boolean addImage(PixelMap pixelmap) {
        return false;
    }

    /**
     * Adds the image in an {@link ImageSource} to the image packer.
     *
     * <p>If the {@link ImageSource} contains multiple images, only the first image will be added.
     *
     * @param source Indicates the {@code ImageSource}.
     * @return Returns {@code true} if the image is added; returns {@code false} otherwise.
     * @throws IllegalArgumentException Throws this exception if the input parameter is invalid.
     * @since 3
     */
    public boolean addImage(ImageSource source) {
        return false;
    }

    /**
     * Adds a specified image in an {@link ImageSource} to the image packer.
     *
     * <p>You can use an index to specify the image to be added.
     *
     * @param source Indicates the {@code ImageSource}.
     * @param index Indicates the index of the target image in the {@code ImageSource}. The value starts from 0.
     * @return Returns {@code true} if the specified image is added; returns {@code false} otherwise.
     * @throws IllegalArgumentException Throws this exception if the input parameter is invalid.
     * @since 3
     */
    public boolean addImage(ImageSource source, int index) {
        return false;
    }

    /**
     * Completes the image packing.
     *
     * <p>Calling this method will inform the {@code ImagePacker} that all the required image data has been
     * added for packing and the {@code ImagePacker} will pack the data into the specified object.
     *
     * @return Returns the output data size (in bytes) if the operation is successful; returns {@code -1} otherwise.
     * @since 3
     */
    public long finalizePacking() {
        return FAILED;
    }

    /**
     * Releases native resources associated with the {@code ImagePacker} object.
     *
     * @since 3
     */
    public void release() {
        return;
    }

    @Override
    protected void finalize() throws Throwable {
        release();
        super.finalize();
    }

    /**
     * Checks whether native resources have been released.
     *
     * @return Returns {@code true} if the resources are released; returns {@code false} otherwise.
     * @since 1
     */
    private boolean isReleased() {
        return nativeImagePacker == 0;
    }

    private boolean isPackerOptionValid(String format, int quality) {
        return false;
    }
}
