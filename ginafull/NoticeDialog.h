// NoticeDialog.h
//
// Dialog displayed for either SAS notice or wksta locked notice
//

#pragma once

#include "WinLogonInterface.h"
#include "GinaModalDialog.h"
#include "resource.h"

class NoticeDialog : public GinaModalDialog {
public:
    NoticeDialog(IWinLogon* pWinLogon, int dialogResourceID)
        : GinaModalDialog(pWinLogon, dialogResourceID) {
    }
};
