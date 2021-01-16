// LogonDialog.h
//
// Gather user credentials for Logon.
//

#pragma once

#include "WinLogonInterface.h"
#include "GinaModalDialog.h"
#include "resource.h"

class LogonDialog : public GinaModalDialog {
public:
    LogonDialog(IWinLogon* pWinLogon)
        : GinaModalDialog(pWinLogon, IDD_LOGON), domain(0), userName(0), password(0) {
    }
    ~LogonDialog() {
        if (domain)   delete domain;
        if (userName) delete userName;
        if (password) delete password;
    }
    
    INT_PTR DialogProc(UINT msg, WPARAM wp, LPARAM lp);

    wchar_t* domain;
    wchar_t* userName;
    wchar_t* password;
};
