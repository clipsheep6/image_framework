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

import static org.easymock.EasyMock.expect;
import static org.easymock.EasyMock.replay;
import static org.easymock.EasyMock.reset;
import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertFalse;
import static org.junit.Assert.assertNotEquals;
import static org.junit.Assert.assertNotNull;
import static org.junit.Assert.assertNull;
import static org.junit.Assert.assertThrows;
import static org.junit.Assert.assertTrue;
import static org.junit.Assert.fail;

import ohos.app.Context;
import ohos.eventhandler.EventHandler;
import ohos.eventhandler.EventRunner;
import ohos.media.image.common.DecodeEvent;
import ohos.media.image.common.ImageFormat;
import ohos.media.image.common.ImageInfo;
import ohos.media.image.common.MemoryUsagePreference;
import ohos.media.image.common.PixelFormat;
import ohos.media.image.common.PropertyKey;
import ohos.media.image.common.Rect;
import ohos.media.image.common.Size;
import ohos.media.utils.PerformanceUtil;
import ohos.media.utils.PrivateMemberAccess;
import ohos.media.utils.log.Logger;
import ohos.media.utils.log.LoggerFactory;
import ohos.unittest.CaseLevel;
import ohos.unittest.CaseType;
import ohos.unittest.Level;
import ohos.unittest.Type;
import ohos.unittest.perf.BaseLine;
import ohos.unittest.perf.PerfVerify;
import ohos.unittest.TestTarget;

import org.easymock.EasyMockSupport;
import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;
import org.junit.Ignore;

import java.nio.ByteBuffer;
import java.nio.channels.FileChannel;
import java.io.BufferedReader;
import java.io.File;
import java.io.FileDescriptor;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.FileReader;
import java.io.IOException;
import java.io.OutputStream;
import java.nio.IntBuffer;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.util.Arrays;
import java.util.ArrayList;
import java.util.Random;
import java.util.concurrent.CountDownLatch;
import java.util.List;
import java.util.Objects;
import java.util.concurrent.TimeUnit;
import java.util.stream.Collectors;
/**
 * The performance test for Image
 *
 * @since 3
 */
public class ImagePerformanceTest extends EasyMockSupport {
    private static final Logger LOGGER = LoggerFactory.getImageLogger(ImagePerformanceTest.class);

    private static final String PNG_SRC_FILE_PATH = "/data/test/test.png";

    private static final String PNG_NINE_PATCH_SRC_FILE_PATH = "/data/test/test.9.png";

    private static final String JPG_PACKED_FILE_PATH = "/data/test/test_packer.jpg";

    private static final String JPG_SRC_FILE_PATH = "/data/test/test.jpg";

    private static final String JPG_THUMBNAIL_SRC_FILE_PATH = "/data/test/test_exif.jpg";

    private static final String DNG_SRC_FILE_PATH = "/data/test/test.dng";

    private static final String COLORS_DATA = "/data/test/colors.txt";

    private static final String TAG = "ImagePerformanceTest";

    private static final int REPEAT_TIMES = 1000;

    private static final int TEST_NUM_FOR_SIMPLE_CASE = 1000;

    private static final int TEST_NUM_FOR_COMPLICATED_CASE = 10;

    private static final int COOL_DOWN_TIME_CASE = 1000;

    private static String BASE_LINE_XML_NAME = "ImagePerformanceTest_baseline.xml";

    private static BaseLine baseLine;

    private Context mockContext;

    private DecodeEvent partialImageListenerTest;

    /**
     * Init baseline
     */
    @BeforeClass
    public static void beforeClass() {
        String baseLineConfigFile = "/data/test/" + BASE_LINE_XML_NAME;
        baseLine = new BaseLine(baseLineConfigFile);
    }

    /**
     * Clear baseline
     */
    @AfterClass
    public static void afterClass() {
        BASE_LINE_XML_NAME = null;
        baseLine = null;
    }

    /**
     * Set up for each case
     *
     * @throws Exception Any exception
     */
    @Before
    public void setUp() throws Exception {
        mockContext = mock(Context.class);
    }

    /**
     * Tear down for each case
     *
     * @throws Exception Any exception
     */
    @After
    public void tearDown() throws Exception {
        mockContext = null;
    }

    private void coolDown4Case() {
        coolDown(COOL_DOWN_TIME_CASE);
    }

    private void coolDown(int time) {
        try {
            Thread.sleep(time);
        } catch (InterruptedException e) {
            LOGGER.warn("coolDown exception", e);
        }
    }

    
    private PixelMap createSrcPixelMap(int[] myColors) {
        LOGGER.debug("myColors length: %{public}d", myColors.length);
        PixelMap.InitializationOptions initializationOptions = new PixelMap.InitializationOptions();
        initializationOptions.size = new Size(300, 300);
        initializationOptions.pixelFormat = PixelFormat.RGBA_8888;
        PixelMap pixelMap = PixelMap.create(myColors, initializationOptions);
        assertNotNull(pixelMap);
        return pixelMap;
    }

    private int[] constructColors() {
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

    private <T> void recordAndReset(List<Long> timeList, List<T> cacheList, String label) {
        Objects.requireNonNull(timeList);
        Objects.requireNonNull(cacheList);
        long middleTime = timeList.stream().sorted().collect(Collectors.toList()).get(timeList.size() / 2);
        LOGGER.warn(label + "===>" + middleTime);
        timeList.clear();
        cacheList.stream().forEach(obj -> {
            if (obj instanceof PixelMap) {
                ((PixelMap) obj).release();
            }
        });
        cacheList.clear();
        try {
            TimeUnit.SECONDS.sleep(2);
        } catch (InterruptedException e) {
            LOGGER.warn("resetAndSleeping occurs Exception");
        }
    }

    private <T> void recordAndResetOnlyTime(List<Long> timeList, String label) {
        Objects.requireNonNull(timeList);
        long middleTime = timeList.stream().sorted().collect(Collectors.toList()).get(timeList.size() / 2);
        LOGGER.warn(label + "===>" + middleTime);
        timeList.clear();
        try {
            TimeUnit.SECONDS.sleep(2);
        } catch (InterruptedException e) {
            LOGGER.warn("recordAndResetOnlyTime occurs Exception");
        }
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
        decodingOpts.desiredPixelFormat = PixelFormat.ARGB_8888;
        return imageSource.createPixelmap(decodingOpts);
    }

    /**
     * Perf test for getInnerJpegThumbnail.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.PERF)
    @TestTarget(className = "ohos.media.image.ImageSource", methodName = "public byte[] getInnerJpegThumbnail()")
    public void testImageSource_getInnerJpegThumbnail_001() {
        PerfVerify verify = new PerfVerify(baseLine);
        List<Long> times = new ArrayList<>();

        String imagePath = JPG_THUMBNAIL_SRC_FILE_PATH;
        ImageSource.SourceOptions srcOpts = new ImageSource.SourceOptions();
        srcOpts.formatHint = "image/jpeg";
        for (int index = 0; index < REPEAT_TIMES; index++) {
            ImageSource imageSource = ImageSource.create(imagePath, srcOpts);
            imageSource.getInnerThumbnailScope();
            long startTime = System.nanoTime();
            byte[] bytes = imageSource.getInnerJpegThumbnail();
            long endTime = System.nanoTime();
            times.add(endTime - startTime);
            if (bytes == null) {
                fail("no thumbnail exits");
            }
            imageSource.release();
        }
        verify.expectSmaller(PerformanceUtil.averageDoubleUsec(times), 1);
    }

    /**
     * Perf test for getInnerThumbnailScope.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.PERF)
    @TestTarget(className = "ohos.media.image.ImageSource", methodName = "public long[] getInnerThumbnailScope()")
    public void testImageSource_getInnerThumbnailScope_001() {
        PerfVerify verify = new PerfVerify(baseLine);
        List<Long> times = new ArrayList<>();

        String imagePath = JPG_THUMBNAIL_SRC_FILE_PATH;
        ImageSource.SourceOptions srcOpts = new ImageSource.SourceOptions();
        srcOpts.formatHint = "image/jpeg";
        for (int index = 0; index < REPEAT_TIMES; index++) {
            ImageSource imageSource = ImageSource.create(imagePath, srcOpts);
            imageSource.getThumbnailFormat();
            long startTime = System.nanoTime();
            long[] scope = imageSource.getInnerThumbnailScope();
            long endTime = System.nanoTime();
            times.add(endTime - startTime);
            if (scope == null) {
                fail("no thumbnail exits");
            }
            imageSource.release();
        }
        verify.expectSmaller(PerformanceUtil.averageDoubleUsec(times), 1);
    }

    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.PERF)
    @TestTarget(className = "ohos.media.image.ImageSource",
        methodName = "public static ImageSource create(String, ImageSource.SourceOptions)")
    public void testImageSource_create_001() {
        PerfVerify verify = new PerfVerify(baseLine);
        List<Long> times = new ArrayList<>();
        for (int index = 0; index < REPEAT_TIMES; index++) {
            long startTime = System.nanoTime();
            ImageSource imageSource = ImageSource.create(JPG_SRC_FILE_PATH, null);
            assertNotNull(imageSource);
            long endTime = System.nanoTime();
            times.add(endTime - startTime);
            imageSource.release();
        }
        verify.expectSmaller(PerformanceUtil.middleDoubleUsec(times), 1);
    }

    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.PERF)
    @TestTarget(className = "ohos.media.image.ImageSource",
        methodName = "public PixelMap createPixelmap(ImageSource.DecodingOptions)")
    public void testImageSource_createPixelmap_001() {
        PerfVerify verify = new PerfVerify(baseLine);
        List<Long> times = new ArrayList<>();
        for (int index = 0; index < REPEAT_TIMES; index++) {
            ImageSource imageSource = ImageSource.create(JPG_SRC_FILE_PATH, null);
            assertNotNull(imageSource);
            long startTime = System.nanoTime();
            ImageSource.DecodingOptions decodingOpts = new ImageSource.DecodingOptions();
            PixelMap pixelMap = imageSource.createPixelmap(decodingOpts);
            long endTime = System.nanoTime();
            times.add(endTime - startTime);
            imageSource.release();
            pixelMap.release();
        }
        verify.expectSmaller(PerformanceUtil.middleDoubleUsec(times), 1);
    }

    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.PERF)
    @TestTarget(className = "ohos.media.image.ImageSource",
        methodName = "public static ImageSource create(ByteBuffer, ImageSource.SourceOptions)")
    public void testImageSource_create_002() {
        PerfVerify verify = new PerfVerify(baseLine);
        List<Long> times = new ArrayList<>();
        for (int index = 0; index < REPEAT_TIMES; index++) {
            String pathName = JPG_SRC_FILE_PATH;
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

            long startTime = System.nanoTime();
            ImageSource imageSource = ImageSource.create(byteBuffer, null);
            long endTime = System.nanoTime();
            times.add(endTime - startTime);

            assertNotNull(imageSource);
            imageSource.release();
        }
        verify.expectSmaller(PerformanceUtil.middleDoubleUsec(times), 1);
    }

    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.PERF)
    @TestTarget(className = "ohos.media.image.ImageSource",
        methodName = "public static ImageSource create(File, ImageSource.SourceOptions)")
    public void testImageSource_create_003() {
        PerfVerify verify = new PerfVerify(baseLine);
        List<Long> times = new ArrayList<>();
        for (int index = 0; index < REPEAT_TIMES; index++) {
            String pathName = JPG_SRC_FILE_PATH;
            File file = new File(pathName);
            ImageSource.SourceOptions srcOpts = new ImageSource.SourceOptions();

            long startTime = System.nanoTime();
            // need test this function
            ImageSource imageSource = ImageSource.create(file, srcOpts);
            long endTime = System.nanoTime();
            times.add(endTime - startTime);

            assertNotNull(imageSource);
            imageSource.release();
        }
        verify.expectSmaller(PerformanceUtil.middleDoubleUsec(times), 1);
    }


    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.PERF)
    @TestTarget(className = "ohos.media.image.ImageSource",
        methodName = "public void setMemoryUsagePreference(MemoryUsagePreference)")
    public void testImageSource_setMemoryUsagePreference_001() {
        PerfVerify verify = new PerfVerify(baseLine);
        List<Long> times = new ArrayList<>();
        for (int index = 0; index < REPEAT_TIMES; index++) {
            ImageSource imageSource = ImageSource.create(JPG_SRC_FILE_PATH, null);
            assertNotNull(imageSource);

            long startTime = System.nanoTime();
            imageSource.setMemoryUsagePreference(MemoryUsagePreference.LOW_RAM);
            long endTime = System.nanoTime();
            times.add(endTime - startTime);

            ImageSource.DecodingOptions decodingOpts = new ImageSource.DecodingOptions();
            decodingOpts.editable = true;
            PixelMap pixelMap = imageSource.createPixelmap(decodingOpts);
            imageSource.release();
            pixelMap.release();
        }
        verify.expectSmaller(PerformanceUtil.middleDoubleUsec(times), 1);
    }

    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.PERF)
    @Ignore
    public void testImageSource_createPixelmap_002() {
        PerfVerify verify = new PerfVerify(baseLine);
        List<Long> times = new ArrayList<>();
        for (int index = 0; index < REPEAT_TIMES; index++) {
            ImageSource imageSource = ImageSource.create(JPG_SRC_FILE_PATH, null);
            assertNotNull(imageSource);
            imageSource.setMemoryUsagePreference(MemoryUsagePreference.LOW_RAM);
            ImageSource.DecodingOptions decodingOpts = new ImageSource.DecodingOptions();
            decodingOpts.editable = true;

            long startTime = System.nanoTime();
            PixelMap pixelMap = imageSource.createPixelmap(decodingOpts);
            long endTime = System.nanoTime();
            times.add(endTime - startTime);

            imageSource.release();
            pixelMap.release();
        }
        verify.expectSmaller(PerformanceUtil.middleDoubleUsec(times), 1);
    }

    /**
     * calculate dng file decode time.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.PERF)
    @Ignore
    public void testImageSource_createPixelmap_003() {
        PerfVerify verify = new PerfVerify(baseLine);
        List<Long> times = new ArrayList<>();
        for (int index = 0; index < REPEAT_TIMES / 100; index++) {
            ImageSource imageSource = ImageSource.create(DNG_SRC_FILE_PATH, null);
            assertNotNull(imageSource);
            long startTime = System.nanoTime();
            ImageSource.DecodingOptions decodingOpts = new ImageSource.DecodingOptions();
            PixelMap pixelMap = imageSource.createPixelmap(decodingOpts);
            long endTime = System.nanoTime();
            times.add(endTime - startTime);
            imageSource.release();
            pixelMap.release();
        }
        verify.expectSmaller(PerformanceUtil.middleDoubleUsec(times), 1);
    }

    class PartialImageListenerTest implements ImageSource.DecodeEventListener {
        @Override
        public void onDecodeEvent(ImageSource source, DecodeEvent event) {
            partialImageListenerTest = event;
            LOGGER.debug("event " + event.getValue());
        }
    }

    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.PERF)
    @TestTarget(className = "ohos.media.image.ImageSource",
        methodName = "public void setDecodeEventListener(ImageSource.DecodeEventListener)")
    public void testImageSource_setOnDecodeEventListener_001() {
        PerfVerify verify = new PerfVerify(baseLine);
        List<Long> times = new ArrayList<>();
        for (int index = 0; index < REPEAT_TIMES; index++) {
            ImageSource imageSource = ImageSource.create(JPG_SRC_FILE_PATH, null);
            assertNotNull(imageSource);
            ImageSource.DecodeEventListener testListener = new PartialImageListenerTest();

            long startTime = System.nanoTime();
            imageSource.setDecodeEventListener(testListener);
            long endTime = System.nanoTime();
            times.add(endTime - startTime);

            ImageSource.DecodingOptions decodingOpts = new ImageSource.DecodingOptions();
            decodingOpts.editable = true;
            PixelMap pixelMap = imageSource.createPixelmap(decodingOpts);
            imageSource.release();
            pixelMap.release();
        }
        verify.expectSmaller(PerformanceUtil.middleDoubleUsec(times), 1);
    }

    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.PERF)
    @TestTarget(className = "ohos.media.image.PropertyFilter",
        methodName = "public long applyToSource(ImageSource)")
    public void testPropertyFilter_applyToSource_001() {
        PerfVerify verify = new PerfVerify(baseLine);
        List<Long> times = new ArrayList<>();
        for (int index = 0; index < REPEAT_TIMES; index++) {
            ImageSource imageSource = ImageSource.create(JPG_SRC_FILE_PATH, null);
            assertNotNull(imageSource);
            String newArtist = "testImagePropertyFilterTest001_" + String.valueOf(System.currentTimeMillis());
            long ret = -100;
            PropertyFilter mPropertyFilter = new PropertyFilter();
            mPropertyFilter.setPropertyString(PropertyKey.Exif.ARTIST, newArtist);
            long startTime = System.nanoTime();
            try {
                ret = mPropertyFilter.applyToSource(imageSource);
            } catch (IOException e) {
                LOGGER.error("file applyToSource exception");
                fail("file applyToSource exception");
            }
            long endTime = System.nanoTime();
            times.add(endTime - startTime);

            assertEquals(newArtist, imageSource.getImagePropertyString(PropertyKey.Exif.ARTIST));
            assertTrue(ret > 0);
            imageSource.release();
        }
        verify.expectSmaller(PerformanceUtil.middleDoubleUsec(times), 1);
    }


    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.PERF)
    @TestTarget(className = "ohos.media.image.PropertyFilter",
        methodName = "public PropertyFilter setPropertyDouble(String, double)")
    public void testPropertyFilter_setPropertyDouble_001() {
        PerfVerify verify = new PerfVerify(baseLine);
        List<Long> times = new ArrayList<>();
        for (int index = 0; index < REPEAT_TIMES; index++) {
            ImageSource imageSource = ImageSource.create(JPG_SRC_FILE_PATH, null);
            assertNotNull(imageSource);
            String oldLocation = imageSource.getImagePropertyString(PropertyKey.Exif.SUBJECT_LOCATION);
            int newLocation = (int)(System.currentTimeMillis()/1.0E8);
            Double newExposure = Double.valueOf(newLocation + 0.1);
            long ret = -100;
            PropertyFilter mPropertyFilter = new PropertyFilter();

            long startTime = System.nanoTime();
            mPropertyFilter.setPropertyDouble(PropertyKey.Exif.EXPOSURE_TIME, newExposure);
            long endTime = System.nanoTime();
            times.add(endTime - startTime);
            try {
                ret = mPropertyFilter.applyToSource(imageSource);
            } catch (IOException e) {
                LOGGER.error("file applyToSource exception");
                fail("file applyToSource exception");
            }

            assertEquals(String.valueOf(newExposure),
                imageSource.getImagePropertyString(PropertyKey.Exif.EXPOSURE_TIME));
            assertTrue(ret > 0);
            imageSource.release();
        }
        verify.expectSmaller(PerformanceUtil.middleDoubleUsec(times), 1);
    }

    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.PERF)
    @TestTarget(className = "ohos.media.image.PropertyFilter",
        methodName = "public PropertyFilter setPropertyInt(String, int)")
    public void testPropertyFilter_setPropertyInt_001() {
        PerfVerify verify = new PerfVerify(baseLine);
        List<Long> times = new ArrayList<>();
        for (int index = 0; index < REPEAT_TIMES; index++) {
            ImageSource imageSource = ImageSource.create(JPG_SRC_FILE_PATH, null);
            assertNotNull(imageSource);
            String oldLocation = imageSource.getImagePropertyString(PropertyKey.Exif.SUBJECT_LOCATION);
            int newLocation = (int)(System.currentTimeMillis()/1.0E8);
            long ret = -100;
            PropertyFilter mPropertyFilter = new PropertyFilter();

            long startTime = System.nanoTime();
            mPropertyFilter.setPropertyInt(PropertyKey.Exif.SUBJECT_LOCATION, newLocation);
            long endTime = System.nanoTime();
            times.add(endTime - startTime);
            try {
                ret = mPropertyFilter.applyToSource(imageSource);
            } catch (IOException e) {
                LOGGER.error("file applyToSource exception");
                fail("file applyToSource exception");
            }

            assertEquals(String.valueOf(newLocation),
                imageSource.getImagePropertyString(PropertyKey.Exif.SUBJECT_LOCATION));
            assertTrue(ret > 0);
            imageSource.release();
        }
        verify.expectSmaller(PerformanceUtil.middleDoubleUsec(times), 1);
    }

    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.PERF)
    @TestTarget(className = "ohos.media.image.PropertyFilter",
        methodName = "public PropertyFilter setPropertyString(String, String)")
    public void testPropertyFilter_setPropertyString_001() {
        PerfVerify verify = new PerfVerify(baseLine);
        List<Long> times = new ArrayList<>();
        for (int index = 0; index < REPEAT_TIMES; index++) {
            ImageSource imageSource = ImageSource.create(JPG_SRC_FILE_PATH, null);
            assertNotNull(imageSource);
            String oldArtist = imageSource.getImagePropertyString(PropertyKey.Exif.ARTIST);
            String newArtist = "testPropertyFilter_setPropertyString_001" + String.valueOf(System.currentTimeMillis());
            long ret = -100;
            PropertyFilter mPropertyFilter = new PropertyFilter();

            long startTime = System.nanoTime();
            mPropertyFilter.setPropertyString(PropertyKey.Exif.ARTIST, newArtist);
            long endTime = System.nanoTime();
            times.add(endTime - startTime);
            try {
                ret = mPropertyFilter.applyToSource(imageSource);
            } catch (IOException e) {
                LOGGER.error("file applyToSource exception");
                fail("file applyToSource exception");
            }

            assertEquals(newArtist, imageSource.getImagePropertyString(PropertyKey.Exif.ARTIST));
            assertTrue(ret > 0);
            imageSource.release();
        }
        verify.expectSmaller(PerformanceUtil.middleDoubleUsec(times), 1);
    }

    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.PERF)
    @TestTarget(className = "ohos.media.image.PropertyFilter",
        methodName = "public PropertyFilter rollbackProperty(String)")
    public void testPropertyFilter_rollbackProperty_001() {
        PerfVerify verify = new PerfVerify(baseLine);
        List<Long> times = new ArrayList<>();
        for (int index = 0; index < REPEAT_TIMES; index++) {
            ImageSource imageSource = ImageSource.create(JPG_SRC_FILE_PATH, null);
            assertNotNull(imageSource);
            String oldLocation = imageSource.getImagePropertyString(PropertyKey.Exif.SUBJECT_LOCATION);
            long ret = -100;
            PropertyFilter mPropertyFilter = new PropertyFilter();

            long startTime = System.nanoTime();
            mPropertyFilter.rollbackProperty(PropertyKey.Exif.SUBJECT_LOCATION);
            long endTime = System.nanoTime();
            times.add(endTime - startTime);
            try {
                ret = mPropertyFilter.applyToSource(imageSource);
            } catch (IOException e) {
                LOGGER.error("file applyToSource exception");
                fail("file applyToSource exception");
            }

            assertEquals(String.valueOf(oldLocation),
                String.valueOf(imageSource.getImagePropertyString(PropertyKey.Exif.SUBJECT_LOCATION)));
            assertTrue(ret > 0);
            imageSource.release();
        }
        verify.expectSmaller(PerformanceUtil.middleDoubleUsec(times), 1);
    }

    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.PERF)
    @TestTarget(className = "ohos.media.image.PropertyFilter",
        methodName = "public PropertyFilter restore()")
    public void testPropertyFilter_restore_001() {
        PerfVerify verify = new PerfVerify(baseLine);
        List<Long> times = new ArrayList<>();
        for (int index = 0; index < REPEAT_TIMES; index++) {
            ImageSource imageSource = ImageSource.create(JPG_SRC_FILE_PATH, null);
            assertNotNull(imageSource);
            String oldArtist = imageSource.getImagePropertyString(PropertyKey.Exif.ARTIST);
            String oldLocation = imageSource.getImagePropertyString(PropertyKey.Exif.SUBJECT_LOCATION);
            String oldExposure = imageSource.getImagePropertyString(PropertyKey.Exif.EXPOSURE_TIME);
            String newArtist = "testsetPropertyFilter002_" + String.valueOf(System.currentTimeMillis());
            int newLocation = (int)(System.currentTimeMillis()/1.0E8);
            Double newExposure = Double.valueOf(newLocation + 0.1);
            long ret = -100;
            PropertyFilter mPropertyFilter = new PropertyFilter();
            mPropertyFilter.setPropertyString(PropertyKey.Exif.ARTIST, newArtist)
                .setPropertyInt(PropertyKey.Exif.SUBJECT_LOCATION, newLocation)
                .setPropertyDouble(PropertyKey.Exif.EXPOSURE_TIME, newExposure);

            long startTime = System.nanoTime();
            mPropertyFilter.restore();
            long endTime = System.nanoTime();
            times.add(endTime - startTime);
            try {
                ret = mPropertyFilter.applyToSource(imageSource);
            } catch (IOException e) {
                LOGGER.error("file applyToSource exception");
                fail("file applyToSource exception");
            }

            assertEquals(oldArtist, imageSource.getImagePropertyString(PropertyKey.Exif.ARTIST));
            assertEquals(String.valueOf(oldLocation),
                String.valueOf(imageSource.getImagePropertyString(PropertyKey.Exif.SUBJECT_LOCATION)));
            assertEquals(String.valueOf(oldExposure),
                String.valueOf(imageSource.getImagePropertyString(PropertyKey.Exif.EXPOSURE_TIME)));
            assertTrue(ret > 0);
            imageSource.release();
        }
        verify.expectSmaller(PerformanceUtil.middleDoubleUsec(times), 1);
    }

    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.PERF)
    @TestTarget(className = "ohos.media.image.PixelMap",
        methodName = "public PixelMap createFromAlpha()")
    public void testPixelMap_createFromAlpha_001() {
        PerfVerify verify = new PerfVerify(baseLine);
        List<Long> times = new ArrayList<>();
        for (int index = 0; index < REPEAT_TIMES; index++) {
            ImageSource.SourceOptions srcOpts = new ImageSource.SourceOptions();
            String pathName = PNG_NINE_PATCH_SRC_FILE_PATH;
            ImageSource imageSource = ImageSource.create(pathName, srcOpts);
            assertNotNull(imageSource);
            ImageSource.DecodingOptions decodingOpts = new ImageSource.DecodingOptions();
            PixelMap pixelMapSrc = imageSource.createPixelmap(decodingOpts);
            assertNotNull(pixelMapSrc);

            long startTime = System.nanoTime();
            PixelMap alphaPixelMap = pixelMapSrc.createFromAlpha();
            long endTime = System.nanoTime();
            times.add(endTime - startTime);

            assertNotNull(alphaPixelMap);
            imageSource.release();
            pixelMapSrc.release();
            alphaPixelMap.release();
        }
        verify.expectSmaller(PerformanceUtil.middleDoubleUsec(times), 1);
    }

    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.PERF)
    @TestTarget(className = "ohos.media.image.PixelMap",
        methodName = "public boolean isReleased()")
    public void testPixelMap_isReleased_001() {
        PerfVerify verify = new PerfVerify(baseLine);
        List<Long> times = new ArrayList<>();
        for (int index = 0; index < REPEAT_TIMES; index++) {
            ImageSource.SourceOptions srcOpts = new ImageSource.SourceOptions();
            String pathName = PNG_NINE_PATCH_SRC_FILE_PATH;
            ImageSource imageSource = ImageSource.create(pathName, srcOpts);
            assertNotNull(imageSource);
            ImageSource.DecodingOptions decodingOpts = new ImageSource.DecodingOptions();
            PixelMap pixelMapSrc = imageSource.createPixelmap(decodingOpts);
            assertNotNull(pixelMapSrc);

            long startTime = System.nanoTime();
            boolean isReleased = pixelMapSrc.isReleased();
            long endTime = System.nanoTime();
            times.add(endTime - startTime);

            assertFalse(isReleased);
            imageSource.release();
            pixelMapSrc.release();
        }
        verify.expectSmaller(PerformanceUtil.middleDoubleUsec(times), 1);
    }

    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.PERF)
    @TestTarget(className = "ohos.media.image.common.ImageInfo",
        methodName = "public String toString()")
    public void testImageInfo_toString_001() {
        PerfVerify verify = new PerfVerify(baseLine);
        List<Long> times = new ArrayList<>();
        for (int index = 0; index < REPEAT_TIMES; index++) {
            ImageSource.SourceOptions srcOpts = new ImageSource.SourceOptions();
            String pathName = PNG_NINE_PATCH_SRC_FILE_PATH;
            ImageSource imageSource = ImageSource.create(pathName, srcOpts);
            assertNotNull(imageSource);
            ImageSource.DecodingOptions decodingOpts = new ImageSource.DecodingOptions();
            PixelMap pixelMapSrc = imageSource.createPixelmap(decodingOpts);
            assertNotNull(pixelMapSrc);

            ImageInfo info = pixelMapSrc.getImageInfo();

            long startTime = System.nanoTime();
            info.toString();
            long endTime = System.nanoTime();
            times.add(endTime - startTime);

            imageSource.release();
            pixelMapSrc.release();
        }
        verify.expectSmaller(PerformanceUtil.middleDoubleUsec(times), 1);
    }

    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.PERF)
    @TestTarget(className = "ohos.media.image.common.DecodeEvent",
        methodName = "public static DecodeEvent getDecodeEvent(int)")
    public void testDecodeEvent_getDecodeEvent_001() {
        PerfVerify verify = new PerfVerify(baseLine);
        List<Long> times = new ArrayList<>();
        for (int index = 0; index < REPEAT_TIMES; index++) {
            int max = DecodeEvent.EVENT_LAST.getValue();
            int min = DecodeEvent.EVENT_COMPLETE_DECODE.getValue();
            Random random = new Random();
            int event = random.nextInt(max) % (max - min + 1) + min;

            long startTime = System.nanoTime();
            DecodeEvent decodeEvent = DecodeEvent.getDecodeEvent(event);
            long endTime = System.nanoTime();
            times.add(endTime - startTime);

            decodeEvent.getValue();
        }
        verify.expectSmaller(PerformanceUtil.middleDoubleUsec(times), 1);
    }

}
