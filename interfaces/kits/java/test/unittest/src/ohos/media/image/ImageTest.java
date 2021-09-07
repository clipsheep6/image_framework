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
import static org.junit.Assert.assertNotNull;
import static org.junit.Assert.assertNull;

import ohos.media.image.common.ImageFormat;
import ohos.media.image.common.Rect;
import ohos.media.image.common.Size;
import ohos.media.utils.log.Logger;
import ohos.media.utils.log.LoggerFactory;
import ohos.unittest.CaseLevel;
import ohos.unittest.CaseType;
import ohos.unittest.Level;
import ohos.unittest.Type;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;

import java.lang.reflect.Constructor;
import java.nio.ByteBuffer;

/**
 * ImageTest, test cases for Image class.
 *
 * @since 1
 */
public class ImageTest {
    private static final Logger LOGGER = LoggerFactory.getImageLogger(ImageTest.class);
    private ImageReceiver imageReceiver;
    private Image image;

    /**
     * Action before test case.
     */
    @Before
    public void setUp() {
        imageReceiver = getImageReceiver(true);
        image = new Image(imageReceiver);
        image.setImageStatus(true);
    }

    /**
     * Action after test case.
     */
    @After
    public void tearDown() {
        try {
            if (imageReceiver != null) {
                imageReceiver.finalize();
                imageReceiver = null;
            }
            if (image != null) {
                image.finalize();
                image = null;
            }
        } catch (Throwable e) {
            LOGGER.error("test case teardown fail, %{public}s", e.getMessage());
        }
    }

    /**
     * @tc.name: testImage001
     * @tc.desc: create image object.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testImage001() {
        /**
         * @tc.steps: step1. create image object use wrong parameters.
         * @tc.expected: step1. the image object is null.
         */
        image = null;
        try {
            imageReceiver = getImageReceiver(false);
            image = new Image(imageReceiver);
        } catch (IllegalArgumentException e) {
            LOGGER.error("create ImageReceiver throws exception.");
            assertNull(image);
        }
    }

    /**
     * @tc.name: testImage002
     * @tc.desc: operate image simple method.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testImage002() {
        /**
         * @tc.steps: step1. set image status true.
         * @tc.expected: step1. check the information is ok.
         */
        image.setImageStatus(true);
        assertEquals(true, image.getImageStatus());
        /**
         * @tc.steps: step2. get the format of this image.
         * @tc.expected: step2. check the information is ok.
         */
        int format = image.getFormat();
        // operation will be called native, but buffer was set by app, only interface cover.
        assertEquals(0, format);
        /**
         * @tc.steps: step3. get the size of this image.
         * @tc.expected: step3. check the information is ok.
         */
        Size size = image.getImageSize();
        // operation will be called native, but buffer was set by app, only interface cover.
        assertEquals(0, size.width);
        assertEquals(0, size.height);
        /**
         * @tc.steps: step4. get the timestamp of the image.
         * @tc.expected: step4. check the information is ok.
         */
        long timestamp = image.getTimestamp();
        assertEquals(timestamp, 0L);
    }

    /**
     * @tc.name: testImage003
     * @tc.desc: get and set the clip rectangle of the image.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testImage003() {
        /**
         * @tc.steps: step1. set the clip rectangle in range of the image.
         * @tc.expected: step1. check the information is ok.
         */
        Rect newCrop = new Rect(0, 0, 100, 120);
        image.setClipRect(newCrop);
        Rect crop = image.getClipRect();
        // operation will be called native, but buffer was set by app, only interface cover.
        assertEquals(0, crop.width);
        assertEquals(0, crop.height);
        /**
         * @tc.steps: step2. set the clip rectangle out range of the image.
         * @tc.expected: step2. check the information is ok.
         */
        newCrop = new Rect(0, 0, 1080, 1920);
        image.setClipRect(newCrop);
        crop = image.getClipRect();
        // operation will be called native, but buffer was set by app, only interface cover.
        assertEquals(0, crop.width);
        assertEquals(0, crop.height);
    }

    /**
     * @tc.name: testImage004
     * @tc.desc: get the specified component of image.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testImage004() {
        /**
         * @tc.steps: step1. get the specified component of image.
         * @tc.expected: step1. check the information is ok.
         */
        Image.Component component = null;
        try {
            // operation will be called native, but buffer was set by app, only interface cover.
            component = image.getComponent(ImageFormat.ComponentType.JPEG);
        } catch (IllegalStateException e) {
            assertNull(component);
        }
    }

    /**
     * @tc.name: testImage005
     * @tc.desc: create component object and operate methods.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testImage005() {
        /**
         * @tc.steps: step1. create component object.
         * @tc.expected: step1. check the information is ok.
         */
        Image.Component component = getComponent();
        assertNotNull(component);
        /**
         * @tc.steps: step2. read one byte from byteBuffer.
         * @tc.expected: step2. check the information is ok.
         */
        int ret = component.read();
        assertEquals(0, ret);
        /**
         * @tc.steps: step3. read bulk data from byteBuffer.
         * @tc.expected: step3. check the information is ok.
         */
        byte[] dstArray = new byte[2];
        ret = component.read(dstArray);
        assertEquals(0, ret);
        /**
         * @tc.steps: step4. read bulk data from byteBuffer.
         * @tc.expected: step4. check the information is ok.
         */
        ret = component.read(dstArray, 0, 1);
        assertEquals(0, ret);
        /**
         * @tc.steps: step5. get the current position.
         * @tc.expected: step5. check the information is ok.
         */
        ret = component.tell();
        assertEquals(4, ret);
        /**
         * @tc.steps: step6. set the current position of the component buffer.
         * @tc.expected: step6. check the information is ok.
         */
        ret = component.seek(0);
        assertEquals(0, ret);
        /**
         * @tc.steps: step7. get the size of remaining data in the component buffer.
         * @tc.expected: step7. check the information is ok.
         */
        ret = component.remaining();
        assertEquals(4, ret);
        /**
         * @tc.steps: step8. release the native resources associated with this component.
         * @tc.expected: step8. check the information is ok.
         */
        component.release();
        assertNotNull(component);
    }

    /**
     * @tc.name: testImage006
     * @tc.desc: create component object and operate methods.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testImage006() {
        /**
         * @tc.steps: step1. create component object.
         * @tc.expected: step1. check the information is ok.
         */
        Image.Component component = getComponent();
        assertNotNull(component);
        /**
         * @tc.steps: step2. get the buffer object in the component.
         * @tc.expected: step2. check the information is ok.
         */
        ByteBuffer byteBuffer = component.getBuffer();
        assertNotNull(byteBuffer);
        /**
         * @tc.steps: step3. release the native resources associated with this component.
         * @tc.expected: step3. check the information is ok.
         */
        component.release();
        // the component release only set byteBuffer null, but the direct memory not release, its depends on system
        assertNotNull(byteBuffer);
        /**
         * @tc.steps: step4. after component release to continue get the buffer object.
         * @tc.expected: step4. check the information is ok.
         */
        ByteBuffer byteBufferNew = null;
        try {
            byteBufferNew = component.getBuffer();
        } catch (IllegalStateException e) {
            assertNull(byteBufferNew);
        }
    }

    private ImageReceiver getImageReceiver(boolean valid) {
        int width = 320;
        if (!valid) {
            width = -1;
        }
        int height = 240;
        int format = ImageFormat.JPEG;
        int capacity = 1;
        ImageReceiver newImageReceiver = ImageReceiver.create(width, height, format, capacity);
        return newImageReceiver;
    }

    private Image.Component getComponent() {
        try {
            Class<?> clazz = Class.forName("ohos.media.image.Image$Component");
            Class<?>[] classes = new Class<?>[] { int.class, int.class, int.class, java.nio.ByteBuffer.class };
            Constructor<?> constructor = clazz.getDeclaredConstructor(classes);
            constructor.setAccessible(true);
            Object componentObj = constructor.newInstance(ImageFormat.JPEG, 1, 1, ByteBuffer.allocate(4));
            if (componentObj instanceof Image.Component) {
                Image.Component component = (Image.Component) componentObj;
                return component;
            }
        } catch (Exception e) {
            LOGGER.error("reflect private Image.Component object error. %{public}s", e.getMessage());
        }
        return null;
    }
}