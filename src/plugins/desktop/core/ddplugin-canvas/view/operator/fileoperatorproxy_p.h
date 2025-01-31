// SPDX-FileCopyrightText: 2022 - 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef FILEOPERATORPROXY_P_H
#define FILEOPERATORPROXY_P_H

#include "fileoperatorproxy.h"

#include <QTimer>
#include <QUrl>

namespace ddplugin_canvas {

class FileOperatorProxyPrivate : public QObject
{
    Q_OBJECT
public:

    enum CallBackFunc {
        kCallBackTouchFile,
        kCallBackTouchFolder,
        kCallBackCopyFiles,
        kCallBackCutFiles,
        kCallBackPasteFiles,
        kCallBackOpenFiles,
        kCallBackRenameFiles,
        kCallBackOpenFilesByApp,
        kCallBackMoveToTrash,
        kCallBackDeleteFiles
    };

    explicit FileOperatorProxyPrivate(FileOperatorProxy *q_ptr);

    void callBackTouchFile(const QUrl &target, const QVariantMap &customData);
    void callBackPasteFiles(const JobInfoPointer info);
    void callBackRenameFiles(const QList<QUrl> &sources, const QList<QUrl> &targets);

    void delaySelectUrls(const QList<QUrl> &urls, int ms = 10);
    void doSelectUrls(const QList<QUrl> &urls);

    void filterDesktopFile(QList<QUrl> &urls);
public:
    FileOperatorProxy *const q = nullptr;
    QSharedPointer<QTimer> selectTimer;
    DFMGLOBAL_NAMESPACE::OperatorCallback callBack;

    QPair<QString, QPair<int, QPoint>> touchFileData;
    QHash<QUrl, QUrl> renameFileData;
};

}

Q_DECLARE_METATYPE(DDP_CANVAS_NAMESPACE::FileOperatorProxyPrivate::CallBackFunc)

#endif // FILEOPERATORPROXY_P_H
