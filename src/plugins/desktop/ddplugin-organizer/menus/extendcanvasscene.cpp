/*
 * Copyright (C) 2022 Uniontech Software Technology Co., Ltd.
 *
 * Author:     zhangyu<zhangyub@uniontech.com>
 *
 * Maintainer: zhangyu<zhangyub@uniontech.com>
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
#include "extendcanvasscene_p.h"
#include "organizermenu_defines.h"
#include "config/configpresenter.h"

#include "plugins/common/dfmplugin-menu/menuscene/menuutils.h"

DDP_ORGANIZER_USE_NAMESPACE

DFMBASE_USE_NAMESPACE

AbstractMenuScene *ExtendCanvasCreator::create()
{
    return new ExtendCanvasScene();
}

ExtendCanvasScenePrivate::ExtendCanvasScenePrivate(ExtendCanvasScene *qq)
    : AbstractMenuScenePrivate(qq)
    , q(qq)
{
    // 获取菜单服务
    menuServer = MenuService::service();
}

void ExtendCanvasScenePrivate::emptyMenu(QMenu *parent)
{
    QAction *tempAction = parent->addAction(predicateName.value(ActionID::kOrganizeDesktop));
    predicateAction[ActionID::kOrganizeDesktop] = tempAction;
    tempAction->setProperty(ActionPropertyKey::kActionID, QString(ActionID::kOrganizeDesktop));
    tempAction->setCheckable(true);
    tempAction->setChecked(turnOn);

    if (turnOn) {
        tempAction = parent->addAction(predicateName.value(ActionID::kOrganizeBy));
        tempAction->setMenu(organizeBySubActions(parent));
        predicateAction[ActionID::kOrganizeBy] = tempAction;
        tempAction->setProperty(ActionPropertyKey::kActionID, QString(ActionID::kOrganizeBy));
    }

//    tempAction = parent->addAction(predicateName.value(ActionID::kOrganizeOptions));
//    predicateAction[ActionID::kOrganizeOptions] = tempAction;
//    tempAction->setProperty(ActionPropertyKey::kActionID, QString(ActionID::kOrganizeOptions));
}

void ExtendCanvasScenePrivate::normalMenu(QMenu *parent)
{

}

QMenu *ExtendCanvasScenePrivate::organizeBySubActions(QMenu *menu)
{
    QMenu *subMenu = new QMenu(menu);

    QAction *tempAction = subMenu->addAction(predicateName.value(ActionID::kOrganizeByCustom));
    predicateAction[ActionID::kOrganizeByCustom] = tempAction;
    tempAction->setProperty(ActionPropertyKey::kActionID, QString(ActionID::kOrganizeByCustom));
    tempAction->setCheckable(true);

    tempAction = subMenu->addAction(predicateName.value(ActionID::kOrganizeByType));
    predicateAction[ActionID::kOrganizeByType] = tempAction;
    tempAction->setProperty(ActionPropertyKey::kActionID, QString(ActionID::kOrganizeByType));
    tempAction->setCheckable(true);

//    tempAction = subMenu->addAction(predicateName.value(ActionID::kOrganizeByTimeAccessed));
//    predicateAction[ActionID::kOrganizeByTimeAccessed] = tempAction;
//    tempAction->setProperty(ActionPropertyKey::kActionID, QString(ActionID::kOrganizeByTimeAccessed));
//    tempAction->setCheckable(true);

//    tempAction = subMenu->addAction(predicateName.value(ActionID::kOrganizeByTimeModified));
//    predicateAction[ActionID::kOrganizeByTimeModified] = tempAction;
//    tempAction->setProperty(ActionPropertyKey::kActionID, QString(ActionID::kOrganizeByTimeModified));
//    tempAction->setCheckable(true);

//    tempAction = subMenu->addAction(predicateName.value(ActionID::kOrganizeByTimeCreated));
//    predicateAction[ActionID::kOrganizeByTimeCreated] = tempAction;
//    tempAction->setProperty(ActionPropertyKey::kActionID, QString(ActionID::kOrganizeByTimeCreated));
//    tempAction->setCheckable(true);

    return subMenu;
}

QString ExtendCanvasScenePrivate::classifierToActionID(Classifier cf)
{
    QString ret;
    switch (cf) {
    case kType:
        ret = ActionID::kOrganizeByType;
        break;
    case kTimeCreated:
        ret = ActionID::kOrganizeByTimeCreated;
        break;
    case kTimeModified:
        ret = ActionID::kOrganizeByTimeModified;
        break;
    case kLabel:

        break;
    case kName:

        break;
    case kSize:

        break;
    default:
        break;
    }
    return ret;
}

ExtendCanvasScene::ExtendCanvasScene(QObject *parent)
    : AbstractMenuScene(parent)
    , d(new ExtendCanvasScenePrivate(this))
{
    d->predicateName[ActionID::kOrganizeDesktop] = tr("Organize desktop");
    d->predicateName[ActionID::kOrganizeOptions] = tr("Organize options");
    d->predicateName[ActionID::kOrganizeBy] = tr("Organize by");

    // organize by subactions
    d->predicateName[ActionID::kOrganizeByCustom] = tr("Custom collection");
    d->predicateName[ActionID::kOrganizeByType] = tr("Type");
    d->predicateName[ActionID::kOrganizeByTimeAccessed] = tr("Time accessed");
    d->predicateName[ActionID::kOrganizeByTimeModified] = tr("Time modified");
    d->predicateName[ActionID::kOrganizeByTimeCreated] = tr("Time created");
}

QString ExtendCanvasScene::name() const
{
    return ExtendCanvasCreator::name();
}

bool ExtendCanvasScene::initialize(const QVariantHash &params)
{
    d->turnOn = CfgPresenter->isEnable();
    d->isEmptyArea = params.value(MenuParamKey::kIsEmptyArea).toBool();
    d->onDesktop = params.value(MenuParamKey::kOnDesktop).toBool();
    return d->onDesktop;
}

AbstractMenuScene *ExtendCanvasScene::scene(QAction *action) const
{
    if (!action)
        return nullptr;

    if (d->predicateAction.values().contains(action))
        return const_cast<ExtendCanvasScene *>(this);

    return AbstractMenuScene::scene(action);
}

bool ExtendCanvasScene::create(QMenu *parent)
{
    if (!parent)
        return false;

    if (d->isEmptyArea) {
        d->emptyMenu(parent);
    } else {
        d->normalMenu(parent);
    }

    // 创建子场景菜单
    return AbstractMenuScene::create(parent);
}

void ExtendCanvasScene::updateState(QMenu *parent)
{
    auto actions = parent->actions();
    auto actionIter = std::find_if(actions.begin(), actions.end(), [](const QAction *ac){
        return ac->property(ActionPropertyKey::kActionID).toString() == QString("display-settings");
    });

    if (actionIter == actions.end()) {
        qWarning() << "can not find action:" << "display-settings";
        return ;
    }

    QAction *indexAction = *actionIter;
    parent->insertAction(indexAction, d->predicateAction[ActionID::kOrganizeDesktop]);
    if (d->turnOn) {
        parent->insertAction(indexAction, d->predicateAction[ActionID::kOrganizeBy]);
        parent->insertAction(indexAction, d->predicateAction[ActionID::kOrganizeOptions]);

        if (CfgPresenter->mode() == OrganizerMode::kCustom) {
            d->predicateAction[ActionID::kOrganizeByCustom]->setChecked(true);
        } else if (CfgPresenter->mode() == OrganizerMode::kNormalized) {
            QString id = d->classifierToActionID(CfgPresenter->classification());
            if (auto ac = d->predicateAction.value(id))
                ac->setChecked(true);
        }

        auto iconSizeIter = std::find_if(actions.begin(), actions.end(), [](const QAction *ac){
            return ac->property(ActionPropertyKey::kActionID).toString() == QString("icon-size");
        });

        if (iconSizeIter != actions.end()) {
            (*iconSizeIter)->setVisible(false);
            (*iconSizeIter)->setEnabled(false);
        }
    } else {
        parent->insertAction(indexAction, d->predicateAction[ActionID::kOrganizeOptions]);
    }

    AbstractMenuScene::updateState(parent);
}

bool ExtendCanvasScene::triggered(QAction *action)
{
    auto actionId = action->property(ActionPropertyKey::kActionID).toString();
    if (d->predicateAction.values().contains(action)) {
        qDebug() << "organizer for canvas:" << actionId;
        if (actionId == ActionID::kOrganizeDesktop) {
            emit CfgPresenter->changeEnableState(action->isChecked());
        } else if (actionId == ActionID::kOrganizeByCustom) {
            emit CfgPresenter->switchToCustom();
        } else if (actionId == ActionID::kOrganizeByType) {
            emit CfgPresenter->switchToNormalized(Classifier::kType);
        } else if (actionId == ActionID::kOrganizeByTimeAccessed) {

        } else if (actionId == ActionID::kOrganizeByTimeModified) {
            emit CfgPresenter->switchToNormalized(Classifier::kTimeModified);
        } else if (actionId == ActionID::kOrganizeByTimeCreated) {
            emit CfgPresenter->switchToNormalized(Classifier::kTimeCreated);
        }
        return true;
    }
    return AbstractMenuScene::triggered(action);
}


