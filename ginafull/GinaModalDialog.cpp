// GinaModalDialog.cpp
//
// Gather user credentials for Modal.
//

#include "stdafx.h"
#include "GinaModalDialog.h"
#include "GuiHelper.h"

INT_PTR CALLBACK GinaModalDialog::_dialogProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    if (WM_INITDIALOG == msg) {
        ((GinaModalDialog*)lp)->_hwnd = hwnd;
        GuiHelper::SetWindowLongPointer(hwnd, GWLP_USERDATA, lp);
        GuiHelper::CenterWindow(hwnd);
    }
	GinaModalDialog* dlg = (GinaModalDialog*)GuiHelper::GetWindowLongPointer(hwnd, GWLP_USERDATA);

    // WM_SETFONT is coming in before WM_INITDIALOG
    // in which case GWLP_USERDATA won't be set yet.
	if (dlg) {
		return dlg->DialogProc(msg, wp, lp);
	}
    return FALSE;
}

INT_PTR GinaModalDialog::DialogProc(UINT, WPARAM, LPARAM) {
    return FALSE;
}