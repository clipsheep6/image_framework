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

package ohos.media.image.common;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotNull;
import static org.junit.Assert.assertNull;

import ohos.media.utils.log.Logger;
import ohos.media.utils.log.LoggerFactory;
import ohos.unittest.CaseLevel;
import ohos.unittest.CaseType;
import ohos.unittest.Level;
import ohos.unittest.Type;

import org.junit.Test;

import java.lang.reflect.Constructor;

/**
 * ImageFormatTest, test cases for ImageFormat class.
 *
 * @since 1
 */
public class ImageFormatTest {
    private static final Logger LOGGER = LoggerFactory.getImageLogger(ImageFormatTest.class);

    /**
     * @tc.name: testImageFormat001
     * @tc.desc: create ImageFormat info for image operations.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testImageFormat001() {
        /**
         * @tc.steps: step1. create image format info.
         * @tc.expected: step1. the image format is not null.
         */
        ImageFormat imageFormat = null;
        try {
            Class<?> clazz = Class.forName("ohos.media.image.common.ImageFormat");
            Constructor<?> constructor = clazz.getDeclaredConstructor();
            constructor.setAccessible(true);
            Object imageFormatObj = constructor.newInstance();
            if (imageFormatObj instanceof ImageFormat) {
                imageFormat = (ImageFormat) imageFormatObj;
                assertNotNull(imageFormat);
            }
        } catch (Exception e) {
            LOGGER.error("reflect image private constructor failed.");
            assertNull(imageFormat);
        }
    }

    /**
     * @tc.name: testImageFormat002
     * @tc.desc: get pixel bits number by format.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testImageFormat002() {
        /**
         * @tc.steps: step1. get pixel bits number by format.
         * @tc.expected: step1. check the information is ok.
         */
        int bitsByPixel = ImageFormat.getBitsNumberPerPixel(ImageFormat.JPEG);
        assertEquals(bitsByPixel, 0);
        /**
         * @tc.steps: step2. get pixel bits number by format.
         * @tc.expected: step2. check the information is ok.
         */
        bitsByPixel = ImageFormat.getBitsNumberPerPixel(ImageFormat.RAW16);
        assertEquals(bitsByPixel, 16);
    }

    /**
     * @tc.name: testImageFormat003
     * @tc.desc: get pixel component number by format.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testImageFormat003() {
        /**
         * @tc.steps: step1. get pixel component number by format.
         * @tc.expected: step1. check the information is ok.
         */
        int componentNum = ImageFormat.getComponentNumber(ImageFormat.JPEG);
        assertEquals(componentNum, 1);
        /**
         * @tc.steps: step2. get pixel component number by format.
         * @tc.expected: step2. check the information is ok.
         */
        componentNum = ImageFormat.getComponentNumber(ImageFormat.YUV420_888);
        assertEquals(componentNum, 3);
    }
}