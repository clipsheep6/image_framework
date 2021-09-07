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

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertFalse;
import static org.junit.Assert.assertNotNull;
import static org.junit.Assert.assertNull;
import static org.junit.Assert.assertTrue;
import static org.junit.Assert.assertThrows;
import static org.junit.Assert.assertNotEquals;

import ohos.media.image.common.AllocatorType;
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
import ohos.rpc.MessageParcel;
import ohos.unittest.CaseLevel;
import ohos.unittest.CaseType;
import ohos.unittest.Level;
import ohos.unittest.Type;
import ohos.utils.Parcel;

import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Rule;
import org.junit.Test;
import org.junit.rules.ExpectedException;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.FileReader;
import java.io.IOException;
import java.io.OutputStream;
import java.nio.IntBuffer;
import java.nio.ByteBuffer;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;
import java.util.stream.Collectors;
import java.util.stream.IntStream;

/**
 * ImagePixelTest, test cases for image pixelMap class.
 *
 * @since 1
 */
public class ImagePixelMapKitTest {
    private static final Logger LOGGER = LoggerFactory.getImageLogger(ImagePixelMapKitTest.class);

    private static final String COLORS_DATA = "/sdcard/multimedia/image/colors.txt";

    private static final String SRC_RESULT = "/sdcard/multimedia/image/test/colors_test.jpeg";

    private static final String FIT_TARGET_SIZE_RESULT = "/sdcard/multimedia/image/test/fit_target_size.jpeg";

    private static final String CENTER_CROP_RESULT = "/sdcard/multimedia/image/test/center_crop.jpeg";

    private static final String COPYED_RESULT = "/sdcard/multimedia/image/test/copyed_test.jpeg";

    private static final String TEST_PNG_IMAGE_DIR = "/sdcard/multimedia/image/test.png";

    private static final String PHONE_DEVICE = "default";

    private static final String DEVICE_TYPE = "ro.build.characteristics";

    private static int DEFAULT_DENSITY;

    private static int[] myColors;

    static {
        try {
            System.loadLibrary("ipc_core.z");
        } catch (UnsatisfiedLinkError | NullPointerException e) {
            LOGGER.error("loadLibrary ipc_core.z fail");
        }
    }

    private boolean isPhoneDevice = false;

    private int[] defaultColors;

    private Size defaultSize;

    private PixelMap.InitializationOptions initializationOptions;

    /**
     * Action before all test case.
     */
    @BeforeClass
    public static void setUpBeforeClass() throws Exception {
        myColors = constructColors();
        File file = new File(FIT_TARGET_SIZE_RESULT);
        if (!file.getParentFile().exists()) {
            file.getParentFile().mkdir();
        }
        PixelMap.InitializationOptions options = new PixelMap.InitializationOptions();
        options.size = new Size(100, 200);
        PixelMap pixelMap = PixelMap.create(myColors, options);
        DEFAULT_DENSITY = pixelMap.getBaseDensity();
    }

    /**
     * Action after all test case.
     */
    @AfterClass
    public static void tearDownAfterClass() throws Exception {
        myColors = null;
    }

    /**
     * Action before test case.
     */
    @Before
    public void setUp() {
        defaultColors = new int[] {5, 5, 5, 5, 6, 6, 3, 3, 3, 0};
        initializationOptions = new PixelMap.InitializationOptions();
        defaultSize = new Size(3, 2);
        initializationOptions.size = defaultSize;
        initializationOptions.pixelFormat = PixelFormat.ARGB_8888;
    }

    /**
     * @tc.name: testImagePixelMap001
     * @tc.desc: create pxielMap object with correct parameters.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testImagePixelMap001() {
        /**
         * @tc.steps: step1. create pixelMap use correct parameters.
         * @tc.expected: step1. get the valid pixelMap.
         */
        int offset = 0;
        int stride = 4;
        PixelMap pixelMap = PixelMap.create(defaultColors, offset, stride, initializationOptions);
        assertNotNull(pixelMap);
        /**
         * @tc.steps: step2. check the params are correct.
         * @tc.expected: step2. the params is valid.
         */
        ImageInfo imageInfo = pixelMap.getImageInfo();
        assertEquals(defaultSize.width, imageInfo.size.width);
        assertEquals(defaultSize.height, imageInfo.size.height);
        assertEquals(PixelFormat.ARGB_8888, imageInfo.pixelFormat);
        assertEquals(AlphaType.PREMUL, imageInfo.alphaType);
        pixelMap.release();
    }

    /**
     * @tc.name: testImagePixelMap002
     * @tc.desc: create pixelMap with wrong parameters.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testImagePixelMap002() {
        /**
         * @tc.steps: step1. create pixelMap use wrong parameters.
         * @tc.expected: step1. get the pixelMap null.
         */
        int offset = 5;
        int stride = 5;
        PixelMap pixelMap = null;
        try {
            pixelMap = PixelMap.create(defaultColors, offset, stride, initializationOptions);
        } catch (IllegalArgumentException e) {
            LOGGER.error("params invalid");
        }
        assertNull(pixelMap);
    }

    /**
     * @tc.name: testImagePixelMap003
     * @tc.desc: create pixelMap use default offset and stride.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testImagePixelMap003() {
        /**
         * @tc.steps: step1. create pixelMap use default offset and stride.
         * @tc.expected: step1. get the valid pixelMap.
         */
        PixelMap pixelMap = PixelMap.create(defaultColors, initializationOptions);
        assertNotNull(pixelMap);
        pixelMap.release();
    }

    /**
     * @tc.name: testImagePixelMap004
     * @tc.desc: create pixelMap use wrong colors parameter.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testImagePixelMap004() {
        /**
         * @tc.steps: step1. create pixelMap use wrong colors parameter.
         * @tc.expected: step1. get the pixelMap null.
         */
        PixelMap pixelMap = null;
        try {
            pixelMap = PixelMap.create(new int[] {}, initializationOptions);
        } catch (IllegalArgumentException e) {
            LOGGER.error("params invalid");
        }
        assertNull(pixelMap);
    }

    /**
     * @tc.name: testImagePixelMap005
     * @tc.desc: create pixelMap only use initialization options
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testImagePixelMap005() {
        /**
         * @tc.steps: step1. create pixelMap only use initialization options.
         * @tc.expected: step1. get the valid pixelMap.
         */
        PixelMap pixelMap = PixelMap.create(initializationOptions);
        assertNotNull(pixelMap);
        ImageInfo imageInfo = pixelMap.getImageInfo();
        assertEquals(defaultSize.width, imageInfo.size.width);
        assertEquals(defaultSize.height, imageInfo.size.height);
        assertEquals(PixelFormat.ARGB_8888, imageInfo.pixelFormat);
        pixelMap.release();
    }

    /**
     * @tc.name: testImagePixelMap006
     * @tc.desc: create pixelMap only use wrong initialization options.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testImagePixelMap006() {
        /**
         * @tc.steps: step1. create pixelMap only use wrong initialization options.
         * @tc.expected: step1. get the pixelMap null.
         */
        initializationOptions.size.height = 0;
        initializationOptions.size.width = 0;
        PixelMap pixelMap = null;
        try {
            pixelMap = PixelMap.create(initializationOptions);
        } catch (IllegalArgumentException e) {
            LOGGER.error("params invalid");
        }
        assertNull(pixelMap);
    }

    /**
     * @tc.name: testImagePixelMap007
     * @tc.desc: create scale fit target size pixelMap use src pixelMap.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testImagePixelMap007() {
        /**
         * @tc.steps: step1. create src pixelMap use file data.
         * @tc.expected: step1. get the valid pixelMap.
         */
        PixelMap source = createSrcPixelMap();
        assertNotNull(source);
        /**
         * @tc.steps: step2. use source pixelMap create new scale fit target size pixelMap.
         * @tc.expected: step2. check the target picture is valid.
         */
        initializationOptions.size.height = 200;
        initializationOptions.size.width = 100;
        PixelMap copyPixelMap = PixelMap.create(source, initializationOptions);
        assertNotNull(copyPixelMap);
        ImageInfo copyImageInfo = copyPixelMap.getImageInfo();
        assertEquals(initializationOptions.size.width, copyImageInfo.size.width);
        assertEquals(initializationOptions.size.height, copyImageInfo.size.height);
        imageEncode(copyPixelMap, FIT_TARGET_SIZE_RESULT);
        copyPixelMap.release();
        source.release();
    }

    /**
     * @tc.name: testImagePixelMap008
     * @tc.desc: create center crop pixelMap use src pixelMap.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testImagePixelMap008() {
        /**
         * @tc.steps: step1. create src pixelMap use file data.
         * @tc.expected: step1. get the valid pixelMap.
         */
        PixelMap source = createSrcPixelMap();
        assertNotNull(source);
        /**
         * @tc.steps: step2. use source pixelMap create new scale fit target size pixelMap.
         * @tc.expected: step2. check the target picture is valid.
         */
        initializationOptions.size.height = 500;
        initializationOptions.size.width = 800;
        initializationOptions.scaleMode = ScaleMode.CENTER_CROP;
        initializationOptions.releaseSource = true;
        PixelMap copyPixelMap = PixelMap.create(source, initializationOptions);
        assertNotNull(copyPixelMap);
        ImageInfo copyImageInfo = copyPixelMap.getImageInfo();
        assertEquals(initializationOptions.size.width, copyImageInfo.size.width);
        assertEquals(initializationOptions.size.height, copyImageInfo.size.height);
        imageEncode(copyPixelMap, CENTER_CROP_RESULT);
        copyPixelMap.release();
        source.release();
    }

    /**
     * @tc.name: testImagePixelMap009
     * @tc.desc: create pixelMap use source options and modify pixelFormat.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testImagePixelMap009() {
        /**
         * @tc.steps: step1. create src pixelMap use file data.
         * @tc.expected: step1. get the valid pixelMap.
         */
        PixelMap source = createSrcPixelMap();
        assertNotNull(source);
        /**
         * @tc.steps: step2. create pixelMap use source options and modify pixelFormat.
         * @tc.expected: step2. check the target is valid.
         */
        PixelMap.InitializationOptions defaultOpts = new PixelMap.InitializationOptions();
        defaultOpts.pixelFormat = PixelFormat.ARGB_8888;
        PixelMap copyedPixelMap = PixelMap.create(source, defaultOpts);
        assertNotNull(copyedPixelMap);
        ImageInfo sourceImageInfo = source.getImageInfo();
        ImageInfo copyedImageInfo = copyedPixelMap.getImageInfo();
        assertEquals(sourceImageInfo.size.width, copyedImageInfo.size.width);
        assertEquals(sourceImageInfo.size.height, copyedImageInfo.size.height);
        assertEquals(defaultOpts.pixelFormat, copyedImageInfo.pixelFormat);
        source.release();
        copyedPixelMap.release();
    }

    /**
     * @tc.name: testImagePixelMap010
     * @tc.desc: create rect pixelMap use src pixelMap.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testImagePixelMap010() {
        /**
         * @tc.steps: step1. create src pixelMap use file data.
         * @tc.expected: step1. get the valid pixelMap.
         */
        initializationOptions.editable = true;
        PixelMap source = createSrcPixelMap();
        assertNotNull(source);
        /**
         * @tc.steps: step2. create copied pixelMap and use rect and scale source operation.
         * @tc.expected: step2. check the target picture is valid.
         */
        Rect rect = new Rect(0, 0, 50, 100);
        PixelMap.InitializationOptions defaultOpts = new PixelMap.InitializationOptions();
        defaultOpts.useSourceIfMatch = true;
        defaultOpts.editable = true;
        defaultOpts.size = new Size(600, 800);
        PixelMap copyedPixelMap = PixelMap.create(source, rect, defaultOpts);
        assertNotNull(copyedPixelMap);
        ImageInfo copyedImageInfo = copyedPixelMap.getImageInfo();
        ImageInfo sourceImageInfo = source.getImageInfo();
        assertEquals(sourceImageInfo.size.width, 300);
        assertEquals(copyedImageInfo.size.width, 600);
        copyedPixelMap.release();
        source.release();
    }

    /**
     * @tc.name: testImagePixelMap011
     * @tc.desc: create pixelMap use null initialization Options.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testImagePixelMap011() {
        /**
         * @tc.steps: step1. create src pixelMap use file data.
         * @tc.expected: step1. get the valid pixelMap.
         */
        initializationOptions.editable = false;
        PixelMap source = createSrcPixelMap();
        assertNotNull(source);
        /**
         * @tc.steps: step2. use null initializationOptions generate new pixelmap like source.
         * @tc.expected: step2. check the target is valid.
         */
        PixelMap copyedPixelMap = PixelMap.create(source, null);
        assertNotNull(copyedPixelMap);
        ImageInfo copyedImageInfo = copyedPixelMap.getImageInfo();
        ImageInfo sourceImageInfo = source.getImageInfo();
        assertEquals(sourceImageInfo.size.width, copyedImageInfo.size.width);
        assertEquals(sourceImageInfo.size.height, copyedImageInfo.size.height);
        imageEncode(copyedPixelMap, COPYED_RESULT);
        source.release();
        copyedPixelMap.release();
    }

    /**
     * @tc.name: testImagePixelMap012
     * @tc.desc: create pixelMap use source and initialization Options.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testImagePixelMap012() {
        /**
         * @tc.steps: step1. create src pixelMap use file data.
         * @tc.expected: step1. get the valid pixelMap.
         */
        initializationOptions.editable = false;
        PixelMap source = createSrcPixelMap();
        assertNotNull(source);

        /**
         * @tc.steps: step2. response source pixelMap.
         * @tc.expected: step2. check the target pixelMap is valid.
         */
        initializationOptions.size.height = 300;
        initializationOptions.size.width = 300;
        initializationOptions.useSourceIfMatch = true;
        initializationOptions.releaseSource = true;
        PixelMap selfpixelMap = PixelMap.create(source, initializationOptions);
        assertNotNull(selfpixelMap);
        ImageInfo srcImageInfo = source.getImageInfo();
        ImageInfo selfImageInfo = selfpixelMap.getImageInfo();
        assertEquals(srcImageInfo.size.width, selfImageInfo.size.width);
        assertEquals(srcImageInfo.size.height, selfImageInfo.size.height);
        assertEquals(srcImageInfo.pixelFormat, selfImageInfo.pixelFormat);
        assertEquals(selfpixelMap, source);
        /**
         * @tc.steps: step3. create new pixelMap from source with rect and crop scale.
         * @tc.expected: step3. check the target pixelMap is valid.
         */
        initializationOptions.size.height = 400;
        initializationOptions.size.width = 400;
        initializationOptions.scaleMode = ScaleMode.CENTER_CROP;
        initializationOptions.pixelFormat = PixelFormat.BGRA_8888;
        initializationOptions.releaseSource = true;
        Rect rect = new Rect(0, 0, 50, 50);
        PixelMap copyedPixelMap = PixelMap.create(source, rect, initializationOptions);
        assertNotNull(copyedPixelMap);
        srcImageInfo = source.getImageInfo();
        ImageInfo copyedImageInfo = copyedPixelMap.getImageInfo();
        assertEquals(srcImageInfo.size.width, 300);
        assertEquals(copyedImageInfo.size.width, 400);
        source.release();
        selfpixelMap.release();
        copyedPixelMap.release();
    }

    /**
     * @tc.name: testImagePixelMap013
     * @tc.desc: create copy pixelMap use source and initialization Options.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testImagePixelMap013() {
        /**
         * @tc.steps: step1. create src pixelMap use file data.
         * @tc.expected: step1. get the valid pixelMap.
         */
        initializationOptions.editable = true;
        initializationOptions.useSourceIfMatch = true;
        initializationOptions.releaseSource = false;
        PixelMap source = createSrcPixelMap();
        assertNotNull(source);
        ImageInfo srcImageInfo = source.getImageInfo();
        /**
         * @tc.steps: step2. copy pixelMap use source.
         * @tc.expected: step2. check the target pixelMap is valid.
         */
        initializationOptions.size.width = 600;
        initializationOptions.size.height = 500;
        PixelMap copyedpixelMap = PixelMap.create(source, initializationOptions);
        assertNotNull(copyedpixelMap);
        ImageInfo copyedImageInfo = copyedpixelMap.getImageInfo();
        assertEquals(srcImageInfo.size.width, 300);
        assertEquals(copyedImageInfo.size.width, 600);
        copyedpixelMap.release();
        /**
         * @tc.steps: step3. copy pixelMap from source with rect and crop scale.
         * @tc.expected: step3. check the target pixelMap is valid.
         */
        initializationOptions.size.height = 400;
        initializationOptions.size.width = 400;
        initializationOptions.scaleMode = ScaleMode.CENTER_CROP;
        initializationOptions.pixelFormat = PixelFormat.BGRA_8888;
        Rect rect = new Rect(0, 0, 50, 100);
        PixelMap copyedPixelMap2 = PixelMap.create(source, rect, initializationOptions);
        assertNotNull(copyedPixelMap2);
        ImageInfo copyedImageInfo2 = copyedPixelMap2.getImageInfo();
        assertEquals(srcImageInfo.size.width, 300);
        assertEquals(copyedImageInfo2.size.width, 400);
        copyedPixelMap2.release();
        /**
         * @tc.steps: step4. copy pixelMap from source with wrong rect and crop scale.
         * @tc.expected: step4. get the pixelMap null.
         */
        rect = new Rect(0, 0, 600, 600);
        PixelMap copyedPixelMap3 = PixelMap.create(source, rect, initializationOptions);
        assertNull(copyedPixelMap3);
        source.release();
    }

    /**
     * @tc.name: testImagePixelMap014
     * @tc.desc: create pixelMap use default initialization Options and response self.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testImagePixelMap014() {
        /**
         * @tc.steps: step1. create src pixelMap use file data.
         * @tc.expected: step1. get the valid pixelMap.
         */
        initializationOptions.editable = false;
        PixelMap source = createSrcPixelMap();
        assertNotNull(source);
        /**
         * @tc.steps: step2. use default initializationOptions generate src pixelmap.
         * @tc.expected: step2. check the target is valid.
         */
        PixelMap.InitializationOptions defaultOpts = new PixelMap.InitializationOptions();
        defaultOpts.useSourceIfMatch = true;
        defaultOpts.editable = false;
        PixelMap selfPixelMap = PixelMap.create(source, defaultOpts);
        assertNotNull(selfPixelMap);
        ImageInfo selfImageInfo = selfPixelMap.getImageInfo();
        ImageInfo sourceImageInfo = source.getImageInfo();
        assertEquals(sourceImageInfo.size.width, selfImageInfo.size.width);
        assertEquals(sourceImageInfo.size.height, selfImageInfo.size.height);
        assertEquals(source, selfPixelMap);
        source.release();
        selfPixelMap.release();
    }

    /**
     * @tc.name: testImagePixelMapExt001
     * @tc.desc: get pxielMap object parameters.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testImagePixelMapExt001() {
        /**
         * @tc.steps: step1. create pixelMap by test.png.
         * @tc.expected: step1. get the valid pixelMap.
         */
        PixelMap testMap = createTestPixelMap(TEST_PNG_IMAGE_DIR);
        assertNotNull(testMap);
        /**
         * @tc.steps: step2. check the params are correct.
         * @tc.expected: step2. the params is valid.
         */
        long capacity = testMap.getPixelBytesCapacity();
        long bytesNumber = testMap.getPixelBytesNumber();
        int rowBytes = testMap.getBytesNumberPerRow();
        AlphaType alpha = testMap.getImageInfo().alphaType;
        assertEquals(141600, capacity);
        assertEquals(141600, bytesNumber);
        assertEquals(1888, rowBytes);
        assertEquals(AlphaType.OPAQUE, alpha);
        boolean isDefaultEditable = testMap.isEditable();
        assertEquals(false, isDefaultEditable);
        /**
         * @tc.steps: step3. set baseDensity and useMipMap params.
         * @tc.expected: step3. the params is valid.
         */
        testMap.setUseMipmap(true);
        testMap.setBaseDensity(720);
        boolean newUseMipMap = testMap.useMipmap();
        int newBaseDensity = testMap.getBaseDensity();
        assertEquals(true, newUseMipMap);
        assertEquals(720, newBaseDensity);
        testMap.release();
    }

    /**
     * @tc.name: testImagePixelMapExt002
     * @tc.desc: get pxielMap object parameters.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testImagePixelMapExt002() {
        isPhoneDevice = PHONE_DEVICE.equals(Parameters.get(DEVICE_TYPE));
        LOGGER.debug("device type is PHONE: %{public}s", isPhoneDevice);
        if (!isPhoneDevice) {
            return;
        }
        /**
         * @tc.steps: step1. create pixelMap by file data.
         * @tc.expected: step1. get the valid pixelMap.
         */
        initializationOptions.editable = true;
        PixelMap map = createSrcPixelMap();
        assertNotNull(map);
        /**
         * @tc.steps: step2. check the params are correct.
         * @tc.expected: step2. the params is valid.
         */
        long capacity = map.getPixelBytesCapacity();
        long bytesNumber = map.getPixelBytesNumber();
        int rowBytes = map.getBytesNumberPerRow();
        AlphaType alpha = map.getImageInfo().alphaType;
        assertEquals(360000, capacity);
        assertEquals(360000, bytesNumber);
        assertEquals(1200, rowBytes);
        assertEquals(AlphaType.PREMUL, alpha);
        int defaultDesity = map.getBaseDensity();
        assertEquals(DEFAULT_DENSITY, defaultDesity);
        boolean defaultMipMap = map.useMipmap();
        assertEquals(false, defaultMipMap);
        boolean editable = map.isEditable();
        assertEquals(true, editable);
        map.release();
    }

    /**
     * @tc.name: testImagePixelMapExt003
     * @tc.desc: is same pxielMap.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testImagePixelMapExt003() {
        /**
         * @tc.steps: step1. create pixelMap by file data.
         * @tc.expected: step1. get the valid pixelMap.
         */
        PixelMap txtmap = createSrcPixelMap();
        PixelMap pngMap = createTestPixelMap(TEST_PNG_IMAGE_DIR);
        assertNotNull(txtmap);
        assertNotNull(pngMap);
        /**
         * @tc.steps: step2. check the two pixelmap is same.
         * @tc.expected: step2. the params is valid.
         */
        boolean isSame = txtmap.isSameImage(pngMap);
        assertEquals(false, isSame);
        txtmap.release();
        pngMap.release();
    }

    /**
     * @tc.name: testImagePixelMapExt004
     * @tc.desc: is same pxielMap.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testImagePixelMapExt004() {
        /**
         * @tc.steps: step1. create pixelMap by file data.
         * @tc.expected: step1. get the valid pixelMap.
         */
        PixelMap txtmap1 = createSrcPixelMap();
        PixelMap txtmap2 = createSrcPixelMap();
        assertNotNull(txtmap1);
        assertNotNull(txtmap1);
        /**
         * @tc.steps: step2. check the two pixelmap is same.
         * @tc.expected: step2. the params is valid.
         */
        boolean isSame = txtmap1.isSameImage(txtmap2);
        assertEquals(true, isSame);
        txtmap1.release();
        txtmap2.release();
    }

    /**
     * @tc.name: testImagePixelMapExt005
     * @tc.desc: set and get AlphaType.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testImagePixelMapExt005() {
        /**
         * @tc.steps: step1. create pixelMap by file data.
         * @tc.expected: step1. get the valid pixelMap.
         */
        PixelMap txtmap = createSrcPixelMap();
        assertNotNull(txtmap);
        /**
         * @tc.steps: step2. set and check the pixelmap AlphaType.
         * @tc.expected: step2. the params is valid.
         */
        txtmap.setAlphaType(AlphaType.PREMUL);
        assertEquals(AlphaType.PREMUL, txtmap.getImageInfo().alphaType);
        txtmap.release();
    }

    /**
     * @tc.name: testImagePixelMapExt006
     * @tc.desc: set and get AlphaType.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testImagePixelMapExt006() {
        /**
         * @tc.steps: step1. create pixelMap by file data.
         * @tc.expected: step1. get the valid pixelMap.
         */
        initializationOptions.pixelFormat = PixelFormat.RGB_565;
        PixelMap txtmap = createSrcPixelMap();
        assertNotNull(txtmap);
        /**
         * @tc.steps: step2. set and check the pixelmap AlphaType.
         * @tc.expected: step2. the params is valid.
         */
        txtmap.setAlphaType(AlphaType.PREMUL);
        assertEquals(AlphaType.OPAQUE, txtmap.getImageInfo().alphaType);
        txtmap.release();
    }

    /**
     * @tc.name: testImagePixelMapExt007
     * @tc.desc: reset config.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testImagePixelMapExt007() {
        /**
         * @tc.steps: step1. create pixelMap by file data.
         * @tc.expected: step1. get the valid pixelMap.
         */
        PixelMap txtmap = createSrcPixelMap();
        assertNotNull(txtmap);
        /**
         * @tc.steps: step2. reset the pixelmap config.
         * @tc.expected: step2. the params is valid.
         */
        ImageInfo srcInfo = txtmap.getImageInfo();
        Size mSize = new Size(txtmap.getImageInfo().size.width - 100, txtmap.getImageInfo().size.width - 100);
        PixelFormat mFormat = PixelFormat.RGB_565;
        txtmap.resetConfig(mSize, mFormat);
        ImageInfo info = txtmap.getImageInfo();
        assertEquals(200, info.size.width);
        assertEquals(200, info.size.height);
        assertEquals(AlphaType.OPAQUE, info.alphaType);
        assertEquals(PixelFormat.RGB_565, info.pixelFormat);
        assertEquals(360000, txtmap.getPixelBytesCapacity());
        assertEquals(400, txtmap.getBytesNumberPerRow());
        txtmap.release();
    }

    /**
     * @tc.name: testImagePixelMapExt008
     * @tc.desc: reset config.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testImagePixelMapExt008() {
        /**
         * @tc.steps: step1. create pixelMap by file data.
         * @tc.expected: step1. get the valid pixelMap.
         */
        PixelMap txtmap = createSrcPixelMap();
        assertNotNull(txtmap);
        /**
         * @tc.steps: step2. reset the pixelmap config.
         * @tc.expected: step2. the params is valid.
         */
        ImageInfo srcInfo = txtmap.getImageInfo();
        Size mSize = new Size(txtmap.getImageInfo().size.width + 100, txtmap.getImageInfo().size.width + 100);
        PixelFormat mFormat = PixelFormat.RGB_565;
        txtmap.resetConfig(mSize, mFormat);
        ImageInfo info = txtmap.getImageInfo();
        assertEquals(400, info.size.width);
        assertEquals(400, info.size.height);
        assertEquals(AlphaType.OPAQUE, info.alphaType);
        assertEquals(PixelFormat.RGB_565, info.pixelFormat);
        assertEquals(360000, txtmap.getPixelBytesCapacity());
        assertEquals(800, txtmap.getBytesNumberPerRow());
        txtmap.release();
    }

    /**
     * @tc.name: testImagePixelMapExt009
     * @tc.desc: read and write per pixel by position.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testImagePixelMapExt009() {
        /**
         * @tc.steps: step1. create pixelMap by file data.
         * @tc.expected: step1. get the valid pixelMap.
         */
        initializationOptions.editable = true;
        PixelMap txtmap = createSrcPixelMap();
        assertNotNull(txtmap);
        /**
         * @tc.steps: step2. write color to pixelmap the position.
         * @tc.expected: step2. the params is valid.
         */
        Position pos = new Position(50, 50);
        int color = 0xFFFF8040;
        txtmap.writePixel(pos, color);
        /**
         * @tc.steps: step2. read pixelmap the position and check the color.
         * @tc.expected: step2. the params is valid.
         */
        int otherColor = txtmap.readPixel(pos);
        LOGGER.debug("color:0x%{public}x, other:0x%{public}x.", color, otherColor);
        assertEquals(true, color == otherColor);
        txtmap.release();
    }

    /**
     * @tc.name: testImagePixelMapExt010
     * @tc.desc: read per pixel by position abormal.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testImagePixelMapExt010() {
        /**
         * @tc.steps: step1. create pixelMap by file data.
         * @tc.expected: step1. get the valid pixelMap.
         */
        PixelMap txtmap = createTestPixelMap(TEST_PNG_IMAGE_DIR);
        assertNotNull(txtmap);
        /**
         * @tc.steps: step2. read the pixelmap position and the position > pixelmap.size.
         * @tc.expected: step2. throw exception.
         */
        ImageInfo info = txtmap.getImageInfo();
        Position pos = new Position(info.size.width + 1, info.size.height + 1);
        assertThrows("should throw IllegalArgumentException", IllegalArgumentException.class, () -> {
            int color = txtmap.readPixel(pos);
        });
        txtmap.release();
    }

    /**
     * @tc.name: testImagePixelMapExt011
     * @tc.desc: Get a new pixelmap data from the original pixelmap by region.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testImagePixelMapExt011() {
        /**
         * @tc.steps: step1. decode png file to get original pixelmap.
         * @tc.expected: step1. get the valid pixelMap from the png image.
         */
        PixelMap txtmap = createTestPixelMap(TEST_PNG_IMAGE_DIR);
        assertNotNull(txtmap);
        /**
         * @tc.steps: step2. read data from the specified region of pxielmap.
         * @tc.expected: step2. get the valid pixelmap data and save to int array.
         */
        int[] pixelArray = new int[50];
        Rect region = new Rect(0, 0, 10, 5);
        txtmap.readPixels(pixelArray, 0, 10, region);
        txtmap.release();
        LOGGER.debug("pixelArray:%{public}s.",
            IntStream.of(pixelArray).mapToObj(item -> Integer.toHexString(item)).collect(Collectors.joining(".")));
        /**
         * @tc.steps: step3. Create another pixelmap and set it to be editable.
         * @tc.expected: step3. get valid a editable pixelmap.
         */
        initializationOptions.editable = true;
        PixelMap otherMap = createSrcPixelMap();
        assertEquals(true, otherMap.isEditable());
        /**
         * @tc.steps: step4. Write the data of the original image into a new pixelmap.
         * @tc.expected: step4. write to new pixelmap successful.
         */
        otherMap.writePixels(pixelArray, 0, 10, region);
        /**
         * @tc.steps: step5. Check if the written data is correct by read interface.
         * @tc.expected: step5. the written data is the same as the original data.
         */
        int[] otherPixelArray = new int[50];
        Rect otherRegion = new Rect(0, 0, 10, 5);
        otherMap.readPixels(otherPixelArray, 0, 10, otherRegion);
        otherMap.release();
        LOGGER.debug("otherPixelArray:%{public}s.",
            IntStream.of(otherPixelArray).mapToObj(item -> Integer.toHexString(item)).collect(Collectors.joining(".")));
        assertEquals(true, Arrays.equals(pixelArray, otherPixelArray));
    }

    /**
     * @tc.name: testImagePixelMapExt012
     * @tc.desc: read and write pixels by Buffer.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testImagePixelMapExt012() {
        /**
         * @tc.steps: step1. create create byte buffer and init.
         * @tc.expected: step1. get invalid int buffer.
         */
        IntBuffer intBuf = IntBuffer.allocate(50);
        intBuf.limit(50);
        int color = 0xFFFF8080;  // ARGB pink color
        int[] array = intBuf.array();
        Arrays.fill(array, color);
        intBuf.put(array);
        intBuf.position(0);
        assertNotNull(intBuf);
        LOGGER.debug("otherPixelArray:%{public}s.",
            IntStream.of(intBuf.array()).mapToObj(item -> Integer.toHexString(item)).collect(Collectors.joining(".")));
        /**
         * @tc.steps: step2. create the empty pixelmap.
         * @tc.expected: step2. get invalid pixelMap.
         */
        initializationOptions.size.height = 5;
        initializationOptions.size.width = 10;
        initializationOptions.editable = true;
        PixelMap txtMap = PixelMap.create(initializationOptions);
        assertNotNull(txtMap);
        /**
         * @tc.steps: step3. int buffer write to pixelmap.
         * @tc.expected: step3. throw exception.
         */
        txtMap.writePixels(intBuf);
        /**
         * @tc.steps: step4. read pixelmap and check.
         * @tc.expected: step4. throw exception.
         */
        IntBuffer otherBuf = IntBuffer.allocate(50);
        assertNotNull(otherBuf);
        txtMap.readPixels(otherBuf);
        assertEquals(true, otherBuf.equals(intBuf));
        LOGGER.debug("otherBufArray:%{public}s.", IntStream.of(otherBuf.array())
            .mapToObj(item -> Integer.toHexString(item))
            .collect(Collectors.joining(".")));
        txtMap.release();
    }

    /**
     * @tc.name: testImagePixelMapExt013
     * @tc.desc: write color to the whole pixelmap.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testImagePixelMapExt013() {
        /**
         * @tc.steps: step2. create the empty pixelmap.
         * @tc.expected: step2. get invalid pixelMap.
         */
        initializationOptions.editable = true;
        PixelMap txtMap = createSrcPixelMap();
        assertNotNull(txtMap);
        /**
         * @tc.steps: step3. write color to pixelmap and check.
         * @tc.expected: step3. throw exception.
         */
        int color = 0xFFFF8080;  // ARGB pink color
        txtMap.writePixels(color);
        Position pos = new Position(299, 299);
        int otherColor = txtMap.readPixel(pos);
        assertEquals(true, color == otherColor);
        LOGGER.debug("color:0x%{public}x, otherColor:0x%{public}x.", color, otherColor);
        txtMap.release();
    }

    /**
     * @tc.name: testImagePixelMapExt014
     * @tc.desc: Get a new pixelmap data from the original pixelmap by region.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testImagePixelMapExt014() {
        /**
         * @tc.steps: step1. decode png file to get original pixelmap.
         * @tc.expected: step1. get the valid pixelMap from the png image.
         */
        PixelMap txtmap = createTestPixelMap(TEST_PNG_IMAGE_DIR);
        assertNotNull(txtmap);
        /**
         * @tc.steps: step2. read data from the specified region of pxielmap,minX>width minY>height.
         * @tc.expected: step2. get the valid pixelmap data and save to int array.
         */
        int[] pixelArray = new int[50];
        Rect region = new Rect(15, 8, 10, 5);
        txtmap.readPixels(pixelArray, 0, 10, region);
        txtmap.release();
        LOGGER.debug("pixelArray:%{public}s.",
            IntStream.of(pixelArray).mapToObj(item -> Integer.toHexString(item)).collect(Collectors.joining(".")));
        /**
         * @tc.steps: step3. Create another pixelmap and set it to be editable.
         * @tc.expected: step3. get valid a editable pixelmap.
         */
        initializationOptions.editable = true;
        PixelMap otherMap = createSrcPixelMap();
        assertEquals(true, otherMap.isEditable());
        /**
         * @tc.steps: step4. Write the data of the original image into a new pixelmap.
         * @tc.expected: step4. write to new pixelmap successful.
         */
        otherMap.writePixels(pixelArray, 0, 10, region);
        /**
         * @tc.steps: step5. Check if the written data is correct by read interface.
         * @tc.expected: step5. the written data is the same as the original data.
         */
        int[] otherPixelArray = new int[50];
        Rect otherRegion = new Rect(0, 0, 10, 5);
        otherMap.readPixels(otherPixelArray, 0, 10, otherRegion);
        otherMap.release();
        LOGGER.debug("otherPixelArray:%{public}s.",
            IntStream.of(otherPixelArray).mapToObj(item -> Integer.toHexString(item)).collect(Collectors.joining(".")));
        assertEquals(true, Arrays.equals(pixelArray, otherPixelArray));
    }

    /**
     * @tc.name: testImagePixelMapExt015
     * @tc.desc: test write pixel throw exception.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testImagePixelMapExt015() {
        /**
         * @tc.steps: step1. decode png file to get original pixelmap.
         * @tc.expected: step1. get the valid pixelMap from the png image.
         */
        PixelMap txtmap = createTestPixelMap(TEST_PNG_IMAGE_DIR);
        assertNotNull(txtmap);

        /**
         * @tc.steps: step2. write pixel with pos Out of range.
         * @tc.expected: step2. expect IllegalArgumentException.
         */
        Position pos = new Position(1000, 1000);
        assertThrows("should throw IllegalArgumentException", IllegalArgumentException.class, () -> {
            txtmap.writePixel(pos, 0X12345678);
        });
        /**
         * @tc.steps: step3. write pixel with rect Out of range.
         * @tc.expected: step3. expect IllegalArgumentException.
         */
        int[] pixelArray = new int[50];
        Rect region = new Rect(500, 400, 500, 400);
        PixelMap otherMap = createSrcPixelMap();
        assertThrows("should throw IllegalArgumentException", IllegalArgumentException.class, () -> {
            otherMap.writePixels(pixelArray, 0, 10, region);
        });
    }

    /**
     * @tc.name: testImagePixelMapExt016
     * @tc.desc: check pixel map read and write
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testImagePixelMapExt016() {
        PixelMap pixelMap = createTestPixelMap("/sdcard/multimedia/image/test_large.webp");
        PixelMap.InitializationOptions opts = new PixelMap.InitializationOptions();
        opts.editable = true;
        PixelMap newPixelMap = PixelMap.create(pixelMap, opts);

        /**
         * @tc.steps: step1.read pixel map by pos.
         * @tc.expected: step1. expect pixel value ok.
         */
        int color = newPixelMap.readPixel(new Position(360, 342));
        byte[] readByPos = new byte[4];
        readByPos[3] = (byte) (color & 0xff); // B 87
        readByPos[2] = (byte) (color >> 8 & 0xff); // G 248
        readByPos[1] = (byte) (color >> 16 & 0xff); // R 243
        readByPos[0] = (byte) (color >> 24 & 0xff); // A 255

        assertEquals(readByPos[0], (byte) -1);
        assertEquals(readByPos[1], (byte) 63);
        assertEquals(readByPos[2], (byte) 71);
        assertEquals(readByPos[3], (byte) -51);

        /**
         * @tc.steps: step2.write pixel map by pos then read pixel map by rect.
         * @tc.expected: step2. expect pixel value ok.
         */
        newPixelMap.writePixel(new Position(360, 342), color);

        int[] pixels = new int[1];
        Rect rect = new Rect(360, 342, 1, 1);
        newPixelMap.readPixels(pixels, 0, 1, rect);
        byte[] readByRect = new byte[4];
        readByRect[3] = (byte) (pixels[0] & 0xff); // B 87
        readByRect[2] = (byte) (pixels[0] >> 8 & 0xff); // G 248
        readByRect[1] = (byte) (pixels[0] >> 16 & 0xff); // R 243
        readByRect[0] = (byte) (pixels[0] >> 24 & 0xff); // A 255

        assertEquals(readByRect[0], (byte) -1);
        assertEquals(readByRect[1], (byte) 63);
        assertEquals(readByRect[2], (byte) 71);
        assertEquals(readByRect[3], (byte) -51);

        /**
         * @tc.steps: step3.write pixel map by rect then read pixel map by rect again.
         * @tc.expected: step3. expect pixel value ok.
         */
        newPixelMap.writePixels(pixels, 0, 1, rect);
        int[] pixelsBack = new int[1];

        newPixelMap.readPixels(pixelsBack, 0, 1, rect);
        byte[] readByRectBack = new byte[4];
        readByRectBack[3] = (byte) (pixelsBack[0] & 0xff); // B 87
        readByRectBack[2] = (byte) (pixelsBack[0] >> 8 & 0xff); // G 248
        readByRectBack[1] = (byte) (pixelsBack[0] >> 16 & 0xff); // R 243
        readByRectBack[0] = (byte) (pixelsBack[0] >> 24 & 0xff); // A 255

        assertEquals(readByRectBack[0], (byte) -1);
        assertEquals(readByRectBack[1], (byte) 63);
        assertEquals(readByRectBack[2], (byte) 71);
        assertEquals(readByRectBack[3], (byte) -51);
    }

    /**
     * @tc.name: testImagePixelMapExt017
     * @tc.desc: check write editable pixel map
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testImagePixelMapExt017() {
        /**
         * @tc.steps: step1. decode webp file to get editable pixelmap.
         * @tc.expected: step1. get the valid pixelMap from the image.
         */
        ImageSource imageSource = ImageSource.create("/sdcard/multimedia/image/test_large.webp", null);
        assertNotNull(imageSource);
        ImageSource.DecodingOptions decodingOpts = new ImageSource.DecodingOptions();
        decodingOpts.editable = true;
        PixelMap pixelMap = imageSource.createPixelmap(decodingOpts);
        assertNotNull(pixelMap);

        /**
         * @tc.steps: step1. write pixel map pixels.
         * @tc.expected: step1. write ok.
         */
        int color = 0xff123456;
        pixelMap.writePixel(new Position(360, 342), color);
    }

    /**
     * @tc.name: testImagePixelMapExt018
     * @tc.desc: write uneditable pixel map
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testImagePixelMapExt018() {
        /**
         * @tc.steps: step1. decode webp file to get uneditable pixelmap.
         * @tc.expected: step1. get the valid pixelMap from the image.
         */
        ImageSource imageSource = ImageSource.create("/sdcard/multimedia/image/test_large.webp", null);
        assertNotNull(imageSource);
        ImageSource.DecodingOptions decodingOpts = new ImageSource.DecodingOptions();
        decodingOpts.editable = false;
        PixelMap pixelMap = imageSource.createPixelmap(decodingOpts);
        assertNotNull(pixelMap);

        /**
         * @tc.steps: step1. write pixel map pixels.
         * @tc.expected: step1. write exception.
         */
        int color = 0xff123456;
        assertThrows("should throw IllegalStateException", IllegalStateException.class, () -> {
            pixelMap.writePixel(new Position(360, 342), color);
        });
    }

    /**
     * @tc.name: testPixelMapParcelable001
     * @tc.desc: create pixel map and test the parcelable interface
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testPixelMapParcelable001() {
        /**
         * @tc.steps: step1.get image source form path.
         * @tc.expected: step1.get image source form path not null
         */
        ImageSource.SourceOptions srcOpts = new ImageSource.SourceOptions();
        String pathName = "/sdcard/multimedia/image/test.bmp";
        ImageSource imageSource = ImageSource.create(pathName, srcOpts);
        assertNotNull(imageSource);
        /**
         * @tc.steps: step2.create pixel map.
         * @tc.expected: step2.create pixel map not null and check information ok
         */
        ImageSource.DecodingOptions decodingOpts = new ImageSource.DecodingOptions();
        decodingOpts.desiredSize = new Size(0, 0);
        decodingOpts.desiredRegion = new Rect(0, 0, 0, 0);
        decodingOpts.allocator = AllocatorType.SHARED_MEMORY;
        PixelMap pixelMap = imageSource.createPixelmap(decodingOpts);
        assertNotNull(pixelMap);
        assertEquals(472, pixelMap.getImageInfo().size.width);
        assertEquals(75, pixelMap.getImageInfo().size.height);

        /**
         * @tc.steps: step3.marshall pixel map to parcel and create pixel map from parcel.
         * @tc.expected: step3.marshall pixel map success and check information of the pixel map created from parcel ok
         */
        MessageParcel parcel = MessageParcel.create();
        assertNotNull(parcel);
        assertTrue(parcel.writeInt(1));
        boolean ret = pixelMap.marshalling(parcel);
        assertEquals(ret, true);
        PixelMap pixelMapFromParcel = PixelMap.PRODUCER.createFromParcel(parcel);
        assertNotNull(pixelMapFromParcel);
        assertEquals(472, pixelMapFromParcel.getImageInfo().size.width);
        assertEquals(75, pixelMapFromParcel.getImageInfo().size.height);
        assertFalse(pixelMap.unmarshalling(parcel));
    }

    /**
     * @tc.name: testPixelMapDensitySize001
     * @tc.desc: application decode image file and display.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testPixelMapDensitySize001() {
        isPhoneDevice = PHONE_DEVICE.equals(Parameters.get(DEVICE_TYPE));
        LOGGER.debug("device type is PHONE: %{public}s", isPhoneDevice);
        if (!isPhoneDevice) {
            return;
        }
        /**
         * @tc.steps: step1.decode a image file to pixel map.
         * @tc.expected: step1.created pixel map not null.
         */
        ImageSource.SourceOptions srcOpts = new ImageSource.SourceOptions();
        String pathName = "/sdcard/multimedia/image/test.png";
        ImageSource imageSource = ImageSource.create(pathName, srcOpts);
        assertNotNull(imageSource);
        ImageSource.DecodingOptions decodingOpts = new ImageSource.DecodingOptions();
        PixelMap pixelMap = imageSource.createPixelmap(decodingOpts);
        assertNotNull(pixelMap);

        /**
         * @tc.steps: step2.get proper pixel map size on a specified density.
         * @tc.expected: step2.check pixel map size ok.
         */
        Size targetSize = new Size(pixelMap.getImageInfo().size.width * 2, pixelMap.getImageInfo().size.height * 2);
        assertEquals(targetSize, pixelMap.getFitDensitySize(DEFAULT_DENSITY * 2));

        /**
         * @tc.steps: step3.if pixel map is no longer needed, release native resource.
         * @tc.expected: step3.check if native resource released.
         */
        pixelMap.release();
        assertTrue(pixelMap.isReleased());
    }

    /**
     * @tc.name: testPixelMapDensitySize002
     * @tc.desc: get fit size by density.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testPixelMapDensitySize002() {
        isPhoneDevice = PHONE_DEVICE.equals(Parameters.get(DEVICE_TYPE));
        LOGGER.debug("device type is PHONE: %{public}s", isPhoneDevice);
        if (!isPhoneDevice) {
            return;
        }
        /**
         * @tc.steps: step1.get image source form path and create pixel map.
         * @tc.expected: step1.image source not null and pixel map not null.
         */
        ImageSource.SourceOptions srcOpts = new ImageSource.SourceOptions();
        String pathName = "/sdcard/multimedia/image/test.png";
        ImageSource imageSource = ImageSource.create(pathName, srcOpts);
        assertNotNull(imageSource);
        ImageSource.DecodingOptions decodingOpts = new ImageSource.DecodingOptions();
        PixelMap pixelMap = imageSource.createPixelmap(decodingOpts);
        assertNotNull(pixelMap);

        /**
         * @tc.steps: step2.get fit size by default density.
         * @tc.expected: step2.size should be equal to original size.
         */
        Size size = new Size(472, 75);
        assertEquals(size, pixelMap.getFitDensitySize(DEFAULT_DENSITY));

        /**
         * @tc.steps: step3.get fit size by density 0.
         * @tc.expected: step3.size should be equal to original size.
         */
        Size emptySize = new Size(472, 75);
        assertEquals(emptySize, pixelMap.getFitDensitySize(0));

        /**
         * @tc.steps: step4.get fit size by larger density or smaller density.
         * @tc.expected: step4.size should be larger or smaller than original size.
         */
        Size doubleSize = new Size(size.width * 3, size.height * 3);
        assertEquals(doubleSize, pixelMap.getFitDensitySize(DEFAULT_DENSITY * 3));

        Size halfSize = new Size(size.width / 2, size.height / 2 + 1);
        assertEquals(halfSize, pixelMap.getFitDensitySize(DEFAULT_DENSITY / 2));

        Size oneThirdSize = new Size(size.width / 3, size.height / 3);
        assertEquals(oneThirdSize, pixelMap.getFitDensitySize(DEFAULT_DENSITY / 3));

        Size oneFourthSize = new Size(size.width / 4, size.height / 4 + 1);
        assertEquals(oneFourthSize, pixelMap.getFitDensitySize(DEFAULT_DENSITY / 4));

        Size oneFifthSize = new Size(size.width / 5, size.height / 5);
        assertEquals(oneFifthSize, pixelMap.getFitDensitySize(DEFAULT_DENSITY / 5));
    }

    /**
     * @tc.name: testPixelMapDensitySize003
     * @tc.desc: check native resource released
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testPixelMapDensitySize003() {
        isPhoneDevice = PHONE_DEVICE.equals(Parameters.get(DEVICE_TYPE));
        LOGGER.debug("device type is PHONE: %{public}s", isPhoneDevice);
        if (!isPhoneDevice) {
            return;
        }
        /**
         * @tc.steps: step1.get image source form path and create pixel map.
         * @tc.expected: step1.image source not null and pixel map not null.
         */
        ImageSource.SourceOptions srcOpts = new ImageSource.SourceOptions();
        String pathName = "/sdcard/multimedia/image/test.png";
        ImageSource imageSource = ImageSource.create(pathName, srcOpts);
        assertNotNull(imageSource);
        ImageSource.DecodingOptions decodingOpts = new ImageSource.DecodingOptions();
        PixelMap pixelMap = imageSource.createPixelmap(decodingOpts);
        assertNotNull(pixelMap);

        /**
         * @tc.steps: step2.release pixel map native resource, then check if native resources released.
         * @tc.expected: step2.check ok.
         */
        assertFalse(pixelMap.isReleased());
        pixelMap.release();
        assertTrue(pixelMap.isReleased());
    }

    /**
     * @tc.name: testPixelMapCreateFromAlpha001
     * @tc.desc: check createFromAlpha of Src PixelMap type of ARGB888
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testPixelMapCreateFromAlpha001() {
        /**
         * @tc.steps: step1. get image source form path and create pixel map.
         * @tc.expected: step1. image source not null and pixel map not null.
         */
        ImageSource.SourceOptions srcOpts = new ImageSource.SourceOptions();
        String pathName = "/sdcard/multimedia/image/test.9.png";
        ImageSource imageSource = ImageSource.create(pathName, srcOpts);
        assertNotNull(imageSource);
        ImageSource.DecodingOptions decodingOpts = new ImageSource.DecodingOptions();
        decodingOpts.desiredPixelFormat = PixelFormat.ARGB_8888;
        PixelMap pixelMapSrc = imageSource.createPixelmap(decodingOpts);
        assertNotNull(pixelMapSrc);

        /**
         * @tc.steps: step2. get alpha pixelmap from source.
         * @tc.expected: step2. check ok.
         */
        PixelMap alphaPixelMap = pixelMapSrc.createFromAlpha();
        assertNotNull(alphaPixelMap);

        /**
         * @tc.steps: step3. check alpha pixelmap format.
         * @tc.expected: step3.check ok.
         */
        ImageInfo imageInfoAlpha = alphaPixelMap.getImageInfo();
        ImageInfo imageInfoSrc = pixelMapSrc.getImageInfo();
        assertEquals(imageInfoAlpha.size.height, imageInfoSrc.size.height);
        assertEquals(imageInfoAlpha.size.width, imageInfoSrc.size.width);
        assertEquals(imageInfoAlpha.alphaType, imageInfoSrc.alphaType);
        assertTrue(Math.abs(pixelMapSrc.getBytesNumberPerRow()/4 - alphaPixelMap.getBytesNumberPerRow()) < 4);

        /**
         * @tc.steps: step4. check alpha pixelmap format.
         * @tc.expected: step4.check ok.
         */
        alphaPixelMap.release();
        pixelMapSrc.release();
        assertTrue(alphaPixelMap.isReleased());
        assertTrue(pixelMapSrc.isReleased());
    }

    /**
     * @tc.name: testPixelMapCreateFromAlpha002
     * @tc.desc: check createFromAlpha of Src PixelMap type of RGB565
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testPixelMapCreateFromAlpha002() {
        /**
         * @tc.steps: step1. get image source form path and create pixel map.
         * @tc.expected: step1. image source not null and pixel map not null.
         */
        ImageSource.SourceOptions srcOpts = new ImageSource.SourceOptions();
        String pathName = "/sdcard/multimedia/image/test.jpg";
        ImageSource imageSource = ImageSource.create(pathName, srcOpts);
        assertNotNull(imageSource);
        ImageSource.DecodingOptions decodingOpts = new ImageSource.DecodingOptions();
        decodingOpts.desiredPixelFormat = PixelFormat.RGB_565;
        PixelMap pixelMapSrc = imageSource.createPixelmap(decodingOpts);
        assertNotNull(pixelMapSrc);

        /**
         * @tc.steps: step2. get alpha pixelmap from source.
         * @tc.expected: step2. check ok.
         */
        PixelMap alphaPixelMap = pixelMapSrc.createFromAlpha();
        assertNotNull(alphaPixelMap);

        /**
         * @tc.steps: step3. check alpha pixelmap format.
         * @tc.expected: step3.check ok.
         */
        ImageInfo imageInfoAlpha = alphaPixelMap.getImageInfo();
        ImageInfo imageInfoSrc = pixelMapSrc.getImageInfo();
        assertEquals(imageInfoAlpha.size.height, imageInfoSrc.size.height);
        assertEquals(imageInfoAlpha.size.width, imageInfoSrc.size.width);
        assertNotEquals(imageInfoAlpha.alphaType, imageInfoSrc.alphaType);
        // RGB565 total 16 bytes, alpha has 8 bytes per pixel
        assertTrue(Math.abs(pixelMapSrc.getBytesNumberPerRow() / 2 - alphaPixelMap.getBytesNumberPerRow()) < 4);

        /**
         * @tc.steps: step4. check alpha pixelmap format.
         * @tc.expected: step4.check ok.
         */
        alphaPixelMap.release();
        pixelMapSrc.release();
        assertTrue(alphaPixelMap.isReleased());
        assertTrue(pixelMapSrc.isReleased());
    }

    /**
     * @tc.name: testPixelMapCreateFromAlpha003
     * @tc.desc: check createFromAlpha of Src PixelMap type of gif
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testPixelMapCreateFromAlpha003() {
        /**
         * @tc.steps: step1. get image source form path and create pixel map.
         * @tc.expected: step1. image source not null and pixel map not null.
         */
        ImageSource.SourceOptions srcOpts = new ImageSource.SourceOptions();
        String pathName = "/sdcard/multimedia/image/test.gif";
        ImageSource imageSource = ImageSource.create(pathName, srcOpts);
        assertNotNull(imageSource);
        ImageSource.DecodingOptions decodingOpts = new ImageSource.DecodingOptions();
        decodingOpts.desiredPixelFormat = PixelFormat.ARGB_8888;
        PixelMap pixelMapSrc = imageSource.createPixelmap(decodingOpts);
        assertNotNull(pixelMapSrc);

        /**
         * @tc.steps: step2. get alpha pixelmap from source.
         * @tc.expected: step2. check ok.
         */
        PixelMap alphaPixelMap = pixelMapSrc.createFromAlpha();
        assertNotNull(alphaPixelMap);

        /**
         * @tc.steps: step3. check alpha pixelmap format.
         * @tc.expected: step3.check ok.
         */
        ImageInfo imageInfoAlpha = alphaPixelMap.getImageInfo();
        ImageInfo imageInfoSrc = pixelMapSrc.getImageInfo();
        assertEquals(imageInfoAlpha.size.height, imageInfoSrc.size.height);
        assertEquals(imageInfoAlpha.size.width, imageInfoSrc.size.width);
        assertTrue(Math.abs(pixelMapSrc.getBytesNumberPerRow()/4 - alphaPixelMap.getBytesNumberPerRow()) < 4);

        /**
         * @tc.steps: step4. check alpha pixelmap format.
         * @tc.expected: step4.check ok.
         */
        alphaPixelMap.release();
        pixelMapSrc.release();
        assertTrue(alphaPixelMap.isReleased());
        assertTrue(pixelMapSrc.isReleased());
    }

    /**
     * @tc.name: testPixelMapCreateFromAlpha004
     * @tc.desc: check createFromAlpha of Src PixelMap type of webp
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testPixelMapCreateFromAlpha004() {
        /**
         * @tc.steps: step1. get image source form path and create pixel map.
         * @tc.expected: step1. image source not null and pixel map not null.
         */
        ImageSource.SourceOptions srcOpts = new ImageSource.SourceOptions();
        String pathName = "/sdcard/multimedia/image/test.webp";
        ImageSource imageSource = ImageSource.create(pathName, srcOpts);
        assertNotNull(imageSource);
        ImageSource.DecodingOptions decodingOpts = new ImageSource.DecodingOptions();
        decodingOpts.desiredPixelFormat = PixelFormat.ARGB_8888;
        PixelMap pixelMapSrc = imageSource.createPixelmap(decodingOpts);
        assertNotNull(pixelMapSrc);

        /**
         * @tc.steps: step2. get alpha pixelmap from source.
         * @tc.expected: step2. check ok.
         */
        PixelMap alphaPixelMap = pixelMapSrc.createFromAlpha();
        assertNotNull(alphaPixelMap);

        /**
         * @tc.steps: step3. check alpha pixelmap format.
         * @tc.expected: step3.check ok.
         */
        ImageInfo imageInfoAlpha = alphaPixelMap.getImageInfo();
        ImageInfo imageInfoSrc = pixelMapSrc.getImageInfo();
        assertEquals(imageInfoAlpha.size.height, imageInfoSrc.size.height);
        assertEquals(imageInfoAlpha.size.width, imageInfoSrc.size.width);
        assertTrue(Math.abs(pixelMapSrc.getBytesNumberPerRow()/4 - alphaPixelMap.getBytesNumberPerRow()) < 4);

        /**
         * @tc.steps: step4. check alpha pixelmap format.
         * @tc.expected: step4.check ok.
         */
        alphaPixelMap.release();
        pixelMapSrc.release();
        assertTrue(alphaPixelMap.isReleased());
        assertTrue(pixelMapSrc.isReleased());
    }

    /**
     * @tc.name: testPixelMapCreateFromAlpha005
     * @tc.desc: check readPixel from Alpha
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testPixelMapCreateFromAlpha005() {
        LOGGER.info("testPixelMapCreateFromAlpha005 begin");

        ImageSource.SourceOptions srcOpts = new ImageSource.SourceOptions();
        String pathName = "/sdcard/multimedia/image/test.png";
        ImageSource imageSource = ImageSource.create(pathName, srcOpts);
        assertNotNull(imageSource);

        ImageSource.DecodingOptions decodingOpts = new ImageSource.DecodingOptions();
        decodingOpts.desiredPixelFormat = PixelFormat.RGB_565;
        PixelMap pixelMapSrc = imageSource.createPixelmap(decodingOpts);

        PixelMap alphaPixelMap = pixelMapSrc.createFromAlpha();
        assertNotNull(alphaPixelMap);

        ImageInfo imageInfoAlpha = alphaPixelMap.getImageInfo();
        ImageInfo imageInfoSrc = pixelMapSrc.getImageInfo();
        assertEquals(imageInfoAlpha.size.height, imageInfoSrc.size.height);
        assertEquals(imageInfoAlpha.size.width, imageInfoSrc.size.width);

        int xPos = alphaPixelMap.getImageInfo().size.width / 2;
        int yPos = alphaPixelMap.getImageInfo().size.height / 2;

        Position pos = new Position(xPos, yPos);
        int color = pixelMapSrc.readPixel(pos);
        color = color & 0xFF000000;
        assertEquals(0xFF000000, color);
        assertEquals(0xFF000000, alphaPixelMap.readPixel(pos));
        assertNotEquals(imageInfoAlpha.alphaType, imageInfoSrc.alphaType);

        assertTrue(Math.abs(pixelMapSrc.getBytesNumberPerRow()/2 - alphaPixelMap.getBytesNumberPerRow()) < 4);
        alphaPixelMap.release();
        pixelMapSrc.release();

        assertTrue(alphaPixelMap.isReleased());
        assertTrue(pixelMapSrc.isReleased());

        LOGGER.info("testPixelMapCreateFromAlpha005 end");
    }

    /**
     * @tc.name: testPixelMapCreateFromAlpha005
     * @tc.desc: check readPixel from Alpha
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testPixelMapCreateFromAlpha006() {
        PixelMap.InitializationOptions options = new PixelMap.InitializationOptions();
        Size size = new Size(40, 40);
        options.size = size;
        options.pixelFormat = PixelFormat.RGB_565;
        options.editable = true;

        PixelMap srcMap = PixelMap.create(options);
        PixelMap alphaPixelMap = srcMap.createFromAlpha();

        assertNotNull(alphaPixelMap);

        Position pos = new Position(20, 20);
        int color1 = srcMap.readPixel(pos);
        int color2 = alphaPixelMap.readPixel(pos);

        assertEquals(color1, color2);
    }

    private PixelMap createSrcPixelMap() {
        LOGGER.debug("myColors length: %{public}d", myColors.length);
        initializationOptions.size.height = 300;
        initializationOptions.size.width = 300;
        PixelMap pixelMap = PixelMap.create(myColors, initializationOptions);
        assertNotNull(pixelMap);
        return pixelMap;
    }

    private static PixelMap createTestPixelMap(String filePath) {
        if (filePath == null) {
            LOGGER.error("test pixelmap to check filePath fail.");
            return null;
        }
        ImageSource.SourceOptions srcOpts = new ImageSource.SourceOptions();
        srcOpts.formatHint = "";
        ImageSource imageSource = ImageSource.create(filePath, srcOpts);
        if (imageSource == null) {
            LOGGER.error("test pixelmap to create the imageSource fail.");
            return null;
        }
        ImageSource.DecodingOptions decodingOpts = new ImageSource.DecodingOptions();
        decodingOpts.desiredSize = new Size(0, 0);
        decodingOpts.desiredRegion = new Rect(0, 0, 0, 0);
        return imageSource.createPixelmap(decodingOpts);
    }

    /**
     * @tc.name: testPixelMapCreateFromAlpha007
     * @tc.desc: check readPixel from Alpha
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testPixelMapCreateFromAlpha007() {
        LOGGER.info("testPixelMapCreateFromAlpha007 begin");

        ImageSource.SourceOptions srcOpts = new ImageSource.SourceOptions();
        String pathName = "/sdcard/multimedia/image/test.jpg";
        ImageSource imageSource = ImageSource.create(pathName, srcOpts);
        assertNotNull(imageSource);

        ImageSource.DecodingOptions decodingOpts = new ImageSource.DecodingOptions();
        decodingOpts.desiredPixelFormat = PixelFormat.RGB_565;
        PixelMap pixelMapSrc = imageSource.createPixelmap(decodingOpts);

        PixelMap alphaPixelMap = pixelMapSrc.createFromAlpha();
        assertNotNull(alphaPixelMap);

        ImageInfo imageInfoAlpha = alphaPixelMap.getImageInfo();
        ImageInfo imageInfoSrc = pixelMapSrc.getImageInfo();

        assertEquals(imageInfoAlpha.size.height, imageInfoSrc.size.height);
        assertEquals(imageInfoAlpha.size.width, imageInfoSrc.size.width);

        int xPos = alphaPixelMap.getImageInfo().size.width / 2;
        int yPos = alphaPixelMap.getImageInfo().size.height / 2;
        Position pos = new Position(xPos, yPos);
        int color = pixelMapSrc.readPixel(pos);
        color = color & 0xFF000000;
        assertEquals(0xFF000000, color);
        assertEquals(0xFF000000, alphaPixelMap.readPixel(pos));
        assertNotEquals(imageInfoAlpha.alphaType, imageInfoSrc.alphaType);

        assertTrue(Math.abs(pixelMapSrc.getBytesNumberPerRow()/2 - alphaPixelMap.getBytesNumberPerRow()) < 4);
        alphaPixelMap.release();
        pixelMapSrc.release();

        assertTrue(alphaPixelMap.isReleased());
        assertTrue(pixelMapSrc.isReleased());

        LOGGER.info("testPixelMapCreateFromAlpha007 end");
    }

    private static int[] constructColors() {
        File file = new File(COLORS_DATA);
        assertNotNull(file);
        try (BufferedReader br = new BufferedReader(new FileReader(COLORS_DATA))) {
            List<Integer> lists = new ArrayList<>();
            String line;
            while ((line = br.readLine()) != null) {
                String[] strArray = line.trim().split(",");
                for (String item : strArray) {
                    lists.add(Integer.parseInt(item.trim()));
                }
            }
            return lists.stream().mapToInt(Integer::valueOf).toArray();
        } catch (IOException e) {
            LOGGER.error("construct data error. " + e.getMessage());
        }
        return new int[0];
    }

    private void imageEncode(PixelMap pixelMap, String targetPath) {
        ImagePacker imagePacker = ImagePacker.create();
        assertNotNull(imagePacker);
        ImagePacker.PackingOptions packingOptions = new ImagePacker.PackingOptions();
        packingOptions.format = "image/jpeg";
        packingOptions.quality = 100;
        packingOptions.numberHint = 1;
        try (OutputStream outputStream = new FileOutputStream(targetPath)) {
            boolean result = imagePacker.initializePacking(outputStream, packingOptions);
            assertTrue(result);
            imagePacker.addImage(pixelMap);
            imagePacker.finalizePacking();
        } catch (FileNotFoundException e) {
            LOGGER.error("ImagePacker, file not found");
        } catch (IOException fio) {
            LOGGER.error("ImagePacker, IO Exception");
        }
        imagePacker.release();
    }
}
