#pragma once

#include "GinaModalDialog.h"

class NoticeDialog : public GinaModalDialog {
public:
    NoticeDialog(IWinlogon * pWinlogon, HANDLE hUserToken, int nDialogResourceID);
    virtual ~NoticeDialog();
    virtual INT_PTR DialogProc(UINT uMsg, WPARAM wParam, LPARAM lParam);

private:
    HANDLE hUserToken_;
};
