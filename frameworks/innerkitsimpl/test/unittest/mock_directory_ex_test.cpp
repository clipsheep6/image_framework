/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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
#include <fstream>
#include "directory_ex.h"
#include <dirent.h>
#include <iostream>
#include <sys/stat.h>
#include "hilog/log.h"
#include "log_tags.h"
#include "unistd.h"

using namespace testing::ext;
using namespace OHOS::Media;
using namespace OHOS::HiviewDFX;
namespace OHOS {
namespace Multimedia {
class MockDirectoryExTest : public testing::Test {
public:
    MockDirectoryExTest() {}
    ~MockDirectoryExTest() {}
};  

/**
 * @tc.name: ExtractFilePath001
 * @tc.desc: test ExtractFilePath
 * @tc.type: FUNC
 */
HWTEST_F(MockDirectoryExTest, ExtractFilePath001, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "MockDirectoryExTest: ExtractFilePath001 start";
    string fileFullName = '';
    std::string dirPath = ExtractFilePath(fileFullName);
    GTEST_LOG_(INFO) << "MockDirectoryExTest: ExtractFilePath001 end";
}

/**
 * @tc.name: ExtractFileName001
 * @tc.desc: test ExtractFileName
 * @tc.type: FUNC
 */
HWTEST_F(MockDirectoryExTest, ExtractFileName001, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "MockDirectoryExTest: ExtractFileName001 start";
    string fileFullName = '';
    std::string dirName = ExtractFileName(fileFullName);
    GTEST_LOG_(INFO) << "MockDirectoryExTest: ExtractFileName001 end";
}
 
/**
 * @tc.name: ForceCreateDirectory001
 * @tc.desc: test ForceCreateDirectory
 * @tc.type: FUNC
 */
HWTEST_F(MockDirectoryExTest, ForceCreateDirectory001, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "MockDirectoryExTest: ForceCreateDirectory001 start";
    string path ='';
    bool ex = ForceCreateDirectory(path);
    ASSERT_EQ(ex, false);
    GTEST_LOG_(INFO) << "MockDirectoryExTest: ForceCreateDirectory001 end";
}

/**
 * @tc.name: ExtractFileExt001
 * @tc.desc: test ExtractFileExt
 * @tc.type: FUNC
 */
HWTEST_F(MockDirectoryExTest, ExtractFileExt001, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "MockDirectoryExTest: ExtractFileExt001 start";
    string fileName = '';
    std::string exFile = ExtractFilePath(fileName);
    GTEST_LOG_(INFO) << "MockDirectoryExTest: ExtractFileExt001 end";
}

/**
 * @tc.name: TransformFileName001
 * @tc.desc: test TransformFileName
 * @tc.type: FUNC
 */
HWTEST_F(MockDirectoryExTest, TransformFileName001, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "MockDirectoryExTest: TransformFileName001 start";
    string fileName = '';
    std::string traFile = TransformFileName(fileName);
    GTEST_LOG_(INFO) << "MockDirectoryExTest: TransformFileName001 end";
}

/**
 * @tc.name: IncludeTrailingPathDelimiter001
 * @tc.desc: test IncludeTrailingPathDelimiter
 * @tc.type: FUNC
 */
HWTEST_F(MockDirectoryExTest, IncludeTrailingPathDelimiter001, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "MockDirectoryExTest: IncludeTrailingPathDelimiter001 start";
    string fileName = '';
    std::string traFile = TransformFileName(fileName);
    GTEST_LOG_(INFO) << "MockDirectoryExTest: IncludeTrailingPathDelimiter001 end";
}

/**
 * @tc.name: GetDirFiles001
 * @tc.desc: test GetDirFiles
 * @tc.type: FUNC
 */
HWTEST_F(MockDirectoryExTest, GetDirFiles001, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "MockDirectoryExTest: GetDirFiles001 start";
    string canonicalPath = '';
    vector<string> strFiles;
    GetDirFiles(canonicalPath, strFiles);
    GTEST_LOG_(INFO) << "MockDirectoryExTest: GetDirFiles001 end";
}

/**
 * @tc.name: PathToRealPath001
 * @tc.desc: test PathToRealPath
 * @tc.type: FUNC
 */
HWTEST_F(MockDirectoryExTest, PathToRealPath001, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "MockDirectoryExTest: PathToRealPath001 start";
    string path ='';
    string realPath ='';
    bool ptr = PathToRealPath(path, realPath);
    ASSERT_EQ(ptr, false);
    GTEST_LOG_(INFO) << "MockDirectoryExTest: PathToRealPath001 end";
}
} // namespace Multimedia
} // namespace OHOS