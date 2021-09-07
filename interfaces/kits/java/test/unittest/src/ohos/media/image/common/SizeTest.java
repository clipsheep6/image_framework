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

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotNull;
import static org.junit.Assert.assertTrue;

import ohos.unittest.CaseLevel;
import ohos.unittest.CaseType;
import ohos.unittest.Level;
import ohos.unittest.Type;

import org.junit.Test;

import java.util.HashSet;

/**
 * SizeTest, test cases for Size class, mainly including test cases for size operations
 *
 * @since 1
 */
public class SizeTest {
    /**
     * @tc.name: testSize001
     * @tc.desc: create size info for image operations.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testSize001() {
        /**
         * @tc.steps: step1.create image size info .
         * @tc.expected: step1.size info not null
         */
        Size size1 = new Size();
        Size size2 = new Size(1, 2);
        assertNotNull(size1);
        assertNotNull(size2);

        /**
         * @tc.steps: step2.check size info.
         * @tc.expected: step2.check information ok
         */
        assertEquals(size2.width, 1);
        assertEquals(size2.height, 2);

        Size size3 = new Size(3, 4);
        Size size4 = new Size(3, 4);
        assertEquals(size3, size4);
        assertEquals(String.valueOf(size3), String.valueOf(size4));
        HashSet<Size> sets = new HashSet<>();
        sets.add(size4);
        assertTrue(sets.contains(size4));
    }
}