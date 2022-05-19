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
#ifndef MENU_DEFINES_H
#define MENU_DEFINES_H

#include "dfm_common_service_global.h"

DSC_BEGIN_NAMESPACE

namespace MenuParamKey {
// file menu params for initialize
inline constexpr char kCurrentDir[] = "currentDir";   // string url
inline constexpr char kSelectFiles[] = "selectFiles";   // string urls
inline constexpr char kOnDesktop[] = "onDesktop";   // bool
inline constexpr char kWindowId[] = "windowId";   // quint64
inline constexpr char kIsEmptyArea[] = "isEmptyArea";   // bool
inline constexpr char kIndexFlags[] = "indexFlags";   // Qt::ItemFlags
inline constexpr char kIsSystemPathIncluded[] = "isSystemPathIncluded";   // bool, true if 'SystemPathUtil::isSystemPath' return true
inline constexpr char kIsDDEDesktopFileIncluded[] = "isDDEDesktopFileIncluded";   // bool, contains 'dde-computer.desktop','dde-trash.desktop' and 'dde-home.desktop'
inline constexpr char kIsFocusOnDDEDesktopFile[] = "isFocusOnDDEDesktopFile";   // bool
}

namespace ActionPropertyKey {
// key for action property
inline constexpr char kActionID[] = "actionID";   // string

}

DSC_END_NAMESPACE

#endif   // MENU_DEFINES_H
