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
 * PositionTest, test cases for Position class, mainly including test cases for position operations
 *
 * @since 1
 */
public class PositionTest {
    /**
     * @tc.name: testPosition001
     * @tc.desc: create position info for image operations.
     */
    @Test
    @CaseLevel(level = Level.LEVEL_3)
    @CaseType(type = Type.FUNC)
    public void testPosition001() {
        /**
         * @tc.steps: step1.create position size info .
         * @tc.expected: step1.position info not null
         */
        Position position1 = new Position();
        Position position2 = new Position(1, 2);
        assertNotNull(position1);
        assertNotNull(position2);
        Position position3 = new Position(1, 2);
        assertEquals(position2, position3);
        assertEquals(String.valueOf(position2), String.valueOf(position3));
        HashSet<Position> sets = new HashSet<>();
        sets.add(position3);
        assertTrue(sets.contains(position3));

        /**
         * @tc.steps: step2.check position info.
         * @tc.expected: step2.check information ok
         */
        assertEquals(position2.posX, 1);
        assertEquals(position2.posY, 2);
    }
}
