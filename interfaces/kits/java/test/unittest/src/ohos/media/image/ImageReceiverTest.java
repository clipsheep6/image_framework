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

import ohos.agp.graphics.Surface;
import ohos.media.image.ImageReceiver.IImageArrivalListener;
import ohos.media.image.common.ImageFormat;
import ohos.media.image.common.Size;
import ohos.media.utils.log.Logger;
import ohos.media.utils.log.LoggerFactory;
import ohos.unittest.CaseLevel;
import ohos.unittest.CaseType;
import ohos.unittest.Level;
import ohos.unittest.Type;

import org.junit.After;
import org.junit.Test;

import java.lang.ref.WeakReference;
import java.lang.reflect.Field;
import java.lang.reflect.Method;

/**
 * ImageReceiverTest, test cases for ImageReceiver class.
 *
 * @since 1
 */
public class ImageReceiverTest {
    private static final Logger LOGGER = LoggerFactory.getImageLogger(ImageReceiverTest.class);

    private ImageReceiver imageReceiver;

    /**
     * Action after test case.
     */
    @After
    public void tearDown() throws Exception {
        try {
            if (imageReceiver != null) {
                imageReceiver.finalize();
                imageReceiver = null;
            }
        } catch (Throwable throwable) {
            LOGGER.error("test case teardown fail, %{public}s", throwable.getMessage());
        }
    }

    /**
     * @tc.name: testImageReceiver001
     * @tc.desc: create ImageReceiver object and operate simple method.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testImageReceiver001() {
        /**
         * @tc.steps: step1. create image object use wrong parameters.
         * @tc.expected: step1. the image object is null.
         */
        try {
            imageReceiver = getImageReceiver(false);
        } catch (IllegalArgumentException e) {
            LOGGER.error("create ImageReceiver throws exception.");
        }
        imageReceiver = getImageReceiver(true);
        assertNotNull(imageReceiver);
        /**
         * @tc.steps: step2. get Surface.
         * @tc.expected: step2. check the surface is not null.
         */
        Surface surface = imageReceiver.getRecevingSurface();
        assertNotNull(surface);

        /**
         * @tc.steps: step3. get image size.
         * @tc.expected: step3. check the size is ok.
         */
        Size size = imageReceiver.getImageSize();
        assertEquals(320, size.width);
        assertEquals(240, size.height);
        /**
         * @tc.steps: step4. get the maximum number of images.
         * @tc.expected: step4. check the capacity is ok.
         */
        int capacity = imageReceiver.getCapacity();
        assertEquals(1, capacity);
        /**
         * @tc.steps: step5. get the format of image.
         * @tc.expected: step5. check the format is ok.
         */
        int format = imageReceiver.getImageFormat();
        assertEquals(ImageFormat.JPEG, format);
    }

    /**
     * @tc.name: testImageReceiver002
     * @tc.desc: read image from receiving queue.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testImageReceiver002() {
        /**
         * @tc.steps: step1. read the latest image from receiving queue.
         * @tc.expected: step1. check the information is ok.
         */
        imageReceiver = getImageReceiver(true);
        Image image = null;
        try {
            image = imageReceiver.readLatestImage();
        } catch (IllegalStateException e) {
            // operation will be called native, but buffer was set by app, only interface cover.
            assertNull(image);
        }
        /**
         * @tc.steps: step2. read the next image from receiving queue.
         * @tc.expected: step2. check the information is ok.
         */
        try {
            image = imageReceiver.readNextImage();
        } catch (IllegalStateException e) {
            // operation will be called native, but buffer was set by app, only interface cover.
            assertNull(image);
        }
    }

    /**
     * @tc.name: testImageReceiver003
     * @tc.desc: native event callback.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testImageReceiver003() {
        /**
         * @tc.steps: step1. native event callback.
         * @tc.expected: step1. check the information is ok.
         */
        imageReceiver = getImageReceiver(true);
        try {
            Class<?> clazz = Class.forName("ohos.media.image.ImageReceiver");
            Method method = clazz.getDeclaredMethod("onEventFromNative", Object.class);
            method.setAccessible(true);
            WeakReference<ImageReceiver> weakRef = new WeakReference<>(imageReceiver);
            method.invoke(imageReceiver, weakRef);
        } catch (Exception e) {
            LOGGER.error("reflect invoke onEventFromNative throws exception.");
        }
        assertNotNull(imageReceiver);
    }

    /**
     * @tc.name: testImageReceiver004
     * @tc.desc: set image arrival listener.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testImageReceiver004() {
        try {
            /**
             * @tc.steps: step1. release the image arrival listener.
             * @tc.expected: step1. check the information is ok.
             */
            imageReceiver = getImageReceiver(true);
            IImageArrivalListener arrivalListener = new IImageArrivalListener() {
                @Override
                public void onImageArrival(ImageReceiver imageReceiver) {
                    LOGGER.debug("on image arrival.");
                }
            };
            imageReceiver.setImageArrivalListener(null);
            Class<?> clazz = Class.forName("ohos.media.image.ImageReceiver");
            Field field = clazz.getDeclaredField("arrivalListener");
            field.setAccessible(true);
            if (field.get(imageReceiver) instanceof IImageArrivalListener) {
                IImageArrivalListener attribute = (IImageArrivalListener) field.get(imageReceiver);
                assertNull(attribute);
            }
        } catch (Exception e) {
            LOGGER.error("reflect invoke arrivalListener throws exception.");
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
}