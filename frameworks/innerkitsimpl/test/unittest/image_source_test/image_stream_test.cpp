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

// Standard library includes
#include "image_stream.h"
#include "out/rk3568/obj/third_party/musl/intermidiates/linux/musl_src_ported/include/stdio.h"
#include <csetjmp>
#include <csignal>
#include <dirent.h>
#include <fcntl.h>
#include <iostream>
#include <memory>
#include <pwd.h>
#include <stdint.h>
#include <string.h>
#include <string>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

// Google Test includes
#include "gmock/gmock-actions.h"
#include "gmock/gmock-cardinalities.h"
#include "gmock/gmock-spec-builders.h"
#include <gmock/gmock.h>
#include <gtest/gtest.h>

// Macros
#define private public

// Local includes
#include "buffer_image_stream.h"
#include "data_buf.h"
#include "file_image_stream.h"

using namespace testing::ext;
using namespace testing;
using namespace OHOS::Media;

namespace OHOS {
namespace Media {

#define SIZE_1024 1024
#define SIZE_512 512
#define SIZE_255 255
#define SIZE_20 20
#define SIZE_10 10
#define TEST_DIR_PERMISSIONS 0777

class ImageStreamTest : public testing::Test {
  public:
    ImageStreamTest() {}
    ~ImageStreamTest() {}
    std::string filePath = "/data/local/tmp/image/testfile.txt";
    std::string filePathSource = "/data/local/tmp/image/test_exif_test.jpg";
    std::string filePathDest = "/data/local/tmp/image/testfile_dest.png";
    std::string backupFilePathSource = "/data/local/tmp/image/test_exif.jpg";

    virtual void SetUp() {
        // Create the directory
        std::string dirPath = "/data/local/tmp/image";
        if (access(dirPath.c_str(), F_OK) != 0) {
            int ret = mkdir(dirPath.c_str(), TEST_DIR_PERMISSIONS);
            if (ret != 0) {
                char buf[IMAGE_STREAM_ERROR_BUFFER_SIZE];
                strerror_r(errno, buf, sizeof(buf));
                GTEST_LOG_(ERROR) << "Failed to create directory: " << dirPath
                                  << ", error: " << buf;
            }
        }

        // Backup the files
        std::filesystem::copy(
            backupFilePathSource, filePathSource,
            std::filesystem::copy_options::overwrite_existing);
    }

    const static std::string tmpDirectory;
    static bool alreadyExist;

    virtual void TearDown() {
        remove(filePath.c_str());
        remove(filePathDest.c_str());
    }

    static void SetUpTestCase() {
        // Create the directory
        if (access(tmpDirectory.c_str(), F_OK) != 0) {
            int ret = mkdir(tmpDirectory.c_str(), TEST_DIR_PERMISSIONS);
            if (ret != 0) {
                char buf[IMAGE_STREAM_ERROR_BUFFER_SIZE];
                strerror_r(errno, buf, sizeof(buf));
                GTEST_LOG_(ERROR)
                    << "Failed to create directory: " << tmpDirectory
                    << ", error: " << buf;
            }
            alreadyExist = false;
        } else {
            alreadyExist = true;
        }
    }
    static void TearDownTestCase() {
        if (!alreadyExist) {
            rmdir(tmpDirectory.c_str());
        }
    }
};

bool ImageStreamTest::alreadyExist = false;
const std::string ImageStreamTest::tmpDirectory = "/data/local/tmp/image";

class MockFileWrapper : public FileWrapper {
  public:
    MOCK_METHOD(size_t, fwrite,
                (const void *src, size_t size, size_t nmemb, FILE *file),
                (override));
    MOCK_METHOD(size_t, fread,
                (void *destv, size_t size, size_t nmemb, FILE *file),
                (override));

    MockFileWrapper() {
        // Set the default behavior of write to call the system's write function
        ON_CALL(*this, fwrite(_, _, _, _))
            .WillByDefault(Invoke(
                [](const void *src, size_t size, size_t nmemb, FILE *file) {
                    return ::fwrite(src, size, nmemb, file);
                }));

        // Set the default behavior of read to call the system's read function
        ON_CALL(*this, fread(_, _, _, _))
            .WillByDefault(
                Invoke([](void *destv, size_t size, size_t nmemb, FILE *file) {
                    return ::fread(destv, size, nmemb, file);
                }));
    }
};

/**
 * @tc.name: FileImageStream_Write001
 * @tc.desc: Test the Write function of FileImageStream, whether it can write
 * normally and verify the written data
 * @tc.type: FUNC
 */
HWTEST_F(ImageStreamTest, FileImageStream_Write001, TestSize.Level3) {
    // GTEST_LOG_(INFO) << "ImageStreamTest: FileImageStream_Write001 start";

    // Create a FileImageStream object
    FileImageStream stream(filePath);

    // Open the file
    ASSERT_TRUE(stream.Open());

    // Create some data to write
    byte data[SIZE_10];
    memset(data, 'a', sizeof(data));

    ASSERT_EQ(stream.Tell(), 0);

    // Write the data to the file
    size_t bytesWritten = stream.Write(data, sizeof(data));

    // Check that the correct number of bytes were written
    ASSERT_EQ(bytesWritten, sizeof(data));
    ASSERT_EQ(bytesWritten, 10);

    // Flush the file
    stream.Flush();

    // Open the file again
    int fileDescriptor = open(filePath.c_str(), O_RDONLY);
    ASSERT_NE(fileDescriptor, -1);

    // Read the data from the file
    byte buffer[SIZE_10];
    ssize_t bytesRead = read(fileDescriptor, buffer, sizeof(buffer));

    // Check that the correct number of bytes were read
    ASSERT_EQ(bytesRead, sizeof(data));

    // Check that the data read is the same as the data written
    ASSERT_EQ(memcmp(data, buffer, sizeof(data)), 0);

    // Close the file
    close(fileDescriptor);

    // GTEST_LOG_(INFO) << "ImageStreamTest: FileImageStream_Write001 end";
}

/**
 * @tc.name: FileImageStream_Write002
 * @tc.desc: Test the Write function of FileImageStream when the file is not
 * open
 * @tc.type: FUNC
 */
HWTEST_F(ImageStreamTest, FileImageStream_Write002, TestSize.Level3) {
    FileImageStream stream(filePath);
    byte data[SIZE_10];
    memset(data, 'a', sizeof(data));
    ASSERT_EQ(stream.Write(data, sizeof(data)), -1);
}

/**
 * @tc.name: FileImageStream_Write003
 * @tc.desc: Test the Write function of FileImageStream when the amount of data
 * written exceeds the remaining space in the file system
 * @tc.type: FUNC
 */
HWTEST_F(ImageStreamTest, FileImageStream_Write003, TestSize.Level3) {
    // This test case requires an ImageStream object that can simulate a read
    // failure, so mock technology may be needed in actual testing
    auto mockFileWrapper = std::make_unique<MockFileWrapper>();
    // Set the behavior of the write function to always return -1, simulating a
    // write failure
    EXPECT_CALL(*mockFileWrapper.get(), fwrite(_, _, _, _))
        .WillOnce(Return(-1));

    FileImageStream stream(filePath, std::move(mockFileWrapper));

    // Test the Write function
    byte buffer[SIZE_1024];
    stream.Open();
    EXPECT_EQ(stream.Write(buffer, sizeof(buffer)), -1);
}

/**
 * @tc.name: FileImageStream_Write004
 * @tc.desc: Test the Write function of FileImageStream when all data from the
 * source image stream has been read
 * @tc.type: FUNC
 */
HWTEST_F(ImageStreamTest, FileImageStream_Write004, TestSize.Level3) {
    FileImageStream stream1(filePathSource);
    FileImageStream stream2(filePathDest);
    ASSERT_TRUE(stream1.Open());
    ASSERT_TRUE(stream2.Open());
    // Read all data from stream1
    byte buffer[IMAGE_STREAM_PAGE_SIZE];
    while (stream1.Read(buffer, sizeof(buffer)) > 0) {
    }
    // At this point, all data from stream1 has been read, so the write should
    // return 0
    ASSERT_EQ(stream2.Write(stream1), 0);
    stream1.Flush();
    stream2.Flush();
}

/**
 * @tc.name: FileImageStream_Write005
 * @tc.desc: Test the Write function of FileImageStream when reading data from
 * the source image stream fails
 * @tc.type: FUNC
 */
HWTEST_F(ImageStreamTest, FileImageStream_Write005, TestSize.Level3) {
    // This test case requires an ImageStream object that can simulate a read
    // failure, so mock technology may be needed in actual testing
    auto mockSourceFileWrapper = std::make_unique<MockFileWrapper>();
    auto mockDestFileWrapper = std::make_unique<MockFileWrapper>();

    // Set the behavior of the write function to always return -1, simulating a
    // write failure
    EXPECT_CALL(*mockDestFileWrapper.get(), fwrite(_, _, _, _))
        .Times(Exactly(0));
    EXPECT_CALL(*mockSourceFileWrapper.get(), fread(_, _, _, _))
        .WillOnce(Return(-1));

    FileImageStream sourceStream(filePathSource,
                                 std::move(mockSourceFileWrapper));
    FileImageStream destStream(filePath, std::move(mockDestFileWrapper));

    // Test the Write function
    sourceStream.Open();
    destStream.Open();
    EXPECT_EQ(destStream.Write(sourceStream), -1);
}

/**
 * @tc.name: FileImageStream_Write006
 * @tc.desc: Test the Write function of FileImageStream when the amount of data
 * written exceeds the remaining space in the file system
 * @tc.type: FUNC
 */
HWTEST_F(ImageStreamTest, FileImageStream_Write006, TestSize.Level3) {
    // This test case requires a nearly full file system, so it may be difficult
    // to implement in actual testing
    auto mockFileWrapper = std::make_unique<MockFileWrapper>();
    FileImageStream stream(filePath, std::move(mockFileWrapper));
}

/**
 * @tc.name: FileImageStream_Write001
 * @tc.desc: Test the Write function of FileImageStream, whether it can write
 * normally and verify the written data
 * @tc.type: FUNC
 */
HWTEST_F(ImageStreamTest, FileImageStream_Write007, TestSize.Level3) {
    // GTEST_LOG_(INFO) << "ImageStreamTest: FileImageStream_Write001 start";

    // Create a FileImageStream object
    FileImageStream stream(filePath);

    // Open the file
    ASSERT_TRUE(stream.Open());

    // Create some data to write
    std::string data = "1Hello, world!";

    ASSERT_EQ(stream.Seek(0, SeekPos::BEGIN), 0);
    ASSERT_EQ(stream.Tell(), 0);
    // Write the data to the file
    ASSERT_EQ(stream.Write((byte *)data.c_str(), data.size()), data.size());
    ASSERT_EQ(stream.Tell(), data.size());
    // Flush the file
    stream.Flush();

    // Open the file again
    int fileDescriptor = open(filePath.c_str(), O_RDONLY);
    ASSERT_NE(fileDescriptor, -1);

    // Read the data from the file
    byte buffer[SIZE_20];
    read(fileDescriptor, buffer, sizeof(buffer));
    // ssize_t bytesRead = read(fd, buffer, sizeof(buffer));

    // Check that the correct number of bytes were read
    // ASSERT_EQ(bytesRead, data.size());

    // Check that the data read is the same as the data written
    ASSERT_EQ(memcmp(data.c_str(), buffer, data.size()), 0);

    // Close the file
    close(fileDescriptor);

    // GTEST_LOG_(INFO) << "ImageStreamTest: FileImageStream_Write001 end";
}

/**
 * @tc.name: FileImageStream_Open001
 * @tc.desc: Test the Open function of FileImageStream when the file path does
 * not exist
 * @tc.type: FUNC
 */
HWTEST_F(ImageStreamTest, FileImageStream_Open001, TestSize.Level3) {
    // Test the case where the file path does not exist
    std::string nonExistFilePath = "/data/local/tmp/image/non_exist_file.txt";
    remove(nonExistFilePath.c_str());
    FileImageStream stream1(nonExistFilePath);
    ASSERT_TRUE(stream1.Open());
    std::string sourceData = "Hello, world!";
    stream1.Write((byte *)sourceData.c_str(), sourceData.size());
    // Read data from stream1
    byte buffer[SIZE_255];
    stream1.Seek(0, SeekPos::BEGIN);
    ssize_t bytesRead = stream1.Read(buffer, sourceData.size());
    ASSERT_EQ(bytesRead, sourceData.size());
    buffer[bytesRead] = '\0'; // Add string termination character
    // Check if the read data is the same as the written data
    ASSERT_STREQ((char *)buffer, sourceData.c_str());
    ASSERT_TRUE(stream1.Flush());
    remove(nonExistFilePath.c_str());
}

/**
 * @tc.name: FileImageStream_Open002
 * @tc.desc: Test the Open function of FileImageStream when the file path exists
 * but is not writable
 * @tc.type: FUNC
 */
HWTEST_F(ImageStreamTest, FileImageStream_Open002, TestSize.Level3) {
    // Get the username of the current user
    uid_t uid = getuid();
    struct passwd *passwordEntry = getpwuid(uid);
    if (passwordEntry == nullptr) {
        perror("getpwuid");
        return;
    }
    std::string username(passwordEntry->pw_name);

    // Test the case where the file path exists but is not writable
    std::string nonWritableFilePath =
        "/data/local/tmp/image/non_writable_file.txt";
    close(open(nonWritableFilePath.c_str(), O_CREAT, S_IRUSR));
    FileImageStream stream2(nonWritableFilePath);
    if (username == "root") {
        // If the current user is root, then it can be opened successfully
        ASSERT_TRUE(stream2.Open());
    } else {
        // If the current user is not root, then it cannot be opened
        ASSERT_FALSE(stream2.Open());
    }
    remove(nonWritableFilePath.c_str());
}

/**
 * @tc.name: FileImageStream_Open003
 * @tc.desc: Test the Open function of FileImageStream when the file is already
 * open
 * @tc.type: FUNC
 */
HWTEST_F(ImageStreamTest, FileImageStream_Open003, TestSize.Level3) {
    // Test the case where the file is already open
    FileImageStream stream3(filePath);
    ASSERT_TRUE(stream3.Open());
    ASSERT_TRUE(stream3.Open());
    ASSERT_TRUE(stream3.Flush());
}

/**
 * @tc.name: FileImageStream_Open004
 * @tc.desc: Test the Open function of FileImageStream when the file does not
 * exist
 * @tc.type: FUNC
 */
HWTEST_F(ImageStreamTest, FileImageStream_Open004, TestSize.Level3) {
    // Test the case where the file does not exist
    const char *nonExistentFilePath = "/path/to/nonexistent/file";
    FileImageStream stream(nonExistentFilePath);
    ASSERT_FALSE(stream.Open(OpenMode::Read));
}

/**
 * @tc.name: FileImageStream_Open005
 * @tc.desc: Test the Open twice
 * @tc.type: FUNC
 */
HWTEST_F(ImageStreamTest, FileImageStream_Open005, TestSize.Level3) {
    FileImageStream stream(filePathSource);
    ASSERT_TRUE(stream.Open(OpenMode::Read));
    ASSERT_TRUE(stream.Open(OpenMode::Read));
}

/**
 * @tc.name: FileImageStream_Read001
 * @tc.desc: Test the Read function of FileImageStream, reading 512 bytes
 * @tc.type: FUNC
 */
HWTEST_F(ImageStreamTest, FileImageStream_Read001, TestSize.Level3) {
    FileImageStream stream(filePathSource);
    byte buffer[SIZE_1024];
    stream.Open();
    // Simulate reading 512 bytes
    ssize_t bytesRead = stream.Read(buffer, SIZE_512);
    EXPECT_EQ(SIZE_512, bytesRead);
}

/**
 * @tc.name: FileImageStream_Read002
 * @tc.desc: Test the Read function of FileImageStream, trying to read from a
 * file that has not been opened
 * @tc.type: FUNC
 */
HWTEST_F(ImageStreamTest, FileImageStream_Read002, TestSize.Level3) {
    FileImageStream stream(filePathSource);
    byte buffer[SIZE_1024];
    // Flush the stream to simulate an unopened file
    ASSERT_FALSE(stream.Flush());
    ssize_t bytesRead = stream.Read(buffer, SIZE_512);
    EXPECT_EQ(-1, bytesRead);
}

// Define a global jmp_buf variable
static sigjmp_buf jmpbuf;

// Define a signal handler function
static void handle_sigsegv(int sig) { siglongjmp(jmpbuf, 1); }

/**
 * @tc.name: FileImageStream_MMap001
 * @tc.desc: Test the MMap function of FileImageStream, trying to write to a
 * memory-mapped file that is not writable
 * @tc.type: FUNC
 */
HWTEST_F(ImageStreamTest, FileImageStream_MMap001, TestSize.Level3) {
    // Assume there is an appropriate way to create or obtain the resources
    // needed for the test YourResource test_resource; Test the behavior of the
    // MMap function when isWriteable is false
    FileImageStream stream(filePathSource);
    byte *result = stream.MMap(false);
    // Assume that checking whether result is not nullptr, or there is another
    // appropriate verification method
    ASSERT_EQ(result, nullptr);
    stream.Open();
    result = stream.MMap(false);
    ASSERT_NE(result, nullptr);

    // Set the signal handler function
    signal(SIGSEGV, handle_sigsegv);

    // Try to write data
    if (sigsetjmp(jmpbuf, 1) == 0) {
        result[0] = 0;
        // If no segmentation fault is triggered, then this is an error
        FAIL() << "Expected a segmentation fault";
    }
}

/**
 * @tc.name: FileImageStream_MMap002
 * @tc.desc: Test the MMap function of FileImageStream, trying to write to a
 * memory-mapped file that is writable
 * @tc.type: FUNC
 */
HWTEST_F(ImageStreamTest, FileImageStream_MMap002, TestSize.Level3) {
    // Test the behavior of the MMap function when isWriteable is true
    FileImageStream stream(filePathSource);
    ASSERT_TRUE(stream.Open());
    byte *result = stream.MMap(true);
    ASSERT_NE(result, nullptr);
    // Try to write data
    result[0] = 123;

    // Read the data and check if it is the same as the written data
    ASSERT_EQ(result[0], 123);
}

/**
 * @tc.name: FileImageStream_MMap003
 * @tc.desc: Test whether the MMap function of FileImageStream can actually
 * modify the content of the file
 * @tc.type: FUNC
 */
HWTEST_F(ImageStreamTest, FileImageStream_MMap003, TestSize.Level3) {
    // Test whether MMap can actually modify the content of the file
    FileImageStream stream(filePathSource);
    ASSERT_TRUE(stream.Open());
    byte *result = stream.MMap(true);
    ASSERT_NE(result, nullptr);

    // Try to write data
    result[0] = 123;

    stream.Seek(0, SeekPos::BEGIN);
    byte buffer[1];
    ASSERT_EQ(stream.Read(buffer, 1), 1);
    ASSERT_EQ(buffer[0], 123);

    // Flush stream
    ASSERT_TRUE(stream.Flush());
    FileImageStream checkStream(filePathSource);
    checkStream.Open();
    byte checkBuffer[1];
    ASSERT_EQ(checkStream.Read(checkBuffer, 1), 1);

    // Check if the data in the file is the same as the data written
    ASSERT_EQ(checkBuffer[0], 123);
}

/**
 * @tc.name: FileImageStream_CopyFrom001
 * @tc.desc: Test the CopyFrom function of FileImageStream, copying data from
 * one stream to another
 * @tc.type: FUNC
 */
HWTEST_F(ImageStreamTest, FileImageStream_CopyFrom001, TestSize.Level3) {
    FileImageStream src(filePathSource);
    FileImageStream dest(filePathDest);

    src.Open();
    // Write some known data to src
    std::string data = "Hello, world!";
    ASSERT_EQ(src.Tell(), 0);
    ASSERT_GE(src.Write((byte *)data.c_str(), data.size()), 0);
    ASSERT_TRUE(src.Flush());
    // Call the Transfer function to transfer data from src to dest
    dest.Open();
    ASSERT_TRUE(dest.CopyFrom(src));
    // Read data from dest and verify that it is the same as the data written to
    // src
    byte buffer[SIZE_255];
    memset(buffer, 0, SIZE_255);
    ASSERT_EQ(dest.Seek(0, SeekPos::BEGIN), 0);
    ASSERT_EQ(dest.Read(buffer, data.size()), data.size());
    ASSERT_EQ(std::string(buffer, buffer + data.size()), data);

    // Verify that src is empty
    ASSERT_EQ(dest.GetSize(), src.GetSize());
}

HWTEST_F(ImageStreamTest, FileImageStream_CopyFrom002, TestSize.Level3) {
    BufferImageStream src;
    FileImageStream dest(filePathDest);
    ASSERT_TRUE(src.Open());
    ASSERT_EQ(dest.Open(), true);
    src.Write((byte *)"Hello, world!", 13);
    ASSERT_TRUE(dest.CopyFrom(src));
    dest.Seek(0, SeekPos::BEGIN);
    char buf[14] = {0};
    dest.Read((byte *)buf, 13);
    ASSERT_STREQ(buf, "Hello, world!");
    ASSERT_EQ(memcmp(src.MMap(), dest.MMap(), 13), 0);
}

HWTEST_F(ImageStreamTest, FileImageStream_CopyFrom003, TestSize.Level3) {
    BufferImageStream src;
    FileImageStream dest(filePathDest);
    src.Open();
    dest.Open();
    char buff[IMAGE_STREAM_PAGE_SIZE + 1] = {0};
    src.Write((byte *)buff, IMAGE_STREAM_PAGE_SIZE + 1);
    ASSERT_TRUE(dest.CopyFrom(src));
    ASSERT_EQ(src.GetSize(), dest.GetSize());
    ASSERT_EQ(memcmp(src.MMap(), dest.MMap(), 4097), 0);
}

HWTEST_F(ImageStreamTest, FileImageStream_CopyFrom004, TestSize.Level3) {
    BufferImageStream src;
    FileImageStream dest(filePathDest);
    src.Open();
    dest.Open();
    char buff[IMAGE_STREAM_PAGE_SIZE - 1] = {0};
    src.Write((byte *)buff, sizeof(buff));
    ASSERT_TRUE(dest.CopyFrom(src));
    ASSERT_EQ(src.GetSize(), dest.GetSize());
    ASSERT_EQ(memcmp(src.MMap(), dest.MMap(), src.GetSize()), 0);
}

HWTEST_F(ImageStreamTest, FileImageStream_CopyFrom005, TestSize.Level3) {
    remove(filePath.c_str());
    FileImageStream src(filePathDest);
    BufferImageStream dest;
    src.Open();
    dest.Open();
    char buff[IMAGE_STREAM_PAGE_SIZE - 1] = {0};
    ASSERT_EQ(src.Write((byte *)buff, sizeof(buff)), sizeof(buff));
    ASSERT_TRUE(src.Flush());
    ASSERT_EQ(src.Tell(), sizeof(buff));
    ASSERT_EQ(src.GetSize(), sizeof(buff));
    ASSERT_TRUE(dest.CopyFrom(src));
    // ASSERT_EQ(src.GetSize(), dest.GetSize());
    // ASSERT_EQ(memcmp(src.MMap(), dest.MMap(), src.GetSize()), 0);
}

HWTEST_F(ImageStreamTest, FileImageStream_IsEof001, TestSize.Level3) {
    FileImageStream src(filePathSource);
    src.Open();
    byte buffer[1];
    ASSERT_EQ(src.Seek(0, SeekPos::BEGIN), 0);
    src.Read(buffer, sizeof(buffer));
    ASSERT_FALSE(src.IsEof());
    ASSERT_EQ(src.Seek(0, SeekPos::END), src.GetSize());
    src.Read(buffer, sizeof(buffer));
    ASSERT_TRUE(src.IsEof());
}

/**
 * @tc.name: FileImageStream_ReadByte001
 * @tc.desc: Test the ReadByte function of FileImageStream, comparing its output
 * with the Read function
 * @tc.type: FUNC
 */
HWTEST_F(ImageStreamTest, FileImageStream_ReadByte001, TestSize.Level3) {
    FileImageStream stream(filePathSource);
    stream.Open();

    // Read 10 bytes using Read function
    byte buffer[SIZE_10];
    stream.Read(buffer, SIZE_10);

    // Reset the file offset
    stream.Seek(0, SeekPos::BEGIN);

    // Read 10 bytes using ReadByte function
    byte byteBuffer[SIZE_10];
    for (int i = 0; i < SIZE_10; i++) {
        byteBuffer[i] = stream.ReadByte();
    }

    // Compare the results
    for (int i = 0; i < SIZE_10; i++) {
        EXPECT_EQ(buffer[i], byteBuffer[i]);
    }
}

/**
 * @tc.name: FileImageStream_ReadByte002
 * @tc.desc: Test the ReadByte function of FileImageStream, trying to read
 * beyond the end of the file
 * @tc.type: FUNC
 */
HWTEST_F(ImageStreamTest, FileImageStream_ReadByte002, TestSize.Level3) {
    FileImageStream stream(filePathSource);
    stream.Open();

    // Set the file offset to the end of the file
    EXPECT_EQ(stream.Seek(0, SeekPos::END), stream.GetSize());

    // Try to read one more byte
    int result = stream.ReadByte();

    // Check if the result is -1
    EXPECT_EQ(result, -1);
}

/**
 * @tc.name: FileImageStream_CONSTRUCTOR001
 * @tc.desc: Test the constructor of FileImageStream, checking if it can
 * correctly initialize a stream from an existing file pointer
 * @tc.type: FUNC
 */
HWTEST_F(ImageStreamTest, FileImageStream_CONSTRUCTOR001, TestSize.Level3) {
    FileImageStream stream(filePath);
    ASSERT_TRUE(stream.Open());
    std::string sourceData = "Hello, world!";
    ASSERT_EQ(stream.Seek(5, SeekPos::BEGIN), 5);
    ASSERT_EQ(stream.Write((byte *)sourceData.c_str(), sourceData.size()),
              sourceData.size());

    FileImageStream cloneStream(stream.fp_);
    ASSERT_TRUE(stream.Flush());
    ASSERT_TRUE(cloneStream.Open());
    // Read the data from cloneStream
    byte buffer[SIZE_255];
    cloneStream.Seek(5, SeekPos::BEGIN);
    ssize_t bytesRead = cloneStream.Read(buffer, sourceData.size());
    ASSERT_EQ(bytesRead, sourceData.size());
    buffer[bytesRead] = '\0'; // Add string termination character

    // Check if the read data is the same as the data in the source file
    ASSERT_STREQ((char *)buffer, sourceData.c_str());

    // Write some new data to cloneStream
    std::string newData = "New data";
    cloneStream.Write((byte *)newData.c_str(), newData.size());

    // Read the data from cloneStream again
    cloneStream.Seek(0, SeekPos::BEGIN);
    bytesRead = cloneStream.Read(buffer, sizeof(buffer) - 1);
    buffer[bytesRead] = '\0'; // Add string termination character

    // Check if the read data contains the new data
    ASSERT_STRNE((char *)buffer, newData.c_str());
}

/**
 * @tc.name: FileImageStream_CONSTRUCTOR002
 * @tc.desc: Test the constructor of FileImageStream, checking if it can
 * correctly initialize a stream from an existing file descriptor
 * @tc.type: FUNC
 */
HWTEST_F(ImageStreamTest, FileImageStream_CONSTRUCTOR002, TestSize.Level3) {
    // Create and open a temporary file
    std::string tempFile = "/tmp/testfile";
    int fileDescription =
        open(tempFile.c_str(), O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    ASSERT_NE(fileDescription, -1);

    // Use the file descriptor to create a new FileImageStream object
    FileImageStream stream(fileDescription);
    ASSERT_TRUE(stream.Open());
    ASSERT_NE(stream.dupFD_, -1);
    // Check the state of the FileImageStream object
    ASSERT_TRUE(stream.fp_ != nullptr);
    ASSERT_EQ(stream.fileSize_, stream.GetSize());
    ASSERT_EQ(stream.mappedMemory_, nullptr);
    ASSERT_EQ(stream.Tell(), 0);

    // Write data
    byte writeData[SIZE_10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    ssize_t bytesWritten = stream.Write(writeData, sizeof(writeData));
    ASSERT_EQ(bytesWritten, sizeof(writeData));

    // Reset the file pointer to the beginning of the file
    stream.Seek(0, SeekPos::BEGIN);

    // Read data
    byte readData[SIZE_10] = {0};
    ssize_t bytesRead = stream.Read(readData, sizeof(readData));
    ASSERT_EQ(bytesRead, sizeof(readData));

    // Check if the read data is the same as the written data
    for (size_t i = 0; i < sizeof(writeData); ++i) {
        ASSERT_EQ(writeData[i], readData[i]);
    }

    // Close the file
    close(fileDescription);
    remove(tempFile.c_str());
}

int countOpenFileDescriptors() {
    DIR *dir;
    int fdCount = 0;

    std::string dirPath = "/proc/" + std::to_string(getpid()) + "/fd/";

    if ((dir = opendir(dirPath.c_str())) != NULL) {
        while (readdir(dir) != NULL) {
            fdCount++;
        }
        closedir(dir);
    } else {
        std::cerr << "Could not open " << dirPath << std::endl;
    }

    return fdCount;
}

/**
 * @tc.name: FileImageStream_CONSTRUCTOR003
 * @tc.desc: Test the constructor of FileImageStream, checking if it can
 * correctly initialize a stream from an existing file descriptor and handle
 * file operations
 * @tc.type: FUNC
 */
HWTEST_F(ImageStreamTest, FileImageStream_CONSTRUCTOR003, TestSize.Level3) {
    // GTEST_LOG_(INFO) << "fd0: " << countOpenFileDescriptors();
    int fdCount = countOpenFileDescriptors();
    int fileDescriptor =
        open("/tmp/testfile", O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    ASSERT_EQ(fdCount + 1, countOpenFileDescriptors());
    // GTEST_LOG_(INFO) << "fd1: " << countOpenFileDescriptors();
    int dupFD = dup(fileDescriptor);
    ASSERT_EQ(fdCount + 2, countOpenFileDescriptors());
    // GTEST_LOG_(INFO) << "fd2: " << countOpenFileDescriptors();
    ASSERT_NE(fileDescriptor, -1);
    FILE *fileStream =
        fdopen(dupFD, "r+"); // Change "rb" to "wb" for writing in binary mode
    ASSERT_EQ(fdCount + 2, countOpenFileDescriptors());
    // GTEST_LOG_(INFO) << "fd3: " << countOpenFileDescriptors();
    ASSERT_NE(fileStream, nullptr);
    std::string text = "Hello, world!";
    ssize_t result =
        fwrite(text.c_str(), sizeof(char), text.size(),
               fileStream); // Use sizeof(char) as the second argument
    ASSERT_EQ(ferror(fileStream), 0);
    ASSERT_EQ(result, text.size());

    // Reset the file pointer to the beginning of the file
    rewind(fileStream);

    ASSERT_EQ(fdCount + 2, countOpenFileDescriptors());
    fileno(fileStream);
    ASSERT_EQ(fdCount + 2, countOpenFileDescriptors());

    // Read and verify the data
    char buffer[SIZE_255];
    result = fread(buffer, sizeof(char), text.size(), fileStream);
    ASSERT_EQ(result, text.size());
    buffer[result] = '\0'; // Add string termination character
    ASSERT_STREQ(buffer, text.c_str());

    // GTEST_LOG_(INFO) << "fd5: " << countOpenFileDescriptors();
    fclose(fileStream);
    ASSERT_EQ(fdCount + 1, countOpenFileDescriptors());
    // GTEST_LOG_(INFO) << "fd6: " << countOpenFileDescriptors();
    // close(dupFD);
    // GTEST_LOG_(INFO) << "fd7: " << countOpenFileDescriptors();
    close(fileDescriptor);
    ASSERT_EQ(fdCount, countOpenFileDescriptors());
    // GTEST_LOG_(INFO) << "fd8: " << countOpenFileDescriptors();
}

/**
 * @tc.name: FileImageStream_CONSTRUCTOR004
 * @tc.desc: Test the constructor of FileImageStream, checking if it can
 * correctly initialize a stream from an existing file descriptor and handle
 * file operations using the stream's file pointer
 * @tc.type: FUNC
 */
HWTEST_F(ImageStreamTest, FileImageStream_CONSTRUCTOR004, TestSize.Level3) {
    int fileDescriptor =
        open("/tmp/testfile", O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    FileImageStream stream(fileDescriptor);
    int dupFD = stream.dupFD_;
    ASSERT_NE(fileDescriptor, -1);
    ASSERT_TRUE(stream.Open());
    FILE *fileStream =
        stream.fp_; // Change "rb" to "wb" for writing in binary mode
    // FILE *f = fdopen(dupFD, "r+"); // Change "rb" to "wb" for writing in
    // binary mode
    ASSERT_NE(fileStream, nullptr);
    std::string text = "Hello, world!";
    ssize_t result =
        fwrite(text.c_str(), sizeof(char), text.size(),
               fileStream); // Use sizeof(char) as the second argument
    ASSERT_EQ(ferror(fileStream), 0);
    ASSERT_EQ(result, text.size());

    // Reset the file pointer to the beginning of the file
    rewind(fileStream);

    // Read and verify the data
    char buffer[SIZE_255];
    result = fread(buffer, sizeof(char), text.size(), fileStream);
    ASSERT_EQ(result, text.size());
    buffer[result] = '\0'; // Add string termination character
    ASSERT_STREQ(buffer, text.c_str());

    fclose(fileStream);
    close(dupFD);
    close(fileDescriptor);
}

HWTEST_F(ImageStreamTest, FileImageStream_Seek001, TestSize.Level3) {
    remove(filePath.c_str());
    FileImageStream stream(filePath);
    stream.Open();
    std::string sourceData = "Hello, world!";
    ASSERT_EQ(stream.Tell(), 0);
    stream.Write((byte *)sourceData.c_str(), sourceData.size());
    ASSERT_EQ(stream.Tell(), sourceData.size());
    stream.Seek(2, SeekPos::BEGIN);
    ASSERT_EQ(stream.Tell(), 2);
    byte buffer[SIZE_255];
    ssize_t bytesRead = stream.Read(buffer, 1);
    buffer[bytesRead] = '\0'; // Add string termination character
    ASSERT_STREQ((char *)buffer, "l");
    ASSERT_EQ(stream.Tell(), 3);
    stream.Seek(3, SeekPos::CURRENT);
    ASSERT_EQ(stream.Tell(), 6);
    bytesRead = stream.Read(buffer, 1);
    buffer[bytesRead] = '\0'; // Add string termination character
    ASSERT_STREQ((char *)buffer, " ");
    stream.Seek(0, SeekPos::END);
    ASSERT_EQ(stream.Tell(), sourceData.size());
}

/**
 * @tc.name: BufferImageStream_Open001
 * @tc.desc: Test the Open function of BufferImageStream, checking if it can
 * correctly open a stream
 * @tc.type: FUNC
 */
HWTEST_F(ImageStreamTest, BufferImageStream_Open001, TestSize.Level3) {
    BufferImageStream stream;
    ASSERT_TRUE(stream.Open());
}

/**
 * @tc.name: BufferImageStream_Read001
 * @tc.desc: Test the Read function of BufferImageStream, checking if it can
 * correctly read data from the stream
 * @tc.type: FUNC
 */
HWTEST_F(ImageStreamTest, BufferImageStream_Read001, TestSize.Level3) {
    BufferImageStream stream;
    ASSERT_TRUE(stream.Open());

    // Write a string
    std::string sourceData = "Hello, world!";
    ASSERT_EQ(stream.Tell(), 0);
    stream.Write((byte *)sourceData.c_str(), sourceData.size());

    // Read the string
    byte buffer[SIZE_255];
    stream.Seek(0, SeekPos::BEGIN);
    size_t bytesRead = stream.Read(buffer, sourceData.size());
    buffer[bytesRead] = '\0'; // Add string termination character

    // Compare the read string with the written string
    ASSERT_STREQ((char *)buffer, sourceData.c_str());
}

/**
 * @tc.name: BufferImageStream_Write001
 * @tc.desc: Test the Write function of BufferImageStream, checking if it can
 * correctly write data to the stream
 * @tc.type: FUNC
 */
HWTEST_F(ImageStreamTest, BufferImageStream_Write001, TestSize.Level3) {
    BufferImageStream stream;
    byte data[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    stream.Open();
    size_t size = sizeof(data) / sizeof(data[0]);
    int offset = 0;
    stream.Seek(0, SeekPos::BEGIN);
    ssize_t bytesWritten = stream.Write(data, size);
    ASSERT_EQ(bytesWritten, size);
    offset = stream.Tell();
    ASSERT_EQ(stream.Tell(), size);
    ASSERT_NE(offset, 0);
    byte readData[10] = {0};
    stream.Seek(0, SeekPos::BEGIN);
    ASSERT_EQ(stream.Tell(), 0);
    ssize_t bytesRead = stream.Read(readData, size);
    ASSERT_EQ(bytesRead, size);

    for (size_t i = 0; i < size; ++i) {
        ASSERT_EQ(data[i], readData[i]);
    }
}

/**
 * @tc.name: BufferImageStream_Write002
 * @tc.desc: Test the Write function of BufferImageStream, checking if it can
 * correctly write a string to the stream
 * @tc.type: FUNC
 */
HWTEST_F(ImageStreamTest, BufferImageStream_Write002, TestSize.Level3) {
    BufferImageStream stream;
    stream.Open();
    stream.Write((byte *)"Hello, world!", 13);
    ASSERT_EQ(stream.capacity_, 4096);
    ASSERT_EQ(stream.Tell(), 13);
}

/**
 * @tc.name: BufferImageStream_Write003
 * @tc.desc: Test the Write function of BufferImageStream, checking if it can
 * correctly handle large data
 * @tc.type: FUNC
 */
HWTEST_F(ImageStreamTest, BufferImageStream_Write003, TestSize.Level3) {
    BufferImageStream stream;
    stream.Open();
    byte data[IMAGE_STREAM_PAGE_SIZE + 1] = {0};    // Create a 4097-byte data
    stream.Write(data, IMAGE_STREAM_PAGE_SIZE + 1); // Write 4097 bytes of data
    ASSERT_GE(stream.capacity_,
              IMAGE_STREAM_PAGE_SIZE *
                  2); // Check if the buffer capacity is at least 4097
    ASSERT_EQ(stream.Tell(), IMAGE_STREAM_PAGE_SIZE +
                                 1); // Check if the write position is correct
}

/**
 * @tc.name: BufferImageStream_Write004
 * @tc.desc: Test the Write function of BufferImageStream, checking if it can
 * correctly handle data of the exact buffer capacity
 * @tc.type: FUNC
 */
HWTEST_F(ImageStreamTest, BufferImageStream_Write004, TestSize.Level3) {
    BufferImageStream stream;
    stream.Open();

    byte data[IMAGE_STREAM_PAGE_SIZE] = {0};    // Create a 4096-byte data
    stream.Write(data, IMAGE_STREAM_PAGE_SIZE); // Write 4096 bytes of data
    ASSERT_EQ(stream.capacity_,
              IMAGE_STREAM_PAGE_SIZE); // Check if the buffer capacity is 4096
    ASSERT_EQ(stream.Tell(),
              IMAGE_STREAM_PAGE_SIZE); // Check if the write position is correct
}

/**
 * @tc.name: BufferImageStream_Write005
 * @tc.desc: Test the Write function of BufferImageStream, checking if it can
 * correctly handle fixed buffer size
 * @tc.type: FUNC
 */
HWTEST_F(ImageStreamTest, BufferImageStream_Write005, TestSize.Level3) {
    char text[] = "Hello, world!";
    BufferImageStream stream((byte *)text, sizeof(text),
                             BufferImageStream::Fix);
    ASSERT_TRUE(stream.Open());
    ASSERT_EQ(stream.Write((byte *)"Hi", 2), 2);
    ASSERT_EQ(stream.Tell(), 2);
    ASSERT_STREQ(text, "Hillo, world!");
    ASSERT_EQ(stream.Write((byte *)"this is a very long text", 24), -1);
    ASSERT_STREQ(text, "Hillo, world!");
}

/**
 * @tc.name: BufferImageStream_Write006
 * @tc.desc: Test the Write function of BufferImageStream, checking if it can
 * correctly handle dynamic buffer size
 * @tc.type: FUNC
 */
HWTEST_F(ImageStreamTest, BufferImageStream_Write006, TestSize.Level3) {
    char text[] = "Hello, world!";
    BufferImageStream stream((byte *)text, sizeof(text),
                             BufferImageStream::Dynamic);
    ASSERT_TRUE(stream.Open());
    ASSERT_EQ(stream.Write((byte *)"Hi", 2), 2);
    // GTEST_LOG_(INFO) << "text: " << text;
    ASSERT_EQ(stream.Tell(), 2);
    ASSERT_STREQ(text, "Hillo, world!");
    stream.Seek(0, SeekPos::BEGIN);
    ASSERT_EQ(stream.Write((byte *)"this is a very long text", 24), 24);
    // GTEST_LOG_(INFO) << "text: " << text;
    ASSERT_STREQ((char *)stream.MMap(false), "this is a very long text");
}

/**
 * @tc.name: BufferImageStream_Close001
 * @tc.desc: Test the Close function of BufferImageStream with an empty stream
 * @tc.type: FUNC
 */
HWTEST_F(ImageStreamTest, BufferImageStream_Close001, TestSize.Level3) {
    BufferImageStream stream;
}

/**
 * @tc.name: BufferImageStream_Close002
 * @tc.desc: Test the Close function of BufferImageStream after writing to the
 * stream
 * @tc.type: FUNC
 */
HWTEST_F(ImageStreamTest, BufferImageStream_Close002, TestSize.Level3) {
    BufferImageStream stream;
    stream.Write((byte *)"Hello, world!", 13);
}

/**
 * @tc.name: BufferImageStream_Close003
 * @tc.desc: Test the Close function of BufferImageStream after releasing the
 * stream
 * @tc.type: FUNC
 */
HWTEST_F(ImageStreamTest, BufferImageStream_Close003, TestSize.Level3) {
    BufferImageStream stream;
    stream.Write((byte *)"Hello, world!", 13);
    delete[] stream.Release();
}

/**
 * @tc.name: BufferImageStream_Close004
 * @tc.desc: Test the Close function of BufferImageStream after closing the
 * stream
 * @tc.type: FUNC
 */
HWTEST_F(ImageStreamTest, BufferImageStream_Close004, TestSize.Level3) {
    BufferImageStream stream;
    stream.Write((byte *)"Hello, world!", 13);
    stream.Close();
}

/**
 * @tc.name: BufferImageStream_Close005
 * @tc.desc: Test the Close function of BufferImageStream with a fixed size
 * buffer
 * @tc.type: FUNC
 */
HWTEST_F(ImageStreamTest, BufferImageStream_Close005, TestSize.Level3) {
    char text[] = "Hello, world!";
    BufferImageStream stream((byte *)text, sizeof(text),
                             BufferImageStream::Fix);
}

/**
 * @tc.name: BufferImageStream_Close006
 * @tc.desc: Test the Close function of BufferImageStream with a fixed size
 * buffer after releasing the stream
 * @tc.type: FUNC
 */
HWTEST_F(ImageStreamTest, BufferImageStream_Close006, TestSize.Level3) {
    char text[] = "Hello, world!";
    BufferImageStream stream((byte *)text, sizeof(text),
                             BufferImageStream::Fix);
    stream.Release();
}

/**
 * @tc.name: BufferImageStream_Close007
 * @tc.desc: Test the Close function of BufferImageStream with a dynamic size
 * buffer after writing and releasing the stream
 * @tc.type: FUNC
 */
HWTEST_F(ImageStreamTest, BufferImageStream_Close007, TestSize.Level3) {
    char text[] = "Hello, world!";
    BufferImageStream stream((byte *)text, sizeof(text),
                             BufferImageStream::Dynamic);
    stream.Write((byte *)"this is a very very long text", 28);
    delete[] stream.Release();

    DataBuf dataBuf(10);
    dataBuf.WriteUInt8(0, 123);
    EXPECT_EQ(dataBuf.ReadUInt8(0), 123);
}

HWTEST_F(ImageStreamTest, BufferImageStream_CopyFrom001, TestSize.Level3) {
    FileImageStream src(filePathSource);
    BufferImageStream dest;
    src.Open();
    dest.Open();
    ASSERT_TRUE(dest.CopyFrom(src));
    ASSERT_EQ(src.GetSize(), dest.GetSize());
    ASSERT_EQ(memcmp(src.MMap(), dest.MMap(), src.GetSize()), 0);
}

HWTEST_F(ImageStreamTest, BufferImageStream_CopyFrom002, TestSize.Level3) {
    BufferImageStream src;
    BufferImageStream dest;
    src.Open();
    dest.Open();
    src.Write((byte *)"Hello, world!", 13);
    ASSERT_TRUE(dest.CopyFrom(src));
    ASSERT_EQ(src.GetSize(), dest.GetSize());
    ASSERT_EQ(memcmp(src.MMap(), dest.MMap(), src.GetSize()), 0);
}

HWTEST_F(ImageStreamTest, BufferImageStream_CopyFrom003, TestSize.Level3) {
    BufferImageStream src;
    BufferImageStream dest;
    src.Open();
    dest.Open();
    char buff[IMAGE_STREAM_PAGE_SIZE + 1] = {0};
    src.Write((byte *)buff, IMAGE_STREAM_PAGE_SIZE + 1);
    ASSERT_TRUE(dest.CopyFrom(src));
    ASSERT_EQ(src.GetSize(), dest.GetSize());
    ASSERT_EQ(memcmp(src.MMap(), dest.MMap(), src.GetSize()), 0);
}

HWTEST_F(ImageStreamTest, BufferImageStream_CopyFrom004, TestSize.Level3) {
    BufferImageStream src;
    BufferImageStream dest;
    src.Open();
    dest.Open();
    char buff[IMAGE_STREAM_PAGE_SIZE - 1] = {0};
    src.Write((byte *)buff, IMAGE_STREAM_PAGE_SIZE - 1);
    ASSERT_TRUE(dest.CopyFrom(src));
    ASSERT_EQ(src.GetSize(), dest.GetSize());
    ASSERT_EQ(memcmp(src.MMap(), dest.MMap(), src.GetSize()), 0);
}

} // namespace Media
} // namespace OHOS