// GinaModalDialog.h
//
// Base class for all modal dialogs.
//

#pragma once

#include "WinLogonInterface.h"
#include "resource.h"

class GinaModalDialog {
public:
    GinaModalDialog(IWinLogon* pWinLogon, int dialogResourceID, HINSTANCE hInst = GetMyInstance())
        : _pWinLogon(pWinLogon), _hInst(hInst), _dialogResourceID(dialogResourceID) {
    }

    int Show() {
        return _pWinLogon->wlxDialogBoxParam(_hInst, MAKEINTRESOURCE(_dialogResourceID), 0, _dialogProc, (LPARAM)this);
    }

    virtual INT_PTR DialogProc(UINT msg, WPARAM wp, LPARAM lp);

protected:
    static INT_PTR CALLBACK _dialogProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);

    IWinLogon* _pWinLogon;
    HINSTANCE  _hInst;
    int        _dialogResourceID;
    HWND       _hwnd;
};
