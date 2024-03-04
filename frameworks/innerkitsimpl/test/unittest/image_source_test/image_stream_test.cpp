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
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <pwd.h>
#define private public
#include "image_stream.h"

using namespace testing::ext;
using namespace OHOS::Media;

namespace OHOS {
namespace Media {

class ImageStreamTest : public testing::Test {
public:
    ImageStreamTest() {}
    ~ImageStreamTest() {}
    std::string filePath = "/data/local/tmp/image/testfile.txt";
    std::string filePathSource = "/data/local/tmp/image/testfile_source.png";
    std::string filePathDest = "/data/local/tmp/image/testfile_dest.png";

    virtual void SetUp() {
        // Create the directory
        std::string dirPath = "/data/local/tmp/image";
        if(access(dirPath.c_str(), F_OK) != 0){
            int ret = mkdir(dirPath.c_str(), 0777);
            if(ret != 0){
                char buf[255];
                strerror_r(errno, buf, sizeof(buf));
                GTEST_LOG_(ERROR) << "Failed to create directory: " << dirPath << ", error: " << buf;
            }
        }
        
    }

    const static std::string tmpDirectory;
    static bool alreadyExist;

    virtual void TearDown() {
        remove(filePath.c_str());
        remove(filePathDest.c_str());
    }

    static void SetUpTestCase() {
        // Create the directory
        if(access(tmpDirectory.c_str(), F_OK) != 0){
            int ret = mkdir(tmpDirectory.c_str(), 0777);
            if(ret != 0){
                char buf[255];
                strerror_r(errno, buf, sizeof(buf));
                GTEST_LOG_(ERROR) << "Failed to create directory: " << tmpDirectory << ", error: " << buf;
            }
            alreadyExist = false;
        }else{
            alreadyExist = true;
        }
    }
    static void TearDownTestCase() {
        if(!alreadyExist){
            rmdir(tmpDirectory.c_str());
        }
    }
};

bool ImageStreamTest::alreadyExist = false;
const std::string ImageStreamTest::tmpDirectory = "/data/local/tmp/image";

HWTEST_F(ImageStreamTest, FileImageStream_Mock001, TestSize.Level3){
    // MockImageStream mock(1);
    // ASSERT_EQ(1, mock.GetIndex());
}

/**
 * @tc.name: FileImageStream_Write001
 * @tc.desc: 测试FileImageStream的Write函数，是否能正常写入并验证写入的数据
 * @tc.type: FUNC
 */
HWTEST_F(ImageStreamTest, FileImageStream_Write001, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageStreamTest: FileImageStream_Write001 start";
   
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

/**
 * @tc.name: FileImageStream_Write002
 * @tc.desc: 测试FileImageStream的Write函数，文件没有打开的情况
 * @tc.type: FUNC
 */
HWTEST_F(ImageStreamTest, FileImageStream_Write002, TestSize.Level3)
{
    FileImageStream stream(filePath);
    uint8_t data[10];
    memset(data, 'a', sizeof(data));
    ASSERT_EQ(stream.Write(data, sizeof(data)), -1);
}

/**
 * @tc.name: FileImageStream_Write003
 * @tc.desc: 测试FileImageStream的Write函数，写入的数据量超过文件系统的剩余空间的情况
 * @tc.type: FUNC
 */
HWTEST_F(ImageStreamTest, FileImageStream_Write003, TestSize.Level3)
{
    // 这个测试用例需要一个几乎已满的文件系统，因此在实际测试中可能难以实现
}

/**
 * @tc.name: FileImageStream_Write004
 * @tc.desc: 测试FileImageStream的Write函数，源图像流的数据已经全部读取的情况
 * @tc.type: FUNC
 */
HWTEST_F(ImageStreamTest, FileImageStream_Write004, TestSize.Level3)
{
    FileImageStream stream1(filePathSource);
    FileImageStream stream2(filePathDest);
    ASSERT_TRUE(stream1.Open());
    ASSERT_TRUE(stream2.Open());
    // 读取stream1的所有数据
    uint8_t buffer[4096];
    while (stream1.Read(buffer, sizeof(buffer)) > 0) {}
    // 此时stream1的数据已经全部读取，所以写入应返回0
    ASSERT_EQ(stream2.Write(stream1), 0);
    stream1.Close();
    stream2.Close();
}

/**
 * @tc.name: FileImageStream_Write005
 * @tc.desc: 测试FileImageStream的Write函数，从源图像流中读取数据失败的情况
 * @tc.type: FUNC
 */
HWTEST_F(ImageStreamTest, FileImageStream_Write005, TestSize.Level3)
{
    // 这个测试用例需要一个能够模拟读取失败的ImageStream对象，因此在实际测试中可能需要使用mock技术
}

/**
 * @tc.name: FileImageStream_Write006
 * @tc.desc: 测试FileImageStream的Write函数，写入的数据量超过文件系统的剩余空间的情况
 * @tc.type: FUNC
 */
HWTEST_F(ImageStreamTest, FileImageStream_Write006, TestSize.Level3)
{
    // 这个测试用例需要一个几乎已满的文件系统，因此在实际测试中可能难以实现
}

/**
 * @tc.name: FileImageStream_Read001
 * @tc.desc: 测试FileImageStream的Write函数，是否能正常写入并验证写入的数据
 * @tc.type: FUNC
 */
HWTEST_F(ImageStreamTest, FileImageStream_Read001, TestSize.Level3){
    FileImageStream* stream = new FileImageStream(filePath);
    delete stream;
}

/**
 * @tc.name: FileImageStream_Open001
 * @tc.desc: 测试FileImageStream的Open函数，文件路径不存在的情况
 * @tc.type: FUNC
 */
HWTEST_F(ImageStreamTest, FileImageStream_Open001, TestSize.Level3)
{
    // 测试文件路径不存在的情况
    std::string nonExistFilePath = "/data/local/tmp/image/non_exist_file.txt";
    FileImageStream stream1(nonExistFilePath);
    ASSERT_TRUE(stream1.Open());
    stream1.Close();
    remove(nonExistFilePath.c_str());
}

/**
 * @tc.name: FileImageStream_Open002
 * @tc.desc: 测试FileImageStream的Open函数，文件路径存在但不可写的情况
 * @tc.type: FUNC
 */
HWTEST_F(ImageStreamTest, FileImageStream_Open002, TestSize.Level3)
{
    // 获取当前执行用户的用户名
    uid_t uid = getuid();
    struct passwd *pw = getpwuid(uid);
    if (!pw) {
        perror("getpwuid");
        return;
    }
    std::string username(pw->pw_name);

    // 测试文件路径存在但不可写的情况
    std::string nonWritableFilePath = "/data/local/tmp/image/non_writable_file.txt";
    close(open(nonWritableFilePath.c_str(), O_CREAT, S_IRUSR));
    FileImageStream stream2(nonWritableFilePath);
    if (username == "root") {
        // 如果当前用户是root，那么可以成功打开
        ASSERT_TRUE(stream2.Open());
    } else {
        // 如果当前用户不是root，那么无法打开
        ASSERT_FALSE(stream2.Open());
    }
    remove(nonWritableFilePath.c_str());
}

/**
 * @tc.name: FileImageStream_Open003
 * @tc.desc: 测试FileImageStream的Open函数，文件已经打开的情况
 * @tc.type: FUNC
 */
HWTEST_F(ImageStreamTest, FileImageStream_Open003, TestSize.Level3)
{
    // 测试文件已经打开的情况
    FileImageStream stream3(filePath);
    ASSERT_TRUE(stream3.Open());
    ASSERT_TRUE(stream3.Open());
    stream3.Close();
}

/**
 * @tc.name: FileImageStream_Open004
 * @tc.desc: 测试FileImageStream的Open函数，获取文件大小失败的情况
 * @tc.type: FUNC
 */
HWTEST_F(ImageStreamTest, FileImageStream_Open004, TestSize.Level3)
{
    // 测试获取文件大小失败的情况
    // 这个测试可能需要一些特殊的设置或者mock技术来模拟fstat失败的情况
}

}
}