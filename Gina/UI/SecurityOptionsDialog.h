#pragma once

#include "GinaModalDialog.h"

class SecurityOptionsDialog : public GinaModalDialog {
public:
    SecurityOptionsDialog(IWinlogon * pWinlogon, HANDLE hUserToken);
    virtual ~SecurityOptionsDialog();
    virtual INT_PTR DialogProc(UINT uMsg, WPARAM wParam, LPARAM lParam);

private:
    void ChangePassword();
    bool ConfirmLogoff();
    bool ConfirmShutdown();

public:
    enum DialogResult {
        Lock = 1001,
        Logoff,
        Shutdown,
        Taskman
    };

private:
    static const int MaxSizeUserName = 21;

private:
    HANDLE hUserToken_;
};
