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

import ohos.utils.system.SystemCap;
import ohos.utils.system.SystemCapability;

/**
 * Parent class for the image module exception.
 *
 * @Syscap {@link SystemCapability.Multimedia#IMAGE}
 * @since 3
 */
@SystemCap("SystemCapability.Multimedia.Image")
public class ImageException extends RuntimeException {
    private static final long serialVersionUID = 6953810857206391682L;

    /**
     * A constructor used to create an {@code ImageException} instance based on the exception message.
     *
     * @param msg Indicates the string representation of the exception message.
     * @since 3
     */
    public ImageException(String msg) {
        super(msg);
    }
}