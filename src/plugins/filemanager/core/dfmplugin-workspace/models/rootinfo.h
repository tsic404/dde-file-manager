// SPDX-FileCopyrightText: 2022 - 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ROOTINFO_H
#define ROOTINFO_H

#include "dfmplugin_workspace_global.h"
#include "utils/traversaldirthreadmanager.h"

#include "dfm-base/dfm_base_global.h"
#include "dfm-base/utils/traversaldirthread.h"
#include "dfm-base/interfaces/abstractfilewatcher.h"

#include <QReadWriteLock>
#include <QQueue>
#include <QFuture>

namespace dfmplugin_workspace {

class FileItemData;
class RootInfo : public QObject
{
    Q_OBJECT

    enum EventType {
        kAddFile,
        kUpdateFile,
        kRmFile
    };

public:
    struct DirIteratorThread
    {
        TraversalThreadManagerPointer traversalThread { nullptr };
        // origin data sort information
        dfmio::DEnumerator::SortRoleCompareFlag originSortRole { dfmio::DEnumerator::SortRoleCompareFlag::kSortRoleCompareDefault };
        Qt::SortOrder originSortOrder { Qt::AscendingOrder };
        bool originMixSort { false };
    };

public:
    explicit RootInfo(const QUrl &u, const bool canCache, QObject *parent = nullptr);
    ~RootInfo();

    bool initThreadOfFileData(const QString &key,
                              DFMGLOBAL_NAMESPACE::ItemRoles role, Qt::SortOrder order, bool isMixFileAndFolder);
    void startWork(const QString &key, const bool getCache = false);
    int clearTraversalThread(const QString &key);

    void reset();

Q_SIGNALS:
    void childrenUpdate(const QUrl &url);

    void itemAdded();
    void iteratorLocalFiles(const QString &key,
                            QList<SortInfoPointer> children,
                            const dfmio::DEnumerator::SortRoleCompareFlag sortRole,
                            const Qt::SortOrder sortOrder,
                            const bool isMixDirAndFile);
    void iteratorAddFile(const QString &key, const SortInfoPointer sortInfo);
    void watcherAddFiles(QList<SortInfoPointer> children);
    void watcherRemoveFiles(QList<SortInfoPointer> children);
    void traversalFinished(const QString &key);
    void sourceDatas(const QString &key,
                     QList<SortInfoPointer> children,
                     const dfmio::DEnumerator::SortRoleCompareFlag sortRole,
                     const Qt::SortOrder sortOrder,
                     const bool isMixDirAndFile,
                     const bool isFinished);
    void watcherUpdateFile(const SortInfoPointer sortInfo);
    void watcherUpdateHideFile(const QUrl &hidUrl);

public Q_SLOTS:
    void doFileDeleted(const QUrl &url);
    void dofileMoved(const QUrl &fromUrl, const QUrl &toUrl);
    void dofileCreated(const QUrl &url);
    void doFileUpdated(const QUrl &url);
    void doWatcherEvent();
    void doThreadWatcherEvent();

    void handleTraversalResult(const AbstractFileInfoPointer &child);
    void handleTraversalLocalResult(QList<SortInfoPointer> children,
                                    dfmio::DEnumerator::SortRoleCompareFlag sortRole,
                                    Qt::SortOrder sortOrder,
                                    bool isMixDirAndFile);
    void handleTraversalFinish();
    void handleGetSourceData(const QString &key);

private:
    void initConnection(const TraversalThreadManagerPointer &traversalThread);

    void addChildren(const QList<QUrl> &urlList);
    void addChildren(const QList<AbstractFileInfoPointer> &children);
    void addChildren(const QList<SortInfoPointer> &children);
    SortInfoPointer addChild(const AbstractFileInfoPointer &child);
    SortInfoPointer sortFileInfo(const AbstractFileInfoPointer &info);
    void removeChildren(const QList<QUrl> &urlList);
    bool containsChild(const QUrl &url);
    void updateChild(const QUrl &url);

    void startWatcher();
    bool checkFileEventQueue();
    void enqueueEvent(const QPair<QUrl, EventType> &e);
    QPair<QUrl, EventType> dequeueEvent();
    AbstractFileInfoPointer fileInfo(const QUrl &url);

public:
    AbstractFileWatcherPointer watcher;

private:
    QUrl url;
    QUrl hiddenFileUrl;

    QMap<QString, QSharedPointer<DirIteratorThread>> traversalThreads;
    QString currentKey;
    std::atomic_bool traversalFinish { false };

    QReadWriteLock childrenLock;
    QList<QUrl> childrenUrlList {};
    QList<SortInfoPointer> sourceDataList {};
    // origin data sort information
    dfmio::DEnumerator::SortRoleCompareFlag originSortRole { dfmio::DEnumerator::SortRoleCompareFlag::kSortRoleCompareDefault };
    Qt::SortOrder originSortOrder { Qt::AscendingOrder };
    bool originMixSort { false };
    bool canCache { false };

    std::atomic_bool cancelWatcherEvent { false };
    QFuture<void> watcherEventFuture;

    QQueue<QPair<QUrl, EventType>> watcherEvent {};
    QMutex watcherEventMutex;
    QAtomicInteger<bool> processFileEventRuning = false;

    QList<TraversalThreadPointer> discardedThread {};
    QList<QSharedPointer<QThread>> threads {};
};
}

#endif   // ROOTINFO_H
