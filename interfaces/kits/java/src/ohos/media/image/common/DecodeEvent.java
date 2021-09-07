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

import ohos.utils.system.SystemCap;
import ohos.utils.system.SystemCapability;

/**
 * Enumerates the events during the decoding process.
 * It should be in sync with native definition.
 *
 * @Syscap {@link SystemCapability.Multimedia#IMAGE}
 * @since 4
 */
@SystemCap("SystemCapability.Multimedia.Image")
public enum DecodeEvent {
    /**
     * Indicates that decoding is complete.
     */
    EVENT_COMPLETE_DECODE(0),

    /**
     * Indicates that partial decoding is complete.
     */
    EVENT_PARTIAL_DECODE(1),

    /**
     * Indicates that the file header has been decoded.
     *
     * @since 5
     */
    EVENT_HEADER_DECODE(2),

    /**
     * Indicates the end of the event list.
     *
     * @since 5
     */
    EVENT_LAST(3);


    private final int decodeEventValue;

    DecodeEvent(int value) {
        this.decodeEventValue = value;
    }

    /**
     * Creates a {@code DecodeEvent} instance based on the passed enum value.
     *
     * @param val Indicates the enum value of the event.
     * @return Returns the created {@code DecodeEvent}.
     * @since 5
     */
    public static DecodeEvent getDecodeEvent(int val) {
        for (DecodeEvent event : DecodeEvent.values()) {
            if (event.getValue() == val) {
                return event;
            }
        }
        return DecodeEvent.EVENT_LAST;
    }

    /**
     * Obtains the value of this enum.
     *
     * @return Returns the enum value.
     * @since 4
     */
    public int getValue() {
        return decodeEventValue;
    }
}
