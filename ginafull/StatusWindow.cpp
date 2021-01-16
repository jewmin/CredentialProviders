// StatusWindow.cpp
//
// Simple status window for displaying WinLogon's status messages
//

#include "stdafx.h"
#include "StatusWindow.h"
#include "GuiHelper.h"
#include "resource.h"

// little helper class that associates the current thread with an alternate desktop
// while it's in scope, then resets the desktop when it goes out of scope
struct EnterDesktop {
    HDESK old;
    EnterDesktop(HDESK hdesk) : old(0) {
        old = GetThreadDesktop(GetCurrentThreadId());
        if (hdesk == old) {
            // obvious case where thread is already on the requisite desktop - nothing to do
            old = 0;
        }
        else if (!SetThreadDesktop(hdesk)) {
            // something bad happened, don't try to restore the desktop
            old = 0;
            LCF1(L"SetThreadDesktop failed: %d", GetLastError());
        }
    }
    ~EnterDesktop() {
        if (old) {
            if (!SetThreadDesktop(old)) LCF1(L"SetThreadDesktop failed: %d", GetLastError());
        }
    }
};

StatusWindow::StatusWindow(HDESK hdesk, const wchar_t* title, const wchar_t* message)
  : _hdesk(hdesk), _hwnd(0) {

    EnterDesktop enterDesktop(_hdesk);

    _hwnd = CreateDialog(GetMyInstance(), MAKEINTRESOURCE(IDD_STATUS), 0, _dialogProc);
    GuiHelper::CenterWindow(_hwnd);
    ShowWindow(_hwnd, SW_SHOW);

    if (_hwnd) {
        if (GuiHelper::SetControlText(_hwnd, IDC_MESSAGE, message)) {
            if (title) {
                SetWindowText(_hwnd, title);
            }
        }
    }
    else LCF1(L"CreateDialogParam failed: %d", GetLastError());
}

StatusWindow::~StatusWindow() {
    EnterDesktop enterDesktop(_hdesk);
    DestroyWindow(_hwnd);
}

INT_PTR CALLBACK StatusWindow::_dialogProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    return FALSE;
}
