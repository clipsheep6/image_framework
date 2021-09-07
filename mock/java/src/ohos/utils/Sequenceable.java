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
 * Writes objects of a class into and restores them from a {@link Parcel} during
 * inter-process communication (IPC).
 * <p>
 * This can be done during IPC only after the class inherits from {@code Sequenceable}
 * and implements the {@code marshalling} and {@code unmarshalling} methods provided in
 * {@code Sequenceable}. If the class has a final member variable whose value cannot be
 * changed by calling {@code unmarshalling}, you must also implement the {@code createFromParcel}
 * method in {@link Sequenceable.Producer}.
 * <p>
 * Example code:
 *
 * <pre>
 * public class Example implements Sequenceable {
 *     private int number;
 *
 *     private String name;
 *
 *     public boolean marshalling(Parcel out) {
 *         return out.writeInt(number) && out.writeString(name);
 *     }
 *
 *     public boolean unmarshalling(Parcel in) {
 *         this.number = in.readInt();
 *         this.name = in.readString();
 *         return true;
 *     }
 *
 *     public static final Sequenceable.Producer<Example> PRODUCER = new Sequenceable.Producer<Example>() {
 *         public Example createFromParcel(Parcel in) {
 *             // Initialize an instance first, then do customized unmarshlling.
 *             Example instance = new Example();
 *             instance.unmarshalling(in);
 *             return instance;
 *         }
 *     };
 * }
 * </pre>
 *
 * @since 1
 */
public interface Sequenceable {
    /**
     * Checks whether the implementation class of this {@code Sequenceable} contains {@code FileDescriptor} object data.
     * <p>
     * If the custom {@code Sequenceable} class contains any {@code FileDescriptor} data, you should override this
     * method.
     * This method return {@code false} by default.
     *
     * @return Returns {@code true} if it contains {@code FileDescriptor} data; returns {@code false} otherwise.
     * @since 3
     */
    default boolean hasFileDescriptor() {
        return false;
    }

    /**
     * Marshals this {@code Sequenceable} object into a {@link Parcel}.
     *
     * @param out Indicates the {@link Parcel} object to which the {@code Sequenceable}
     *        object will be marshaled..
     * @return Returns {@code true} if the marshalling is successful; returns {@code false} otherwise.
     * @throws ParcelException Throws this exception if the operation fails.
     * @since 1
     */
    boolean marshalling(Parcel out);

    /**
     * Unmarshals this {@code Sequenceable} object from a {@link Parcel}.
     *
     * @param in Indicates the {@link Parcel} object into which the {@code Sequenceable}
     *        object has been marshaled.
     * @return Returns {@code true} if the unmarshalling is successful; returns {@code false} otherwise.
     * @throws ParcelException Throws this exception if the operation fails.
     * @since 1
     */
    boolean unmarshalling(Parcel in);

    /**
     * Creates a {@code Sequenceable} instance from a {@link Parcel}.
     * If the class has a final member variable whose value cannot be changed by
     * {@link Sequenceable#unmarshalling(Parcel)}, it should contain an extra non-null static member variable
     * {@code Sequenceable.Producer}.
     *
     * @since 1
     */
    interface Producer<T> {
        /**
         * Creates a {@code Sequenceable} instance from a {@link Parcel}.
         *
         * @param parcel Indicates the {@link Parcel} object into which the {@code Sequenceable}
         *        object has been written.
         * @return Returns an instance of the {@code Sequenceable} class.
         * @since 1
         */
        T createFromParcel(Parcel parcel);
    }
}
