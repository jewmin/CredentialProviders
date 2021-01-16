// SecurityOptionsDialog.h
//
// Gather user credentials for Logon.
//

#pragma once

#include "WinLogonInterface.h"
#include "GinaModalDialog.h"
#include "resource.h"

class SecurityOptionsDialog : public GinaModalDialog {
public:
    SecurityOptionsDialog(IWinLogon* pWinLogon, HANDLE hToken)
        : GinaModalDialog(pWinLogon, IDD_OPTIONS), _hToken(hToken) {
    }

    enum DialogResults {
        Lock = 1001,
        Logoff,
        Shutdown,
        TaskMan
    };

    INT_PTR DialogProc(UINT msg, WPARAM wp, LPARAM lp);

private:

    void _changePassword();
    bool _confirmLogoff();
    bool _confirmShutdown();

    HANDLE _hToken;
};
