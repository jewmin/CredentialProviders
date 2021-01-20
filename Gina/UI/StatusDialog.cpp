#include "StatusDialog.h"

StatusDialog::StatusDialog(HDESK hDesktop, PWSTR pTitle, PWSTR pMessage)
    : hDesktop_(hDesktop) {
    EnterDesktop enter(hDesktop_);
    hWnd_ = ::CreateDialog(GetInstance(), MAKEINTRESOURCE(IDD_STATUS), NULL, StatusDialog::DlgProc);
    if (!hWnd_) {
        Utils::Output(Utils::StringFormat(L"StatusDialog::StatusDialog Error: %s", Utils::GetLastErrorString().c_str()));
        return;
    }

    GuiHelper::CenterWindow(hWnd_);
    ::ShowWindow(hWnd_, SW_SHOW);
    if (GuiHelper::SetControlText(hWnd_, IDC_MESSAGE, pMessage)) {
        if (pTitle) {
            ::SetWindowText(hWnd_, pTitle);
        }
    }
}

StatusDialog::~StatusDialog() {
    EnterDesktop enter(hDesktop_);
    if (hWnd_) {
        ::DestroyWindow(hWnd_);
    }
}

INT_PTR CALLBACK StatusDialog::DlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    return FALSE;
}

EnterDesktop::EnterDesktop(HDESK hDesktop) {
    hOldDesktop_ = ::GetThreadDesktop(::GetCurrentThreadId());
    if (hDesktop == hOldDesktop_) {
        hOldDesktop_ = NULL;
    } else if (!SetThreadDesktop(hDesktop)) {
        hOldDesktop_ = NULL;
        Utils::Output(Utils::StringFormat(L"EnterDesktop::EnterDesktop Error: %s", Utils::GetLastErrorString().c_str()));
    }
}

EnterDesktop::~EnterDesktop() {
    if (hOldDesktop_) {
        if (!SetThreadDesktop(hOldDesktop_)) {
            Utils::Output(Utils::StringFormat(L"EnterDesktop::~EnterDesktop Error: %s", Utils::GetLastErrorString().c_str()));
        }
    }
}
