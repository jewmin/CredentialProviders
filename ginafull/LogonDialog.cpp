// LogonDialog.cpp
//
// Gather user credentials for Logon.
//

#include "stdafx.h"
#include "LogonDialog.h"
#include "GuiHelper.h"

#include "resource.h"

INT_PTR LogonDialog::DialogProc(UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
        case WM_COMMAND: {
            switch (LOWORD(wp)) {
                case IDOK:
                    GuiHelper::ExtractControlText(_hwnd, IDC_NAME,     &userName);
                    GuiHelper::ExtractControlText(_hwnd, IDC_PASSWORD, &password);
                    GuiHelper::ExtractControlText(_hwnd, IDC_DOMAIN,   &domain);
                    EndDialog(_hwnd, IDOK);
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
