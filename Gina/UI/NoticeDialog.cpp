#include "NoticeDialog.h"
#include "../SecurityHelper.h"

NoticeDialog::NoticeDialog(IWinlogon * pWinlogon, HANDLE hUserToken, int nDialogResourceID)
    : GinaModalDialog(pWinlogon, nDialogResourceID)
    , hUserToken_(hUserToken) {
    if (nDialogResourceID == IDD_LOCKEDNOTICE) {
        WCHAR szUserName[GinaModalDialog::MaxSizeUserName] = L"";
        if (SecurityHelper::ImpersonateAndGetUserName(hUserToken_, szUserName, GinaModalDialog::MaxSizeUserName)) {
            GuiHelper::SetControlText(hWnd_, IDC_NAME, Utils::StringFormat(L"Only %s or an administrator", szUserName).c_str());
        }
    }
}

NoticeDialog::~NoticeDialog() {
}
