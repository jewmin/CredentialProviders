// GuiHelper.cpp
//
// Common utilities for window management.
//

#include "stdafx.h"
#include "GUIHelper.h"

#pragma warning(push)
#pragma warning(disable : 4244)

void GuiHelper::SetWindowLongPointer(HWND hwnd, int index, LONG_PTR dwNewLong) {
    SetWindowLongPtr(hwnd, index, dwNewLong);
}

LONG_PTR GuiHelper::GetWindowLongPointer(HWND hwnd, int index) {
    return GetWindowLongPtr(hwnd, index);
}

#pragma warning(pop)

void GuiHelper::CenterWindow(HWND hwnd) {
    RECT rc;
    if (!GetWindowRect(hwnd, &rc)) return;

    const int width  = rc.right  - rc.left;
    const int height = rc.bottom - rc.top;

    MoveWindow(hwnd,
        (GetSystemMetrics(SM_CXSCREEN) - width)  / 2,
        (GetSystemMetrics(SM_CYSCREEN) - height) / 2,
        width, height, true);
}

bool GuiHelper::SetControlText(HWND hDlg, int id, const wchar_t* pText) {
    HWND hwnd = GetDlgItem(hDlg, id);
    if (!hwnd) return false;

    return SetWindowText(hwnd, pText ? pText : L"") ? true : false;
}

bool GuiHelper::ExtractControlText(HWND hDlg, int id, wchar_t** ppText) {
    HWND hwnd = GetDlgItem(hDlg, id);
    if (!hwnd) return false;

    const int cch = GetWindowTextLength(hwnd);
    const int cchMax = cch + 1;
    *ppText = new wchar_t[cchMax];
    if (!*ppText) {
        LOOM;
        return false;
    }

    return cch == GetWindowText(hwnd, *ppText, cchMax);
}
