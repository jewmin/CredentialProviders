#include "NoticeDialog.h"

NoticeDialog::NoticeDialog(IWinlogon * pWinlogon, int nDialogResourceID)
    : GinaModalDialog(pWinlogon, nDialogResourceID) {
}

NoticeDialog::~NoticeDialog() {
}
