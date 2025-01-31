// SPDX-FileCopyrightText: 2022 - 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef EXTENDMENUSCENE_P_H
#define EXTENDMENUSCENE_P_H

#include "extendmenuscene/extendmenuscene.h"
#include "extendmenuscene/extendmenu/dcustomactiondefine.h"
#include "extendmenuscene/extendmenu/dcustomactionparser.h"

#include "interfaces/private/abstractmenuscene_p.h"

namespace dfmplugin_menu {

namespace MenuParamKeyExt {
inline constexpr char kIsHiddenExtActions[] = "isHiddenExtActions";   // quint64

}

class ExtendMenuScenePrivate : public dfmbase::AbstractMenuScenePrivate
{
public:
    explicit ExtendMenuScenePrivate(ExtendMenuScene *qq);
    void menuVisiableControl();
    QList<QAction *> childActions(QAction *action);

public:
    DCustomActionParser *customParser = nullptr;
    QList<QAction *> extendActions;
    QList<QAction *> extendChildActions;

    QMap<int, QList<QAction *>> cacheLocateActions;
    QMap<QAction *, DCustomActionDefines::Separator> cacheActionsSeparator;
};

}

#endif   // EXTENDMENUSCENE_P_H
