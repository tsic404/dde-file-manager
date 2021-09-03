/*
 * Copyright (C) 2020 ~ 2021 Uniontech Software Technology Co., Ltd.
 *
 * Author:     yanghao<yanghao@uniontech.com>
 *
 * Maintainer: zhengyouge<zhengyouge@uniontech.com>
 *             yanghao<yanghao@uniontech.com>
 *             hujianzhong<hujianzhong@uniontech.com>
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
#include "browseview.h"

DSB_FM_BEGIN_NAMESPACE

Q_GLOBAL_STATIC(DFMBrowseWidgetFactory,_browseViewFactory)

DFMBrowseView::DFMBrowseView(QWidget *parent)
    : FileView (parent)
{

}

DFMBrowseView::~DFMBrowseView()
{

}

void DFMBrowseView::setRootUrl(const QUrl &url)
{
    FileView::setRootUrl(url);
}

QUrl DFMBrowseView::rootUrl()
{
    return FileView::rootUrl();
}

DFMBrowseWidgetFactory &DFMBrowseWidgetFactory::instance() {
    return * _browseViewFactory;
}

DSB_FM_END_NAMESPACE
