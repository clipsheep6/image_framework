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

import ohos.media.image.common.PropertyKey;
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

import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.FileNotFoundException;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.io.InputStream;

/**
 * PropertyFilterTest, test cases for PropertyFilter class.
 *
 * @since 3
 */
public class PropertyFilterTest {
    private static final Logger LOGGER = LoggerFactory.getImageLogger(PropertyFilterTest.class);

    private static final String TEST_JPEG = "/sdcard/multimedia/image/test.jpg";

    private static final String HAS_NO_EXIF = "/sdcard/multimedia/image/hasNoExif.JPG";

    private static File pngFile;
    ImageSource mImageSourcePath;
    ImageSource mImageSourceFile;
    PropertyFilter mPropertyFilter;

    /**
     * Action before all test case.
     */
    @BeforeClass
    public static void setUpBeforeClass() {
        pngFile = new File(TEST_JPEG);
        if (!pngFile.exists()) {
            fail("files not exist");
        }
    }

    /**
     * Action before test case.
     */
    @Before
    public void setUp() {
        mImageSourcePath = ImageSource.create(TEST_JPEG, null);
        mImageSourceFile = ImageSource.create(new File(TEST_JPEG), null);
        assertNotNull(mImageSourcePath);
        assertNotNull(mImageSourceFile);
        mPropertyFilter = new PropertyFilter();
    }

    /**
     * Action after test case.
     */
    @After
    public void tearDown() {
    }

    /**
     * @tc.name: testsetPropertyFilter001
     * @tc.desc: set property of filePath ImageSource.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testsetPropertyFilter001() {
        /**
         * @tc.steps: step1. get current properties of ImageSource.
         */
        String oldArtist = mImageSourcePath.getImagePropertyString(PropertyKey.Exif.ARTIST);
        String oldLocation = mImageSourcePath.getImagePropertyString(PropertyKey.Exif.SUBJECT_LOCATION);
        String oldExposure = mImageSourcePath.getImagePropertyString(PropertyKey.Exif.EXPOSURE_TIME);

        /**
         * @tc.steps: step2. set properties for ImageSource.
         * @tc.expected: step2. no error
         */
        String newArtist = "testsetPropertyFilter001_" + String.valueOf(System.currentTimeMillis());
        int newLocation = (int)(System.currentTimeMillis() / 1.0E8);
        Double newExposure = Double.valueOf(newLocation + 0.1);
        long ret = -100;
        try {
            ret = mPropertyFilter.setPropertyString(PropertyKey.Exif.ARTIST, newArtist)
                .setPropertyInt(PropertyKey.Exif.SUBJECT_LOCATION, newLocation)
                .setPropertyDouble(PropertyKey.Exif.EXPOSURE_TIME, newExposure)
                .applyToSource(mImageSourcePath);
        } catch (IOException e) {
            LOGGER.error("file applyToSource exception");
            fail("file applyToSource exception");
        }

        /**
         * @tc.steps: step3. get properties of changed ImageSource.
         * @tc.expected: step3. equal to changed value.
         */
        assertEquals(newArtist, mImageSourcePath.getImagePropertyString(PropertyKey.Exif.ARTIST));
        assertEquals(String.valueOf(newLocation),
            mImageSourcePath.getImagePropertyString(PropertyKey.Exif.SUBJECT_LOCATION));
        assertEquals(String.valueOf(newExposure),
            mImageSourcePath.getImagePropertyString(PropertyKey.Exif.EXPOSURE_TIME));
        assertTrue(ret > 0);

        /**
         * @tc.steps: step4. get properties of changed reloaded ImageSource.
         * @tc.expected: step4. equal to changed value.
         */
        mImageSourcePath = ImageSource.create(TEST_JPEG, null);
        assertEquals(newArtist, mImageSourcePath.getImagePropertyString(PropertyKey.Exif.ARTIST));
        assertEquals(String.valueOf(newLocation),
            mImageSourcePath.getImagePropertyString(PropertyKey.Exif.SUBJECT_LOCATION));
        assertEquals(String.valueOf(newExposure),
            mImageSourcePath.getImagePropertyString(PropertyKey.Exif.EXPOSURE_TIME));
    }

    /**
     * @tc.name: testsetPropertyFilter002
     * @tc.desc: set property of File ImageSource.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testsetPropertyFilter002() {
        /**
         * @tc.steps: step1. get current properties of ImageSource.
         */
        String oldArtist = mImageSourceFile.getImagePropertyString(PropertyKey.Exif.ARTIST);
        String oldLocation = mImageSourceFile.getImagePropertyString(PropertyKey.Exif.SUBJECT_LOCATION);
        String oldExposure = mImageSourceFile.getImagePropertyString(PropertyKey.Exif.EXPOSURE_TIME);

        /**
         * @tc.steps: step2. set properties for ImageSource.
         * @tc.expected: step2. no error
         */
        String newArtist = "testsetPropertyFilter002_" + String.valueOf(System.currentTimeMillis());
        int newLocation = (int)(System.currentTimeMillis() / 1.0E8);
        Double newExposure = Double.valueOf(newLocation + 0.1);
        long ret = -100;
        try {
            ret = mPropertyFilter.setPropertyString(PropertyKey.Exif.ARTIST, newArtist)
                .setPropertyInt(PropertyKey.Exif.SUBJECT_LOCATION, newLocation)
                .setPropertyDouble(PropertyKey.Exif.EXPOSURE_TIME, newExposure)
                .applyToSource(mImageSourceFile);
        } catch (IOException e) {
            LOGGER.error("file applyToSource exception");
            fail("file applyToSource exception");
        }

        /**
         * @tc.steps: step3. get properties of changed ImageSource.
         * @tc.expected: step3. equal to changed value.
         */
        assertEquals(newArtist, mImageSourceFile.getImagePropertyString(PropertyKey.Exif.ARTIST));
        assertEquals(String.valueOf(newLocation),
            mImageSourceFile.getImagePropertyString(PropertyKey.Exif.SUBJECT_LOCATION));
        assertEquals(String.valueOf(newExposure),
            mImageSourceFile.getImagePropertyString(PropertyKey.Exif.EXPOSURE_TIME));
        assertTrue(ret > 0);

        /**
         * @tc.steps: step4. get properties of changed reloaded ImageSource.
         * @tc.expected: step4. equal to changed value.
         */
        mImageSourceFile = ImageSource.create(new File(TEST_JPEG), null);
        assertEquals(newArtist, mImageSourceFile.getImagePropertyString(PropertyKey.Exif.ARTIST));
        assertEquals(String.valueOf(newLocation),
            mImageSourceFile.getImagePropertyString(PropertyKey.Exif.SUBJECT_LOCATION));
        assertEquals(String.valueOf(newExposure),
            mImageSourceFile.getImagePropertyString(PropertyKey.Exif.EXPOSURE_TIME));
    }

    /**
     * @tc.name: testsetPropertyFilter003
     * @tc.desc: reset property of FilePath ImageSource.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testsetPropertyFilter003() {
        /**
         * @tc.steps: step1. get current properties of ImageSource.
         */
        String oldArtist = mImageSourcePath.getImagePropertyString(PropertyKey.Exif.ARTIST);
        String oldLocation = mImageSourcePath.getImagePropertyString(PropertyKey.Exif.SUBJECT_LOCATION);
        String oldExposure = mImageSourcePath.getImagePropertyString(PropertyKey.Exif.EXPOSURE_TIME);

        /**
         * @tc.steps: step2. set properties for ImageSource.
         * @tc.expected: step2. no error
         */
        String newArtist = "testsetPropertyFilter003_" + String.valueOf(System.currentTimeMillis());
        int newLocation = (int)(System.currentTimeMillis() / 1.0E8);
        Double newExposure = Double.valueOf(newLocation + 0.1);
        long ret = -100;
        try {
            ret = mPropertyFilter.setPropertyString(PropertyKey.Exif.ARTIST, newArtist)
                .setPropertyInt(PropertyKey.Exif.SUBJECT_LOCATION, newLocation)
                .setPropertyDouble(PropertyKey.Exif.EXPOSURE_TIME, newExposure)
                .restore()
                .applyToSource(mImageSourcePath);
        } catch (IOException e) {
            LOGGER.error("file applyToSource exception");
            fail("file applyToSource exception");
        }

        /**
         * @tc.steps: step3. get properties of changed ImageSource.
         * @tc.expected: step3. equal to changed value.
         */
        assertEquals(oldArtist, mImageSourcePath.getImagePropertyString(PropertyKey.Exif.ARTIST));
        assertEquals(String.valueOf(oldLocation),
            mImageSourcePath.getImagePropertyString(PropertyKey.Exif.SUBJECT_LOCATION));
        assertEquals(String.valueOf(oldExposure),
            mImageSourcePath.getImagePropertyString(PropertyKey.Exif.EXPOSURE_TIME));
        assertEquals(-1, ret);

        /**
         * @tc.steps: step4. get properties of changed reloaded ImageSource.
         * @tc.expected: step4. equal to changed value.
         */
        mImageSourcePath = ImageSource.create(TEST_JPEG, null);
        assertEquals(oldArtist, mImageSourcePath.getImagePropertyString(PropertyKey.Exif.ARTIST));
        assertEquals(String.valueOf(oldLocation),
            mImageSourcePath.getImagePropertyString(PropertyKey.Exif.SUBJECT_LOCATION));
        assertEquals(String.valueOf(oldExposure),
            mImageSourcePath.getImagePropertyString(PropertyKey.Exif.EXPOSURE_TIME));
    }

    /**
     * @tc.name: testsetPropertyFilter004
     * @tc.desc: reset property of File ImageSource.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testsetPropertyFilter004() {
        /**
         * @tc.steps: step1. get current properties of ImageSource.
         */
        String oldArtist = mImageSourceFile.getImagePropertyString(PropertyKey.Exif.ARTIST);
        String oldLocation = mImageSourceFile.getImagePropertyString(PropertyKey.Exif.SUBJECT_LOCATION);
        String oldExposure = mImageSourceFile.getImagePropertyString(PropertyKey.Exif.EXPOSURE_TIME);

        /**
         * @tc.steps: step2. set properties for ImageSource.
         * @tc.expected: step2. no error
         */
        String newArtist = "testsetPropertyFilter004_" + String.valueOf(System.currentTimeMillis());
        int newLocation = (int)(System.currentTimeMillis() / 1.0E8);
        Double newExposure = Double.valueOf(newLocation + 0.1);
        long ret = -100;
        try {
            ret = mPropertyFilter.setPropertyString(PropertyKey.Exif.ARTIST, newArtist)
                .setPropertyInt(PropertyKey.Exif.SUBJECT_LOCATION, newLocation)
                .setPropertyDouble(PropertyKey.Exif.EXPOSURE_TIME, newExposure)
                .restore()
                .applyToSource(mImageSourceFile);
        } catch (IOException e) {
            LOGGER.error("file applyToSource exception");
            fail("file applyToSource exception");
        }

        /**
         * @tc.steps: step3. get properties of changed ImageSource.
         * @tc.expected: step3. equal to changed value.
         */
        assertEquals(oldArtist, mImageSourceFile.getImagePropertyString(PropertyKey.Exif.ARTIST));
        assertEquals(String.valueOf(oldLocation),
            mImageSourceFile.getImagePropertyString(PropertyKey.Exif.SUBJECT_LOCATION));
        assertEquals(String.valueOf(oldExposure),
            mImageSourceFile.getImagePropertyString(PropertyKey.Exif.EXPOSURE_TIME));
        assertEquals(-1, ret);

        /**
         * @tc.steps: step4. get properties of changed reloaded ImageSource.
         * @tc.expected: step4. equal to changed value.
         */
        mImageSourceFile = ImageSource.create(new File(TEST_JPEG), null);
        assertEquals(oldArtist, mImageSourceFile.getImagePropertyString(PropertyKey.Exif.ARTIST));
        assertEquals(String.valueOf(oldLocation),
            mImageSourceFile.getImagePropertyString(PropertyKey.Exif.SUBJECT_LOCATION));
        assertEquals(String.valueOf(oldExposure),
            mImageSourceFile.getImagePropertyString(PropertyKey.Exif.EXPOSURE_TIME));
    }

    /**
     * @tc.name: testsetPropertyFilter005
     * @tc.desc: remove partial property of filePath ImageSource.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testsetPropertyFilter005() {
        /**
         * @tc.steps: step1. get current properties of ImageSource.
         */
        String oldArtist = mImageSourcePath.getImagePropertyString(PropertyKey.Exif.ARTIST);
        String oldLocation = mImageSourcePath.getImagePropertyString(PropertyKey.Exif.SUBJECT_LOCATION);
        String oldExposure = mImageSourcePath.getImagePropertyString(PropertyKey.Exif.EXPOSURE_TIME);

        /**
         * @tc.steps: step2. set properties for ImageSource.
         * @tc.expected: step2. no error
         */
        String newArtist = "testsetPropertyFilter005_" + String.valueOf(System.currentTimeMillis());
        int newLocation = (int)(System.currentTimeMillis() / 1.0E8);
        Double newExposure = Double.valueOf(newLocation + 0.1);
        long ret = -100;
        try {
            ret = mPropertyFilter.setPropertyString(PropertyKey.Exif.ARTIST, newArtist)
                .setPropertyInt(PropertyKey.Exif.SUBJECT_LOCATION, newLocation)
                .setPropertyDouble(PropertyKey.Exif.EXPOSURE_TIME, newExposure)
                .rollbackProperty(PropertyKey.Exif.SUBJECT_LOCATION)
                .applyToSource(mImageSourcePath);
        } catch (IOException e) {
            LOGGER.error("file applyToSource exception");
            fail("file applyToSource exception");
        }

        /**
         * @tc.steps: step3. get properties of changed ImageSource.
         * @tc.expected: step3. equal to changed value.
         */
        assertEquals(newArtist, mImageSourcePath.getImagePropertyString(PropertyKey.Exif.ARTIST));
        assertEquals(String.valueOf(oldLocation),
            mImageSourcePath.getImagePropertyString(PropertyKey.Exif.SUBJECT_LOCATION));
        assertEquals(String.valueOf(newExposure),
            mImageSourcePath.getImagePropertyString(PropertyKey.Exif.EXPOSURE_TIME));
        assertTrue(ret > 0);

        /**
         * @tc.steps: step4. get properties of changed reloaded ImageSource.
         * @tc.expected: step4. equal to changed value.
         */
        mImageSourcePath = ImageSource.create(TEST_JPEG, null);
        assertEquals(newArtist, mImageSourcePath.getImagePropertyString(PropertyKey.Exif.ARTIST));
        assertEquals(String.valueOf(oldLocation),
            mImageSourcePath.getImagePropertyString(PropertyKey.Exif.SUBJECT_LOCATION));
        assertEquals(String.valueOf(newExposure),
            mImageSourcePath.getImagePropertyString(PropertyKey.Exif.EXPOSURE_TIME));
    }

    /**
     * @tc.name: testsetPropertyFilter006
     * @tc.desc: remove partial property of File ImageSource.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testsetPropertyFilter006() {
        /**
         * @tc.steps: step1. get current properties of ImageSource.
         */
        String oldArtist = mImageSourceFile.getImagePropertyString(PropertyKey.Exif.ARTIST);
        String oldLocation = mImageSourceFile.getImagePropertyString(PropertyKey.Exif.SUBJECT_LOCATION);
        String oldExposure = mImageSourceFile.getImagePropertyString(PropertyKey.Exif.EXPOSURE_TIME);

        /**
         * @tc.steps: step2. set properties for ImageSource.
         * @tc.expected: step2. no error
         */
        String newArtist = "testsetPropertyFilter006_" + String.valueOf(System.currentTimeMillis());
        int newLocation = (int)(System.currentTimeMillis() / 1.0E8);
        Double newExposure = Double.valueOf(newLocation + 0.1);
        long ret = -100;
        try {
            ret = mPropertyFilter.setPropertyString(PropertyKey.Exif.ARTIST, newArtist)
                .setPropertyInt(PropertyKey.Exif.SUBJECT_LOCATION, newLocation)
                .setPropertyDouble(PropertyKey.Exif.EXPOSURE_TIME, newExposure)
                .rollbackProperty(PropertyKey.Exif.SUBJECT_LOCATION)
                .applyToSource(mImageSourceFile);
        } catch (IOException e) {
            LOGGER.error("file applyToSource exception");
            fail("file applyToSource exception");
        }

        /**
         * @tc.steps: step3. get properties of changed ImageSource.
         * @tc.expected: step3. equal to changed value.
         */
        assertEquals(newArtist, mImageSourceFile.getImagePropertyString(PropertyKey.Exif.ARTIST));
        assertEquals(String.valueOf(oldLocation),
            mImageSourceFile.getImagePropertyString(PropertyKey.Exif.SUBJECT_LOCATION));
        assertEquals(String.valueOf(newExposure),
            mImageSourceFile.getImagePropertyString(PropertyKey.Exif.EXPOSURE_TIME));
        assertTrue(ret > 0);

        /**
         * @tc.steps: step4. get properties of changed reloaded ImageSource.
         * @tc.expected: step4. equal to changed value.
         */
        mImageSourceFile = ImageSource.create(new File(TEST_JPEG), null);
        assertEquals(newArtist, mImageSourceFile.getImagePropertyString(PropertyKey.Exif.ARTIST));
        assertEquals(String.valueOf(oldLocation),
            mImageSourceFile.getImagePropertyString(PropertyKey.Exif.SUBJECT_LOCATION));
        assertEquals(String.valueOf(newExposure),
            mImageSourceFile.getImagePropertyString(PropertyKey.Exif.EXPOSURE_TIME));
    }

    /**
     * @tc.name: testsetPropertyFilter007
     * @tc.desc: set property of filePath ImageSource and test invalid key.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testsetPropertyFilter007() {
        /**
         * @tc.steps: step1. get current properties of ImageSource.
         */
        String oldArtist = mImageSourcePath.getImagePropertyString(PropertyKey.Exif.ARTIST);
        String oldLocation = mImageSourcePath.getImagePropertyString(PropertyKey.Exif.SUBJECT_LOCATION);
        String oldExposure = mImageSourcePath.getImagePropertyString(PropertyKey.Exif.EXPOSURE_TIME);

        /**
         * @tc.steps: step2. set properties for ImageSource.
         * @tc.expected: step2. no error
         */
        String newArtist = "testsetPropertyFilter007_" + String.valueOf(System.currentTimeMillis());
        int newLocation = (int)(System.currentTimeMillis() / 1.0E8);
        Double newExposure = Double.valueOf(newLocation + 0.1);
        long ret = -100;
        try {
            ret = mPropertyFilter.setPropertyString(PropertyKey.Exif.ARTIST, newArtist)
                .setPropertyInt(null, newLocation)
                .setPropertyDouble("invalid_key_test", newExposure)
                .rollbackProperty(null)
                .rollbackProperty("invalid_key_test_rollback")
                .restore()
                .setPropertyInt(PropertyKey.Exif.SUBJECT_LOCATION, newLocation)
                .applyToSource(mImageSourcePath);
        } catch (IOException e) {
            LOGGER.error("file applyToSource exception");
            fail("file applyToSource exception");
        }

        /**
         * @tc.steps: step3. get properties of changed ImageSource.
         * @tc.expected: step3. equal to changed value.
         */
        assertEquals(oldArtist, mImageSourcePath.getImagePropertyString(PropertyKey.Exif.ARTIST));
        assertNotEquals(String.valueOf(newExposure),
            mImageSourcePath.getImagePropertyString("invalid_key_test"));
        assertEquals(String.valueOf(newLocation),
            mImageSourceFile.getImagePropertyString(PropertyKey.Exif.SUBJECT_LOCATION));
        assertTrue(ret > 0);

        /**
         * @tc.steps: step4. get properties of changed reloaded ImageSource.
         * @tc.expected: step4. equal to changed value.
         */
        mImageSourcePath = ImageSource.create(TEST_JPEG, null);
        assertEquals(oldArtist, mImageSourcePath.getImagePropertyString(PropertyKey.Exif.ARTIST));
        assertNotEquals(String.valueOf(newExposure),
            mImageSourcePath.getImagePropertyString("invalid_key_test"));
        assertEquals(String.valueOf(newLocation),
            mImageSourceFile.getImagePropertyString(PropertyKey.Exif.SUBJECT_LOCATION));
    }

    /**
     * @tc.name: testsetPropertyFilter008
     * @tc.desc: set property of filePath ImageSource and test invalid imagesource.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testsetPropertyFilter008() {
        /**
         * @tc.steps: step1. set properties for released ImageSource.
         * @tc.expected: step1. no Exception and return error code.
         */
        String newArtist = "testsetPropertyFilter008_" + String.valueOf(System.currentTimeMillis());
        int newLocation = (int)(System.currentTimeMillis() / 1.0E8);
        Double newExposure = Double.valueOf(newLocation + 0.1);
        long ret = -100;
        mImageSourcePath.release();

        assertThrows("should throw IllegalStateException", IllegalStateException.class, () -> {
            mPropertyFilter.setPropertyString(PropertyKey.Exif.ARTIST, newArtist)
                .setPropertyInt(PropertyKey.Exif.SUBJECT_LOCATION, newLocation)
                .applyToSource(mImageSourcePath);
            });
    }

    /**
     * @tc.name: testsetPropertyFilter009
     * @tc.desc: set property of filePath ImageSource and test invalid imagesource.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testsetPropertyFilter009() {
        /**
         * @tc.steps: step1. set properties for null.
         * @tc.expected: step1. no Exception and return error code
         */
        String newArtist = "testsetPropertyFilter009_" + String.valueOf(System.currentTimeMillis());
        int newLocation = (int)(System.currentTimeMillis() / 1.0E8);
        Double newExposure = Double.valueOf(newLocation + 0.1);
        long ret = -100;

        assertThrows("should throw IllegalArgumentException", IllegalArgumentException.class, () -> {
            mPropertyFilter.setPropertyString(PropertyKey.Exif.ARTIST, newArtist)
                .setPropertyInt(PropertyKey.Exif.SUBJECT_LOCATION, newLocation)
                .applyToSource(null);
            });
    }

    /**
     * @tc.name: testsetPropertyFilter010
     * @tc.desc: set property of unsupported ImageSource.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testsetPropertyFilter010() {
        InputStream inputStream = null;
        try {
            /**
             * @tc.steps: step1. construct unsupported ImageSource of inputStream source.
             */
            File file = new File(TEST_JPEG);
            inputStream = new FileInputStream(file);
            ImageSource imageSourceStream = ImageSource.create(inputStream, null);
            assertNotNull(imageSourceStream);

            /**
             * @tc.steps: step2. set properties for unsupported ImageSource.
             * @tc.expected: step2. no error
             */
            String newArtist = "testsetPropertyFilter010_" + String.valueOf(System.currentTimeMillis());
            int newLocation = (int)(System.currentTimeMillis() / 1.0E8);
            Double newExposure = Double.valueOf(newLocation + 0.1);

            assertThrows("should throw IllegalStateException", IllegalStateException.class, () -> {
                mPropertyFilter.setPropertyString(PropertyKey.Exif.ARTIST, newArtist)
                    .setPropertyInt(PropertyKey.Exif.SUBJECT_LOCATION, newLocation)
                    .applyToSource(imageSourceStream);
            });
        } catch (FileNotFoundException e) {
            LOGGER.error("FileNotFoundException in testsetPropertyFilter010");
            fail("FileNotFoundException in testsetPropertyFilter010");
        } finally {
            try {
                inputStream.close();
            } catch (IOException e) {
                fail("inputStream.close failed in testsetPropertyFilter010");
            }
        }
    }

    /**
     * @tc.name: testsetPropertyFilter011
     * @tc.desc: set property of unsupported ImageSource.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testsetPropertyFilter011() {
        /**
         * @tc.steps: step1. construct unsupported ImageSource of byte buffer source.
         */
        ImageSource imageSourceByte = getByteImageSource();
        assertNotNull(imageSourceByte);

        /**
         * @tc.steps: step2. set properties for unsupported ImageSource.
         * @tc.expected: step2. no error
         */
        String newArtist = "testsetPropertyFilter011_" + String.valueOf(System.currentTimeMillis());
        int newLocation = (int)(System.currentTimeMillis() / 1.0E8);
        Double newExposure = Double.valueOf(newLocation + 0.1);

        assertThrows("should throw IOException", IOException.class, () -> {
            mPropertyFilter.setPropertyString(PropertyKey.Exif.ARTIST, newArtist)
                .setPropertyInt(PropertyKey.Exif.SUBJECT_LOCATION, newLocation)
                .applyToSource(imageSourceByte);
        });
    }

    /**
     * @tc.name: testsetPropertyFilter012
     * @tc.desc: set property of null key.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testsetPropertyFilter012() {
        /**
         * @tc.steps: step1. get current properties of ImageSource.
         */
        String oldArtist = mImageSourcePath.getImagePropertyString(PropertyKey.Exif.ARTIST);
        String oldLocation = mImageSourcePath.getImagePropertyString(PropertyKey.Exif.SUBJECT_LOCATION);
        String oldExposure = mImageSourcePath.getImagePropertyString(PropertyKey.Exif.EXPOSURE_TIME);

        /**
         * @tc.steps: step2. set properties for ImageSource.
         * @tc.expected: step2. no error
         */
        String newArtist = "testsetPropertyFilter012_" + String.valueOf(System.currentTimeMillis());
        int newLocation = (int)(System.currentTimeMillis() / 1.0E8);
        Double newExposure = Double.valueOf(newLocation + 0.1);
        long ret = -100;
        try {
            ret = mPropertyFilter.setPropertyString(null, newArtist)
                .applyToSource(mImageSourcePath);
        } catch (IOException e) {
            LOGGER.error("file applyToSource exception");
            fail("file applyToSource exception");
        }

        /**
         * @tc.steps: step3. get properties of changed ImageSource.
         * @tc.expected: step3. equal to changed value.
         */
        assertEquals(oldArtist, mImageSourcePath.getImagePropertyString(PropertyKey.Exif.ARTIST));
        assertEquals(-1, ret);
        mImageSourcePath.release();
    }

    /**
     * @tc.name: testsetPropertyFilter013
     * @tc.desc: set property of null key and rollbackProperty.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testsetPropertyFilter013() {
        /**
         * @tc.steps: step1. get current properties of ImageSource.
         */
        String oldArtist = mImageSourcePath.getImagePropertyString(PropertyKey.Exif.ARTIST);
        String oldLocation = mImageSourcePath.getImagePropertyString(PropertyKey.Exif.SUBJECT_LOCATION);
        String oldExposure = mImageSourcePath.getImagePropertyString(PropertyKey.Exif.EXPOSURE_TIME);

        /**
         * @tc.steps: step2. set properties for ImageSource.
         * @tc.expected: step2. no error
         */
        String newArtist = "testsetPropertyFilter013_" + String.valueOf(System.currentTimeMillis());
        int newLocation = (int)(System.currentTimeMillis() / 1.0E8);
        Double newExposure = Double.valueOf(newLocation + 0.1);
        long ret = -100;
        try {
            ret = mPropertyFilter.setPropertyString(null, newArtist)
                .rollbackProperty(null)
                .applyToSource(mImageSourcePath);
        } catch (IOException e) {
            LOGGER.error("file applyToSource exception");
            fail("file applyToSource exception");
        }

        /**
         * @tc.steps: step3. get properties of changed ImageSource.
         * @tc.expected: step3. equal to changed value.
         */
        assertTrue(ret > 0);
        mImageSourcePath.release();
    }

    /**
     * @tc.number testsetPropertyFilter014
     * @tc.name string exif interface, two parameters
     * @tc.desc Edit the pic exif info.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testsetPropertyFilter014() {
        LOGGER.info("testsetPropertyFilter014 begin");
        boolean exceptionFlag = false;

        /**
         * @tc.steps: step1. set properties for ImageSource.
         * @tc.expected: step1. the properties is correct.
         */
        String oldValue = mImageSourcePath.getImagePropertyString(PropertyKey.Exif.MAKE);
        long ret = -100;
        try {
            ret = mPropertyFilter.setPropertyString("", "").applyToSource(mImageSourcePath);
        } catch (IOException e) {
            exceptionFlag = true;
            LOGGER.error("file applyToSource exception");
            fail("file applyToSource exception");
        }

        /**
         * @tc.steps: step2. get properties of changed ImageSource.
         * @tc.expected: step2. equal to changed value.
         */
        assertEquals(oldValue, mImageSourcePath.getImagePropertyString(PropertyKey.Exif.MAKE));
        assertEquals(-1, ret);
        assertFalse(exceptionFlag);

        mImageSourcePath.release();

        /**
         * @tc.steps: step3. get properties of changed reloaded ImageSource.
         * @tc.expected: step3. equal to changed value.
         */
        ImageSource newImageSource = ImageSource.create(TEST_JPEG, null);
        assertEquals(oldValue, newImageSource.getImagePropertyString(PropertyKey.Exif.MAKE));

        newImageSource.release();
        LOGGER.info("testsetPropertyFilter014 end");
    }

    /**
     * @tc.number testsetPropertyFilter015
     * @tc.name the image does not contain exif
     * @tc.desc Edite the pic exif info.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testsetPropertyFilter015() {
        LOGGER.info("testsetPropertyFilter015 begin");
        boolean exceptionFlag = false;

        /**
         * @tc.steps: step1. set properties for ImageSource.
         * @tc.expected: step1. the properties is correct.
         */
        ImageSource noExifImageSource = ImageSource.create(HAS_NO_EXIF, null);
        assertNotNull(noExifImageSource);
        mPropertyFilter = new PropertyFilter();

        String newStringValue = String.valueOf(System.currentTimeMillis());
        int newIntValue = (int) (System.currentTimeMillis() / 1.0E8);
        Double newDoubleValue = 2.5;
        LOGGER.error("file newIntValue %{public}d", newIntValue);

        long ret = -100;
        try {
            ret = mPropertyFilter.setPropertyString(PropertyKey.Exif.MAKE, newStringValue)
                .setPropertyDouble(PropertyKey.Exif.F_NUMBER, newDoubleValue)
                .setPropertyInt(PropertyKey.Exif.EXIF_VERSION, newIntValue)
                .applyToSource(mImageSourcePath);
        } catch (IOException e) {
            exceptionFlag = true;
            LOGGER.error("file applyToSource exception");
            fail("file applyToSource exception");
        }

        /**
         * @tc.steps: step2. get properties of changed ImageSource.
         * @tc.expected: step2. equal to changed value.
         */
        assertEquals(null, noExifImageSource.getImagePropertyString(PropertyKey.Exif.MAKE));
        assertEquals(0, noExifImageSource.getImagePropertyInt(PropertyKey.Exif.EXIF_VERSION, 0));
        assertEquals(null, noExifImageSource.getImagePropertyString(PropertyKey.Exif.F_NUMBER));
        assertFalse(exceptionFlag);

        noExifImageSource.release();

        /**
         * @tc.steps: step3. get properties of changed reloaded ImageSource.
         * @tc.expected: step3. equal to changed value.
         */
        ImageSource newImageSource = ImageSource.create(HAS_NO_EXIF, null);

        assertEquals(null, newImageSource.getImagePropertyString(PropertyKey.Exif.MAKE));
        assertEquals(0, newImageSource.getImagePropertyInt(PropertyKey.Exif.EXIF_VERSION, 0));
        assertEquals(null, newImageSource.getImagePropertyString(PropertyKey.Exif.F_NUMBER));

        newImageSource.release();
        LOGGER.info("testsetPropertyFilter015 end");
    }

    /**
     * @tc.number testsetPropertyFilter016
     * @tc.name get exif failure after image source released
     * @tc.desc Edite the pic exif info.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testsetPropertyFilter016() {
        LOGGER.info("testsetPropertyFilter016 begin");
        boolean exceptionFlag = false;

        /**
         * @tc.steps: step1. set properties for released ImageSource.
         * @tc.expected: step1. no Exception and return error code.
         */
        String newArtist = "testEditingPictureExif040_" + String.valueOf(System.currentTimeMillis());
        int newLocation = (int) (System.currentTimeMillis() / 1.0E8);
        long ret = -100;
        mImageSourcePath.release();

        try {
            ret = mPropertyFilter.setPropertyString(PropertyKey.Exif.ARTIST, newArtist)
                .setPropertyInt(PropertyKey.Exif.SUBJECT_LOCATION, newLocation)
                .applyToSource(mImageSourcePath);
        } catch (IOException e) {
            LOGGER.error("file applyToSource exception");
            fail("file applyToSource exception");
        } catch (IllegalStateException e) {
            exceptionFlag = true;
            LOGGER.error("file applyToSource exception");
        }

        /**
         * @tc.steps: step2. get result of applyToSource null.
         * @tc.expected: step2. equal to error code.
         */
        assertTrue(exceptionFlag);

        LOGGER.info("testsetPropertyFilter016 end");
    }

    private ImageSource getByteImageSource() {
        byte[] data = null;
        try {
            data = Files.readAllBytes(Paths.get(TEST_JPEG));
        } catch (IOException e) {
            LOGGER.error("file readAllBytes get exception");
            fail("file readAllBytes exception");
        }
        data[0] = 0xa;
        return ImageSource.create(data, null);
    }

}
