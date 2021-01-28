#pragma once

#include "GinaWindow.h"

class NoticeWindow : public GinaWindow {
public:
    NoticeWindow(HDESK hDesktop, int nDialogResourceID);
    virtual ~NoticeWindow();
};
