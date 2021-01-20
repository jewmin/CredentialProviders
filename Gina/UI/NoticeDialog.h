#pragma once

#include "GinaModalDialog.h"

class NoticeDialog : public GinaModalDialog {
public:
    NoticeDialog(IWinlogon * pWinlogon, int nDialogResourceID);
    virtual ~NoticeDialog();
};
