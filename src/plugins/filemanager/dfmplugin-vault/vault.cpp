// SPDX-FileCopyrightText: 2022 - 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "vault.h"
#include "utils/vaultvisiblemanager.h"
#include "utils/vaulthelper.h"
#include "events/vaulteventreceiver.h"

#include "dfm-base/widgets/dfmwindow/filemanagerwindowsmanager.h"

#include <QUrl>

Q_DECLARE_METATYPE(QList<QUrl> *)
Q_DECLARE_METATYPE(QString *)
Q_DECLARE_METATYPE(Qt::DropAction *)

using namespace dfmplugin_vault;

DFMBASE_USE_NAMESPACE

void Vault::initialize()
{
    VaultVisibleManager::instance()->infoRegister();
    VaultEventReceiver::instance()->connectEvent();
    VaultVisibleManager::instance()->pluginServiceRegister();
    bindWindows();
}

bool Vault::start()
{
    dpfSignalDispatcher->subscribe("dfmplugin_computer", "signal_View_Refreshed",
                                   VaultVisibleManager::instance(), &VaultVisibleManager::onComputerRefresh);
    return true;
}

void Vault::bindWindows()
{
    const auto &winIdList { FMWindowsIns.windowIdList() };
    std::for_each(winIdList.begin(), winIdList.end(), [](quint64 id) {
        VaultVisibleManager::instance()->onWindowOpened(id);
    });
    connect(&FMWindowsIns, &FileManagerWindowsManager::windowOpened,
            VaultVisibleManager::instance(), &VaultVisibleManager::onWindowOpened, Qt::DirectConnection);
}
