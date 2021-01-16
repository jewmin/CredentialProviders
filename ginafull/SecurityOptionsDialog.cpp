// SecurityOptionsDialog.cpp
//
// Dialog that is displayed when a logged on user presses Control-Alt-Delete.
//

#include "stdafx.h"
#include "SecurityOptionsDialog.h"
#include "GuiHelper.h"
#include "SecurityHelper.h"

#include "resource.h"
#include "ntsecapi.h"

void SecurityOptionsDialog::_changePassword() {
    // this sample does not implement this functionality, but here are some tips:
    //   1) Use NetUserChangePassword to attempt the password change
    //   2) Use SecureZeroMemory to clear out any buffers that hold passwords
    _pWinLogon->wlxMessageBox(_hwnd, L"Your password change dialog goes here!", L"Change Password", MB_ICONINFORMATION);
}

bool SecurityOptionsDialog::_confirmLogoff() {
    return IDOK == _pWinLogon->wlxMessageBox(_hwnd, L"Are you sure you want to log off?", L"Log Off Windows", MB_ICONQUESTION | MB_OKCANCEL);
}

bool SecurityOptionsDialog::_confirmShutdown() {
    return IDOK == _pWinLogon->wlxMessageBox(_hwnd, L"Are you sure you want to shut down?", L"Shut Down Windows", MB_ICONQUESTION | MB_OKCANCEL);
}

INT_PTR SecurityOptionsDialog::DialogProc(UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {

        case WM_INITDIALOG: {
            wchar_t loginMsg[6 + MAX_SIZE_USER_NAME] = L"User: ";
            if (SecurityHelper::ImpersonateAndGetUserName(_hToken, loginMsg + 6, MAX_SIZE_USER_NAME)) {
                GuiHelper::SetControlText(_hwnd, IDC_NAME, loginMsg);
            }
            
            GuiHelper::CenterWindow(_hwnd);
            return TRUE;
        }
        case WM_COMMAND: {
            switch (LOWORD(wp)) {
                case IDC_LOCK:
                    EndDialog(_hwnd, Lock);
                    break;
                case IDC_LOGOFF:
                    if (_confirmLogoff()) EndDialog(_hwnd, Logoff);
                    break;
                case IDC_SHUTDOWN:
                    if (_confirmShutdown()) EndDialog(_hwnd, Shutdown);
                    break;
                case IDC_CHANGEPASS:
                    _changePassword();
                    EndDialog(_hwnd, IDCANCEL);
                    break;
                case IDC_TASKMAN:
                    EndDialog(_hwnd, TaskMan);
                    break;
                case IDCANCEL:
                    EndDialog(_hwnd, IDCANCEL);
                    break;
            }
            return TRUE;
        }
    }
    return FALSE;
}
