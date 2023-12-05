import BackupExtensionAbility, {BundleVersion} from '@ohos.application.BackupExtensionAbility';
import fs from '@ohos.file.fs';
// @ts-ignore
import mediabackup from '@ohos.multimedia.mediabackup';

const TAG = 'MediaBackupExtAbility';

const backupPath = '/data/storage/el2/backup/restore/';
const backupClonePath = '/data/storage/el2/backup/restore/storage/cloud/files/';
const documentPath = '/storage/media/local/files/Docs/Documents';
const galleryAppName = 'com.huawei.photos';
const mediaAppName = 'com.android.providers.media.module';
const cameraAppName = 'com.huawei.camera';

const UPDATE_RESTORE : number = 0;
const CLONE_RESTORE : number = 1;

export default class MediaBackupExtAbility extends BackupExtensionAbility {
  async onBackup() : Promise<void> {
    console.log(TAG, 'onBackup ok.');
  }

  async onRestore(bundleVersion : BundleVersion) : Promise<void> {
    console.log(TAG, `onRestore ok ${JSON.stringify(bundleVersion)}`);
    console.time(TAG + ' RESTORE');
    let path:string;
    if (bundleVersion.name === '0.0.0.0' && bundleVersion.code === 0) {
      await mediabackup.startRestore(UPDATE_RESTORE, galleryAppName, mediaAppName, cameraAppName);
      path = backupPath;
    } else {
      await mediabackup.startRestore(CLONE_RESTORE, galleryAppName, mediaAppName, cameraAppName);
      path = backupClonePath;
    }
    console.timeEnd(TAG + ' RESTORE');
    console.time(TAG + ' MOVE REST FILES');
    await this.moveRestFiles(path);
    console.timeEnd(TAG + ' MOVE REST FILES');
  }

  private isFileExist(filePath : string) : boolean {
    try {
      return fs.accessSync(filePath);
    } catch (err) {
      console.error(TAG, `accessSync failed, message = ${err.message}; code = ${err.code}`);
      return false;
    }
  }

  private async moveRestFiles(path : string) : Promise<void> {
    console.log(TAG, 'Start to move rest files.');
    const MOVE_ERR_CODE = 13900015;
    let list = [];
    try {
      await fs.moveDir(path, documentPath, 1);
      console.info(TAG, 'Move rest files succeed');
    } catch (err) {
      if (err.code === MOVE_ERR_CODE) {
        list = err.data;
      } else {
        console.error(TAG, `move directory failed, message = ${err.message}; code = ${err.code}`);
      }
    }

    for (let i = 0; i < list.length; i++) {
      try {
        await this.moveConflictFile(list[i].srcFile, list[i].destFile);
      } catch (err) {
        console.error(TAG, `MoveConflictFile failed, message = ${err.message}; code = ${err.code}`);
      }
    }
  }

  private async moveConflictFile(srcFile : string, dstFile : string) : Promise<void> {
    const srcArr = srcFile.split('/');
    const dstArr = dstFile.split('/');
    const srcFileName = srcArr[srcArr.length - 1];
    const dirPath = dstArr.splice(0, dstArr.length - 1).join('/');
    let fileExt : string = '';
    let fileNameWithoutExt = srcFileName;
    if (srcFileName.lastIndexOf('.') !== -1) {
      let tmpValue = srcFileName.split('.').pop();
      if (tmpValue !== undefined) {
        fileExt = tmpValue;
        fileNameWithoutExt = srcFileName.slice(0, srcFileName.length - fileExt.length - 1);
      }
    }
    let newFileName = srcFileName;
    let count = 1;
    while (this.isFileExist(`${dirPath}/${newFileName}`)) {
      if (fileExt === '') {
        newFileName = `${fileNameWithoutExt}(${count})`;
      } else {
        newFileName = `${fileNameWithoutExt}(${count}).${fileExt}`;
      }
      count++;
    }
    try {
      await fs.moveFile(srcFile, `${dirPath}/${newFileName}`);
    } catch (err) {
      console.error(TAG, `moveFile file failed, message = ${err.message}; code = ${err.code}`);
    }
  }
}
