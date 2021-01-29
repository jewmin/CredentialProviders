#include "NoticeDialog.h"
#include "../SecurityHelper.h"

NoticeDialog::NoticeDialog(IWinlogon * pWinlogon, HANDLE hUserToken, int nDialogResourceID)
    : GinaModalDialog(pWinlogon, nDialogResourceID)
    , hUserToken_(hUserToken) {
}

NoticeDialog::~NoticeDialog() {
}

INT_PTR NoticeDialog::DialogProc(UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_INITDIALOG: {
            if (nDialogResourceID_ == IDD_LOCKEDNOTICE) {
                WCHAR szUserName[GinaModalDialog::MaxSizeUserName] = L"";
                if (SecurityHelper::ImpersonateAndGetUserName(hUserToken_, szUserName, GinaModalDialog::MaxSizeUserName)) {
                    GuiHelper::SetControlText(hWnd_, IDC_NAME, Utils::StringFormat(L"Only %s or an administrator", szUserName).c_str());
                }
            }
        }
        return TRUE;
    }
    return FALSE;
}
