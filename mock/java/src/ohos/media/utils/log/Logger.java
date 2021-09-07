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
import ohos.hiviewdfx.HiLogConstString;
import ohos.hiviewdfx.HiLogLabel;

import java.io.PrintWriter;
import java.io.StringWriter;

/**
 * Multimedia logger, based on OHOS HiLog, support turn off by system parameter {@code sys.multimedia.logger.on}.
 *
 * @since 1
 */
public final class Logger {
    /**
     * Logger is off
     *
     * @since 3
     */
    public static final int LOGGER_OFF = -1;

    /**
     * Logger level is debug
     *
     * @since 3
     */
    public static final int LEVEL_DEBUG = HiLog.DEBUG;

    /**
     * Logger level is info
     *
     * @since 3
     */
    public static final int LEVEL_INFO = HiLog.INFO;

    /**
     * Logger level is warn
     *
     * @since 3
     */
    public static final int LEVEL_WARN = HiLog.WARN;

    /**
     * Logger level is error
     *
     * @since 3
     */
    public static final int LEVEL_ERROR = HiLog.ERROR;

    /**
     * Logger level is fatal
     *
     * @since 3
     */
    public static final int LEVEL_FATAL = HiLog.FATAL;

    /**
     * Logger level is off
     *
     * @since 3
     */
    public static final int LEVEL_OFF = Integer.MAX_VALUE;

    private final HiLogLabel label;

    private final int level;

    Logger(HiLogLabel label, int level) {
        this.label = label;
        this.level = level;
    }

    /**
     * Print debug level log.
     *
     * @param format String to print
     * @param args args to replace the type placeholder
     * @return HiLog return value
     * @since 1
     */
    public int debug(@HiLogConstString String format, Object... args) {
        if (level <= LEVEL_DEBUG) {
            return HiLog.debug(label, format, args);
        }
        return LOGGER_OFF;
    }

    /**
     * Print info level log.
     *
     * @param format String to print
     * @param args args to replace the type placeholder
     * @return HiLog return value
     * @since 1
     */
    public int info(@HiLogConstString String format, Object... args) {
        if (level <= LEVEL_INFO) {
            return HiLog.info(label, format, args);
        }
        return LOGGER_OFF;
    }

    /**
     * Print warn level log.
     *
     * @param format String to print
     * @param args args to replace the type placeholder
     * @return HiLog return value
     * @since 1
     */
    public int warn(@HiLogConstString String format, Object... args) {
        if (level <= LEVEL_WARN) {
            return HiLog.warn(label, format, args);
        }
        return LOGGER_OFF;
    }

    /**
     * Print error level log.
     *
     * @param format String to print
     * @param args args to replace the type placeholder
     * @return HiLog return value
     * @since 1
     */
    public int error(@HiLogConstString String format, Object... args) {
        if (level <= LEVEL_ERROR) {
            return HiLog.error(label, format, args);
        }
        return LOGGER_OFF;
    }

    /**
     * Print error level log, contains throwable stack trace.
     *
     * @param format String to print
     * @param throwable The throwable to print
     * @param args args to replace the type placeholder
     * @return HiLog return value
     * @since 3
     */
    public int error(@HiLogConstString String format, Throwable throwable, Object... args) {
        return LOGGER_OFF;
    }

    private String parseStackTrace(Throwable throwable) {
        StringWriter sw = new StringWriter();
        PrintWriter pw = new PrintWriter(sw);
        throwable.printStackTrace(pw);
        pw.close();
        return sw.toString();
    }

    /**
     * Print fatal level log.
     *
     * @param format String to print
     * @param args args to replace the type placeholder
     * @return HiLog return value
     * @since 1
     */
    public int fatal(@HiLogConstString String format, Object... args) {
        if (level <= LEVEL_FATAL) {
            return HiLog.fatal(label, format, args);
        }
        return LOGGER_OFF;
    }

    /**
     * Print fatal level log, contains throwable stack trace.
     *
     * @param format String to print
     * @param throwable The throwable to print
     * @param args args to replace the type placeholder
     * @return HiLog return value
     * @since 3
     */
    public int fatal(@HiLogConstString String format, Throwable throwable, Object... args) {
        return LOGGER_OFF;
    }

    /**
     * Print begin message in debug level log.
     *
     * @param msg Message to print
     * @since 3
     */
    public void begin(String msg) {
        if (level <= LEVEL_DEBUG) {
            debug("%{public}s begin", msg);
        }
    }

    /**
     * Print end message in debug level log.
     *
     * @param msg Message to print
     * @since 3
     */
    public void end(String msg) {
        if (level <= LEVEL_DEBUG) {
            debug("%{public}s end", msg);
        }
    }

    /**
     * Get the label of current logger.
     *
     * @return The label of current logger
     * @since 1
     */
    public HiLogLabel getLabel() {
        return label;
    }
}
