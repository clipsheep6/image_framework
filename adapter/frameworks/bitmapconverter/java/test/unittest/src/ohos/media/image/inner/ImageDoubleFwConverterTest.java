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

package ohos.media.image.inner;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotNull;
import static org.junit.Assert.assertTrue;
import static org.junit.Assert.fail;

import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.os.Parcel;

import ohos.media.image.ImageSource;
import ohos.media.image.PixelMap;
import ohos.media.image.common.AllocatorType;
import ohos.media.image.common.PixelFormat;
import ohos.media.image.common.Position;
import ohos.media.image.common.Rect;
import ohos.media.image.common.Size;
import ohos.rpc.MessageParcel;
import ohos.media.utils.log.Logger;
import ohos.media.utils.log.LoggerFactory;
import ohos.unittest.CaseLevel;
import ohos.unittest.CaseType;
import ohos.unittest.Level;
import ohos.unittest.Type;

import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;

import java.io.File;
import java.nio.ByteBuffer;
import java.util.Arrays;

/**
 * ImageDoubleFwConverterTest, test cases for ImageDoubleFwConverter class,
 * mainly including test cases for PixelMap and Bitmap conversion.
 *
 * @since 1
 */
public class ImageDoubleFwConverterTest {
    private static final Logger LOGGER = LoggerFactory.getImageLogger(ImageDoubleFwConverterTest.class);

    private static File pngFile;

    static {
        try {
            System.loadLibrary("ipc_core.z");
        } catch (UnsatisfiedLinkError | NullPointerException e) {
            LOGGER.error("loadLibrary ipc_core.z fail");
        }
    }

    private ImageSource imageSource;

    private PixelMap pixelMap;

    private ImageDoubleFwConverter converter = new ImageDoubleFwConverter();

    /**
     * Action before all test case.
     */
    @BeforeClass
    public static void setUpBeforeClass() throws Exception {
        pngFile = new File("/sdcard/multimedia/image/test.png");
        if (!pngFile.exists()) {
            LOGGER.error("test file not exist.");
            fail("files not exist");
        }
    }

    /**
     * Action after all test case.
     */
    @AfterClass
    public static void tearDownAfterClass() throws Exception {
    }

    /**
     * Action before test case.
     */
    @Before
    public void setUp() throws Exception {
    }

    /**
     * Action after test case.
     */
    @After
    public void tearDown() throws Exception {
        try {
            if (imageSource != null) {
                imageSource.release();
                imageSource = null;
            }
            if (pixelMap != null) {
                pixelMap.release();
                pixelMap = null;
            }
        } catch (Throwable throwable) {
            LOGGER.error("test case teardown fail, %{public}s", throwable.getMessage());
        }
    }

    /**
     * @tc.name: testImageDoubleFwConverter001
     * @tc.desc: convert between Bitmap and PixelMap.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testImageDoubleFwConverter001() {
        /**
         * @tc.steps: step1.get pixelmap from path.
         * @tc.expected: step1.get image source form path not null
         */
        ImageSource.SourceOptions srcOpts = new ImageSource.SourceOptions();
        srcOpts.formatHint = "image/png";
        String pathName = "/sdcard/multimedia/image/test.png";
        imageSource = ImageSource.create(pathName, srcOpts);
        assertNotNull(imageSource);

        ImageSource.DecodingOptions decodingOpts = new ImageSource.DecodingOptions();
        decodingOpts.desiredSize = new Size(0, 0);
        decodingOpts.desiredRegion = new Rect(0, 0, 0, 0);
        decodingOpts.desiredPixelFormat = PixelFormat.ARGB_8888;
        pixelMap = imageSource.createPixelmap(decodingOpts);
        assertNotNull(pixelMap);

        /**
         * @tc.steps: step2.convert pixelmap to bitmap and convert bitmap to pixelmap back.
         * @tc.expected: step2.conversion ok
         */
        Bitmap bitmap = ImageDoubleFwConverter.createShadowBitmap(pixelMap);
        assertNotNull(bitmap);
        assertNotNull(bitmap.getConfig());

        Bitmap bitmap1 = bitmap.copy(bitmap.getConfig(), false);
        assertNotNull(bitmap1);

        Bitmap bitmap2 = BitmapFactory.decodeFile(pathName);
        PixelMap pixelMapNew = ImageDoubleFwConverter.createShellPixelMap(bitmap2);
        assertNotNull(pixelMapNew);

        assertEquals(472, pixelMapNew.getImageInfo().size.width);
        assertEquals(75, pixelMapNew.getImageInfo().size.height);
    }

    /**
     * @tc.name: testImageDoubleFwConverter002
     * @tc.desc: convert between Bitmap and PixelMap.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testImageDoubleFwConverter002() {
        /**
         * @tc.steps: step1.get pixelmap from path.
         * @tc.expected: step1.get image source form path not null
         */
        ImageSource.SourceOptions srcOpts = new ImageSource.SourceOptions();
        srcOpts.formatHint = "image/png";
        String pathName = "/sdcard/multimedia/image/test_large.webp";
        imageSource = ImageSource.create(pathName, srcOpts);
        assertNotNull(imageSource);

        /**
         * @tc.steps: step2.decode pixelmap.
         * @tc.expected: step2.decode ok
         */
        ImageSource.DecodingOptions decodingOpts = new ImageSource.DecodingOptions();
        decodingOpts.desiredSize = new Size(0, 0);
        decodingOpts.desiredRegion = new Rect(0, 0, 0, 0);
        decodingOpts.desiredPixelFormat = PixelFormat.ARGB_8888;
        pixelMap = imageSource.createPixelmap(decodingOpts);
        assertNotNull(pixelMap);

        /**
         * @tc.steps: step3.read pixel map by pos.
         * @tc.expected: step3.read ok
         */
        int zColor = pixelMap.readPixel(new Position(360, 342));
        byte[] zBytes = new byte[4];
        zBytes[3] = (byte) (zColor & 0xff); // B 87
        zBytes[2] = (byte) (zColor >> 8 & 0xff); // G 248
        zBytes[1] = (byte) (zColor >> 16 & 0xff); // R 243
        zBytes[0] = (byte) (zColor >> 24 & 0xff); // A 255

        assertEquals(zBytes[0], (byte)-1);
        assertEquals(zBytes[1], (byte)63);
        assertEquals(zBytes[2], (byte)71);
        assertEquals(zBytes[3], (byte)-51);

        /**
         * @tc.steps: step4.convert pixelmap to bitmap and read same pos.
         * @tc.expected: step4.pixelmap's color is same of bitmap's color
         */
        Bitmap bitmap = ImageDoubleFwConverter.createShadowBitmap(pixelMap);
        assertNotNull(bitmap);
        assertNotNull(bitmap.getConfig());
        int aColor = bitmap.getPixel(360, 342);
        assertEquals(zColor, aColor);

        /**
         * @tc.steps: step5.convert bitmap to pixelmap and read same pos.
         * @tc.expected: step5.pixelmapback's color is same of pixelmap's color
         */
        PixelMap pixelMapBack = ImageDoubleFwConverter.createShellPixelMap(bitmap);
        int zColorBack = pixelMapBack.readPixel(new Position(360, 342));
        assertEquals(zColor, zColorBack);

        /**
         * @tc.steps: step6.copy bitmap and read same pos.
         * @tc.expected: step6.bitmapcopy's color is same of pixelmap's color
         */
        Bitmap bitmap1 = bitmap.copy(bitmap.getConfig(), false);
        assertNotNull(bitmap1);
        int colorCopy = bitmap1.getPixel(360, 342);
        assertEquals(zColor, colorCopy);
        assertEquals(aColor, colorCopy);

        /**
         * @tc.steps: step7.decode bitmap and convert to pixelmap then read same pos.
         * @tc.expected: step7.bitmap's color is same of pixelmap's color
         */
        Bitmap bitmap2 = BitmapFactory.decodeFile(pathName);
        PixelMap pixelMapNew = ImageDoubleFwConverter.createShellPixelMap(bitmap2);
        assertNotNull(pixelMapNew);
        int aColor2 = bitmap2.getPixel(360, 342);
        int zColor2 = pixelMapNew.readPixel(new Position(360, 342));
        assertEquals(aColor2, zColor2);
    }

    /**
     * @tc.name: testImageDoubleFwConverter003
     * @tc.desc: PixelMap write to parcel.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testImageDoubleFwConverter003() {
        /**
        * @tc.steps: step1.get pixelmap from path.
        * @tc.expected: step1.get image source form path not null
        */
        ImageSource.SourceOptions srcOpts = new ImageSource.SourceOptions();
        srcOpts.formatHint = "image/png";
        String pathName = "/sdcard/multimedia/image/test_large.webp";
        imageSource = ImageSource.create(pathName, srcOpts);
        assertNotNull(imageSource);

        /**
        * @tc.steps: step2.create pixelmap and bitmap.
        * @tc.expected: step2.create ok
        */
        ImageSource.DecodingOptions decodingOpts = new ImageSource.DecodingOptions();
        decodingOpts.desiredSize = new Size(0, 0);
        decodingOpts.desiredRegion = new Rect(0, 0, 0, 0);
        decodingOpts.desiredPixelFormat = PixelFormat.ARGB_8888;
        pixelMap = imageSource.createPixelmap(decodingOpts);
        assertNotNull(pixelMap);
        Bitmap bitmap = ImageDoubleFwConverter.createShadowBitmap(pixelMap);
        assertNotNull(bitmap);

        /**
         * @tc.steps: step3.write bitmap to android parcel, pixelMap to MessageParcel, compare size.
         * @tc.expected: step3.size equals
         */
        android.os.Parcel aParcel = android.os.Parcel.obtain();
        bitmap.writeToParcel(aParcel, 0);
        int aParcelSize = aParcel.dataSize();

        MessageParcel zParcel = MessageParcel.obtain();
        ImageDoubleFwConverter.writeToParcel(pixelMap, zParcel);
        int zParcelSize = zParcel.getSize();

        assertEquals(zParcelSize, aParcelSize);

        aParcel.recycle();
        zParcel.reclaim();
    }

    /**
     * @tc.name: testImageDoubleFwConverter004
     * @tc.desc: PixelMap write to parcel using shared memory.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testImageDoubleFwConverter004() {
        /**
        * @tc.steps: step1.get pixelmap from path.
        * @tc.expected: step1.get image source form path not null
        */
        ImageSource.SourceOptions srcOpts = new ImageSource.SourceOptions();
        srcOpts.formatHint = "image/png";
        String pathName = "/sdcard/multimedia/image/test.png";
        imageSource = ImageSource.create(pathName, srcOpts);
        assertNotNull(imageSource);

        /**
        * @tc.steps: step2.create pixelmap and bitmap.
        * @tc.expected: step2.create ok
        */
        ImageSource.DecodingOptions decodingOpts = new ImageSource.DecodingOptions();
        decodingOpts.desiredSize = new Size(0, 0);
        decodingOpts.desiredRegion = new Rect(0, 0, 0, 0);
        decodingOpts.desiredPixelFormat = PixelFormat.ARGB_8888;
        decodingOpts.allocator = AllocatorType.SHARED_MEMORY;
        pixelMap = imageSource.createPixelmap(decodingOpts);
        assertNotNull(pixelMap);

        Bitmap bitmap = ImageDoubleFwConverter.createShadowBitmap(pixelMap);
        assertNotNull(bitmap);

        assertEquals(Bitmap.Config.ARGB_8888, bitmap.getConfig());
        assertEquals(472, bitmap.getWidth());
        assertEquals(75, bitmap.getHeight());

        int pixelMapDataSize = (int) pixelMap.getPixelBytesCapacity();
        ByteBuffer pixelMapByteBuffer = ByteBuffer.allocate(pixelMapDataSize);
        pixelMap.readPixels(pixelMapByteBuffer);

        int bitMapdataSize = bitmap.getByteCount();
        ByteBuffer bitmapByteBuffer = ByteBuffer.allocate(bitMapdataSize);
        bitmap.copyPixelsToBuffer(bitmapByteBuffer);

        assertEquals(0, pixelMapByteBuffer.compareTo(bitmapByteBuffer));
    }

}
