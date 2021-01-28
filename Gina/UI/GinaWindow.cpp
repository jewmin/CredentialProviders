#include "GinaWindow.h"

GinaWindow::GinaWindow(HDESK hDesktop, int nDialogResourceID, HINSTANCE hInst)
    : hDesktop_(hDesktop) {
    EnterDesktop enter(hDesktop_);
    hWnd_ = ::CreateDialog(hInst, MAKEINTRESOURCE(nDialogResourceID), NULL, GinaWindow::DlgProc);
    if (!hWnd_) {
        Utils::Output(Utils::StringFormat(L"GinaWindow::GinaWindow Error: %s", Utils::GetLastErrorString().c_str()));
        return;
    }

    GuiHelper::CenterWindow(hWnd_);
    ::ShowWindow(hWnd_, SW_SHOW);
}

GinaWindow::~GinaWindow() {
    EnterDesktop enter(hDesktop_);
    if (hWnd_) {
        ::DestroyWindow(hWnd_);
    }
}

INT_PTR CALLBACK GinaWindow::DlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    return FALSE;
}

EnterDesktop::EnterDesktop(HDESK hDesktop) {
    hOldDesktop_ = ::GetThreadDesktop(::GetCurrentThreadId());
    if (hDesktop == hOldDesktop_) {
        hOldDesktop_ = NULL;
    } else if (!::SetThreadDesktop(hDesktop)) {
        hOldDesktop_ = NULL;
        Utils::Output(Utils::StringFormat(L"EnterDesktop::EnterDesktop Error: %s", Utils::GetLastErrorString().c_str()));
    }
}

EnterDesktop::~EnterDesktop() {
    if (hOldDesktop_) {
        if (!::SetThreadDesktop(hOldDesktop_)) {
            Utils::Output(Utils::StringFormat(L"EnterDesktop::~EnterDesktop Error: %s", Utils::GetLastErrorString().c_str()));
        }
    }
}
