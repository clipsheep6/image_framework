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

import ohos.media.image.common.AlphaType;
import ohos.media.image.common.ImageInfo;
import ohos.media.image.common.PixelFormat;
import ohos.media.image.common.Position;
import ohos.media.image.common.Rect;
import ohos.media.image.common.ScaleMode;
import ohos.media.image.common.Size;
import ohos.media.utils.log.Logger;
import ohos.media.utils.log.LoggerFactory;
import ohos.system.Parameters;
import ohos.utils.Parcel;
import ohos.utils.Sequenceable;
import ohos.utils.system.SystemCap;
import ohos.utils.system.SystemCapability;

import java.nio.Buffer;
import java.nio.ByteBuffer;
import java.nio.IntBuffer;
import java.nio.ShortBuffer;
import java.util.Objects;
import java.util.Locale;

import ark.system.NativeAllocationNotifier;

/**
 * Provides images in forms of pixel matrices.
 *
 * <p>Applications can use methods, such as {@link #getImageInfo}, to obtain basic image information.
 *
 * @Syscap {@link SystemCapability.Multimedia#IMAGE}
 * @since 1
 */
@SystemCap("SystemCapability.Multimedia.Image")
public class PixelMap implements Sequenceable {
    private static final Logger LOGGER = LoggerFactory.getImageLogger(PixelMap.class);

    /**
     * Creates a {@link PixelMap} instance from a {@link Parcel}. If the class has a final member variable whose value
     * cannot be changed by {@link Sequenceable#unmarshalling}.
     *
     * @since 1
     */
    public static final Sequenceable.Producer<PixelMap> PRODUCER = new Sequenceable.Producer<PixelMap>() {
        /**
         * Create a new instance of the Sequenceable class from the given parcel and
         * the parcel had been inited.
         */
        public PixelMap createFromParcel(Parcel parcel) {
            if (parcel.readInt() == 0) {
                return null;
            }
            PixelMap pixelMap = nativeCreateFromParcel(parcel);
            if (pixelMap == null) {
                LOGGER.error("create pixel map from parcel failed.");
            }
            return pixelMap;
        }
    };

    /**
     * Pixelmap default density data
     */
    private static final int DENSITY_DEFAULT = 160;

    /**
     * Pixelmap unknown density.
     */
    private static final int DENSITY_NONE = 0;

    /**
     * max width or height.
     */
    private static final int MAX_DIMENSION = Integer.MAX_VALUE >> 2;

    static {
        LOGGER.debug("Begin loading pixelmap_jni library");
        String osName = System.getProperty("os.name");
        String jniLibName = "pixelmap_jni.z";
        if (osName != null) {
            osName = osName.toUpperCase(Locale.US);
            if (osName.contains("WINDOWS")) {
                jniLibName = "libpixelmap_jni";
            } else if (osName.contains("MAC")) {
                jniLibName = "pixelmap_jni";
            } else {
                jniLibName = "pixelmap_jni.z";
            }
        }
        try {
            System.loadLibrary(jniLibName);
        } catch (UnsatisfiedLinkError | NullPointerException e) {
            LOGGER.error("loadLibrary %{public}s fail", jniLibName);
        }
    }

    /**
     * Accessed by native methods
     */
    private long nativeImagePixelMap;

    /**
     * Accessed by native methods
     */
    private byte[] nativeNinePatchChunk;

    /**
     * Accessed by native methods
     */
    private long nativeImageRelatedRes;

    /**
     * Pixelmap init the desity and mipmap data
     */
    private int baseDensity = getdefaultBaseDensity();

    private boolean useMipMap = false;

    private Size size;

    /**
     * A constructor used to create a {@link PixelMap} instance.
     *
     * @param nativeImagePixelMap the native object pointer.
     * @param nativeAllocBytes the native object size.
     * @since 1
     */
    protected PixelMap(long nativeImagePixelMap, long nativeAllocBytes) {
        this.nativeImagePixelMap = nativeImagePixelMap;
        String osName = System.getProperty("os.name");
        if (osName != null) {
            osName = osName.toUpperCase(Locale.US);
            if (!(osName.contains("WINDOWS") || osName.contains("MAC"))) {
                NativeAllocationNotifier.notifyMallocAllocation(PixelMap.class.getClassLoader(),
                    nativeGetFreeFunction(), nativeAllocBytes, this, nativeImagePixelMap);
            }
        } else {
            NativeAllocationNotifier.notifyMallocAllocation(PixelMap.class.getClassLoader(), nativeGetFreeFunction(),
            nativeAllocBytes, this, nativeImagePixelMap);
        }
    }

    /**
     * Private constructor called by JNI.
     *
     * @param nativeImagePixelMap the native object pointer.
     * @param nativeAllocBytes the native object size.
     * @param nativeImageRelatedRes the native related object pointer.
     */
    private PixelMap(long nativeImagePixelMap, long nativeAllocBytes, long nativeImageRelatedRes) {
        this(nativeImagePixelMap, nativeAllocBytes);
        this.nativeImageRelatedRes = nativeImageRelatedRes;
    }

    /**
     * Private constructor called by JNI.
     *
     * @param nativeImagePixelMap the native object pointer.
     * @param nativeAllocBytes the native object size.
     * @param nativeNinePatchChunk the native nine patch object pointer.
     */
    private PixelMap(long nativeImagePixelMap, long nativeAllocBytes, byte[] nativeNinePatchChunk) {
        this(nativeImagePixelMap, nativeAllocBytes);
        this.nativeNinePatchChunk = nativeNinePatchChunk;
    }

    /**
     * Creates a pixel map based on initialization options (such as the image size, pixel format, and alpha type) and
     * the data source described by a pixel color array.
     *
     * @param colors Indicates an int array of pixel colors. Each element in the array is in {@link
     * PixelFormat#ARGB_8888} format.
     * @param opts Indicates initialization options of the pixel map.
     * @return Returns the {@code PixelMap} instance if the creation is successful; returns {@code null} otherwise.
     * @throws IllegalArgumentException Throws this exception if any of the following occurs: <ul><li>The image size is
     * {@code null}.</li> <li>The width and height are less than or equal to {@code 0}.</li> <li>The length of the pixel
     * color array is less than the number of image pixels.</li></ul>
     * @since 3
     */
    public static PixelMap create(int[] colors, InitializationOptions opts) {
        if (Objects.isNull(opts) || Objects.isNull(opts.size)) {
            throw new IllegalArgumentException("initial options or size is null");
        }
        return doCreate(colors, 0, opts.size.width, opts);
    }

    /**
     * Creates a pixel map based on initialization options (such as the image size, pixel format, and alpha type) and
     * the data source described by a pixel color array, start offset, and number of pixels in a row.
     *
     * @param colors Indicates an int array of color values. Each element in the array is in {@link
     * PixelFormat#ARGB_8888} format.
     * @param offset Indicates the offset from where to read pixel color data in the array.
     * @param stride Indicates the number of pixel color elements in each row of the array. The value must be greater
     * than or equal to the width of the pixel map to be created.
     * @param opts Indicates initialization options of the pixel map.
     * @return Returns the {@code PixelMap} instance if the creation is successful; returns {@code null} otherwise.
     * @throws IllegalArgumentException Throws this exception if any of the following occurs: <ul><li>The image size is
     * {@code null}.</li> <li>The width and height are less than or equal to {@code 0}.</li> <li>The number of available
     * elements in the array is less than the number of image pixels.</li> <li>The value of {@code stride} is smaller
     * than the width of the pixel map.</li></ul>
     * @since 3
     */
    public static PixelMap create(int[] colors, int offset, int stride, InitializationOptions opts) {
        if (Objects.isNull(opts) || Objects.isNull(opts.size)) {
            throw new IllegalArgumentException("initial options or size is null");
        }
        return doCreate(colors, offset, stride, opts);
    }

    private static PixelMap doCreate(int[] colors, int offset, int stride, InitializationOptions opts) {
        if (Objects.isNull(colors) || colors.length <= 0) {
            throw new IllegalArgumentException("init colors is invalid");
        }
        int width = opts.size.width;
        int height = opts.size.height;
        if (width <= 0 || height <= 0) {
            throw new IllegalArgumentException("init size is invalid");
        }
        if (stride < width) {
            throw new IllegalArgumentException("init stride must be >= width");
        }
        long lastLine = (height - 1) * (long) stride + offset;
        if (offset < 0 || offset + width > colors.length || lastLine + width > colors.length) {
            throw new IllegalArgumentException("colors length is less than target pixelMap");
        }
        return nativeCreate(colors, offset, stride, opts.size.width, opts.size.height, opts.pixelFormat.getValue(),
            opts.alphaType.getValue(), opts.editable);
    }

    /**
     * Creates a pixel map based on initialization options (such as the image size, pixel format, and alpha type).
     *
     * @param opts Indicates initialization options of the pixel map.
     * @return Returns the {@code PixelMap} instance if the creation is successful; returns {@code null} otherwise.
     * @throws IllegalArgumentException Throws this exception if the image size is {@code null}, or the width and height
     * are less than or equal to {@code 0}.
     * @since 3
     */
    public static PixelMap create(InitializationOptions opts) {
        if (Objects.isNull(opts) || Objects.isNull(opts.size)) {
            throw new IllegalArgumentException("initial options or size is null");
        }
        if (opts.size.width <= 0 || opts.size.height <= 0) {
            throw new IllegalArgumentException("init size is invalid");
        }
        return nativeCreate(opts.size.width, opts.size.height, opts.pixelFormat.getValue(), opts.alphaType.getValue(),
            opts.editable);
    }

    /**
     * Creates a pixel map based on initialization options (such as the image size, pixel format, and alpha type) and
     * the data source described by a source pixel map.
     *
     * @param source Indicates the {@code PixelMap} instance as the data source.
     * @param opts Indicates initialization options of the pixel map. If the value is {@code null}, the target pixel map
     * will be created from a copy of the source pixel map.
     * @return Returns the new {@code PixelMap} instance if the creation is successful; returns {@code null} otherwise.
     * @throws IllegalArgumentException Throws this exception if any of the following occurs: <ul><li>The source pixel
     * map is {@code null}.</li> <li>Native resources associated with the source pixel map have been released.</li>
     * <li>The image width or height specified in {@code opts} is less than {@code 0}.</li></ul>
     * @since 3
     */
    public static PixelMap create(PixelMap source, InitializationOptions opts) {
        return create(source, null, opts);
    }

    /**
     * Creates a pixel map based on initialization options (such as the image size, pixel format, and alpha type) and
     * the data source described by a source pixel map and the expected area on it.
     *
     * @param source Indicates the source {@code PixelMap}.
     * @param srcRegion Indicates the expected region on the source pixel map.
     * @param opts Indicates initialization options of the pixel map. If this parameter is {@code null}, the new pixel
     * map will be a copy of the source pixel map.
     * @return Returns the new {@code PixelMap} instance if the creation is successful; returns {@code null} otherwise.
     * @throws IllegalArgumentException Throws this exception if any of the following occurs: <ul><li>The source pixel
     * map is {@code null}.</li> <li>Native resources associated with the source pixel map have been released.</li>
     * <li>The {@code srcRegion} is out of the source pixel map.</li> <li>The image width or height specified in {@code
     * opts} is less than {@code 0}.</li></ul>
     * @since 3
     */
    public static PixelMap create(PixelMap source, Rect srcRegion, InitializationOptions opts) {
        if (Objects.isNull(source) || source.isReleased()) {
            throw new IllegalArgumentException("the source pixelMap is invalid, maybe has released");
        }
        if (srcRegion == null) {
            srcRegion = new Rect();
        }
        if (opts == null) {
            opts = new InitializationOptions();
        }
        if (opts.size == null) {
            opts.size = new Size();
        }
        if (srcRegion.minX < 0 || srcRegion.minY < 0 || srcRegion.width < 0 || srcRegion.height < 0
            || opts.size.width < 0 || opts.size.height < 0) {
            throw new IllegalArgumentException("srcRegion or option size invalid");
        }
        PixelMap pixelMap = nativeCreate(source, srcRegion.minX, srcRegion.minY, srcRegion.width, srcRegion.height,
            opts.size.width, opts.size.height, opts.pixelFormat.getValue(), opts.alphaType.getValue(),
            opts.scaleMode.getValue(), opts.useSourceIfMatch, opts.editable);
        if (pixelMap != null && source.nativeImagePixelMap != pixelMap.nativeImagePixelMap && opts != null
            && opts.releaseSource && !opts.useSourceIfMatch) {
            LOGGER.debug("release source pixelMap");
            source.release();
        }
        return pixelMap;
    }

    /**
     * Obtains basic image information.
     *
     * <p>Basic image information is provided by {@link ImageInfo},
     * including the image size, pixel format, and color space.
     *
     * @return Returns an {@link ImageInfo} object.
     * @since 1
     */
    public ImageInfo getImageInfo() {
        return nativeGetImageInfo(nativeImagePixelMap);
    }

    /**
     * Releases native resources associated with an object.
     *
     * @since 1
     */
    public void release() {
        if (!isReleased()) {
            nativeRelease(nativeImagePixelMap, nativeImageRelatedRes);
            nativeImagePixelMap = 0L;
            nativeImageRelatedRes = 0L;
            nativeNinePatchChunk = null;
            size = null;
        }
    }

    @Override
    protected void finalize() throws Throwable {
        release();
        super.finalize();
    }

    /**
     * Obtains 9 patch chunk data of this {@code PixelMap}.
     *
     * @return Returns the 9 patch chunk data if any; returns {@code null} otherwise.
     * @since 3
     */
    public byte[] getNinePatchChunk() {
        return nativeNinePatchChunk;
    }

    /**
     * Checks whether native resources have been released.
     *
     * @return Returns {@code true} if resources are released; returns {@code false} otherwise.
     * @since 1
     */
    public boolean isReleased() {
        return nativeImagePixelMap == 0;
    }

    private void checkRelease(String errorMsg) {
        if (isReleased()) {
            throw new IllegalStateException(errorMsg);
        }
    }

    /**
     * Writes a {@link PixelMap} object into a {@link Parcel}.
     *
     * @param out Indicates the {@link Parcel} object to which the {@link PixelMap} object will be written.
     * @return Returns {@code true} if the marshalling is successful; returns {@code false} otherwise.
     * @since 1
     */
    public boolean marshalling(Parcel out) {
        return nativeWriteToParcel(nativeImagePixelMap, out);
    }

    /**
     * Restores a {@link PixelMap} object from a {@link Parcel}.
     *
     * @param in Indicates the {@link Parcel} object into which the {@link PixelMap} object has been written.
     * @return Returns {@code true} if the marshalling is successful; returns {@code false} otherwise.
     * @since 1
     */
    public boolean unmarshalling(Parcel in) {
        return false;
    }

    /**
     * Obtains the number of bytes in each row of pixels.
     *
     * @return Returns the number of bytes.
     * @throws IllegalStateException Throws this exception if native resources associated with this {@code PixelMap}
     * have been released.
     * @since 3
     */
    public int getBytesNumberPerRow() {
        checkRelease("getBytesNumberPerRow but current pixelmap had released.");
        return nativeGetBytesNumberPerRow(nativeImagePixelMap);
    }

    /**
     * Obtains the pixel data size in bytes.
     *
     * @return Returns the number of bytes of all the pixel data.
     * @throws IllegalStateException Throws this exception if native resources associated with this {@code PixelMap}
     * have been released.
     * @since 3
     */
    public long getPixelBytesNumber() {
        checkRelease("getBytesNumber but current pixelmap had released.");
        return nativeGetBytesNumber(nativeImagePixelMap);
    }

    /**
     * Obtains the memory capacity for storing the pixels of this {@code PixelMap}.
     *
     * @return Returns the memory capacity.
     * @throws IllegalStateException Throws this exception if native resources associated with this {@code PixelMap}
     * have been released.
     * @since 3
     */
    public long getPixelBytesCapacity() {
        checkRelease("getPixelBytesCapacity but current pixelmap had released.");
        return nativeGetPixelBytesCapacity(nativeImagePixelMap);
    }

    /**
     * Checks whether this {@code PixelMap} can be edited.
     *
     * @return Returns {@code true} if the pixel map is editable; returns {@code false} otherwise.
     * @throws IllegalStateException Throws this exception if native resources associated with this {@code PixelMap}
     * have been released.
     * @since 3
     */
    public boolean isEditable() {
        checkRelease("isEditable but current pixelmap had released.");
        return nativeIsEditable(nativeImagePixelMap);
    }

    /**
     * Checks whether this {@code PixelMap} is the same as the specified one, including the attribute information
     * described by the {@code ImageInfo} instance and pixel data.
     *
     * @param other Indicates the specified {@code PixelMap} to compare.
     * @return Returns {@code true} if the two images are the same; returns {@code false} otherwise.
     * @throws IllegalArgumentException Throws this exception if the specified {@code PixelMap} is empty.
     * @throws IllegalStateException Throws this exception if native resources associated with this {@code PixelMap}
     * have been released.
     * @since 3
     */
    public boolean isSameImage(PixelMap other) {
        if (other == null) {
            LOGGER.error("isSameImage input other pixelmap is null.");
            throw new IllegalStateException("other pixelmap object is null.");
        }
        checkRelease("isSameImage but current pixelmap had released.");
        return nativeIsSameImage(nativeImagePixelMap, other.nativeImagePixelMap);
    }

    /**
     * Reads the color value at the specified position.
     *
     * @param pos Indicates the coordinate of the pixel.
     * @return Returns the color value, which is of the {@link AlphaType#UNPREMUL} type and in {@link
     * PixelFormat#ARGB_8888} format.
     * @throws IllegalArgumentException Throws this exception if the passed coordinate position is {@code null} or out
     * of the valid range of the image.
     * @throws IllegalStateException Throws this exception if native resources associated with this {@code PixelMap}
     * have been released.
     * @since 3
     */
    public int readPixel(Position pos) {
        if (pos == null || pos.posX > MAX_DIMENSION || pos.posY > MAX_DIMENSION || pos.posX < 0 || pos.posY < 0) {
            LOGGER.error("readPixel pos object is invalid.");
            throw new IllegalArgumentException("read Pixelpos object is invalid.");
        }
        checkRelease("readPixel but current pixelmap had released.");
        return nativeReadPixel(nativeImagePixelMap, pos.posX, pos.posY);
    }

    /**
     * Reads the color values of a specified region and writes them into a {@code pixels} array with the specified start
     * offset and stride. The color values are of the {@link AlphaType#UNPREMUL} type and in {@link
     * PixelFormat#ARGB_8888} format.
     *
     * @param pixels Indicates the array storing the color value copies.
     * @param offset Indicates the offset into which the color values will be written.
     * @param stride Indicates the number of color values stored in each row of the array. The parameter value must be
     * greater than or equal to the width of the specified source region.
     * @param region Indicates the source region to obtain the color values.
     * @throws IllegalStateException Throws this exception if native resources associated with this {@code PixelMap}
     * have been released.
     * @throws IllegalArgumentException Throws this exception if any of the following occurs: <ul><li>The {@code pixels}
     * array is {@code null}.</li> <li>The {@code offset} value is less than {@code 0} or larger than the {@code pixels}
     * array length.</li> <li>The {@code stride} value is less than {@code 0}.</li> <li>The {@code region} value is
     * {@code null} or out of the valid range of the image.</li></ul>
     * @throws ArrayIndexOutOfBoundsException Throws this exception if the {@code pixels} array is too small to
     * accommodate the color values.
     * @since 3
     */
    public void readPixels(int[] pixels, int offset, int stride, Rect region) {
        if (region == null || region.height < 0 || region.height > MAX_DIMENSION || region.width < 0
            || region.width > MAX_DIMENSION) {
            LOGGER.error("readPixels region object is invalid.");
            throw new IllegalArgumentException("read Pixels region object is invalid.");
        }
        if (pixels == null) {
            LOGGER.error("readPixels pixels array is null.");
            throw new IllegalArgumentException("read Pixels array is null.");
        }
        if (offset < 0 || stride < 0 || offset > (pixels.length)) {
            LOGGER.error("readPixels offset or stride is invalid.");
            throw new IllegalArgumentException("read Pixels offset or stride is invalid.");
        }
        checkRelease("readPixels called read Pixels to int[] but the pixelmap is release.");
        if (!nativeReadPixels(nativeImagePixelMap, pixels, offset, stride, region.minX, region.minY, region.width,
            region.height)) {
            LOGGER.error("nativeReadPixels return fail.");
            throw new IllegalStateException("native readPixels fail.");
        }
    }

    /**
     * Reads a copy of color values of this {@code PixelMap} instance and stores it to the specified buffer.
     *
     * @param dst Indicates the buffer storing the color value copies.
     * @throws IllegalStateException Throws this exception if native resources associated with this {@code PixelMap}
     * have been released.
     * @throws IllegalArgumentException Throws this exception if the buffer is {@code null} or the format is not
     * supported.
     * @throws ArrayIndexOutOfBoundsException Throws this exception if the size of the color values exceeds the buffer
     * capacity.
     * @since 3
     */
    public void readPixels(Buffer dst) {
        if (dst == null) {
            LOGGER.error("readPixels Buffer is null.");
            throw new IllegalArgumentException("read pixels but Buffer is null");
        }
        checkRelease("called read Pixels to Buffer but the pixelmap is release.");
        int len = dst.remaining();
        int shift;
        if (dst instanceof ByteBuffer) {
            shift = 0; // byte buffer type is same pixel bytes
        } else if (dst instanceof ShortBuffer) {
            shift = 1; // short buffer type need buffer len << 1 bit
        } else if (dst instanceof IntBuffer) {
            shift = 2; // Int buffer type need buffer len << 2 bit
        } else {
            LOGGER.error("readPixels dst is invalid.");
            throw new IllegalArgumentException("unsupported Buffer subclass");
        }
        long dstSize = ((long) len) << shift;
        long curPixelSize = getPixelBytesNumber();

        if (dstSize < curPixelSize) {
            LOGGER.error("readPixels dstSize:%{public}d < curPixelSize:%{public}d.", dstSize, curPixelSize);
            throw new ArrayIndexOutOfBoundsException("Buffer not large enough for pixels");
        }

        if (!nativeReadPixels(nativeImagePixelMap, dst, dstSize)) {
            LOGGER.error("nativeImagePixelMap return fail.");
            throw new IllegalStateException("native readPixels fail.");
        }

        // now update the buffer's position
        int pos = dst.position();
        pos += curPixelSize >> shift;
        dst.position(pos);
    }

    /**
     * Resets the size and pixel format of this {@code PixelMap}. This method does not change the original pixel data,
     * or reallocate the memory for storing the pixel data.
     *
     * @param size Indicates the target image size after the reset, in bytes. The value must not exceed the memory
     * occupied by this {@code PixelMap}.
     * @param pixelFormat Indicates pixel format to reset.
     * @throws IllegalStateException Throws this exception if native resources associated with this {@code PixelMap}
     * have been released, or the {@code PixelMap} cannot be edited.
     * @throws IllegalArgumentException Throws this exception if the memory capacity required by the target image size
     * and pixel format exceeds the available capacity.
     * @since 3
     */
    public void resetConfig(Size size, PixelFormat pixelFormat) {
        if (size == null || size.height <= 0 || size.width <= 0 || size.height > MAX_DIMENSION
            || size.width > MAX_DIMENSION) {
            LOGGER.error("resetConfig input parameter invalid.");
            throw new IllegalArgumentException("reset config width and height must be > 0");
        }
        if (pixelFormat == null || pixelFormat == PixelFormat.UNKNOWN) {
            LOGGER.error("resetConfig pixelFormat invalid.");
            throw new IllegalArgumentException("reset config pixelFormat invalid");
        }
        checkRelease("reset pixelmap but it is release.");
        if (!nativeResetConfig(nativeImagePixelMap, size.width, size.height, pixelFormat.getValue())) {
            LOGGER.error("nativeReset return fail.");
            throw new IllegalStateException("native reset config fail.");
        }
    }

    /**
     * Sets the alpha type of this {@code PixelMap}.
     *
     * @param alphaType Indicates the {@code AlphaType} to set.
     * @throws IllegalStateException Throws this exception if native resources associated with this {@code PixelMap}
     * have been released, or the {@code PixelMap} cannot be edited.
     * @throws IllegalArgumentException Throws this exception if the input {@code AlphaType} is invalid.
     * @since 3
     */
    public void setAlphaType(AlphaType alphaType) {
        if (alphaType == null || alphaType == AlphaType.UNKNOWN) {
            LOGGER.error("setAlphaType input alphaType is unknown.");
            throw new IllegalArgumentException("set AlphaType is UNKNOWN.");
        }
        checkRelease("set current pixelmap Alpha but pixlmap is release.");
        if (!nativeSetAlphaType(nativeImagePixelMap, alphaType.getValue())) {
            LOGGER.error("nativeSetAlphaType return fail.");
            throw new IllegalArgumentException("set Alpha Type fail.");
        }
    }

    /**
     * Writes the color value into the specified position.
     *
     * @param pos Indicates the coordinate position to write the pixel value.
     * @param color Indicates the color value to write, which is of the {@link AlphaType#UNPREMUL} type and in {@link
     * PixelFormat#ARGB_8888} format.
     * @throws IllegalStateException Throws this exception if native resources associated with this {@code PixelMap}
     * have been released, or the {@code PixelMap} cannot be edited.
     * @throws IllegalArgumentException Throws this exception if the coordinate position is {@code null} or out of the
     * valid range of the image.
     * @since 3
     */
    public void writePixel(Position pos, int color) {
        if (pos == null || pos.posX < 0 || pos.posY < 0 || pos.posX > MAX_DIMENSION || pos.posY > MAX_DIMENSION) {
            LOGGER.error("writePixel input invalid.");
            throw new IllegalArgumentException("write pixel pos object is invalid");
        }
        checkRelease("write current pixelmap by color but pixlmap is release.");
        if (!nativeWritePixel(nativeImagePixelMap, pos.posX, pos.posY, color)) {
            LOGGER.error("nativeWritePixel return fail.");
            throw new IllegalStateException("native write pixel fail.");
        }
    }

    /**
     * Writes data from the specified color data array (based on the start offset and stride) into the specified region
     * of this {@code PixelMap}. The color data to write is of the {@link AlphaType#UNPREMUL} type and in {@link
     * PixelFormat#ARGB_8888} format.
     *
     * @param pixels Indicates the array of color data.
     * @param offset Indicates the offset from which the color data will be read.
     * @param stride Indicates the number of pixels in each row of the array. The value must be greater than or equal to
     * the width of the target region in this {@code PixelMap}.
     * @param region Indicates the target pixel map region into which the data will be written.
     * @throws IllegalStateException Throws this exception if native resources associated with this {@code PixelMap}
     * have been released, or the {@code PixelMap} cannot be edited.
     * @throws IllegalArgumentException Throws this exception if any of the following occurs: <ul><li>The {@code pixels}
     * array is {@code null}.</li> <li>The {@code offset} value is less than {@code 0} or larger than the {@code pixels}
     * array length.</li> <li>The {@code stride} value is less than {@code 0}.</li> <li>The {@code region} value is out
     * of the valid range of the image.</li></ul>
     * @since 3
     */
    public void writePixels(int[] pixels, int offset, int stride, Rect region) {
        if (region == null || region.height < 0 || region.height > MAX_DIMENSION || region.width < 0
            || region.width > MAX_DIMENSION) {
            LOGGER.error("writePixels region object is invalid.");
            throw new IllegalArgumentException("write pixels region object is invalid.");
        }
        if (pixels == null) {
            LOGGER.error("writePixels pixels array is null.");
            throw new IllegalArgumentException("write pixels array is null.");
        }
        if (offset < 0 || stride < 0 || offset > (pixels.length)) {
            LOGGER.error("writePixels offset or stride is invalid.");
            throw new IllegalArgumentException("write pixels offset or stride is invalid.");
        }
        checkRelease("writePixels from intArray but the pixelmap is release.");
        if (!nativeWritePixels(nativeImagePixelMap, pixels, offset, stride, region.minX, region.minY, region.width,
            region.height)) {
            LOGGER.error("nativeWritePixels return fail.");
            throw new IllegalStateException("native WritePixels from intArray fail.");
        }
    }

    /**
     * Writes the pixel data in the specified buffer into this {@code PixelMap}. The buffer data will overwrite the
     * {@code PixelMap} data, so the color format of the source data must be compatible with this {@code PixelMap}.
     *
     * @param src Indicates the buffer whose pixel data will be written.
     * @throws IllegalStateException Throws this exception if native resources associated with this {@code PixelMap}
     * have been released, or the {@code PixelMap} cannot be edited.
     * @throws IllegalArgumentException Throws this exception if the buffer is {@code null} or the format is not
     * supported.
     * @throws ArrayIndexOutOfBoundsException Throws this exception if the data in the buffer is insufficient to fill
     * the {@code PixelMap}.
     * @since 3
     */
    public void writePixels(Buffer src) {
        if (src == null) {
            LOGGER.error("writePixels Buffer is null.");
            throw new IllegalArgumentException("write pixels but Buffer is null");
        }
        checkRelease("writePixels from Buffer but the pixelmap is release.");
        int len = src.remaining();
        int shift;
        if (src instanceof ByteBuffer) {
            shift = 0; // byte buffer type is same pixel bytes
        } else if (src instanceof ShortBuffer) {
            shift = 1; // short buffer type need buffer len << 1 bit
        } else if (src instanceof IntBuffer) {
            shift = 2; // Int buffer type need buffer len << 2 bit
        } else {
            LOGGER.error("writePixels src is invalid.");
            throw new IllegalStateException("unsupported Buffer subclass");
        }
        long srcSize = ((long) len) << shift;
        long curPixelSize = getPixelBytesNumber();

        if (srcSize < curPixelSize) {
            LOGGER.error("writePixels srcSize:%{public}d < curPixelSize:%{public}d.", srcSize, curPixelSize);
            throw new ArrayIndexOutOfBoundsException("write pixels Buffer not large enough for pixels");
        }
        if (!nativeWritePixels(nativeImagePixelMap, src, srcSize)) {
            LOGGER.error("nativeWritePixels by Buffer return false.");
            throw new IllegalStateException("native WritePixels from Buffer fail.");
        }
        // now update the buffer's position
        int pos = src.position();
        pos += curPixelSize >> shift;
        src.position(pos);
    }

    /**
     * Writes the specified color value into this {@code PixelMap}.
     *
     * @param color Indicates the color value to write, which is of the {@link AlphaType#UNPREMUL} type and in {@link
     * PixelFormat#ARGB_8888} format.
     * @throws IllegalStateException Throws this exception if native resources associated with this {@code PixelMap}
     * have been released, or the {@code PixelMap} cannot be edited.
     * @since 3
     */
    public void writePixels(int color) {
        checkRelease("writePixels from int color but the pixelmap is release.");
        if (!nativeWritePixels(nativeImagePixelMap, color)) {
            LOGGER.error("nativeWritePixels by int color return false.");
            throw new IllegalStateException("nativeWritePixels from int color fail.");
        }
    }

    /**
     * Sets the base pixel density of this {@code PixelMap}.
     *
     * @param baseDensity Indicates the base pixel density to set.
     * @since 3
     */
    public void setBaseDensity(int baseDensity) {
        this.baseDensity = baseDensity;
    }

    /**
     * Obtains the base pixel density of this {@code PixelMap}.
     *
     * @return Returns the base pixel density.
     * @since 3
     */
    public int getBaseDensity() {
        return baseDensity;
    }

    /**
     * Sets whether MIP mapping will be used for rendering this {@code PixelMap}.
     *
     * @param useMipmap Specifies whether to use MIP mapping. Set it to {@code true} if the renderer will use MIP
     * mapping and to {@code false} otherwise.
     * @since 3
     */
    public final void setUseMipmap(boolean useMipmap) {
        this.useMipMap = useMipmap;
    }

    /**
     * Checks whether MIP mapping will be used for rendering this {@code PixelMap}.
     *
     * @return Returns {@code true} if the renderer will try MIP mapping; returns {@code false} otherwise.
     * @throws IllegalStateException Throws this exception if native resources associated with this {@code PixelMap}
     * have been released.
     * @since 3
     */
    public final boolean useMipmap() {
        checkRelease("call useMipmap() but the pixelmap is release.");
        return useMipMap;
    }

    /**
     * Obtains the image size that fits the specified pixel density.
     *
     * @param targetDensity Indicates the pixel density.
     * @return Returns the image size fitting the specified pixel density.
     * @throws IllegalStateException Throws this exception if native resources associated with this {@code PixelMap}
     * have been released.
     * @since 3
     */
    public Size getFitDensitySize(int targetDensity) {
        if (size == null) {
            checkRelease("PixelMap has been released");
            ImageInfo imageInfo = getImageInfo();
            if (imageInfo == null) {
                LOGGER.error("getFitDensitySize getImageInfo failed.");
                throw new IllegalStateException("PixelMap does not contain valid info.");
            }
            size = imageInfo.size;
        }
        int sourceDensity = baseDensity;
        if (sourceDensity == DENSITY_NONE || targetDensity == DENSITY_NONE || sourceDensity == targetDensity) {
            return size;
        }

        int targetHeight = (size.height * targetDensity + (sourceDensity >> 1)) / sourceDensity;
        int targetWidth = (size.width * targetDensity + (sourceDensity >> 1)) / sourceDensity;
        return new Size(targetWidth, targetHeight);
    }

    /**
     * Creates a {@code PixelMap} using the alpha channel of this {@code PixelMap}.
     *
     * @return Returns the new {@code PixelMap} containing only the alpha channel.
     * @since 4
     */
    public PixelMap createFromAlpha() {
        return nativeCreateFromAlpha(nativeImagePixelMap);
    }

    private int getdefaultBaseDensity() {
        int realMaxDeviceDensity = Parameters.getInt("ro.sf.real_lcd_density",
            Parameters.getInt("ro.sf.lcd_density", DENSITY_DEFAULT));
        if (Parameters.getInt("persist.sys.rog.width", 0) > 0) {
            int dpi = Parameters.getInt("persist.sys.dpi", realMaxDeviceDensity);
            return Parameters.getInt("persist.sys.realdpi", dpi);
        }
        int dpi = Parameters.getInt("persist.sys.dpi", 0);
        if ((dpi > 0) && (dpi != realMaxDeviceDensity)) {
            return dpi;
        }
        return Parameters.getInt("qemu.sf.lcd_density", realMaxDeviceDensity);
    }

    private native void nativeRelease(long nativeImagePixelMap, long nativeImageRelatedRes);

    private native ImageInfo nativeGetImageInfo(long nativeImagePixelMap);

    private native boolean nativeWriteToParcel(long nativeImagePixelMap, Parcel parcel);

    private static native PixelMap nativeCreateFromParcel(Parcel parcel);

    private static native PixelMap nativeCreate(int[] colors, int offset, int stride, int width, int height,
        int pixelFormat, int alphaType, boolean editable);

    private static native PixelMap nativeCreate(int width, int height, int pixelFormat, int alphaType,
        boolean editable);

    private static native PixelMap nativeCreate(PixelMap srcPixelMap, int srcRectX, int srcRectY, int srcRectWidth,
        int srcRectHeight, int width, int height, int pixelFormat, int alphaType, int scaleMode,
        boolean useSourceIfMatch, boolean editable);

    private native int nativeGetBytesNumberPerRow(long nativePixelMap);

    private native long nativeGetBytesNumber(long nativePixelMap);

    private native long nativeGetPixelBytesCapacity(long nativePixelMap);

    private native boolean nativeIsEditable(long nativePixelMap);

    private native boolean nativeIsSameImage(long nativePixelMap, long otherNativePixelMap);

    private native int nativeReadPixel(long nativePixelMap, int x, int y);

    private native boolean nativeReadPixels(long nativePixelMap, int[] pixels, int offset, int stride, int x, int y,
        int width, int height);

    private native boolean nativeReadPixels(long nativePixelMap, Buffer dst, long dstSize);

    private native boolean nativeResetConfig(long nativePixelMap, int width, int height, int pixelFormat);

    private native boolean nativeSetAlphaType(long nativePixelMap, int alphaType);

    private native boolean nativeWritePixel(long nativePixelMap, int x, int y, int color);

    private native boolean nativeWritePixels(long nativePixelMap, int[] pixels, int offset, int stride, int x, int y,
        int width, int height);

    private native boolean nativeWritePixels(long nativePixelMap, Buffer source, long srcSize);

    private native boolean nativeWritePixels(long nativePixelMap, int color);

    private static native long nativeGetFreeFunction();

    private native PixelMap nativeCreateFromAlpha(long nativePixelMap);

    /**
     * Represents pixel map initialization options, including the image size, pixel format, and alpha type.
     *
     * @since 3
     */
    public static class InitializationOptions {
        /**
         * Indicates the expected size of the pixel map to be created. If the value is {@code null} and there is a
         * source pixel map, the size of the source pixel map will be used for the creation.
         *
         * @since 3
         */
        public Size size;

        /**
         * Indicates the scaling effect used when the aspect ratio of the original image is different from that of the
         * target image. The default value is {@link ScaleMode#FIT_TARGET_SIZE}.
         *
         * @since 3
         */
        public ScaleMode scaleMode = ScaleMode.FIT_TARGET_SIZE;

        /**
         * Indicates the expected format of the pixel map to be created. When the value is {@link PixelFormat#UNKNOWN}:
         * <ul><li> If there is a source pixel map, its format will be used for the creation.</li> <li>If there is no
         * source pixel map, {@link PixelFormat#ARGB_8888} will be used.</li></ul>.
         *
         * @since 3
         */
        public PixelFormat pixelFormat = PixelFormat.UNKNOWN;

        /**
         * Indicates the expected alpha type of the pixel map to be created. When the value is {@link
         * AlphaType#UNKNOWN}: <ul><li>If there is a source pixel map, its alpha type will be used for the
         * creation.</li> <li>If there is no source pixel map, either {@link AlphaType#PREMUL} (for the alpha channel is
         * included in the pixel format) or {@link AlphaType#OPAQUE} (for the alpha channel not included in the pixel
         * format) will be used for the creation.</li></ul>
         *
         * @since 3
         */
        public AlphaType alphaType = AlphaType.UNKNOWN;

        /**
         * Specifies whether pixel values of the pixel map to be created can be edited. Value {@code true} means that
         * the values can be edited, and {@code false} means the opposite. The default value is {@code false}.
         *
         * @since 3
         */
        public boolean editable = false;

        /**
         * Specifies whether the source pixel map will be directly returned as a new one when the source one matches the
         * initialization options for creating the new one. Value {@code true} means that the source pixel map will be
         * returned, and {@code false} means the opposite. The default value is {@code false}.
         *
         * @since 3
         */
        public boolean useSourceIfMatch = false;

        /**
         * Specifies whether the source pixel map will be automatically released after a new one is created based on the
         * source one. Value {@code true} means that the source one will be released except that it is directly returned
         * as the new pixel map, and {@code false} means that the source one will not be released.
         *
         * @since 3
         */
        public boolean releaseSource = false;
    }
}