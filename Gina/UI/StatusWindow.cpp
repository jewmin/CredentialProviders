#include "StatusWindow.h"

StatusWindow::StatusWindow(HDESK hDesktop, PWSTR pTitle, PWSTR pMessage)
    : GinaWindow(hDesktop, IDD_STATUS) {
    if (hWnd_) {
        if (GuiHelper::SetControlText(hWnd_, IDC_MESSAGE, pMessage)) {
            if (pTitle) {
                ::SetWindowText(hWnd_, pTitle);
            }
        }
    }
}

StatusWindow::~StatusWindow() {
}
