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
import static org.junit.Assert.assertNotEquals;
import static org.junit.Assert.assertNotNull;
import static org.junit.Assert.assertNull;
import static org.junit.Assert.assertTrue;
import static org.junit.Assert.assertThrows;
import static org.junit.Assert.fail;

import ohos.media.image.common.AllocatorType;
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
import ohos.system.Parameters;
import ohos.unittest.CaseLevel;
import ohos.unittest.CaseType;
import ohos.unittest.Level;
import ohos.unittest.Type;
import ohos.utils.Pair;

import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Rule;
import org.junit.Test;
import org.junit.rules.ExpectedException;

import java.util.ArrayList;
import java.io.BufferedReader;
import java.io.File;
import java.io.FileDescriptor;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.io.InputStream;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.nio.ByteBuffer;
import java.nio.channels.FileChannel;
import java.util.HashSet;
import java.util.List;

/**
 * ImageSourceKitTest, test cases for ImageSource class, mainly including test cases for image decoding functions
 *
 * @since 1
 */
public class ImageSourceKitTest {
    private static final Logger LOGGER = LoggerFactory.getImageLogger(ImageSourceKitTest.class);

    private static File pngFile;

    private static final String PHONE_DEVICE = "default";

    private static final String SHARED_MEMORY_STRING = "Ashmem";

    private static final String HEAP_STRING = "Native";

    private static final long CONST_NUMBER = 1000000000;

    private static final String DEVICE_TYPE = "ro.build.characteristics";

    private boolean isPhoneDevice = false;

    private ImageSource imageSource;

    private PixelMap pixelMap;

    private ExifUtils exifUtils = new ExifUtils();

    /**
     * Action before all test case.
     */
    @BeforeClass
    public static void setUpBeforeClass() {
        pngFile = new File("/sdcard/multimedia/image/test.png");
        if (!pngFile.exists()) {
            fail("files not exist");
        }
    }

    /**
     * Action after all test case.
     */
    @AfterClass
    public static void tearDownAfterClass() {
    }

    /**
     * Action before test case.
     */
    @Before
    public void setUp() {
    }

    /**
     * Action after test case.
     */
    @After
    public void tearDown() {
        try {
            if (imageSource != null) {
                imageSource.finalize();
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
     * @tc.name: testImageSource001
     * @tc.desc: get supported image formats.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testImageSource001() {
        /**
         * @tc.steps: step1.get supported image formats.
         * @tc.expected: step1.supported image formats not null
         */
        HashSet<String> formats = ImageSource.getSupportedFormats();
        assertNotNull(formats);

        /**
         * @tc.steps: step2.check supported image formats.
         * @tc.expected: step2.supported image formats checked ok
         */
        assertTrue(formats.contains("image/jpeg"));
        assertTrue(formats.contains("image/png"));
        assertTrue(formats.contains("image/gif"));
        assertTrue(formats.contains("image/webp"));
    }

    /**
     * @tc.name: testImageSource002
     * @tc.desc: create image source from null stream.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testImageSource002() {
        /**
         * @tc.steps: step1.get image source from null input stream.
         * @tc.expected: step1.throw IllegalArgumentException.
         */
        FileInputStream inputStream = null;
        ImageSource.SourceOptions srcOpts = new ImageSource.SourceOptions();

        assertThrows("should throw IllegalArgumentException", IllegalArgumentException.class, () -> {
            imageSource = ImageSource.create(inputStream, srcOpts);
        });
    }

    /**
     * @tc.name: testImageSource003
     * @tc.desc: create image source from null byte array.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testImageSource003() {
        /**
         * @tc.steps: step1.get image source from null byte array.
         * @tc.expected: step1.throw IllegalArgumentException.
         */
        byte[] soureData = null;
        ImageSource.SourceOptions srcOpts = new ImageSource.SourceOptions();

        assertThrows("should throw IllegalArgumentException", IllegalArgumentException.class, () -> {
            imageSource = ImageSource.create(soureData, srcOpts);
        });
    }

    /**
     * @tc.name: testImageSource004
     * @tc.desc: create image source from null byte array.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testImageSource004() {
        /**
         * @tc.steps: step1.get image source from null byte array.
         * @tc.expected: step1.throw IllegalArgumentException.
         */
        byte[] soureData = null;
        ImageSource.SourceOptions srcOpts = new ImageSource.SourceOptions();

        assertThrows("should throw IllegalArgumentException", IllegalArgumentException.class, () -> {
            imageSource = ImageSource.create(soureData, 0, 0, srcOpts);
        });
    }

    /**
     * @tc.name: testImageSource005
     * @tc.desc: create image source from byte array with invalid offset.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testImageSource005() {
        /**
         * @tc.steps: step1.get image source from byte array with invalid offset.
         * @tc.expected: step1.throw IndexOutOfBoundsException.
         */
        byte[] soureData = new byte[10];
        ImageSource.SourceOptions srcOpts = new ImageSource.SourceOptions();

        assertThrows("should throw IndexOutOfBoundsException", IndexOutOfBoundsException.class, () -> {
            imageSource = ImageSource.create(soureData, -1, 0, srcOpts);
        });
    }

    /**
     * @tc.name: testImageSource006
     * @tc.desc: create image source from byte array with invalid length.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testImageSource006() {
        /**
         * @tc.steps: step1.get image source from byte array with invalid length.
         * @tc.expected: step1.throw IndexOutOfBoundsException.
         */
        byte[] soureData = new byte[10];
        ImageSource.SourceOptions srcOpts = new ImageSource.SourceOptions();

        assertThrows("should throw IndexOutOfBoundsException", IndexOutOfBoundsException.class, () -> {
            imageSource = ImageSource.create(soureData, 0, -1, srcOpts);
        });
    }

    /**
     * @tc.name: testImageSource007
     * @tc.desc: create image source from byte array with invalid offset.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testImageSource007() {
        /**
         * @tc.steps: step1.get image source from byte array with invalid offset.
         * @tc.expected: step1.throw IndexOutOfBoundsException.
         */
        byte[] soureData = new byte[10];
        ImageSource.SourceOptions srcOpts = new ImageSource.SourceOptions();

        assertThrows("should throw IndexOutOfBoundsException", IndexOutOfBoundsException.class, () -> {
            imageSource = ImageSource.create(soureData, soureData.length, soureData.length, srcOpts);
        });
    }

    /**
     * @tc.name: testImageSource008
     * @tc.desc: create image source from byte array with invalid offset and length.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testImageSource008() {
        /**
         * @tc.steps: step1.get image source from byte array with invalid offset and length.
         * @tc.expected: step1.throw IndexOutOfBoundsException.
         */
        byte[] soureData = new byte[10];
        ImageSource.SourceOptions srcOpts = new ImageSource.SourceOptions();

        assertThrows("should throw IndexOutOfBoundsException", IndexOutOfBoundsException.class, () -> {
            imageSource = ImageSource.create(soureData, 1, soureData.length, srcOpts);
        });
    }

    /**
     * @tc.name: testImageSource009
     * @tc.desc: create image source from null path.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testImageSource009() {
        /**
         * @tc.steps: step1.get image source from null path.
         * @tc.expected: step1.throw IllegalArgumentException.
         */
        String pathName = null;
        ImageSource.SourceOptions srcOpts = new ImageSource.SourceOptions();

        assertThrows("should throw IllegalArgumentException", IllegalArgumentException.class, () -> {
            imageSource = ImageSource.create(pathName, srcOpts);
        });
    }

    /**
     * @tc.name: testImageSource010
     * @tc.desc: create image source from empty image path.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testImageSource010() {
        /**
         * @tc.steps: step1.get image source from emplty image path.
         * @tc.expected: step1.throw IllegalArgumentException.
         */
        String pathName = "";
        ImageSource.SourceOptions srcOpts = new ImageSource.SourceOptions();

        assertThrows("should throw IllegalArgumentException", IllegalArgumentException.class, () -> {
            imageSource = ImageSource.create(pathName, srcOpts);
        });
    }

    /**
     * @tc.name: testImageSource011
     * @tc.desc: create image source from unavailable image path.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testImageSource011() {
        /**
         * @tc.steps: step1.get image source from unavailable image path.
         * @tc.expected: step1.throw DataSourceUnavailableException.
         */
        String pathName = "/random/to/image.jpg";
        ImageSource.SourceOptions srcOpts = new ImageSource.SourceOptions();

        assertThrows("should throw DataSourceUnavailableException", DataSourceUnavailableException.class, () -> {
            imageSource = ImageSource.create(pathName, srcOpts);
        });
    }

    /**
     * @tc.name: testImageSource012
     * @tc.desc: create image source from null File object.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testImageSource012() {
        /**
         * @tc.steps: step1.get image source from null File object.
         * @tc.expected: step1.throw IllegalArgumentException.
         */
        File imageFile = null;
        ImageSource.SourceOptions srcOpts = new ImageSource.SourceOptions();

        assertThrows("should throw IllegalArgumentException", IllegalArgumentException.class, () -> {
            imageSource = ImageSource.create(imageFile, srcOpts);
        });
    }

    /**
     * @tc.name: testImageSource013
     * @tc.desc: create image source from null FileDescriptor object.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testImageSource013() {
        /**
         * @tc.steps: step1.get image source from null FileDescriptor object.
         * @tc.expected: step1.throw IllegalArgumentException.
         */
        FileDescriptor imageFile = null;
        ImageSource.SourceOptions srcOpts = new ImageSource.SourceOptions();

        assertThrows("should throw IllegalArgumentException", IllegalArgumentException.class, () -> {
            imageSource = ImageSource.create(imageFile, srcOpts);
        });
    }

    /**
     * @tc.name: testImageSource014
     * @tc.desc: create pixel map from invalid index.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testImageSource014() {
        /**
         * @tc.steps: step1.get image source form path.
         * @tc.expected: step1.get image source form path not null
         */
        String pathName = "/sdcard/multimedia/image/test.jpg";
        imageSource = ImageSource.create(pathName, null);

        /**
         * @tc.steps: step2.get pixel map from invalid index.
         * @tc.expected: step2.throw IllegalArgumentException.
         */
        assertThrows("should throw IllegalArgumentException", IllegalArgumentException.class, () -> {
            pixelMap = imageSource.createPixelmap(-1, null);
        });
    }

    /**
     * @tc.name: testImageSource015
     * @tc.desc: create pixel map from released image source.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testImageSource015() {
        /**
         * @tc.steps: step1.get image source form path.
         * @tc.expected: step1.get image source form path not null
         */
        String pathName = "/sdcard/multimedia/image/test.jpg";
        imageSource = ImageSource.create(pathName, null);

        /**
         * @tc.steps: step2.get pixel map from released image source.
         * @tc.expected: step2.throw IllegalStateException.
         */
        imageSource.release();
        assertThrows("should throw IllegalStateException", IllegalStateException.class, () -> {
            pixelMap = imageSource.createPixelmap(0, null);
        });
    }

    /**
     * @tc.name: testImageSource016
     * @tc.desc: create thumbnail pixel map from released image source.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testImageSource016() {
        /**
         * @tc.steps: step1.get image source form path.
         * @tc.expected: step1.get image source form path not null
         */
        String pathName = "/sdcard/multimedia/image/test.jpg";
        imageSource = ImageSource.create(pathName, null);

        /**
         * @tc.steps: step2.get thumbnail pixel map from released image source.
         * @tc.expected: step2.throw IllegalStateException.
         */
        imageSource.release();
        assertThrows("should throw IllegalStateException", IllegalStateException.class, () -> {
            pixelMap = imageSource.createThumbnailPixelmap(null, false);
        });
    }

    /**
     * @tc.name: testImageSource017
     * @tc.desc: create pixel map image info by invalid index.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testImageSource017() {
        /**
         * @tc.steps: step1.get image source form path.
         * @tc.expected: step1.get image source form path not null
         */
        String pathName = "/sdcard/multimedia/image/test.jpg";
        imageSource = ImageSource.create(pathName, null);

        /**
         * @tc.steps: step2.get pixel map image info by invalid index.
         * @tc.expected: step2.throw IllegalArgumentException.
         */
        assertThrows("should throw IllegalArgumentException", IllegalArgumentException.class, () -> {
            imageSource.getImageInfo(-1);
        });
    }

    /**
     * @tc.name: testImageSource018
     * @tc.desc: create pixel map image info from released image source.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testImageSource018() {
        /**
         * @tc.steps: step1.get image source form path.
         * @tc.expected: step1.get image source form path not null
         */
        String pathName = "/sdcard/multimedia/image/test.jpg";
        imageSource = ImageSource.create(pathName, null);

        /**
         * @tc.steps: step2.get pixel map image info from released image source.
         * @tc.expected: step2.throw IllegalStateException.
         */
        imageSource.release();
        assertThrows("should throw IllegalStateException", IllegalStateException.class, () -> {
            imageSource.getImageInfo(0);
        });
    }

    /**
     * @tc.name: testImageSource019
     * @tc.desc: get pixel map image property by null key.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testImageSource019() {
        /**
         * @tc.steps: step1.get image source form path.
         * @tc.expected: step1.get image source form path not null
         */
        String pathName = "/sdcard/multimedia/image/test.jpg";
        imageSource = ImageSource.create(pathName, null);

        /**
         * @tc.steps: step2.get pixel map image property by null key.
         * @tc.expected: step2.throw IllegalArgumentException.
         */
        assertThrows("should throw IllegalArgumentException", IllegalArgumentException.class, () -> {
            imageSource.getImagePropertyString(null);
        });
    }

    /**
     * @tc.name: testImageSource020
     * @tc.desc: get pixel map image property by empty key.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testImageSource020() {
        /**
         * @tc.steps: step1.get image source form path.
         * @tc.expected: step1.get image source form path not null
         */
        String pathName = "/sdcard/multimedia/image/test.jpg";
        imageSource = ImageSource.create(pathName, null);

        /**
         * @tc.steps: step2.get pixel map image property by empty key.
         * @tc.expected: step2.check null.
         */
        assertNull(imageSource.getImagePropertyString(""));
    }

    /**
     * @tc.name: testImageSource021
     * @tc.desc: get pixel map image property from released image source.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testImageSource021() {
        /**
         * @tc.steps: step1.get image source form path.
         * @tc.expected: step1.get image source form path not null
         */
        String pathName = "/sdcard/multimedia/image/test.jpg";
        imageSource = ImageSource.create(pathName, null);

        /**
         * @tc.steps: step2.get pixel map image property from released image source.
         * @tc.expected: step2.throw IllegalStateException.
         */
        imageSource.release();
        assertThrows("should throw IllegalStateException", IllegalStateException.class, () -> {
            imageSource.getImagePropertyString(PropertyKey.Exif.APERTURE_VALUE);
        });
    }

    /**
     * @tc.name: testImageSource022
     * @tc.desc: get pixel map thumbnail from released image source.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testImageSource022() {
        /**
         * @tc.steps: step1.get image source form path.
         * @tc.expected: step1.get image source form path not null
         */
        String pathName = "/sdcard/multimedia/image/test.jpg";
        imageSource = ImageSource.create(pathName, null);

        /**
         * @tc.steps: step2.get pixel map thumbnail from released image source.
         * @tc.expected: step2.throw IllegalStateException.
         */
        imageSource.release();
        assertThrows("should throw IllegalStateException", IllegalStateException.class, () -> {
            imageSource.getThumbnailInfo();
        });
    }

    /**
     * @tc.name: testImageSource023
     * @tc.desc: get source info from released image source.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testImageSource023() {
        /**
         * @tc.steps: step1.get image source form path.
         * @tc.expected: step1.get image source form path not null
         */
        String pathName = "/sdcard/multimedia/image/test.jpg";
        imageSource = ImageSource.create(pathName, null);

        /**
         * @tc.steps: step2.get source info from released image source.
         * @tc.expected: step2.throw IllegalStateException.
         */
        imageSource.release();
        assertThrows("should throw IllegalStateException", IllegalStateException.class, () -> {
            imageSource.getSourceInfo();
        });
    }

    /**
     * @tc.name: testImageSource024
     * @tc.desc: update incremental image source with a null byte array.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testImageSource024() {
        /**
         * @tc.steps: step1.get incremental image source.
         * @tc.expected: step1.get incremental image source not null
         */
        ImageSource.SourceOptions srcOpts = new ImageSource.SourceOptions();
        imageSource = ImageSource.createIncrementalSource(srcOpts);
        assertNotNull(imageSource);

        /**
         * @tc.steps: step2.update data with a null byte array.
         * @tc.expected: step2.throw IllegalArgumentException.
         */
        byte[] soureData = null;
        assertThrows("should throw IllegalArgumentException", IllegalArgumentException.class, () -> {
            imageSource.updateData(soureData, 0, 0, false);
        });
    }

    /**
     * @tc.name: testImageSource025
     * @tc.desc: update incremental image source from byte array with invalid offset.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testImageSource025() {
        /**
         * @tc.steps: step1.get incremental image source.
         * @tc.expected: step1.get incremental image source not null
         */
        ImageSource.SourceOptions srcOpts = new ImageSource.SourceOptions();
        imageSource = ImageSource.createIncrementalSource(srcOpts);
        assertNotNull(imageSource);

        /**
         * @tc.steps: step2.update data from byte array with invalid offset.
         * @tc.expected: step2.throw IndexOutOfBoundsException.
         */
        byte[] soureData = new byte[10];
        assertThrows("should throw IndexOutOfBoundsException", IndexOutOfBoundsException.class, () -> {
            imageSource.updateData(soureData, -1, 0, false);
        });
    }

    /**
     * @tc.name: testImageSource026
     * @tc.desc: update incremental image source from byte array with invalid length.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testImageSource026() {
        /**
         * @tc.steps: step1.get incremental image source.
         * @tc.expected: step1.get incremental image source not null
         */
        ImageSource.SourceOptions srcOpts = new ImageSource.SourceOptions();
        imageSource = ImageSource.createIncrementalSource(srcOpts);
        assertNotNull(imageSource);

        /**
         * @tc.steps: step2.update data from byte array with invalid length.
         * @tc.expected: step2.throw IndexOutOfBoundsException.
         */
        byte[] soureData = new byte[10];
        assertThrows("should throw IndexOutOfBoundsException", IndexOutOfBoundsException.class, () -> {
            imageSource.updateData(soureData, 0, -1, false);
        });
    }

    /**
     * @tc.name: testImageSource027
     * @tc.desc: update incremental image source from byte array with invalid offset.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testImageSource027() {
        /**
         * @tc.steps: step1.get incremental image source.
         * @tc.expected: step1.get incremental image source not null
         */
        ImageSource.SourceOptions srcOpts = new ImageSource.SourceOptions();
        imageSource = ImageSource.createIncrementalSource(srcOpts);
        assertNotNull(imageSource);

        /**
         * @tc.steps: step2.update data from byte array with invalid offset.
         * @tc.expected: step2.throw IndexOutOfBoundsException.
         */
        byte[] soureData = new byte[10];
        assertThrows("should throw IndexOutOfBoundsException", IndexOutOfBoundsException.class, () -> {
            imageSource.updateData(soureData, soureData.length, soureData.length, false);
        });
    }

    /**
     * @tc.name: testImageSource028
     * @tc.desc: update incremental image source from byte array with invalid offset and length.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testImageSource028() {
        /**
         * @tc.steps: step1.get incremental image source.
         * @tc.expected: step1.get incremental image source not null
         */
        ImageSource.SourceOptions srcOpts = new ImageSource.SourceOptions();
        imageSource = ImageSource.createIncrementalSource(srcOpts);
        assertNotNull(imageSource);

        /**
         * @tc.steps: step2.update data from byte array with invalid offset and length.
         * @tc.expected: step2.throw IndexOutOfBoundsException.
         */
        byte[] soureData = new byte[10];
        assertThrows("should throw IndexOutOfBoundsException", IndexOutOfBoundsException.class, () -> {
            imageSource.updateData(soureData, 1, soureData.length, false);
        });
    }

    /**
     * @tc.name: testImageSource029
     * @tc.desc: update date with released incremental image source.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testImageSource029() {
        /**
         * @tc.steps: step1.get incremental image source.
         * @tc.expected: step1.get incremental image source not null
         */
        ImageSource.SourceOptions srcOpts = new ImageSource.SourceOptions();
        imageSource = ImageSource.createIncrementalSource(srcOpts);
        assertNotNull(imageSource);

        /**
         * @tc.steps: step2.update data from released image source.
         * @tc.expected: step2.throw IllegalStateException.
         */
        imageSource.release();
        byte[] soureData = new byte[10];
        assertThrows("should throw IllegalStateException", IllegalStateException.class, () -> {
            imageSource.updateData(soureData, 0, soureData.length, false);
        });
    }

    /**
     * @tc.name: testImageSource030
     * @tc.desc: get pixel map thumbnail data from released image source.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testImageSource030() {
        /**
         * @tc.steps: step1.get image source form path.
         * @tc.expected: step1.get image source form path not null
         */
        String pathName = "/sdcard/multimedia/image/test.jpg";
        imageSource = ImageSource.create(pathName, null);

        /**
         * @tc.steps: step2.get pixel map thumbnail data from released image source.
         * @tc.expected: step2.throw IllegalStateException.
         */
        imageSource.release();
        assertThrows("should throw IllegalStateException", IllegalStateException.class, () -> {
            imageSource.getImageThumbnailBytes();
        });
    }

    /**
     * @tc.name: testImageSource031
     * @tc.desc: get pixel map from incomplete image source.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testImageSource031() {
        isPhoneDevice = PHONE_DEVICE.equals(Parameters.get(DEVICE_TYPE));
        LOGGER.debug("device type is PHONE: %{public}s", isPhoneDevice);
        if (!isPhoneDevice) {
            return;
        }
        /**
         * @tc.steps: step1.get image source byte array.
         * @tc.expected: step1.get image source form byte array not null
         */
        String pathName = "/sdcard/multimedia/image/test.jpg";
        byte[] data = null;
        try {
            data = Files.readAllBytes(Paths.get(pathName));
        } catch (IOException e) {
            fail("file read exception");
        }
        imageSource = ImageSource.create(data, 0, data.length - 100, null);
        assertNotNull(imageSource);

        /**
         * @tc.steps: step2.create pixel map from incomplete image source.
         * @tc.expected: step2.throw SourceDataIncompleteException.
         */
        ImageSource.DecodingOptions decodingOpts = new ImageSource.DecodingOptions();
        decodingOpts.allowPartialImage = false;

        assertThrows("should throw SourceDataIncompleteException", SourceDataIncompleteException.class, () -> {
            imageSource.createPixelmap(decodingOpts);
        });
    }

    /**
     * @tc.name: testImageSource032
     * @tc.desc: get pixel map from malformed image source.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testImageSource032() {
        /**
         * @tc.steps: step1.get image source byte array.
         * @tc.expected: step1.get image source form byte array not null
         */
        String pathName = "/sdcard/multimedia/image/test.jpg";
        byte[] data = null;
        try {
            data = Files.readAllBytes(Paths.get(pathName));
        } catch (IOException e) {
            fail("file read exception");
        }
        data[0] = 0xa;
        imageSource = ImageSource.create(data, 0, data.length, null);
        assertNotNull(imageSource);

        /**
         * @tc.steps: step2.create pixel map from malformed image source.
         * @tc.expected: step2.throw SourceDataMalformedException.
         */
        ImageSource.DecodingOptions decodingOpts = new ImageSource.DecodingOptions();
        decodingOpts.allowPartialImage = false;

        assertThrows("should throw SourceDataMalformedException", SourceDataMalformedException.class, () -> {
            imageSource.createPixelmap(decodingOpts);
        });
    }

    /**
     * @tc.name: testImageSource033
     * @tc.desc: get pixel map from ByteBuffer.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testImageSource033() {
        isPhoneDevice = PHONE_DEVICE.equals(Parameters.get(DEVICE_TYPE));
        LOGGER.debug("device type is PHONE: %{public}s", isPhoneDevice);
        if (!isPhoneDevice) {
            return;
        }

        /**
         * @tc.steps: step1.get image source ByteBuffer.
         * @tc.expected: step1.get image source form ByteBuffer not null
         */
        String pathName = "/sdcard/multimedia/image/test.jpg";
        File file = new File(pathName);
        FileChannel channel = null;
        FileInputStream fileStream = null;
        ByteBuffer byteBuffer = null;
        try {
            fileStream = new FileInputStream(file);
            channel = fileStream.getChannel();
            byteBuffer = ByteBuffer.allocate((int) channel.size());
            channel.read(byteBuffer);
        } catch (IOException e) {
            LOGGER.error("read file IOException");
        } finally {
            try {
                channel.close();
            } catch (IOException e) {
                LOGGER.error("channel close IOException");
            }
            try {
                fileStream.close();
            } catch (IOException e) {
                LOGGER.error("fileStream close IOException");
            }
        }
        imageSource = ImageSource.create(byteBuffer, null);
        assertNotNull(imageSource);
        assertEquals(472, imageSource.getImageInfo().size.width);
        assertEquals(226, imageSource.getImageInfo().size.height);

        /**
         * @tc.steps: step2.create pixel map.
         * @tc.expected: step2.create pixel map not null and check information ok
         */
        ImageSource.DecodingOptions decodingOpts = new ImageSource.DecodingOptions();
        pixelMap = imageSource.createPixelmap(decodingOpts);
        assertNotNull(pixelMap);
        assertEquals(472, pixelMap.getImageInfo().size.width);
        assertEquals(226, pixelMap.getImageInfo().size.height);
    }

    /**
     * @tc.name: testImageSource034
     * @tc.desc: get pixel map from ByteBuffer.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testImageSource034() {
        isPhoneDevice = PHONE_DEVICE.equals(Parameters.get(DEVICE_TYPE));
        LOGGER.debug("device type is PHONE: %{public}s", isPhoneDevice);
        if (!isPhoneDevice) {
            return;
        }

        /**
         * @tc.steps: step1.get image source from null ByteBuffer.
         * @tc.expected: step1.throw illegal argument exception
         */
        ByteBuffer byteBuffer = null;
        ImageSource.SourceOptions srcOpts = new ImageSource.SourceOptions();
        assertThrows("data is null", IllegalArgumentException.class, () -> {
            ImageSource.create(byteBuffer, srcOpts);
        });
    }

    /**
     * @tc.name: testImageSource035
     * @tc.desc: get pixel map from ByteBuffer.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testImageSource035() {
        isPhoneDevice = PHONE_DEVICE.equals(Parameters.get(DEVICE_TYPE));
        LOGGER.debug("device type is PHONE: %{public}s", isPhoneDevice);
        if (!isPhoneDevice) {
            return;
        }

        /**
         * @tc.steps: step1.get image source ByteBuffer.
         * @tc.expected: step1.get image source form ByteBuffer not null
         */
        String pathName = "/sdcard/multimedia/image/test.jpg";
        File file = new File(pathName);
        FileChannel channel = null;
        FileInputStream fileStream = null;
        ByteBuffer byteBuffer = null;
        try {
            fileStream = new FileInputStream(file);
            channel = fileStream.getChannel();
            byteBuffer = ByteBuffer.allocate((int)channel.size()/2);
            channel.read(byteBuffer);
        } catch (IOException e) {
            LOGGER.error("read file IOException");
        } finally {
            try {
                channel.close();
            } catch (IOException e) {
                LOGGER.error("channel close IOException");
            }
            try {
                fileStream.close();
            } catch (IOException e) {
                LOGGER.error("fileStream close IOException");
            }
        }

        imageSource = ImageSource.create(byteBuffer, null);
        assertNotNull(imageSource);
        assertEquals(472, imageSource.getImageInfo().size.width);
        assertEquals(226, imageSource.getImageInfo().size.height);

        /**
         * @tc.steps: step2.create pixel map.
         * @tc.expected: step2.create pixel map not null and check information ok
         */
        ImageSource.DecodingOptions decodingOpts = new ImageSource.DecodingOptions();
        pixelMap = imageSource.createPixelmap(decodingOpts);
        assertNotNull(pixelMap);
        assertEquals(472, pixelMap.getImageInfo().size.width);
        assertEquals(226, pixelMap.getImageInfo().size.height);
        imageSource.release();
        pixelMap.release();
    }

    /**
     * @tc.name: testImageSource036
     * @tc.desc: get pixel map from ByteBuffer.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testImageSource036() {
        isPhoneDevice = PHONE_DEVICE.equals(Parameters.get(DEVICE_TYPE));
        LOGGER.debug("device type is PHONE: %{public}s", isPhoneDevice);
        if (!isPhoneDevice) {
            return;
        }

        /**
         * @tc.steps: step1.get image source ByteBuffer.
         * @tc.expected: step1.get image source form ByteBuffer not null
         */
        String pathName = "/sdcard/multimedia/image/test.jpg";
        File file = new File(pathName);
        FileChannel channel = null;
        FileInputStream fileStream = null;
        ByteBuffer byteBuffer = null;
        try {
            fileStream = new FileInputStream(file);
            channel = fileStream.getChannel();
            byteBuffer = ByteBuffer.allocate(1);
            channel.read(byteBuffer);
        } catch (IOException e) {
            LOGGER.error("read file IOException");
        } finally {
            try {
                channel.close();
            } catch (IOException e) {
                LOGGER.error("channel close IOException");
            }
            try {
                fileStream.close();
            } catch (IOException e) {
                LOGGER.error("fileStream close IOException");
            }
        }

        imageSource = ImageSource.create(byteBuffer, null);
        assertNotNull(imageSource);
        assertNull(imageSource.getImageInfo());

        /**
         * @tc.steps: step2.create pixel map.
         * @tc.expected: step2.create pixel map not null and check information ok
         */
        ImageSource.DecodingOptions decodingOpts = new ImageSource.DecodingOptions();
        assertThrows("should throw SourceDataIncompleteException", SourceDataIncompleteException.class, () -> {
            imageSource.createPixelmap(decodingOpts);
        });
    }

    /**
     * @tc.name: testImageSource037
     * @tc.desc: get ImageInfo test.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testImageSource037() {
        isPhoneDevice = PHONE_DEVICE.equals(Parameters.get(DEVICE_TYPE));
        LOGGER.debug("device type is PHONE: %{public}s", isPhoneDevice);
        if (!isPhoneDevice) {
            return;
        }

        /**
         * @tc.steps: step1.get image source ByteBuffer.
         * @tc.expected: step1.get image source form ByteBuffer not null
         */
        String pathName = "/sdcard/multimedia/image/test.jpg";
        File file = new File(pathName);
        FileChannel channel = null;
        FileInputStream fileStream = null;
        ByteBuffer byteBuffer = null;
        try {
            fileStream = new FileInputStream(file);
            channel = fileStream.getChannel();
            byteBuffer = ByteBuffer.allocate((int) channel.size());
            channel.read(byteBuffer);
        } catch (IOException e) {
            LOGGER.error("read file IOException");
        } finally {
            try {
                channel.close();
            } catch (IOException e) {
                LOGGER.error("channel close IOException");
            }
            try {
                fileStream.close();
            } catch (IOException e) {
                LOGGER.error("fileStream close IOException");
            }
        }
        imageSource = ImageSource.create(byteBuffer, null);
        assertNotNull(imageSource);
        assertEquals(472, imageSource.getImageInfo().size.width);
        assertEquals(226, imageSource.getImageInfo().size.height);
        assertTrue(imageSource.getImageInfo().toString().contains("pixelFormat"));
    }

    /**
     * @tc.name: testImageSourcePng001
     * @tc.desc: create image source from file path.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testImageSourcePng001() {
        /**
         * @tc.steps: step1.get image source form path.
         * @tc.expected: step1.get image source form path not null
         */
        ImageSource.SourceOptions srcOpts = new ImageSource.SourceOptions();
        String pathName = "/sdcard/multimedia/image/test.png";
        imageSource = ImageSource.create(pathName, srcOpts);
        assertNotNull(imageSource);
        ImageSource.SourceInfo sourceInfo = imageSource.getSourceInfo();
        assertNotNull(sourceInfo);
        assertEquals(1, sourceInfo.topLevelImageNum);
        assertEquals("image/png", sourceInfo.encodedFormat);

        /**
         * @tc.steps: step2.create pixel map.
         * @tc.expected: step2.create pixel map not null and check information ok
         */
        ImageSource.DecodingOptions decodingOpts = new ImageSource.DecodingOptions();
        decodingOpts.desiredSize = new Size(0, 0);
        decodingOpts.desiredRegion = new Rect(0, 0, 0, 0);
        pixelMap = imageSource.createPixelmap(decodingOpts);
        assertNotNull(pixelMap);
        assertEquals(472, pixelMap.getImageInfo().size.width);
        assertEquals(75, pixelMap.getImageInfo().size.height);
        assertEquals(472, imageSource.getImageInfo().size.width);
        assertEquals(75, imageSource.getImageInfo().size.height);
    }

    /**
     * @tc.name: testImageSourcePng002
     * @tc.desc: create image source from byte array.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testImageSourcePng002() {
        /**
         * @tc.steps: step1.get image source byte array.
         * @tc.expected: step1.get image source form byte array not null
         */
        ImageSource.SourceOptions srcOpts = new ImageSource.SourceOptions();
        srcOpts.formatHint = "image/png";
        String pathName = "/sdcard/multimedia/image/test.png";
        Path fileLocation = Paths.get(pathName);
        byte[] data = null;
        try {
            data = Files.readAllBytes(fileLocation);
        } catch (IOException e) {
            LOGGER.error("file read exception");
            fail("file read exception");
        }
        imageSource = ImageSource.create(data, srcOpts);
        assertNotNull(imageSource);

        /**
         * @tc.steps: step2.create pixel map.
         * @tc.expected: step2.create pixel map not null and check information ok
         */
        ImageSource.DecodingOptions decodingOpts = new ImageSource.DecodingOptions();
        decodingOpts.desiredSize = new Size(0, 0);
        decodingOpts.desiredRegion = new Rect(0, 0, 0, 0);
        pixelMap = imageSource.createPixelmap(0, decodingOpts);
        assertNotNull(pixelMap);
        assertEquals(472, pixelMap.getImageInfo().size.width);
        assertEquals(75, pixelMap.getImageInfo().size.height);
        assertEquals(472, imageSource.getImageInfo(0).size.width);
        assertEquals(75, imageSource.getImageInfo(0).size.height);
    }

    /**
     * @tc.name: testImageSourcePng003
     * @tc.desc: create image source from byte array with offset and length.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testImageSourcePng003() {
        /**
         * @tc.steps: step1.get image source byte array with offset and length.
         * @tc.expected: step1.get image source form byte array with offset and length not null
         */
        ImageSource.SourceOptions srcOpts = new ImageSource.SourceOptions();
        String pathName = "/sdcard/multimedia/image/test.png";
        Path fileLocation = Paths.get(pathName);
        byte[] data = null;
        try {
            data = Files.readAllBytes(fileLocation);
        } catch (IOException e) {
            LOGGER.error("file read exception");
            fail("file read exception");
        }
        imageSource = ImageSource.create(data, 0, data.length, srcOpts);
        assertNotNull(imageSource);

        /**
         * @tc.steps: step2.create pixel map.
         * @tc.expected: step2.create pixel map not null and check information ok
         */
        ImageSource.DecodingOptions decodingOpts = new ImageSource.DecodingOptions();
        decodingOpts.desiredSize = new Size(0, 0);
        decodingOpts.desiredRegion = new Rect(0, 0, 0, 0);
        pixelMap = imageSource.createPixelmap(decodingOpts);
        assertNotNull(pixelMap);
        assertEquals(472, pixelMap.getImageInfo().size.width);
        assertEquals(75, pixelMap.getImageInfo().size.height);
    }

    /**
     * @tc.name: testImageSourcePng004
     * @tc.desc: create image source from File.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testImageSourcePng004() {
        /**
         * @tc.steps: step1.get image source File.
         * @tc.expected: step1.get image source form File not null
         */
        ImageSource.SourceOptions srcOpts = new ImageSource.SourceOptions();
        String pathName = "/sdcard/multimedia/image/test.png";
        File file = new File(pathName);
        imageSource = ImageSource.create(file, srcOpts);
        assertNotNull(imageSource);

        /**
         * @tc.steps: step2.create pixel map.
         * @tc.expected: step2.create pixel map not null and check information ok
         */
        ImageSource.DecodingOptions decodingOpts = new ImageSource.DecodingOptions();
        decodingOpts.desiredSize = new Size(0, 0);
        decodingOpts.desiredRegion = new Rect(0, 0, 0, 0);
        pixelMap = imageSource.createPixelmap(decodingOpts);
        assertNotNull(pixelMap);
        assertEquals(472, pixelMap.getImageInfo().size.width);
        assertEquals(75, pixelMap.getImageInfo().size.height);
    }

    /**
     * @tc.name: testImageSourcePng005
     * @tc.desc: create image source from FileDescriptor.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testImageSourcePng005() {
        /**
         * @tc.steps: step1.get image source FileDescriptor.
         * @tc.expected: step1.get image source form FileDescriptor not null
         */
        ImageSource.SourceOptions srcOpts = new ImageSource.SourceOptions();
        String pathName = "/sdcard/multimedia/image/test.png";
        FileDescriptor fileDescriptor;
        try (FileInputStream inputStream = new FileInputStream(pathName)) {
            fileDescriptor = inputStream.getFD();
            imageSource = ImageSource.create(fileDescriptor, srcOpts);
            assertNotNull(imageSource);
        } catch (FileNotFoundException fe) {
            LOGGER.error("createImageSource from InputStream, file not found.");
        } catch (IOException fio) {
            LOGGER.error("createImageSource from InputStream, IO Exception");
        }

        /**
         * @tc.steps: step2.create pixel map.
         * @tc.expected: step2.create pixel map not null and check information ok
         */
        ImageSource.DecodingOptions decodingOpts = new ImageSource.DecodingOptions();
        decodingOpts.desiredSize = new Size(0, 0);
        decodingOpts.desiredRegion = new Rect(0, 0, 0, 0);
        pixelMap = imageSource.createPixelmap(decodingOpts);
        assertNotNull(pixelMap);
        assertEquals(472, pixelMap.getImageInfo().size.width);
        assertEquals(75, pixelMap.getImageInfo().size.height);
    }

    /**
     * @tc.name: testImageSourcePng006
     * @tc.desc: create image source from InputStream.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testImageSourcePng006() {
        /**
         * @tc.steps: step1.get image source InputStream.
         * @tc.expected: step1.get image source form InputStream not null
         */
        ImageSource.SourceOptions srcOpts = new ImageSource.SourceOptions();
        srcOpts.formatHint = "image/png";
        String pathName = "/sdcard/multimedia/image/test.png";
        try (InputStream inputStream = new FileInputStream(pathName)) {
            imageSource = ImageSource.create(inputStream, srcOpts);
            assertNotNull(imageSource);
        } catch (FileNotFoundException fe) {
            LOGGER.error("createImageSource from InputStream, file not found");
        } catch (IOException ioe) {
            LOGGER.error("file read io exception");
        }

        /**
         * @tc.steps: step2.create pixel map.
         * @tc.expected: step2.create pixel map not null and check information ok
         */
        ImageSource.DecodingOptions decodingOpts = new ImageSource.DecodingOptions();
        decodingOpts.desiredSize = new Size(0, 0);
        decodingOpts.desiredRegion = new Rect(0, 0, 0, 0);
        pixelMap = imageSource.createPixelmap(decodingOpts);
        assertNotNull(pixelMap);
        assertEquals(472, pixelMap.getImageInfo().size.width);
        assertEquals(75, pixelMap.getImageInfo().size.height);
    }

    /**
     * @tc.name: testImageSourcePng007
     * @tc.desc: create incremental image source.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testImageSourcePng007() {
        /**
         * @tc.steps: step1.get incremental image source.
         * @tc.expected: step1.get incremental image source not null
         */
        ImageSource.SourceOptions srcOpts = new ImageSource.SourceOptions();
        imageSource = ImageSource.createIncrementalSource(srcOpts);

        String pathName = "/sdcard/multimedia/image/test.png";
        Path fileLocation = Paths.get(pathName);
        byte[] data = null;
        try {
            data = Files.readAllBytes(fileLocation);
        } catch (IOException e) {
            LOGGER.error("file read io exception");
            fail("file read exception");
        }
        imageSource.updateData(data, true);
        assertNotNull(imageSource);

        /**
         * @tc.steps: step2.create pixel map.
         * @tc.expected: step2.create pixel map not null and check information ok
         */
        ImageSource.DecodingOptions decodingOpts = new ImageSource.DecodingOptions();
        decodingOpts.desiredSize = new Size(0, 0);
        decodingOpts.desiredRegion = new Rect(0, 0, 0, 0);
        pixelMap = imageSource.createPixelmap(decodingOpts);
        assertNotNull(pixelMap);
        assertEquals(472, pixelMap.getImageInfo().size.width);
        assertEquals(75, pixelMap.getImageInfo().size.height);
    }

    /**
     * @tc.name: testImageSourceNinePathcPng001
     * @tc.desc: The upper-layer application(ex:graphic) obtained nine patch info from the .9.png file.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testImageSourceNinePathcPng001() {
        /**
         * @tc.steps: step1.get image source from test.9.png file.
         * @tc.expected: step1.get image source form File not null.
         */
        ImageSource.SourceOptions srcOpts = new ImageSource.SourceOptions();
        String pathName = "/sdcard/multimedia/image/test.9.png";
        File file = new File(pathName);
        imageSource = ImageSource.create(file, srcOpts);
        assertNotNull(imageSource);

        /**
         * @tc.steps: step2.create pixel map by image source.
         * @tc.expected: step2.create pixel map not null.
         */
        ImageSource.DecodingOptions decodingOpts = new ImageSource.DecodingOptions();
        pixelMap = imageSource.createPixelmap(decodingOpts);
        assertNotNull(pixelMap);

        /**
         * @tc.steps: step3.get nine patch info from pixelmap, fill the content (ex:text,sound)
         * and send or display by adjust (stretch) the nine patch info
         * @tc.expected: step3.nine patch info is not null.
         */
        byte[] result = pixelMap.getNinePatchChunk();
        assertNotNull(result);
    }

    /**
     * @tc.name: testImageSourceJpg001
     * @tc.desc: create image source from file path.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testImageSourceJpg001() {
        /**
         * @tc.steps: step1.get image source form path.
         * @tc.expected: step1.get image source form path not null
         */
        String pathName = "/sdcard/multimedia/image/test.jpg";
        imageSource = ImageSource.create(pathName, null);
        assertNotNull(imageSource);
        ImageSource.SourceInfo sourceInfo = imageSource.getSourceInfo();
        assertNotNull(sourceInfo);
        assertEquals(1, sourceInfo.topLevelImageNum);
        assertEquals("image/jpeg", sourceInfo.encodedFormat);

        /**
         * @tc.steps: step2.create pixel map.
         * @tc.expected: step2.create pixel map not null and check information ok
         */
        pixelMap = imageSource.createPixelmap(null);
        assertNotNull(pixelMap);
        assertEquals(472, pixelMap.getImageInfo().size.width);
        assertEquals(226, pixelMap.getImageInfo().size.height);
        assertEquals(472, imageSource.getImageInfo().size.width);
        assertEquals(226, imageSource.getImageInfo().size.height);
    }

    /**
     * @tc.name: testImageSourceJpg002
     * @tc.desc: create image source from byte array.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testImageSourceJpg002() {
        /**
         * @tc.steps: step1.get image source byte array.
         * @tc.expected: step1.get image source form byte array not null
         */
        String pathName = "/sdcard/multimedia/image/test.jpg";
        Path fileLocation = Paths.get(pathName);
        byte[] data = null;
        try {
            data = Files.readAllBytes(fileLocation);
        } catch (IOException e) {
            LOGGER.error("file read exception");
            fail("file read exception");
        }
        imageSource = ImageSource.create(data, null);
        assertNotNull(imageSource);

        /**
         * @tc.steps: step2.create pixel map.
         * @tc.expected: step2.create pixel map not null and check information ok
         */
        pixelMap = imageSource.createPixelmap(0, null);
        assertNotNull(pixelMap);
        assertEquals(472, pixelMap.getImageInfo().size.width);
        assertEquals(226, pixelMap.getImageInfo().size.height);
    }

    /**
     * @tc.name: testImageSourceJpg003
     * @tc.desc: create image source from byte array with offset and length.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testImageSourceJpg003() {
        /**
         * @tc.steps: step1.get image source byte array with offset and length.
         * @tc.expected: step1.get image source form byte array with offset and length not null
         */
        String pathName = "/sdcard/multimedia/image/test.jpg";
        Path fileLocation = Paths.get(pathName);
        byte[] data = null;
        try {
            data = Files.readAllBytes(fileLocation);
        } catch (IOException e) {
            LOGGER.error("file read exception");
            fail("file read exception");
        }
        imageSource = ImageSource.create(data, 0, data.length, null);
        assertNotNull(imageSource);

        /**
         * @tc.steps: step2.create pixel map.
         * @tc.expected: step2.create pixel map not null and check information ok
         */
        pixelMap = imageSource.createPixelmap(null);
        assertNotNull(pixelMap);
        assertEquals(472, pixelMap.getImageInfo().size.width);
        assertEquals(226, pixelMap.getImageInfo().size.height);
    }

    /**
     * @tc.name: testImageSourceJpg004
     * @tc.desc: create image source from File.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testImageSourceJpg004() {
        /**
         * @tc.steps: step1.get image source File.
         * @tc.expected: step1.get image source form File not null
         */
        String pathName = "/sdcard/multimedia/image/test.jpg";
        File file = new File(pathName);
        imageSource = ImageSource.create(file, null);
        assertNotNull(imageSource);

        /**
         * @tc.steps: step2.create pixel map.
         * @tc.expected: step2.create pixel map not null and check information ok
         */
        pixelMap = imageSource.createPixelmap(null);
        assertNotNull(pixelMap);
        assertEquals(472, pixelMap.getImageInfo().size.width);
        assertEquals(226, pixelMap.getImageInfo().size.height);
    }

    /**
     * @tc.name: testImageSourceJpg005
     * @tc.desc: create image source from FileDescriptor.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testImageSourceJpg005() {
        /**
         * @tc.steps: step1.get image source FileDescriptor.
         * @tc.expected: step1.get image source form FileDescriptor not null
         */
        String pathName = "/sdcard/multimedia/image/test.jpg";
        FileDescriptor fileDescriptor;
        try (FileInputStream inputStream = new FileInputStream(pathName)) {
            fileDescriptor = inputStream.getFD();
            imageSource = ImageSource.create(fileDescriptor, null);
            ExifAdapter exifAdapter = new ExifAdapter(fileDescriptor);
            assertNotNull(imageSource);
        } catch (FileNotFoundException fe) {
            LOGGER.error("createImageSource from InputStream, file not found.");
        } catch (IOException fio) {
            LOGGER.error("createImageSource from InputStream, IO Exception");
        }

        /**
         * @tc.steps: step2.create pixel map.
         * @tc.expected: step2.create pixel map not null and check information ok
         */
        pixelMap = imageSource.createPixelmap(null);
        assertNotNull(pixelMap);
        assertEquals(472, pixelMap.getImageInfo().size.width);
        assertEquals(226, pixelMap.getImageInfo().size.height);
    }

    /**
     * @tc.name: testImageSourceJpg006
     * @tc.desc: create image source from InputStream.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testImageSourceJpg006() {
        /**
         * @tc.steps: step1.get image source InputStream.
         * @tc.expected: step1.get image source form InputStream not null
         */
        String pathName = "/sdcard/multimedia/image/test.jpg";
        try (InputStream inputStream = new FileInputStream(pathName)) {
            imageSource = ImageSource.create(inputStream, null);
            ExifAdapter exifAdapter = new ExifAdapter(inputStream);
            assertNotNull(imageSource);
        } catch (FileNotFoundException fe) {
            LOGGER.error("createImageSource from InputStream, file not found");
        } catch (IOException ioe) {
            LOGGER.error("file read io exception");
        }

        /**
         * @tc.steps: step2.create pixel map.
         * @tc.expected: step2.create pixel map not null and check information ok
         */
        pixelMap = imageSource.createPixelmap(null);
        assertNotNull(pixelMap);
        assertEquals(472, pixelMap.getImageInfo().size.width);
        assertEquals(226, pixelMap.getImageInfo().size.height);
    }

    /**
     * @tc.name: testImageSourceJpg007
     * @tc.desc: create incremental image source.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testImageSourceJpg007() {
        /**
         * @tc.steps: step1.get incremental image source.
         * @tc.expected: step1.get incremental image source not null
         */
        ImageSource.SourceOptions srcOpts = null;
        imageSource = ImageSource.createIncrementalSource(srcOpts);

        String pathName = "/sdcard/multimedia/image/test.jpg";
        Path fileLocation = Paths.get(pathName);
        byte[] data = null;
        try {
            data = Files.readAllBytes(fileLocation);
        } catch (IOException e) {
            LOGGER.error("file read io exception");
            fail("file read exception");
        }
        imageSource.updateData(data, true);
        assertNotNull(imageSource);

        /**
         * @tc.steps: step2.create pixel map.
         * @tc.expected: step2.create pixel map not null and check information ok
         */
        pixelMap = imageSource.createPixelmap(null);
        assertNotNull(pixelMap);
        assertEquals(472, pixelMap.getImageInfo().size.width);
        assertEquals(226, pixelMap.getImageInfo().size.height);
    }

    /**
     * @tc.name: testImageSourceJpg008
     * @tc.desc: create incremental image source.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testImageSourceJpg008() {
        /**
         * @tc.steps: step1.get incremental image source.
         * @tc.expected: step1.get incremental image source not null
         */
        ImageSource.IncrementalSourceOptions incOpts = null;
        imageSource = ImageSource.createIncrementalSource(incOpts);
        assertNotNull(imageSource);

        /**
         * @tc.steps: step2.create pixel map.
         * @tc.expected: step2.create pixel map not null and check information ok
         */
        String pathName = "/sdcard/multimedia/image/test.jpg";
        final int size = 100;
        byte[] data = new byte[size];
        File file = new File(pathName);
        try (FileInputStream fileInputStream = new FileInputStream(file)) {
            int bytes = 0;
            while (bytes != -1) {
                bytes = fileInputStream.read(data);
                if (bytes == -1) {
                    break;
                }
                boolean isFinal = (bytes != size);
                imageSource.updateData(data, 0, bytes, isFinal);
                pixelMap = imageSource.createPixelmap(null);
                assertNotNull(pixelMap);
                if (pixelMap.getImageInfo().size.width == 0) {
                    continue;
                }
                assertEquals(472, pixelMap.getImageInfo().size.width);
                assertEquals(226, pixelMap.getImageInfo().size.height);
                pixelMap.release();
            }
        } catch (IOException e) {
            fail("file read exception");
        }
    }

    /**
     * @tc.name: testImageSourceJpg009
     * @tc.desc: decode editable pixel map
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testImageSourceJpg009() {
        /**
         * @tc.steps: step1. decode webp file to get editable pixelmap.
         * @tc.expected: step1. get the valid pixelMap from the image.
         */
        imageSource = ImageSource.create("/sdcard/multimedia/image/test.jpg", null);
        assertNotNull(imageSource);
        ImageSource.DecodingOptions decodingOpts = new ImageSource.DecodingOptions();
        decodingOpts.editable = true;
        pixelMap = imageSource.createPixelmap(decodingOpts);
        assertNotNull(pixelMap);

        /**
         * @tc.steps: step1. check pixel map if editable.
         * @tc.expected: step1. check ok.
         */
        assertTrue(pixelMap.isEditable());
    }

    /**
     * @tc.name: testImageSourceJpg010
     * @tc.desc: decode uneditable pixel map
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testImageSourceJpg010() {
        /**
         * @tc.steps: step1. decode webp file to get uneditable pixelmap.
         * @tc.expected: step1. get the valid pixelMap from the image.
         */
        imageSource = ImageSource.create("/sdcard/multimedia/image/test.jpg", null);
        assertNotNull(imageSource);
        ImageSource.DecodingOptions decodingOpts = new ImageSource.DecodingOptions();
        decodingOpts.editable = false;
        pixelMap = imageSource.createPixelmap(decodingOpts);
        assertNotNull(pixelMap);

        /**
         * @tc.steps: step1. check pixel map if editable.
         * @tc.expected: step1. check ok.
         */
        assertFalse(pixelMap.isEditable());
    }

    /**
     * @tc.name: testImageSourceExif001
     * @tc.desc: get image exif info.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testImageSourceExif001() {
        /**
         * @tc.steps: step1.get imagesource.
         * @tc.expected: step1.imagesource not null.
         */
        ImageSource.SourceOptions srcOpts = new ImageSource.SourceOptions();
        srcOpts.formatHint = "image/jpeg";
        String pathName = "/sdcard/multimedia/image/test_exif.jpg";
        imageSource = ImageSource.create(pathName, srcOpts);
        assertNotNull(imageSource);

        /**
         * @tc.steps: step2.get image exif info.
         * @tc.expected: step2.check image exif info not null and ok.
         */
        byte[] thumbnailData = imageSource.getImageThumbnailBytes();
        assertNotNull(thumbnailData);
        assertEquals(ImageFormat.JPEG, imageSource.getThumbnailFormat());

        /**
         * @tc.steps: step3.get image thumbnail pixel map.
         * @tc.expected: step3.check thumbnail pixel map ok.
         */
        ImageSource.DecodingOptions decodingOpts = new ImageSource.DecodingOptions();
        PixelMap thumbnailPixelmap = imageSource.createThumbnailPixelmap(decodingOpts, false);
        assertNotNull(thumbnailPixelmap);
        PixelMap thumbnailPixelmap2 = imageSource.createThumbnailPixelmap(decodingOpts, false);
        assertNotNull(thumbnailPixelmap2);
        assertEquals(160, thumbnailPixelmap.getImageInfo().size.width);
        assertEquals(120, thumbnailPixelmap.getImageInfo().size.height);
        assertEquals(160, imageSource.getThumbnailInfo().size.width);
        assertEquals(120, imageSource.getThumbnailInfo().size.height);

        /**
         * @tc.steps: step4.get image property info.
         * @tc.expected: step4.check image property info ok.
         */
        PropertyKey.Exif exif = new PropertyKey.Exif();
        assertEquals("6", imageSource.getImagePropertyString(PropertyKey.Exif.COMPRESSION));
        assertEquals(5952, imageSource.getImagePropertyInt(PropertyKey.Exif.PIXEL_X_DIMENSION, 0));
    }

    /**
     * @tc.name: testImageSourceExif002
     * @tc.desc: get image thumbnail pixel map from image without exif info.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testImageSourceExif002() {
        /**
         * @tc.steps: step1.get image source.
         * @tc.expected: step1.image source not null.
         */
        ImageSource.SourceOptions srcOpts = new ImageSource.SourceOptions();
        srcOpts.formatHint = "image/jpeg";
        String pathName = "/sdcard/multimedia/image/test.jpg";
        imageSource = ImageSource.create(pathName, srcOpts);
        assertNotNull(imageSource);

        /**
         * @tc.steps: step2.get image exif thumbnail from original image.
         * @tc.expected: step2.check image thumbnail info not null and ok.
         */
        ImageSource.DecodingOptions decodingOpts = new ImageSource.DecodingOptions();
        decodingOpts.desiredSize = new Size(40, 10);
        PixelMap thumbnailPixelmap = imageSource.createThumbnailPixelmap(decodingOpts, true);
        assertNotNull(thumbnailPixelmap);
        assertEquals(40, thumbnailPixelmap.getImageInfo().size.width);
        assertEquals(10, thumbnailPixelmap.getImageInfo().size.height);
    }

    /**
     * @tc.name: testImageSourceExif003
     * @tc.desc: get image exif thumbnail data from image without exif info.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testImageSourceExif003() {
        /**
         * @tc.steps: step1.get image source.
         * @tc.expected: step1.image source not null.
         */
        ImageSource.SourceOptions srcOpts = new ImageSource.SourceOptions();
        srcOpts.formatHint = "image/jpeg";
        String pathName = "/sdcard/multimedia/image/test.jpg";
        imageSource = ImageSource.create(pathName, srcOpts);
        assertNotNull(imageSource);

        /**
         * @tc.steps: step2.get image exif info.
         * @tc.expected: step2.check image exif info null.
         */
        assertThrows("should throw UnsupportedOperationException", UnsupportedOperationException.class, () -> {
            imageSource.getImageThumbnailBytes();
        });
    }

    /**
     * @tc.name: testImageSourceExif004
     * @tc.desc: get image exif thumbnail pixel map from image without exif info.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testImageSourceExif004() {
        /**
         * @tc.steps: step1.get image source.
         * @tc.expected: step1.image source not null.
         */
        ImageSource.SourceOptions srcOpts = new ImageSource.SourceOptions();
        srcOpts.formatHint = "image/jpeg";
        String pathName = "/sdcard/multimedia/image/test.jpg";
        imageSource = ImageSource.create(pathName, srcOpts);
        assertNotNull(imageSource);

        /**
         * @tc.steps: step2.get image thumbnail info.
         * @tc.expected: step2.check image thumbnail info null.
         */
        ImageSource.DecodingOptions decodingOpts = new ImageSource.DecodingOptions();
        PixelMap thumbnailPixelmap = imageSource.createThumbnailPixelmap(decodingOpts, false);
        assertNull(thumbnailPixelmap);
    }

    /**
     * @tc.name: testImageSourceExif005
     * @tc.desc: get image exif thumbnail info from image without exif info.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testImageSourceExif005() {
        /**
         * @tc.steps: step1.get image source.
         * @tc.expected: step1.image source not null.
         */
        ImageSource.SourceOptions srcOpts = new ImageSource.SourceOptions();
        srcOpts.formatHint = "image/jpeg";
        String pathName = "/sdcard/multimedia/image/test.jpg";
        imageSource = ImageSource.create(pathName, srcOpts);
        assertNotNull(imageSource);

        /**
         * @tc.steps: step2.get image exif thumbnail info.
         * @tc.expected: step2.check image exif thumbnail info null.
         */
        assertThrows("should throw UnsupportedOperationException", UnsupportedOperationException.class, () -> {
            imageSource.getThumbnailInfo();
        });
    }

    /**
     * @tc.name: testImageSourceExif006
     * @tc.desc: get image exif tag info from image without exif info.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testImageSourceExif006() {
        /**
         * @tc.steps: step1.get image source.
         * @tc.expected: step1.image source not null.
         */
        ImageSource.SourceOptions srcOpts = new ImageSource.SourceOptions();
        srcOpts.formatHint = "image/jpeg";
        String pathName = "/sdcard/multimedia/image/test.jpg";
        imageSource = ImageSource.create(pathName, srcOpts);
        assertNotNull(imageSource);

        /**
         * @tc.steps: step2.get image exif tag info.
         * @tc.expected: step2.check image exif tag info null.
         */
        assertNull(imageSource.getImagePropertyString(PropertyKey.Exif.COMPRESSION));
    }

    /**
     * @tc.name: testImageSourceExif007
     * @tc.desc: get image exif thumbnail data when source released.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testImageSourceExif007() {
        /**
         * @tc.steps: step1.get image source.
         * @tc.expected: step1.image source not null.
         */
        ImageSource.SourceOptions srcOpts = new ImageSource.SourceOptions();
        srcOpts.formatHint = "image/jpeg";
        String pathName = "/sdcard/multimedia/image/test_exif.jpg";
        imageSource = ImageSource.create(pathName, srcOpts);
        assertNotNull(imageSource);
        imageSource.release();

        /**
         * @tc.steps: step2.get image exif thumbnail data.
         * @tc.expected: step2.check exception ok.
         */
        assertThrows("should throw IllegalStateException", IllegalStateException.class, () -> {
            imageSource.getImageThumbnailBytes();
        });
    }

    /**
     * @tc.name: testImageSourceExif008
     * @tc.desc: get image exif thumbnail pixel map when source released.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testImageSourceExif008() {
        /**
         * @tc.steps: step1.get image source.
         * @tc.expected: step1.imagesource not null.
         */
        ImageSource.SourceOptions srcOpts = new ImageSource.SourceOptions();
        srcOpts.formatHint = "image/jpeg";
        String pathName = "/sdcard/multimedia/image/test_exif.jpg";
        imageSource = ImageSource.create(pathName, srcOpts);
        assertNotNull(imageSource);
        imageSource.release();

        /**
         * @tc.steps: step2.get image exif thumbnail pixel map.
         * @tc.expected: step2.check image exif thumbnail pixel map exception.
         */
        ImageSource.DecodingOptions decodingOpts = new ImageSource.DecodingOptions();
        assertThrows("should throw IllegalStateException", IllegalStateException.class, () -> {
            imageSource.createThumbnailPixelmap(decodingOpts, false);
        });
    }

    /**
     * @tc.name: testImageSourceExif009
     * @tc.desc: get image exif thumbnail info when source released.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testImageSourceExif009() {
        /**
         * @tc.steps: step1.get imagesource.
         * @tc.expected: step1.imagesource not null.
         */
        ImageSource.SourceOptions srcOpts = new ImageSource.SourceOptions();
        srcOpts.formatHint = "image/jpeg";
        String pathName = "/sdcard/multimedia/image/test_exif.jpg";
        imageSource = ImageSource.create(pathName, srcOpts);
        assertNotNull(imageSource);
        imageSource.release();

        /**
         * @tc.steps: step2.get image exif thumbnail info.
         * @tc.expected: step2.check image exif thumbnail info exception.
         */
        assertThrows("should throw IllegalStateException", IllegalStateException.class, () -> {
            imageSource.getThumbnailInfo();
        });
    }

    /**
     * @tc.name: testImageSourceExif010
     * @tc.desc: get image exif info when source released.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testImageSourceExif010() {
        /**
         * @tc.steps: step1.get image source.
         * @tc.expected: step1.image source not null.
         */
        ImageSource.SourceOptions srcOpts = new ImageSource.SourceOptions();
        srcOpts.formatHint = "image/jpeg";
        String pathName = "/sdcard/multimedia/image/test_exif.jpg";
        imageSource = ImageSource.create(pathName, srcOpts);
        assertNotNull(imageSource);
        imageSource.release();

        /**
         * @tc.steps: step2.get image exif info.
         * @tc.expected: step2.check image exif info exception.
         */
        assertThrows("should throw IllegalStateException", IllegalStateException.class, () -> {
            imageSource.getImagePropertyString(PropertyKey.Exif.COMPRESSION);
        });
    }

    /**
     * @tc.name: testImageSourceExif011
     * @tc.desc: get image exif info when source released.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testImageSourceExif011() {
        /**
         * @tc.steps: step1.get image source.
         * @tc.expected: step1.image source not null.
         */
        ImageSource.SourceOptions srcOpts = new ImageSource.SourceOptions();
        srcOpts.formatHint = "image/jpeg";
        String pathName = "/sdcard/multimedia/image/test_exif.jpg";
        imageSource = ImageSource.create(pathName, srcOpts);
        assertNotNull(imageSource);

        /**
         * @tc.steps: step2.get image exif info.
         * @tc.expected: step2.check image exif info exception.
         */
        assertThrows("should throw IllegalArgumentException", IllegalArgumentException.class, () -> {
            imageSource.getImagePropertyString(null);
        });
    }

    /**
     * @tc.name: testImageSourceExif012
     * @tc.desc: check whether image has thumbnail on image without exif.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testImageSourceExif012() {
        /**
         * @tc.steps: step1.get image source.
         * @tc.expected: step1.image source not null.
         */
        ImageSource.SourceOptions srcOpts = new ImageSource.SourceOptions();
        srcOpts.formatHint = "image/jpeg";
        String pathName = "/sdcard/multimedia/image/test.jpg";
        imageSource = ImageSource.create(pathName, srcOpts);
        assertNotNull(imageSource);

        /**
         * @tc.steps: step2.get image exif thumbnail info.
         * @tc.expected: step2.check image exif thumbnail info false.
         */
        assertThrows("should throw UnsupportedOperationException", UnsupportedOperationException.class, () -> {
            imageSource.getThumbnailInfo();
        });
    }

    /**
     * @tc.name: testImageSourceExif013
     * @tc.desc: check whether image has thumbnail on image with exif.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testImageSourceExif013() {
        /**
         * @tc.steps: step1.get image source.
         * @tc.expected: step1.image source not null.
         */
        ImageSource.SourceOptions srcOpts = new ImageSource.SourceOptions();
        srcOpts.formatHint = "image/jpeg";
        String pathName = "/sdcard/multimedia/image/test_exif.jpg";
        imageSource = ImageSource.create(pathName, srcOpts);
        assertNotNull(imageSource);

        /**
         * @tc.steps: step2.get image exif thumbnail info.
         * @tc.expected: step2.check image exif thumbnail info ok.
         */
        assertNotNull(imageSource.getThumbnailInfo());
    }

    /**
     * @tc.name: testImageSourceExif014
     * @tc.desc: get image exif latitude and longitude info.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testImageSourceExif014() {
        /**
         * @tc.steps: step1.get image source.
         * @tc.expected: step1.image source not null.
         */
        ImageSource.SourceOptions srcOpts = new ImageSource.SourceOptions();
        srcOpts.formatHint = "image/jpeg";
        String pathName = "/sdcard/multimedia/image/test_exif.jpg";
        imageSource = ImageSource.create(pathName, srcOpts);
        assertNotNull(imageSource);

        /**
         * @tc.steps: step2.get image exif latitude and longitude info.
         * @tc.expected: step2.check image exif info ok.
         */
        Pair<Float, Float> latlong = ExifUtils.getLatLong(imageSource);
        assertEquals(31.258055, latlong.f, 1e-6);
        assertEquals(121.619164, latlong.s, 1e-6);
    }

    /**
     * @tc.name: testImageSourceExif015
     * @tc.desc: get image exif latitude and longitude info which not exist.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testImageSourceExif015() {
        /**
         * @tc.steps: step1.get image source.
         * @tc.expected: step1.image source not null.
         */
        ImageSource.SourceOptions srcOpts = new ImageSource.SourceOptions();
        srcOpts.formatHint = "image/jpeg";
        String pathName = "/sdcard/multimedia/image/test.jpg";
        imageSource = ImageSource.create(pathName, srcOpts);
        assertNotNull(imageSource);

        /**
         * @tc.steps: step2.get image exif latitude and longitude info.
         * @tc.expected: step2.check image exif info ok.
         */
        assertNull(ExifUtils.getLatLong(imageSource));
    }

    /**
     * @tc.name: testImageSourceExif016
     * @tc.desc: get image exif altitude info.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testImageSourceExif016() {
        /**
         * @tc.steps: step1.get image source.
         * @tc.expected: step1.image source not null.
         */
        ImageSource.SourceOptions srcOpts = new ImageSource.SourceOptions();
        srcOpts.formatHint = "image/jpeg";
        String pathName = "/sdcard/multimedia/image/test_exif.jpg";
        imageSource = ImageSource.create(pathName, srcOpts);
        assertNotNull(imageSource);

        /**
         * @tc.steps: step2.get image exif altitude info.
         * @tc.expected: step2.check image exif info ok.
         */
        double defaultValue = 1.0;
        assertEquals(0.0, ExifUtils.getAltitude(imageSource, defaultValue), 1e-6);
    }

    /**
     * @tc.name: testImageSourceExif017
     * @tc.desc: get image exif altitude info which not exist.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testImageSourceExif017() {
        /**
         * @tc.steps: step1.get image source.
         * @tc.expected: step1.image source not null.
         */
        ImageSource.SourceOptions srcOpts = new ImageSource.SourceOptions();
        srcOpts.formatHint = "image/jpeg";
        String pathName = "/sdcard/multimedia/image/test.jpg";
        imageSource = ImageSource.create(pathName, srcOpts);
        assertNotNull(imageSource);

        /**
         * @tc.steps: step2.get image exif altitude info.
         * @tc.expected: step2.check image exif info ok.
         */
        double defaultValue = 1.0;
        assertEquals(defaultValue, ExifUtils.getAltitude(imageSource, defaultValue), 1e-6);
    }

    /**
     * @tc.name: testImageSourceExif018
     * @tc.desc: get image exif info with byte array image source.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testImageSourceExif018() {
        /**
         * @tc.steps: step1.get image source from byte array.
         * @tc.expected: step1.image source not null.
         */
        ImageSource.SourceOptions srcOpts = new ImageSource.SourceOptions();
        srcOpts.formatHint = "image/jpeg";
        String pathName = "/sdcard/multimedia/image/test_exif.jpg";
        Path fileLocation = Paths.get(pathName);
        byte[] data = null;
        try {
            data = Files.readAllBytes(fileLocation);
        } catch (IOException e) {
            LOGGER.error("file read exception");
            fail("file read exception");
        }
        imageSource = ImageSource.create(data, null);
        assertNotNull(imageSource);

        /**
         * @tc.steps: step2.get image exif info.
         * @tc.expected: step2.check image exif info not null and ok.
         */
        byte[] thumbnailData = imageSource.getImageThumbnailBytes();
        assertNotNull(thumbnailData);
        assertEquals(ImageFormat.JPEG, imageSource.getThumbnailFormat());

        /**
         * @tc.steps: step3.get image thumbnail pixel map.
         * @tc.expected: step3.check thumbnail pixel map ok.
         */
        ImageSource.DecodingOptions decodingOpts = new ImageSource.DecodingOptions();
        PixelMap thumbnailPixelmap = imageSource.createThumbnailPixelmap(decodingOpts, false);
        assertNotNull(thumbnailPixelmap);
        PixelMap thumbnailPixelmap2 = imageSource.createThumbnailPixelmap(decodingOpts, false);
        assertNotNull(thumbnailPixelmap2);
        assertEquals(160, thumbnailPixelmap.getImageInfo().size.width);
        assertEquals(120, thumbnailPixelmap.getImageInfo().size.height);
        assertEquals(160, imageSource.getThumbnailInfo().size.width);
        assertEquals(120, imageSource.getThumbnailInfo().size.height);

        /**
         * @tc.steps: step4.get image property info.
         * @tc.expected: step4.check image property info ok.
         */
        assertEquals("6", imageSource.getImagePropertyString(PropertyKey.Exif.COMPRESSION));
        assertEquals(5952, imageSource.getImagePropertyInt(PropertyKey.Exif.PIXEL_X_DIMENSION, 0));
    }

    /**
     * @tc.name: testImageSourceExif019
     * @tc.desc: get image exif thumbnail format from image without exif info.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testImageSourceExif019() {
        /**
         * @tc.steps: step1.get image source.
         * @tc.expected: step1.image source not null.
         */
        ImageSource.SourceOptions srcOpts = new ImageSource.SourceOptions();
        srcOpts.formatHint = "image/jpeg";
        String pathName = "/sdcard/multimedia/image/test.jpg";
        imageSource = ImageSource.create(pathName, srcOpts);
        assertNotNull(imageSource);

        /**
         * @tc.steps: step2.get image exif thumbnail format info.
         * @tc.expected: step2.check image exif thumbnail format info exception.
         */
        assertThrows("should throw UnsupportedOperationException", UnsupportedOperationException.class, () -> {
            imageSource.getThumbnailFormat();
        });
    }

    /**
     * @tc.name: testImageSourceExif020
     * @tc.desc: get image exif info with file image source.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testImageSourceExif020() {
        /**
         * @tc.steps: step1.get image source from file object.
         * @tc.expected: step1.image source not null.
         */
        ImageSource.SourceOptions srcOpts = new ImageSource.SourceOptions();
        srcOpts.formatHint = "image/jpeg";
        String pathName = "/sdcard/multimedia/image/test_exif.jpg";
        File file = new File(pathName);
        imageSource = ImageSource.create(file, null);
        assertNotNull(imageSource);

        /**
         * @tc.steps: step2.get image exif info.
         * @tc.expected: step2.check image exif info not null and ok.
         */
        byte[] thumbnailData = imageSource.getImageThumbnailBytes();
        assertNotNull(thumbnailData);
        assertEquals(ImageFormat.JPEG, imageSource.getThumbnailFormat());

        /**
         * @tc.steps: step3.get image thumbnail pixel map.
         * @tc.expected: step3.check thumbnail pixel map ok.
         */
        ImageSource.DecodingOptions decodingOpts = new ImageSource.DecodingOptions();
        PixelMap thumbnailPixelmap = imageSource.createThumbnailPixelmap(decodingOpts, false);
        assertNotNull(thumbnailPixelmap);
        PixelMap thumbnailPixelmap2 = imageSource.createThumbnailPixelmap(decodingOpts, false);
        assertNotNull(thumbnailPixelmap2);
        assertEquals(160, thumbnailPixelmap.getImageInfo().size.width);
        assertEquals(120, thumbnailPixelmap.getImageInfo().size.height);
        assertEquals(160, imageSource.getThumbnailInfo().size.width);
        assertEquals(120, imageSource.getThumbnailInfo().size.height);

        /**
         * @tc.steps: step4.get image property info.
         * @tc.expected: step4.check image property info ok.
         */
        assertEquals("6", imageSource.getImagePropertyString(PropertyKey.Exif.COMPRESSION));
        assertEquals(5952, imageSource.getImagePropertyInt(PropertyKey.Exif.PIXEL_X_DIMENSION, 0));
    }

    /**
     * test get inner thumbnail.
     *
     * @tc.name: testGetInnerJpegThumbnail010
     * @tc.desc: test get inner thumbnail.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testGetInnerJpegThumbnail010() {
        ImageSource.SourceOptions srcOpts = new ImageSource.SourceOptions();
        srcOpts.formatHint = "image/jpeg";
        String pathName = "/sdcard/multimedia/image/test_exif.jpg";
        File file = new File(pathName);
        imageSource = ImageSource.create(file, srcOpts);
        assertNotNull(imageSource);

        byte[] thumbnailData = imageSource.getInnerJpegThumbnail();
        assertNotNull(thumbnailData);
        imageSource.release();
    }

    /**
     * test get inner thumbnail from jpeg after released.
     *
     * @tc.name: testGetInnerJpegThumbnail020
     * @tc.desc: test get inner thumbnail from jpeg after released.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testGetInnerJpegThumbnail020() {
        ImageSource.SourceOptions srcOpts = new ImageSource.SourceOptions();
        srcOpts.formatHint = "image/jpeg";
        String pathName = "/sdcard/multimedia/image/test_exif.jpg";
        File file = new File(pathName);
        imageSource = ImageSource.create(file, srcOpts);
        assertNotNull(imageSource);

        imageSource.release();

        assertThrows("should throw IllegalStateException", IllegalStateException.class, () -> {
            imageSource.getInnerJpegThumbnail();
        });
    }

    /**
     * test get inner thumbnail from jpeg without thumbnail.
     *
     * @tc.name: testGetInnerJpegThumbnail030
     * @tc.desc: test get inner thumbnail from jpeg without thumbnail.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testGetInnerJpegThumbnail030() {
        ImageSource.SourceOptions srcOpts = new ImageSource.SourceOptions();
        srcOpts.formatHint = "image/jpeg";
        String pathName = "/sdcard/multimedia/image/test.jpg";
        File file = new File(pathName);
        imageSource = ImageSource.create(file, srcOpts);
        assertNotNull(imageSource);

        byte[] thumbnailData = imageSource.getInnerJpegThumbnail();
        assertEquals(thumbnailData, null);
        imageSource.release();
    }

    /**
     * test get inner thumbnail from jpeg with exception.
     *
     * @tc.name: testGetInnerJpegThumbnail040
     * @tc.desc: test get inner thumbnail from jpeg with exception.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testGetInnerJpegThumbnail040() {
        ImageSource.SourceOptions srcOpts = new ImageSource.SourceOptions();
        srcOpts.formatHint = "image/jpeg";
        String pathName = "/sdcard/multimedia/image/test_exif.jpg";
        InputStream inputStream = null;
        try {
            inputStream = new FileInputStream(pathName);
            imageSource = ImageSource.create(inputStream, srcOpts);
            assertNotNull(imageSource);
        } catch (FileNotFoundException | IllegalArgumentException e) {
            fail("file read exception");
        } finally {
            try {
                inputStream.close();
            } catch (IOException e) {
                LOGGER.error("fileStream close IOException");
            }
        }

        assertThrows("should throw IllegalStateException", IllegalStateException.class, () -> {
            imageSource.getInnerJpegThumbnail();
        });
        imageSource.release();
    }

    /**
     * test get inner thumbnail from jpeg and assert size of thumbnail.
     *
     * @tc.name: testGetInnerJpegThumbnail050
     * @tc.desc: test get and compare inner thumbnail size from jpeg.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testGetInnerJpegThumbnail050() {
        ImageSource.SourceOptions srcOpts = new ImageSource.SourceOptions();
        srcOpts.formatHint = "image/jpeg";
        String pathName = "/sdcard/multimedia/image/test_exif.jpg";
        File file = new File(pathName);
        imageSource = ImageSource.create(file, srcOpts);
        assertNotNull(imageSource);

        byte[] thumbnailData = imageSource.getInnerJpegThumbnail();
        ImageSource newImageSource = ImageSource.create(thumbnailData, null);
        PixelMap craetedThumb = newImageSource.createPixelmap(null);

        ImageSource.DecodingOptions decodingOpts = new ImageSource.DecodingOptions();
        PixelMap innerThumb = imageSource.createThumbnailPixelmap(decodingOpts, false);

        assertEquals(innerThumb.getImageInfo().size.width, craetedThumb.getImageInfo().size.width);
        assertEquals(innerThumb.getImageInfo().size.height, craetedThumb.getImageInfo().size.height);
        imageSource.release();
        newImageSource.release();
    }

    /**
     * test get inner thumbnail from jpeg and assert created thumbnail.
     *
     * @tc.name: testGetInnerJpegThumbnail060
     * @tc.desc: test get inner thumbnail from jpeg and assert size of thumbnail.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testGetInnerJpegThumbnail060() {
        ImageSource.SourceOptions srcOpts = new ImageSource.SourceOptions();
        srcOpts.formatHint = "image/jpeg";
        String pathName = "/sdcard/multimedia/image/test_hw.jpg";
        File file = new File(pathName);
        imageSource = ImageSource.create(file, srcOpts);
        assertNotNull(imageSource);
        byte[] thumbnailData = imageSource.getInnerJpegThumbnail();
        assertNotNull(thumbnailData);

        int hightFromTag = imageSource.getImagePropertyInt(PropertyKey.Exif.THUMBNAIL_IMAGE_LENGTH, 160);
        int widthFromTag = imageSource.getImagePropertyInt(PropertyKey.Exif.THUMBNAIL_IMAGE_WIDTH, 120);

        ImageSource.DecodingOptions decodingOpts = new ImageSource.DecodingOptions();
        ImageSource newImageSource = ImageSource.create(thumbnailData, null);
        PixelMap thumbnailPixelmap = newImageSource.createPixelmap(null);

        assertEquals(hightFromTag, thumbnailPixelmap.getImageInfo().size.height);
        assertEquals(widthFromTag, thumbnailPixelmap.getImageInfo().size.width);
        imageSource.release();
        newImageSource.release();
    }

    /**
     * test get inner thumbnail scope.
     *
     * @tc.name: testGetInnerThumbnailScope010
     * @tc.desc: test get inner thumbnail scope.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testGetInnerThumbnailScope010() {
        ImageSource.SourceOptions srcOpts = new ImageSource.SourceOptions();
        srcOpts.formatHint = "image/jpeg";
        String pathName = "/sdcard/multimedia/image/test_exif.jpg";
        File file = new File(pathName);
        imageSource = ImageSource.create(file, srcOpts);
        assertNotNull(imageSource);

        byte[] thumbnailData = imageSource.getInnerJpegThumbnail();
        assertNotNull(thumbnailData);

        long[] range = imageSource.getInnerThumbnailScope();
        assertNotNull(range);
        imageSource.release();
    }

    /**
     * test get inner thumbnail scope and assert offset and length in scope.
     *
     * @tc.name: testGetInnerThumbnailScope020
     * @tc.desc: test get inner thumbnail scope and assert offset and length in scope.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testGetInnerThumbnailScope020() {
        ImageSource.SourceOptions srcOpts = new ImageSource.SourceOptions();
        srcOpts.formatHint = "image/jpeg";
        String pathName = "/sdcard/multimedia/image/test_exif.jpg";
        File file = new File(pathName);
        imageSource = ImageSource.create(file, srcOpts);
        assertNotNull(imageSource);

        long[] range = imageSource.getInnerThumbnailScope();
        assertEquals(5448L, range[0]);
        assertEquals(18601L, range[1]);
        imageSource.release();
    }

    /**
     * test get inner thumbnail scope.
     *
     * @tc.name: testGetInnerThumbnailScope030
     * @tc.desc: test get inner thumbnail scope from a jpeg without thumbnail.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testGetInnerThumbnailScope030() {
        ImageSource.SourceOptions srcOpts = new ImageSource.SourceOptions();
        srcOpts.formatHint = "image/jpeg";
        String pathName = "/sdcard/multimedia/image/test.jpg";
        File file = new File(pathName);
        imageSource = ImageSource.create(file, srcOpts);
        assertNotNull(imageSource);

        byte[] thumbnailData = imageSource.getInnerJpegThumbnail();
        assertEquals(thumbnailData, null);

        long[] range = imageSource.getInnerThumbnailScope();
        assertEquals(range, null);
        imageSource.release();
    }

    /**
     * test get inner thumbnail scope after source released.
     *
     * @tc.name: testGetInnerThumbnailScope040
     * @tc.desc: test get inner thumbnail scope after source released.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testGetInnerThumbnailScope040() {
        ImageSource.SourceOptions srcOpts = new ImageSource.SourceOptions();
        srcOpts.formatHint = "image/jpeg";
        String pathName = "/sdcard/multimedia/image/test_exif.jpg";
        File file = new File(pathName);
        imageSource = ImageSource.create(file, srcOpts);
        assertNotNull(imageSource);

        imageSource.release();

        assertThrows("should throw IllegalStateException", IllegalStateException.class, () -> {
            imageSource.getInnerThumbnailScope();
        });
    }

    /**
     * test get inner thumbnail scope  with exception.
     *
     * @tc.name: testGetInnerThumbnailScope050
     * @tc.desc: test get inner thumbnail scope  with exception.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testGetInnerThumbnailScope050() {
        ImageSource.SourceOptions srcOpts = new ImageSource.SourceOptions();
        srcOpts.formatHint = "image/jpeg";
        String pathName = "/sdcard/multimedia/image/test_exif.jpg";
        InputStream inputStream = null;
        try {
            inputStream = new FileInputStream(pathName);
            imageSource = ImageSource.create(inputStream, srcOpts);
            assertNotNull(imageSource);
        } catch (FileNotFoundException | IllegalArgumentException e) {
            fail("file read exception");
        } finally {
            try {
                inputStream.close();
            } catch (IOException e) {
                LOGGER.error("fileStream close IOException");
            }
        }

        assertThrows("should throw IllegalStateException", IllegalStateException.class, () -> {
            imageSource.getInnerThumbnailScope();
        });
        imageSource.release();
    }

    /**
     * @tc.name: testImageSourceGif001
     * @tc.desc: get image property loop count.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testImageSourceGif001() {
        /**
         * @tc.steps: step1.get image source.
         * @tc.expected: step1.image source not null.
         */
        ImageSource.SourceOptions srcOpts = new ImageSource.SourceOptions();
        srcOpts.formatHint = "image/gif";
        String pathName = "/sdcard/multimedia/image/moving_test.gif";
        imageSource = ImageSource.create(pathName, srcOpts);
        assertNotNull(imageSource);
        PropertyKey.GIF gif = new PropertyKey.GIF();

        /**
         * @tc.steps: step2.get image property loop count.
         * @tc.expected: step2.check image loop count ok.
         */
        int defaultValue = 10;
        assertEquals(0, imageSource.getImagePropertyInt(-1, PropertyKey.GIF.LOOP_COUNT, defaultValue));
        assertEquals(0, imageSource.getImagePropertyInt(0, PropertyKey.GIF.LOOP_COUNT, defaultValue));
        assertEquals(0, imageSource.getImagePropertyInt(1, PropertyKey.GIF.LOOP_COUNT, defaultValue));
        assertEquals(0, imageSource.getImagePropertyInt(2, PropertyKey.GIF.LOOP_COUNT, defaultValue));
        assertEquals(0, imageSource.getImagePropertyInt(3, PropertyKey.GIF.LOOP_COUNT, defaultValue));

        /**
         * @tc.steps: step2.get image property delay time.
         * @tc.expected: step2.check image delay time ok.
         */
        assertNotEquals(70, imageSource.getImagePropertyInt(-1, PropertyKey.GIF.DELAY_TIME, defaultValue));
        assertEquals(70, imageSource.getImagePropertyInt(0, PropertyKey.GIF.DELAY_TIME, defaultValue));
        assertEquals(70, imageSource.getImagePropertyInt(1, PropertyKey.GIF.DELAY_TIME, defaultValue));
        assertEquals(70, imageSource.getImagePropertyInt(2, PropertyKey.GIF.DELAY_TIME, defaultValue));
        assertNotEquals(70, imageSource.getImagePropertyInt(3, PropertyKey.GIF.DELAY_TIME, defaultValue));
    }

    /**
     * @tc.name: testImageSourceGif002
     * @tc.desc: get image property loop count.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testImageSourceGif002() {
        /**
         * @tc.steps: step1.get image source.
         * @tc.expected: step1.image source not null.
         */
        ImageSource.SourceOptions srcOpts = new ImageSource.SourceOptions();
        srcOpts.formatHint = "image/gif";
        String pathName = "/sdcard/multimedia/image/test.gif";
        imageSource = ImageSource.create(pathName, srcOpts);
        assertNotNull(imageSource);

        /**
         * @tc.steps: step2.get image property loop count.
         * @tc.expected: step2.check image loop count ok.
         */
        int defaultValue = 10;
        assertEquals(0, imageSource.getImagePropertyInt(PropertyKey.GIF.LOOP_COUNT, defaultValue));

        /**
         * @tc.steps: step2.get image property delay time.
         * @tc.expected: step2.check image delay time ok.
         */
        assertEquals(70, imageSource.getImagePropertyInt(PropertyKey.GIF.DELAY_TIME, defaultValue));

        ImageSource.DecodingOptions decodingOpts = new ImageSource.DecodingOptions();
        decodingOpts.desiredPixelFormat = PixelFormat.RGB_565;
        pixelMap = imageSource.createPixelmap(decodingOpts);
        PixelFormat pixelFormat = pixelMap.getImageInfo().pixelFormat;
        assertEquals(PixelFormat.RGB_565, pixelFormat);
    }

    /**
     * @tc.name: testImageSourceWebp001
     * @tc.desc: create image source from file path.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testImageSourceWebp001() {
        /**
         * @tc.steps: step1.get image source form path.
         * @tc.expected: step1.get image source form path not null
         */
        ImageSource.SourceOptions srcOpts = new ImageSource.SourceOptions();
        String pathName = "/sdcard/multimedia/image/test.webp";
        imageSource = ImageSource.create(pathName, srcOpts);
        assertNotNull(imageSource);
        ImageSource.SourceInfo sourceInfo = imageSource.getSourceInfo();
        assertNotNull(sourceInfo);
        assertEquals(1, sourceInfo.topLevelImageNum);
        assertEquals("image/webp", sourceInfo.encodedFormat);

        /**
         * @tc.steps: step2.create pixel map.
         * @tc.expected: step2.create pixel map not null and check information ok
         */
        ImageSource.DecodingOptions decodingOpts = new ImageSource.DecodingOptions();
        decodingOpts.desiredSize = new Size(0, 0);
        decodingOpts.desiredRegion = new Rect(0, 0, 0, 0);
        pixelMap = imageSource.createPixelmap(decodingOpts);
        assertNotNull(pixelMap);
        assertEquals(286, pixelMap.getImageInfo().size.width);
        assertEquals(221, pixelMap.getImageInfo().size.height);
    }

    /**
     * @tc.name: testImageSourceWebp002
     * @tc.desc: create image source from byte array.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testImageSourceWebp002() {
        /**
         * @tc.steps: step1.get image source byte array.
         * @tc.expected: step1.get image source form byte array not null
         */
        ImageSource.SourceOptions srcOpts = new ImageSource.SourceOptions();
        srcOpts.formatHint = "image/webp";
        String pathName = "/sdcard/multimedia/image/test.webp";
        Path fileLocation = Paths.get(pathName);
        byte[] data = null;
        try {
            data = Files.readAllBytes(fileLocation);
        } catch (IOException e) {
            LOGGER.error("file read exception");
            fail("file read exception");
        }
        imageSource = ImageSource.create(data, srcOpts);
        assertNotNull(imageSource);

        /**
         * @tc.steps: step2.create pixel map.
         * @tc.expected: step2.create pixel map not null and check information ok
         */
        ImageSource.DecodingOptions decodingOpts = new ImageSource.DecodingOptions();
        decodingOpts.desiredSize = new Size(0, 0);
        decodingOpts.desiredRegion = new Rect(0, 0, 0, 0);
        pixelMap = imageSource.createPixelmap(0, decodingOpts);
        assertNotNull(pixelMap);
        assertEquals(286, pixelMap.getImageInfo().size.width);
        assertEquals(221, pixelMap.getImageInfo().size.height);
    }

    /**
     * @tc.name: testImageSourceWebp003
     * @tc.desc: create image source from byte array with offset and length.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testImageSourceWebp003() {
        /**
         * @tc.steps: step1.get image source byte array with offset and length.
         * @tc.expected: step1.get image source form byte array with offset and length not null
         */
        ImageSource.SourceOptions srcOpts = new ImageSource.SourceOptions();
        String pathName = "/sdcard/multimedia/image/test.webp";
        Path fileLocation = Paths.get(pathName);
        byte[] data = null;
        try {
            data = Files.readAllBytes(fileLocation);
        } catch (IOException e) {
            LOGGER.error("file read exception");
            fail("file read exception");
        }
        imageSource = ImageSource.create(data, 0, data.length, srcOpts);
        assertNotNull(imageSource);

        /**
         * @tc.steps: step2.create pixel map.
         * @tc.expected: step2.create pixel map not null and check information ok
         */
        ImageSource.DecodingOptions decodingOpts = new ImageSource.DecodingOptions();
        decodingOpts.desiredSize = new Size(0, 0);
        decodingOpts.desiredRegion = new Rect(0, 0, 0, 0);
        pixelMap = imageSource.createPixelmap(decodingOpts);
        assertNotNull(pixelMap);
        assertEquals(286, pixelMap.getImageInfo().size.width);
        assertEquals(221, pixelMap.getImageInfo().size.height);
    }

    /**
     * @tc.name: testImageSourceWebp004
     * @tc.desc: create image source from File.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testImageSourceWebp004() {
        /**
         * @tc.steps: step1.get image source File.
         * @tc.expected: step1.get image source form File not null
         */
        ImageSource.SourceOptions srcOpts = new ImageSource.SourceOptions();
        String pathName = "/sdcard/multimedia/image/test.webp";
        File file = new File(pathName);
        imageSource = ImageSource.create(file, srcOpts);
        assertNotNull(imageSource);

        /**
         * @tc.steps: step2.create pixel map.
         * @tc.expected: step2.create pixel map not null and check information ok
         */
        ImageSource.DecodingOptions decodingOpts = new ImageSource.DecodingOptions();
        decodingOpts.desiredSize = new Size(1000, 500);
        decodingOpts.desiredRegion = new Rect(10, 10, 187, 152);
        decodingOpts.rotateDegrees = 90;
        decodingOpts.desiredPixelFormat = PixelFormat.BGRA_8888;
        pixelMap = imageSource.createPixelmap(decodingOpts);
        assertNotNull(pixelMap);
        assertEquals(1000, pixelMap.getImageInfo().size.width);
        assertEquals(500, pixelMap.getImageInfo().size.height);
    }

    /**
     * @tc.name: testImageSourceWebp005
     * @tc.desc: create image source from InputStream.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testImageSourceWebp005() {
        /**
         * @tc.steps: step1.get image source InputStream.
         * @tc.expected: step1.get image source form InputStream not null
         */
        ImageSource.SourceOptions srcOpts = new ImageSource.SourceOptions();
        srcOpts.formatHint = "image/webp";
        String pathName = "/sdcard/multimedia/image/test.webp";
        try (InputStream inputStream = new FileInputStream(pathName)) {
            imageSource = ImageSource.create(inputStream, srcOpts);
            assertNotNull(imageSource);
        } catch (FileNotFoundException fe) {
            LOGGER.error("createImageSource from InputStream, file not found");
        } catch (IOException ioe) {
            LOGGER.error("file read io exception");
        }

        /**
         * @tc.steps: step2.create pixel map.
         * @tc.expected: step2.create pixel map not null and check information ok
         */
        ImageSource.DecodingOptions decodingOpts = new ImageSource.DecodingOptions();
        decodingOpts.desiredSize = new Size(0, 0);
        decodingOpts.desiredRegion = new Rect(0, 0, 0, 0);
        decodingOpts.rotateDegrees = 180;
        pixelMap = imageSource.createPixelmap(decodingOpts);
        assertNotNull(pixelMap);
        assertEquals(286, pixelMap.getImageInfo().size.width);
        assertEquals(221, pixelMap.getImageInfo().size.height);
    }

    /**
     * @tc.name: testImageSourceWebp006
     * @tc.desc: create incremental image source.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testImageSourceWebp006() {
        /**
         * @tc.steps: step1.get incremental image source.
         * @tc.expected: step1.get incremental image source not null
         */
        ImageSource.SourceOptions srcOpts = new ImageSource.SourceOptions();
        ImageSource.IncrementalSourceOptions incOpts = new ImageSource.IncrementalSourceOptions();
        incOpts.opts = srcOpts;
        incOpts.mode = ImageSource.UpdateMode.INCREMENTAL_DATA;
        imageSource = ImageSource.createIncrementalSource(incOpts);
        assertNotNull(imageSource);

        /**
         * @tc.steps: step2.create pixel map.
         * @tc.expected: step2.create pixel map not null and check information ok
         */
        ImageSource.DecodingOptions decodingOpts = new ImageSource.DecodingOptions();
        decodingOpts.desiredSize = new Size(100, 60);
        decodingOpts.desiredRegion = new Rect(10, 10, 150, 120);
        decodingOpts.rotateDegrees = 90;
        decodingOpts.desiredPixelFormat = PixelFormat.BGRA_8888;
        decodingOpts.allowPartialImage = true;

        String pathName = "/sdcard/multimedia/image/test.webp";
        final int size = 1024;
        byte[] data = new byte[size];
        File file = new File(pathName);
        try (FileInputStream fileInputStream = new FileInputStream(file)) {
            int bytes = 0;
            while (bytes != -1) {
                bytes = fileInputStream.read(data);
                if (bytes == -1) {
                    break;
                }
                boolean isFinal = (bytes != size);
                imageSource.updateData(data, 0, bytes, isFinal);
                pixelMap = imageSource.createPixelmap(decodingOpts);
                assertNotNull(pixelMap);
                if (pixelMap.getImageInfo().size.width == 0) {
                    continue;
                }
                if (isFinal) {
                    assertEquals(100, pixelMap.getImageInfo().size.width);
                    assertEquals(60, pixelMap.getImageInfo().size.height);
                } else {
                    assertEquals(286, pixelMap.getImageInfo().size.width);
                    assertEquals(221, pixelMap.getImageInfo().size.height);
                }
                pixelMap.release();
            }
        } catch (IOException e) {
            fail("file read exception");
        }
    }

    /**
     * @tc.name: testImageSourceWebp007
     * @tc.desc: create image source from file path.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testImageSourceWebp007() {
        /**
         * @tc.steps: step1.get image source form path.
         * @tc.expected: step1.get image source form path not null
         */
        ImageSource.SourceOptions srcOpts = new ImageSource.SourceOptions();
        String pathName = "/sdcard/multimedia/image/test.webp";
        imageSource = ImageSource.create(pathName, srcOpts);
        assertNotNull(imageSource);
        ImageSource.SourceInfo sourceInfo = imageSource.getSourceInfo();
        assertNotNull(sourceInfo);
        assertEquals(1, sourceInfo.topLevelImageNum);
        assertEquals("image/webp", sourceInfo.encodedFormat);

        /**
         * @tc.steps: step2.create pixel map.
         * @tc.expected: step2.create pixel map not null and check information ok
         */
        ImageSource.DecodingOptions decodingOpts = new ImageSource.DecodingOptions();
        decodingOpts.desiredSize = new Size(0, 0);
        decodingOpts.desiredRegion = new Rect(0, 0, 0, 0);
        pixelMap = imageSource.createPixelmap(decodingOpts);
        assertNotNull(pixelMap);
        assertEquals(286, pixelMap.getImageInfo().size.width);
        assertEquals(221, pixelMap.getImageInfo().size.height);

        PixelMap pixelMap1 = imageSource.createPixelmap(decodingOpts);
        assertNotNull(pixelMap1);
        assertEquals(286, pixelMap1.getImageInfo().size.width);
        assertEquals(221, pixelMap1.getImageInfo().size.height);
    }

    private static final class RawImage {
        /**
        * file path.
        */
        public final String path;
        /**
        * file mime.
        */
        public final String mime;
        /**
        * image width.
        */
        public final int width;
        /**
        * image height.
        */
        public final int height;

        RawImage(String pathName, String mime, int width, int height) {
            this.path    = pathName;
            this.mime    = mime;
            this.width    = width;
            this.height   = height;
        }
    }

    /**
     * @tc.name: testImageSourceRaw001
     * @tc.desc: decode all raw img from file path.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testImageSourceRaw001() {
        RawImage[] rawImgs = new RawImage[]{
            new RawImage("/sdcard/multimedia/image/test.dng", "image/x-adobe-dng", 5976, 3992),
            new RawImage("/sdcard/multimedia/image/test.arw", "image/x-sony-arw", 1920, 1080),
            new RawImage("/sdcard/multimedia/image/test.cr2", "image/x-canon-cr2", 5616, 3744),
            new RawImage("/sdcard/multimedia/image/test.nrw", "image/x-nikon-nrw", 4000, 3000),
            new RawImage("/sdcard/multimedia/image/test.pef", "image/x-pentax-pef", 4928, 3264),
            new RawImage("/sdcard/multimedia/image/test.raf", "image/x-fuji-raf", 2048, 1536),
            new RawImage("/sdcard/multimedia/image/test.rw2", "image/x-panasonic-rw2", 1920, 1440),
        };

        for (RawImage img : rawImgs) {
            LOGGER.info("createImageSource file :%{public}s", img.path);
            ImageSource.SourceOptions srcOpts = new ImageSource.SourceOptions();
            imageSource = ImageSource.create(img.path, srcOpts);
            assertNotNull(imageSource);
            ImageSource.SourceInfo sourceInfo = imageSource.getSourceInfo();
            assertNotNull(sourceInfo);
            if (img.path.endsWith("dng")) {
                assertEquals("image/x-adobe-dng", sourceInfo.encodedFormat);
            } else {
                assertEquals("image/jpeg", sourceInfo.encodedFormat);
            }

            ImageSource.DecodingOptions decodingOpts = new ImageSource.DecodingOptions();
            decodingOpts.desiredPixelFormat = PixelFormat.ARGB_8888;
            pixelMap = imageSource.createPixelmap(decodingOpts);
            assertNotNull(pixelMap);
            assertEquals(img.width, pixelMap.getImageInfo().size.width);
            assertEquals(img.height, pixelMap.getImageInfo().size.height);
            PixelFormat pixelFormat = pixelMap.getImageInfo().pixelFormat;
            assertEquals(PixelFormat.ARGB_8888, pixelFormat);
        }
    }

    /**
     * @tc.name: testImageSourceRaw002
     * @tc.desc: create image source by correct raw file path and wrong format hit
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testImageSourceRaw002() {
        /**
         * @tc.steps: step1.get image source form path.
         * @tc.expected: step1.get image source form path not null
         */
        ImageSource.SourceOptions srcOpts = new ImageSource.SourceOptions();
        String pathName = "/sdcard/multimedia/image/test.dng";
        imageSource = ImageSource.create(pathName, srcOpts);
        srcOpts.formatHint = "image/png";
        assertNotNull(imageSource);
        ImageSource.SourceInfo sourceInfo = imageSource.getSourceInfo();
        assertNotNull(sourceInfo);
        assertEquals("image/x-adobe-dng", sourceInfo.encodedFormat);
    }

    /**
     * @tc.name: testImageSourceRaw003
     * @tc.desc: create image source from file path with wrong data
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testImageSourceRaw003() {
        /**
         * @tc.steps: step1.get image source form path.
         * @tc.expected: step1.get image source form path not null
         */
        String pathName = "/sdcard/multimedia/image/test.dng";
        File file = new File(pathName);
        FileChannel channel = null;
        FileInputStream fileStream = null;
        ByteBuffer byteBuffer = null;
        try {
            fileStream = new FileInputStream(file);
            channel = fileStream.getChannel();
            byteBuffer = ByteBuffer.allocate((int) channel.size());
            LOGGER.info("createImageSource file size:%{public}d", channel.size());
            channel.read(byteBuffer);
        } catch (IOException e) {
            LOGGER.error("read file IOException");
        } finally {
            try {
                channel.close();
            } catch (IOException e) {
                LOGGER.error("channel close IOException");
            }
            try {
                fileStream.close();
            } catch (IOException e) {
                LOGGER.error("fileStream close IOException");
            }
        }

        byteBuffer.put(2, (byte)12);
        byteBuffer.put(3, (byte)34);
        ImageSource.SourceOptions srcOpts = new ImageSource.SourceOptions();
        imageSource = ImageSource.create(byteBuffer, srcOpts);
        assertNotNull(imageSource);
        ImageSource.SourceInfo sourceInfo = imageSource.getSourceInfo();
        assertNotNull(sourceInfo);
        assertEquals("image/x-raw", sourceInfo.encodedFormat);

        /**
         * @tc.steps: step2.create pixel map.
         * @tc.expected: step2.create pixel map should fail
         */
        ImageSource.DecodingOptions decodingOpts = new ImageSource.DecodingOptions();
        assertThrows("should throw SourceDataMalformedException", SourceDataMalformedException.class, () -> {
            imageSource.createPixelmap(decodingOpts);
        });
    }

    /**
     * @tc.name: testImageSourceRaw004
     * @tc.desc: decode all raw img from file path multiple times
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testImageSourceRaw004() {
        /**
         * @tc.steps: step1.get image source form path.
         * @tc.expected: step1.get image source form path not null
         */
        ImageSource.SourceOptions srcOpts = new ImageSource.SourceOptions();
        String pathName = "/sdcard/multimedia/image/test.dng";
        imageSource = ImageSource.create(pathName, srcOpts);
        assertNotNull(imageSource);
        ImageSource.SourceInfo sourceInfo = imageSource.getSourceInfo();
        assertNotNull(sourceInfo);
        assertEquals("image/x-adobe-dng", sourceInfo.encodedFormat);

        /**
         * @tc.steps: step2.create pixel map.
         * @tc.expected: step2.create pixel map not null and check information ok
         */
        ImageSource.DecodingOptions decodingOpts = new ImageSource.DecodingOptions();
        pixelMap = imageSource.createPixelmap(decodingOpts);
        assertNotNull(pixelMap);
        assertEquals(5976, pixelMap.getImageInfo().size.width);
        assertEquals(3992, pixelMap.getImageInfo().size.height);

        PixelMap pixelMap1 = imageSource.createPixelmap(decodingOpts);
        assertNotNull(pixelMap1);

        PixelMap pixelMap2 = imageSource.createPixelmap(decodingOpts);
        assertNotNull(pixelMap2);
    }

    /**
     * @tc.name: testImageSourceWbmp001
     * @tc.desc: create image source by correct wbmp file
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testImageSourceWbmp001() {
        /**
         * @tc.steps: step1.get image source form path.
         * @tc.expected: step1.get image source form path not null
         */
        ImageSource.SourceOptions srcOpts = new ImageSource.SourceOptions();
        String pathName = "/sdcard/multimedia/image/test.wbmp";
        imageSource = ImageSource.create(pathName, srcOpts);
        assertNotNull(imageSource);
        ImageSource.SourceInfo sourceInfo = imageSource.getSourceInfo();
        assertNotNull(sourceInfo);
        assertEquals("image/vnd.wap.wbmp", sourceInfo.encodedFormat);

        /**
         * @tc.steps: step2.create pixel map.
         * @tc.expected: step2.create pixel map not null and check information ok
         */
        ImageSource.DecodingOptions decodingOpts = new ImageSource.DecodingOptions();
        pixelMap = imageSource.createPixelmap(decodingOpts);
        assertNotNull(pixelMap);
        assertEquals(472, pixelMap.getImageInfo().size.width);
        assertEquals(75, pixelMap.getImageInfo().size.height);
    }

    /**
     * @tc.name: testImageSourceWbmp002
     * @tc.desc: create image source by wrong hint
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testImageSourceWbmp002() {
        /**
         * @tc.steps: step1.get image source form path.
         * @tc.expected: step1.get image source form path not null
         */
        ImageSource.SourceOptions srcOpts = new ImageSource.SourceOptions();
        String pathName = "/sdcard/multimedia/image/test.wbmp";
        srcOpts.formatHint = "image/png";
        imageSource = ImageSource.create(pathName, srcOpts);
        assertNotNull(imageSource);
        ImageSource.SourceInfo sourceInfo = imageSource.getSourceInfo();
        assertNotNull(sourceInfo);
        assertEquals("image/vnd.wap.wbmp", sourceInfo.encodedFormat);

        /**
         * @tc.steps: step2.create pixel map.
         * @tc.expected: step2.create pixel map not null and check information ok
         */
        ImageSource.DecodingOptions decodingOpts = new ImageSource.DecodingOptions();
        pixelMap = imageSource.createPixelmap(decodingOpts);
        assertNotNull(pixelMap);
        assertEquals(472, pixelMap.getImageInfo().size.width);
        assertEquals(75, pixelMap.getImageInfo().size.height);
    }

    /**
     * @tc.name: testImageSourceWbmp003
     * @tc.desc: create image source by wrong wbmp file
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testImageSourceWbmp003() {
        /**
         * @tc.steps: step1.get image source form path.
         * @tc.expected: step1.get image source form path not null
         */
        String pathName = "/sdcard/multimedia/image/test.wbmp";
        File file = new File(pathName);
        FileChannel channel = null;
        FileInputStream fileStream = null;
        ByteBuffer byteBuffer = null;
        try {
            fileStream = new FileInputStream(file);
            channel = fileStream.getChannel();
            byteBuffer = ByteBuffer.allocate((int) channel.size());
            LOGGER.info("createImageSource file size:%{public}d", channel.size());
            channel.read(byteBuffer);
        } catch (IOException e) {
            LOGGER.error("read file IOException");
        } finally {
            try {
                channel.close();
            } catch (IOException e) {
                LOGGER.error("channel close IOException");
            }
            try {
                fileStream.close();
            } catch (IOException e) {
                LOGGER.error("fileStream close IOException");
            }
        }

        byteBuffer.put(1, (byte)12);
        byteBuffer.put(2, (byte)34);
        ImageSource.SourceOptions srcOpts = new ImageSource.SourceOptions();
        imageSource = ImageSource.create(byteBuffer, srcOpts);
        assertNotNull(imageSource);
        ImageSource.SourceInfo sourceInfo = imageSource.getSourceInfo();
        assertNotNull(sourceInfo);
        assertEquals("image/x-raw", sourceInfo.encodedFormat);

        /**
         * @tc.steps: step2.create pixel map.
         * @tc.expected: step2.create pixel map should fail
         */
        ImageSource.DecodingOptions decodingOpts = new ImageSource.DecodingOptions();
        assertThrows("should throw SourceDataMalformedException", SourceDataMalformedException.class, () -> {
            imageSource.createPixelmap(decodingOpts);
        });
    }

    /**
     * @tc.name: testImageSourceWbmp004
     * @tc.desc: create image source multiple times
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testImageSourceWbmp004() {
        /**
         * @tc.steps: step1.get image source form path.
         * @tc.expected: step1.get image source form path not null
         */
        ImageSource.SourceOptions srcOpts = new ImageSource.SourceOptions();
        String pathName = "/sdcard/multimedia/image/test.wbmp";
        imageSource = ImageSource.create(pathName, srcOpts);
        assertNotNull(imageSource);
        ImageSource.SourceInfo sourceInfo = imageSource.getSourceInfo();
        assertNotNull(sourceInfo);
        assertEquals("image/vnd.wap.wbmp", sourceInfo.encodedFormat);

        ImageSource.DecodingOptions decodingOpts = new ImageSource.DecodingOptions();
        PixelMap pixelMap1 = imageSource.createPixelmap(decodingOpts);
        assertNotNull(pixelMap1);

        PixelMap pixelMap2 = imageSource.createPixelmap(decodingOpts);
        assertNotNull(pixelMap2);
    }

    /**
     * @tc.name: testImageSourceBmp001
     * @tc.desc: create image source from bmp file path, decode to ARGB_8888 format pixel map, then scale/rotate/crop
     * the pixel map.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testImageSourceBmp001() {
        /**
         * @tc.steps: step1.get image source form path.
         * @tc.expected: step1.get image source form path not null.
         */
        ImageSource.SourceOptions srcOpts = new ImageSource.SourceOptions();
        String pathName = "/sdcard/multimedia/image/test.bmp";
        imageSource = ImageSource.create(pathName, srcOpts);
        assertNotNull(imageSource);
        ImageSource.SourceInfo sourceInfo = imageSource.getSourceInfo();
        assertNotNull(sourceInfo);
        assertEquals(1, sourceInfo.topLevelImageNum);
        assertEquals("image/bmp", sourceInfo.encodedFormat);

        /**
         * @tc.steps: step2.set decode option about crop/rotate/scale, corp to 100*50 from [150, 10] point,
         * rotate 90 degree and then scale to 100*200 size.
         * @tc.expected: step2.create pixel map not null and check information ok.
         */
        ImageSource.DecodingOptions decodingOpts = new ImageSource.DecodingOptions();
        decodingOpts.desiredRegion = new Rect(150, 10, 100, 50);
        decodingOpts.rotateDegrees = 90;
        decodingOpts.desiredSize = new Size(100, 200);
        decodingOpts.desiredPixelFormat = PixelFormat.ARGB_8888;
        decodingOpts.allowPartialImage = true;
        pixelMap = imageSource.createPixelmap(decodingOpts);
        assertNotNull(pixelMap);
        assertEquals(100, pixelMap.getImageInfo().size.width);
        assertEquals(200, pixelMap.getImageInfo().size.height);
    }

    /**
     * @tc.name: testMemoryPolicy001
     * @tc.desc: low memory will change pixelFormat to RGB_565
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testMemoryPolicy001() {
        /**
         * @tc.steps: step1. decode jpg file to get editable pixelmap.
         * @tc.expected: step1. get the valid pixelMap from the image.
         */
        imageSource = ImageSource.create("/sdcard/multimedia/image/test.jpg", null);
        assertNotNull(imageSource);
        imageSource.setMemoryUsagePreference(MemoryUsagePreference.LOW_RAM);
        ImageSource.DecodingOptions decodingOpts = new ImageSource.DecodingOptions();
        decodingOpts.editable = true;
        pixelMap = imageSource.createPixelmap(decodingOpts);
        assertNotNull(pixelMap);

        /**
         * @tc.steps: step2. get pixelFormat from pixelMap.
         * @tc.expected: step2. pixelFormat equals RGB_565.
         */
        PixelFormat pixelFormat = pixelMap.getImageInfo().pixelFormat;
        assertEquals(PixelFormat.RGB_565, pixelFormat);
    }

    /**
     * @tc.name: testMemoryPolicy002
     * @tc.desc: low memory will not change pixelFormat to RGB_565, because AlphaType not equals OPAQUE
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testMemoryPolicy002() {
        /**
         * @tc.steps: step1. decode webp file to get editable pixelmap.
         * @tc.expected: step1. get the valid pixelMap from the image.
         */
        imageSource = ImageSource.create("/sdcard/multimedia/image/test.webp", null);
        assertNotNull(imageSource);
        imageSource.setMemoryUsagePreference(MemoryUsagePreference.LOW_RAM);
        ImageSource.DecodingOptions decodingOpts = new ImageSource.DecodingOptions();
        decodingOpts.editable = true;
        pixelMap = imageSource.createPixelmap(decodingOpts);
        assertNotNull(pixelMap);

        /**
         * @tc.steps: step2. get pixelFormat from pixelMap.
         * @tc.expected: step2. pixelFormat equals ARGB_8888.
         */
        PixelFormat pixelFormat = pixelMap.getImageInfo().pixelFormat;
        assertEquals(PixelFormat.ARGB_8888, pixelFormat);
    }

    /**
     * @tc.name: testMemoryPolicy003
     * @tc.desc: test default memory
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testMemoryPolicy003() {
        /**
         * @tc.steps: step1. decode webp file to get editable pixelmap.
         * @tc.expected: step1. get the valid pixelMap from the image and set memory default.
         */
        imageSource = ImageSource.create("/sdcard/multimedia/image/test.webp", null);
        assertNotNull(imageSource);
        imageSource.setMemoryUsagePreference(MemoryUsagePreference.DEFAULT);
        ImageSource.DecodingOptions decodingOpts = new ImageSource.DecodingOptions();
        decodingOpts.editable = true;
        pixelMap = imageSource.createPixelmap(decodingOpts);
        assertNotNull(pixelMap);

        /**
         * @tc.steps: step2. get pixelFormat from pixelMap.
         * @tc.expected: step2. pixelFormat equals ARGB_8888.
         */
        PixelFormat pixelFormat = pixelMap.getImageInfo().pixelFormat;
        assertEquals(PixelFormat.ARGB_8888, pixelFormat);
    }

    /**
     * @tc.name: testMemoryPolicy004
     * @tc.desc: test low memory and desired pixel format set at same time
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testMemoryPolicy004() {
        /**
         * @tc.steps: step1. decode jpg file to get editable pixelmap, set desired pixel format ARGB_8888
         * and memory low ram.
         * @tc.expected: step1. get the valid pixelMap from the image.
         */
        imageSource = ImageSource.create("/sdcard/multimedia/image/test.jpg", null);
        assertNotNull(imageSource);
        imageSource.setMemoryUsagePreference(MemoryUsagePreference.LOW_RAM);
        ImageSource.DecodingOptions decodingOpts = new ImageSource.DecodingOptions();
        decodingOpts.desiredPixelFormat = PixelFormat.ARGB_8888;
        decodingOpts.editable = true;
        pixelMap = imageSource.createPixelmap(decodingOpts);
        assertNotNull(pixelMap);

        /**
         * @tc.steps: step2. get pixelFormat from pixelMap.
         * @tc.expected: step2. pixelFormat equals ARGB_8888.
         */
        PixelFormat pixelFormat = pixelMap.getImageInfo().pixelFormat;
        assertEquals(PixelFormat.ARGB_8888, pixelFormat);
    }

    class MyDecodeListener implements ImageSource.DecodeEventListener {
        private List<DecodeEvent> decodeEvents = new ArrayList<DecodeEvent>();

        private ImageSource imageSouceBack;

        private ImageInfo imageInfoInListener;

        private MemoryUsagePreference memUsage = MemoryUsagePreference.DEFAULT;

        @Override
        public void onDecodeEvent(ImageSource source, DecodeEvent event) {
            LOGGER.info("onDecodeEvent event: %{public}d", event.getValue());
            decodeEvents.add(event);
            if (imageSouceBack == null) {
                imageSouceBack = source;
            }
            DecodeEvent myEvent = DecodeEvent.getDecodeEvent(2);
            if (event == myEvent) {
                imageInfoInListener = source.getImageInfo();

                if (memUsage != MemoryUsagePreference.DEFAULT) {
                    source.setMemoryUsagePreference(memUsage);
                }
            }
        }

        List<DecodeEvent> getEvents() {
            return decodeEvents;
        }

        ImageSource getMySource() {
            return imageSouceBack;
        }

        ImageInfo getMyImageInfo() {
            return imageInfoInListener;
        }

        void setMemoryUsage(MemoryUsagePreference preference) {
            memUsage = preference;
        }
    }

    /**
     * @tc.name: testDecodeEventListener001
     * @tc.desc: set a listener and will get a listener
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testDecodeEventListener001() {
        /**
         * @tc.steps: step1.get image source from path name.
         * @tc.expected: step1.get image source form path name not null.
         */
        imageSource = ImageSource.create("/sdcard/multimedia/image/test.webp", null);
        assertNotNull(imageSource);

        /**
         * @tc.steps: step2.set a decode event listener.
         * @tc.expected: step2.partialImageListenerTest equals EVENT_COMPLETE_DECODE,
         */
        MyDecodeListener testListener = new MyDecodeListener();
        imageSource.setDecodeEventListener(testListener);
        ImageSource.DecodingOptions decodingOpts = new ImageSource.DecodingOptions();
        decodingOpts.editable = true;
        pixelMap = imageSource.createPixelmap(decodingOpts);
        assertTrue(testListener.getEvents().contains(DecodeEvent.EVENT_HEADER_DECODE));
        assertTrue(testListener.getEvents().contains(DecodeEvent.EVENT_COMPLETE_DECODE));
        assertEquals(testListener.getMySource(), imageSource);
        assertNotNull(pixelMap);
    }

    /**
     * @tc.name: testDecodeEventListener002
     * @tc.desc: set a listener and will get a listener
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testDecodeEventListener002() {
        /**
         * @tc.steps: step1.get image source byte array.
         * @tc.expected: step1.get image source form byte array not null
         */
        String pathName = "/sdcard/multimedia/image/test.jpg";
        byte[] data = null;
        try {
            data = Files.readAllBytes(Paths.get(pathName));
        } catch (IOException e) {
            fail("file read exception");
        }
        imageSource = ImageSource.create(data, 0, data.length - 100, null);
        assertNotNull(imageSource);

        /**
         * @tc.steps: step2.set a decode event listener.
         * @tc.expected: step2.partialImageListenerTest equals EVENT_PARTIAL_DECODE,
         */
        MyDecodeListener testListener = new MyDecodeListener();
        imageSource.setDecodeEventListener(testListener);
        ImageSource.DecodingOptions decodingOpts = new ImageSource.DecodingOptions();
        decodingOpts.allowPartialImage = true;
        pixelMap = imageSource.createPixelmap(decodingOpts);
        assertTrue(testListener.getEvents().contains(DecodeEvent.EVENT_HEADER_DECODE));
        assertTrue(testListener.getEvents().contains(DecodeEvent.EVENT_PARTIAL_DECODE));
        assertEquals(testListener.getMySource(), imageSource);
        assertNotNull(pixelMap);
    }

    /**
     * @tc.name: testDecodeEventListener003
     * @tc.desc: set a listener and will not get a listener because not allow partial image
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testDecodeEventListener003() {
        /**
         * @tc.steps: step1.get image source byte array.
         * @tc.expected: step1.get image source form byte array not null
         */
        String pathName = "/sdcard/multimedia/image/test.jpg";
        byte[] data = null;
        try {
            data = Files.readAllBytes(Paths.get(pathName));
        } catch (IOException e) {
            fail("file read exception");
        }
        imageSource = ImageSource.create(data, 0, data.length - 100, null);
        assertNotNull(imageSource);

        /**
         * @tc.steps: step2.set a decode event listener.
         * @tc.expected: step2.throw SourceDataIncompleteException.
         */
        MyDecodeListener testListener = new MyDecodeListener();
        imageSource.setDecodeEventListener(testListener);
        ImageSource.DecodingOptions decodingOpts = new ImageSource.DecodingOptions();
        decodingOpts.allowPartialImage = false;
        assertThrows("should throw SourceDataIncompleteException", SourceDataIncompleteException.class, () -> {
            imageSource.createPixelmap(decodingOpts);
        });
    }

    /**
     * @tc.name: testDecodeEventListener004
     * @tc.desc: set a listener and will get a callback at proper time.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testDecodeEventListener004() {
        /**
         * @tc.steps: step1.get image source from path name.
         * @tc.expected: step1.get image source form path name not null.
         */
        imageSource = ImageSource.create("/sdcard/multimedia/image/test.webp", null);
        assertNotNull(imageSource);

        /**
         * @tc.steps: step2.set a decode event listener.
         * @tc.expected: step2.we can get EVENT_HEADER_DECODE,
         */
        MyDecodeListener testListener = new MyDecodeListener();
        imageSource.setDecodeEventListener(testListener);
        ImageSource.DecodingOptions decodingOpts = new ImageSource.DecodingOptions();
        decodingOpts.editable = true;
        pixelMap = imageSource.createPixelmap(decodingOpts);
        assertTrue(testListener.getEvents().contains(DecodeEvent.EVENT_HEADER_DECODE));
        assertEquals(testListener.getMySource(), imageSource);
        ImageInfo myInfo = testListener.getMyImageInfo();
        ImageInfo info = imageSource.getImageInfo();
        assertEquals(myInfo.size, info.size);
        assertEquals(myInfo.pixelFormat, info.pixelFormat);
        assertNotNull(pixelMap);
    }

    /**
     * @tc.name: testDecodeEventListener005
     * @tc.desc: set two listener and check state.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testDecodeEventListener005() {
        /**
         * @tc.steps: step1.get image source from path name.
         * @tc.expected: step1.get image source form path name not null.
         */
        imageSource = ImageSource.create("/sdcard/multimedia/image/test.webp", null);
        assertNotNull(imageSource);

        /**
         * @tc.steps: step2.set two decode event listener.
         * @tc.expected: step2.we can get EVENT_HEADER_DECODE,
         */
        MyDecodeListener testListener = new MyDecodeListener();
        imageSource.setDecodeEventListener(testListener);

        MyDecodeListener testListener2 = new MyDecodeListener();
        imageSource.setDecodeEventListener(testListener2);

        ImageSource.DecodingOptions decodingOpts = new ImageSource.DecodingOptions();
        decodingOpts.editable = true;
        pixelMap = imageSource.createPixelmap(decodingOpts);
        assertFalse(testListener.getEvents().contains(DecodeEvent.EVENT_HEADER_DECODE));
        assertTrue(testListener2.getEvents().contains(DecodeEvent.EVENT_HEADER_DECODE));
        assertNull(testListener.getMySource());
        assertEquals(testListener2.getMySource(), imageSource);
        assertNotNull(pixelMap);
    }

    /**
     * @tc.name: testDecodeEventListener006
     * @tc.desc: set listener and check state in listener.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testDecodeEventListener006() {
        /**
         * @tc.steps: step1.get image source from path name.
         * @tc.expected: step1.get image source form path name not null.
         */
        imageSource = ImageSource.create("/sdcard/multimedia/image/test.jpg", null);
        assertNotNull(imageSource);

        /**
         * @tc.steps: step2.set decode event listener.
         * @tc.expected: step2. set memory usage is effect when header is decoded.
         */
        MyDecodeListener testListener = new MyDecodeListener();
        imageSource.setDecodeEventListener(testListener);
        testListener.setMemoryUsage(MemoryUsagePreference.LOW_RAM);

        ImageSource.DecodingOptions decodingOpts = new ImageSource.DecodingOptions();
        decodingOpts.editable = true;
        pixelMap = imageSource.createPixelmap(decodingOpts);
        assertNotNull(pixelMap);
        assertTrue(testListener.getEvents().contains(DecodeEvent.EVENT_HEADER_DECODE));

        /**
         * @tc.steps: step3. get pixelFormat from pixelMap.
         * @tc.expected: step3. pixelFormat equals RGB_565.
         */
        PixelFormat pixelFormat = pixelMap.getImageInfo().pixelFormat;
        assertEquals(PixelFormat.RGB_565, pixelFormat);
    }

    /**
     * @tc.name: testDecodeEventListener007
     * @tc.desc: set same listener on two source.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testDecodeEventListener007() {
        /**
         * @tc.steps: step1.get image source from path name.
         * @tc.expected: step1.get image source form path name not null.
         */
        imageSource = ImageSource.create("/sdcard/multimedia/image/test.jpg", null);
        assertNotNull(imageSource);

        ImageSource imageSource2 = ImageSource.create("/sdcard/multimedia/image/test.webp", null);
        assertNotNull(imageSource);

        /**
         * @tc.steps: step2.set decode event listener on different imagesource.
         * @tc.expected: step2. check the state.
         */
        MyDecodeListener testListener = new MyDecodeListener();
        imageSource.setDecodeEventListener(testListener);
        imageSource2.setDecodeEventListener(testListener);

        ImageSource.DecodingOptions decodingOpts = new ImageSource.DecodingOptions();
        decodingOpts.editable = true;
        pixelMap = imageSource.createPixelmap(decodingOpts);
        assertNotNull(pixelMap);
        PixelMap pixelMap2 = imageSource2.createPixelmap(decodingOpts);
        assertNotNull(pixelMap2);

        assertTrue(testListener.getEvents().contains(DecodeEvent.EVENT_HEADER_DECODE));
        assertTrue(testListener.getEvents().contains(DecodeEvent.EVENT_COMPLETE_DECODE));
    }

    /**
     * @tc.name: testDecodeEventListener008
     * @tc.desc: set listener for Incremental decode.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testDecodeEventListener008() {
        /**
         * @tc.steps: step1.get incremental image source.
         * @tc.expected: step1.get incremental image source not null
         */
        ImageSource.SourceOptions srcOpts = new ImageSource.SourceOptions();
        ImageSource.IncrementalSourceOptions incOpts = new ImageSource.IncrementalSourceOptions();
        incOpts.opts = srcOpts;
        incOpts.mode = ImageSource.UpdateMode.INCREMENTAL_DATA;
        imageSource = ImageSource.createIncrementalSource(incOpts);
        assertNotNull(imageSource);

        MyDecodeListener testListener = new MyDecodeListener();
        imageSource.setDecodeEventListener(testListener);

        /**
         * @tc.steps: step2.create pixel map.
         * @tc.expected: step2.create pixel map not null and check information ok
         */
        ImageSource.DecodingOptions decodingOpts = new ImageSource.DecodingOptions();
        decodingOpts.desiredSize = new Size(100, 60);
        decodingOpts.desiredRegion = new Rect(10, 10, 150, 120);
        decodingOpts.rotateDegrees = 90;
        decodingOpts.desiredPixelFormat = PixelFormat.BGRA_8888;
        decodingOpts.allowPartialImage = true;
        decodingOpts.allocator = AllocatorType.HEAP;

        String pathName = "/sdcard/multimedia/image/test.webp";
        final int size = 1024;
        int accumulatedData = 0;
        byte[] data = new byte[size];
        File file = new File(pathName);
        try (FileInputStream fileInputStream = new FileInputStream(file)) {
            int bytes = 0;
            while (bytes != -1) {
                bytes = fileInputStream.read(data);
                if (bytes == -1) {
                    break;
                }
                boolean isFinal = (bytes != size);
                imageSource.updateData(data, 0, bytes, isFinal);
                pixelMap = imageSource.createPixelmap(decodingOpts);
                assertNotNull(pixelMap);

                accumulatedData += bytes;
                if (accumulatedData > 1024) {
                    assertTrue(testListener.getEvents().contains(DecodeEvent.EVENT_HEADER_DECODE));
                }
                // assume we can get a valid header now
                if (accumulatedData > 2048) {
                    assertTrue(testListener.getEvents().contains(DecodeEvent.EVENT_HEADER_DECODE));
                    assertEquals(286, testListener.getMyImageInfo().size.width);
                    assertEquals(221, testListener.getMyImageInfo().size.height);
                }

                if (isFinal) {
                    assertTrue(testListener.getEvents().contains(DecodeEvent.EVENT_COMPLETE_DECODE));
                }
            }
        } catch (IOException e) {
            fail("file read exception");
        } finally {
            pixelMap.release();
        }
    }

    /**
     * @tc.name: testDecodeEventListener009
     * @tc.desc: set listener for Incremental decode.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testDecodeEventListener009() {
        /**
         * @tc.steps: step1.get incremental image source.
         * @tc.expected: step1.get incremental image source not null
         */
        ImageSource.SourceOptions srcOpts = new ImageSource.SourceOptions();
        ImageSource.IncrementalSourceOptions incOpts = new ImageSource.IncrementalSourceOptions();
        incOpts.opts = srcOpts;
        incOpts.mode = ImageSource.UpdateMode.INCREMENTAL_DATA;
        imageSource = ImageSource.createIncrementalSource(incOpts);
        assertNotNull(imageSource);

        MyDecodeListener testListener = new MyDecodeListener();
        imageSource.setDecodeEventListener(testListener);

        /**
         * @tc.steps: step2.create pixel map.
         * @tc.expected: step2.create pixel map not null and check information ok
         */
        ImageSource.DecodingOptions decodingOpts = new ImageSource.DecodingOptions();
        decodingOpts.desiredSize = new Size(100, 60);
        decodingOpts.desiredRegion = new Rect(10, 10, 150, 120);
        decodingOpts.rotateDegrees = 90;
        decodingOpts.desiredPixelFormat = PixelFormat.BGRA_8888;
        decodingOpts.allowPartialImage = true;

        String pathName = "/sdcard/multimedia/image/test.webp";
        final int size = 1024;
        int accumulatedData = 0;
        byte[] data = new byte[size];
        File file = new File(pathName);
        long fileLength = file.length();
        try (FileInputStream fileInputStream = new FileInputStream(file)) {
            int bytes = 0;
            boolean done = false;
            while (!done) {
                bytes = fileInputStream.read(data);
                if (bytes == -1) {
                    break;
                }
                accumulatedData += bytes;
                // assume we can get a valid header now
                if (accumulatedData > (fileLength / 2)) {
                    imageSource.updateData(data, 0, bytes, true);
                    pixelMap = imageSource.createPixelmap(decodingOpts);
                    assertNotNull(pixelMap);
                    done = true;
                } else {
                    imageSource.updateData(data, 0, bytes, false);
                    pixelMap = imageSource.createPixelmap(decodingOpts);
                    assertNotNull(pixelMap);
                }
            }
            assertFalse(testListener.getEvents().contains(DecodeEvent.EVENT_PARTIAL_DECODE));
            assertFalse(testListener.getEvents().contains(DecodeEvent.EVENT_COMPLETE_DECODE));
        } catch (IOException e) {
            fail("file read exception");
        } finally {
            pixelMap.release();
        }
    }

    /**
     * @tc.name: testDensityChange001
     * @tc.desc: set fitDensity and baseDensity, size will change
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testDensityChange001() {
        /**
         * @tc.steps: step1.get image source form path.
         * @tc.expected: step1.get image source form path not null.
         */
        ImageSource.SourceOptions srcOpts = new ImageSource.SourceOptions();
        srcOpts.baseDensity = 200;
        String pathName = "/sdcard/multimedia/image/test.bmp";
        imageSource = ImageSource.create(pathName, srcOpts);
        assertNotNull(imageSource);
        ImageSource.SourceInfo sourceInfo = imageSource.getSourceInfo();
        assertNotNull(sourceInfo);
        assertEquals(1, sourceInfo.topLevelImageNum);
        assertEquals("image/bmp", sourceInfo.encodedFormat);

        /**
         * @tc.steps: step2.get the image after density change.
         * @tc.expected: step2.width and height will be doubled, base density is the density set before.
         */
        ImageSource.DecodingOptions decodingOpts = new ImageSource.DecodingOptions();
        decodingOpts.fitDensity = 400;
        pixelMap = imageSource.createPixelmap(decodingOpts);
        assertNotNull(pixelMap);
        assertEquals(200, pixelMap.getBaseDensity());
        assertEquals(944, pixelMap.getImageInfo().size.width);
        assertEquals(150, pixelMap.getImageInfo().size.height);
    }

    /**
     * @tc.name: testDensityChange002
     * @tc.desc: set fitDensity and baseDensity, also set desired size, size will change by desired size
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testDensityChange002() {
        /**
         * @tc.steps: step1.get image source form path.
         * @tc.expected: step1.get image source form path not null.
         */
        ImageSource.SourceOptions srcOpts = new ImageSource.SourceOptions();
        srcOpts.baseDensity = 200;
        String pathName = "/sdcard/multimedia/image/test.bmp";
        imageSource = ImageSource.create(pathName, srcOpts);
        assertNotNull(imageSource);
        ImageSource.SourceInfo sourceInfo = imageSource.getSourceInfo();
        assertNotNull(sourceInfo);
        assertEquals(1, sourceInfo.topLevelImageNum);
        assertEquals("image/bmp", sourceInfo.encodedFormat);

        /**
         * @tc.steps: step2.get the image after density change.
         * @tc.expected: step2.width and height will be same with desired width and height.
         */
        ImageSource.DecodingOptions decodingOpts = new ImageSource.DecodingOptions();
        decodingOpts.fitDensity = 400;
        decodingOpts.desiredSize = new Size(100, 200);
        pixelMap = imageSource.createPixelmap(decodingOpts);
        assertNotNull(pixelMap);
        assertEquals(200, pixelMap.getBaseDensity());
        assertEquals(100, pixelMap.getImageInfo().size.width);
        assertEquals(200, pixelMap.getImageInfo().size.height);
    }

    /**
     * @tc.name: testDensityChange003
     * @tc.desc: just set baseDensity
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testDensityChange003() {
        /**
         * @tc.steps: step1.get image source form path.
         * @tc.expected: step1.get image source form path not null.
         */
        ImageSource.SourceOptions srcOpts = new ImageSource.SourceOptions();
        srcOpts.baseDensity = 200;
        String pathName = "/sdcard/multimedia/image/test.bmp";
        imageSource = ImageSource.create(pathName, srcOpts);
        assertNotNull(imageSource);
        ImageSource.SourceInfo sourceInfo = imageSource.getSourceInfo();
        assertNotNull(sourceInfo);
        assertEquals(1, sourceInfo.topLevelImageNum);
        assertEquals("image/bmp", sourceInfo.encodedFormat);

        /**
         * @tc.steps: step2.get the image after density change.
         * @tc.expected: step2.width and height will be same with original width and height.
         */
        ImageSource.DecodingOptions decodingOpts = new ImageSource.DecodingOptions();
        decodingOpts.fitDensity = 0;
        pixelMap = imageSource.createPixelmap(decodingOpts);
        assertNotNull(pixelMap);
        assertEquals(200, pixelMap.getBaseDensity());
        assertEquals(472, pixelMap.getImageInfo().size.width);
        assertEquals(75, pixelMap.getImageInfo().size.height);
    }

    private double[] getMemorySize(String filePath) {
        double[] memorySize = new double[2];
        File txtFile = new File(filePath);
        if (txtFile.exists()) {
            FileReader fileReader = null;
            BufferedReader bufferReader = null;
            try {
                fileReader = new FileReader(txtFile);
                bufferReader = new BufferedReader(fileReader);
                String lineContent = null;
                while ((lineContent = bufferReader.readLine()) != null) {
                    String[] content = lineContent.split("\\s+");
                    if (content.length > 0 && HEAP_STRING.equals(content[1])) {
                        memorySize[0] = Integer.valueOf(content[3]) / CONST_NUMBER;
                    }
                    if (content.length > 0 && SHARED_MEMORY_STRING.equals(content[1])) {
                        memorySize[1] = Integer.valueOf(content[3]) / CONST_NUMBER;
                        break;
                    }
                }
            } catch (FileNotFoundException e) {
                LOGGER.error("read file FileNotFoundException");
            } catch (IOException e) {
                LOGGER.error("read file IOException");
            } finally {
                try {
                    bufferReader.close();
                    fileReader.close();
                } catch (IOException e) {
                    LOGGER.error("close stream IOException");
                }
            }
        }
        return memorySize;
    }

    /**
     * @tc.name: testAllocatorType001
     * @tc.desc: create pixel map with shared memory
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testAllocatorType001() {
        /**
         * @tc.steps: step1.get image source File.
         * @tc.expected: step1.get image source form File not null
         */
        ImageSource.SourceOptions srcOpts = new ImageSource.SourceOptions();
        String pathName = "/sdcard/multimedia/image/test.png";
        File file = new File(pathName);
        imageSource = ImageSource.create(file, srcOpts);
        assertNotNull(imageSource);

        /**
         * @tc.steps: step3.create pixel map with shared memory.
         * @tc.expected: step3.create pixel map not null and check information ok
         */
        ImageSource.DecodingOptions decodingOpts = new ImageSource.DecodingOptions();
        decodingOpts.allocator = AllocatorType.SHARED_MEMORY;
        pixelMap = imageSource.createPixelmap(decodingOpts);
        assertNotNull(pixelMap);
        assertEquals(472, pixelMap.getImageInfo().size.width);
        assertEquals(75, pixelMap.getImageInfo().size.height);
    }

    /**
     * @tc.name: testAllocatorType002
     * @tc.desc: create pixel map with heap
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testAllocatorType002() {
        /**
         * @tc.steps: step1.get image source File.
         * @tc.expected: step1.get image source form File not null
         */
        ImageSource.SourceOptions srcOpts = new ImageSource.SourceOptions();
        String pathName = "/sdcard/multimedia/image/test.png";
        File file = new File(pathName);
        imageSource = ImageSource.create(file, srcOpts);
        assertNotNull(imageSource);

        /**
         * @tc.steps: step3.create pixel map with heap.
         * @tc.expected: step3.create pixel map not null and check information ok
         */
        ImageSource.DecodingOptions decodingOpts = new ImageSource.DecodingOptions();
        decodingOpts.allocator = AllocatorType.HEAP;
        pixelMap = imageSource.createPixelmap(decodingOpts);
        assertNotNull(pixelMap);
        assertEquals(472, pixelMap.getImageInfo().size.width);
        assertEquals(75, pixelMap.getImageInfo().size.height);
    }

    /**
     * @tc.name: testAllocatorType003
     * @tc.desc: create pixel map with default memory
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testAllocatorType003() {
        /**
         * @tc.steps: step1.get image source File.
         * @tc.expected: step1.get image source form File not null
         */
        ImageSource.SourceOptions srcOpts = new ImageSource.SourceOptions();
        String pathName = "/sdcard/multimedia/image/test.png";
        File file = new File(pathName);
        imageSource = ImageSource.create(file, srcOpts);
        assertNotNull(imageSource);

        /**
         * @tc.steps: step3.create pixel map with default memory.
         * @tc.expected: step3.create pixel map not null and check information ok
         */
        ImageSource.DecodingOptions decodingOpts = new ImageSource.DecodingOptions();
        decodingOpts.allocator = AllocatorType.DEFAULT;
        pixelMap = imageSource.createPixelmap(decodingOpts);
        assertNotNull(pixelMap);
        assertEquals(472, pixelMap.getImageInfo().size.width);
        assertEquals(75, pixelMap.getImageInfo().size.height);
    }

    /**
     * @tc.name: testAllocatorType004
     * @tc.desc: create pixel map with default memory
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testAllocatorType004() {
        /**
         * @tc.steps: step1.get image source File.
         * @tc.expected: step1.get image source form File not null
         */
        ImageSource.SourceOptions srcOpts = new ImageSource.SourceOptions();
        String pathName = "/sdcard/multimedia/image/test.jpg";
        File file = new File(pathName);
        imageSource = ImageSource.create(file, srcOpts);
        assertNotNull(imageSource);
        /**
         * @tc.steps: step3.create pixel map with default memory.
         * @tc.expected: step3.create pixel map not null and check information ok
         */
        ImageSource.DecodingOptions decodingOpts = new ImageSource.DecodingOptions();
        pixelMap = imageSource.createPixelmap(decodingOpts);
        assertNotNull(pixelMap);
        assertEquals(472, pixelMap.getImageInfo().size.width);
        assertEquals(226, pixelMap.getImageInfo().size.height);
    }
}
