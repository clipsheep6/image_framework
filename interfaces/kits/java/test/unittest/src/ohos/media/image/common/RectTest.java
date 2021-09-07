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
 * RectTest, test cases for Rect class, mainly including test cases for rect operations
 *
 * @since 1
 */
public class RectTest {
    /**
     * @tc.name: testRect001
     * @tc.desc: create rect info for image operations.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testRect001() {
        /**
         * @tc.steps: step1.create image rect info .
         * @tc.expected: step1.rect info not null
         */
        Rect rect1 = new Rect();
        Rect rect2 = new Rect(1, 2, 3, 4);
        assertNotNull(rect1);
        assertNotNull(rect2);
        assertEquals(rect2.minX, 1);
        assertEquals(rect2.minY, 2);
        assertEquals(rect2.width, 3);
        assertEquals(rect2.height, 4);
        Rect rect3 = new Rect(rect2);
        assertEquals(rect3.minX, 1);
        assertEquals(rect3.minY, 2);
        assertEquals(rect3.width, 3);
        assertEquals(rect3.height, 4);
        assertEquals(rect2, rect3);
        assertEquals(String.valueOf(rect2), String.valueOf(rect3));
        HashSet<Rect> sets = new HashSet<>();
        sets.add(rect3);
        assertTrue(sets.contains(rect3));

        /**
         * @tc.steps: step2.check rect info.
         * @tc.expected: step2.check information ok
         */
        rect2.setEmpty();
        assertEquals(rect2.minX, 0);
        assertEquals(rect2.minY, 0);
        assertEquals(rect2.width, 0);
        assertEquals(rect2.height, 0);
    }

    /**
     * @tc.name: testRect002
     * @tc.desc: for image crop operations.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testRect002() {
        /**
         * @tc.steps: step1.create image rect info .
         * @tc.expected: step1.rect info not null
         */
        Rect rect = new Rect(0, 0, 300, 400);
        assertNotNull(rect);

        /**
         * @tc.steps: step2.crop rect.
         * @tc.expected: step2.check information ok
         */
        rect.cropRect(10, 10, 100, 200);
        assertEquals(rect.minX, 10);
        assertEquals(rect.minY, 10);
        assertEquals(rect.width, 100);
        assertEquals(rect.height, 200);
    }
}