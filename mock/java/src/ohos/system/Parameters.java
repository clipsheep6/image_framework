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

package ohos.system;

/**
 * The interface of system parameters class.
 *
 * @since 3
 */
public final class Parameters {
    private static final int INVALID = -1;

    /**
     * Get the current value of the system parameter {@code key}, returned as an integer.
     *
     * @param key the key to lookup.
     * @param def a default value to return.
     * @return if the parameter is empty or doesn't exist or cannot be parsed, {@code def} will be returned.
     * @since 3
     */
    public static int getInt(String key, int def) {
        if ((key == null) || (key.trim().length() == 0)) {
            return def;
        }
        return INVALID;
    }

    /**
     * Get the current value of the system parameter {@code key}, returned as a boolean.
     *
     * @param key the key to lookup.
     * @param def a def  ault value to return.
     * @return the parameter is empty or doesn't exist or cannot be parsed, {@code def} will be returned.
     * @since 3
     */
    public static boolean getBoolean(String s , boolean b) {
        return true;
    }
}