#pragma once

#include "../resource.h"
#include "GuiHelper.h"
#include "Utils.h"

extern HINSTANCE GetInstance();

class StatusDialog {
public:
    StatusDialog(HDESK hDesktop, PWSTR pTitle, PWSTR pMessage);
    ~StatusDialog();

protected:
    static INT_PTR CALLBACK DlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

private:
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
