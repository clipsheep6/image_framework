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

package ohos.utils;

/**
 * A data object used for inter-process communication (IPC).
 * <p>
 * During IPC, the sender can use the write methods provided by {@code Parcel} to
 * write the to-be-sent data into a {@code Parcel} object in a specific format, and the receiver can use the
 * read methods provided by {@code Parcel} to read data of the specific format from the {@code Parcel} object.
 * <p>
 * <p>
 * The default capacity of a {@code Parcel} instance is 200KB. If you want more or less, use {@link #setCapacity(int)}
 * to change it.
 * </p>
 * <b>Note</b>: Only data of the following data types can be written into or read from a {@code Parcel}: byte,
 * byteArray, short, shortArray, int, intArray, long, longArray, float, floatArray, double, doubleArray, boolean,
 * booleanArray, char, charArray, String, StringArray, {@link PlainBooleanArray}, {@link Serializable},
 * {@link Sequenceable}, and SequenceableArray.
 *
 * @since 1
 */
public class Parcel {
    private static final int INVALID = -1;

    protected Parcel() {
    }

    /**
     * Reads an integer value from the {@code Parcel} instance.
     *
     * @return Returns an integer value.
     *
     * @since 1
     */
    public final int readInt() {
        return INVALID;
    }
}
