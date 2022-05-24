/*
 * Copyright (C) 2022 Uniontech Software Technology Co., Ltd.
 *
 * Author:     lixiang<lixianga@uniontech.com>
 *
 * Maintainer: lixiang<lixianga@uniontech.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "filemanipulation.h"
#include "events/vaulteventcaller.h"
#include "services/common/delegate/delegateservice.h"

#include "dfm-base/dfm_event_defines.h"

#include <dfm-framework/framework.h>

#include <QUrl>
#include <QFileInfo>

DFMBASE_USE_NAMESPACE
DSC_USE_NAMESPACE
DPVAULT_USE_NAMESPACE
FileManipulation::FileManipulation(QObject *parent)
    : QObject(parent)
{
}

bool FileManipulation::openFilesHandle(quint64 windowId, const QList<QUrl> urls, const QString *error)
{
    Q_UNUSED(error)

    QList<QUrl> redirectedFileUrls;
    for (const QUrl &url : urls) {
        QUrl redirectedFileUrl = delegateServIns->urlTransform(url);
        redirectedFileUrls << redirectedFileUrl;
    }

    if (!redirectedFileUrls.isEmpty())
        VaultEventCaller::sendOpenFiles(windowId, redirectedFileUrls);

    return true;
}

bool FileManipulation::writeToClipBoardHandle(const quint64 windowId, const ClipBoard::ClipboardAction action, const QList<QUrl> urls)
{
    QList<QUrl> redirectedFileUrls;
    for (QUrl url : urls) {
        redirectedFileUrls << delegateServIns->urlTransform(url);
    }

    dpfInstance.eventDispatcher().publish(GlobalEventType::kWriteUrlsToClipboard, windowId, action, redirectedFileUrls);

    return true;
}

JobHandlePointer FileManipulation::moveToTrashHandle(const quint64 windowId, const QList<QUrl> sources, const AbstractJobHandler::JobFlags flags)
{
    Q_UNUSED(flags)
    QList<QUrl> redirectedFileUrls;
    for (QUrl url : sources) {
        redirectedFileUrls << delegateServIns->urlTransform(url);
    }

    dpfInstance.eventDispatcher().publish(GlobalEventType::kDeleteFiles,
                                          windowId,
                                          redirectedFileUrls, flags, nullptr);
    return {};
}

JobHandlePointer FileManipulation::deletesHandle(const quint64 windowId, const QList<QUrl> sources, const AbstractJobHandler::JobFlags flags)
{
    Q_UNUSED(flags)
    QList<QUrl> redirectedFileUrls;
    for (QUrl url : sources) {
        redirectedFileUrls << delegateServIns->urlTransform(url);
    }

    dpfInstance.eventDispatcher().publish(GlobalEventType::kDeleteFiles,
                                          windowId,
                                          redirectedFileUrls, flags, nullptr);
    return {};
}

JobHandlePointer FileManipulation::copyHandle(const quint64 windowId, const QList<QUrl> sources, const QUrl target, const AbstractJobHandler::JobFlags flags)
{
    QUrl url = delegateServIns->urlTransform(target);
    dpfInstance.eventDispatcher().publish(GlobalEventType::kCopy, windowId, sources, url, flags, nullptr);
    return {};
}

JobHandlePointer FileManipulation::cutHandle(const quint64 windowId, const QList<QUrl> sources, const QUrl target, const AbstractJobHandler::JobFlags flags)
{
    QUrl url = delegateServIns->urlTransform(target);
    dpfInstance.eventDispatcher().publish(GlobalEventType::kCutFile, windowId, sources, url, flags, nullptr);
    return {};
}

bool FileManipulation::mkdirHandle(const quint64 windowId, const QUrl url, QString *errore)
{
    QUrl dirUrl = delegateServIns->urlTransform(url);
    return dpfInstance.eventDispatcher().publish(GlobalEventType::kMkdir,
                                                 windowId,
                                                 dirUrl);
}

bool FileManipulation::touchFileHandle(const quint64 windowId, const QUrl url, QString *error, const Global::CreateFileType type)
{
    QUrl dirUrl = delegateServIns->urlTransform(url);
    return dpfInstance.eventDispatcher().publish(GlobalEventType::kTouchFile,
                                                 windowId,
                                                 dirUrl,
                                                 type, *error);
}

bool FileManipulation::renameHandle(const quint64 windowId, const QUrl oldUrl, const QUrl newUrl, QString *error)
{
    QUrl ourl = delegateServIns->urlTransform(oldUrl);
    QUrl nurl = delegateServIns->urlTransform(newUrl);
    return dpfInstance.eventDispatcher().publish(GlobalEventType::kRenameFile, windowId, ourl, nurl);
}
