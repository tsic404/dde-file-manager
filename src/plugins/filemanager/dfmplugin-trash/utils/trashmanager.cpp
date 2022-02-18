/*
 * Copyright (C) 2022 Uniontech Software Technology Co., Ltd.
 *
 * Author:     yanghao<yanghao@uniontech.com>
 *
 * Maintainer: huangyu<huangyub@uniontech.com>
 *             liuyangming<liuyangming@uniontech.com>
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

#include "trashmanager.h"
#include "trashfileinfo.h"
#include "trashfilewatcher.h"
#include "events/trasheventcaller.h"

#include "services/filemanager/windows/windowsservice.h"
#include "services/filemanager/workspace/workspaceservice.h"
#include "services/common/propertydialog/propertydialogservice.h"

#include "dfm-base/base/schemefactory.h"
#include "dfm-base/base/standardpaths.h"
#include "dfm-base/utils/dialogmanager.h"

#include <DHorizontalLine>
#include <DApplicationHelper>

#include <QFileInfo>
#include <QFile>
#include <QMenu>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>

DPTRASH_USE_NAMESPACE
DSB_FM_USE_NAMESPACE
DSC_USE_NAMESPACE
DFMBASE_USE_NAMESPACE

TrashManager *TrashManager::instance()
{
    static TrashManager instance;
    return &instance;
}

QUrl TrashManager::rootUrl()
{
    QUrl url;
    url.setScheme(scheme());
    url.setPath("/");
    return url;
}

quint64 TrashManager::windowId(QWidget *sender)
{
    auto &ctx = dpfInstance.serviceContext();
    auto windowService = ctx.service<WindowsService>(WindowsService::name());
    return windowService->findWindowId(sender);
}

void TrashManager::contenxtMenuHandle(quint64 windowId, const QUrl &url, const QPoint &globalPos)
{
    QMenu *menu = new QMenu;
    menu->addAction(QObject::tr("Open in new window"), [url]() {
        TrashEventCaller::sendOpenWindow(url);
    });

    auto newTabAct = menu->addAction(QObject::tr("Open in new tab"), [windowId, url]() {
        TrashEventCaller::sendOpenTab(windowId, url);
    });

    auto &ctx = dpfInstance.serviceContext();
    auto workspaceService = ctx.service<WorkspaceService>(WorkspaceService::name());
    if (!workspaceService) {
        qCritical() << "Failed, trashManager contenxtMenuHandle \"WorkspaceService\" is empty";
        abort();
    }

    newTabAct->setDisabled(!workspaceService->tabAddable(windowId));

    menu->addSeparator();
    menu->addAction(QObject::tr("Empty Trash"), [windowId, url]() {
        QUrl localUrl = TrashManager::toLocalFile(url);
        TrashEventCaller::sendEmptyTrash(windowId, { localUrl });
    });

    menu->addSeparator();
    menu->addAction(QObject::tr("Properties"), [url]() {
        TrashEventCaller::sendTrashPropertyDialog(url);
    });
    menu->exec(globalPos);
    delete menu;
}

bool TrashManager::openFilesHandle(quint64 windowId, const QList<QUrl> urls, const QString *error)
{
    Q_UNUSED(error)

    bool isOpenFile = false;
    QList<QUrl> redirectedFileUrls;
    for (const QUrl &url : urls) {
        QUrl redirectedFileUrl = TrashManager::toLocalFile(url);
        QFileInfo fileInfo(redirectedFileUrl.path());
        if (fileInfo.isFile()) {
            isOpenFile = true;
            continue;
        }
        redirectedFileUrls << url;
    }
    if (!redirectedFileUrls.isEmpty())
        TrashEventCaller::sendOpenFiles(windowId, redirectedFileUrls);

    // Todo(yanghao)
    if (isOpenFile) {
        QString strMsg = tr("Unable to open items in the trash, please restore it first");
        DialogManagerInstance->showMessageDialog(DialogManager::kMsgWarn, strMsg);
    }
    return true;
}

bool TrashManager::writeToClipBoardHandle(const quint64 windowId, const ClipBoard::ClipboardAction action, const QList<QUrl> urls)
{
    QList<QUrl> redirectedFileUrls;
    for (QUrl url : urls) {
        url.setScheme(SchemeTypes::kFile);
        redirectedFileUrls << url;
    }
    // Todo(yanghao)
    return true;
}

QFrame *TrashManager::createEmptyTrashTopWidget()
{
    QFrame *emptyTrashHolder = new QFrame;
    emptyTrashHolder->setFrameShape(QFrame::NoFrame);

    QHBoxLayout *emptyTrashLayout = new QHBoxLayout;
    QLabel *trashLabel = new QLabel(emptyTrashHolder);

    trashLabel->setText(tr("Trash"));
    QFont f = trashLabel->font();
    f.setPixelSize(17);
    f.setBold(true);
    trashLabel->setFont(f);
    QPushButton *emptyTrashButton = new QPushButton;
    emptyTrashButton->setContentsMargins(0, 0, 0, 0);
    emptyTrashButton->setObjectName("EmptyTrashButton");

    emptyTrashButton->setText(tr("Empty"));
    emptyTrashButton->setToolTip(tr("Empty Trash"));
    emptyTrashButton->setFixedSize({ 86, 36 });
    DPalette pal = DApplicationHelper::instance()->palette(emptyTrashHolder);
    QPalette buttonPalette = emptyTrashButton->palette();
    buttonPalette.setColor(QPalette::ButtonText, pal.color(DPalette::Active, DPalette::TextWarning));
    emptyTrashButton->setPalette(buttonPalette);

    QObject::connect(emptyTrashButton, &QPushButton::clicked,

                     TrashManager::instance(), [emptyTrashButton] {
                         auto windId = TrashManager::instance()->windowId(emptyTrashButton);
                         QUrl url = TrashManager::toLocalFile(TrashManager::rootUrl());
                         TrashEventCaller::sendEmptyTrash(windId, { url });
                     });
    QPalette pa = emptyTrashButton->palette();
    pa.setColor(QPalette::ColorRole::Text, QColor("#FF5736"));
    emptyTrashButton->setPalette(pa);
    emptyTrashLayout->addWidget(trashLabel, 0, Qt::AlignLeft);
    emptyTrashLayout->addWidget(emptyTrashButton, 0, Qt::AlignRight);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    DHorizontalLine *emptyTrashSplitLine = new DHorizontalLine;
    mainLayout->addLayout(emptyTrashLayout);
    mainLayout->addWidget(emptyTrashSplitLine);
    emptyTrashHolder->setLayout(mainLayout);
    return emptyTrashHolder;
}

QUrl TrashManager::toLocalFile(const QUrl &url)
{
    return QUrl::fromLocalFile(StandardPaths::location(StandardPaths::kTrashFilesPath) + url.path());
}

TrashManager::TrashManager(QObject *parent)
    : QObject(parent)
{
    init();
}

TrashManager::~TrashManager()
{
}

void TrashManager::init()
{
}
