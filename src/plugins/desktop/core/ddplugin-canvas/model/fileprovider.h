// SPDX-FileCopyrightText: 2022 - 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef FILEPROVIDER_H
#define FILEPROVIDER_H

#include "ddplugin_canvas_global.h"

#include <dfm-base/utils/traversaldirthread.h>
#include <interfaces/abstractfilewatcher.h>

#include <QObject>
#include <QMutex>
#include <QUrl>

namespace ddplugin_canvas {
class FileFilter;
class FileProvider : public QObject
{
    Q_OBJECT
public:
    explicit FileProvider(QObject *parent = nullptr);
    bool setRoot(const QUrl &url);
    QUrl root() const;
    bool isUpdating() const;
    void refresh(QDir::Filters filters = QDir::NoFilter);
    void installFileFilter(QSharedPointer<FileFilter> filter);
    void removeFileFilter(QSharedPointer<FileFilter> filter);
signals:
    void refreshEnd(const QList<QUrl> &urls);
    void fileRemoved(const QUrl &url);
    void fileInserted(const QUrl &url);
    void fileRenamed(const QUrl &oldurl, const QUrl &newurl);
    void fileUpdated(const QUrl &url);
protected slots:
    void traversalFinished();
    void reset(QList<QUrl> children);
    void insert(const QUrl &url);
    void remove(const QUrl &url);
    void rename(const QUrl &oldUrl, const QUrl &newUrl);
    void update(const QUrl &url);
    void preupdateData(const QUrl &url);

protected:
    QUrl rootUrl;
    AbstractFileWatcherPointer watcher;
    QList<QSharedPointer<FileFilter>> fileFilters;

private:
    QAtomicInteger<bool> updateing = false;
    QSharedPointer<DFMBASE_NAMESPACE::TraversalDirThread> traversalThread;
};

}

#endif   // FILEPROVIDER_H
