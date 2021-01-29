#pragma once

#include "../resource.h"
#include "../IWinlogon.h"
#include "GuiHelper.h"
#include "Utils.h"

extern HINSTANCE GetInstance();

class GinaModalDialog {
public:
    GinaModalDialog(IWinlogon * pWinlogon, int nDialogResourceID, HINSTANCE hInst = GetInstance());
    virtual ~GinaModalDialog();
    int Show();
    virtual INT_PTR DialogProc(UINT uMsg, WPARAM wParam, LPARAM lParam);

protected:
    static INT_PTR CALLBACK DlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

protected:
    static const int MaxSizeUserName = 21;

protected:
    IWinlogon *             pWinlogon_;
    int                     nDialogResourceID_;
    HINSTANCE               hInst_;
    HWND                    hWnd_;
};
