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
import static org.junit.Assert.assertTrue;
import static org.junit.Assert.assertThrows;
import static org.junit.Assert.fail;

import ohos.media.image.common.Size;
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
import org.junit.Rule;
import org.junit.Test;
import org.junit.rules.ExpectedException;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.OutputStream;
import java.util.HashSet;

/**
 * ImagePackerTest, test cases for ImagePacker class, mainly including test cases for image encoding functions
 *
 * @since 1
 */
public class ImagePackerTest {
    private static final Logger LOGGER = LoggerFactory.getImageLogger(ImagePackerTest.class);

    private static final String PNG_SRC_FILE_PATH = "/sdcard/multimedia/image/test.png";

    private static final String JPG_PACKED_FILE_PATH = "/sdcard/multimedia/image/test_packer.jpg";

    private static File pngFile;

    /**
     * Action before all test case.
     */
    @BeforeClass
    public static void setUpBeforeClass() throws Exception {
        pngFile = new File(PNG_SRC_FILE_PATH);
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
        File file = new File(JPG_PACKED_FILE_PATH);
        if (file.exists()) {
            file.delete();
        }
    }

    /**
     * Action after test case.
     */
    @After
    public void tearDown() throws Exception {
    }

    /**
     * @tc.name: testImagePacker001
     * @tc.desc: get supported image formats.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testImagePacker001() {
        /**
         * @tc.steps: step1.get supported image formats.
         * @tc.expected: step1.supported image formats not null
         */
        HashSet<String> formats = ImagePacker.getSupportedFormats();
        assertNotNull(formats);

        /**
         * @tc.steps: step2.check supported image formats.
         * @tc.expected: step2.supported image formats checked ok
         */
        assertTrue(formats.contains("image/jpeg"));
    }

    /**
     * @tc.name: testImagePacker002
     * @tc.desc: create ImagePacker instance
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testImagePacker002() {
        /**
         * @tc.steps: step1.create image packer instance.
         * @tc.expected: step1.check the image packer valid.
         */
        ImagePacker imagePacker = ImagePacker.create();
        assertNotNull(imagePacker);
        imagePacker.release();
    }

    /**
     * @tc.name: testImagePacker003
     * @tc.desc: initialize packing settings
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testImagePacker003() {
        /**
         * @tc.steps: step1.create image packer instance.
         * @tc.expected: step1.check the image packer valid.
         */
        ImagePacker imagePacker = ImagePacker.create();
        assertNotNull(imagePacker);

        /**
         * @tc.steps: step2.init image packer with byte array and option.
         * @tc.expected: step2.check init image packer success.
         */
        ImagePacker.PackingOptions packingOptions = new ImagePacker.PackingOptions();
        packingOptions.format = "image/jpeg";
        byte[] data = new byte[10];
        boolean succeeded = imagePacker.initializePacking(data, packingOptions);
        assertTrue(succeeded);

        /**
         * @tc.steps: step3.init image packer with byte array、offset and option.
         * @tc.expected: step3.check init image packer success.
         */
        int offset = 3;
        succeeded = imagePacker.initializePacking(data, offset, packingOptions);
        assertTrue(succeeded);
        imagePacker.release();
    }

    /**
     * @tc.name: testImagePacker004
     * @tc.desc: initialize packing settings with wrong parameters
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testImagePacker004() {
        /**
         * @tc.steps: step1.create image packer instance.
         * @tc.expected: step1.check the image packer valid.
         */
        ImagePacker imagePacker = ImagePacker.create();
        assertNotNull(imagePacker);

        /**
         * @tc.steps: step2.init image packer with byte array and option.
         * @tc.expected: step2.check init image packer success.
         */
        ImagePacker.PackingOptions packingOptions = new ImagePacker.PackingOptions();
        packingOptions.format = "image/jpeg";
        byte[] data = new byte[10];
        boolean succeed = imagePacker.initializePacking(data, null);
        assertTrue(succeed);
        imagePacker.release();
    }

    /**
     * @tc.name: testImagePacker005
     * @tc.desc: test add pixelMap to image packer
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testImagePacker005() {
        /**
         * @tc.steps: step1.create image packer instance.
         * @tc.expected: step1.check the image packer valid.
         */
        ImagePacker imagePacker = ImagePacker.create();
        assertNotNull(imagePacker);
        ImagePacker.PackingOptions packingOptions = new ImagePacker.PackingOptions();
        packingOptions.format = "image/jpeg";
        byte[] data = new byte[10];
        boolean succeeded = imagePacker.initializePacking(data, packingOptions);
        assertTrue(succeeded);

        /**
         * @tc.steps: step2.add image source with default.
         * @tc.expected: step2.check add image.
         */
        ImageSource.SourceOptions srcOpts = new ImageSource.SourceOptions();
        ImageSource imageSource = ImageSource.create(PNG_SRC_FILE_PATH, srcOpts);
        succeeded = imagePacker.addImage(imageSource);
        assertTrue(succeeded);
        imagePacker.release();

        /**
         * @tc.steps: step3.add image source with index.
         * @tc.expected: step3.check add image.
         */
        ImagePacker imagePacker1 = ImagePacker.create();
        assertNotNull(imagePacker1);
        succeeded = imagePacker1.initializePacking(data, packingOptions);
        assertTrue(succeeded);
        succeeded = imagePacker1.addImage(imageSource, 0);
        assertTrue(succeeded);
        imagePacker1.release();

        /**
         * @tc.steps: step4.add image source with index.
         * @tc.expected: step4.check add image.
         */
        ImageSource.DecodingOptions decodingOpts = new ImageSource.DecodingOptions();
        PixelMap pixelMap = imageSource.createPixelmap(decodingOpts);
        assertNotNull(pixelMap);
        ImagePacker imagePacker2 = ImagePacker.create();
        assertNotNull(imagePacker2);
        succeeded = imagePacker2.initializePacking(data, packingOptions);
        assertTrue(succeeded);
        succeeded = imagePacker2.addImage(pixelMap);
        assertTrue(succeeded);
        imagePacker2.release();
        pixelMap.release();
        imageSource.release();
    }

    /**
     * @tc.name: testImagePacker006
     * @tc.desc: test add pixelMap to image packer failed
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testImagePacker006() {
        /**
         * @tc.steps: step1.create image packer instance.
         * @tc.expected: step1.check the image packer valid.
         */
        ImagePacker imagePacker = ImagePacker.create();
        assertNotNull(imagePacker);
        ImagePacker.PackingOptions packingOptions = new ImagePacker.PackingOptions();
        packingOptions.format = "image/jpeg";
        byte[] data = new byte[10];
        boolean succeeded = imagePacker.initializePacking(data, packingOptions);
        assertTrue(succeeded);

        /**
         * @tc.steps: step2.add image source with more than one time.
         * @tc.expected: step2.check add image.
         */
        ImageSource.SourceOptions srcOpts = new ImageSource.SourceOptions();
        ImageSource imageSource = ImageSource.create(PNG_SRC_FILE_PATH, srcOpts);
        imagePacker.addImage(imageSource);
        succeeded = imagePacker.addImage(imageSource);
        assertFalse(succeeded);
        boolean failed = imagePacker.addImage(imageSource);
        assertFalse(failed);
        failed = imagePacker.addImage(imageSource, 0);
        assertFalse(failed);

        /**
         * @tc.steps: step3.add image source with wrong index.
         * @tc.expected: step3.check add image.
         */
        failed = imagePacker.addImage(imageSource, 10);
        assertFalse(failed);

        /**
         * @tc.steps: step4.add image source with index.
         * @tc.expected: step4.check add image.
         */
        ImageSource.DecodingOptions decodingOpts = new ImageSource.DecodingOptions();
        PixelMap pixelMap = imageSource.createPixelmap(decodingOpts);
        assertNotNull(pixelMap);
        failed = imagePacker.addImage(pixelMap);
        assertFalse(failed);
        imagePacker.release();
        imageSource.release();
        pixelMap.release();
    }

    /**
     * @tc.name: testImagePacker007
     * @tc.desc: encode image to file with specified object.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testImagePacker007() {
        /**
         * @tc.steps: step1.get image packer and start packing.
         * @tc.expected: step1.get image packer not null
         */
        ImagePacker imagePacker = ImagePacker.create();
        assertNotNull(imagePacker);

        /**
         * @tc.steps: step2.init with not enough data.
         * @tc.expected: step2.get image packer not null
         */
        ImagePacker.PackingOptions packingOptions = new ImagePacker.PackingOptions();
        packingOptions.format = "image/jpeg";
        packingOptions.quality = 100;
        byte[] data = new byte[1024];
        boolean result = imagePacker.initializePacking(data, 100, packingOptions);
        assertTrue(result);

        /**
         * @tc.steps: step3.check packed image.
         * @tc.expected: step3.image info checked ok
         */
        ImageSource.SourceOptions srcOpts = new ImageSource.SourceOptions();
        ImageSource imageSource = ImageSource.create(PNG_SRC_FILE_PATH, srcOpts);
        result = imagePacker.addImage(imageSource);
        assertTrue(result);
        long dataSize = imagePacker.finalizePacking();
        assertEquals(dataSize, -1L);
        imagePacker.release();
        imageSource.release();
    }

    /**
     * @tc.name: testImagePacker008
     * @tc.desc: encode image to file with specified object.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testImagePacker008() {
        /**
         * @tc.steps: step1.get image packer and start packing.
         * @tc.expected: step1.get image packer not null
         */
        ImagePacker imagePacker = ImagePacker.create();
        assertNotNull(imagePacker);

        /**
         * @tc.steps: step2.init with enough data.
         * @tc.expected: step2.get image packer not null
         */
        ImagePacker.PackingOptions packingOptions = new ImagePacker.PackingOptions();
        packingOptions.format = "image/jpeg";
        packingOptions.quality = 100;
        byte[] data = new byte[409600];
        boolean result = imagePacker.initializePacking(data, packingOptions);
        assertTrue(result);

        /**
         * @tc.steps: step3.check packed image.
         * @tc.expected: step3.image info checked ok
         */
        ImageSource.SourceOptions srcOpts = new ImageSource.SourceOptions();
        ImageSource imageSource = ImageSource.create(PNG_SRC_FILE_PATH, srcOpts);
        result = imagePacker.addImage(imageSource);
        assertTrue(result);
        long dataSize = imagePacker.finalizePacking();
        assertNotEquals(dataSize, -1L);
        imagePacker.release();

        /**
         * @tc.steps: step3.check packed image data.
         * @tc.expected: step3.image info checked ok
         */
        ImageSource packedSource = ImageSource.create(data, srcOpts);
        checkPackImageValid(imageSource, packedSource);
        imageSource.release();
        packedSource.release();
    }

    private void checkPackImageValid(ImageSource source, ImageSource packed) {
        assertNotNull(source);
        assertNotNull(packed);
        // source
        assertEquals(472, source.getImageInfo().size.width);
        assertEquals(75, source.getImageInfo().size.height);
        // packed
        assertEquals(472, packed.getImageInfo().size.width);
        assertEquals(75, packed.getImageInfo().size.height);
        ImageSource.DecodingOptions decodingOpts = new ImageSource.DecodingOptions();
        decodingOpts.desiredSize = new Size(472, 75);
        PixelMap packedPixelMap = packed.createPixelmap(decodingOpts);
        assertNotNull(packedPixelMap);
        assertEquals(472, packedPixelMap.getImageInfo().size.width);
        assertEquals(75, packedPixelMap.getImageInfo().size.height);
        packedPixelMap.release();
    }

    /**
     * @tc.name: testImagePacker009
     * @tc.desc: encode image to file with specified object.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testImagePacker009() {
        /**
         * @tc.steps: step1.get image packer and start packing.
         * @tc.expected: step1.get image packer not null
         */
        ImagePacker imagePacker = ImagePacker.create();
        assertNotNull(imagePacker);
        imagePacker.release();

        ImagePacker.PackingOptions packingOptions = new ImagePacker.PackingOptions();
        packingOptions.format = "image/jpeg";
        packingOptions.quality = 100;

        /**
         * @tc.steps: step2.init with byte array use release imagePacker.
         * @tc.expected: step2.packer the image
         */
        byte[] data = new byte[10];
        boolean result = imagePacker.initializePacking(data, packingOptions);
        assertFalse(result);
        ImageSource.SourceOptions srcOpts = new ImageSource.SourceOptions();
        ImageSource imageSource = ImageSource.create(PNG_SRC_FILE_PATH, srcOpts);
        ImageSource.DecodingOptions decodingOpts = new ImageSource.DecodingOptions();
        PixelMap pixelMap = imageSource.createPixelmap(decodingOpts);
        assertNotNull(pixelMap);
        result = imagePacker.addImage(pixelMap);
        assertFalse(result);
        long dataSize = imagePacker.finalizePacking();
        assertEquals(dataSize, -1L);
        imagePacker.release();
        pixelMap.release();
    }

    /**
     * @tc.name: testImagePacker0010
     * @tc.desc: encode image to file with specified object.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testImagePacker010() {
        /**
         * @tc.steps: step1.get image packer and start packing.
         * @tc.expected: step1.get image packer not null
         */
        ImagePacker imagePacker = ImagePacker.create();
        assertNotNull(imagePacker);

        ImagePacker.PackingOptions packingOptions = new ImagePacker.PackingOptions();
        packingOptions.format = "image/jpeg";
        packingOptions.quality = 20;

        /**
         * @tc.steps: step2.init with file.
         * @tc.expected: step2.packer the image
         */
        try (OutputStream outputStream = new FileOutputStream(JPG_PACKED_FILE_PATH);) {
            boolean result = imagePacker.initializePacking(outputStream, packingOptions);
            assertTrue(result);
            ImageSource.SourceOptions srcOpts = new ImageSource.SourceOptions();
            ImageSource imageSource = ImageSource.create(PNG_SRC_FILE_PATH, srcOpts);
            result = imagePacker.addImage(imageSource);
            assertTrue(result);
            long dataSize = imagePacker.finalizePacking();
            assertNotEquals(dataSize, -1L);

            /**
             * @tc.steps: step3.check packed image data.
             * @tc.expected: step3.image info checked ok
             */
            ImageSource packedSource = ImageSource.create(JPG_PACKED_FILE_PATH, srcOpts);
            checkPackImageValid(imageSource, packedSource);
            imageSource.release();
            packedSource.release();
        } catch (FileNotFoundException e) {
            LOGGER.error("ImagePacker, file not found");
        } catch (IOException fio) {
            LOGGER.error("ImagePacker, IO Exception");
        }
        imagePacker.release();
    }

    /**
     * @tc.name: testImagePacker011
     * @tc.desc: encode image to file with specified object.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testImagePacker011() {
        /**
         * @tc.steps: step1.get image packer and start packing.
         * @tc.expected: step1.get image packer not null
         */
        ImagePacker imagePacker = ImagePacker.create();
        assertNotNull(imagePacker);

        ImagePacker.PackingOptions packingOptions = new ImagePacker.PackingOptions();
        packingOptions.format = "image/jpeg";
        packingOptions.quality = 80;

        /**
         * @tc.steps: step2.init with file.
         * @tc.expected: step2.get image packer not null
         */
        try (FileOutputStream outputStream = new FileOutputStream(JPG_PACKED_FILE_PATH)) {
            boolean result = imagePacker.initializePacking(outputStream, packingOptions);
            assertTrue(result);
            ImageSource.SourceOptions srcOpts = new ImageSource.SourceOptions();
            ImageSource imageSource = ImageSource.create(PNG_SRC_FILE_PATH, srcOpts);
            result = imagePacker.addImage(imageSource, 0);
            assertTrue(result);
            long dataSize = imagePacker.finalizePacking();
            assertNotEquals(dataSize, -1L);

            /**
             * @tc.steps: step3.check packed image data.
             * @tc.expected: step3.image info checked ok
             */
            ImageSource packedSource = ImageSource.create(JPG_PACKED_FILE_PATH, srcOpts);
            checkPackImageValid(imageSource, packedSource);
            imageSource.release();
            packedSource.release();
        } catch (FileNotFoundException e) {
            LOGGER.error("ImagePacker, file not found");
        } catch (IOException fio) {
            LOGGER.error("ImagePacker, IO Exception");
        }
        imagePacker.release();
    }

    /**
     * @tc.name: testImagePacker012
     * @tc.desc: encode image to file with specified object.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testImagePacker012() {
        /**
         * @tc.steps: step1.get image packer and start packing.
         * @tc.expected: step1.get image packer not null
         */
        ImagePacker imagePacker = ImagePacker.create();
        assertNotNull(imagePacker);

        ImagePacker.PackingOptions packingOptions = new ImagePacker.PackingOptions();
        packingOptions.format = "image/jpeg";
        packingOptions.quality = 0;

        /**
         * @tc.steps: step2.init with file.
         * @tc.expected: step2.get image packer not null
         */
        try (FileOutputStream outputStream = new FileOutputStream(JPG_PACKED_FILE_PATH);) {
            boolean result = imagePacker.initializePacking(outputStream, packingOptions);
            assertTrue(result);
            ImageSource.SourceOptions srcOpts = new ImageSource.SourceOptions();
            ImageSource imageSource = ImageSource.create(PNG_SRC_FILE_PATH, srcOpts);
            result = imagePacker.addImage(imageSource);
            assertTrue(result);
            long dataSize = imagePacker.finalizePacking();
            assertNotEquals(dataSize, -1L);
            /**
             * @tc.steps: step3.check packed image data.
             * @tc.expected: step3.image info checked ok
             */
            ImageSource packedSource = ImageSource.create(JPG_PACKED_FILE_PATH, srcOpts);
            checkPackImageValid(imageSource, packedSource);
            imageSource.release();
            packedSource.release();
        } catch (FileNotFoundException e) {
            LOGGER.error("ImagePacker, file not found");
        } catch (IOException fio) {
            LOGGER.error("ImagePacker, IO Exception");
        }
        imagePacker.release();
    }

    /**
     * @tc.name: testImagePacker013
     * @tc.desc: encode image to file with specified object.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testImagePacker013() {
        /**
         * @tc.steps: step1.get image packer and start packing.
         * @tc.expected: step1.get image packer not null
         */
        ImagePacker imagePacker = ImagePacker.create();
        assertNotNull(imagePacker);

        /**
         * @tc.steps: step2.init with enough data.
         * @tc.expected: step2.get image packer not null
         */
        ImagePacker.PackingOptions packingOptions = new ImagePacker.PackingOptions();
        packingOptions.format = "image/jpeg";
        packingOptions.quality = 100;
        byte[] data = new byte[409600];
        boolean result = imagePacker.initializePacking(data, 409600 - 1, packingOptions);
        assertTrue(result);

        /**
         * @tc.steps: step3.check packed image.
         * @tc.expected: step3.image info checked ok
         */
        ImageSource.SourceOptions srcOpts = new ImageSource.SourceOptions();
        ImageSource imageSource = ImageSource.create(PNG_SRC_FILE_PATH, srcOpts);
        result = imagePacker.addImage(imageSource);
        assertTrue(result);
        long dataSize = imagePacker.finalizePacking();
        assertEquals(dataSize, -1L);
        imagePacker.release();
        imageSource.release();
    }

    /**
     * @tc.name: testImagePacker014
     * @tc.desc: encode image to null byte array.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testImagePacker014() {
        /**
         * @tc.steps: step1.get image packer and start packing.
         * @tc.expected: step1.get image packer not null.
         */
        ImagePacker imagePacker = ImagePacker.create();
        assertNotNull(imagePacker);

        /**
         * @tc.steps: step2.init with null byte array.
         * @tc.expected: step2.throw IllegalArgumentException.
         */
        ImagePacker.PackingOptions packingOptions = null;
        byte[] data = null;

        assertThrows("should throw IllegalArgumentException", IllegalArgumentException.class, () -> {
            imagePacker.initializePacking(data, packingOptions);
        });
    }

    /**
     * @tc.name: testImagePacker015
     * @tc.desc: encode image to null byte array.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testImagePacker015() {
        /**
         * @tc.steps: step1.get image packer and start packing.
         * @tc.expected: step1.get image packer not null.
         */
        ImagePacker imagePacker = ImagePacker.create();
        assertNotNull(imagePacker);

        /**
         * @tc.steps: step2.init with null byte array.
         * @tc.expected: step2.throw IllegalArgumentException.
         */
        ImagePacker.PackingOptions packingOptions = null;
        byte[] data = null;

        assertThrows("should throw IllegalArgumentException", IllegalArgumentException.class, () -> {
            imagePacker.initializePacking(data, 0, packingOptions);
        });
    }

    /**
     * @tc.name: testImagePacker016
     * @tc.desc: encode image to byte array with negative offset.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testImagePacker016() {
        /**
         * @tc.steps: step1.get image packer and start packing.
         * @tc.expected: step1.get image packer not null.
         */
        ImagePacker imagePacker = ImagePacker.create();
        assertNotNull(imagePacker);

        /**
         * @tc.steps: step2.init with byte array by negative offset.
         * @tc.expected: step2.throw IndexOutOfBoundsException.
         */
        ImagePacker.PackingOptions packingOptions = null;
        byte[] data = new byte[1024];

        assertThrows("should throw IndexOutOfBoundsException", IndexOutOfBoundsException.class, () -> {
            imagePacker.initializePacking(data, -1, packingOptions);
        });
    }

    /**
     * @tc.name: testImagePacker017
     * @tc.desc: encode image to byte array with large offset.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testImagePacker017() {
        /**
         * @tc.steps: step1.get image packer and start packing.
         * @tc.expected: step1.get image packer not null.
         */
        ImagePacker imagePacker = ImagePacker.create();
        assertNotNull(imagePacker);

        /**
         * @tc.steps: step2.init with byte array by large offset.
         * @tc.expected: step2.throw IndexOutOfBoundsException.
         */
        ImagePacker.PackingOptions packingOptions = null;
        byte[] data = new byte[1024];

        assertThrows("should throw IndexOutOfBoundsException", IndexOutOfBoundsException.class, () -> {
            imagePacker.initializePacking(data, data.length, packingOptions);
        });
    }

    /**
     * @tc.name: testImagePacker018
     * @tc.desc: encode image to null output stream.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testImagePacker018() {
        /**
         * @tc.steps: step1.get image packer and start packing.
         * @tc.expected: step1.get image packer not null.
         */
        ImagePacker imagePacker = ImagePacker.create();
        assertNotNull(imagePacker);

        /**
         * @tc.steps: step2.init with null output stream.
         * @tc.expected: step2.throw IllegalArgumentException.
         */
        ImagePacker.PackingOptions packingOptions = null;
        FileOutputStream outputStream = null;

        assertThrows("should throw IllegalArgumentException", IllegalArgumentException.class, () -> {
            imagePacker.initializePacking(outputStream, packingOptions);
        });
    }

    /**
     * @tc.name: testImagePacker019
     * @tc.desc: add null pixel map to packer.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testImagePacker019() {
        /**
         * @tc.steps: step1.get image packer and start packing.
         * @tc.expected: step1.get image packer not null.
         */
        ImagePacker imagePacker = ImagePacker.create();
        assertNotNull(imagePacker);

        /**
         * @tc.steps: step2.add null pixel map to packer.
         * @tc.expected: step2.throw IllegalArgumentException.
         */
        ImagePacker.PackingOptions packingOptions = null;
        byte[] data = new byte[1024];
        imagePacker.initializePacking(data, packingOptions);

        PixelMap pixelMap = null;
        assertThrows("should throw IllegalArgumentException", IllegalArgumentException.class, () -> {
            imagePacker.addImage(pixelMap);
        });
    }

    /**
     * @tc.name: testImagePacker020
     * @tc.desc: add null image source to packer.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testImagePacker020() {
        /**
         * @tc.steps: step1.get image packer and start packing.
         * @tc.expected: step1.get image packer not null.
         */
        ImagePacker imagePacker = ImagePacker.create();
        assertNotNull(imagePacker);

        /**
         * @tc.steps: step2.add null image source to packer.
         * @tc.expected: step2.throw IllegalArgumentException.
         */
        ImagePacker.PackingOptions packingOptions = null;
        byte[] data = new byte[1024];
        imagePacker.initializePacking(data, packingOptions);

        ImageSource imageSource = null;
        assertThrows("should throw IllegalArgumentException", IllegalArgumentException.class, () -> {
            imagePacker.addImage(imageSource);
        });
    }

    /**
     * @tc.name: testImagePacker021
     * @tc.desc: add negative image source index to packer.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testImagePacker021() {
        /**
         * @tc.steps: step1.get image packer and start packing.
         * @tc.expected: step1.get image packer not null.
         */
        ImagePacker imagePacker = ImagePacker.create();
        assertNotNull(imagePacker);

        /**
         * @tc.steps: step2.add negative image source index to packer.
         * @tc.expected: step2.throw IllegalArgumentException.
         */
        ImagePacker.PackingOptions packingOptions = null;
        byte[] data = new byte[1024];
        imagePacker.initializePacking(data, packingOptions);

        ImageSource imageSource = ImageSource.create(PNG_SRC_FILE_PATH, null);
        assertThrows("should throw IllegalArgumentException", IllegalArgumentException.class, () -> {
            imagePacker.addImage(imageSource, -1);
        });
    }
}


