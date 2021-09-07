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

import ohos.media.image.ImageSource;
import ohos.utils.system.SystemCap;
import ohos.utils.system.SystemCapability;

import java.io.IOException;

/**
 * Abstract base class for image processing.
 *
 * @Syscap {@link SystemCapability.Multimedia#IMAGE}
 * @since 4
 */
@SystemCap("SystemCapability.Multimedia.Image")
public abstract class Filter {
    /**
     * Clears all configurations that have not been applied to the source file.
     *
     * @return Returns this {@code Filter} object, which can be used for subsequent setting operations.
     * @since 4
     */
    public abstract Filter restore();

    /**
     * Applies the configurations to the source file.
     *
     * <p>This method requires the permission to modify the source file represented by the {@code ImageSource}
     * object. The configurations do not take effect until this method is called.
     *
     * @param source Indicates the image source file to which the configurations will be applied.
     * @return Returns the size (in bytes) of the image file edited based on the configurations if the operation is
     * successful; returns {@code -1} otherwise.
     * @throws IOException Throws this exception if the modification permission has not been obtained.
     * @since 4
     */
    public abstract long applyToSource(ImageSource source) throws IOException;
}

