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

import ohos.media.image.common.AllocatorType;
import ohos.media.image.common.ColorSpace;
import ohos.media.image.common.DecodeEvent;
import ohos.media.image.common.ImageFormat;
import ohos.media.image.common.ImageInfo;
import ohos.media.image.common.MemoryUsagePreference;
import ohos.media.image.common.PixelFormat;
import ohos.media.image.common.PropertyKey;
import ohos.media.image.common.Rect;
import ohos.media.image.common.Size;
import ohos.media.image.exifadapter.ExifAdapter;
import ohos.media.utils.log.Logger;
import ohos.media.utils.log.LoggerFactory;
import ohos.media.utils.trace.Tracer;
import ohos.media.utils.trace.TracerFactory;
import ohos.global.resource.RawFileDescriptor;
import ohos.utils.system.SystemCap;
import ohos.utils.system.SystemCapability;

import java.io.File;
import java.io.FileDescriptor;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStream;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.nio.ByteBuffer;
import java.util.HashSet;
import java.util.concurrent.Callable;
import java.util.Locale;

/**
 * Describes image data sources used to share source data in a series of operations, such as image decoding.
 *
 * <p>Applications can use this class to decode image.
 * <p>Create an {@code ImageSource} by calling {@code create}. Obtain basic image information by
 * calling {@code getImageInfo}. Decode images by calling {@code createPixelMap}.
 *
 * @Syscap {@link SystemCapability.Multimedia#IMAGE}
 * @since 1
 */
@SystemCap("SystemCapability.Multimedia.Image")
public class ImageSource {
    private static final Logger LOGGER = LoggerFactory.getImageLogger(ImageSource.class);

    private static final Tracer TRACER = TracerFactory.getImageTracer();

    /**
     * Success state by image source native interface return.
     */
    private static final int SUCCESS = 0;

    private static SourceOptions inputOpts;

    private static long fileSize;

    private volatile ExifAdapter exifAdapter;

    /**
     * Saved data source, maybe ByteArrayDataSource, or InputStreamDataSource, or FileDataSource, or
     * FileDescriptorDataSource.
     */
    private Object imgDataSource;

    /**
     * Accessed by native methods
     */
    private long nativeImageSource;

    private DecodeEventListener decodeEventListener;

    private final Object exifLock = new Object();

    /**
     * Describes the update mode of the image data source.
     *
     * <p>There are two modes: full data update and incremental data update.
     *
     * @since 1
     */
    public enum UpdateMode {
        /**
         * Indicates that all the accumulated obtained data is input to update the source.
         */
        FULL_DATA(0),
        /**
         * Indicates that only incremental data is input to update the source.
         */
        INCREMENTAL_DATA(1);

        private final int updateMode;

        UpdateMode(int mode) {
            updateMode = mode;
        }

        /**
         * Obtains an enum value.
         *
         * @return Returns the enum value.
         * @since 1
         */
        public int getValue() {
            return updateMode;
        }
    }

    /**
     * Describes image data source options, including the image format represented as {@code image/png}, for example.
     *
     * @since 1
     */
    public static class SourceOptions {
        /**
         * Indicates an optional prompt about the image format, which is represented by an MIME type string. Proper
         * configuration improves the decoding efficiency.
         *
         * @since 1
         */
        public String formatHint = "";

        /**
         * Indicates the pixel density of the source image.
         *
         * @since 4
         */
        public int baseDensity = 0;
    }

    /**
     * Describes options of the incremental data source.
     *
     * <p>The options include image data source options provided by {@link SourceOptions}
     * and the data update mode defined by {@link UpdateMode}.
     *
     * @since 1
     */
    public static class IncrementalSourceOptions {
        /**
         * Indicates image data source options defined by {@link SourceOptions}.
         *
         * @since 1
         */
        public SourceOptions opts;

        /**
         * Indicates the data update mode defined by {@link UpdateMode}.
         *
         * @since 1
         */
        public UpdateMode mode = UpdateMode.FULL_DATA;
    }

    /**
     * Provides image decoding options.
     *
     * <p>The options include the pixel format, color space,
     * and parameters for cropping, scaling, and rotating an image.
     *
     * @since 1
     */
    public static class DecodingOptions {
        /**
         * Indicates the default sampling size.
         *
         * @since 1
         */
        public static final int DEFAULT_SAMPLE_SIZE = 1;

        /**
         * Indicates the pixel density of the decoded image.
         *
         * @since 4
         */
        public int fitDensity = 0;

        /**
         * Indicates the memory allocation type for decoded image data.
         *
         * @since 4
         */
        public AllocatorType allocator = AllocatorType.DEFAULT;

        /**
         * Indicates an image area to be decoded.
         * <p>If area decoding is not supported, the entire image is decoded and then cropped with a rectangle.
         * The area to be decoded is specified based on the original image instead of an edited image.
         *
         * @since 1
         */
        public Rect desiredRegion;

        /**
         * Indicates the expected output size.
         * <p>If the opted size is different from the original image size, the image is output
         * after being scaled up or down. If both {@code sampleSize} and this option are set, this option prevails. This
         * variable specifies the final size of an output image. If this variable, {@code desiredRegion}, and {@code
         * rotateDegrees} are all set, the expected image area will be decoded, rotated, and then scaled to the expected
         * size.
         *
         * @since 1
         */
        public Size desiredSize;

        /**
         * Indicates the rotation angle, ranging from 0 to 360. An image is rotated clockwise with the original image
         * center as the rotation center.
         *
         * @since 1
         */
        public float rotateDegrees = 0;

        /**
         * Indicates the sampling size based on which an image is scaled down. For example, if the value is 2, both the
         * width and height of the image are scaled down to half of the original. The value must be an integer, 1 or
         * greater.
         *
         * @since 1
         */
        public int sampleSize = DEFAULT_SAMPLE_SIZE;

        /**
         * Indicates the pixel format of a decoded image, which is defined by {@link PixelFormat}.
         * <p>If this format is supported, the decoder will decode the image to the target format.
         * Otherwise, the decoder will select the most suitable format for the image.
         *
         * @since 1
         */
        public PixelFormat desiredPixelFormat = PixelFormat.UNKNOWN;

        /**
         * Indicates the color space of a decoded image, which is defined by {@link ColorSpace}.
         * <p>If this color space is supported, the decoder will decode the image to one with this color space.
         * Otherwise, the decoder will select the most suitable color space for the image.
         *
         * @since 1
         */
        public ColorSpace desiredColorSpace = ColorSpace.SRGB;

        /**
         * Indicates whether to allow partial image decoding.
         * <p>When this option is set to {@code false} and an incomplete image is partially decoded, the decoding
         * fails.
         *
         * @since 1
         */
        public boolean allowPartialImage = true;

        /**
         * Specifies whether pixel values of the pixel map to be decoded can be edited. Value {@code true} means that
         * the values can be edited, and {@code false} means the opposite. The default value is {@code false}.
         *
         * @since 4
         */
        public boolean editable = false;
    }

    /**
     * Describes image source information, including the image quantity and file format.
     *
     * @since 1
     */
    public static class SourceInfo {
        /**
         * Indicates the number of top-level images contained in the source.
         *
         * @since 1
         */
        public int topLevelImageNum = 0;

        /**
         * Indicates the image format represented by an MIME type string, for example, {@code image/png}.
         *
         * @since 1
         */
        public String encodedFormat;
    }

    /**
     * Data file path of encoded image data.
     *
     * @since 3
     */
    private static class FilePathDataSource {
        private final String filePath;

        FilePathDataSource(String filePath) {
            this.filePath = filePath;
        }
    }

    /**
     * Data byte array of encoded image data.
     *
     * @since 1
     */
    private static class ByteArrayDataSource {
        private final byte[] data;

        private final int offset;

        private final int length;

        ByteArrayDataSource(byte[] data, int offset, int length) {
            this.data = data;
            this.offset = offset;
            this.length = length;
        }
    }

    /**
     * Input stream of encoded image data.
     *
     * @since 1
     */
    private static class InputStreamDataSource {
        private final InputStream is;

        InputStreamDataSource(InputStream is) {
            this.is = is;
        }
    }

    /**
     * File of encoded image data.
     *
     * @since 1
     */
    private static class FileDataSource {
        private final File file;

        FileDataSource(File file) {
            this.file = file;
        }
    }

    /**
     * File descriptor of encoded image data.
     *
     * @since 1
     */
    private static class FileDescriptorDataSource {
        private final FileDescriptor fd;

        FileDescriptorDataSource(FileDescriptor fd) {
            this.fd = fd;
        }
    }

    static {
        LOGGER.debug("Begin loading image_source_jni library");
        String osName = System.getProperty("os.name");
        String jniLibName = "image_source_jni.z";
        if (osName != null) {
            osName = osName.toUpperCase(Locale.US);
            if (osName.contains("WINDOWS")) {
                jniLibName = "libimage_source_jni";
            } else if (osName.contains("MAC")) {
                jniLibName = "image_source_jni";
            } else {
                jniLibName = "image_source_jni.z";
            }
        }
        try {
            System.loadLibrary(jniLibName);
        } catch (UnsatisfiedLinkError | NullPointerException e) {
            LOGGER.error("loadLibrary %{public}s fail", jniLibName);
        }
        nativeInit();
    }

    /**
     * Private constructor called by JNI.
     *
     * @param nativePtr the native object point.
     */
    private ImageSource(long nativePtr) {
        nativeImageSource = nativePtr;
    }

    /**
     * Obtains supported image formats for decoding, which are represented by MIME type strings.
     *
     * @return Returns the supported formats represented by MIME type strings.
     * @since 1
     */
    public static HashSet<String> getSupportedFormats() {
        return nativeGetSupportedFormats();
    }

    /**
     * Listens for the events occurred during image decoding.
     *
     * <p>To listen for the events, you should implement the {@link #onDecodeEvent} method, which
     * obtains the {@link ImageSource} and {@link ohos.media.image.common.DecodeEvent}.
     *
     * @since 4
     */
    public static interface DecodeEventListener {
        /**
         * Called when a decoding event occurs.
         *
         * @param source Indicates the image source.
         * @param event Indicates the event, which can be
         * {@link ohos.media.image.common.DecodeEvent#EVENT_COMPLETE_DECODE}
         * or {@link ohos.media.image.common.DecodeEvent#EVENT_PARTIAL_DECODE}.
         * @since 4
         */
        void onDecodeEvent(ImageSource source, DecodeEvent event);
    }

    /**
     * Sets a listener for observing decoding events.
     *
     * @param listener Indicates the listener to set.
     * @since 4
     */
    public void setDecodeEventListener(DecodeEventListener listener) {
        decodeEventListener = listener;
        nativeSetDecodeEventListener(this, nativeImageSource);
    }

    private void postEventFromNative(ImageSource source, int event) {
        if (decodeEventListener == null) {
            LOGGER.error("decodeEventListener is null.");
            return;
        }

        DecodeEvent recEvent = DecodeEvent.getDecodeEvent(event);
        if (recEvent.getValue() >= DecodeEvent.EVENT_LAST.getValue()) {
            LOGGER.error("event is invalid: %{public}d", event);
            return;
        }
        decodeEventListener.onDecodeEvent(source, recEvent);
    }

    /**
     * Sets memory use preference.
     *
     * @param preference Indicates the preference to set,
     * which is defined in {@link ohos.media.image.common.MemoryUsagePreference}.
     * @since 4
     */
    public void setMemoryUsagePreference(MemoryUsagePreference preference) {
        nativeSetMemoryUsagePreference(nativeImageSource, preference);
    }

    /**
     * Creates an {@link ImageSource} based on an input stream.
     *
     * <p>This method takes an {@code InputStream} instance as the source where the image data derives from. The {@code
     * ImageSource} instance to create cannot be used to obtain Exif information.
     *
     * @param is Indicates an input stream of the image data source.
     * @param opts Indicates data source options.
     * @return Returns the {@link ImageSource} object if created; returns {@code null} if an exception occurs.
     * @throws IllegalArgumentException Throws this exception if the input parameter is invalid.
     * @since 1
     */
    public static ImageSource create(InputStream is, SourceOptions opts) {
        if (is == null) {
            throw new IllegalArgumentException("is is null");
        }

        ImageSource imgSource = nativeCreateImageSource(is, opts);
        inputOpts = opts;
        if (imgSource == null) {
            LOGGER.error("create ImageSource from input stream fail.");
            return null;
        }

        imgSource.imgDataSource = new InputStreamDataSource(is);
        try {
            fileSize = is.available();
        } catch (IOException e) {
            LOGGER.error("get file size failed of InputStream ImageSource");
        }
        return imgSource;
    }

    /**
     * Creates an {@link ImageSource} based on a byte array.
     *
     * @param data Indicates the byte array containing the source data.
     * @param opts Indicates data source options.
     * @return Returns the {@link ImageSource} object if created; returns {@code null} if an exception occurs.
     * @throws IllegalArgumentException Throws this exception if the input parameter is invalid.
     * @since 1
     */
    public static ImageSource create(byte[] data, SourceOptions opts) {
        if (data == null) {
            throw new IllegalArgumentException("data is null");
        }

        return create(data, 0, data.length, opts);
    }

    /**
     * Creates an {@code ImageSource} based on a {@link java.nio.ByteBuffer} and source options.
     *
     * @param data Indicates the {@code ByteBuffer} that contains the image source data.
     * @param opts Indicates the source options.
     * @return Returns the {@code ImageSource} if the creation is successful; returns {@code null} otherwise.
     * @throws IllegalArgumentException Throws this exception if an input parameter is invalid.
     * @since 4
     */
    public static ImageSource create(ByteBuffer data, SourceOptions opts) {
        if (data == null) {
            throw new IllegalArgumentException("data is null");
        }
        data.flip();
        int length = data.limit() - data.position();
        byte[] byteData = new byte[length];
        if (data.isReadOnly()) {
            throw new IllegalArgumentException("data is read only");
        } else {
            data.get(byteData);
        }

        return create(byteData, 0, byteData.length, opts);
    }

    /**
     * Creates an {@code ImageSource} based on a {@link ohos.global.resource.RawFileDescriptor}
     * that is returned by a {@code Callable} object.
     *
     * @param callable Indicates the object that implements the {@link java.util.concurrent.Callable} interface.
     * @param opts Indicates the source options.
     * @return Returns the {@link ImageSource} if the creation is successful; returns {@code null} otherwise.
     * @throws IllegalArgumentException Throws this exception if an input parameter is invalid.
     * @since 4
     */
    public static ImageSource create(Callable<RawFileDescriptor> callable, SourceOptions opts) {
        if (callable == null) {
            throw new IllegalArgumentException("RawFileDescriptor is null");
        }
        RawFileDescriptor rawFileDes = null;
        try {
            rawFileDes = callable.call();
        } catch (Exception e) {
            if (e instanceof IOException) {
                LOGGER.error("create ImageSource from raw file descriptor fail, reason : IOException.");
                return null;
            } else {
                LOGGER.error("create ImageSource from raw file descriptor fail, reason : Exception.");
                return null;
            }
        }
        if (rawFileDes == null) {
            throw new IllegalArgumentException("rawFileDes is null");
        }
        FileDescriptor fileDes = rawFileDes.getFileDescriptor();
        long startPosition = rawFileDes.getStartPosition();
        if (startPosition < 0 || rawFileDes.getFileSize() < 0) {
            LOGGER.error("rawFileDes startPosition is illegal %{public}d", startPosition);
            closeRawFileDescriptor(rawFileDes);
            return null;
        }
        FileInputStream inputStream = new FileInputStream(fileDes);
        try {
            inputStream.skip(startPosition);
        } catch (IOException e) {
            LOGGER.error("inputStream skip fail, reason : IOException");
            closeInputStream(inputStream);
            closeRawFileDescriptor(rawFileDes);
            return null;
        }
        return create(inputStream, opts);
    }

    /**
     * Creates an {@link ImageSource} based on a byte array with specified offset and length.
     *
     * @param data Indicates the byte array containing the source data.
     * @param offset Indicates the offset into image data for where the reading begins.
     * @param length Indicates the image data size, in bytes.
     * @param opts Indicates data source options.
     * @return Returns the {@link ImageSource} object if created; returns {@code null} if an exception occurs.
     * @throws IllegalArgumentException Throws this exception if the input parameter is invalid.
     * @throws IndexOutOfBoundsException Throws this exception if the offset or length is invalid.
     * @since 1
     */
    public static ImageSource create(byte[] data, int offset, int length, SourceOptions opts) {
        if (data == null) {
            throw new IllegalArgumentException("data is null");
        }
        if ((offset < 0) || (length < 0) || (offset >= data.length) || (offset + length > data.length)) {
            throw new IndexOutOfBoundsException("offset or length is invalid");
        }

        ImageSource imgSource = nativeCreateImageSource(data, offset, length, opts);
        inputOpts = opts;
        if (imgSource == null) {
            LOGGER.error("create ImageSource from data array fail. offset : %{public}d, length :  %{public}d.", offset,
                length);
            return null;
        }

        imgSource.imgDataSource = new ByteArrayDataSource(data, offset, length);
        fileSize = length;
        return imgSource;
    }

    /**
     * Creates an {@link ImageSource} based on an image file path.
     *
     * @param pathName Indicates the file path based on which the {@link ImageSource} is created.
     * @param opts Indicates data source options.
     * @return Returns the {@link ImageSource} object if created; returns {@code null} if an exception occurs.
     * @throws IllegalArgumentException Throws this exception if the input parameter is invalid.
     * @since 1
     */
    public static ImageSource create(String pathName, SourceOptions opts) {
        if ((pathName == null) || pathName.isEmpty()) {
            throw new IllegalArgumentException("pathName is invalid");
        }

        if (!Files.isReadable(Paths.get(pathName))) {
            throw new DataSourceUnavailableException("pathName can not read");
        }

        ImageSource imageSource = nativeCreateImageSource(pathName, opts);
        inputOpts = opts;
        if (imageSource == null) {
            LOGGER.error("create ImageSource from file path fail");
            return null;
        }
        imageSource.imgDataSource = new FilePathDataSource(pathName);
        fileSize = (new File(pathName)).length();
        return imageSource;
    }

    /**
     * Creates an {@link ImageSource} based on a {@link java.io.File} object.
     *
     * @param file Indicates a {@link java.io.File} object.
     * @param opts Indicates data source options.
     * @return Returns the {@link ImageSource} object if created; returns {@code null} if an exception occurs.
     * @throws IllegalArgumentException Throws this exception if the input parameter is invalid.
     * @since 1
     */
    public static ImageSource create(File file, SourceOptions opts) {
        if (file == null) {
            throw new IllegalArgumentException("file is null");
        }

        ImageSource imgSource;
        try (InputStream inStream = new FileInputStream(file)) {
            imgSource = nativeCreateImageSource(inStream, opts);
            inputOpts = opts;
            if (imgSource == null) {
                LOGGER.error("create ImageSource from file fail");
                return null;
            }

            imgSource.imgDataSource = new FileDataSource(file);
        } catch (FileNotFoundException fe) {
            LOGGER.error("create ImageSource from file fail, reason : file not found.");
            return null;
        } catch (IOException ioe) {
            LOGGER.error("create ImageSource from file, IO Exception");
            return null;
        }
        fileSize = file.length();
        return imgSource;
    }

    /**
     * Creates an {@link ImageSource} based on a file descriptor.
     *
     * <p>This method takes a {@code FileDescriptor} instance as the source where the image data derives from. The
     * {@code ImageSource} instance to create cannot be used to obtain Exif information.
     *
     * @param fd Indicates the descriptor of an image source file.
     * @param opts Indicates data source options.
     * @return Returns the {@link ImageSource} object if created; returns {@code null} if an exception occurs.
     * @throws IllegalArgumentException Throws this exception if the input parameter is invalid.
     * @since 1
     */
    public static ImageSource create(FileDescriptor fd, SourceOptions opts) {
        if ((fd == null) || (!fd.valid())) {
            throw new IllegalArgumentException("fd is invalid");
        }

        ImageSource imgSource;
        try (InputStream inStream = new FileInputStream(fd)) {
            imgSource = nativeCreateImageSource(inStream, opts);
            inputOpts = opts;
            if (imgSource == null) {
                LOGGER.error("createImageSource from fd fail");
                return null;
            }

            imgSource.imgDataSource = new FileDescriptorDataSource(fd);
            fileSize = inStream.available();
        } catch (SecurityException fe) {
            LOGGER.error("createImageSource from file descriptor fail, SecurityException : %{public}s.",
                fe.getMessage());
            return null;
        } catch (IOException ioe) {
            LOGGER.error("createImageSource from file, IO Exception");
            return null;
        }
        return imgSource;
    }

    /**
     * Creates an incremental data source based on a {@link SourceOptions} object.
     *
     * <p>This method is used with {@link ImageSource#updateData(byte[], int, int, boolean)} to update incremental
     * data.
     *
     * @param opts Indicates data source options.
     * @return Returns the {@link ImageSource} object if created; returns {@code null} if an exception occurs.
     * @throws IllegalArgumentException Throws this exception if the input parameter is invalid.
     * @since 1
     */
    public static ImageSource createIncrementalSource(SourceOptions opts) {
        IncrementalSourceOptions increOpts = new IncrementalSourceOptions();
        increOpts.opts = opts;
        inputOpts = opts;
        return createIncrementalSource(increOpts);
    }

    /**
     * Creates an incremental data source based on an {@link IncrementalSourceOptions} object.
     *
     * <p>This method is used with {@link ImageSource#updateData(byte[], int, int, boolean)} to update incremental
     * data.
     *
     * @param opts Indicates incremental data source options.
     * @return Returns the {@link ImageSource} object if created; returns {@code null} if an exception occurs.
     * @throws IllegalArgumentException Throws this exception if the input parameter is invalid.
     * @since 1
     */
    public static ImageSource createIncrementalSource(IncrementalSourceOptions opts) {
        return nativeCreateIncrementalImageSource(opts);
    }

    /**
     * Decodes source image data and creates a pixel map.
     *
     * <p>For a source containing multiple images, the image data of index 0 is decoded by default.
     *
     * @param opts Indicates image decoding options.
     * @return Returns the image if the data is decoded; returns {@code null} if an exception occurs.
     * @throws IllegalArgumentException Throws this exception if the input parameter is invalid.
     * @since 1
     */
    public PixelMap createPixelmap(DecodingOptions opts) {
        return createPixelmap(0, opts);
    }

    /**
     * Decodes source image data based on a specified index location in the {@link ImageSource} and creates a pixel
     * map.
     *
     * <p>For a source containing multiple images, use an index to specify the image location.
     *
     * @param index Indicates an index specifying the image location. The index starts from 0.
     * @param opts Indicates image decoding options.
     * @return Returns the image if the data is decoded; returns {@code null} if an exception occurs.
     * @throws IllegalArgumentException Throws this exception if the input parameter is invalid.
     * @since 1
     */
    public PixelMap createPixelmap(int index, DecodingOptions opts) {
        if (index < 0) {
            throw new IllegalArgumentException("index must not be negative");
        }

        checkReleased();

        PixelMap pixelMap = null;
        String traceVal = "ImageSource_createPixelmap";
        TRACER.startTrace(traceVal);
        pixelMap = nativeCreatePixelmap(this, nativeImageSource, index, opts);
        TRACER.finishTrace(traceVal);
        return pixelMap;
    }

    /**
     * Decodes thumbnail data contained in the {@link ImageSource} instance to generate a thumbnail and creates a
     * thumbnail pixel map.
     *
     * @param opts Indicates image decoding options.
     * @param allowFromImage Specifies whether to allow creation based on the original image if the {@link ImageSource}
     * does not contain thumbnail data.
     * @return Returns the created {@link PixelMap} if the decoding is successful; returns {@code null} if the decoding
     * fails.
     * @throws IllegalArgumentException Throws this exception if the input parameter is invalid.
     * @throws IllegalStateException Throws this exception if native resources associated with this {@link ImageSource}
     * have been released.
     * @since 2
     */
    public PixelMap createThumbnailPixelmap(DecodingOptions opts, boolean allowFromImage) {
        LOGGER.info("enter createThumbnailPixelmap");
        checkExifSource();
        byte[] data = exifAdapter.getThumbnailBytes();
        if (data == null) {
            LOGGER.info("image source has no inner thumbnail");
            if (allowFromImage) {
                LOGGER.info("create thumbnail pixel map from original image");
                return createPixelmap(opts);
            }
            LOGGER.info("get thumbnail data null");
            return null;
        }

        ImageSource imageSource = ImageSource.create(data, null);
        if (imageSource == null) {
            LOGGER.error("create thumbnail image source failed");
            return null;
        }
        LOGGER.info("createThumbnailPixelmap from inner thumbnail bytes");
        return imageSource.createPixelmap(opts);
    }

    /**
     * Updates incremental data to an image data source using a byte array.
     *
     * <p>When incremental data is obtained, use this method to update source data.
     * Input data requirements are described by {@link UpdateMode}.
     *
     * @param data Indicates the byte array inputting incremental data.
     * @param isFinal Indicates whether the data update is the last batch.
     * @return Returns {@code true} if the source data is updated; returns {@code false} otherwise.
     * @throws IllegalArgumentException Throws this exception if the input parameter is invalid.
     * @since 1
     */
    public boolean updateData(byte[] data, boolean isFinal) {
        return updateData(data, 0, data.length, isFinal);
    }

    /**
     * Updates incremental data to an image data source using a byte array with specified offset and length.
     *
     * <p>When incremental data is obtained, use this method to update source data.
     * Input data requirements are described by {@link UpdateMode}.
     *
     * @param data Indicates the byte array inputting incremental data.
     * @param offset Indicates offset into image data for where the reading starts.
     * @param length Indicates the size of the incremental data, in bytes.
     * @param isFinal Indicates whether the data is the last batch.
     * @return Returns {@code true} if the data is updated; returns {@code false} otherwise.
     * @throws IllegalArgumentException Throws this exception if the input parameter is invalid.
     * @throws IndexOutOfBoundsException Throws this exception if the offset or length is invalid.
     * @since 1
     */
    public boolean updateData(byte[] data, int offset, int length, boolean isFinal) {
        if (data == null) {
            throw new IllegalArgumentException("data must not be null");
        }
        if ((offset < 0) || (length < 0) || (offset >= data.length) || (offset + length > data.length)) {
            throw new IndexOutOfBoundsException("offset or length is invalid");
        }

        checkReleased();
        int ret = nativeUpdateData(nativeImageSource, data, offset, length, isFinal);
        if (ret != SUCCESS) {
            LOGGER.error("updateData failed from data array, error code is %{public}d", ret);
            return false;
        }

        return true;
    }

    /**
     * Obtains basic image information.
     *
     * @return Returns the {@link ohos.media.image.common.ImageInfo} object if obtained; returns {@code null} otherwise.
     * Note that the returned {@code ImageInfo} object contains only the {@link ohos.media.image.common.ImageInfo#size}
     * of this {@code ImageSource} object. Other image information can be obtained only after image decoding.
     * @since 1
     */
    public ImageInfo getImageInfo() {
        return getImageInfo(0);
    }

    /**
     * Obtains basic information about the image at a specified index.
     *
     * @param index Indicates the image index, which starts from 0.
     * @return Returns the {@link ohos.media.image.common.ImageInfo} object if obtained; returns {@code null} otherwise.
     * Note that the returned {@code ImageInfo} object contains only the {@link ohos.media.image.common.ImageInfo#size}
     * of this {@code ImageSource} object. Other image information can be obtained only after image decoding.
     * @throws IllegalArgumentException Throws this exception if the input parameter is invalid.
     * @since 1
     */
    public ImageInfo getImageInfo(int index) {
        if (index < 0) {
            throw new IllegalArgumentException("index must not be negative");
        }
        checkReleased();
        return nativeGetImageInfo(nativeImageSource, index);
    }

    /**
     * Obtains the string value of a specified image property key.
     *
     * @param key Indicates a key defined in {@code PropertyKey}.
     * @return Returns the property key value if obtained; returns {@code null} if the property does not exist.
     * @throws IllegalStateException Throws this exception if native resources associated with this {@code ImageSource}
     * instance have been released.
     * @throws IllegalArgumentException Throws this exception if {@code key} is null.
     * @since 4
     */
    public final String getImagePropertyString(String key) {
        if (key == null) {
            throw new IllegalArgumentException("key is null");
        }

        checkExifSource();
        return exifAdapter.getImagePropertyString(key);
    }

    /**
     * Obtains the integer value of a specified image property key.
     *
     * @param key Indicates a key defined in {@code PropertyKey}.
     * @param defaultValue Indicates the custom default value of the property key.
     * @return Returns the integer value if obtained; returns the custom default value if the property key does not
     * exist or is incompatible with integers.
     * @throws IllegalStateException Throws this exception if native resources associated with this {@code ImageSource}
     * instance have been released.
     * @throws IllegalArgumentException Throws this exception if {@code key} is null.
     * @since 4
     */
    public final int getImagePropertyInt(String key, int defaultValue) {
        return getImagePropertyInt(0, key, defaultValue);
    }

    /**
     * Obtains the integer value of a specified property key for an image at the given index in the {@link
     * ImageSource}.
     *
     * @param index Indicates an index specifying the image location. The index starts from 0.
     * @param key Indicates a key defined in {@code PropertyKey}.
     * @param defaultValue Indicates the custom default value of the property key.
     * @return Returns the integer value if obtained; returns the custom default value if the property key does not
     * exist or is incompatible with integers.
     * @throws IllegalStateException Throws this exception if native resources associated with this {@code ImageSource}
     * instance have been released.
     * @throws IllegalArgumentException Throws this exception if {@code key} is null.
     * @since 4
     */
    public final int getImagePropertyInt(int index, String key, int defaultValue) {
        if (key == null) {
            throw new IllegalArgumentException("key is null");
        }

        checkExifSource();
        if (key.startsWith("GIF")) {
            return nativeGetImagePropertyInt(nativeImageSource, index, key, defaultValue);
        } else {
            return exifAdapter.getImagePropertyInt(key, defaultValue);
        }
    }

    /**
     * Obtains basic information (such as the size) about the thumbnail from the {@code ImageSource} instance.
     * <p>Currently, only thumbnails in JPEG format can be identified.
     *
     * @return Returns the basic information if obtained; returns {@code null} if the thumbnail is not in JPEG format.
     * @throws IllegalStateException Throws this exception if native resources associated with this {@code ImageSource}
     * instance have been released.
     * @throws UnsupportedOperationException Throws this exception if the source image file does not contain a
     * thumbnail.
     * @since 4
     */
    public ImageInfo getThumbnailInfo() {
        checkExifSource();
        byte[] data = exifAdapter.getThumbnailBytes();
        if (data == null) {
            throw new UnsupportedOperationException("image does not contain thumbnail");
        }

        ImageSource imageSource = ImageSource.create(data, null);
        if (imageSource == null) {
            LOGGER.error("getThumbnailInfo: create thumbnail image source failed");
            return null;
        }
        return imageSource.getImageInfo();
    }

    /**
     * Obtains the byte data of the JPEG thumbnail stored in the Exif information
     * about this JPEG {@code ImageSource} instance.
     *
     * @return Returns a {@code byte} array containing the thumbnail data if the
     *         Exif information of this JPEG {@code ImageSource} instance contains a
     *         thumbnail in JPEG format; returns {@code null} if the Exif
     *         information does not contain a thumbnail or the thumbnail is not a
     *         JPEG file.
     * @throws IllegalStateException IllegalStateException Throws this exception if
     *                               this {@code ImageSource} instance:
     *                               <ul>
     *                               <li>Is in an unknown format.</li>
     *                               <li>Is abnormal.</li>
     *                               <li>Has been released.</li>
     *                               </ul>
     * @since 6
     */
    public byte[] getInnerJpegThumbnail() {
        checkExifSource();
        return exifAdapter.getThumbnail();
    }

    /**
     * Obtains the start position and data length of the thumbnail data saved in the
     * Exif information about this JPEG {@code ImageSource} instance.
     *
     * @return Returns a {@code long} array containing the start position of the
     *         thumbnail data and the data length if the Exif information of this
     *         JPEG {@code ImageSource} instance has thumbnail data; returns
     *         {@code null} if there is no thumbnail data in the Exif information or
     *         the thumbnail data is discrete.
     * @throws IllegalStateException Throws this exception if this
     *                               {@code ImageSource} instance:
     *                               <ul>
     *                               <li>Is in an unknown format.</li>
     *                               <li>Is abnormal.</li>
     *                               <li>Has been released.</li>
     *                               </ul>
     * @since 6
     */
    public long[] getInnerThumbnailScope() {
        checkExifSource();
        long[] offsetLength = null;
        try {
            offsetLength = exifAdapter.getThumbnailRange();
        } catch (IllegalStateException e) {
            throw new IllegalStateException("Current state illegal,offsets may have changed");
        }
        return offsetLength;
    }

    /**
     * Obtains raw data of the thumbnail from the {@code ImageSource} instance.
     *
     * @return Returns the byte array containing the thumbnail data if the thumbnail exists.
     * @throws IllegalStateException Throws this exception if native resources associated with this {@code ImageSource}
     * instance have been released.
     * @throws UnsupportedOperationException Throws this exception if the source image file does not contain a
     * thumbnail.
     * @since 3
     */
    public byte[] getImageThumbnailBytes() {
        checkExifSource();
        if (exifAdapter.getThumbnailBytes() == null) {
            throw new UnsupportedOperationException("image does not contain thumbnail");
        }
        return exifAdapter.getThumbnailBytes();
    }

    /**
     * Obtains the format of the thumbnail from the {@code ImageSource} instance. <p>Currently, only thumbnails in JPEG
     * format can be identified.
     *
     * @return Returns {@link ohos.media.image.common.ImageFormat#JPEG} if the thumbnail is in JPEG format; returns
     * {@link ohos.media.image.common.ImageFormat#UNKNOWN} otherwise.
     * @throws IllegalStateException Throws this exception if native resources associated with this {@code ImageSource}
     * have been released.
     * @throws UnsupportedOperationException Throws this exception if the source image file does not contain a
     * thumbnail.
     * @since 3
     */
    public int getThumbnailFormat() {
        checkExifSource();
        if (exifAdapter.getThumbnailBytes() == null) {
            throw new UnsupportedOperationException("image does not contain thumbnail");
        }

        if (exifAdapter.getThumbnail() != null) {
            return ImageFormat.JPEG;
        }

        return ImageFormat.UNKNOWN;
    }

    private static void closeInputStream(FileInputStream inputStream) {
        try {
            inputStream.close();
        } catch(IOException e) {
            LOGGER.error("create ImageSource inputStream close reason : Exception.");
        }
    }

    private static void closeRawFileDescriptor(RawFileDescriptor rawFileDes) {
        try {
            rawFileDes.close();
        } catch(IOException e) {
            LOGGER.error("create ImageSource rawFileDes close reason : Exception.");
        }
    }

    private void checkReleased() {
        if (isReleased()) {
            throw new IllegalStateException("native resources has been released");
        }
    }

    /**
     * Obtains image source information.
     *
     * @return Returns the image source information if obtained; returns {@code null} if an exception occurs.
     * @since 1
     */
    public SourceInfo getSourceInfo() {
        checkReleased();
        return nativeGetSourceInfo(nativeImageSource);
    }

    /**
     * Releases native resources associated with an object.
     *
     * @since 1
     */
    public void release() {
        if (!isReleased()) {
            nativeRelease(nativeImageSource);
            nativeImageSource = 0L;
            exifAdapter = null;
        }
        imgDataSource = null;
    }

    /**
     * Used to determine if the native resources of the Image Source has been released.
     *
     * @return True if the native resources of the ImageSource has been released
     * @since 4
     */
    public boolean isReleased() {
        return nativeImageSource == 0;
    }

    @Override
    protected void finalize() throws Throwable {
        release();
        super.finalize();
    }

    private void checkExifSource() {
        checkReleased();
        if (exifAdapter == null) {
            synchronized (exifLock) {
                if (exifAdapter == null) {
                    try {
                        if (imgDataSource instanceof FilePathDataSource) {
                            FilePathDataSource dataSource = (FilePathDataSource) imgDataSource;
                            exifAdapter = new ExifAdapter(dataSource.filePath);
                        } else if (imgDataSource instanceof ByteArrayDataSource) {
                            ByteArrayDataSource dataSource = (ByteArrayDataSource) imgDataSource;
                            exifAdapter = new ExifAdapter(dataSource.data, dataSource.offset, dataSource.length);
                        } else if (imgDataSource instanceof InputStreamDataSource) {
                            InputStreamDataSource dataSource = (InputStreamDataSource) imgDataSource;
                            dataSource.is.mark(0);
                            dataSource.is.reset();
                            exifAdapter = new ExifAdapter(dataSource.is);
                        } else if (imgDataSource instanceof FileDataSource) {
                            FileDataSource dataSource = (FileDataSource) imgDataSource;
                            exifAdapter = new ExifAdapter(dataSource.file);
                        } else if (imgDataSource instanceof FileDescriptorDataSource) {
                            FileDescriptorDataSource dataSource = (FileDescriptorDataSource) imgDataSource;
                            exifAdapter = new ExifAdapter(dataSource.fd);
                        } else {
                            throw new IllegalStateException("unknown type of image source");
                        }
                    } catch (IOException e) {
                        LOGGER.error("create exif adapter from data source failed");
                        throw new IllegalStateException("image data source invalid:" + e.getMessage());
                    }
                }
            }
        }
    }

    final double getImagePropertyDouble(String key, double defaultValue) {
        if (key == null) {
            throw new IllegalArgumentException("key is null");
        }

        checkExifSource();
        return exifAdapter.getImagePropertyDouble(key, defaultValue);
    }

    ExifAdapter getExifAdapterInstance() {
        checkExifSource();
        return exifAdapter;
    }

    /**
     * get updated ImageSource if image file source changed, such as after edit exif infos.
     *
     * @return new ImageSoure instance.
     * @since 3
     */
    ImageSource updateImageSource() {
        try {
            if (imgDataSource instanceof FilePathDataSource) {
                FilePathDataSource dataSource = (FilePathDataSource) imgDataSource;
                return create(dataSource.filePath, inputOpts);
            } else if (imgDataSource instanceof ByteArrayDataSource) {
                ByteArrayDataSource dataSource = (ByteArrayDataSource) imgDataSource;
                return create(dataSource.data, dataSource.offset, dataSource.length, inputOpts);
            } else if (imgDataSource instanceof InputStreamDataSource) {
                InputStreamDataSource dataSource = (InputStreamDataSource) imgDataSource;
                dataSource.is.mark(0);
                dataSource.is.reset();
                return create(dataSource.is, inputOpts);
            } else if (imgDataSource instanceof FileDataSource) {
                FileDataSource dataSource = (FileDataSource) imgDataSource;
                return create(dataSource.file, inputOpts);
            } else if (imgDataSource instanceof FileDescriptorDataSource) {
                FileDescriptorDataSource dataSource = (FileDescriptorDataSource) imgDataSource;
                return create(dataSource.fd, inputOpts);
            } else {
                LOGGER.error("update ImageSource failed.");
                return null;
            }
        }catch (IOException e) {
            LOGGER.error("update ImageSource failed of IOException");
        }
        return null;
    }

    /**
     * Used to get ImageSource related file Size.
     *
     * @return valid fileSize or -1 means get file size error.
     * @since 3
     */
    long getFileSize() {
        return fileSize > 0 ? fileSize : -1;
    }

    private static native void nativeInit();

    private native void nativeRelease(long nativePtr);

    private native void nativeSetDecodeEventListener(ImageSource sourceObj, long nativePtr);

    private static native HashSet<String> nativeGetSupportedFormats();

    private static native ImageSource nativeCreateImageSource(String pathName, SourceOptions opts);

    private static native ImageSource nativeCreateImageSource(InputStream is, SourceOptions opts);

    private static native ImageSource nativeCreateImageSource(byte[] data, int offset, int length, SourceOptions opts);

    private static native ImageSource nativeCreateIncrementalImageSource(IncrementalSourceOptions opts);

    private native int nativeUpdateData(long nativePtr, byte[] data, int offset, int length, boolean isCompleted);

    private native PixelMap nativeCreatePixelmap(ImageSource sourceObj, long nativePtr,
        int index, DecodingOptions opts);

    private native ImageInfo nativeGetImageInfo(long nativePtr, int index);

    private native SourceInfo nativeGetSourceInfo(long nativePtr);

    private native int nativeGetImagePropertyInt(long nativePtr, int index, String key, int defaultValue);

    private native void nativeSetMemoryUsagePreference(long nativePtr, MemoryUsagePreference preference);
}
