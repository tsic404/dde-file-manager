// SPDX-FileCopyrightText: 2022 - 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "canvasmanager.h"
#include "private/canvasmanager_p.h"
#include "displayconfig.h"
#include "grid/canvasgrid.h"
#include "view/canvasview_p.h"

#include "stubext.h"

#include <gtest/gtest.h>

DDP_CANVAS_USE_NAMESPACE

TEST(CanvasManager, autoArrange)
{
    CanvasManager obj;
    stub_ext::StubExt stub;
    bool ret = false;
    stub.set_lamda(&DisplayConfig::autoAlign, [&ret](){
        return ret;
    });

    EXPECT_EQ(ret, obj.autoArrange());

    ret = true;
    EXPECT_EQ(ret, obj.autoArrange());
}

TEST(CanvasManager, refresh)
{
    CanvasManager obj;
    CanvasViewPointer vp(new CanvasView());
    obj.d->viewMap.insert("test", vp);

    stub_ext::StubExt stub;
    bool silent = false;
    stub.set_lamda(&CanvasView::refresh,[&silent](CanvasView *self, bool s){
        silent = s;
    });

    obj.refresh(true);
    EXPECT_TRUE(silent);

    silent = true;
    obj.refresh(false);
    EXPECT_FALSE(false);
}

TEST(CanvasManager, setAutoArrange)
{
    CanvasManager obj;
    obj.d->hookIfs = new CanvasManagerHook(&obj);

    stub_ext::StubExt stub;
    bool isOn = false;
    stub.set_lamda(&DisplayConfig::setAutoAlign, [&isOn](DisplayConfig *, bool on){
        isOn = on;
    });

    CanvasGrid::Mode mode = CanvasGrid::Mode::Custom;
    stub.set_lamda(&CanvasGrid::setMode, [&mode](CanvasGrid *, CanvasGrid::Mode m){
        mode = m;
    });

    bool call = false;
    stub.set_lamda(&CanvasGrid::arrange, [&call]() {
        call = true;
    });

    bool notify = false;
    stub.set_lamda(VADDR(CanvasManagerHook, autoArrangeChanged), [&notify]() {
        notify = true;
    });

    obj.setAutoArrange(true);
    EXPECT_TRUE(isOn);
    EXPECT_EQ(mode, CanvasGrid::Mode::Align);
    EXPECT_TRUE(call);
    EXPECT_TRUE(notify);

    isOn = true;
    mode = CanvasGrid::Mode::Align;
    call = false;
    notify = false;
    obj.setAutoArrange(false);
    EXPECT_FALSE(isOn);
    EXPECT_EQ(mode, CanvasGrid::Mode::Custom);
    EXPECT_FALSE(call);
    EXPECT_TRUE(notify);
}
