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

package ohos.media.utils.log;

import ohos.hiviewdfx.HiLog;
import ohos.hiviewdfx.HiLogLabel;
import ohos.system.Parameters;

/**
 * Factory of multimedia logger.
 *
 * @since 1
 */
public class LoggerFactory {
    private static final int MEDIA_DOMAIN_ID = 0xD002B01;

    private static final int CAMERA_DOMAIN_ID = 0xD002B02;

    private static final int PLUGIN_DOMAIN_ID = 0xD002B03;

    private static final int DATA_TUNNEL_DOMAIN_ID = 0xD002B04;

    private static final int IMAGE_DOMAIN_ID = 0xD002B05;

    private static final int AUDIO_DOMAIN_ID = 0xD002B06;

    private static final int RADIO_DOMAIN_ID = 0xD002B07;

    /**
     * Get audio logger.
     *
     * @param clazz class who uses the logger
     * @return Audio logger
     * @since 1
     */
    public static Logger getAudioLogger(Class<?> clazz) {
        String tag = clazz.getSimpleName();
        HiLogLabel label = new HiLogLabel(HiLog.LOG_CORE, AUDIO_DOMAIN_ID, tag);
        return new Logger(label, getLoggerLevel(tag));
    }

    /**
     * Get media logger.
     *
     * @param clazz class who uses the logger
     * @return Media logger
     * @since 1
     */
    public static Logger getMediaLogger(Class<?> clazz) {
        String tag = clazz.getSimpleName();
        HiLogLabel label = new HiLogLabel(HiLog.LOG_CORE, MEDIA_DOMAIN_ID, tag);
        return new Logger(label, getLoggerLevel(tag));
    }

    /**
     * Get camera logger.
     *
     * @param clazz class who uses the logger
     * @return Camera logger
     * @since 1
     */
    public static Logger getCameraLogger(Class<?> clazz) {
        String tag = clazz.getSimpleName();
        HiLogLabel label = new HiLogLabel(HiLog.LOG_CORE, CAMERA_DOMAIN_ID, tag);
        return new Logger(label, getLoggerLevel(tag));
    }

    /**
     * Get plugin logger.
     *
     * @param clazz class who uses the logger
     * @return Plugin logger
     * @since 1
     */
    public static Logger getPluginLogger(Class<?> clazz) {
        String tag = clazz.getSimpleName();
        HiLogLabel label = new HiLogLabel(HiLog.LOG_CORE, PLUGIN_DOMAIN_ID, tag);
        return new Logger(label, getLoggerLevel(tag));
    }

    /**
     * Get data tunnel logger.
     *
     * @param clazz class who uses the logger
     * @return Data tunnel logger
     * @since 1
     */
    public static Logger getDataTunnelLogger(Class<?> clazz) {
        String tag = clazz.getSimpleName();
        HiLogLabel label = new HiLogLabel(HiLog.LOG_CORE, DATA_TUNNEL_DOMAIN_ID, tag);
        return new Logger(label, getLoggerLevel(tag));
    }

    /**
     * Get image logger.
     *
     * @param clazz class who uses the logger
     * @return Image logger
     * @since 1
     */
    public static Logger getImageLogger(Class<?> clazz) {
        String tag = clazz.getSimpleName();
        HiLogLabel label = new HiLogLabel(HiLog.LOG_CORE, IMAGE_DOMAIN_ID, tag);
        return new Logger(label, getLoggerLevel(tag));
    }

    /**
     * Get radio logger.
     *
     * @param clazz class who uses the logger
     * @return Radio logger
     * @since 3
     */
    public static Logger getRadioLogger(Class<?> clazz) {
        String tag = clazz.getSimpleName();
        HiLogLabel label = new HiLogLabel(HiLog.LOG_CORE, RADIO_DOMAIN_ID, tag);
        return new Logger(label, getLoggerLevel(tag));
    }

    private static int getLoggerLevel(String tag) {
        return 0;
    }

    private static boolean isLoggerEnable() {
        return true;
    }

    private static boolean isLoggable(String tag, int level) {
        return HiLog.isLoggable(HiLog.LOG_CORE, tag, level);
    }
}
