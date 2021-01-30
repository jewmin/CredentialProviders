#pragma once

#include "GinaModalDialog.h"

class LogonDialog : public GinaModalDialog {
public:
    LogonDialog(IWinlogon * pWinlogon);
    virtual ~LogonDialog();
    virtual INT_PTR DialogProc(UINT uMsg, WPARAM wParam, LPARAM lParam);

private:
    bool ConfirmShutdown();

public:
    PWSTR domain_;
    PWSTR username_;
    PWSTR password_;
};
