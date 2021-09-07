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

package ohos.hiviewdfx;

/**
 * HiLog
 *
 * @since 1
 */
public final class HiLog {
    /**
     * Log Type 1
     */
    public static final int LOG_INIT = 1;
    /**
     * Log Type 3
     */
    public static final int LOG_CORE = 3;

    /**
     * log level debug
     */
    public static final int DEBUG = 3;
    /**
     * log level info
     */
    public static final int INFO = 4;
    /**
     * log level warn
     */
    public static final int WARN = 5;
    /**
     * log level error
     */
    public static final int ERROR = 6;
    /**
     * log level fatal
     */
    public static final int FATAL = 7;

    /**
     * print debug log
     *
     * @param label log label
     * @param format log format
     * @param args log format arguments
     * @return result of success is large than 0
     */
    public static int debug(HiLogLabel label, @HiLogConstString String format, Object[] args) {
        return DEBUG;
    }

    /**
     * print info log
     *
     * @param label log label
     * @param format log format
     * @param args log format arguments
     * @return result of success is large than 0
     */
    public static int info(HiLogLabel label, @HiLogConstString String format, Object[] args) {
        return INFO;
    }

    /**
     * print warn log
     *
     * @param label log label
     * @param format log format
     * @param args log format arguments
     * @return result of success is large than 0
     */
    public static int warn(HiLogLabel label, @HiLogConstString String format, Object[] args) {
        return WARN;
    }

    /**
     * print error log
     *
     * @param label log label
     * @param format log format
     * @param args log format arguments
     * @return result of success is large than 0
     */
    public static int error(HiLogLabel label, @HiLogConstString String format, Object[] args) {
        return ERROR;
    }

    /**
     * print fatal log
     *
     * @param label log label
     * @param format log format
     * @param args log format arguments
     * @return result of success is large than 0
     */
    public static int fatal(HiLogLabel label, @HiLogConstString String format, Object[] args) {
        return FATAL;
    }

    /**
     * whether debug
     *
     * @param domain log domain
     * @param tag log tag
     * @param level log level
     * @return true is debug, false is not debug
     */
    public static boolean isLoggable(int domain, String tag, int level) {
        return true;
    }
}