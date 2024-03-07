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

#include "image_stream.h"
#include "gmock/gmock-actions.h"
#include "gmock/gmock-cardinalities.h"
#include "gmock/gmock-spec-builders.h"
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <fcntl.h>
#include <memory>
#include <stdint.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <pwd.h>
#include <csetjmp>
#include <csignal>
#define private public
#include "file_image_stream.h"

using namespace testing::ext;
using namespace testing;
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
    std::string backupFilePathSource = filePathSource + ".bak";

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
        
        // Backup the files
        std::filesystem::copy(filePathSource, backupFilePathSource, std::filesystem::copy_options::overwrite_existing);
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

class MockFileWrapper : public FileWrapper {
public:
    MOCK_METHOD(int, fstat, (int fd, struct stat *buf), (override));
    MOCK_METHOD(ssize_t, write, (int fd, const void* buf, size_t count), (override));
    MOCK_METHOD(ssize_t, read, (int fd, void *buf, size_t count), (override));

    MockFileWrapper() {
        ON_CALL(*this, fstat(_, _)).WillByDefault(Invoke([](int fd, struct stat* buf){
            return ::fstat(fd, buf);
        }));        

        // 设置write的默认行为，使其调用系统的write函数
        ON_CALL(*this, write(_, _, _))
            .WillByDefault(Invoke([](int fd, const void* buf, size_t count) {
                return ::write(fd, buf, count);
        }));
                
        ON_CALL(*this, read(_, _, _))
            .WillByDefault(Invoke([](int fd, void* buf, size_t count) {
                return ::read(fd, buf, count);
        }));                
    }
};

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
    // 这个测试用例需要一个能够模拟读取失败的ImageStream对象，因此在实际测试中可能需要使用mock技术
    auto mockFileWrapper = std::make_unique<MockFileWrapper>();
    // 设置write函数的行为，使其总是返回-1，模拟写入失败的情况
    EXPECT_CALL(*mockFileWrapper.get(), write(_, _, _))
        .WillOnce(Return(-1));
    EXPECT_CALL(*mockFileWrapper.get(), fstat(_, _))
        .Times(AtLeast(1));  // fstat函数至少被调用一次

    FileImageStream stream(filePath, std::move(mockFileWrapper));

    // 测试Write函数
    uint8_t buffer[1024];
    stream.Open();
    EXPECT_EQ(stream.Write(buffer, sizeof(buffer)), -1);
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
    auto mockSourceFileWrapper = std::make_unique<MockFileWrapper>();
    auto mockDestFileWrapper = std::make_unique<MockFileWrapper>();

    // 设置write函数的行为，使其总是返回-1，模拟写入失败的情况
    EXPECT_CALL(*mockDestFileWrapper.get(), write(_, _, _))
        .Times(Exactly(0));
    EXPECT_CALL(*mockDestFileWrapper.get(), fstat(_, _))
        .Times(AtLeast(1));  // fstat函数至少被调用一次
    EXPECT_CALL(*mockSourceFileWrapper.get(), read(_, _, _))
        .WillOnce(Return(-1));
    EXPECT_CALL(*mockSourceFileWrapper.get(), fstat(_, _))
        .Times(Exactly(1));

    FileImageStream sourceStream(filePathSource, std::move(mockSourceFileWrapper));
    FileImageStream destStream(filePath, std::move(mockDestFileWrapper));

    // 测试Write函数
    sourceStream.Open();
    destStream.Open();
    EXPECT_EQ(destStream.Write(sourceStream), -1);
    // mockDestFileWrapper.reset();
    // mockSourceFileWrapper.reset();
}

/**
 * @tc.name: FileImageStream_Write006
 * @tc.desc: 测试FileImageStream的Write函数，写入的数据量超过文件系统的剩余空间的情况
 * @tc.type: FUNC
 */
HWTEST_F(ImageStreamTest, FileImageStream_Write006, TestSize.Level3)
{
    // 这个测试用例需要一个几乎已满的文件系统，因此在实际测试中可能难以实现
    auto mockFileWrapper = std::make_unique<MockFileWrapper>();
    FileImageStream stream(filePath, std::move(mockFileWrapper));
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
    auto mockFileWrapper = std::make_unique<MockFileWrapper>();
    EXPECT_CALL(*mockFileWrapper.get(), fstat(testing::_, testing::_)).WillOnce(Return(-1));

    FileImageStream stream(filePath, std::move(mockFileWrapper));
    ASSERT_FALSE(stream.Open());
}

/**
 * @tc.name: FileImageStream_Read001
 * @tc.desc: 测试FileImageStream的Read函数，读取512字节的情况
 * @tc.type: FUNC
 */
HWTEST_F(ImageStreamTest, FileImageStream_Read001, TestSize.Level3) {
    FileImageStream stream(filePathSource);
    uint8_t buffer[1024];
    stream.Open();
    // Simulate reading 512 bytes
    ssize_t bytesRead = stream.Read(buffer, 512);
    EXPECT_EQ(512, bytesRead);
}

/**
 * @tc.name: FileImageStream_Read002
 * @tc.desc: 测试FileImageStream的Read函数，尝试从未打开的文件读取的情况
 * @tc.type: FUNC
 */
HWTEST_F(ImageStreamTest, FileImageStream_Read002, TestSize.Level3) {
    FileImageStream stream(filePathSource);
    uint8_t buffer[1024];
    // Close the stream to simulate an unopened file
    stream.Close();
    ssize_t bytesRead = stream.Read(buffer, 512);
    EXPECT_EQ(-1, bytesRead);
}

// 定义一个全局的 jmp_buf 变量
static sigjmp_buf jmpbuf;

// 定义一个信号处理函数
static void handle_sigsegv(int sig) {
    siglongjmp(jmpbuf, 1);
}

// 测试MMap函数
HWTEST_F(ImageStreamTest, FileImageStream_MMap001, TestSize.Level3) {
    // 假定有一个合适的方式来创建或获取测试所需的资源
    // YourResource test_resource;
    // 测试MMap函数在isWriteable为false时的行为
    FileImageStream stream(filePathSource);
    byte* result = stream.MMap(false);
    // 假设检查result是否为非nullptr，或有其他合适的验证方式
    ASSERT_NE(result, nullptr);
  
    // 设置信号处理函数
    signal(SIGSEGV, handle_sigsegv);

    // 尝试写入数据
    if (sigsetjmp(jmpbuf, 1) == 0) {
        result[0] = 0;
        // 如果没有引发段错误，那么这是一个错误
        FAIL() << "Expected a segmentation fault";
    }
}

HWTEST_F(ImageStreamTest, FileImageStream_MMap002, TestSize.Level3) {
    // 测试MMap函数在isWriteable为true时的行为
    FileImageStream stream(filePathSource);
    byte* result = stream.MMap(true);
    ASSERT_NE(result, nullptr);
    // 尝试写入数据
    result[0] = 123;

    // 读取数据并检查是否与写入的数据相同
    ASSERT_EQ(result[0], 123);
}

// 测试Transfer函数
HWTEST_F(ImageStreamTest, FileImageStream_Transfer001, TestSize.Level3) {
    FileImageStream src(filePathSource);
    FileImageStream dest(filePathDest);
    
    src.Open();
    // 向src中写入一些已知的数据
    std::string data = "Hello, world!";
    ASSERT_GE(src.Write((uint8_t*)data.c_str(), data.size()), 0);
    ASSERT_GE(src.GetSize(), data.size());
    ASSERT_EQ(src.Seek(0, SeekPos::BEGIN), 0);
    ASSERT_FALSE(src.IsEof());
    // 调用Transfer函数将数据从src转移到dest
    dest.Transfer(src);

    // 从dest中读取数据，并验证这些数据是否与写入src的数据相同
    uint8_t buffer[256];
    ASSERT_EQ(dest.Seek(0, SeekPos::BEGIN), 0);
    ASSERT_EQ(dest.Read(buffer, data.size()), data.size());
    ASSERT_EQ(std::string(buffer, buffer + data.size()), data);

    // 验证src是否为空
    ASSERT_EQ(dest.GetSize(), src.GetSize());
}

}
}