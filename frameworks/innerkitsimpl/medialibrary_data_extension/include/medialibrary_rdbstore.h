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

#ifndef OHOS_MEDIALIBRARY_RDBSTORE_H
#define OHOS_MEDIALIBRARY_RDBSTORE_H

#include "medialibrary_unistore.h"
#include "medialibrary_sync_operation.h"
#include "timer.h"
#include "value_object.h"

namespace OHOS {
namespace Media {
class MediaLibraryRdbStoreObserver;
class MediaLibraryDataCallBack;

class MediaLibraryRdbStore final : public MediaLibraryUnistore {
public:
    explicit MediaLibraryRdbStore(const std::shared_ptr<OHOS::AbilityRuntime::Context> &context);
    virtual ~MediaLibraryRdbStore();

    virtual int32_t Init() override;
    virtual void Stop() override;

    virtual int32_t Insert(MediaLibraryCommand &cmd, int64_t &rowId) override;
    virtual int32_t Delete(MediaLibraryCommand &cmd, int32_t &deletedRows) override;
    virtual int32_t Update(MediaLibraryCommand &cmd, int32_t &changedRows) override;
    std::shared_ptr<NativeRdb::ResultSet> Query(MediaLibraryCommand &cmd,
        const std::vector<std::string> &columns) override;

    bool SyncPullTable(const std::string &bundleName, const std::string &tableName,
        int32_t rowId, const std::vector<std::string> &devices) override;
    bool SyncPushTable(const std::string &bundleName, const std::string &tableName,
        int32_t rowId, const std::vector<std::string> &devices, bool isBlock = false) override;
    int32_t ExecuteSql(const std::string &sql) override;
    std::shared_ptr<NativeRdb::ResultSet> QuerySql(const std::string &sql) override;

    int32_t BeginTransaction();
    int32_t RollBack();
    int32_t Commit();

    std::shared_ptr<NativeRdb::RdbStore> GetRaw() const;
    std::string ObtainTableName(MediaLibraryCommand &cmd) override;

    static void BuildValuesSql(const NativeRdb::ValuesBucket &values, std::vector<NativeRdb::ValueObject> &bindArgs,
        std::string &sql);
    static void BuildQuerySql(const NativeRdb::AbsRdbPredicates &predicates, const std::vector<std::string> &columns,
        std::vector<NativeRdb::ValueObject> &bindArgs, std::string &sql);
    static int32_t ExecuteForLastInsertedRowId(const std::string &sql,
        const std::vector<NativeRdb::ValueObject> &bindArgs);
    static std::shared_ptr<NativeRdb::ResultSet> Query(const NativeRdb::AbsRdbPredicates &predicates,
        const std::vector<std::string> &columns);
    static int32_t Delete(const NativeRdb::AbsRdbPredicates &predicates);
    static int32_t Update(int32_t &changedRows, const NativeRdb::ValuesBucket &values,
        const NativeRdb::AbsRdbPredicates &predicates);

private:
    bool SubscribeRdbStoreObserver();
    bool UnSubscribeRdbStoreObserver();
    void SetSyncOpts(MediaLibrarySyncOpts &syncOpts, const std::string &bundleName,
        const std::string &tableName, int32_t rowId);

    static constexpr int RDB_CONNECT_NUM = 10;
    static constexpr int RDB_TRANSACTION_WAIT_MS = 1000;
    std::mutex transactionMutex_;
    std::condition_variable transactionCV_;
    std::atomic<bool> isInTransaction_;
    static std::shared_ptr<NativeRdb::RdbStore> rdbStore_;
    std::shared_ptr<MediaLibraryRdbStoreObserver> rdbStoreObs_;
    std::string bundleName_{BUNDLE_NAME};
    NativeRdb::RdbStoreConfig config_{""};
};

class MediaLibraryDataCallBack : public NativeRdb::RdbOpenCallback {
public:
    struct DirValuesBucket {
        int32_t directoryType;
        std::string dirValues;
        std::string typeValues;
        std::string extensionValues;
    };

    struct SmartAlbumValuesBucket {
        int32_t albumId;
        std::string albumName;
        int32_t albumType;
    };

    struct UniqueMemberValuesBucket {
        std::string assetMediaType;
        int32_t startNumber;
    };

    int32_t OnCreate(NativeRdb::RdbStore &rdbStore) override;
    int32_t OnUpgrade(NativeRdb::RdbStore &rdbStore, int32_t oldVersion, int32_t newVersion) override;
    bool HasDistributedTables();

private:
    int32_t PrepareDir(NativeRdb::RdbStore &store);
    int32_t PrepareSmartAlbum(NativeRdb::RdbStore &store);
    int32_t PrepareUniqueMemberTable(NativeRdb::RdbStore &store);
    int32_t InsertDirValues(const DirValuesBucket &dirValuesBucket, NativeRdb::RdbStore &store);
    int32_t InsertSmartAlbumValues(const SmartAlbumValuesBucket &smartAlbum, NativeRdb::RdbStore &store);
    int32_t InsertUniqueMemberTableValues(const UniqueMemberValuesBucket &uniqueMemberValues,
        NativeRdb::RdbStore &store);
    bool isDistributedTables = false;
};

class MediaLibraryRdbStoreObserver : public NativeRdb::RdbStore::RdbStoreObserver {
public:
    explicit MediaLibraryRdbStoreObserver(const std::string &bundleName);
    virtual ~MediaLibraryRdbStoreObserver();
    void OnChange(const std::vector<std::string> &devices) override;

private:
    void NotifyDeviceChange();
    static constexpr int NOTIFY_TIME_INTERVAL = 10000;
    std::unique_ptr<OHOS::Utils::Timer> timer_;
    uint32_t timerId_{0};
    std::string bundleName_;
    bool isNotifyDeviceChange_;
};
} // namespace Media
} // namespace OHOS

#endif // OHOS_MEDIALIBRARY_RDBSTORE_OPERATIONS_H
