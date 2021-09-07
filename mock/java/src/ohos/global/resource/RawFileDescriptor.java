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

package ohos.global.resource;

import java.io.Closeable;
import java.io.FileDescriptor;

/**
 * Provides the file descriptor of a raw file from {@code RawFileEntry}.
 *
 * <p>You can use the file descriptor to read data, obtain the data start position, and obtain the length of the raw
 * file. This class inherits from {@code BaseFileDescriptor}. The {@code RawFileDescriptor} instance can be obtained
 * only by using the {@link ohos.global.resource.RawFileEntry#openRawFileDescriptor()} method.
 *
 * @see RawFileEntry
 * @see BaseFileDescriptor
 * @since 3
 */
public abstract class RawFileDescriptor implements Closeable {
    /**
     * Obtains the file descriptor of the raw file.
     *
     * <p>You can use the file descriptor to read data in the raw file.
     *
     * @return Returns the file descriptor.
     * @since 3
     */
    public abstract FileDescriptor getFileDescriptor();

    /**
     * Obtains the size of the raw file.
     *
     * @return Returns the raw file size, in bytes.
     * @since 3
     */
    public abstract long getFileSize();

    /**
     * Obtains the start position of data in the raw file.
     *
     * @return Returns the start position.
     * @since 3
     */
    public abstract long getStartPosition();
}