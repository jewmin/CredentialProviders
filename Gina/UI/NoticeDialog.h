#pragma once

#include "GinaModalDialog.h"

class NoticeDialog : public GinaModalDialog {
public:
    NoticeDialog(IWinlogon * pWinlogon, HANDLE hUserToken, int nDialogResourceID);
    virtual ~NoticeDialog();

private:
    HANDLE hUserToken_;
};
