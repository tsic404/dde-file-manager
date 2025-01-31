// SPDX-FileCopyrightText: 2021 - 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef FILEOPERATEBASEWORKER_H
#define FILEOPERATEBASEWORKER_H

#include "fileoperations/fileoperationutils/abstractworker.h"

#include "dfm-base/interfaces/abstractfileinfo.h"
#include "dfm-base/utils/threadcontainer.hpp"

#include <QTime>

class QObject;

DPFILEOPERATIONS_BEGIN_NAMESPACE
class StorageInfo;
class DoCopyFileWorker;
using StoragePointer = QSharedPointer<StorageInfo>;
class FileOperateBaseWorker : public AbstractWorker, public QEnableSharedFromThis<AbstractFileInfo>
{

public:
    explicit FileOperateBaseWorker(QObject *parent = nullptr);
    virtual ~FileOperateBaseWorker() override;

    struct DirSetPermissonInfo
    {
        QFileDevice::Permissions permission;
        QUrl target;
    };

    using DirPermsissonPointer = QSharedPointer<DirSetPermissonInfo>;
    using DirPermissonList = DThreadList<DirPermsissonPointer>;

    struct SmallFileThreadCopyInfo
    {
        AbstractFileInfoPointer fromInfo { nullptr };
        AbstractFileInfoPointer toInfo { nullptr };
    };

public:
    bool doCheckFile(const AbstractFileInfoPointer &fromInfo, const AbstractFileInfoPointer &toInfo, const QString &fileName,
                     AbstractFileInfoPointer &newTargetInfo, bool *skip);
    bool doCheckNewFile(const AbstractFileInfoPointer &fromInfo, const AbstractFileInfoPointer &toInfo,
                        AbstractFileInfoPointer &newTargetInfo, QString &fileNewName,
                        bool *skip, bool isCountSize = false);
    bool checkDiskSpaceAvailable(const QUrl &fromUrl, const QUrl &toUrl,
                                 QSharedPointer<StorageInfo> targetStorageInfo, bool *skip);
    void setTargetPermissions(const AbstractFileInfoPointer &fromInfo, const AbstractFileInfoPointer &toInfo);
    void setAllDirPermisson();
    void determineCountProcessType();
    qint64 getWriteDataSize();
    qint64 getTidWriteSize();
    qint64 getSectorsWritten();
    void readAheadSourceFile(const AbstractFileInfoPointer &fileInfo);
    void syncFilesToDevice();
    AbstractJobHandler::SupportAction doHandleErrorAndWait(const QUrl &from, const QUrl &to,
                                                           const AbstractJobHandler::JobErrorType &error,
                                                           const bool isTo = false,
                                                           const QString &errorMsg = QString(),
                                                           const bool errorMsgAll = false);
    // notify
    void emitSpeedUpdatedNotify(const qint64 &writSize);

    bool deleteFile(const QUrl &fromUrl, const QUrl &toUrl, bool *result, const bool force = false);
    bool deleteDir(const QUrl &fromUrl, const QUrl &toUrl, bool *result, const bool force = false);
    bool copyFileFromTrash(const QUrl &urlSource, const QUrl &urlTarget, dfmio::DFile::CopyFlag flag);

    bool copyAndDeleteFile(const AbstractFileInfoPointer &fromInfo, const AbstractFileInfoPointer &targetPathInfo, const AbstractFileInfoPointer &toInfo,
                           bool *result);
    bool createSystemLink(const AbstractFileInfoPointer &fromInfo, const AbstractFileInfoPointer &toInfo,
                          const bool followLink, const bool doCopy,
                          bool *result);
    bool canWriteFile(const QUrl &url) const;

    bool doCopyFile(const AbstractFileInfoPointer &fromInfo, const AbstractFileInfoPointer &toInfo, bool *skip);
    bool checkAndCopyFile(const AbstractFileInfoPointer fromInfo, const AbstractFileInfoPointer toInfo, bool *skip);
    bool checkAndCopyDir(const AbstractFileInfoPointer &fromInfo, const AbstractFileInfoPointer &toInfo, bool *skip);

protected:
    void waitThreadPoolOver();
    void initCopyWay();

private:
    void setSkipValue(bool *skip, AbstractJobHandler::SupportAction action);
    void initThreadCopy();
    void initSignalCopyWorker();
    bool actionOperating(const AbstractJobHandler::SupportAction action, const qint64 size, bool *skip);
    bool createNewTargetInfo(const AbstractFileInfoPointer &fromInfo, const AbstractFileInfoPointer &toInfo,
                             AbstractFileInfoPointer &newTargetInfo, const QUrl &fileNewUrl,
                             bool *skip, bool isCountSize = false);
    QUrl createNewTargetUrl(const AbstractFileInfoPointer &toInfo, const QString &fileName);
    bool doCopyLocalFile(const AbstractFileInfoPointer fromInfo, const AbstractFileInfoPointer toInfo);
    bool doCopyExBlockFile(const AbstractFileInfoPointer fromInfo, const AbstractFileInfoPointer toInfo);
    bool doCopyOtherFile(const AbstractFileInfoPointer fromInfo, const AbstractFileInfoPointer toInfo, bool *skip);
    bool doCopyLocalBigFile(const AbstractFileInfoPointer fromInfo, const AbstractFileInfoPointer toInfo, bool *skip);

private:   // do copy local big file
    bool doCopyLocalBigFileResize(const AbstractFileInfoPointer fromInfo, const AbstractFileInfoPointer toInfo, int toFd, bool *skip);
    char *doCopyLocalBigFileMap(const AbstractFileInfoPointer fromInfo, const AbstractFileInfoPointer toInfo, int fd, const int per, bool *skip);
    void memcpyLocalBigFile(const AbstractFileInfoPointer fromInfo, const AbstractFileInfoPointer toInfo, char *fromPoint, char *toPoint);
    void doCopyLocalBigFileClear(const size_t size, const int fromFd,
                                 const int toFd, char *fromPoint, char *toPoint);
    int doOpenFile(const AbstractFileInfoPointer fromInfo, const AbstractFileInfoPointer toInfo, const bool isTo,
                   const int openFlag, bool *skip);

private:   // do copy extra block file
    void createExBlockFileCopyInfo(const AbstractFileInfoPointer fromInfo,
                                   const AbstractFileInfoPointer toInfo,
                                   const qint64 currentPos,
                                   const bool closeFlag,
                                   const qint64 size,
                                   char *buffer = nullptr,
                                   const bool isDir = false,
                                   const QFileDevice::Permissions permission = QFileDevice::Permission::ReadOwner);
    void startBlockFileCopy();

protected Q_SLOTS:
    void emitErrorNotify(const QUrl &from, const QUrl &to, const AbstractJobHandler::JobErrorType &error,
                         const bool isTo = false, const quint64 id = 0, const QString &errorMsg = QString(),
                         const bool allUsErrorMsg = false) override;
    virtual void emitCurrentTaskNotify(const QUrl &from, const QUrl &to) override;
    void skipMemcpyBigFile(const QUrl url);

private:
    QVariant
    checkLinkAndSameUrl(const AbstractFileInfoPointer &fromInfo, const AbstractFileInfoPointer &newTargetInfo, const bool isCountSize);
    QVariant doActionReplace(const AbstractFileInfoPointer &fromInfo, const AbstractFileInfoPointer &newTargetInfo, const bool isCountSize);
    QVariant doActionMerge(const AbstractFileInfoPointer &fromInfo, const AbstractFileInfoPointer &newTargetInfo, const bool isCountSize);

protected:
    QTime time;   // time eslape
    AbstractFileInfoPointer targetInfo { nullptr };   // target file infor pointer
    StoragePointer targetStorageInfo { nullptr };   // target file's device infor
    CountWriteSizeType countWriteType { CountWriteSizeType::kTidType };   // get write size type
    long copyTid = { -1 };   // 使用 /pric/[pid]/task/[tid]/io 文件中的的 writeBytes 字段的值作为判断已写入数据的依据
    qint64 targetDeviceStartSectorsWritten { 0 };   // 记录任务开始时目标磁盘设备已写入扇区数
    QString targetSysDevPath;   // /sys/dev/block/x:x
    qint16 targetLogSecionSize { 512 };   // 目标设备逻辑扇区大小
    qint8 targetIsRemovable { 1 };   // 目标磁盘设备是不是可移除或者热插拔设备
    DirPermissonList dirPermissonList;   // dir set Permisson list

    std::atomic_int threadCopyFileCount { 0 };
};
DPFILEOPERATIONS_END_NAMESPACE

#endif   // FILEOPERATEBASEWORKER_H
