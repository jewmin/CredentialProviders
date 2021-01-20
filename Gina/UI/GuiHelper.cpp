#include "GuiHelper.h"
#include "Utils.h"

void GuiHelper::SetWindowLongPointer(HWND hWnd, int nIndex, LONG_PTR dwNewLong) {
    ::SetWindowLongPtr(hWnd, nIndex, dwNewLong);
}

LONG_PTR GuiHelper::GetWindowLongPointer(HWND hWnd, int nIndex) {
    return ::GetWindowLongPtr(hWnd, nIndex);
}

void GuiHelper::CenterWindow(HWND hWnd) {
    RECT rc;
    if (::GetWindowRect(hWnd, &rc)) {
        const LONG width = rc.right - rc.left;
        const LONG height = rc.bottom - rc.top;
        ::MoveWindow(hWnd, (::GetSystemMetrics(SM_CXSCREEN) - width) / 2, (::GetSystemMetrics(SM_CYSCREEN) - height) / 2, width, height, TRUE);
    }
}

bool GuiHelper::SetControlText(HWND hDlg, int nIDDlgItem, LPCWSTR lpString) {
    HWND hWnd = ::GetDlgItem(hDlg, nIDDlgItem);
    if (hWnd) {
        return ::SetWindowText(hWnd, lpString ? lpString : L"") ? true : false;
    }
    return false;
}

bool GuiHelper::ExtractControlText(HWND hDlg, int nIDDlgItem, LPWSTR * ppString) {
    HWND hWnd = ::GetDlgItem(hDlg, nIDDlgItem);
    if (hWnd) {
        const int cch = ::GetWindowTextLength(hWnd);
        const int cchMax = cch + 1;
        *ppString = new WCHAR[cchMax];
        if (!*ppString) {
            Utils::Output(L"GuiHelper::ExtractControlText *ppString: Out Of Memory");
            return false;
        }
        return cch == ::GetWindowText(hWnd, *ppString, cchMax);
    }
    return false;
}
