#include "LogonDialog.h"

LogonDialog::LogonDialog(IWinlogon * pWinlogon)
    : GinaModalDialog(pWinlogon, IDD_LOGON)
    , domain_(NULL)
    , username_(NULL)
    , password_(NULL) {
}

LogonDialog::~LogonDialog() {
    if (domain_) {
        delete domain_;
    }
    if (username_) {
        delete username_;
    }
    if (password_) {
        delete password_;
    }
}

INT_PTR LogonDialog::DialogProc(UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDOK:
            GuiHelper::ExtractControlText(hWnd_, IDC_DOMAIN, &domain_);
            GuiHelper::ExtractControlText(hWnd_, IDC_NAME, &username_);
            GuiHelper::ExtractControlText(hWnd_, IDC_PASSWORD, &password_);
            ::EndDialog(hWnd_, IDOK);
            break;

        case IDCANCEL:
            ::EndDialog(hWnd_, IDCANCEL);
            break;

        case IDC_SHUTDOWN:
            if (ConfirmShutdown()) {
                ::EndDialog(hWnd_, IDC_SHUTDOWN);
            }
        }
        return TRUE;
    }
    return FALSE;
}

bool LogonDialog::ConfirmShutdown() {
    return IDOK == pWinlogon_->WlxMessageBox(hWnd_, L"Are you sure you want to shut down?", L"Shut Down Windows", MB_ICONQUESTION | MB_OKCANCEL);
}
