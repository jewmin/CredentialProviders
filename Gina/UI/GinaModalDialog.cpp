#include "GinaModalDialog.h"

GinaModalDialog::GinaModalDialog(IWinlogon * pWinlogon, int nDialogResourceID, HINSTANCE hInst)
    : pWinlogon_(pWinlogon)
    , nDialogResourceID_(nDialogResourceID)
    , hInst_(hInst)
    , hWnd_(NULL) {
}

GinaModalDialog::~GinaModalDialog() {
}

int GinaModalDialog::Show() {
    return pWinlogon_->WlxDialogBoxParam(hInst_, MAKEINTRESOURCE(nDialogResourceID_), 0, GinaModalDialog::DlgProc, reinterpret_cast<LPARAM>(this));
}

INT_PTR GinaModalDialog::DialogProc(UINT uMsg, WPARAM wParam, LPARAM lParam) {
    return FALSE;
}

INT_PTR CALLBACK GinaModalDialog::DlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    if (WM_INITDIALOG == uMsg) {
        reinterpret_cast<GinaModalDialog *>(lParam)->hWnd_ = hWnd;
        GuiHelper::SetWindowLongPointer(hWnd, GWLP_USERDATA, lParam);
        GuiHelper::CenterWindow(hWnd);
    }

    GinaModalDialog * dlg = reinterpret_cast<GinaModalDialog *>(GuiHelper::GetWindowLongPointer(hWnd, GWLP_USERDATA));
    if (dlg) {
        return dlg->DialogProc(uMsg, wParam, lParam);
    }
    return FALSE;
}
