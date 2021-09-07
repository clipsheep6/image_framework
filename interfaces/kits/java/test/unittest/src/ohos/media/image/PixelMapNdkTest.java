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

import ohos.unittest.CaseLevel;
import ohos.unittest.CaseType;
import ohos.unittest.Level;
import ohos.unittest.Type;
import ohos.media.image.common.ImageInfo;
import ohos.media.utils.log.Logger;
import ohos.media.utils.log.LoggerFactory;

import org.junit.AfterClass;
import org.junit.Before;
import org.junit.After;
import org.junit.BeforeClass;
import org.junit.Rule;
import org.junit.Test;

import java.io.File;

public class PixelMapNdkTest {
    private static final Logger LOGGER = LoggerFactory.getImageLogger(PixelMapNdkTest.class);

    static {
        try {
            System.loadLibrary("image_ndk_test_jni.z");
        } catch (UnsatisfiedLinkError | NullPointerException e) {
            LOGGER.error("loadLibrary image_ndk_test_jni.z fail");
        }
        nativeInit();
    }

    private static File file;

    private ImageSource imageSource;

    private PixelMap pixelMap;

    /**
     * Action before all test case.
     */
    @BeforeClass
    public static void setUpBeforeClass() {
        file = new File("/sdcard/multimedia/image/test_large.webp");
        if (!file.exists()) {
            LOGGER.error("test file not exist.");
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
        pixelMap = createTestPixelMap(file.getPath());
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
     * @tc.name: testPixelMapNdk001
     * @tc.desc: compare the width and height with native pixels
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testPixelMapNdk001() {
        /**
         * @tc.steps: step1. input correct width/height.
         * @tc.expected: step1. expect 0 ok.
         */
        ImageInfo imageInfo = pixelMap.getImageInfo();
        int result = nativeValidatePixelMapInfo(pixelMap, imageInfo.size.width, imageInfo.size.height, false);
        assertEquals(result, 0);

        /**
         * @tc.steps: step2. input incorrect width/height.
         * @tc.expected: step2. expect -1 ok.
         */
        result = nativeValidatePixelMapInfo(pixelMap, 1, 1, false);
        assertEquals(result, -1);
    }

    /**
     * @tc.name: testPixelMapNdk002
     * @tc.desc: access/unaccess pixels
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testPixelMapNdk002() {
        /**
         * @tc.steps: step1. access pixels.
         * @tc.expected: step1. expect 0 ok.
         */
        int result = nativeAccessPixels(pixelMap);
        assertEquals(result, 0);

        /**
         * @tc.steps: step2. unaccess pixels.
         * @tc.expected: step2. expect 0 ok.
         */
        result = nativeUnAccessPixels(pixelMap);
        assertEquals(result, 0);
    }

    /**
     * @tc.name: testPixelMapNdk003
     * @tc.desc: get pixel format
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testPixelMapNdk003() {
        /**
         * @tc.steps: step1. get native pixel format.
         * @tc.expected: step1. expect RGBA format ok.
         */
        int result = nativeGetPixelFormat(pixelMap);
        assertEquals(result, 3);
    }

    /**
     * @tc.name: testPixelMapNdk004
     * @tc.desc: native read pixel and check value
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testPixelMapNdk004() {
        /**
         * @tc.steps: step1. get pixel value,pos is 360,342.
         * @tc.expected: step1. expect value ok.
         */
        int result = nativeReadPixelsValue(pixelMap);
        assertEquals(result, -3324097);

        /**
         * @tc.steps: step2. release pixel map then access pixel.
         * @tc.expected: step2. expect -2 exception.
         */
        pixelMap.release();
        result = nativeAccessPixels(pixelMap);
        assertEquals(result, -2);
    }

    private static PixelMap createTestPixelMap(String filePath) {
        ImageSource.SourceOptions opts = new ImageSource.SourceOptions();
        opts.formatHint = "";
        ImageSource imageSource = ImageSource.create(filePath, opts);
        if (imageSource == null) {
            LOGGER.error("test pixelmap to create the imageSource fail.");
            return null;
        }
        ImageSource.DecodingOptions options = new ImageSource.DecodingOptions();
        return imageSource.createPixelmap(options);
    }

    private static native void nativeInit();

    private native int nativeValidatePixelMapInfo(PixelMap pixelMap, int width, int height, boolean is565);

    private native int nativeAccessPixels(PixelMap pixelMap);

    private native int nativeReadPixelsValue(PixelMap pixelMap);

    private native int nativeUnAccessPixels(PixelMap pixelMap);

    private native int nativeGetPixelFormat(PixelMap pixelMap);
}