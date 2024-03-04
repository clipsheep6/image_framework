/*
 * Copyright (C) 2024 Huawei Device Co., Ltd.
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

#include <gtest/gtest.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#define private public
#include "image_stream.h"
#include "file_image_stream.h"

using namespace testing::ext;
using namespace OHOS::Media;

namespace OHOS {
namespace Media {

class ImageStreamTest : public testing::Test {
public:
    ImageStreamTest() {}
    ~ImageStreamTest() {}
};

HWTEST_F(ImageStreamTest, FileImageStream_Mock001, TestSize.Level3){
    // MockImageStream mock(1);
    // ASSERT_EQ(1, mock.GetIndex());
}

/**
 * @tc.name: ImageReceiver001
 * @tc.desc: test SaveBufferAsImage buffer is not nullptr
 * @tc.type: FUNC
 */
HWTEST_F(ImageStreamTest, FileImageStream_Write001, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageStreamTest: FileImageStream_Write001 start";
    std::string filePath = "/data/local/tmp/image/testfile.txt";
    // Create a FileImageStream object
    FileImageStream stream(filePath);

    // Open the file
    ASSERT_TRUE(stream.Open());

    // Create some data to write
    uint8_t data[10];
    memset(data, 'a', sizeof(data));

    // Write the data to the file
    size_t bytesWritten = stream.Write(data, sizeof(data));

    // Check that the correct number of bytes were written
    ASSERT_EQ(bytesWritten, sizeof(data));

    // Close the file
    stream.Close();

    // Open the file again
    int fd = open(filePath.c_str(), O_RDONLY);
    ASSERT_NE(fd, -1);

    // Read the data from the file
    uint8_t buffer[10];
    ssize_t bytesRead = read(fd, buffer, sizeof(buffer));

    // Check that the correct number of bytes were read
    ASSERT_EQ(bytesRead, sizeof(data));

    // Check that the data read is the same as the data written
    ASSERT_EQ(memcmp(data, buffer, sizeof(data)), 0);

    // Close the file
    close(fd);

    GTEST_LOG_(INFO) << "ImageStreamTest: FileImageStream_Write001 end";
}

}
}