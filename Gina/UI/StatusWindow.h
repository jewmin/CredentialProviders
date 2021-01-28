#pragma once

#include "GinaWindow.h"

class StatusWindow : public GinaWindow {
public:
    StatusWindow(HDESK hDesktop, PWSTR pTitle, PWSTR pMessage);
    virtual ~StatusWindow();
};
