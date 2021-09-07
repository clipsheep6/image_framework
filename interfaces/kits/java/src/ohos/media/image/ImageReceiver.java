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

import ohos.agp.graphics.Surface;
import ohos.eventhandler.EventHandler;
import ohos.eventhandler.EventRunner;
import ohos.media.image.common.ImageFormat;
import ohos.media.image.common.Size;
import ohos.media.utils.log.Logger;
import ohos.media.utils.log.LoggerFactory;
import ohos.utils.system.SystemCap;
import ohos.utils.system.SystemCapability;

import java.lang.ref.WeakReference;
import java.util.HashSet;
import java.util.Set;

/**
 * Describes an image receiver that connects to an image output device and
 * provides a buffer queue to receive image data.
 *
 * @Syscap {@link SystemCapability.Multimedia#IMAGE}
 * @since 1
 */
@SystemCap("SystemCapability.Multimedia.Image")
public class ImageReceiver {
    private static final Logger LOGGER = LoggerFactory.getImageLogger(ImageReceiver.class);
    private static final String HANDLER_NAME = "nativeEventHandler";
    private static final long USAGE_CPU_READ_OFTEN = 3L;  // native hardware setting
    private static final int MIN_POSITIVE = 1;
    private final int width;
    private final int height;
    private final int format;
    private final int capacity;
    private final Object receiverLock = new Object();
    private volatile boolean isReceiverValid = false;  // receiver availability status
    private volatile IImageArrivalListener arrivalListener;  // native callback listener
    private final Set<Image> images = new HashSet<>();  // image release when free receiver
    private long nativeContext;  // native used,tracing native object
    private final Surface surface;
    private volatile EventHandler handler;

    static {
        LOGGER.debug("Begin loading imagereceiver_jni library.");
        try {
            System.loadLibrary("imagereceiver_jni.z");
        } catch (UnsatisfiedLinkError | NullPointerException e) {
            LOGGER.error("loadLibrary imagereceiver_jni.z fail");
        }
        nativeCacheClass();  // native cache some field
    }

    /**
     * Creates an {@link ImageReceiver} instance based on a specified image width, height, format,
     * and the buffer queue capacity.
     *
     * @param width Indicates the image width.
     * @param height Indicates the image height.
     * @param format Indicates the image format, which is defined by {@link ImageFormat}.
     * @param capacity Indicates the maximum buffer capacity of the image receiver. The value must be greater than 0.
     * @return Returns the {@link ImageReceiver} object.
     * @throws IllegalArgumentException Throws this exception if a parameter is incorrect.
     * @throws IllegalStateException Throws this exception if the operation is not supported in the current state,
     * for example, if native resources fail to be applied for the receiver.
     * @since 1
     */
    public static ImageReceiver create(int width, int height, int format, int capacity) {
        return new ImageReceiver(width, height, format, capacity, USAGE_CPU_READ_OFTEN);
    }

    /**
     * Constructor implement.
     *
     * @param width The width in pixels of the image.
     * @param height The height in pixels of the image.
     * @param format The format of image, which is one of values from {@link ImageFormat}.
     * @param capacity The maximum number of images cached by ImageReceiver, must be larger than 0.
     * @param usage The intended usage of the image.
     * @throws IllegalArgumentException when width, height or imageCount not valid.
     * @throws IllegalStateException Error when native create failed.
     */
    private ImageReceiver(int width, int height, int format, int capacity, long usage) {
        if (width < MIN_POSITIVE || height < MIN_POSITIVE) {
            LOGGER.error("The image size:[%{public}d, %{public}d] must be positive.", width, height);
            throw new IllegalArgumentException("The image size must be positive.");
        }
        if (capacity < MIN_POSITIVE) {
            LOGGER.error("The image capacity %{public}d must be larger than zero.", capacity);
            throw new IllegalArgumentException("The image capacity must be larger than zero.");
        }
        // ImageReceiver can't support NV21 format
        if (format == ImageFormat.NV21) {
            LOGGER.error("NV21 format is not supported.");
            throw new IllegalArgumentException("NV21 format is not supported.");
        }
        this.width = width;
        this.height = height;
        this.format = format;
        this.capacity = capacity;
        nativeInit(new WeakReference<>(this), width, height, format, capacity, usage);
        surface = nativeGetReceiverSurface();
        isReceiverValid = true;
    }

    /**
     * Obtains the receiver surface of the image receiver.
     *
     * @return Returns a {@link Surface} object of the image receiver.
     * @since 3
     */
    public Surface getRecevingSurface() {
        return surface;
    }

    /**
     * Obtains the image size.
     *
     * @return Returns the image size specified in a {@link Size} object.
     * @since 1
     */
    public Size getImageSize() {
        return new Size(width, height);
    }

    /**
     * Obtains the maximum buffer capacity of the image receiver.
     *
     * @return Returns the buffer size of the image receiver.
     * @since 1
     */
    public int getCapacity() {
        return capacity;
    }

    /**
     * Obtains the image format.
     *
     * @return Returns the image format, which is defined by {@link ImageFormat}.
     * @since 1
     */
    public int getImageFormat() {
        return format;
    }

    /**
     * Reads the latest image data from the buffer queue and clears earlier images from the queue.
     *
     * @return Returns the latest image data if obtained; returns {@code null} otherwise.
     * @throws IllegalStateException Throws this exception if the operation is not supported in the current state,
     * for example, if native resources associated with the receiver are released.
     * @since 1
     */
    public Image readLatestImage() {
        Image image = readNextImage();
        if (image == null) {
            return null;
        }

        while (true) {
            Image nextImage = readNextImage();
            if (nextImage == null) {
                return image;
            }
            image.release();
            synchronized (receiverLock) {
                images.remove(image);
            }
            image = nextImage;
        }
    }

    /**
     * Reads the next image from the buffer queue.
     *
     * @return Returns next image data if obtained; returns {@code null} otherwise.
     * @throws IllegalStateException Throws this exception if the operation is not supported in the current state,
     * for example, if native resources associated with the receiver are released.
     * @since 1
     */
    public Image readNextImage() {
        Image image = null;
        synchronized(receiverLock) {
            if (isReceiverValid) {
                image = new Image(this);
                boolean hasNext = nativeReadNext(image);
                if (!hasNext) {
                    LOGGER.error("get image from native response empty.");
                    return null;
                }
                image.setImageStatus(true);
                images.add(image);
            }
        }
        return image;
    }

    /**
     * Releases native resources associated with the image receiver, including the surface and obtained images.
     *
     * @since 1
     */
    public void release() {
        setImageArrivalListener(null);
        synchronized (receiverLock) {
            isReceiverValid = false;
            for (Image image : images) {
                if (image.getImageStatus()) {
                    image.release();
                }
            }
            images.clear();
            nativeRelease();
        }
    }

    @Override
    protected void finalize() throws Throwable {
        try {
            release();
        } finally {
            super.finalize();
        }
    }

    /**
     * Native call back when an Event happens.
     *
     * @param objectRef ImageReceiver reference form native.
     */
    private static void onEventFromNative(Object objectRef) {
        if (objectRef instanceof WeakReference<?>) {
            @SuppressWarnings("unchecked")
            WeakReference<ImageReceiver> weakRef = (WeakReference<ImageReceiver>)objectRef;
            final ImageReceiver imageReceiver = weakRef.get();
            if (imageReceiver == null) {
                LOGGER.error("native transmit image receiver is null.");
                return;
            }
            final EventHandler handler = imageReceiver.handler;
            if (handler == null) {
                LOGGER.error("event handler is null, set image arrival listener firstly.");
                return;
            }
            final IImageArrivalListener listener = imageReceiver.arrivalListener;
            final boolean receiverValid = imageReceiver.isReceiverValid;
            handler.postTask(new Runnable() {
                @Override
                public void run() {
                    if (listener != null && receiverValid) {
                        listener.onImageArrival(imageReceiver);
                    }
                }
            });
        } else {
            LOGGER.error("can't cast object to weak reference.");
        }
    }

    /**
     * Sets the image listener which will be called when new image data is received.
     *
     * @param listener Indicates the image listener, which is defined by {@link IImageArrivalListener}.
     * @since 1
     */
    public void setImageArrivalListener(IImageArrivalListener listener) {
        if (listener == null) {
            arrivalListener = null;
            if (handler == null) {
                return;
            }
            handler.removeAllEvent();
            handler = null;
        } else {
            arrivalListener = listener;
            if (handler == null) {
                handler = new EventHandler(EventRunner.create(HANDLER_NAME));
            }
        }
    }

    /**
     * Callback interface used when new image data is received.
     *
     * @since 1
     */
    public interface IImageArrivalListener {
        /**
         * Called when new image data is received.
         *
         * @param imageReceiver Indicates an {@link ImageReceiver} object.
         * @since 1
         */
        void onImageArrival(ImageReceiver imageReceiver);
    }

    private synchronized native void nativeInit(Object weakThiz, int width, int height, int format, int capacity,
                                                long consumerUsage);

    private synchronized native boolean nativeReadNext(Image image);

    private synchronized native void nativeRelease();

    private synchronized native void nativeReleaseFreeBuffers();

    private synchronized native Surface nativeGetReceiverSurface();

    private static native void nativeCacheClass();
}
