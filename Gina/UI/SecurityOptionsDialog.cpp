#include "SecurityOptionsDialog.h"
#include "SecurityHelper.h"

SecurityOptionsDialog::SecurityOptionsDialog(IWinlogon * pWinlogon, HANDLE hUserToken)
    : GinaModalDialog(pWinlogon, IDD_OPTIONS)
    , hUserToken_(hUserToken) {
}

SecurityOptionsDialog::~SecurityOptionsDialog() {
}

INT_PTR SecurityOptionsDialog::DialogProc(UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_INITDIALOG: {
            WCHAR szLogonMsg[6 + SecurityOptionsDialog::MaxSizeUserName] = L"User: ";
            if (SecurityHelper::ImpersonateAndGetUserName(hUserToken_, szLogonMsg + 6, SecurityOptionsDialog::MaxSizeUserName)) {
                GuiHelper::SetControlText(hWnd_, IDC_NAME, szLogonMsg);
            }
        }
        return TRUE;

    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDC_LOCK:
            ::EndDialog(hWnd_, SecurityOptionsDialog::Lock);
            break;

        case IDC_LOGOFF:
            if (ConfirmLogoff()) {
                ::EndDialog(hWnd_, SecurityOptionsDialog::Logoff);
            }
            break;

        case IDC_SHUTDOWN:
            if (ConfirmShutdown()) {
                ::EndDialog(hWnd_, SecurityOptionsDialog::Shutdown);
            }
            break;

        case IDC_CHANGEPASS:
            ChangePassword();
            ::EndDialog(hWnd_, IDCANCEL);
            break;

        case IDC_TASKMAN:
            ::EndDialog(hWnd_, SecurityOptionsDialog::Taskman);
            break;

        case IDCANCEL:
            ::EndDialog(hWnd_, IDCANCEL);
            break;
        }
        return TRUE;
    }
    return FALSE;
}

void SecurityOptionsDialog::ChangePassword() {
    pWinlogon_->WlxMessageBox(hWnd_, L"Your password change dialog goes here!", L"Change Password", MB_ICONINFORMATION);
}

bool SecurityOptionsDialog::ConfirmLogoff() {
    return IDOK == pWinlogon_->WlxMessageBox(hWnd_, L"Are you sure you want to log off?", L"Log Off Windows", MB_ICONQUESTION | MB_OKCANCEL);
}

bool SecurityOptionsDialog::ConfirmShutdown() {
    return IDOK == pWinlogon_->WlxMessageBox(hWnd_, L"Are you sure you want to shut down?", L"Shut Down Windows", MB_ICONQUESTION | MB_OKCANCEL);
}
