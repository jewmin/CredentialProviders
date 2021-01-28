#pragma once

#include "../resource.h"
#include "GuiHelper.h"
#include "Utils.h"

extern HINSTANCE GetInstance();

class GinaWindow {
public:
    GinaWindow(HDESK hDesktop, int nDialogResourceID, HINSTANCE hInst = GetInstance());
    virtual ~GinaWindow();

protected:
    static INT_PTR CALLBACK DlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

protected:
    HDESK hDesktop_;
    HWND hWnd_;
};

class EnterDesktop {
public:
    EnterDesktop(HDESK hDesktop);
    ~EnterDesktop();

private:
    HDESK hOldDesktop_;
};
